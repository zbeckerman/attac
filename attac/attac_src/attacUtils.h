#ifndef __attacUtils_h__
#define __attacUtils_h__

#include <stdio.h>		       /* for FILENAME_MAX */
#include <windows.h>

#define MAXAPPDIRLEN 1024
//for rexx ask and request
#define MAX_LENGTH 4096
#define MAX_AMOUNT 4096

#define LGTYP_ATTAC 4		       /* logmode: send this to attac buffer */
#define LGTYP_REXX 5		       /* logmode: send this to rexx buffer */

#define IDM_REXX     0x0300

#define	IAC	255		       /* interpret as command: */
#define	DONT	254		       /* you are not to use option */
#define	DO	253		       /* please, you use option */
#define	WONT	252		       /* I won't use option */
#define	WILL	251		       /* I will use option */
#define	SB	250		       /* interpret as subnegotiation */
#define	SE	240		       /* end sub negotiation */
#define TWX_ECHOMARK    200		       /* Enable TWX Echo-Marking... */
#define TWX_START 0
#define TWX_STOP 1
#define TWX_SCRIPT_LOADED 2
#define TWX_SCRIPT_TERMINATED 3

#define CR 13
#define LF 10
#define NUL 0

extern int attacloaded;

extern void *ldisc_extern;
//extern Terminal *term_extern;

extern char *logfile;
extern char reginaver[];
extern char ver[];
extern char *message_center_name;

/*
 * Window handles for the dialog boxes that can be running during a
 * PuTTY session.
 */
extern HWND logbox;
extern HWND globalHWnd;
extern HWND rexxask;
extern HWND rexxrequest;
extern HWND rexxnotify;
extern HWND showpp;
extern HWND showevilpp;
extern HWND showcolonization;
extern HWND showbuydump;
extern HWND showsellplanettoport;
extern HWND showpairport;
extern HWND showcimoptions;
extern HWND listbox_data;
extern HWND showswitchdatabase;
extern HWND showregistration;
extern HWND showquasarcalculator;
extern HWND showpaytolls;
extern HWND showbubblefinder;
extern HWND showbuyplanetshields;
extern HWND showstealmove;
extern HWND showstealtransport;
extern HWND showztm;
extern HWND showmegajettison;
extern HWND showbustlist;
extern HWND showfighter;
extern HWND showdeadends;
extern HWND showporttype;
extern HWND showmaxports;
extern HWND showmessagecenter;
extern HWND showmover;
extern HWND showsectorinfo;
extern HWND showetherexplore;
extern HWND showetherexplorelog;
extern HWND setallies;
extern HWND showplanet;
extern HWND shownearestfig;
extern HWND showblockedport;
extern HWND hwndTool; 
extern HWND hwndStatus; 
extern HWND showinfocenter;
extern HWND showfedcomm;
extern HWND showprivatecomm;
extern HWND showmacro;
extern HWND showmacroedit;
extern HWND showmap;
extern HWND showlimpitscanner;
extern HWND showremoteconnection;
extern HWND showworldtrade;
extern HWND showworldssm;
extern HWND showautoexplorer;
extern HWND showplanetbust;
extern HWND showaddbust;
extern HWND showtwxsync;

// Fields we care about for colorizing -- don't wanna look these
// up each time
extern HWND ctlAlign;
extern HWND ctlFigs;
extern HWND ctlShields;
extern HWND ctlTurns;
extern HWND ctlCredits;

//for report forms
extern HWND hShowPairPort;
extern HWND hShowEvilPairPort;
extern int showPPTab;

extern int isFedCommVisible;
extern int isPrivateCommVisible;

extern char releaseNumber[];
extern char majorNumber[];
extern char minorNumber[];



extern char exeDir[MAXAPPDIRLEN];
extern char scriptDir[MAXAPPDIRLEN];
extern char appDir[MAXAPPDIRLEN];
extern char fullFilePath[MAXAPPDIRLEN];

extern char rexxAskText[MAX_LENGTH];
extern short promptSave;

void updateATTACSettings(void *data);
void authorizeCallback (int isRegistrationValid, char *returnBuffer);
void test_debug(void);

#endif

