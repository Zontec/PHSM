#include <stdio.h>
#include <string.h>
#include "communication_driver.h"
#include "log.h"
#include "sha256.h"
#include "hmac.h"
#include "aes.h"
#include "pbkdf2.h"
#include "protocol.h"
#include "SECTIONS.h"
#include "entropy.h"
#include "hw_configs.h"

void fw_start() FW_MEMORY_SECTION;

typedef void (*handler_function)(protocol_message_t *);

typedef struct FW_MEMORY_SECTION key_store {
    uint8_t key[GEN_KEY_SIZE];
    uint8_t salt[SALT_KEY_SIZE];
    uint8_t is_active;
}key_store_t;

typedef struct FW_MEMORY_SECTION user_cred {
    uint8_t key[KEY_IN_SIZE];
    uint8_t token[TOKEN_SIZE];
    uint8_t is_token_active;
    uint8_t is_user_active;
}user_cred_t;


key_store_t key_storage[MAX_KEY_STORAGE] = {0};
user_cred_t users_storage[MAX_KEY_STORAGE] = {0};
handler_function handler_function_array[255] = {NULL};

uint8_t count_crc(uint8_t *data, uint32_t size)
{
    uint8_t crc = 0;
    for (uint32_t i = 0; i < size; ++i)
    {
        crc ^= data[i];
    }
    return crc;
}

void prng_gen(uint8_t out[32])
{
    uint32_t entropy = get_entropy();
    uint32_t entropy1 = get_entropy();
    uint8_t input[32] = {0};
    uint8_t aes_in[32] = {0};
    uint8_t aes_key[32] = {0};
    uint8_t round[32] = {0};
    uint8_t xor_in[32] = {0};
    mbcrypt_aes_input_t in;
    mbcrypt_aes_output_t out_;

    for (int i = 0; i < 32; i+= 4) {
        memcpy(input + i, &entropy, 4);
        entropy1++;
        memcpy(round + i, &entropy1, 4);
        entropy1++;
    }

    mbcrypt_sha256(input, 32, aes_key);
    mbcrypt_sha256(round, 32, xor_in);

    in.data = aes_in;
    in.data_len = 32;
    in.key = aes_key;
    in.key_len = 32;
    out_.out = out;
    out_.out_len = 32;
    mbcrypt_aes_encrypt(MBCRYPT_AES_ECB, AES256, MBCRYPT_AES_KEY_EXPANSION_NOT_REQUIRED, 
                        &in, &out_);
}

uint32_t check_user(protocol_message_t *message)
{
    uint32_t user_id = 0;
    uint8_t user_found = 0;
    uint32_t key_id;
    for (uint32_t user_id = 0; user_id < MAX_USER_STORAGE; ++user_id)
    {
        if (users_storage[user_id].is_user_active && users_storage[user_id].is_token_active)
        {
            if (memcmp(message->header.token, users_storage[user_id].token, TOKEN_SIZE) == 0)
            {
                user_found = 1;
                break;
            }
        }
    }
    if (user_found == 0)
    {
        user_id = -1; // max uint32_t
    }
    return user_id;
}

void handler_function__connect(protocol_message_t *message)
{
    LOG("Connect handler\n");

    uint32_t name_len = strlen(message->payload);
    uint32_t pass_len;
    uint32_t entropy = get_entropy();
    uint32_t user_index = 0;

    char name[MAX_NAME_SIZE] = {0};
    uint8_t pass[MAX_PASS_SIZE] = {0};
    uint8_t salt[MAX_NAME_SIZE + sizeof(entropy)];
    uint8_t data_buffer[MAX_TRANSMISSION_DATA_SIZE] = {0x00};
    uint8_t hashed_creds[KEY_IN_SIZE] = {0x00};

    mbcrypt_hash_callbacks_t hash_cbs;
    mbcrypt_sha256_t hash_ctx;
    mbcrypt_hmac_t hmac_ctx;
    mbcrypt_hmac_callbacks_t hmac_cbs;
    data_block_t data;
    

    hash_cbs.hash_init = mbcrypt_sha256_init;
    hash_cbs.hash_update = mbcrypt_sha256_update;
    hash_cbs.hash_final = mbcrypt_sha256_final;
    hash_cbs.hash_ctx = &hash_ctx;

    hmac_ctx.cbs = &hash_cbs;
    hmac_ctx.hash_type = MBCRYPT_HASH_TYPE_SHA256;

    hmac_cbs.hmac_ctx = &hmac_ctx;
    hmac_cbs.hmac_init = mbcrypt_hmac_init;
    hmac_cbs.hmac_update = mbcrypt_hmac_update;
    hmac_cbs.hmac_final = mbcrypt_hmac_final;

    if (name_len > message->header.length - 2)
    {
        LOG("Message wrong\n");
        return;
    }

    pass_len = message->header.length - name_len - 2; // '\x00' and CRC

    memcpy(name, message->payload, name_len);
    memcpy(pass, message->payload + name_len + 1, pass_len);

    mbcrypt_status_e res = mbcrypt_pbkdf2_hmac(MBCRYPT_HASH_TYPE_SHA256, &hmac_cbs, 
                    pass, pass_len, name, name_len, 
                    4096, hashed_creds, KEY_IN_SIZE);
    
    if (res != 0)
    {
        LOG("PBKDF2 error\n");
        return;
    }

    uint8_t user_found = 0;
    for (uint32_t i = 0; i < MAX_USER_STORAGE; ++i)
    {
        if (users_storage[i].is_user_active)
        {
            user_found = 1;
            for (uint32_t j = 0; j < KEY_IN_SIZE; ++j)
            {
                if (users_storage[i].key[j] != hashed_creds[j])
                {
                    user_found = 0;
                    break;
                }
            }
            if (user_found == 1)
            {
                user_index = i;
                break;
            }
        }
    }
    
    if (user_found == 0)
    {
        LOG("User cred not correct!\n");
        return;
    }

    LOG("User found!");

    memcpy(salt, &entropy, sizeof(entropy));
    memcpy(salt + sizeof(entropy), name, name_len);


    // password = pass
    // salt = entropy||name 
    res = mbcrypt_pbkdf2_hmac(MBCRYPT_HASH_TYPE_SHA256, &hmac_cbs, 
                    pass, pass_len, salt, sizeof(salt), 
                    4096, message->header.token, sizeof(message->header.token));

    if (res != 0)
    {
        LOG("PBKDF2 error\n");
        return;
    }

    // for (int i = 0; i < 32; i++)
    //     printf("%02X", message->header.token[i]);
    // printf("\n");

    users_storage[user_index].is_token_active = 1;
    memcpy(users_storage[user_index].token, message->header.token, sizeof(message->header.token));

    message->header.length = 1;
    data.size = sizeof(default_header_t) + 1;
    data.data = data_buffer;

    memcpy(data.data, &(message->header), sizeof(default_header_t));
    memcpy(data.data + sizeof(default_header_t), message->payload, 1);
    
    data.data[data.size - 1] = count_crc(data.data, data.size - 1);

    send_to(&data);
}

void handler_function__hash(protocol_message_t *message)
{
    LOG("hash handler\n");
    
    data_block_t data;
    uint8_t hash_out[MBCRYPT_SHA256_HASH_SIZE] = {0x00};
    uint8_t data_buffer[MAX_TRANSMISSION_DATA_SIZE] = {0x00};

    mbcrypt_sha256(message->payload, message->header.length - 1, hash_out);

    memcpy(data_buffer, &(message->header), sizeof(default_header_t));
    memcpy(data_buffer + sizeof(default_header_t), hash_out, MBCRYPT_SHA256_HASH_SIZE + 1);

    data.data = data_buffer;
    data.size = sizeof(default_header_t) + MBCRYPT_SHA256_HASH_SIZE + 1;   

    data.data[data.size - 1] = count_crc(data.data, data.size - 1);

    send_to(&data);
}

void handler_function__gen_key(protocol_message_t *message)
{
    LOG("Gen key handler\n");
    uint32_t new_key_id = -1;
    uint32_t user_id = 0;
    data_block_t data;
    uint32_t key_id;
    uint8_t data_buffer[MAX_TRANSMISSION_DATA_SIZE] = {0x00};

    user_id = check_user(message);

    if (user_id == (uint32_t)-1) 
    {
        LOG("No such user found!\n");
        return;
    }

    LOG("User found: %d\n", user_id);

    for (uint32_t i = 0; i < MAX_KEY_STORAGE; ++i) {
        if (key_storage[i].is_active == 0)
        {
            new_key_id = i;
            break;
        }
    }

    if (new_key_id > MAX_KEY_STORAGE)
    {
        LOG("Storage out of space\n");
        return;
    }

    prng_gen(key_storage[new_key_id].key);
    
    memcpy(message->payload, &new_key_id, sizeof(new_key_id));

    key_storage[new_key_id].is_active = 1;
    data.data = data_buffer;
    data.size = sizeof(default_header_t) + sizeof(new_key_id);   

    memcpy(data_buffer, &(message->header), sizeof(default_header_t));
    memcpy(data_buffer + sizeof(default_header_t), &new_key_id, sizeof(new_key_id));
    data.data[data.size - 1] = count_crc(data.data, data.size - 1);

    send_to(&data);
}

void handler_function__get_key(protocol_message_t *message)
{
    uint32_t user_id = 0;
    data_block_t data;
    uint32_t key_id;
    uint8_t data_buffer[MAX_TRANSMISSION_DATA_SIZE] = {0x00};

    user_id = check_user(message);

    if (user_id == (uint32_t)-1) 
    {
        LOG("No such user found!\n");
        return;
    }

    LOG("User found: %d\n", user_id);
    
    memcpy(&key_id, message->payload, sizeof(key_id));
    
    if (key_id >= MAX_KEY_STORAGE || key_storage[key_id].is_active != 1)
    {
        LOG("key id wrong!\n");
        return;
    }
    data.size = sizeof(message->header) + 1;
    data.size += GEN_KEY_SIZE;

    data.data = data_buffer;

    memcpy(data.data, &(message->header), sizeof(default_header_t));
    memcpy(data.data + sizeof(default_header_t), key_storage[key_id].key, GEN_KEY_SIZE);
    data.data[data.size - 1] = count_crc(data.data, data.size - 1);
    
    send_to(&data);
}

void handler_function__enc(protocol_message_t *message)
{
    LOG("Encryption handler\n");

    uint32_t user_id = 0;
    data_block_t data;
    uint32_t key_id;
    uint8_t data_buffer[MAX_TRANSMISSION_DATA_SIZE] = {0x00};
    uint8_t enc_out[MAX_TRANSMISSION_DATA_SIZE] = {0x00};
    uint8_t input_buffer[MAX_TRANSMISSION_DATA_SIZE] = {0x00};

    mbcrypt_aes_input_t aes_input;
    mbcrypt_aes_output_t aes_output;

    user_id = check_user(message);

    if (user_id == (uint32_t)-1) 
    {
        LOG("No such user found!\n");
        return;
    }
    
    memcpy(&key_id, message->payload, sizeof(key_id));
    
    if (key_id >= MAX_KEY_STORAGE || key_storage[key_id].is_active != 1)
    {
        LOG("key id wrong!\n");
        return;
    }
    aes_input.data = input_buffer;
    aes_input.data_len = message->header.length - 1 - sizeof(key_id);

    memcpy(input_buffer, message->payload + sizeof(key_id), aes_input.data_len);
    
    aes_input.key = key_storage[key_id].key;
    aes_input.key_len = GEN_KEY_SIZE;
    aes_output.out = enc_out;
    mbcrypt_aes_encrypt(MBCRYPT_AES_ECB, AES256, MBCRYPT_AES_KEY_EXPANSION_NOT_REQUIRED, 
                        &aes_input, &aes_output);

    data.size = sizeof(message->header) + 1;
    data.size += aes_output.out_len;

    data.data = data_buffer;

    memcpy(data.data, &(message->header), sizeof(default_header_t));
    memcpy(data.data + sizeof(default_header_t), aes_output.out, aes_output.out_len);
    data.data[data.size - 1] = count_crc(data.data, data.size - 1);
    
    send_to(&data);
}

void init_handler_function_array()
{
    handler_function_array[OP_CODE_CONNECT] = (handler_function)handler_function__connect;
    handler_function_array[OP_CODE_HASH] = (handler_function)handler_function__hash;
    handler_function_array[OP_CODE_GEN_KEY] = (handler_function)handler_function__gen_key;
    handler_function_array[OP_CODE_ENC] = (handler_function)handler_function__enc;
    handler_function_array[OP_CODE_GET_KEY] = (handler_function)handler_function__get_key;
}

// TEST fill in
void init_users_from_memory()
{
    /* TEST login and password */
    /* login - ilya*/
    /* password - ilya*/
    uint8_t key[] = {"\x04\x88\xff\x0f\x48\x82\xc7\x5f\x0d\x54\x6e\xd9\xb7\x73\x41\x80\x68\x0f\x7a\x43\x3a\x06\x97\x3a\xfb\x13\x3c\xb9\xb3\x5d\x6e\xf5"};
    users_storage[0].is_token_active = 0;
    users_storage[0].is_user_active = 1;
    memcpy(users_storage[0].key, key, 32);
}

void fw_start()
{
    LOG("Run core! \n");

    data_block_t packet;
    uint8_t message_buf[MAX_MESSAGE_SIZE] = {0x00};
    uint8_t data_buffer[MAX_TRANSMISSION_DATA_SIZE] = {0x00};

    protocol_message_t message;
    const uint32_t message_header_size = sizeof(message.header);
    uint8_t crc = 0;

    init_handler_function_array();
    init_users_from_memory();

    message.payload = message_buf;
    packet.data = data_buffer;

    while (1)
    {
        listen_to_connection(&packet);

        if (packet.size < message_header_size + 1)
        {
            LOG("Incorrect message!\n");
            continue;
        }

        if (packet.size >= MAX_TRANSMISSION_DATA_SIZE)
        {
            LOG("Message too big!\n");
            continue;
        }

        crc = count_crc(packet.data, packet.size - 1);

        if (crc != packet.data[packet.size - 1])
        {
            LOG("CRC is wrong!");
        }


        memcpy(&message.header, packet.data, message_header_size);
        memcpy(message.payload, packet.data + message_header_size, packet.size - message_header_size);

        if (handler_function_array[message.header.op_code] == NULL) 
        {
            LOG("Function undefined!\n");
        }
        else 
        {
            handler_function_array[message.header.op_code](&message);
        }
    }    
}