// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <windows.h>
#include <commctrl.h>

#include "attacGlobal.h"
#include "attacGUI.h"
#include "../PUTTY.H"
#include "attacUtils.h"
#include "rexx.h"
#include "structs.h"
#include "util.h"
#include "shortestPath.h"
#include "win_res.h"
#include "linklist.h"
#include "parsedatathread.h"
#include "statusthread.h"

TWX_FILE_HEADER header;
TWX_SECTOR *sectors=NULL;
TWX_PLAYER *players=NULL;
TWX_PLANET *planets=NULL;

MACRO_STRUCT *theMacros=NULL;
int macroCount=0;
int helperActive=1;

planetListPtr **listPlanets;

int loadedPlanets=0;
long currentSector;
int isOffline=0;
int isConnected=1;

void EnumerateFolderFS(char *path)
{
	char searchPath[MAX_PATH];
	WIN32_FIND_DATA ffd; // file information struct
	HANDLE sh;
	int flip=0;
	char buffer1[MAX_LENGTH];
	FILETIME ft;
	SYSTEMTIME st;

	// a wildcard needs to be added to the end of the path, e.g. "C:\*"
	strcpy(searchPath, path);

	sh = FindFirstFile(searchPath, &ffd);
	if(INVALID_HANDLE_VALUE == sh) return; // not a proper path i guess

	addToMainWindow("", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	// enumerate all items; NOTE: FindFirstFile has already got info for an item
	do {
		FileTimeToLocalFileTime(&ffd.ftLastWriteTime, &ft);
		FileTimeToSystemTime(&ft, &st);

		if (flip==0)
		{
			sprintf (buffer1, "%s: %s - %ldk - Last Written: %04d/%02d/%02d %02d:%02d:%02d", ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY? "dir" : "file", ffd.cFileName, ffd.nFileSizeLow, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
			flip=1;

		}
		else
		{
			sprintf (buffer1, "%s: %s - %ldk - Last Written: %04d/%02d/%02d %02d:%02d:%02d", ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY? "dir" : "file", ffd.cFileName, ffd.nFileSizeLow, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
			flip=0;
		}

	} while (FindNextFile(sh, &ffd));

	FindClose(sh);
}


void printdebuginfo()
{
	char buffer1[50], buf[MAX_PATH];
	char *dirname;
    int i, flip=0, tmpInt;
	FILE *fp;
	struct sesslist sesslist;

	get_sesslist(&sesslist, TRUE);

	addToMainWindow("", ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	

	sprintf(buffer1, "Version: %s", ver);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Is Registered: %s", (validRegCode==1?"Yes":"No"));
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Is HandleValid: %s", (isRegistered==1?"Yes":"No"));
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Is TWX Sync Registered: %s", (validTWXSyncRegCode==1?"Yes":"No"));
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	

	sprintf(buffer1, "ID: %s", header.id);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "atc file version: %ld", header.ver);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "universe size: %ld", header.sector_cnt);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Rylos: %ld", header.cls0port_rylos);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Alpha: %ld", header.cls0port_alpha);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Sol: %ld", header.cls0port_sol);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "CRC: %ld", header.crc32);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Max Planets In DB: %ld", header.planet_cnt);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Max Players In DB: %ld", header.player_cnt);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Name of Game: %s", header.name);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Address of Game: %s", header.address);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Port of Game: %d", header.port);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Update CIM Sectors: %d", header.cimUpdateSectorsDefault);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Pay Tolls: %d", header.tollTolls);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Days to Clear Busts: %d", header.daysToClearBust);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Update CIM Ports: %d", header.cimUpdatePortsDefault);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Kill Defensive Figs: %d", header.defensiveTolls);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	
	sprintf(buffer1, "escape Sector: %ld", header.escapeSector);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Terra: %ld", header.terra);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "ZTM end-1: %ld", header.lastZTMFromEndTillOne);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "ZTM 1-end: %ld", header.lastZTMFromOneTillEnd);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
	sprintf(buffer1, "Last Robbed: %ld", header.lastRobbedSector);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Last Busted: %ld", header.lastBustedSector);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	

	sprintf(buffer1, "last Announcment: %ld", header.lastAnnouncementStartup);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	

	sprintf(buffer1, "last Update Time: %ld", header.lastUpdateTime);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "last Sync Time: %ld", header.lastSyncTimeWithServer);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "TWX SessionId: %s", header.TWXSyncSession);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "TWX Session Password: %s", header.TWXSyncPassword);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "TWXSync Server: %s", header.TWXSyncServer);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "TWXSync Port: %d", header.TWXSyncPort);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	

	

	sprintf(buffer1, "Self Destruct: %d", header.selfDestructProtection);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
  	sprintf(buffer1, "Port On Map: %d", header.showPortOnMap);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Blind Twarp: %d", header.blindTwarpProtection);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Steal Factor: %d", header.stealFactor);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Update Map: %d", header.autoUpdateMap);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Worst Factor: %d", header.worstFactor);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	

	sprintf(buffer1, "Valid SD keys: %s", header.validStardockKeys);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	
	sprintf(buffer1, "Fed Comm Key: %s", header.fedCommKey);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Update Info Bar: %d", header.updateInfoBarStartup);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Update Announcements: %d", header.announcementStartup);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
  
	sprintf(buffer1, "Last CIM: %ld", header.lastTimeCIMRun);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	

	sprintf(buffer1, "Detach Windows: %d", header.detachWindows);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);	
	sprintf(buffer1, "Popup Comms: %d", header.popupComms);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);	

	strcpy (buf, exeDir);
	strcat (buf, "*.*");
	addToMainWindow(buf, ATTAC_RED, ATTAC_BLACK, TRUE, 1);	
	dirname = (char *) strdup (buf);
	EnumerateFolderFS(dirname);
	#ifdef P_FREE
		pfree(&dirname);
	#else
		free(dirname);
	#endif

	strcpy (buf, appDir);
	strcat (buf, "*.*");
	addToMainWindow(buf, ATTAC_RED, ATTAC_BLACK, TRUE, 1);	
	dirname = (char *) strdup (buf);
	EnumerateFolderFS(dirname);
	#ifdef P_FREE
		pfree(&dirname);
	#else
		free(dirname);
	#endif

	strcpy (buf, scriptDir);
	strcat (buf, "*.*");
	addToMainWindow(buf, ATTAC_RED, ATTAC_BLACK, TRUE, 1);	
	dirname = (char *) strdup (buf);
	EnumerateFolderFS(dirname);
	#ifdef P_FREE
		pfree(&dirname);
	#else
		free(dirname);
	#endif


	addToMainWindow("", ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("", ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("Current Game", ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("-----------", ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
	sprintf(buffer1, "Game: %s", gameName);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);

	addToMainWindow("", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("Saved Games", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("-----------", ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);

	for (i = 0; i < sesslist.nsessions; i++)
	{
		if (strcmp(sesslist.sessions[i], "Default Settings") != 0)
		{
			if (flip==0)
			{
				sprintf(buffer1, "Game %d: %s.atc", i, sesslist.sessions[i]);
				addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
				flip=1;
			}
			else
			{
				sprintf(buffer1, "Game %d: %s.atc", i, sesslist.sessions[i]);
				addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
				flip=0;
			}
		}
	}
	
	DisplayOS();
	DisplaySystemVersion();

	sprintf(buffer1, "ATTAC Version %s", ver);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);

	for (i=0; i< LIMPIT_COUNT_MAX; i++)
		statusTest[i]=1;


	fp = fopen("statusdata.txt", "r");

	if(fp==NULL) 
		outputData("Error: can't open file statusdata.txt.");
	else 
	{
		outputData("File statusdata.txt opened successfully.");

		i = 0 ;    

		while(!feof(fp)) 
		{ /* loop through and store the numbers into the array */
		  fscanf(fp, "%d", &tmpInt);
		  statusTest[tmpInt]=0;
		  i++;
		}

		fclose(fp);
	}

	for (i=0; i< LIMPIT_COUNT_MAX; i++)
		parserTest[i]=1;


	fp = fopen("parsedata.txt", "r");

	if(fp==NULL) 
		outputData("Error: can't open file parsedata.txt.");
	else 
	{
		outputData("File parsedata.txt opened successfully.");

		i = 0 ;    

		while(!feof(fp)) 
		{ /* loop through and store the numbers into the array */
		  fscanf(fp, "%d", &tmpInt);
		  parserTest[tmpInt]=0;
		  i++;
		}

		fclose(fp);
	}
}


ATTAC_ERRORS create_new_twx(char *session_file)
{
  long i;

	//free code here
	if (sectors!=NULL) {
		pfree(&sectors);
	}
	if (players!=NULL) {
		pfree(&players);
	}
	if (planets!=NULL) {
		pfree(&planets);
	}


  // clean session info
  sectors=NULL;
  players=NULL;
  planets=NULL;

  //set initial values for header
  strncpy(header.id, "ATTC", 4);
  header.ver = 2;
  header.sector_cnt=cfg.universesize;
  header.stardock=-1;
  header.cls0port_sol=-1;
  header.cls0port_alpha=-1;
  header.cls0port_rylos=-1;
  header.crc32=0;
  header.player_cnt=MAX_PLAYERS;
  header.planet_cnt=MAX_PLANETS;
  strcpy(header.name, cfg.host);
  strcpy(header.address, cfg.host);
  header.port=cfg.port;
  header.tollTolls=cfg.fighters_action_toll;
  header.defensiveTolls=cfg.fighters_action_def;
  header.escapeSector=cfg.escapesector;
  header.daysToClearBust=(short) cfg.daystoclearbusts;
  header.lastBustedSector=-1;
  header.lastRobbedSector=-1;
  strcpy(header.fedCommKey, cfg.fedcommencrypt);
  strcpy(header.validStardockKeys, cfg.validstardockkeys);
  header.lastZTMFromOneTillEnd=1;
  header.lastZTMFromEndTillOne=1;
  header.lastTimeCIMRun=-1;
  
  header.stealFactor=(short) cfg.stealfactor;
  header.worstFactor= (short) cfg.worsttradepercent;
  header.selfDestructProtection= cfg.selfdestructprotection;  //1 is Y, 0 is N
  header.blindTwarpProtection= cfg.blindtwarpprotection;  //1 is Y, 0 is N
  header.cimUpdateSectorsDefault= cfg.cimsectors;  //1 is Y, 0 is N
  header.cimUpdatePortsDefault= cfg.cimports;  //1 is Y, 0 is N
  header.updateInfoBarStartup=cfg.updateinfoonstartup;  //1 is Y, 0 is N
  header.lastAnnouncementStartup=0;  //time of last announcement
  header.announcementStartup=cfg.sendannouncementstartup;  //1 is Y, 0 is N
  header.autoUpdateMap=cfg.updatemapwhenmoving;  //1 is Y, 0 is N
  header.showPortOnMap=1;   //1 is Y, 0 is N
  header.detachWindows=cfg.makewindowsdetachable;   //1 is Y, 0 is N
  header.popupComms=cfg.usepopupwindowsforcomm;   //1 is Y, 0 is N
  header.autoHaggle=cfg.autohaggle;
  header.forceNoHaggle=0;	//data remembered for pair port
  header.remoteConnectionPort=cfg.remoteconnectionport;
  strcpy (header.remoteConnectionPassword, cfg.remoteconnectionpassword);
  header.minPairPortPercent= (short) cfg.minpairportpercent;
  strcpy (header.TWXSyncServer, cfg.twxsyncserver);
  header.TWXSyncPort= (short) cfg.twxsyncport;

  header.lastUpdateTime=0;
  header.lastSyncTimeWithServer=0;
  header.popupOnRexxCompletion=cfg.popupmessagewhenrexxcomplete;

  header.terra=cfg.terrasector;
  memset(header.TWXSyncSession, 0, sizeof(header.TWXSyncSession));
  memset(header.TWXSyncPassword, 0, sizeof(header.TWXSyncPassword));
  memset(header.reserved, 0, sizeof(header.reserved));
  memset(header.reserved2, 0, sizeof(header.reserved2));

  // get memory for session info and clear it
  sectors = (TWX_SECTOR *) malloc(header.sector_cnt * sizeof(TWX_SECTOR));
  players = (TWX_PLAYER *) malloc(header.player_cnt * sizeof(TWX_PLAYER));
  planets = (TWX_PLANET *) malloc(header.planet_cnt * sizeof(TWX_PLANET));
  
  for (i = 0; i < header.sector_cnt; i++)
  {
    sectors[i].info = -1;

    sectors[i].navhaz= -1;
    sectors[i].sector_update=0;
    sectors[i].ftrs=-1;
    sectors[i].ftr_owner=-1;
    sectors[i].ftr_type=-1;
    sectors[i].anom=-1;
    sectors[i].armids=-1;
    sectors[i].armid_owner=-1;
    sectors[i].limpets=-1;
    sectors[i].limpet_owner=-1;
    sectors[i].port_amt[0]=-1;
    sectors[i].port_amt[1]=-1;
    sectors[i].port_amt[2]=-1;
    sectors[i].port_per[0]=-1;
    sectors[i].port_per[1]=-1;
    sectors[i].port_per[2]=-1;
    sectors[i].warps=0;
    sectors[i].reserved2=0;
    sectors[i].warp_sect[0]=0;
    sectors[i].warp_sect[1]=0;
    sectors[i].warp_sect[2]=0;
    sectors[i].warp_sect[3]=0;
    sectors[i].warp_sect[4]=0;
    sectors[i].warp_sect[5]=0;
    sectors[i].port_update=0;
    sectors[i].density=-1;
    sectors[i].lastBusted=-1;
    sectors[i].playerIdBusted=-1;
    sectors[i].avoided=0;
    memset(&sectors[i].reserved, 0, sizeof(sectors[i].reserved));
  }
  for (i = 0; i < header.player_cnt; i++)
  {
    strcpy(players[i].id, "PLAY");
    players[i].autonum = -1;
    players[i].corpNumber = -1;
    players[i].alliance = '1';	//set everyone to an enemy
	players[i].reserved2 = 0;	
    players[i].experience = -1;
    players[i].alignment = -1;
    players[i].lastKnownSector = -1;
    players[i].last_update = -1;
    memset(&players[i].shipName, 0, sizeof(players[i].shipName));
    memset(&players[i].name, 0, sizeof(players[i].name));
	memset(&players[i].reserved, 0, sizeof(players[i].reserved));
  }
  /* set the default user */
  strcpy(players[0].name, "ME");
  players[0].autonum=0;
  players[0].alliance='0';
  players[0].last_update = ATTAC_Time();
  players[0].experience = 0;
  players[0].alignment = 0;
  players[0].lastKnownSector = 1;
  players[0].corpNumber=-1;

  /* set the rogue mercenary user */
  strcpy(players[1].name, "Rogue Mercenaries");
  players[1].autonum=1;
  players[1].last_update = ATTAC_Time();
  players[1].experience = -1;
  players[1].alignment = -1;
  players[1].lastKnownSector = -1;
  players[1].corpNumber=-1;


  for (i = 0; i < header.planet_cnt; i++)
  {
    strcpy(planets[i].id, "PLAN");
    planets[i].autonum = -1;
    planets[i].planetNumber = -1;
    planets[i].sector = -1;
    planets[i].createdBy = -1;
    planets[i].ownedBy = -1;
    planets[i].citadel = -1;
    planets[i].citclass = 0;
    planets[i].mReaction = -1;
    planets[i].iGenerator = -1;
    planets[i].transwarpPower = -1;
    planets[i].transportPower = -1;
    planets[i].qCannonPower = -1;
    planets[i].qCannonAtmos = -1;
    planets[i].qCannonSector = -1;
    planets[i].reserved2 = 0;
    planets[i].fighters = -1;
    planets[i].shields = -1;
    planets[i].treasury = -1;
    planets[i].figsPerDay = -1;
    planets[i].totalColonists = -1;
    planets[i].last_update = -1;    
    memset(&planets[i].name, 0, sizeof(planets[i].name));
	memset(&planets[i].reserved, 0, sizeof(planets[i].reserved));
    }

	//create planet link list
	/* create data struct for planets */
	if (loadedPlanets==1)
	{
		for (i=0; i < header.planet_cnt; i++)
		{
			if (listPlanets[i] != NULL)
			{
				delete_link_list(i);
			}
		}
		
		pfree((void**)&listPlanets);
		

	}

	loadedPlanets=1;
	listPlanets = (planetListPtr**) malloc (header.sector_cnt * sizeof (planetListPtr));
	//NULL out the array
	for (i = 0; i < header.sector_cnt; i++)
	{
		listPlanets[i]=NULL;
	}

 
  return ERROR_NONE;
}


ATTAC_ERRORS load_twx(char *session_file)
{
	FILE *fp;
	size_t read;
	long i;
	char buffer[40];
	struct htab *curhash;

	TWX_FILE_HEADER tmp_header;
	TWX_SECTOR tmp_sectors;
	TWX_PLAYER tmp_players;
	TWX_PLANET tmp_planets;

	//delete old players
	//now create the player hashTable from the data in memory
	if (hashInUse==1)
	{
		for (i=0; i<MAX_PLAYERS; i++)
		{
			if (players[i].autonum >=0)
			{
				sprintf(data, "Removing key \'%s\'", players[i].name);
				outputData (data);
				delhash(players[i].name) ? printf("Removed Key\n") : printf("Could not remove %s\n", players[i].name);
			}
		}
	}  
	// clean session info
	sectors = NULL;
	players = NULL;
	planets = NULL;

	// open file
	fp = fopen(session_file, "rb");
	if (fp == NULL)
	{
		if (restoreDBFromBackup()==0)
			load_twx(session_file);
		return ERROR_FILE_ACCESS;
	}

	// get twx header
	read = fread(&tmp_header, sizeof(TWX_FILE_HEADER), READ_WRITE_AMOUNT, fp);
	if (read < READ_WRITE_AMOUNT)
	{
		if (restoreDBFromBackup()==0)
			load_twx(session_file);
		fclose(fp);
		return ERROR_FILE_ACCESS;
	}

	strcpy (header.id, tmp_header.id);
	header.time_created = ntohl(tmp_header.time_created);
	header.ver = ntohl(tmp_header.ver);

	//version check for attac DB
	if (header.ver<2)
	{
		MessageBox(NULL, "Please delete and recreate your database. i updated the format. ATTAC now backs up and restores your database if there is an error", "ATTAC Fatal Error", MB_ICONERROR | MB_OK);
		attacErrorMessage ("Please delete and recreate your database. i updated the format");
		return 0;
	}

	header.sector_cnt = ntohl(tmp_header.sector_cnt);
	header.stardock = ntohl(tmp_header.stardock);
	header.cls0port_sol = ntohl(tmp_header.cls0port_sol);
	header.cls0port_alpha = ntohl(tmp_header.cls0port_alpha);
	header.cls0port_rylos = ntohl(tmp_header.cls0port_rylos);
	header.crc32 = ntohl(tmp_header.crc32);
	header.player_cnt = ntohl(tmp_header.player_cnt);
	header.planet_cnt = ntohl(tmp_header.planet_cnt);
	strcpy(header.reserved, tmp_header.reserved);
	strcpy(header.reserved2, tmp_header.reserved2);

	//read connection data
	strcpy(header.name, tmp_header.name);
	strcpy(header.address,tmp_header.address);
	header.port = ntohs(tmp_header.port);
	header.tollTolls=ntohs(tmp_header.tollTolls);
	header.defensiveTolls=ntohs(tmp_header.defensiveTolls);
	header.escapeSector = ntohl(tmp_header.escapeSector);
	header.daysToClearBust = ntohs(tmp_header.daysToClearBust);
	header.lastBustedSector = ntohl(tmp_header.lastBustedSector);
	header.lastRobbedSector = ntohl(tmp_header.lastRobbedSector);
	strcpy(header.fedCommKey, tmp_header.fedCommKey );
	strcpy(header.validStardockKeys, tmp_header.validStardockKeys);
	header.lastZTMFromOneTillEnd = ntohl(tmp_header.lastZTMFromOneTillEnd);
	header.lastZTMFromEndTillOne = ntohl(tmp_header.lastZTMFromEndTillOne);
	header.lastTimeCIMRun = ntohl(tmp_header.lastTimeCIMRun);

	header.stealFactor = ntohs(tmp_header.stealFactor);
	header.worstFactor = ntohs(tmp_header.worstFactor);
	header.selfDestructProtection = ntohs(tmp_header.selfDestructProtection);
	header.blindTwarpProtection = ntohs(tmp_header.blindTwarpProtection);
	header.cimUpdateSectorsDefault = ntohs(tmp_header.cimUpdateSectorsDefault);
	header.cimUpdatePortsDefault = ntohs(tmp_header.cimUpdatePortsDefault);
	header.updateInfoBarStartup = ntohs(tmp_header.updateInfoBarStartup);
	header.lastAnnouncementStartup = ntohl(tmp_header.lastAnnouncementStartup);
	header.announcementStartup = ntohs(tmp_header.announcementStartup);


    header.detachWindows=ntohs(tmp_header.detachWindows);
    header.popupComms=ntohs(tmp_header.popupComms);
	header.autoHaggle=ntohs(tmp_header.autoHaggle);
	header.forceNoHaggle = ntohs(tmp_header.forceNoHaggle);
    header.remoteConnectionPort=ntohl(tmp_header.remoteConnectionPort);
    strcpy (header.remoteConnectionPassword, tmp_header.remoteConnectionPassword);
	header.minPairPortPercent = ntohs(tmp_header.minPairPortPercent);

    header.autoUpdateMap = ntohs(tmp_header.autoUpdateMap);
	header.showPortOnMap = ntohs(tmp_header.showPortOnMap);
	header.terra = ntohl(tmp_header.terra);
	header.popupOnRexxCompletion = ntohs(tmp_header.popupOnRexxCompletion);
	header.lastUpdateTime = ntohl(tmp_header.lastUpdateTime);
	header.lastSyncTimeWithServer = ntohl(tmp_header.lastSyncTimeWithServer);
	strcpy(header.TWXSyncSession, tmp_header.TWXSyncSession );
	strcpy(header.TWXSyncPassword, tmp_header.TWXSyncPassword );
	strcpy(header.TWXSyncServer, tmp_header.TWXSyncServer );
	header.TWXSyncPort = ntohs(tmp_header.TWXSyncPort);
	
	strcpy(header.reserved, tmp_header.reserved );

	if (strncmp(header.id, "ATTC", 4) != 0)
	{
		if (restoreDBFromBackup()==0)
			load_twx(session_file);
		fclose(fp);
		return ERROR_FILE_ACCESS;
	}

	if (sectors!=NULL) {
		pfree(&sectors);
	}
	if (players!=NULL) {
		pfree(&players);
	}
	if (planets!=NULL) {
		pfree(&planets);
	}

	// get memory for session info and clear it
	sectors = (TWX_SECTOR *) malloc(header.sector_cnt * sizeof(TWX_SECTOR));
	players = (TWX_PLAYER *) malloc(header.player_cnt * sizeof(TWX_PLAYER));
	planets = (TWX_PLANET *) malloc(header.planet_cnt * sizeof(TWX_PLANET));

	for (i = 0; i < header.sector_cnt; i++)
	{
		sectors[i].info = -1;
	}
	for (i = 0; i < header.player_cnt; i++)
	{
		players[i].autonum = -1;
	}
	for (i = 0; i < header.planet_cnt; i++)
	{
		planets[i].autonum = -1;
		planets[i].sector = -1;
	}

	// read sector info
	for (i = 0; i < header.sector_cnt; i++)
	{
		read = fread(&tmp_sectors, sizeof(TWX_SECTOR), READ_WRITE_AMOUNT, fp);

		if (read < READ_WRITE_AMOUNT)
		{
			if (restoreDBFromBackup()==0)
				load_twx(session_file);
			fclose(fp);
			return ERROR_FILE_ACCESS;
		}

		sectors[i].info = tmp_sectors.info;
		sectors[i].navhaz = tmp_sectors.navhaz;
		sectors[i].reserved2 = ntohs(tmp_sectors.reserved2);
		sectors[i].ftr_type = tmp_sectors.ftr_type;
		sectors[i].anom = tmp_sectors.anom;
		sectors[i].port_per[0] = tmp_sectors.port_per[0];
		sectors[i].port_per[1] = tmp_sectors.port_per[1];
		sectors[i].port_per[2] = tmp_sectors.port_per[2];
		sectors[i].warps = tmp_sectors.warps;

		sectors[i].sector_update = ntohl(tmp_sectors.sector_update);
		sectors[i].ftrs = ntohl(tmp_sectors.ftrs);
		sectors[i].port_amt[0] = ntohl(tmp_sectors.port_amt[0]);
		sectors[i].port_amt[1] = ntohl(tmp_sectors.port_amt[1]);
		sectors[i].port_amt[2] = ntohl(tmp_sectors.port_amt[2]);
		sectors[i].warp_sect[0] = ntohl(tmp_sectors.warp_sect[0]);
		sectors[i].warp_sect[1] = ntohl(tmp_sectors.warp_sect[1]);
		sectors[i].warp_sect[2] = ntohl(tmp_sectors.warp_sect[2]);
		sectors[i].warp_sect[3] = ntohl(tmp_sectors.warp_sect[3]);
		sectors[i].warp_sect[4] = ntohl(tmp_sectors.warp_sect[4]);
		sectors[i].warp_sect[5] = ntohl(tmp_sectors.warp_sect[5]);
		sectors[i].port_update = ntohl(tmp_sectors.port_update);
		sectors[i].density = ntohl(tmp_sectors.density);
		sectors[i].lastBusted = ntohl(tmp_sectors.lastBusted);    
		sectors[i].playerIdBusted = ntohl(tmp_sectors.playerIdBusted);    

		sectors[i].ftr_owner = ntohs(tmp_sectors.ftr_owner);
		sectors[i].armids = ntohs(tmp_sectors.armids);
		sectors[i].armid_owner = ntohs(tmp_sectors.armid_owner);
		sectors[i].limpets = ntohs(tmp_sectors.limpets);
		sectors[i].limpet_owner = ntohs(tmp_sectors.limpet_owner);
		sectors[i].avoided = tmp_sectors.avoided;
		strcpy(sectors[i].reserved, tmp_sectors.reserved);
	}

	// read player info
	for (i = 0; i < header.player_cnt; i++)
	{
		read = fread(&tmp_players, sizeof(TWX_PLAYER), READ_WRITE_AMOUNT, fp);
		if (read < READ_WRITE_AMOUNT)
		{
			if (restoreDBFromBackup()==0)
				load_twx(session_file);
			fclose(fp);
			return ERROR_FILE_ACCESS;
		}
		players[i].experience = ntohl(tmp_players.experience);
		players[i].alignment = ntohl (tmp_players.alignment);
		players[i].lastKnownSector = ntohl(tmp_players.lastKnownSector);
		players[i].last_update = ntohl(tmp_players.last_update);
		players[i].reserved2 = tmp_players.reserved2;
		players[i].corpNumber = ntohs(tmp_players.corpNumber);
		players[i].alliance = tmp_players.alliance;
		strcpy(players[i].name, tmp_players.name);
		players[i].autonum = ntohs(tmp_players.autonum);
		strcpy(players[i].reserved, tmp_players.reserved);
	}

	// read planet info
	for (i = 0; i < header.planet_cnt; i++)
	{
		read = fread(&tmp_planets, sizeof(TWX_PLANET), READ_WRITE_AMOUNT, fp);
		if (read < READ_WRITE_AMOUNT)
		{
			if (restoreDBFromBackup()==0)
				load_twx(session_file);
			fclose(fp);
			return ERROR_FILE_ACCESS;
		}

		planets[i].autonum = ntohs(tmp_planets.autonum);
		planets[i].planetNumber = ntohs(tmp_planets.planetNumber);
		planets[i].createdBy = ntohs(tmp_planets.createdBy);
		planets[i].ownedBy = ntohs(tmp_planets.ownedBy);
		planets[i].citadel = tmp_planets.citadel; 
		planets[i].citclass = tmp_planets.citclass;
		planets[i].mReaction = tmp_planets.mReaction;
		planets[i].iGenerator = tmp_planets.iGenerator;
		planets[i].transwarpPower = ntohs(tmp_planets.transwarpPower);
		planets[i].transportPower = ntohs(tmp_planets.transportPower);
		planets[i].qCannonPower = tmp_planets.qCannonPower;
		planets[i].qCannonAtmos = tmp_planets.qCannonAtmos;
		planets[i].qCannonSector = tmp_planets.qCannonSector;
		planets[i].reserved2 = tmp_planets.reserved2;
		planets[i].sector = ntohl (tmp_planets.sector);
		planets[i].fighters = ntohl (tmp_planets.fighters); 
		planets[i].shields = ntohl (tmp_planets.shields); 
		planets[i].treasury = ntohl (tmp_planets.treasury);
		planets[i].figsPerDay = ntohl (tmp_planets.figsPerDay);
		planets[i].totalColonists = ntohl (tmp_planets.totalColonists);
		planets[i].last_update = ntohl (tmp_planets.last_update);
		strcpy(planets[i].name, tmp_planets.name);
		strcpy(planets[i].reserved, tmp_planets.reserved);
	}

	fclose(fp);

	//create hash table for players

	hashInUse=1;
	//add data to hash table
	for (i = 0; i < header.player_cnt; i++)
	{
		if (players[i].autonum>=0)
		{
			sprintf (buffer, "%d", players[i].autonum);

			curhash = addhash(players[i].name, buffer);
			sprintf(data, "Added \'%s\' to \'%s\' - corp %d", curhash->data, curhash->key, players[atoi(curhash->data)].corpNumber);
			outputData (data);
		}
	}


	//create planet link list
	/* create data struct for planets */
	if (loadedPlanets==1)
	{
		for (i=0; i < header.planet_cnt; i++)
		{
			if (listPlanets[i] != NULL)
			{
				delete_link_list(i);
			}
		}
		
		pfree((void**)&listPlanets);
	}

	listPlanets = (planetListPtr**) malloc (header.sector_cnt * sizeof (planetListPtr));
	loadedPlanets=1;

	//NULL out the array
	for (i = 0; i < header.sector_cnt; i++)
	{
		listPlanets[i]=NULL;
	}

	//now read in the memory from disk

	for (i=0; i < header.planet_cnt; i++)
	{
		if (planets[i].sector > 0)
		{
			insert_link_list_node(planets[i].name, (planets[i].sector)-1);
			sprintf (data, "loading %s into sector %ld", planets[i].name,planets[i].sector);
			outputData (data);
			printf ("%ld: %s\n", i+1, data);

		}
	}
	return ERROR_NONE;
}


/*passing a 1 for type returns the equip-org with the highest amount, 
  passing a 2 for type returns the equip-fuel with the highest amount, 
  passing a 3 for type returns the org-fuel with the highest amount

  when type==0 then we print to the widget, otherwise... we dont access it
  since it would result in a gtk-error/warning since the widget doesnt exist
*/

char* generate_pp(long amount_filter, long percent_filter, long route_sector, int type, int pairType)
{
	long i, j, k;
	long warp, min_amount=0, min_percent=0;
	int EOCount=0, EFCount=0, OFCount=0, pp_count=0;
	char *tmp[5];
	char *buffer;
	unsigned int *shortestPath;
	long greatestAmount=0;
	char* greatestAmountSectors;

	greatestAmountSectors = (char*) malloc (50);
	strcpy (greatestAmountSectors, "0 0"); /* set sectors to 0 */

	if (isConnected==0 && isOffline==0)
	{
		attacErrorMessage("No Game Selected. Click File->Connect. Then select a game. Then Work Offline or Connect");
		return (greatestAmountSectors);
	}

	buffer = (char*) malloc (50);

	//generate shortest path list
	if (route_sector<=0 || route_sector>header.sector_cnt)
		shortestPath = (unsigned int*) ShortestPathToAll(currentSector);
	else
		shortestPath = (unsigned int*) ShortestPathToAll(route_sector);

	ListView_DeleteAllItems (hShowPairPort);

	// generate pp information
	for (i = 0; i < header.sector_cnt; i++)
	{
		if (sectors[i].info != -1)
		{
			for (j = 0; j < 6; j++)
			{
				warp = sectors[i].warp_sect[j];
				if (warp == 0)
				break;
				warp--;
				if (warp > i && sectors[warp].info != -1)
				{
					for (k = 0; k < 6; k++)
					{
						if (sectors[warp].warp_sect[k] == 0)
						{
							break;
						}
						/* equipment - organics */
						if (sectors[warp].warp_sect[k] - 1 == i)
						{
							if ((pairType==1) &&
								(((sectors[i].info == 1 || sectors[i].info == 5) &&
								(sectors[warp].info == 2 || sectors[warp].info == 4))
								||
								((sectors[i].info == 2 || sectors[i].info == 4) &&
								(sectors[warp].info == 1 || sectors[warp].info == 5))))
							{
								/* if sector is on bust list mark with asterik */
								if (isBustedSector(i+1))
									sprintf (buffer, "%ld*\n", i+1);
								else
									sprintf (buffer, "%ld", i+1);
								tmp[0] = (char *) strdup(buffer);


								if (isBustedSector(warp+1))
									sprintf (buffer, "%ld*", warp+1);
								else
									sprintf (buffer, "%ld", warp+1);
								tmp[1] = (char *) strdup(buffer);

								/*
								x = (a<b ? a : b);
								y = (c<d ? c : d);
								min = ( (a<b ? a : b) < (c<d ? c : d) ? (a<b ? a : b) : (c<d ? c : d));
								*/
								min_amount = 
									( (sectors[i].port_amt[1] < sectors[i].port_amt[2] 
									? sectors[i].port_amt[1] : sectors[i].port_amt[2])
									< (sectors[warp].port_amt[1] < sectors[warp].port_amt[2] 
									? sectors[warp].port_amt[1] : sectors[warp].port_amt[2])
									? (sectors[i].port_amt[1] < sectors[i].port_amt[2] 
									? sectors[i].port_amt[1] : sectors[i].port_amt[2]) 
									: (sectors[warp].port_amt[1] < sectors[warp].port_amt[2] 
									? sectors[warp].port_amt[1] : sectors[warp].port_amt[2]));
								if (min_amount==-1)
									sprintf (buffer, "??");	
								else
									sprintf (buffer, "%ld", min_amount);
								tmp[2] = (char *) strdup(buffer);

								min_percent = 
									( (sectors[i].port_per[1] < sectors[i].port_per[2] 
									? sectors[i].port_per[1] : sectors[i].port_per[2])
									< (sectors[warp].port_per[1] < sectors[warp].port_per[2] 
									? sectors[warp].port_per[1] : sectors[warp].port_per[2]) 
									? (sectors[i].port_per[1] < sectors[i].port_per[2]
									? sectors[i].port_per[1] :  sectors[i].port_per[2])
									: (sectors[warp].port_per[1] < sectors[warp].port_per[2] 
									? sectors[warp].port_per[1] : sectors[warp].port_per[2]));
								if (min_percent==-1)
									sprintf (buffer, "??");
								else
									sprintf (buffer, "%ld%%", min_percent);
								tmp[3] = (char *) strdup(buffer);

								if (shortestPath[i]==255)
									strcpy(buffer, "N/A");
								else
									sprintf (buffer, "%d", shortestPath[i]);
								tmp[4] = (char *) strdup(buffer);

								//set best port here for rexx
								if (type==1)
								{
									if (sectors[i].avoided==0 &&
										sectors[warp].avoided==0 &&
										min_percent > header.minPairPortPercent && 
										min_amount> greatestAmount &&
										((i+1) != header.lastRobbedSector
										&& (warp+1) != header.lastRobbedSector) &&
										(sectors[i].lastBusted == -1 
										&& sectors[warp].lastBusted == -1))
									{
										greatestAmount = min_amount;
										sprintf (buffer, "%ld %ld", i+1, warp+1);
										strcpy (greatestAmountSectors, buffer);
									}
								}

								//only add if it is greater than filter
								if (((min_amount>= amount_filter) && (min_percent>=percent_filter)) ||
								(amount_filter==0 && percent_filter==0))
								{
									listview_insert(hShowPairPort, pp_count++, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
									EOCount++;
								}		    


								#ifdef P_FREE
									pfree(&tmp[0]);
									pfree(&tmp[1]);
									pfree(&tmp[2]);
									pfree(&tmp[3]);
									pfree(&tmp[4]);
								#else
									free(tmp[0]);
									free(tmp[1]);
									free(tmp[2]);
									free(tmp[3]);
									free(tmp[4]);
								#endif		
							}

							/* equipment - fuel */
							if ((pairType==2) &&
								(((sectors[i].info == 1 || sectors[i].info == 6) &&
								(sectors[warp].info == 3 || sectors[warp].info == 4))
								||
								((sectors[i].info == 3 || sectors[i].info == 4) &&
								(sectors[warp].info == 1 || sectors[warp].info == 6))))
							{
								/* if sector is on bust list mark with asterik */
								if (isBustedSector(i+1))
									sprintf (buffer, "%ld*", i+1);
								else
									sprintf (buffer, "%ld", i+1);
								tmp[0] = (char *) strdup(buffer);

								if (isBustedSector(warp+1))
									sprintf (buffer, "%ld*", warp+1);
								else
									sprintf (buffer, "%ld", warp+1);
								tmp[1] = (char *) strdup(buffer);


								/*
								x = (a<b ? a : b);
								y = (c<d ? c : d);
								min = ( (a<b ? a : b) < (c<d ? c : d) ? (a<b ? a : b) : (c<d ? c : d));
								*/
								min_amount = 
									( (sectors[i].port_amt[0] < sectors[i].port_amt[2] 
									? sectors[i].port_amt[0] : sectors[i].port_amt[2])
									< (sectors[warp].port_amt[0] < sectors[warp].port_amt[2] 
									? sectors[warp].port_amt[0] : sectors[warp].port_amt[2])
									? (sectors[i].port_amt[0] < sectors[i].port_amt[2] 
									? sectors[i].port_amt[0] : sectors[i].port_amt[2]) 
									: (sectors[warp].port_amt[0] < sectors[warp].port_amt[2] 
									? sectors[warp].port_amt[0] : sectors[warp].port_amt[2]));

								if (min_amount==-1)
									sprintf (buffer, "??");	
								else
									sprintf (buffer, "%ld", min_amount);
								tmp[2] = (char *) strdup(buffer);

								min_percent = 
									( (sectors[i].port_per[0] < sectors[i].port_per[2] 
									? sectors[i].port_per[0] : sectors[i].port_per[2])
									< (sectors[warp].port_per[0] < sectors[warp].port_per[2] 
									? sectors[warp].port_per[0] : sectors[warp].port_per[2]) 
									? (sectors[i].port_per[0] < sectors[i].port_per[2]
									? sectors[i].port_per[0] :  sectors[i].port_per[2])
									: (sectors[warp].port_per[0] < sectors[warp].port_per[2] 
									? sectors[warp].port_per[0] : sectors[warp].port_per[2]));

								if (min_percent==-1)
									sprintf (buffer, "??");	
								else
									sprintf (buffer, "%ld%%", min_percent);
								tmp[3] = (char *) strdup(buffer);

								if (shortestPath[i]==255)
									strcpy(buffer, "N/A");
								else
									sprintf (buffer, "%d", shortestPath[i]);
								tmp[4] = (char *) strdup(buffer);

								//set best port here for rexx
								if (type==2)
								{
									if (sectors[i].avoided==0 &&
										sectors[warp].avoided==0 &&
										min_percent > header.minPairPortPercent && 
										min_amount> greatestAmount &&
										((i+1) != header.lastRobbedSector
										&& (warp+1) != header.lastRobbedSector) &&
										(sectors[i].lastBusted == -1 
										&& sectors[warp].lastBusted == -1))
									{
										greatestAmount = min_amount;
										sprintf (buffer, "%ld %ld", i+1, warp+1);
										strcpy (greatestAmountSectors, buffer);
									}
								}

								if (((min_amount>= amount_filter) && (min_percent>=percent_filter)) ||
									(amount_filter==0 && percent_filter==0))
								{
									listview_insert(hShowPairPort, pp_count++, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
									EFCount++;
								}

								#ifdef P_FREE
									pfree(&tmp[0]);
									pfree(&tmp[1]);
									pfree(&tmp[2]);
									pfree(&tmp[3]);
									pfree(&tmp[4]);
								#else
									free(tmp[0]);
									free(tmp[1]);
									free(tmp[2]);
									free(tmp[3]);
									free(tmp[4]);
								#endif		

							}

							/* fuel - organics */
							if ((pairType==3) &&
								(((sectors[i].info == 2 || sectors[i].info == 6) &&
								(sectors[warp].info == 3 || sectors[warp].info == 5))
								||
								((sectors[i].info == 3 || sectors[i].info == 5) &&
								(sectors[warp].info == 2 || sectors[warp].info == 6))))
							{
								/* if sector is on bust list mark with asterik */
								if (isBustedSector(i+1))
									sprintf (buffer, "%ld*", i+1);
								else
									sprintf (buffer, "%ld", i+1);
								tmp[0] = (char *) strdup(buffer);

								if (isBustedSector(warp+1))
									sprintf (buffer, "%ld*", warp+1);
								else
									sprintf (buffer, "%ld", warp+1);
								tmp[1] = (char *) strdup(buffer);

								/*
								x = (a<b ? a : b);
								y = (c<d ? c : d);
								min = ( (a<b ? a : b) < (c<d ? c : d) ? (a<b ? a : b) : (c<d ? c : d));
								*/
								min_amount = 
									( (sectors[i].port_amt[1] < sectors[i].port_amt[0] 
									? sectors[i].port_amt[1] : sectors[i].port_amt[0])
									< (sectors[warp].port_amt[1] < sectors[warp].port_amt[0] 
									? sectors[warp].port_amt[1] : sectors[warp].port_amt[0])
									? (sectors[i].port_amt[1] < sectors[i].port_amt[0] 
									? sectors[i].port_amt[1] : sectors[i].port_amt[0]) 
									: (sectors[warp].port_amt[1] < sectors[warp].port_amt[0] 
									? sectors[warp].port_amt[1] : sectors[warp].port_amt[0]));

								if (min_amount==-1)
									sprintf (buffer, "??");	
								else
									sprintf (buffer, "%ld", min_amount);
								tmp[2] = (char *) strdup(buffer);

								min_percent = 
									( (sectors[i].port_per[1] < sectors[i].port_per[0] 
									? sectors[i].port_per[1] : sectors[i].port_per[0])
									< (sectors[warp].port_per[1] < sectors[warp].port_per[0] 
									? sectors[warp].port_per[1] : sectors[warp].port_per[0]) 
									? (sectors[i].port_per[1] < sectors[i].port_per[0]
									? sectors[i].port_per[1] :  sectors[i].port_per[0])
									: (sectors[warp].port_per[1] < sectors[warp].port_per[0] 
									? sectors[warp].port_per[1] : sectors[warp].port_per[0]));

								if (min_percent==-1)
									sprintf (buffer, "??");	
								else
									sprintf (buffer, "%ld%%", min_percent);
								tmp[3] = (char *) strdup(buffer);

								if (shortestPath[i]==255)
									strcpy(buffer, "N/A");
								else
									sprintf (buffer, "%d", shortestPath[i]);
								tmp[4] = (char *) strdup(buffer);

								//set best port here for rexx
								if (type==3)
								{
									if (sectors[i].avoided==0 &&
										sectors[warp].avoided==0 &&
										min_percent > header.minPairPortPercent && 
										min_amount> greatestAmount &&
										((i+1) != header.lastRobbedSector
										&& (warp+1) != header.lastRobbedSector) &&
										(sectors[i].lastBusted == -1 
										&& sectors[warp].lastBusted == -1))
									{
										greatestAmount = min_amount;
										sprintf (buffer, "%ld %ld", i+1, warp+1);
										strcpy (greatestAmountSectors, buffer);
									}
								}

								if (((min_amount>= amount_filter) && (min_percent>=percent_filter)) ||
									(amount_filter==0 && percent_filter==0))
								{
									listview_insert(hShowPairPort, pp_count++, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
									OFCount++;
								}

								#ifdef P_FREE
									pfree(&tmp[0]);
									pfree(&tmp[1]);
									pfree(&tmp[2]);
									pfree(&tmp[3]);
									pfree(&tmp[4]);
								#else
									free(tmp[0]);
									free(tmp[1]);
									free(tmp[2]);
									free(tmp[3]);
									free(tmp[4]);
								#endif		

							}
							break;
						}
					}
				}
			}
		}

		//check for registration or restrict report
		//we multiply by 3 since there are three reports
		if (isRegistered == 0 && EOCount >= MAXCYCLES)
		{
			if (validRegCode==0 || isConnected==1)
				break;
		}
		if (isRegistered == 0 && EFCount >= MAXCYCLES)
		{
			if (validRegCode==0 || isConnected==1)
				break;
		}
		if (isRegistered == 0 && OFCount >= MAXCYCLES)
		{
			if (validRegCode==0 || isConnected==1)
				break;
		}
		
	}

	if (pairType==1)
		sprintf (buffer, "Total: %d", EOCount);
	else if (pairType==2)
		sprintf (buffer, "Total: %d", EFCount);
	else if (pairType==3)
		sprintf (buffer, "Total: %d", OFCount);

	SetDlgItemText(GetParent(hShowPairPort), IDC_SHOW_PP_COUNT, buffer);

	#ifdef P_FREE
		pfree(&shortestPath);
		pfree(&buffer);
	#else
		free(shortestPath);
		free(buffer);
	#endif		

	return (greatestAmountSectors);
}


char* generate_evil_pp(long amount_filter, long percent_filter, long route_sector, int type)
{
  long i, j, k;
  long warp, min_amount=0, min_percent=0;
  int evilCount=0;
  char *tmp[5];
  char *buffer;
  unsigned int *shortestPath;
  long greatestAmount=0;
  char* greatestAmountSectors;
  long evil_pp_count=0;

  greatestAmountSectors = (char*) malloc (50);
  strcpy (greatestAmountSectors, "0 0"); /* set sectors to 0 */

  if (isConnected==0 && isOffline==0)
  {
    attacErrorMessage("No Game Selected. Click File->Connect. Then select a game. Then Work Offline or Connect");
    return (greatestAmountSectors);
  }

  buffer = (char*) malloc (50);


  ListView_DeleteAllItems (hShowEvilPairPort);
  //generate shortest path
	if (route_sector<=0 || route_sector>header.sector_cnt)
		shortestPath = (unsigned int*) ShortestPathToAll(currentSector);
	else
		shortestPath = (unsigned int*) ShortestPathToAll(route_sector);

  // generate evil pp information
  for (i = 0; i < header.sector_cnt; i++)
  {
    if (sectors[i].info != -1)
    {
      for (j = 0; j < 6; j++)
      {
        warp = sectors[i].warp_sect[j];
        if (warp == 0)
          break;
        warp--;
        if (warp > i && sectors[warp].info != -1)
        {
          for (k = 0; k < 6; k++)
          {
            if (sectors[warp].warp_sect[k] == 0)
            {
              break;
            }
            if (sectors[warp].warp_sect[k] - 1 == i)
	      {
		/* evil pair ports */
		if (((sectors[i].info == 2 || sectors[i].info == 3 || 
		      sectors[i].info == 4 || sectors[i].info == 8) &&
		     (sectors[warp].info == 2 || sectors[warp].info == 3 ||
		      sectors[warp].info == 4 || sectors[warp].info == 8)))
		  {
		    /* if sector is on bust list mark with asterik */
		    if (isBustedSector(i+1))
		      sprintf (buffer, "%ld*", i+1);
		    else
		      sprintf (buffer, "%ld", i+1);
		    tmp[0] = (char *) strdup(buffer);

		    if (isBustedSector(warp+1))
		      sprintf (buffer, "%ld*", warp+1);
		    else
		      sprintf (buffer, "%ld", warp+1);
		    tmp[1] = (char *) strdup(buffer);

		    /*
		      x = (a<b ? a : b);
		      y = (c<d ? c : d);
		      min = ( (a<b ? a : b) < (c<d ? c : d) ? (a<b ? a : b) : (c<d ? c : d));
		    */
		    min_amount = 
		      ( (sectors[i].port_amt[1] < sectors[i].port_amt[2] 
			 ? sectors[i].port_amt[1] : sectors[i].port_amt[2])
			< (sectors[warp].port_amt[1] < sectors[warp].port_amt[2] 
			   ? sectors[warp].port_amt[1] : sectors[warp].port_amt[2])
			? (sectors[i].port_amt[1] < sectors[i].port_amt[2] 
			   ? sectors[i].port_amt[1] : sectors[i].port_amt[2]) 
			: (sectors[warp].port_amt[1] < sectors[warp].port_amt[2] 
			   ? sectors[warp].port_amt[1] : sectors[warp].port_amt[2]));

		    if (min_amount==-1)
		      sprintf (buffer, "??");
		    else
		      sprintf (buffer, "%ld", min_amount);
		    tmp[2] = (char *) strdup(buffer);
		    
		    min_percent = 
		      ( (sectors[i].port_per[1] < sectors[i].port_per[2] 
			 ? sectors[i].port_per[1] : sectors[i].port_per[2])
			< (sectors[warp].port_per[1] < sectors[warp].port_per[2] 
			   ? sectors[warp].port_per[1] : sectors[warp].port_per[2]) 
			? (sectors[i].port_per[1] < sectors[i].port_per[2]
			   ? sectors[i].port_per[1] :  sectors[i].port_per[2])
			: (sectors[warp].port_per[1] < sectors[warp].port_per[2] 
			   ? sectors[warp].port_per[1] : sectors[warp].port_per[2]));

		    if (min_percent==-1)
		      sprintf (buffer, "??");
		    else
		      sprintf (buffer, "%ld%%", min_percent);
		    tmp[3] = (char *) strdup(buffer);

		    if (shortestPath[i]==255)
		      strcpy(buffer, "N/A");
		    else
		      sprintf (buffer, "%d", shortestPath[i]);
		    tmp[4] = (char *) strdup(buffer);
		    
		    /* set the best ports for rexx */
		    if (type==1)
		      {
				if (sectors[i].avoided==0 &&
					sectors[warp].avoided==0 &&
					min_percent > header.minPairPortPercent && 
					min_amount> greatestAmount &&
					((i+1) != header.stardock
					 && (i+1) != header.cls0port_alpha
					 && (i+1) != header.cls0port_rylos
					 && (i+1) != header.lastRobbedSector
					 && (warp+1) != header.lastRobbedSector) &&
					(sectors[i].lastBusted == -1 
					 && sectors[warp].lastBusted == -1))
				  {
					greatestAmount = min_amount;
					sprintf (buffer, "%ld %ld", i+1, warp+1);
					strcpy (greatestAmountSectors, buffer);
				  }
		      }

			//only add if it is greater than filter
		    if (((min_amount>= amount_filter) && (min_percent>=percent_filter)) ||
				(amount_filter==0 && percent_filter==0))
		      {
				listview_insert(hShowEvilPairPort, evil_pp_count++, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
				evilCount++;
		      }		    

			#ifdef P_FREE
				pfree(&tmp[0]);
				pfree(&tmp[1]);
				pfree(&tmp[2]);
				pfree(&tmp[3]);
				pfree(&tmp[4]);
			#else
				free(tmp[0]);
				free(tmp[1]);
				free(tmp[2]);
				free(tmp[3]);
				free(tmp[4]);
			#endif		
		  }
		break;
	      }
	  }
	}
      }
    }
    //check for registration or restrict report
    //we multiply by 3 since there are three reports
    if (isRegistered == 0 && (evilCount >= MAXCYCLES))
      {
		if (validRegCode==0 || isConnected==1)
		  break;
      }
  }

  sprintf (buffer, "Total: %d", evilCount);

  SetDlgItemText(GetParent(hShowEvilPairPort), IDC_SHOW_EVIL_PP_COUNT, buffer);
	
	#ifdef P_FREE
		pfree(&shortestPath);
		pfree(&buffer);
	#else
		free(shortestPath);
		free(buffer);
	#endif		
  return (greatestAmountSectors);
}

void showPortTypeData(HWND hPortType, int portType)
{
	long i;
	int portTypeCount=0;
	char *tmp[8];
	char *buffer;
	unsigned int *shortestPath;

	if (isConnected==0 && isOffline==0)
	{
		attacErrorMessage("No Game Selected. Click File->Connect. Then select a game. Then Work Offline or Connect");
		return;
	}

	buffer = (char*) malloc (50);

	//generate shortest path
	shortestPath = (unsigned int*) ShortestPathToAll(currentSector);

	ListView_DeleteAllItems (hPortType);

	// generate pp information
	for (i = 0; i < header.sector_cnt; i++)
	{
		if (sectors[i].info == portType)
		{
			/* if sector is on bust list mark with asterik */
			if (isBustedSector(i+1))
				sprintf (buffer, "%ld*", i+1);
			else
				sprintf (buffer, "%ld", i+1);
			tmp[0] = (char *) strdup(buffer);

			if (sectors[i].port_amt[0]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%ld", sectors[i].port_amt[0]);

			tmp[1] = (char *) strdup(buffer);		

			if (sectors[i].port_per[0]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%d%%", sectors[i].port_per[0]);
			tmp[2] = (char *) strdup(buffer);

			if (sectors[i].port_amt[1]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%ld", sectors[i].port_amt[1]);
			tmp[3] = (char *) strdup(buffer);		

			if (sectors[i].port_per[1]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%d%%", sectors[i].port_per[1]);
			tmp[4] = (char *) strdup(buffer);

			if (sectors[i].port_amt[2]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%ld", sectors[i].port_amt[2]);
			tmp[5] = (char *) strdup(buffer);		

			if (sectors[i].port_per[2]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%d%%", sectors[i].port_per[2]);
			tmp[6] = (char *) strdup(buffer);

			if (shortestPath[i]==255)
				strcpy(buffer, "N/A");
			else
				sprintf (buffer, "%d", shortestPath[i]);
			tmp[7] = (char *) strdup(buffer);

			listview_insert_porttype(hPortType, portTypeCount++, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7]);
			
			#ifdef P_FREE
				pfree(&tmp[0]);
				pfree(&tmp[1]);
				pfree(&tmp[2]);
				pfree(&tmp[3]);
				pfree(&tmp[4]);
				pfree(&tmp[5]);
				pfree(&tmp[6]);
				pfree(&tmp[7]);
			#else
				free(tmp[0]);
				free(tmp[1]);
				free(tmp[2]);
				free(tmp[3]);
				free(tmp[4]);
				free(tmp[5]);
				free(tmp[6]);
				free(tmp[7]);
			#endif		
		}

		//check for registration or restrict report
		//we multiply by 3 since there are three reports
		if (isRegistered == 0 && (portTypeCount > MAXCYCLES))
		{
			if (validRegCode==0 || isConnected==1)
				break;
		}
	}

	switch (portType)
	{
		case 1:
			sprintf (buffer, "There are %d BBS Ports (Class 1)", portTypeCount);
			SetDlgItemText(GetParent(hPortType), IDC_PORTTYPE_COUNT_STATIC, buffer);
		break;
		case 2:
			sprintf (buffer, "There are %d BSB Ports (Class 2)", portTypeCount);
			SetDlgItemText(GetParent(hPortType), IDC_PORTTYPE_COUNT_STATIC, buffer);
		break;
		case 3:
			sprintf (buffer, "There are %d SBB Ports (Class 3)", portTypeCount);
			SetDlgItemText(GetParent(hPortType), IDC_PORTTYPE_COUNT_STATIC, buffer);
		break;
		case 4:
			sprintf (buffer, "There are %d SSB Ports (Class 4)", portTypeCount);
			SetDlgItemText(GetParent(hPortType), IDC_PORTTYPE_COUNT_STATIC, buffer);
		break;
		case 5:
			sprintf (buffer, "There are %d SBS Ports (Class 5)", portTypeCount);
			SetDlgItemText(GetParent(hPortType), IDC_PORTTYPE_COUNT_STATIC, buffer);
		break;
		case 6:
			sprintf (buffer, "There are %d BSS Ports (Class 6)", portTypeCount);
			SetDlgItemText(GetParent(hPortType), IDC_PORTTYPE_COUNT_STATIC, buffer);
		break;
		case 7:
			sprintf (buffer, "There are %d SSS Ports (Class 7)", portTypeCount);
			SetDlgItemText(GetParent(hPortType), IDC_PORTTYPE_COUNT_STATIC, buffer);
		break;
		case 8:
			sprintf (buffer, "There are %d BBB Ports (Class 8)", portTypeCount);
			SetDlgItemText(GetParent(hPortType), IDC_PORTTYPE_COUNT_STATIC, buffer);
		break;
		default:
			sprintf (buffer, "There are %d Ports", portTypeCount);
			SetDlgItemText(GetParent(hPortType), IDC_PORTTYPE_COUNT_STATIC, buffer);
	}

	#ifdef P_FREE
		pfree(&shortestPath);
		pfree(&buffer);
	#else
		free(shortestPath);
		free(buffer);
	#endif	
}

void generate_maxed_ports(HANDLE hMaxPort, long maxAmount, int cargoType)
{
	long i;
	int portTypeCount=0;
	char *tmp[8];
	char *buffer;
	unsigned int *shortestPath;

	if (isConnected==0 && isOffline==0)
	{
		attacErrorMessage("No Game Selected. Click File->Connect. Then select a game. Then Work Offline or Connect");
		return;
	}

	buffer = (char*) malloc (50);

	//generate shortest path
	shortestPath = (unsigned int*) ShortestPathToAll(currentSector);

	ListView_DeleteAllItems (hMaxPort);

	// generate pp information
	for (i = 0; i < header.sector_cnt; i++)
	{   //hardcode for fuel ore
		if (sectors[i].port_amt[cargoType] >= maxAmount)
		{
			/* if sector is on bust list mark with asterik */
			if (isBustedSector(i+1))
				sprintf (buffer, "%ld*", i+1);
			else
				sprintf (buffer, "%ld", i+1);
			tmp[0] = (char *) strdup(buffer);

			if (sectors[i].port_amt[0]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%ld (%c)", sectors[i].port_amt[0], portSellsFuel(sectors[i].info)?'S':'B');

			tmp[1] = (char *) strdup(buffer);		

			if (sectors[i].port_per[0]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%d%%", sectors[i].port_per[0]);
			tmp[2] = (char *) strdup(buffer);

			if (sectors[i].port_amt[1]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%ld (%c)", sectors[i].port_amt[1], portSellsOrg(sectors[i].info)?'S':'B');
			tmp[3] = (char *) strdup(buffer);		

			if (sectors[i].port_per[1]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%d%%", sectors[i].port_per[1]);
			tmp[4] = (char *) strdup(buffer);

			if (sectors[i].port_amt[2]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%ld (%c)", sectors[i].port_amt[2], portSellsEquip(sectors[i].info)?'S':'B');
			tmp[5] = (char *) strdup(buffer);		

			if (sectors[i].port_per[2]==-1)
				sprintf (buffer, "??");
			else
				sprintf (buffer, "%d%%", sectors[i].port_per[2]);
			tmp[6] = (char *) strdup(buffer);

			if (shortestPath[i]==255)
				strcpy(buffer, "N/A");
			else
				sprintf (buffer, "%d", shortestPath[i]);
			tmp[7] = (char *) strdup(buffer);

			listview_insert_porttype(hMaxPort, portTypeCount++, tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7]);

			#ifdef P_FREE
				pfree(&tmp[0]);
				pfree(&tmp[1]);
				pfree(&tmp[2]);
				pfree(&tmp[3]);
				pfree(&tmp[4]);
				pfree(&tmp[5]);
				pfree(&tmp[6]);
				pfree(&tmp[7]);
			#else
				free(tmp[0]);
				free(tmp[1]);
				free(tmp[2]);
				free(tmp[3]);
				free(tmp[4]);
				free(tmp[5]);
				free(tmp[6]);
				free(tmp[7]);
			#endif		
		}

		//check for registration or restrict report
		//we multiply by 3 since there are three reports
		if (isRegistered == 0 && (portTypeCount > MAXCYCLES))
		{
			if (validRegCode==0 || isConnected==1)
				break;
		}
	}

	sprintf (buffer, "There are %d Ports", portTypeCount);
	SetDlgItemText(GetParent(hMaxPort), IDC_MAXPORT_COUNT_STATIC, buffer);

	#ifdef P_FREE
		pfree(&shortestPath);
		pfree(&buffer);
	#else
		free(shortestPath);
		free(buffer);
	#endif	
}

int isBustedSector (long theSector)
{
  if (sectors[theSector-1].lastBusted>0 && sectors[theSector-1].playerIdBusted==0)
    return 1;
  else
    return 0;
}

int isEnemy ()
{
	int figOwner=0;
	int corpNumber=0;
	int isEnemy=1;
	char buffer[10];
	int i;

	if (currentSector<=0 || currentSector > header.sector_cnt)
		return 0;
	else
	{
		figOwner = sectors[currentSector-1].ftr_owner;

		if (figOwner>CORP_CONSTANT)
		{
			corpNumber = figOwner-CORP_CONSTANT;
			for (i=0; i<MAX_PLAYERS; i++)
			{
				if (players[i].autonum >=0 && players[i].corpNumber==corpNumber)
				{
					if (players[i].alliance=='1')
						isEnemy=1;
					else 
						isEnemy=0;

					//break out if we see that one corp member is a ally
					if (isEnemy==0)
						i=MAX_PLAYERS;
				}
			}
		}
		else
		{
			if (figOwner<0)
				isEnemy=1;
			else if (players[figOwner].autonum >=0)
			{
				sprintf (buffer, "%c", players[figOwner].alliance);
				isEnemy = atoi(buffer);
			}
			else
				isEnemy=1;
		}

//		sprintf (data, "is enemy: %s", isEnemy==1?"Yes":"No");
//		outputData (data);
	}
	return isEnemy;
}

int isPlayerAnEnemy (int figOwner)
{
	int corpNumber=0;
	int isEnemy=1;
	char buffer[10];
	int i;

	if (figOwner>=0 && figOwner <32000)
	{
		if (figOwner>CORP_CONSTANT)
		{
			corpNumber = figOwner-CORP_CONSTANT;
			for (i=0; i<MAX_PLAYERS; i++)
			{
				if (players[i].autonum >=0 && players[i].corpNumber==corpNumber)
				{
					if (players[i].alliance=='1')
						isEnemy=1;
					else 
						isEnemy=0;

					//break out if we see that one corp member is a ally
					if (isEnemy==0)
						i=MAX_PLAYERS;
				}
			}
		}
		else
		{
			if (figOwner<0 || figOwner>=MAX_PLAYERS)
				isEnemy=1;
			else if (players[figOwner].autonum >=0)
			{
				sprintf (buffer, "%c", players[figOwner].alliance);
				isEnemy = atoi(buffer);
			}
			else
				isEnemy=1;
		}

//		sprintf (data, "is enemy: %s", isEnemy==1?"Yes":"No");
//		outputData (data);
	}
	else
		isEnemy=1;

	return isEnemy;
}


int portSellsFuel (int porttype)
{
	if (porttype==3 || porttype==4 || porttype==5 || porttype==7)
		return 1;
	else
		return 0;
}

int portSellsOrg (int porttype)
{
	if (porttype==2 || porttype==4 || porttype==6 || porttype==7)
		return 1;
	else
		return 0;
}


int portSellsEquip (int porttype)
{
	if (porttype==1 || porttype==5 || porttype==6 || porttype==7)
		return 1;
	else
		return 0;
}


int buyHaggle (long offer2, int haggle)
{

	long haggle106, haggle105, haggle104, haggle103, haggle102, haggle101, haggle100;
	int a, x, dead;
	char buffer[128];
	long timeout;
	int result;

	haggle106=offer2*94/100-1;
	haggle105=offer2*95/100-1;
	haggle104=offer2*96/100-1;
	haggle103=offer2*97/100-1;
	haggle102=offer2*98/100-1;
	haggle101=offer2*99/100-1;
	haggle100=offer2*(100/100);

	if (haggle==1)
	{
	  sprintf(buffer, "%ld", haggle100);
	  attac_send(buffer);
	  attac_send_char('\015');
	  a=1;
	}
	else
	{
	  sprintf(buffer, "%ld", haggle105);
	  attac_send(buffer);
	  attac_send_char('\015');
	  a=3 ;
	}

	while (a != 0)
	{
		timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "You have", "Your offer [", "We're not int", NULL);
		if (timeout == TIMEOUT_CONSTANT)
			return (640);
		result = StatusWaitThreadResult;
		if (result==1)
			x=2;
		else if (result==2)
			x=1;
		else if (result==3)
			x=3;

		if (x==1)
		{
			if (a==4)
			{
			  sprintf(buffer, "%ld", haggle105);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			if (a==3)       
			{
			  sprintf(buffer, "%ld", haggle104);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			if (a==2)   
			{
			  sprintf(buffer, "%ld", haggle103);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			if (a==1)      
			{
			  sprintf(buffer, "%ld", haggle102);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			if (a==0) 
			{
			  sprintf(buffer, "%ld", haggle101);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			a=a-1;
		}  

		if (x==2)
		{
			a=0;	
			dead=0;     
			return(0);
		}
	}
	return (0);
}



int sellHaggle (long offer1, int haggle)
{

	long haggle106, haggle105, haggle104, haggle103, haggle102, haggle101, haggle100;
	int a, x, dead;
	char buffer[128];
	long timeout;
	int result;

	haggle106=offer1*106/100-1;
	haggle105=offer1*105/100-1;
	haggle104=offer1*104/100-1;
	haggle103=offer1*103/100-1;
	haggle102=offer1*102/100-1;
	haggle101=offer1*101/100-1;
	haggle100=offer1*(100/100);

	if (haggle==1)
	{
	  sprintf(buffer, "%ld", haggle100);
	  attac_send(buffer);
	  attac_send_char('\015');
	  a=1;
	}
	else
	{
	  sprintf(buffer, "%ld", haggle105);
	  attac_send(buffer);
	  attac_send_char('\015');
	  a=3 ;
	}

	while (a != 0)
	{
		timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "You have", "Your offer [", "We're not int", NULL);
		if (timeout == TIMEOUT_CONSTANT)
			return (640);
		result = StatusWaitThreadResult;
		if (result==1)
			x=2;
		else if (result==2)
			x=1;
		else if (result==3)
			x=3;


		if (x==1)
		{
			if (a==4)
			{
			  sprintf(buffer, "%ld", haggle105);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			if (a==3)       
			{
			  sprintf(buffer, "%ld", haggle104);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			if (a==2)   
			{
			  sprintf(buffer, "%ld", haggle103);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			if (a==1)      
			{
			  sprintf(buffer, "%ld", haggle102);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			if (a==0) 
			{
			  sprintf(buffer, "%ld", haggle101);
			  attac_send(buffer);
			  attac_send_char('\015');
			}         
			a=a-1;
		}  

		if (x==2)
		{
			a=0;	
			dead=0;     
			return(0);
		}
	}
	return (0);
}



BOOL DisplayOS()
{
	SYSTEM_INFO siSysInfo;
	char buffer1[MAX_LENGTH];

	// Copy the hardware information to the SYSTEM_INFO structure. 

	GetSystemInfo(&siSysInfo); 

	// Display the contents of the SYSTEM_INFO structure. 

	addToMainWindow("", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("Hardware Summary", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("----------------", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);

	sprintf(buffer1, "OEM ID: %u", siSysInfo.dwOemId);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
	sprintf(buffer1, "Number of Processors: %u", siSysInfo.dwNumberOfProcessors);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	sprintf(buffer1, "Page Size: %u", siSysInfo.dwPageSize);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
	sprintf(buffer1, "Processor Type: %u", siSysInfo.dwProcessorType);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	sprintf(buffer1, "Minimum Application Address: %lx", siSysInfo.lpMinimumApplicationAddress);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
	sprintf(buffer1, "Maximum Application Address: %lx", siSysInfo.lpMaximumApplicationAddress);
	addToMainWindow(buffer1, ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	sprintf(buffer1, "Active Processor Mask: %u", siSysInfo.dwActiveProcessorMask);
	addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);

	return TRUE;
}


BOOL DisplaySystemVersion()
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	char buffer1[MAX_LENGTH];

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   // If that fails, try using the OSVERSIONINFO structure.
	
	addToMainWindow("", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("Operating System", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
	addToMainWindow("----------------", ATTAC_GREEN, ATTAC_BLACK, TRUE, 1);
   
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
		 return FALSE;
	}


   switch (osvi.dwPlatformId)
   {
      // Tests for Windows NT product family.
      case VER_PLATFORM_WIN32_NT:

      // Test for the product.
         if ( osvi.dwMajorVersion <= 4 )
			addToMainWindow("Microsoft Windows NT ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			addToMainWindow("Microsoft Windows 2000 ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);

         if( bOsVersionInfoEx )  // Use information from GetVersionEx.
         { 
         // Test for the workstation type.
/*fix
            if ( osvi.wProductType == VER_NT_WORKSTATION )
            {
               if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
				   addToMainWindow("Microsoft Windows XP ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);

               if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
				   addToMainWindow("Home Edition ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
               else
				   addToMainWindow("Professional ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
            }

         // Test for the server type.
            else if ( osvi.wProductType == VER_NT_SERVER )
            {
               if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
				   addToMainWindow("Microsoft Windows .NET ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);

               if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
				   addToMainWindow("DataCenter Server ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
               else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                  if( osvi.dwMajorVersion == 4 )
				   addToMainWindow("Advanced Server ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
                  else
				   addToMainWindow("Enterprise Server ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
               else
				   addToMainWindow("Server ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
            }
*/
         }

         else   // Use the registry on early versions of Windows NT.
         {
            HKEY hKey;
            char szProductType[BUFSIZE];
            DWORD dwBufLen=BUFSIZE;
            LONG lRet;

            lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
               0, KEY_QUERY_VALUE, &hKey );
            if( lRet != ERROR_SUCCESS )
                return FALSE;

            lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
               (LPBYTE) szProductType, &dwBufLen);
            if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
                return FALSE;

            RegCloseKey( hKey );

            if ( lstrcmpi( "WINNT", szProductType) == 0 )
				addToMainWindow("Professional ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
            if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
				addToMainWindow("Server ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
            if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
				addToMainWindow("Advanced Server ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
         }

      // Display version, service pack (if any), and build number.

         if ( osvi.dwMajorVersion <= 4 )
         {
            sprintf (buffer1, "version %ld.%ld %s (Build %ld)",
               osvi.dwMajorVersion,
               osvi.dwMinorVersion,
               osvi.szCSDVersion,
               osvi.dwBuildNumber & 0xFFFF);
			addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
         }
         else
         { 
            sprintf (buffer1, "%s (Build %ld)",
               osvi.szCSDVersion,
               osvi.dwBuildNumber & 0xFFFF);
			addToMainWindow(buffer1, ATTAC_CYAN, ATTAC_BLACK, TRUE, 1);
         }
         break;

      // Test for the Windows 95 product family. 
      case VER_PLATFORM_WIN32_WINDOWS:
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
         {
			 addToMainWindow("Microsoft Windows 95 ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
             if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				 addToMainWindow("OSR2 ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
         } 

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
         {
			 addToMainWindow("Microsoft Windows 98 ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
             if ( osvi.szCSDVersion[1] == 'A' )
				addToMainWindow("SE ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
         } 

         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
         {
			 addToMainWindow("Microsoft Windows Millennium Edition ", ATTAC_CYAN, ATTAC_BLACK, TRUE, 0);
         } 
         break;
   }
   return TRUE; 
}
