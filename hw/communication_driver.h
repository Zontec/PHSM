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
*   @file communication_driver.h
*   @brief Communication driver API file. The implementation should use the API 
*           for setting the communication between the client and the core.
*	@author Zontec
*	@version 1.1
*	@date 15.12.2022
*/

#ifndef __COMMUNICATION_DRIVER_H__
#define __COMMUNICATION_DRIVER_H__

#include <stdint.h>
#include "hw_configs.h"

/**
 * @brief enum for returning errors
 * 
 */
typedef enum {
    COM_DRIVER_SUCCESS = 0x0000,
    COM_DRIVER_ERROR_UNKNOWN = 0x0001,
    COM_DRIVER_ERROR_CLOSE_CONNECTION = 0x0002,
    COM_DRIVER_ERROR_SEND_DATA = 0x0004,
    COM_DRIVER_ERROR_LISTEN_CONNECTION = 0x0008,
    COM_DRIVER_ERROR_INIT = 0x000A,
} communication_driver_status_e;


/**
 * @struct data_block_t
 * @brief Data block type for sending or receiving packets.
 */
typedef struct {
    uint8_t *data;
    uint32_t size; 
} data_block_t;

/**
 * @brief Init the communication driver. Should be called once.
 * 
 * @return communication_driver_status_e 
 */
communication_driver_status_e init_communication_driver();

/**
 * @brief Listen for any data to be received. Blocking function.
 * 
 * @param packet - pointer to the structure
 * @return communication_driver_status_e 
 */
communication_driver_status_e listen_to_connection(data_block_t *packet);

/**
 * @brief Sent data to the pre-defined client/
 * 
 * @param packet - pointer to the packet to be sent
 * @return communication_driver_status_e 
 */
communication_driver_status_e send_to(data_block_t *packet);

/**
 * @brief Close communication.
 * 
 * @return communication_driver_status_e 
 */
communication_driver_status_e close_communication();

#endif /* __COMMUNICATION_DRIVER_H__ */