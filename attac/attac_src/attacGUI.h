#ifndef __attacGUI_h__
#define __attacGUI_h__

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <process.h>

#include "attacUtils.h"
#include "win_res.h"
#include "rexx.h"
#include "util.h"
#include "writeMemory.h"
#include "structs.h"
#include "ztm.h"
#include "linklist.h"
#include "bubbleFinder.h"
#include "info.h"
#include "map.h"
#include "remoteConnection.h"
#include "parsedatathread.h"

void listview_insert_column(HWND hlistTmp, int column, int col_width, int pos, char *text);
void listview_insert(HWND hlistTmp, int row, char *port1, char *port2, char *minamount, char *minpercent, char *dist);
void listview_insert_twxsync(HWND hlistTmp, int row, char *name, char *sessionId);
void listview_insert_planet(HWND hlistTmp, int row, char *sector, char *planet, char * owner);
void listview_insert_bubble_entrances(HWND hlistTmp, int row, char *sector, char *size);
void listview_insert_bubble_sectors(HWND hlistTmp, int row, char *sector);
void listview_insert_allied(HWND hlistTmp, int row, char *trader, char *status);
void listview_insert_bust(HWND hlistTmp, int row, char *sector, char *daybusted, char *person, char *daysfromtoday);
void listview_insert_limpitscanner(HWND hlistTmp, int row, char *tkey, char *tdata);
void listview_insert_remote_connection(HWND hlistTmp, int row, char *num, char *ipaddress, char *time, char *status);
void listview_insert_macro(HWND hlistTmp, int row, char *tkey, char *tdata);
void listview_insert_blocked_port(HWND hlistTmp, int row, char *sector, char *timestamp);
void listview_insert_fighter(HWND hlistTmp, int row, char *sector, char *amount, char *owner, char* type);
void listview_insert_deadend(HWND hlistTmp, int row, char *deadend);
void listview_insert_porttype(HWND hlistTmp, int row, char *sector, char *fuelamount, char *fuelpercent, char* orgamount, char* orgpercent, char* equipamount, char* equippercent, char* dist);

void updateRemoteConnectionList (HWND remoteListHandle);
int CALLBACK AboutProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void showmacroform(HWND hwnd);
void showsellplanettoportform(HWND hwnd);
void showplanetbustform(HWND hwnd);
void showworldtradeform(HWND hwnd);
void showworldssmform(HWND hwnd);
void showautoexplorerform(HWND hwnd);
void showremoteconnectionform(HWND hwnd);
void showinfocenterform(HWND hwnd);

void showztmform(HWND hwnd);
void showpaytollsform(HWND hwnd);
void showdeadendform(HWND hwnd);
void showfighterform(HWND hwnd);
void showporttypeform(HWND hwnd, int type);
void show_evil_pair_ports(HWND hwnd);
void show_pair_ports(HWND hwnd);
void showmegajettisonform(HWND hwnd);
void showrexxnotify(HWND hwnd, char *promptData);
void showregistrationform(HWND hwnd);
void showsetallies(HWND hwnd);
void showswitchdatabaseform(HWND hwnd);
void showbubblefinderform(HWND hwnd);
void showsectorinfoform(HWND hwnd);
void showplanetform (HWND hwnd);
void shownearestfigform(HWND hwnd);
void showcolonizationform(HWND hwnd);
void showmoverform(HWND hwnd, int type);
void showmessagecenterform(HWND hwnd);
void showmessagecenterform(HWND hwnd);
void showbuydumpform(HWND hwnd);
void showpairportform(HWND hwnd);
void showstealtransportform(HWND hwnd);
void showbustlistform(HWND hwnd);
void showmaxportsform(HWND hwnd);
void showstealmoveform(HWND hwnd);
void showquasarcalculatorform(HWND hwnd);
void showetherexplorelogform(HWND hwnd);
void showetherexploreform(HWND hwnd);
void showcimoptionsform(HWND hwnd);
void showrexxask(HWND hwnd, char *promptData, char *defaultData);
void showrexxaskpassword(HWND hwnd, char *promptData, char *defaultData);
void showrexxrequest(HWND hwnd, long RequestArgumentCount, char *RequestArguments[], int rexxRequestList);
void showbuyplanetshieldsform(HWND hwnd);
void showfedcommform(HWND hwnd);
void showprivatecommform(HWND hwnd);
void reDrawMacros (HWND macrolisthandle);
void readMacros();
void writeMacros();
void listview_insert_macro(HWND hlistTmp, int row, char *key, char *data);
void showmacroeditform(HWND hwnd, char *caption, char *data);
void showmapform(HWND hwnd);
void showlimpitscannerform(HWND hwnd);
void updateLimpitScanner (HWND limpitscannerhandle, long foundSector);
void showaddbustform(HWND hwnd);
void showtwxsyncform(HWND hwnd);

void show_listbox_data(HWND hwnd, int type);
void showblockedportform(HWND hwnd);
void showBlockedPortData(HANDLE hlistBlockedPort);
HWND DoCreateStatusBar(HWND hwndParent, int nStatusID, HINSTANCE hinst, int nParts);
void save_session (char str[], HWND hwnd);
int CALLBACK MyCompProc(LPARAM lp1, LPARAM lp2, LPARAM lp3);
void set_MessageWindow_title(char *title);
void startSysopPortScript();
void startSysopAddToClosedGame ();

void CALLBACK ShowRexxAskTimer(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );
void CALLBACK ShowRexxAskPassTimer(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );
void CALLBACK ShowRexxRequestTimer(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );

#define NO_OF_SUBITEM 15
#define UP 1
#define DOWN 2

extern int enableLimpitScanner;
extern int nItem1, nItem2;
extern int SortSubno[NO_OF_SUBITEM];
extern char *defaultDataRexxAsk;
extern char *promptDataRexxAsk;


void findBusts(HANDLE bustlisthandle, long bustDaysToClear_int, long sectorToClear_int);
void updatepairportcheckboxes (HANDLE frmHnd, int type, long curSector);
void clearBustsFunc();
void showFighterData(HANDLE hlistFig);
void showDeadEndData(HWND hdeadEndList, int depth);
void loadEtherLog(HWND hwnd, char option[], char thedate[]);
void loadAlliedList(HWND AlliedHwnd);
void loadPlanetList(HWND PlanetHwnd);


#endif

