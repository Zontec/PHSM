#include "SECTIONS.h"
#include "log.h"
#include "sha256.h"
#include "hmac.h"
#include "communication_driver.h"
#include "hw_configs.h"


/* Functions are nacked as used as labels */
void __bootloader_secure_code_begin__() BOOTLOADER_MEMORY_SECTION FUNCTION_NACKED;
void __bootloader_secure_code_end__() BOOTLOADER_MEMORY_SECTION FUNCTION_NACKED;

/* Bootloader safe function declaration */
void boot() BOOTLOADER_MEMORY_SECTION;

extern struct fw_section *__start_fw_section;
extern struct fw_section *__stop_fw_section;
extern void fw_start();
extern void fw_update();
extern void hw_config_next();

extern uint8_t master_key[MBCRYPT_SHA256_HASH_SIZE];
extern uint8_t fw_check_sum[MBCRYPT_SHA256_HASH_SIZE];

/* Begin of safe code */
void __bootloader_secure_code_begin__() {}

#define BOOT_DIE()  goto BOOT_ABORT;

void boot_memcpy(uint8_t *dst, uint8_t *src, uint32_t size) 
{
    for (uint32_t i = 0; i < size; ++i)
    {
        dst[i] = src[i];
    }
}

void boot()
{
    LOG("Boot proccess in non-rom memory!\n");

    mbcrypt_sha256_t sha_ctx;

    uint64_t check_region_begin = (uint64_t)&__start_fw_section;
    uint64_t check_region_end = (uint64_t)&__stop_fw_section;

    const uint64_t block_size = 128;
    uint8_t tmp_buf[block_size];
    uint8_t check_sum_out[MBCRYPT_SHA256_HASH_SIZE];
    mbcrypt_hash_callbacks_t cbs;
    uint8_t fw_signature_out[MBCRYPT_SHA256_HASH_SIZE];
    mbcrypt_sha256_t hash_ctx;

    mbcrypt_sha256_init(&sha_ctx);

    cbs.hash_init = mbcrypt_sha256_init;
    cbs.hash_update = mbcrypt_sha256_update;
    cbs.hash_final = mbcrypt_sha256_final;
    cbs.hash_ctx = &hash_ctx;

    for (uint64_t region_shift = check_region_begin; region_shift < check_region_end; region_shift += block_size)
    {
        boot_memcpy(tmp_buf, region_shift, block_size);
        if (mbcrypt_sha256_update(&sha_ctx, tmp_buf, block_size) != MBCRYPT_STATUS_OK)
        {
            BOOT_DIE();
        } 
    }

    if (check_region_end & (block_size - 1))
    {
        if (mbcrypt_sha256_update(&sha_ctx, tmp_buf, check_region_end & (block_size - 1)) != MBCRYPT_STATUS_OK)
        {
            BOOT_DIE();
        } 
    }

    if (mbcrypt_sha256_final(&sha_ctx, check_sum_out) != MBCRYPT_STATUS_OK)
    {
        BOOT_DIE();
    } 

    if (mbcrypt_hmac(MBCRYPT_HASH_TYPE_SHA256, &cbs, master_key, sizeof(master_key), 
                                check_sum_out, sizeof(check_sum_out), fw_signature_out) != MBCRYPT_STATUS_OK)
    {
        BOOT_DIE();
    }

/* Can be set up in HW configs */
#ifndef FW_SKIP_CHECK
    for (uint32_t i = 0; i < sizeof(master_key); ++i)
    {
        if (fw_check_sum[i] != fw_signature_out[i])
        {
            LOG("Checksum fw fault!\n");
            BOOT_DIE();
        }
    }
#endif // FW_SKIP_CHECK

    LOG("Checksum fw success!\n");

    boot_unsecure();

BOOT_ABORT:
    while(1);
}

/* End of safe code */
void __bootloader_secure_code_end__() {}

void boot_unsecure()
{
    LOG("Unprotected boot section!\n");

    hw_config_next();
    LOG("HW configuration done!\n");

    init_communication_driver();
    LOG("Init communication driver done!\n");

    fw_update();
    LOG("FW update done!\n");

    fw_start();   
}