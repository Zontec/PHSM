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
*   @file vendor.h
*   @brief Vendor hash API file.
*	@author Zontec
*	@version 1.1
*	@date 15.12.2022
*/

#ifndef __VENDOR_H__
#define __VENDOR_H__

#include "SECTIONS.h"

/**
 * @brief ROM funcion. Get the vendor hash object. The number of round can be up to implementation.
 * 
 */
void get_vendor_hash() ROM_MEMORY_SECTION;

#endif /* __VENDOR_H__ */