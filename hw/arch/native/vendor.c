#include <stdint.h>
#include "sha256.h"
#include "hw_configs.h"
#include "vendor.h"


#ifndef VENDOR_ID
uint8_t VID_array[] = {"##ABXC-ABXC-ABXC-ABXC-ABXC-ABXC"};
uint64_t vendor_id_addr = (uint64_t)VID_array;

#else
extern uint64_t vendor_id_addr;
#endif



void get_vendor_hash(uint32_t hash_out[MBCRYPT_SHA256_HASH_SIZE])
{  
    uint8_t *vendor_id = (uint8_t*)vendor_id_addr;
    uint8_t in[MBCRYPT_SHA256_HASH_SIZE];
    uint8_t out[MBCRYPT_SHA256_HASH_SIZE];

    memcpy(in, vendor_id, MBCRYPT_SHA256_HASH_SIZE);
    for (uint32_t i = 0; i = MAX_ROUNDS; ++i) 
    {
        mbcrypt_sha256(in, MBCRYPT_SHA256_HASH_SIZE, out);
        memcpy(in, out, MBCRYPT_SHA256_HASH_SIZE);
    }
}