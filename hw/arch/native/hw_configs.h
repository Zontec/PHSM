/****************************INFORMATION***********************************
* Copyright (c) 2022 Zontec
* Email: dehibeo@gmail.com
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
**************************************************************************/
/*!
*   @file hw_configs.h
*   @brief Platform specific configuration file.
*	@author Zontec
*	@version 1.1
*	@date 15.12.2022
*/

#ifndef __NATIVE_SETTINGS__
#define __NATIVE_SETTINGS__


#define UDP_PORT                            (8888)
#define MAX_ROUNDS                          (4096)
#define MAX_MESSAGE_SIZE                    (1024)
#define MAX_TRANSMISSION_DATA_SIZE          (1024)
#define GEN_KEY_SIZE                        (32)
#define SALT_KEY_SIZE                       (32)
#define MAX_KEY_STORAGE                     (32)
#define MAX_USER_STORAGE                    (4)
#define MAX_NAME_SIZE                       (32)
#define MAX_PASS_SIZE                       (32)
#define KEY_IN_SIZE                         (32)
#define TOKEN_SIZE                          (32)

/**
 * @brief If defined - skip the first bootrom check
 * 
 */
#define BOOT_SKIP_CHECK

/**
 * @brief If defined - skip the second bootloader check of the firmware
 * 
 */
#define FW_SKIP_CHECK

#endif // __NATIVE_SETTINGS__