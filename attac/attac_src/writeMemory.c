// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <windows.h>
#include <sys/stat.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "attacGUI.h"
#include "util.h"
#include "writeMemory.h"
#include "linklist.h"
#include "rexx.h"

HANDLE WriteMemoryWaitEvent;
HANDLE MemoryData_Mutex;
int writeReady = 0;
int writePause = 0;

//globals for writing memory
TWX_FILE_HEADER header;
TWX_SECTOR *sectors;
TWX_PLAYER *players;
TWX_PLANET *planets;

char gameName[MAXAPPDIRLEN];
int restoreCount=0;


void writeSetReady()
{
	writeReady = 1;
}

void writeMemory()
{
	writeReady = 1;
	SetEvent (WriteMemoryWaitEvent);
}


 // handle of CWnd that called SetTimer
 // WM_TIMER
 // timer identification
 // system time
void CALLBACK writeMemoryTimerProc(	HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime )
{
	SetEvent (WriteMemoryWaitEvent);
}

void WriteMemoryThreadProc(void *dummy)
{
	writeReady = 0;
	writePause = 0;

	WriteMemoryWaitEvent = CreateEvent(NULL, 0, 0, NULL);
	while (1)
	{
		//reset the event state to non-signaled
		ResetEvent(WriteMemoryWaitEvent);

		WaitForSingleObject(WriteMemoryWaitEvent, INFINITE);

		if (writeReady)
			do_writeMemory();

		writeReady=0;
	}

	_endthread();
	return;
}

int do_writeMemory()
{
  FILE *fp;
  size_t write;
  long i=0;
  long j=0;
  long k=0;
  int tmpInt=0;
  char *tmpStr, *tp;
  char buffer[MAX_LENGTH], buffer1[20];
  long num_explored=0;
  float percent_explored = 0.0;
  char backupFilePath[MAX_LENGTH];
  struct stat buf;
  int existsFiles;
  long totalsize;

  TWX_FILE_HEADER tmp_header;
  TWX_SECTOR tmp_sectors;
  TWX_PLAYER tmp_players;
  TWX_PLANET tmp_planets;

  WaitForSingleObject(MemoryData_Mutex, INFINITE);

  for (i = 0; i < header.sector_cnt; i++)
    if (sectors[i].info != -1 && sectors[i].info != 11)
      num_explored++;

  percent_explored = (float) num_explored / (float) header.sector_cnt;

  sprintf(buffer1, "%0.2f", percent_explored * 100.0);
  if (strcmp(buffer1, "100.00") == 0 && num_explored != header.sector_cnt)
    sprintf(buffer1, "99.99");
  sprintf(buffer, "ATTAC Message Center - Total Sectors: %ld  -  Total Explored: %ld  -  Percent Explored: %s%%", header.sector_cnt, num_explored, buffer1);

  set_MessageWindow_title(buffer);

  memset (&tmp_header, 0, sizeof (TWX_FILE_HEADER));
  memset (&tmp_sectors, 0, sizeof (TWX_SECTOR));
  memset (&tmp_players, 0, sizeof (TWX_PLAYER));
  memset (&tmp_planets, 0, sizeof (TWX_PLANET));

  printf ("START WRITING FILE %s\n", gameName);

  strcpy (fullFilePath, appDir);
  strcat (fullFilePath, gameName);

  // open file
  fp = fopen(fullFilePath, "wb");
  if (fp == NULL)
  {
	if (restoreDBFromBackup()==0)
		attacErrorMessage ("Recovered corruped database");
	else
		attacErrorMessage ("Database corrupted. Please recreate game.");

	ReleaseMutex(MemoryData_Mutex);
    return 0;
  }
      
  strcpy (tmp_header.id, header.id);
  tmp_header.time_created = htonl(header.time_created);
  tmp_header.ver = htonl(header.ver);
  tmp_header.sector_cnt = htonl(header.sector_cnt);
  tmp_header.stardock = htonl(header.stardock);
  tmp_header.cls0port_sol = htonl(header.cls0port_sol);
  tmp_header.cls0port_alpha = htonl(header.cls0port_alpha);
  tmp_header.cls0port_rylos = htonl(header.cls0port_rylos);
  tmp_header.crc32 = htonl(header.crc32);
  tmp_header.player_cnt = htonl(header.player_cnt);
  tmp_header.planet_cnt = htonl(header.planet_cnt);

  //writing connection data
  strcpy(tmp_header.name, header.name);
  strcpy(tmp_header.address,header.address);
  tmp_header.port = htons(header.port);

  tmp_header.tollTolls = htons(header.tollTolls);
  tmp_header.defensiveTolls = htons(header.defensiveTolls);
  tmp_header.escapeSector = htonl(header.escapeSector);
  tmp_header.daysToClearBust = htons(header.daysToClearBust);
  tmp_header.lastBustedSector = htonl(header.lastBustedSector);
  tmp_header.lastRobbedSector = htonl(header.lastRobbedSector);
  strcpy(tmp_header.fedCommKey, header.fedCommKey);
  strcpy(tmp_header.validStardockKeys, header.validStardockKeys);
  tmp_header.lastZTMFromOneTillEnd = htonl(header.lastZTMFromOneTillEnd);
  tmp_header.lastZTMFromEndTillOne = htonl(header.lastZTMFromEndTillOne);
  tmp_header.lastTimeCIMRun = htonl(header.lastTimeCIMRun);
  
  tmp_header.autoUpdateMap = htons(header.autoUpdateMap);
  tmp_header.showPortOnMap = htons(header.showPortOnMap);
  tmp_header.stealFactor=htons(header.stealFactor);
  tmp_header.worstFactor=htons(header.worstFactor);
  tmp_header.selfDestructProtection=htons(header.selfDestructProtection);
  tmp_header.blindTwarpProtection=htons(header.blindTwarpProtection);
  tmp_header.cimUpdateSectorsDefault=htons(header.cimUpdateSectorsDefault);
  tmp_header.cimUpdatePortsDefault=htons(header.cimUpdatePortsDefault);
  tmp_header.updateInfoBarStartup=htons(header.updateInfoBarStartup);
  tmp_header.lastAnnouncementStartup=htonl(header.lastAnnouncementStartup);
  tmp_header.announcementStartup=htons(header.announcementStartup);

  tmp_header.detachWindows=htons(header.detachWindows);
  tmp_header.popupComms=htons(header.popupComms);
  tmp_header.autoHaggle=htons(header.autoHaggle);	
  tmp_header.forceNoHaggle=htons(header.forceNoHaggle);

  tmp_header.remoteConnectionPort=htonl(header.remoteConnectionPort);
  strcpy (tmp_header.remoteConnectionPassword, header.remoteConnectionPassword);
  tmp_header.minPairPortPercent=htons(header.minPairPortPercent);
  tmp_header.popupOnRexxCompletion=htons(header.popupOnRexxCompletion);
  tmp_header.lastUpdateTime=htonl(header.lastUpdateTime);
  tmp_header.lastSyncTimeWithServer = htonl(header.lastSyncTimeWithServer);
  strcpy(tmp_header.TWXSyncSession, header.TWXSyncSession);
  strcpy(tmp_header.TWXSyncPassword, header.TWXSyncPassword);
  strcpy(tmp_header.TWXSyncServer, header.TWXSyncServer);
  tmp_header.TWXSyncPort=htons(header.TWXSyncPort);
  
  tmp_header.terra = htonl(header.terra);
  strcpy(tmp_header.reserved, header.reserved);
  strcpy(tmp_header.reserved2, header.reserved2);

  // write twx header
  fseek(fp, 0, SEEK_SET);
  write = fwrite(&tmp_header, sizeof(TWX_FILE_HEADER), READ_WRITE_AMOUNT, fp);
      
  if (write < READ_WRITE_AMOUNT)
  {
	restoreDBFromBackup();
    fclose(fp);

	ReleaseMutex(MemoryData_Mutex);
    return 0;
  }

  // write sector info
  for (i = 0; i < header.sector_cnt; i++)
  {
    tmp_sectors.info = sectors[i].info;
    tmp_sectors.navhaz = sectors[i].navhaz;
    tmp_sectors.reserved2 = htons(sectors[i].reserved2);
    tmp_sectors.ftr_type = sectors[i].ftr_type;
    tmp_sectors.anom = sectors[i].anom;
    tmp_sectors.port_per[0] = sectors[i].port_per[0];
    tmp_sectors.port_per[1] = sectors[i].port_per[1];
    tmp_sectors.port_per[2] = sectors[i].port_per[2];
    tmp_sectors.warps = sectors[i].warps;

    tmp_sectors.sector_update = htonl(sectors[i].sector_update);
    tmp_sectors.ftrs = htonl(sectors[i].ftrs);
    tmp_sectors.port_amt[0] = htonl(sectors[i].port_amt[0]);
    tmp_sectors.port_amt[1] = htonl(sectors[i].port_amt[1]);
    tmp_sectors.port_amt[2] = htonl(sectors[i].port_amt[2]);
    tmp_sectors.warp_sect[0] = htonl(sectors[i].warp_sect[0]);
    tmp_sectors.warp_sect[1] = htonl(sectors[i].warp_sect[1]);
    tmp_sectors.warp_sect[2] = htonl(sectors[i].warp_sect[2]);
    tmp_sectors.warp_sect[3] = htonl(sectors[i].warp_sect[3]);
    tmp_sectors.warp_sect[4] = htonl(sectors[i].warp_sect[4]);
    tmp_sectors.warp_sect[5] = htonl(sectors[i].warp_sect[5]);
    tmp_sectors.port_update = htonl(sectors[i].port_update);
    tmp_sectors.density = htonl(sectors[i].density);
    tmp_sectors.lastBusted = htonl(sectors[i].lastBusted);
    tmp_sectors.playerIdBusted = htonl(sectors[i].playerIdBusted);

    tmp_sectors.ftr_owner = htons(sectors[i].ftr_owner);
    tmp_sectors.armids = htons(sectors[i].armids);
    tmp_sectors.armid_owner = htons(sectors[i].armid_owner);
    tmp_sectors.limpets = htons(sectors[i].limpets);
    tmp_sectors.limpet_owner = htons(sectors[i].limpet_owner);
    tmp_sectors.avoided = sectors[i].avoided;
	strcpy(tmp_sectors.reserved, sectors[i].reserved);

    write = fwrite(&tmp_sectors, sizeof(TWX_SECTOR), READ_WRITE_AMOUNT, fp);
    if (write < READ_WRITE_AMOUNT)
    {
		restoreDBFromBackup();

		fclose(fp);
		ReleaseMutex(MemoryData_Mutex);
		return 0;
    }
  }

  // write player info
  for (i = 0; i < header.player_cnt; i++)
  {
    tmp_players.autonum = htons(players[i].autonum);	  
    tmp_players.corpNumber = htons(players[i].corpNumber);
    tmp_players.alliance = players[i].alliance;
	tmp_players.reserved2 = players[i].reserved2;
    tmp_players.experience = htonl(players[i].experience);
    tmp_players.alignment = htonl(players[i].alignment);
    tmp_players.lastKnownSector = htonl(players[i].lastKnownSector);
    tmp_players.last_update = htonl(players[i].last_update);
    strcpy(tmp_players.name, players[i].name);
	strcpy(tmp_players.reserved, players[i].reserved);

    write = fwrite(&tmp_players, sizeof(TWX_PLAYER), READ_WRITE_AMOUNT, fp);
    if (write < READ_WRITE_AMOUNT)
    {

 	  restoreDBFromBackup();

	  fclose(fp);
  	  ReleaseMutex(MemoryData_Mutex);
      return 0;
    }

  }
      
  // we need to convert the link list structure we have in memory to the 
  // correct data structure to save to disk
	j=0;

	if (loadedPlanets==1)
	{
		for (i=0; i<header.sector_cnt; i++)
		{
			if (listPlanets[i] != NULL)
			{
				tmpInt = get_link_list_length(i);
				for (k=0; k < tmpInt; k++)
				{
					planets[j].sector = i+1;
					tmpStr = (char *) get_link_list_nth_data (i, k);
					tp  = left (tmpStr, 127);
					strcpy(planets[j].name, tp);
				  	#ifdef P_FREE
						pfree(&tmpStr);
						pfree(&tp);
					#else
						free(tmpStr);
						free(tp);
					#endif	
					printf ("planet %s in sector %ld\n", planets[j].name, i+1);
					j++;
				}
			}
		}
	}

  //clear out the remainder of planets in the old planet array
  for (i=j; i<MAX_PLANETS; i++)
  {
    planets[i].sector = -1;
    strcpy(planets[i].name,"");
    planets[i].autonum=-1;
  }

  // write planet info
  for (i = 0; i < header.planet_cnt; i++)
  {
    tmp_planets.autonum = htons(planets[i].autonum);
    tmp_planets.planetNumber = htons(planets[i].planetNumber);
    tmp_planets.createdBy = htons(planets[i].createdBy);
    tmp_planets.ownedBy = htons(planets[i].ownedBy);
    tmp_planets.citadel = planets[i].citadel;
    tmp_planets.citclass = planets[i].citclass;
    tmp_planets.mReaction = planets[i].mReaction;
    tmp_planets.iGenerator = planets[i].iGenerator;
    tmp_planets.transwarpPower = htons(planets[i].transwarpPower);
    tmp_planets.transportPower = htons(planets[i].transportPower);
    tmp_planets.qCannonPower = planets[i].qCannonPower;
    tmp_planets.qCannonAtmos = planets[i].qCannonAtmos;
    tmp_planets.qCannonSector = planets[i].qCannonSector;
    tmp_planets.reserved2 = planets[i].reserved2;
    tmp_planets.sector = htonl(planets[i].sector);
    tmp_planets.fighters = htonl(planets[i].fighters);
    tmp_planets.shields = htonl(planets[i].shields);
    tmp_planets.treasury = htonl(planets[i].treasury);
    tmp_planets.figsPerDay = htonl(planets[i].figsPerDay);
    tmp_planets.totalColonists = htonl(planets[i].totalColonists);
    tmp_planets.last_update = htonl(planets[i].last_update);
    strcpy(tmp_planets.name, planets[i].name);
    strcpy(tmp_planets.reserved, planets[i].reserved);

	  
    write = fwrite(&tmp_planets, sizeof(TWX_PLANET), READ_WRITE_AMOUNT, fp);
    if (write < READ_WRITE_AMOUNT)
    {
		restoreDBFromBackup();

		fclose(fp);

		ReleaseMutex(MemoryData_Mutex);

		return 0;
    }
  }
      
  fclose(fp);


  printf ("DONE WRITING FILE %s\n", gameName);

  strcpy (backupFilePath, fullFilePath);
  strcat (backupFilePath, ".bak");
	
  existsFiles = stat(fullFilePath, &buf);
  totalsize = sizeof(TWX_FILE_HEADER) + (sizeof(TWX_SECTOR)*header.sector_cnt) + (sizeof(TWX_PLAYER)*MAX_PLAYERS) +  (sizeof(TWX_PLANET)*MAX_PLANETS);

  if (totalsize == buf.st_size)
  {
	  //a good file. back it up
	  if (CopyFile (fullFilePath, backupFilePath, FALSE)) 
		  printf ("Backing up %s to %s\n", fullFilePath, backupFilePath);
	  else 
		  fprintf (stderr,"Cannot backup %s\n", fullFilePath);
  }
  else
  {
	  attacErrorMessage ("A file error has occured. The backup file which was save about 5 mintues ago has been restored");
	  //restore the backup since the size of the original file is incorrect
	  if (CopyFile (backupFilePath, fullFilePath, FALSE)) 
		  printf ("Restoring backup file %s to %s\n", backupFilePath, fullFilePath);
	  else 
		  fprintf (stderr,"Cannot restore %s\n", backupFilePath);
  }

  ReleaseMutex(MemoryData_Mutex);
  return 1;
}

int restoreDBFromBackup ()
{
	char backupFilePath[MAX_LENGTH];
	char buffer[256];

	strcpy (backupFilePath, fullFilePath);
	strcat (backupFilePath, ".bak");

	if (CopyFile (backupFilePath, fullFilePath, FALSE) && restoreCount < MAX_RESTORE_ATTEMPTS) 
	{
		printf ("Restoring %s to %s\n", fullFilePath, backupFilePath);
		restoreCount++;
		return 0;
  	} 
	else 
	{
		restoreCount=0;
		sprintf (buffer, "Found Corrupted Database. Unable to restore after %d attempts. Please delete and recreate this game even though you might make a successful connection.", MAX_RESTORE_ATTEMPTS);
        MessageBox(NULL, buffer, "Database Load Error", MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

}
