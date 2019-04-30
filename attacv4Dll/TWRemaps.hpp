#ifndef _TWREMAPS_HPP_
    #define _TWREMAPS_HPP_
    #if (defined(__DJGPP__) || defined(__EMX__))
        #define strcmpi(X,Y) stricmp(X,Y)
    #endif
    #if (defined(__linux__))
        #define strcmpi(X,Y) strcasecmp(X,Y)
        int getch(void);
        int kbhit(void);
    #endif
    // Add your compiler's define if it has _getch and _kbhit
    #if (!defined(__IBMCPP__) && !defined(__MINGW32__))
        #define _kbhit() kbhit()
    #endif
    #if (!defined(__IBMCPP__))
        #define _getch() getch()
    #endif
    void DisableCursor(void);
#endif
