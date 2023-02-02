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
*   @file base_init.h
*   @brief Basic initialization API.
*	@author Zontec
*	@version 1.1
*	@date 15.12.2022
*/

#ifndef __BASE_INIT_H__
#define __BASE_INIT_H__

#include "SECTIONS.h"

/**
 * @brief Function that is platform specific and called for the first stage of booting. 
 * Platform dependent. Empty for native platform
 * 
 */
void base_init() ROM_MEMORY_SECTION;

#endif // __BASE_INIT_H__