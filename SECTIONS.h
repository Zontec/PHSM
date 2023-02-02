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
*   @file SECTIONS.h
*   @brief Sections definition file.
*	@author Zontec
*	@version 1.1
*	@date 15.12.2022
*/

#ifndef __SECTIONS_H__
#define __SECTIONS_H__

/**
    Memory section marker used for bootloader sections. 
*/
#define BOOTLOADER_MEMORY_SECTION       __attribute__ ((section ("bootloader")))

/**
    Memory section marker used for ROM sections. 
*/
#define ROM_MEMORY_SECTION              __attribute__ ((section ("rom")))

/**
    Memory section marker used for main firmware sections. 
*/
#define FW_MEMORY_SECTION               __attribute__ ((section ("fw_section")))

/**
    Marks function as empty. Used as label in C. 
*/
#define FUNCTION_NACKED                 __attribute__ ((nacked))

#endif /* __SECTIONS_H__ */