/* Status Info for ATTAC v4.0 
   Michael C. Welsh
   06/15/2002
   RagingMF@attbi.com
*/

#ifndef INFO_DOT_H
#define INFO_DOT_H

#ifdef _WINDOWS
#include <windows.h>
#endif

#define WM_UNHIDE		WM_USER+16
#define WM_PARENTMOVED	WM_USER+17
#define WM_PARENTSIZED	WM_USER+18
#define WM_MSG			WM_USER+19

#define BLACK	RGB(0,0,0)
#define GREEN	RGB(0,255,0)
#define RED		RGB(255, 0, 0)
#define YELLOW	RGB(255, 255, 0)
#define BLUE	RGB(0, 0, 255)
#define CYAN	RGB(0, 255, 255)
#define MAGENTA RGB(255, 0, 255)
#define WHITE	RGB(255, 255, 255)
#define BROWN	RGB(128, 128, 0)

#define ANSI_BROWN 27
#define ANSI_BLACK	30
#define ANSI_RED	31
#define ANSI_GREEN	32
#define ANSI_YELLOW	33
#define ANSI_BLUE	34
#define ANSI_MAGENTA	35
#define ANSI_CYAN	36
#define ANSI_WHITE	37
#define INFO_LENGTH 128

#define TRUE 1
#define FALSE 0

void update_status_bar();
HWND createInfoWindow(HINSTANCE inst, HWND parent);
HWND createMsgWindow(HINSTANCE inst, HWND parent);
void addToMessageCenter(char * msg, int fgcolor, int bgcolor, BOOL flag);
void setScriptMessage(char* msg);
char* addCommaToNumber(long number, int shrink);

COLORREF GetColorForTextFromAnsiString(char str[]);
char* StripAnsiFromString(char str[]);
COLORREF GetColorForText(HDC, HWND);

extern long countMax;
extern HANDLE StatusBarMutex;

#endif
