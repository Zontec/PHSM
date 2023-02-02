#ifndef __LOG_H__
#define __LOG_H__


#if defined(ARCH) && (ARCH == NATIVE)
    #include <stdio.h>
    #define LOG     printf
#else
    #define LOG     (void)
#endif

#endif // __LOG_H__