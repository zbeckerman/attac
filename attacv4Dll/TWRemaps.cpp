#include "TWRemaps.hpp"
#if (defined(__TCPLUSPLUS__) || defined(__DJGPP__))
    #include <conio.h>
#endif
#if (defined(__EMX__) || defined(__OS2__))
    #define INCL_VIO
    #include <os2.h>
    #include <memory.h>
#endif
#if (defined(__MINGW32__))
    #include <windows.h>
#endif

void DisableCursor(void)
{
    #if (defined(__EMX__) || defined(__OS2__))
        VIOCURSORINFO cursorInfo;
        memset(&cursorInfo, 0, sizeof(cursorInfo));
        cursorInfo.attr=-1;
        VioSetCurType(&cursorInfo, 0);
    #endif
    
    #if (defined(__TCPLUSPLUS__) || defined(__DJGPP__))
        _setcursortype(_NOCURSOR);
    #endif

    // the following doesn't work, but might in future compiler releases
    #ifdef __MINGW32__
        CONSOLE_CURSOR_INFO cursorInfo;
        HANDLE so;
        so=GetStdHandle(1);
        GetConsoleCursorInfo(so, &cursorInfo);
        cursorInfo.bVisible=FALSE;
        SetConsoleCursorInfo(so, &cursorInfo);
    #endif
}

#ifdef __linux__
    #include <stdio.h>
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <sys/time.h>
    #include <sys/types.h>
  
    #define CMIN  1
    #ifdef CTIME
        #undef CTIME
    #endif
    #define CTIME 1
  
    int getch(void)
        {
        char ch;
        int error;
        static struct termios Otty, Ntty;
  
        fflush(stdout);
        tcgetattr(0, &Otty);
        Ntty=Otty;
  
        Ntty.c_iflag=0;
        Ntty.c_oflag=0;
        Ntty.c_lflag=0;
        Ntty.c_cc[VMIN]=CMIN;
        Ntty.c_cc[VTIME]=CTIME;
  
        #define FLAG TCSANOW
  
        if (0==(error=tcsetattr(0, FLAG, &Ntty)))
            {
            error=read(0, &ch, 1);
            error+=tcsetattr(0, FLAG, &Otty);
            }
  
        return (error==1?(int)ch:-1);
        }
  
    int kbhit(void)
        {
        int cnt=0;
        int error;
        static struct termios Otty, Ntty;
  
        tcgetattr(0, &Otty);
        Ntty=Otty;
  
        Ntty.c_iflag= 0;
        Ntty.c_oflag= 0;
        Ntty.c_lflag &= ~ICANON;
        Ntty.c_cc[VMIN]=CMIN;
        Ntty.c_cc[VTIME]=CTIME;
  
        if (0==(error=tcsetattr(0, TCSANOW, &Ntty)))
            {
            struct timeval tv;
            error+=ioctl(0, FIONREAD, &cnt);
            error+=tcsetattr(0, TCSANOW, &Otty);
            tv.tv_sec=0;
            tv.tv_usec=100;
            select(1, NULL, NULL, NULL, &tv);
            }
  
        return (error==0?cnt:-1);
        }
#endif
