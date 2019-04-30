#ifndef _TWTYPES_HPP_
    #define _TWTYPES_HPP_

    #define ULONG unsigned long
    #define LONG long
    #define USHORT unsigned short
    #define UCHAR unsigned char
    #define CHAR char

    typedef unsigned char Bool;

    #if (!defined(__OS2__) && !defined(__linux__))
        #define NULL 0
    #endif
#endif
