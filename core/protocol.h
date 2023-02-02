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
*   @file protocol.h
*   @brief Protocol specification file.
*	@author Zontec
*	@version 1.1
*	@date 15.12.2022
*/

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>

/**
* @brief op-codes for the packets
*/
enum {
    OP_CODE_CONNECT = 0xAF,
    OP_CODE_HASH = 0xBF,
    OP_CODE_GEN_KEY = 0xCF,
    OP_CODE_ENC = 0xDF,
    OP_CODE_GET_KEY = 0x2F,
};

/**
 * @brief default_header - header that is included to each transaction 
 * 
 */
#pragma pack(1)
typedef struct default_header {
    uint8_t op_code;
    uint32_t nonce;
    uint8_t token[32];
    uint32_t length;
} default_header_t;

/**
 * @brief protocol_message - complete message used for the exchange of data between client and PHSM
 * 
 */
#pragma pack(1)
typedef struct protocol_message {
    default_header_t header;
    uint8_t *payload;
    uint8_t crc;
} protocol_message_t;


#endif /* __PROTOCOL_H__ */