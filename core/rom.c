#include "log.h"
#include "sha256.h"
#include "base_init.h"
#include "hw_configs.h"

#define ROM_DIE()   goto ROM_ABORT;

extern void __bootloader_secure_code_begin__();
extern void __bootloader_secure_code_end__();
extern void boot();

extern uint8_t bootloader_check_sum[MBCRYPT_SHA256_HASH_SIZE];


void main()
{
    LOG("ROM has started!\n");

    LOG("Init basic HW preif\n");
    /* Platform specific function. Should call the initialization of the hardware critical for the initial run */
    base_init();

    LOG("Check bootrom hashcode for integrity!\n");

    mbcrypt_sha256_t sha_ctx;

    mbcrypt_sha256_init(&sha_ctx);

    /* Begin and end addresses of checking regions. As the x64 platforms could be supported or PAE - int64 is used */
    uint64_t check_region_begin = (uint64_t)&__bootloader_secure_code_begin__;
    uint64_t check_region_end = (uint64_t)&__bootloader_secure_code_end__;

    const uint64_t block_size = 128; /* Block size could be any that is convenient */
    uint8_t tmp_buf[block_size];
    uint8_t check_sum_out[MBCRYPT_SHA256_HASH_SIZE];

    for (uint64_t region_shift = check_region_begin; region_shift < check_region_end; region_shift += block_size)
    {
        memcpy(tmp_buf, region_shift, block_size);
        if (mbcrypt_sha256_update(&sha_ctx, tmp_buf, block_size) != MBCRYPT_STATUS_OK)
        {
            ROM_DIE();
        } 
    }

    if (check_region_end & (block_size - 1))
    {
        if (mbcrypt_sha256_update(&sha_ctx, tmp_buf, check_region_end & (block_size - 1)) != MBCRYPT_STATUS_OK)
        {
            ROM_DIE();
        } 
    }

    if (mbcrypt_sha256_final(&sha_ctx, check_sum_out) != MBCRYPT_STATUS_OK)
    {
        ROM_DIE();
    } 

/* The line could be undef in platform-specific configuration file*/
#ifndef BOOT_SKIP_CHECK
    for (uint32_t i = 0; i < MBCRYPT_SHA256_HASH_SIZE; ++i)
    {
        if (check_sum_out[i] != bootloader_check_sum[i])
        {
            LOG("Checksum fail!\n");
            ROM_DIE();
        }
    }
#endif // BOOT_SKIP_CHECK

    LOG("Checksum success!\n");

    boot();

ROM_ABORT:
    LOG("ROM abort! \n");
    while(1);
}