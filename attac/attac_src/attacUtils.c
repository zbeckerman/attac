// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <windows.h>

#include "attacGlobal.h"
#include "../PUTTY.H"
#include "../DIALOG.H"
#include "attacUtils.h"
#include "win_res.h"
#include "util.h"

int enableLimpitScanner=0;


int attacloaded=0;
void *ldisc_extern;
//Terminal *term_extern;

char *logfile;
char *message_center_name;

/*
 * Window handles for the dialog boxes that can be running during a
 * PuTTY session.
 */
HWND logbox;
HWND globalHWnd;
HWND rexxask;
HWND rexxrequest;
HWND rexxnotify;
HWND showpp;
HWND showevilpp;
HWND showcolonization;
HWND showbuydump;
HWND showsellplanettoport;
HWND showpairport;
HWND showcimoptions;
HWND listbox_data;
HWND showswitchdatabase;
HWND showregistration;
HWND showquasarcalculator;
HWND showpaytolls;
HWND showbubblefinder;
HWND showbuyplanetshields;
HWND showstealmove;
HWND showstealtransport;
HWND showztm;
HWND showmegajettison;
HWND showbustlist;
HWND showfighter;
HWND showdeadends;
HWND showporttype;
HWND showmaxports;
HWND showmessagecenter;
HWND showmover;
HWND showsectorinfo;
HWND showetherexplore;
HWND showetherexplorelog;
HWND setallies;
HWND showplanet;
HWND shownearestfig;
HWND showblockedport;
HWND hwndTool; 
HWND hwndStatus; 
HWND showinfocenter;
HWND showfedcomm;
HWND showprivatecomm;
HWND showmacro;
HWND showmacroedit;
HWND showmap;
HWND showlimpitscanner;
HWND showremoteconnection;
HWND showworldtrade;
HWND showworldssm;
HWND showautoexplorer;
HWND showplanetbust;
HWND showaddbust;
HWND showtwxsync;

// Fields we care about for colorizing -- don't wanna look these
// up each time
HWND ctlAlign;
HWND ctlFigs;
HWND ctlShields;
HWND ctlTurns;
HWND ctlCredits;

//for report forms
HWND hShowPairPort;
HWND hShowEvilPairPort;
int showPPTab;

int isFedCommVisible;
int isPrivateCommVisible;


char exeDir[MAXAPPDIRLEN];
char scriptDir[MAXAPPDIRLEN];
char appDir[MAXAPPDIRLEN];
char fullFilePath[MAXAPPDIRLEN];

char rexxAskText[MAX_LENGTH];

void updateATTACSettings(void *data)
{
	char str[2048];
	HMENU hmenu;

    Config *tempCfg = (Config *)data;

	strcpy (str, gameName);
	if (strlen(str)>4)
		str[strlen(str)-4]=0;
	save_settings(str, !!strcmp(str, "Default Settings"),  tempCfg);

	header.sector_cnt= tempCfg->universesize;
	header.cimUpdateSectorsDefault = tempCfg->cimsectors;
	header.cimUpdatePortsDefault = tempCfg->cimports;
	header.daysToClearBust = tempCfg->daystoclearbusts;
	header.escapeSector = tempCfg->escapesector;
	header.terra = tempCfg->terrasector;
	
	header.tollTolls = tempCfg->fighters_action_toll;
	header.defensiveTolls = tempCfg->fighters_action_def;
	header.blindTwarpProtection = tempCfg->blindtwarpprotection;
	header.selfDestructProtection = tempCfg->selfdestructprotection;

	header.stealFactor = tempCfg->stealfactor;

	header.worstFactor = tempCfg->worsttradepercent;

	strcpy (header.fedCommKey, tempCfg->fedcommencrypt);
	strcpy (header.validStardockKeys, tempCfg->validstardockkeys);

	header.updateInfoBarStartup = tempCfg->updateinfoonstartup;
	header.announcementStartup = tempCfg->sendannouncementstartup;
	header.detachWindows = tempCfg->makewindowsdetachable;
	header.popupComms = tempCfg->usepopupwindowsforcomm;

	header.autoHaggle = tempCfg->autohaggle;
	header.autoUpdateMap = tempCfg->updatemapwhenmoving;
	
	header.remoteConnectionPort = tempCfg->remoteconnectionport;
	strcpy (header.remoteConnectionPassword, tempCfg->remoteconnectionpassword);
	
	header.minPairPortPercent = tempCfg->minpairportpercent;

	strcpy (header.TWXSyncServer, tempCfg->twxsyncserver);
	header.TWXSyncPort = tempCfg->twxsyncport;

	header.popupOnRexxCompletion = tempCfg->popupmessagewhenrexxcomplete;
	
	/* here we update the menu for the checkboxes */
	hmenu = GetMenu(GetParent(globalHWnd));

	if (header.blindTwarpProtection==1 && validRegCode==1)
	{
		CheckMenuItem(hmenu, IDM_BLIND_TWARP_PROTECTION, MF_CHECKED);
	}
	else
	{
		CheckMenuItem(hmenu, IDM_BLIND_TWARP_PROTECTION, MF_UNCHECKED);
	}

	if (header.selfDestructProtection==1 && validRegCode==1)
	{
		CheckMenuItem(hmenu, IDM_SELF_DESTRUCT_PROTECTION, MF_CHECKED);
	}
	else
	{
		CheckMenuItem(hmenu, IDM_SELF_DESTRUCT_PROTECTION, MF_UNCHECKED);
	}

	if (header.autoHaggle==1 && validRegCode==1)
	{
		CheckMenuItem(hmenu, IDM_AUTOHAGGLE, MF_CHECKED);
	}
	else
	{
		CheckMenuItem(hmenu, IDM_AUTOHAGGLE, MF_UNCHECKED);
	}			

	if (header.autoUpdateMap==1 && validRegCode==1)
	{
		CheckMenuItem(hmenu, IDM_AUTOUPDATEMAP, MF_CHECKED);
	}
	else
	{
		CheckMenuItem(hmenu, IDM_AUTOUPDATEMAP, MF_UNCHECKED);
	}			

	/* end update menu for check boxes */
}
