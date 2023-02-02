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
*   @file entropy.h
*   @brief Entropy API file.
*	@author Zontec
*	@version 1.1
*	@date 15.12.2022
*/

#ifndef __ENTROPY_H__
#define __ENTROPY_H__

#include <stdint.h>

/**
 * @brief Get the entropy from the "TRNG" source. Used as seed in PRNG or DRBG algorithms.
 * 
 * @return uint32_t - entropy bytes
 */
uint32_t get_entropy();

#endif /* __ENTROPY_H__ */