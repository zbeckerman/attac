// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <stdarg.h>
#include <commctrl.h>
#include <commdlg.h>
#include <ctype.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "attacGUI.h"
#include "cim.h"
#include "win_res.h"
#include "util.h"
#include "rexx.h"
#include "parsedatathread.h"
#include "statusthread.h"
#include "encrypt.h"
#include "queue.h"
#include "ztm.h"
#include "info.h"
#include "structs.h"
#include "remoteConnection.h"
#include "twxSync.h"
#include "../PUTTY.H"
#include "../LDISC.H"

#include "../../TWCommon/TWCommon.h"

char theUserId[100];
int validTWXSyncRegCode=0;  //if code is valid
int validRegCode=0;  //if code is valid
int isRegistered=0;  //if user ids match. only check if validRegCode=1

// for player data
//hash_table hTable;
int hashInUse=0;
int recordFastMacro=-1;
char fastMacroString[FASTMACROLEN];
int gettingTWXData=0;
int runningTWXScript=0;
int passToREXX=1;

HANDLE RemoveCRsMutex;
HANDLE PFreeMutex;
HANDLE CleanLineMutex;
HANDLE UnCommaMutex;
HANDLE SyncDataMutex;
HANDLE MessageMutex;

HANDLE Data_Mutex;
HANDLE Wait_Event;
long Still_Waiting=0;
char DSM_Buffer[MAX_LENGTH];
char DSM_BufferParseData[MAX_LENGTH];
char DSM_BufferStatus[MAX_LENGTH];
int countParens=0;
int CIMParseType=0;

long numFigs=0;
long numShields=0;
long numTurns=0;
long numExp=0;
long numAlign=0;
long numCredits=0;
long numFuel=0;
long numOrgs=0;
long numEquip=0;
long numColos=0;
long numEmptyHolds=0;
long numTotalHolds=0;
long numSector=0;
long numCloaks=0;
long numPhotons=0;
long numProbes=0;
long numAMines=0;
long numLMines=0;

int sendingCommMessage=0;
int Keyboard_Locked=0;
char traderTitle[TOTAL_TRADER_TITLES][24];
int buyingSelling;
int buyingSellingAmount;
int validDoubleY;
int commandPrompt;  

/*1 is command, 2 is computer, 3 is corp
			4 is planet, 5 is citadel
			6 is on main stardock prompt
			 7 is anywhere else on stardock
		      */

void resetGame(HWND tHwnd) {
	if ((MessageBox(tHwnd, "Do you really want to reset this game? All game data will be reset", "Game Reset Confirmation", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2))==IDYES) {
		
		strcpy (fullFilePath, appDir);
		strcat (fullFilePath, gameName);

		if (create_new_twx(fullFilePath)==ERROR_FILE_ACCESS)
		{
			MessageBox(tHwnd, "The Database file was not found.", "Game File Not Found", MB_OK);
		}

		clearBustsFunc();

		writeSetReady();

		attacInfoMessage("Game Data has been reset");
	} else {
		attacInfoMessage("No changes have been made");
	}

}
void attac_send (char message[])
{
	int i;
	char *new_message;

	new_message = Translate_String(message, 0, 0);
	for (i = 0; i < (int) strlen(new_message); i++)
	{
		ldisc_send (ldisc_extern, &new_message[i], 1, 1);
	}
	pfree(&new_message);
}


void attac_send_char (char message)
{
	int i;
	char *new_message;
	char buffer[20];

	sprintf (buffer, "%c", message);

	new_message = Translate_String(buffer, 0, 0);
	for (i = 0; i < (int) strlen(new_message); i++)
	{
		ldisc_send (ldisc_extern, &new_message[i], 1, 1);
	}
	pfree(&new_message);
}

void test_debug(void)
{
	unsigned char ch;
	FILE *fp;

	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, "test.log");

	fp = fopen (fullFilePath, "r");
	ch = fgetc( fp );
	while ((char) ch != EOF)
	{	
		DSM_ProcessInbound ((unsigned char)ch, (int) LGTYP_ATTAC);
		ch = fgetc( fp );
	}
	fclose(fp);

}

void DSM_ProcessInbound (unsigned char c, int logmode)
{
	unsigned char orig_inchar;
	char *tmp;
	char buffer[50],  s1[10], s2[10], tChar[1];
	long i, j, firstSectorParse=0;
	long parseLen=0;
	long statusLen=0;
	long waitLen=0;
	HANDLE hProgress;
	long rtmp1, rtmp2;
	char *dataMessage;
	
	{
		//fix zev
//		FILE *fp;
//		fp = fopen ("attacOutputDSM.txt", "a+");
//		fprintf (fp, "%c - %d\n", c, logmode);
//		fclose(fp);

	}

	if (logmode==LGTYP_ATTAC)
	{

		WaitForSingleObject(Data_Mutex, INFINITE);

		orig_inchar = c;
		c = toupper(c);



//		printf ("char: %c - %d\n", orig_inchar, orig_inchar);
		sprintf (tChar, "%c", (unsigned char) orig_inchar);
		strcat (DSM_Buffer, tChar);
		if (strlen (DSM_Buffer) > (MAX_LENGTH-10))
		{
			tmp = right (DSM_Buffer, 300);
			strcpy (DSM_Buffer, tmp);
			pfree(&tmp);
		}

		if (parseCIMData==0 && parseZTMData==0 && helperActive==1 && gettingTWXData==0)   
		{
			//START PARSE DATA
//			printf ("c: %c - %d\n", orig_inchar, orig_inchar);
			if (startParsing==1)	//only parse if we are in tradewars
			{
				sprintf (tChar, "%c", (unsigned char) orig_inchar);
				strcat (DSM_BufferParseData, tChar);
				if (strlen (DSM_BufferParseData) > (MAX_LENGTH-10))
				{
					tmp = right (DSM_BufferParseData, 300);
					strcpy (DSM_BufferParseData, tmp);
					pfree(&tmp);
				}

				parseLen = strlen (DSM_BufferParseData);
//				printf ("parseLen: %ld\n", parseLen);
				//here we are parsing the parseDataThread and seeing if we want to set the event for it
				for (i=0; i<parseDataWaitCount; i++)
				{
					waitLen = strlen(parseDataWaitArray[i]);
					if (waitLen>0 && parseLen >= waitLen && DSM_BufferParseData[parseLen-1]==parseDataWaitArray[i][waitLen-1])
					{
						tmp = right (DSM_BufferParseData, waitLen);
//						printf ("%ld: %s - %s\n", i, tmp, parseDataWaitArray[i]);
						if (strcmp(tmp, parseDataWaitArray[i]) ==0) 
						{
							parseDataWaitThreadResult=i+1;

							Update_LastLine_ParseData(parseLen);
							i = parseDataWaitCount;

							//blank the string out. we wouldnt want to find it in the buffer by accident
							for (j=0; j<parseDataWaitCount; j++)
								strcpy (parseDataWaitArray[j],"***abcdefg***");

							parseDataWaitCount=0;

							SetEvent(ParseDataWaitEvent);
					//		ResetEvent(parseDataResettingMutex);
					//		WaitForSingleObject(parseDataResettingMutex, utilParserTimeout);
						}
						pfree(&tmp);
					}
				}

			}
			//END PARSE DATA

			//START STATUS
			sprintf (tChar, "%c", (unsigned char) orig_inchar);
			strcat (DSM_BufferStatus, tChar);
			if (strlen (DSM_BufferStatus) > (MAX_LENGTH-10))
			{
				tmp = right (DSM_BufferStatus, 300);
				strcpy (DSM_BufferStatus, tmp);
				pfree(&tmp);
			}

			statusLen = strlen (DSM_BufferStatus);
			//here we are parsing the StatusThread and seeing if we want to set the event for it
			for (i=0; i<StatusWaitCount; i++)
			{
				waitLen = strlen(StatusWaitArray[i]);
				if (waitLen>0 && statusLen >= waitLen && DSM_BufferStatus[statusLen-1]==StatusWaitArray[i][waitLen-1])
				{
					tmp = right (DSM_BufferStatus, waitLen);

//					printf ("%s --- %s\n", tmp, StatusWaitArray[i]);
					if (strcmp(tmp, StatusWaitArray[i]) ==0) 
					{
						StatusWaitThreadResult=i+1;

						Update_LastLine_Status(statusLen);
						i = StatusWaitCount;

						//blank the string out. we wouldnt want to find it in the buffer by accident
						for (j=0; j<StatusWaitCount; j++)
							strcpy (StatusWaitArray[j],"***abcdefg***");

						StatusWaitCount=0;

						SetEvent(StatusWaitEvent);
					}
					pfree(&tmp);
				}
			}

			//END STATUS
		}

		if (parseCIMData==1) 
		{
			if (Instr (1, DSM_Buffer, "\n") || Instr (1, DSM_Buffer, ": "))
			{
				if (Instr (1, DSM_Buffer, "Computer"))
				{	
					countParens=0;
				}
				else if (Instr (1, DSM_Buffer, ":"))
				{
					countParens++;

					if (GetCIMData==3 && countParens==2)
					{
						attac_send_char('r');
						CIMParseType = 2;
					}
					else if (GetCIMData==3 && countParens>2)
					{
						parseCIMData=0;
						attac_send("qq");

						//start TWXSync
						buildSendTWXData_WaitForData(INFINITE);
						dataMessage = sendAllNewTWXSyncData();
						postSendMessageToTWXSync(threadId, strdup(dataMessage));
						pfree(&dataMessage);
						buildSendTWXData_ReleaseData();
						//end TWXSync

						writeMemory();
					}
					else if (GetCIMData<3 && countParens>1)
					{
						parseCIMData=0;
						attac_send("qq");
						
						//start TWXSync
						buildSendTWXData_WaitForData(INFINITE);
						dataMessage = sendAllNewTWXSyncData();
						postSendMessageToTWXSync(threadId, strdup(dataMessage));
						pfree(&dataMessage);
						buildSendTWXData_ReleaseData();
						//end TWXSync

						writeMemory();
					}

				}
				else if (isalpha(DSM_Buffer[0]))
					;
				else if (DSM_Buffer[0]==13)
					;
				else
				{	
					tmp = (char *) strdup (DSM_Buffer);
					parseCIMInformation(CIMParseType, tmp);
					pfree(&tmp);
				}
				strcpy(DSM_Buffer,"");
			}
		}
		if (parseZTMData==1) 
		{
			
			if (c=='S')
			{
				if (Instr(1, DSM_Buffer, "Clear Avoids"))
				{
					attac_send_char('n');
					strcpy(DSM_Buffer,"");
				}
			}
			else if (c==':')
			{
				if (forwardZTM==1 && header.lastZTMFromOneTillEnd <= header.sector_cnt)
				{
					currentZTM=1;
				}
				else if (backwardZTM==1 && header.lastZTMFromEndTillOne <= header.sector_cnt)
				{
					currentZTM=2;
				}
				else if (oneWayZTM==1 && oneWayWarpCount>0)
				{
					currentZTM=3;
				}
				else if (randomZTM==1)
				{
					currentZTM=4;
				}
				else
					currentZTM=0;

				if (Instr(1, DSM_Buffer, "Help")==0)
				{
					parseZTMBufferData (DSM_Buffer);
				}

				strcpy(DSM_Buffer,"");

				hProgress = GetDlgItem(showztm, IDC_ZTM_PROGRESS);

				if (currentZTM==1)
				{
					attac_send_char('f');
					sprintf (buffer, "%ld", header.lastZTMFromOneTillEnd);
					attac_send(buffer);
					attac_send_char('\015');
					sprintf (buffer, "%ld", (header.sector_cnt+1)-header.lastZTMFromOneTillEnd);
					attac_send(buffer);
					attac_send_char('\015');
					header.lastZTMFromOneTillEnd++;

					//advance progress bar
					setProgessBar ((header.lastZTMFromOneTillEnd*100)/header.sector_cnt, hProgress);
				}
				else if (currentZTM==2)
				{
					if (header.lastZTMFromEndTillOne % 2 != 0)
					{
						attac_send_char('f');
						sprintf (buffer, "%ld", header.lastZTMFromEndTillOne);
						attac_send(buffer);
						attac_send_char('\015');
						sprintf (buffer, "%ld", header.lastZTMFromEndTillOne+1);
						attac_send(buffer);
						attac_send_char('\015');
					}
					else
					{
						attac_send_char('f');
						sprintf (buffer, "%ld", header.lastZTMFromEndTillOne);
						attac_send(buffer);
						attac_send_char('\015');
						sprintf (buffer, "%ld", header.lastZTMFromEndTillOne-1);
						attac_send(buffer);
						attac_send_char('\015');
					}
					header.lastZTMFromEndTillOne++;

					//advance progress bar
					setProgessBar ((header.lastZTMFromEndTillOne*100)/header.sector_cnt, hProgress);
				}
				else if (currentZTM==3)
				{
					firstSectorParse=0;
					j=0;
					for (i=0; i< (int) strlen (oneWayWarps[oneWayWarpCount-1]); i++)
					{
						if (oneWayWarps[oneWayWarpCount-1][i]==':')
						{
							firstSectorParse++;
							s1[j]=0;
							j=0;
						}
						else if (firstSectorParse==0)
							s1[j++] = oneWayWarps[oneWayWarpCount-1][i];
						else if (firstSectorParse==1)
							s2[j++] = oneWayWarps[oneWayWarpCount-1][i];;
					}
					s2[j]=0;

					attac_send_char('f');
					attac_send(s1);
					attac_send_char('\015');
					attac_send(s2);
					attac_send_char('\015');
					oneWayWarpCount--;

					//advance progress bar
					setProgessBar (((totalOneWayWarpCount - oneWayWarpCount)*100)/totalOneWayWarpCount, hProgress);
				}
				else if (currentZTM==4)
				{
					attac_send_char('f');
					rtmp1  = (rand()%(header.sector_cnt-1))+1;
					sprintf (buffer, "%ld", rtmp1);
					attac_send(buffer);
					attac_send_char('\015');
					rtmp2 = 0;
					while ((rtmp1  == rtmp2) || rtmp2==0)
						rtmp2  = (rand()%(header.sector_cnt-1))+1;
					sprintf (buffer, "%ld", rtmp2);
					attac_send(buffer);
					attac_send_char('\015');
				}
				else
				{
						parseZTMData=0;
						attac_send("qq");

						//start TWXSync
						buildSendTWXData_WaitForData(INFINITE);
						dataMessage = sendAllNewTWXSyncData();
						postSendMessageToTWXSync(threadId, strdup(dataMessage));
						pfree(&dataMessage);
						buildSendTWXData_ReleaseData();
						//end TWXSync

						writeMemory();
				}
				writeSetReady();
			}
		}
		else if (parseZTMData==2) 
		{

			if (c=='?')
			{
				strcpy(DSM_Buffer,"");
				parseZTMData=1;		//we are now in interrogation mode. so start ztm
			}
		}

		ReleaseMutex(Data_Mutex);
	}
}

void setProgessBar (int newPosition, HANDLE progress)
{
	int lResult;

	lResult = SendMessage(progress, PBM_GETPOS, 0, 0);

	while (lResult < newPosition)
	{
		SendMessage(progress, PBM_STEPIT, 0, 0); 
		lResult = SendMessage(progress, PBM_GETPOS, 0, 0);
	}
}


void parseCIMInformation(int type, char *line)
{
	char *token;
	char buffer[256], buf1[256], buf2[256], tmp1, tmp2;
	int pos, warps=0, i=0, j=0, count=0;
    long cim_sectors[7];
	long sector;
	long trading[3];
	short percent[3];
	short buying[3];	//	1 = YES , 0 = NO
	int underConstruction=0;
	int isNew=0;

	if (type==1)
	{
		pos=0;
		for (i=0; i<7; i++)
			cim_sectors[i]=0;

		token = strtok(line, "\n ");
		while (token != NULL)
		{
			cim_sectors[pos] = atol(token);
			token = strtok(NULL, "\n ");
			pos++;
		}

		warps = pos - 1;

		while (pos != 7)
		{
			cim_sectors[pos] = 0;
			pos++;
		}

		if (cim_sectors[0] >19000)
		{
			warps=pos-1;
		}
		if (cim_sectors[0] <= 0 ||  cim_sectors[0]>header.sector_cnt)
		{
//			printf ("sector '%ld'\n", cim_sectors[0]);
			attacErrorMessage ("Error Running CIM due to universive size inconsistency. Check universize size to make sure it is correct");
			return;
		}

		//check if any of the sectors are new. if any are, update the timestamp
		//otherwise, we dont bother since nothing changed.
		isNew=0;
		if (!isNew)
			isNew = sectorExistsInDatabase(cim_sectors[1], cim_sectors[0]);
		if (!isNew)
			isNew = sectorExistsInDatabase(cim_sectors[2], cim_sectors[0]);
		if (!isNew)
			isNew = sectorExistsInDatabase(cim_sectors[3], cim_sectors[0]);
		if (!isNew)
			isNew = sectorExistsInDatabase(cim_sectors[4], cim_sectors[0]);
		if (!isNew)
			isNew = sectorExistsInDatabase(cim_sectors[5], cim_sectors[0]);
		if (!isNew)
			isNew = sectorExistsInDatabase(cim_sectors[6], cim_sectors[0]);
		sectors[cim_sectors[0] - 1].warp_sect[0] = cim_sectors[1];
		sectors[cim_sectors[0] - 1].warp_sect[1] = cim_sectors[2];
		sectors[cim_sectors[0] - 1].warp_sect[2] = cim_sectors[3];
		sectors[cim_sectors[0] - 1].warp_sect[3] = cim_sectors[4];
		sectors[cim_sectors[0] - 1].warp_sect[4] = cim_sectors[5];
		sectors[cim_sectors[0] - 1].warp_sect[5] = cim_sectors[6];
		sectors[cim_sectors[0] - 1].warps = warps;
		if (isNew)
			sectors[cim_sectors[0] - 1].sector_update = header.lastTimeCIMRun;

      
		  //if this is a new sectors. set it to no port. the cim port report will
		  //add a port if there is one

		if (sectors[cim_sectors[0] - 1].info == -1) {
			sectors[cim_sectors[0] - 1].info = 10;
			sectors[cim_sectors[0] - 1].sector_update = header.lastTimeCIMRun;
		}
	}
	else if (type==2)
	{
		pos=0;
		for (i=0; i<3; i++)
		{
			trading[i]=0;
			percent[i]=0;
			buying[i]=0;
		}

		token = strtok(line, " ");
		if (token != NULL)
			sector = atol(token);

		while (token != NULL)
		{
			j=0;
			count=0;
			tmp1=0;
			tmp2=0;
			token = strtok(NULL, "%");
			if (token != NULL && pos<3)
			{
				if (Instr(1, token, "-"))
					buying[pos]=1;
				else
					buying[pos]=0;
				strcpy (buffer, token);
				buffer[strlen(token)]='\x0';
				strcpy(buf1, "");
				strcpy(buf2, "");

				for (i=0; i< (int)strlen(buffer); i++)
				{
					if (isdigit(buffer[i]))
					{
						if (count==0)
						{
							buf1[j++] = buffer[i];
							tmp1++;
						}
						else
						{
							buf2[j++] = buffer[i];
							tmp2++;
						}
					}
					else if (buffer[i]==32 && strlen (buf1)>0)
					{
						count++;
						j=0;
					}
				}
				buf1[(int)tmp1]=(char)NULL;
				buf2[(int)tmp2]=(char)NULL;

				trading[pos] = atol(buf1);
				percent[pos] = atoi(buf2);
				pos++;
			}
		}

		if (sector <= 0 || sector>header.sector_cnt)
		{
//			printf ("sector1 '%ld'\n", cim_sectors[0]);
			attacErrorMessage ("Error Running CIM due to universive size inconsistency. Check universize size to make sure it is correct");
			return;
		}

		sectors[sector-1].port_update=header.lastTimeCIMRun;
		sectors[sector-1].port_amt[0]=(long)trading[0];
		sectors[sector-1].port_amt[1]=(long)trading[1];
		sectors[sector-1].port_amt[2]=(long)trading[2];

		sectors[sector-1].port_per[0]=(char)((short)percent[0]);
		sectors[sector-1].port_per[1]=(char)((short)percent[1]);
		sectors[sector-1].port_per[2]=(char)((short)percent[2]);


		if (trading[0]==0 && trading[1]==0 && trading[2]==0 && percent[0]==0 && percent[1]==0 && percent[2]==0)
			underConstruction=16;
		else
			underConstruction=0;

		if (buying[0]==1 && buying[1]==1 && buying[2]==0)
			sectors[sector-1].info=(1+underConstruction);
		else if (buying[0]==1 && buying[1]==0 && buying[2]==1)
			sectors[sector-1].info=(2+underConstruction);
		else if (buying[0]==0 && buying[1]==1 && buying[2]==1)
			sectors[sector-1].info=(3+underConstruction);
		else if (buying[0]==0 && buying[1]==0 && buying[2]==1)
			sectors[sector-1].info=(4+underConstruction);
		else if (buying[0]==0 && buying[1]==1 && buying[2]==0)
			sectors[sector-1].info=(5+underConstruction);
		else if (buying[0]==1 && buying[1]==0 && buying[2]==0)
			sectors[sector-1].info=(6+underConstruction);
		else if (buying[0]==0 && buying[1]==0 && buying[2]==0)
			sectors[sector-1].info=(7+underConstruction);
		else if (buying[0]==1 && buying[1]==1 && buying[2]==1)
			sectors[sector-1].info=(8+underConstruction);
		else
		{
			if (sectors[sector-1].info == -1)
				sectors[sector-1].info=11;
		}

	}
}




void exportAllCIMData_Text(int sector, int port)
{
  FILE *fp;
  long i;
  char fuel  = ' ';
  char org   = ' ';
  char equip = ' ';

  if (sector==1 && port==1)
  {
	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, "cim.txt");
    fp = fopen (fullFilePath, "w");
  }
  else if (sector==1 && port==0)
  {
	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, "sector.txt");
    fp = fopen (fullFilePath, "w");
  }
  else if (sector==0 && port==1)
  {
	strcpy (fullFilePath, appDir);
	strcat (fullFilePath, "port.txt");
    fp = fopen (fullFilePath, "w");
  }
  else
    return;
    
  fprintf(fp, ": \n");
  if (sector==1)
    {
      for (i=0; i<header.sector_cnt; i++)
	{
	  if (header.sector_cnt<10000)
	    {
	      if (sectors[i].info != -1)
		{
		  if (sectors[i].warp_sect[0]<=0 && sectors[i].warp_sect[1]<=0)
		    {
		      //		      fprintf(fp, "%4d %4d\n",  i+1, sectors[i].warp_sect[0]); 
		    }
		  else if (sectors[i].warp_sect[1]<=0)
		    {
		      fprintf(fp, "%4ld %4ld\n",  i+1, 
			      sectors[i].warp_sect[0] );
		    }
		  else if (sectors[i].warp_sect[2]<=0)
		    {
		      fprintf(fp, "%4ld %4ld %4ld\n",  i+1, 
			      sectors[i].warp_sect[0],
			      sectors[i].warp_sect[1] );
		    }
		  else if (sectors[i].warp_sect[3]<=0)
		    {
		      fprintf(fp, "%4ld %4ld %4ld %4ld\n",  i+1, 
			      sectors[i].warp_sect[0],
			      sectors[i].warp_sect[1],
			      sectors[i].warp_sect[2] );
		    }
		  else if (sectors[i].warp_sect[4]<=0)
		    {
		      fprintf(fp, "%4ld %4ld %4ld %4ld %4ld\n",  i+1, 
			      sectors[i].warp_sect[0],
			      sectors[i].warp_sect[1],
			      sectors[i].warp_sect[2],
			      sectors[i].warp_sect[3] );
		    }
		  else if (sectors[i].warp_sect[5]<=0)
		    {
		      fprintf(fp, "%4ld %4ld %4ld %4ld %4ld %4ld\n", 
			      i+1, 
			      sectors[i].warp_sect[0], 
			      sectors[i].warp_sect[1],
			      sectors[i].warp_sect[2],
			      sectors[i].warp_sect[3],
			      sectors[i].warp_sect[4]);
		    }
		  else 
		    {
		      fprintf(fp, "%4ld %4ld %4ld %4ld %4ld %4ld %4ld\n", 
			      i+1, 
			      sectors[i].warp_sect[0], 
			      sectors[i].warp_sect[1],
			      sectors[i].warp_sect[2],
			      sectors[i].warp_sect[3],
			      sectors[i].warp_sect[4],
			      sectors[i].warp_sect[5]);
		    }
		}
	    }
	  else
	    {
	      if (sectors[i].info != -1)
		{
		  if (sectors[i].warp_sect[0]<=0 && sectors[i].warp_sect[1]<=0)
		    {
		      fprintf(fp, "%5ld %5ld\n",  i+1, sectors[i].warp_sect[0]); 
		    }
		  else if (sectors[i].warp_sect[2]<=0)
		    {
		      fprintf(fp, "%5ld %5ld %5ld\n",  i+1, 
			      sectors[i].warp_sect[0],
			      sectors[i].warp_sect[1] );
		    }
		  else if (sectors[i].warp_sect[3]<=0)
		    {
		      fprintf(fp, "%5ld %5ld %5ld %5ld\n",  i+1, 
			      sectors[i].warp_sect[0],
			      sectors[i].warp_sect[1],
			      sectors[i].warp_sect[2] );
		    }
		  else if (sectors[i].warp_sect[4]<=0)
		    {
		      fprintf(fp, "%5ld %5ld %5ld %5ld %5ld\n",  i+1, 
			      sectors[i].warp_sect[0],
			      sectors[i].warp_sect[1],
			      sectors[i].warp_sect[2],
			      sectors[i].warp_sect[3] );
		    }
		  else if (sectors[i].warp_sect[5]<=0)
		    {
		      fprintf(fp, "%5ld %5ld %5ld %5ld %5ld %5ld\n", 
			      i+1, 
			      sectors[i].warp_sect[0], 
			      sectors[i].warp_sect[1],
			      sectors[i].warp_sect[2],
			      sectors[i].warp_sect[3],
			      sectors[i].warp_sect[4]);
		    }
		  else 
		    {
		      fprintf(fp, "%5ld %5ld %5ld %5ld %5ld %5ld %5ld\n", 
			      i+1, 
			      sectors[i].warp_sect[0], 
			      sectors[i].warp_sect[1],
			      sectors[i].warp_sect[2],
			      sectors[i].warp_sect[3],
			      sectors[i].warp_sect[4],
			      sectors[i].warp_sect[5]);
		    }
		}
	      
	      
	    }
	}
    }


	if (sector==1 && port==1)
	{
		fprintf(fp, "\n: \n");
	}

	if (port==1)
	{
		for (i=0; i<header.sector_cnt; i++)
		{
			if (sectors[i].info >=1 && sectors[i].info <=8 )
			{
				if (sectors[i].info==1)
				{
					fuel='-'; org='-'; equip=' ';
				}
				else if (sectors[i].info==2)
				{
					fuel='-'; org=' '; equip='-';
				}
				else if (sectors[i].info==3)
				{
					fuel=' '; org='-'; equip='-';
				}
				else if (sectors[i].info==4)
				{
					fuel=' '; org=' '; equip='-';
				}
				else if (sectors[i].info==5)
				{
					fuel=' '; org='-'; equip=' ';
				}
				else if (sectors[i].info==6)
				{
					fuel='-'; org=' '; equip=' ';
				}
				else if (sectors[i].info==7)
				{
					fuel=' '; org=' '; equip=' ';
				}
				else if (sectors[i].info==8)
				{
					fuel='-'; org='-'; equip='-';
				}
				if (header.sector_cnt<10000)
				{
					fprintf(fp, "%4ld %c%5ld %3d%% %c%5ld %3d%% %c%5ld %3d%%\n",
					i+1,
					fuel, 
					sectors[i].port_amt[0],
					(int)sectors[i].port_per[0],
					org, 
					sectors[i].port_amt[1],
					(int)sectors[i].port_per[1],
					equip, 
					sectors[i].port_amt[2],
					(int)sectors[i].port_per[2]);
				}
				else
				{
					fprintf(fp, "%5ld %c%5ld %3d%% %c%5ld %3d%% %c%5ld %3d%%\n",
					i+1,
					fuel, 
					sectors[i].port_amt[0],
					(int)sectors[i].port_per[0],
					org, 
					sectors[i].port_amt[1],
					(int)sectors[i].port_per[1],
					equip, 
					sectors[i].port_amt[2],
					(int)sectors[i].port_per[2]);
				}
			}      
		}
	}
	fprintf(fp, "\n: ENDINTERROG\n");

	fclose(fp);
}
void exportCIMSectorData_Text()
{
  exportAllCIMData_Text (1, 0);
}
void exportCIMPortData_Text()
{
  exportAllCIMData_Text (0, 1);
}

int sectorExistsInDatabase(long newSector, long currentSector) {
	return (sectors[currentSector-1].warp_sect[0] == newSector ||
			sectors[currentSector-1].warp_sect[1] == newSector ||
			sectors[currentSector-1].warp_sect[2] == newSector ||
			sectors[currentSector-1].warp_sect[3] == newSector ||
			sectors[currentSector-1].warp_sect[4] == newSector ||
			sectors[currentSector-1].warp_sect[5] == newSector);

}
void parseAndEnterUserFigs(char *fig_data)
{
	char *isEnd;
	char *token;
	int i=0;
	long mySector = -1;

	if (Instr(1, fig_data, "No fighters deployed"))
	{
		printf ("No figs deployed\n");
		return;
	}

	isEnd = strstr(fig_data, "Total");

	//if we did NOT find the Total string
	if (isEnd==NULL && strcmp(fig_data, "")!=0)
	{
		token = strtok(fig_data," ");
		while(token != NULL)
		{
			//  printf ("%s ", token);
			if (i==0)
				mySector=atol(token);
			else if (i==1)
			{
				if (mySector>0 && mySector<=header.sector_cnt)
				{
					sectors[mySector-1].ftrs=atol(token);
//					sprintf (data, "sector %ld: figs: %ld", mySector, sectors[mySector-1].ftrs);
//					outputData(data);
				}
			}
			else if (i==2)
			{
				if (mySector>0 && mySector<=header.sector_cnt)
				{
					if (strcmp(token, "Personal")==0)
						sectors[mySector-1].ftr_owner=0;
					else if (strcmp(token, "Corp")==0)
					{
//						sprintf (data, "corp num: %d", players[0].corpNumber);
//						outputData(data);
						if (players[0].corpNumber >= 0)
							sectors[mySector-1].ftr_owner=CORP_CONSTANT+players[0].corpNumber;
						else
							sectors[mySector-1].ftr_owner=0;
					}
//					sprintf (data, "sector %ld: owner: %d", mySector, sectors[mySector-1].ftr_owner);
//					outputData(data);
				}
			}
			else if (i==3)
			{
				if (mySector>0 && mySector<=header.sector_cnt)
				{
					if (strcmp(token, "Toll")==0)
						sectors[mySector-1].ftr_type=1;
					else if (strcmp(token, "Offensive")==0)
						sectors[mySector-1].ftr_type=2;
					else if (strcmp(token, "Defensive")==0)
						sectors[mySector-1].ftr_type=3;
					else if (strcmp(token, "Mercenary")==0)
						sectors[mySector-1].ftr_type=0;
					else if (strcmp(token, "Unknown")==0)
						sectors[mySector-1].ftr_type=-1;

//					sprintf (data, "sector %ld: type: %d", mySector, sectors[mySector-1].ftr_type);
//					outputData(data);
				}
			}
			i++;
			token = strtok(NULL," ");
		}	  
	}
}


int parseTradeNumber (char * traderData)
{
	char name[100], name1[100], corp[5], *tmp=NULL, *tmp1=NULL, buffer[100];
	char userName[100];
	int i=0, j=0, k=0, returnVal=0;
	struct htab *curhash=NULL;

	int findSlash=0;
//	outputData("start parseTradeNumber");
	findSlash = Instr (1, traderData, ", w/");

	for (i=0; i<findSlash-1 && i < (int) strlen (traderData); i++)
		name[i] = traderData[i];
	name[i]=0;

	strcpy (name1, name);

	if (name[i-1]==']')
	{
		for (; i>0; i--)
		{
			if (name[i] != '[')
				name[i]=0;
			else
			{
				j=i;
				i=0;
			}
		}
		
		name[j]=0;
		name[j-1]=0;
	}

	for (i=strlen(name1)-1; i>=0; i--)
		if (name1[i]=='[')
		{
			j=i;
			i=0;
		}

	k=0;
	for (i=j; i < (int) strlen(name1); i++)
	{
		if (name1[i] != '[' && name1[i] != ']' && name1[i] != ' ')
		{
				corp[k++]=name1[i];
		}
	}
	corp[k]=0;

	
	findSlash=0;

	for (i=0; i < TOTAL_TRADER_TITLES && findSlash==0; i++)
	{
		findSlash = Instr(1, name, traderTitle[i]);
		if (findSlash != 0)
		{
			tmp = right (name, strlen(name)-strlen(traderTitle[i])-1);
			if (strlen(tmp)>0 && tmp[0] == ' ')
			{
				tmp1 = right (tmp, strlen(tmp)-1);
				pfree (&tmp);
				tmp = tmp1;
			}
			strcpy(userName, tmp);
			pfree(&tmp);
		}
	}

	/* try looking up this key */
	curhash=findhash(userName);

	//playerHashTable
	/* if we didn't find anything, insert the new key and value */
	if(curhash == NULL)
	{
		for(i=0; i< MAX_PLAYERS; i++)
		{
			if (players[i].autonum==-1)
				break;
		}
		players[i].autonum=i;
		tmp = left (userName, 40);
		strcpy(players[i].name, tmp);
		pfree(&tmp);
		players[i].corpNumber=atoi(corp);
		writeSetReady();

		sprintf (buffer, "%d", i);
		returnVal = i;

		curhash = addhash(userName, buffer);

		sprintf(data, "Added \'%s\' to \'%s\'", curhash->data, curhash->key);
		outputData(data);
	}
	else
	{
		sprintf (data, "user found %s at %d", players[atoi(curhash->data)].name, players[atoi(curhash->data)].autonum);
		outputData(data);
		returnVal = players[atoi(curhash->data)].autonum;
	}
	return(returnVal);
}


char* getTraderName(char *tName)
{
	int findSlash=0, i;
	char *returnVal=NULL, *returnVal1=NULL;

	for (i=0; i < TOTAL_TRADER_TITLES && findSlash==0; i++)
	{
		findSlash = Instr(1, tName, traderTitle[i]);
		if (findSlash != 0)
		{
			returnVal = right (tName, strlen(tName)-strlen(traderTitle[i])-1);
			if (strlen(returnVal)>0 && returnVal[0] == ' ')
			{
				returnVal1 = right (returnVal, strlen(returnVal)-1);
				pfree (&returnVal);
				returnVal = returnVal1;
			}
		}
	}
	if (returnVal==NULL)
	{
		return (tName);
	}
	else
	{
		pfree(&tName);
		return (returnVal);
	}
}


int getTradeNumber (char * traderData)
{
	char buffer[100], *tmp=NULL;
	char userName[40];
	int i=0, returnVal=0;
	struct htab *curhash=NULL;

	strcpy(userName, traderData);

	/* try looking up this key */
	curhash=findhash(userName);

	/* if we didn't find anything, insert the new key and value */
	if(curhash == NULL)
	{
		for(i=0; i< MAX_PLAYERS; i++)
		{
			if (players[i].autonum==-1)
				break;
		}
		players[i].autonum=i;
		tmp = left (userName, 40);
		strcpy(players[i].name, tmp);
		pfree(&tmp);
		writeSetReady();

		sprintf (buffer, "%d", i);
		returnVal = i;

		curhash = addhash(players[i].name, buffer);
		sprintf(data, "Added \'%s\' to \'%s\'", curhash->data, curhash->key);

	}
	else
	{
		returnVal = players[atoi(curhash->data)].autonum;
	}

	return(returnVal);
}

int getTradeNumberWithoutAdding (char * traderData)
{
	int returnVal=0;
	struct htab *curhash=NULL;

	/* try looking up this key */
	curhash=findhash(traderData);

	/* if we didn't find anything, insert the new key and value */
	if(curhash != NULL) {
		returnVal = players[atoi(curhash->data)].autonum;
	} else {
		returnVal=-1;
	}

	return(returnVal);
}

void loadTraderTitles()
{
  strcpy(traderTitle[0],"Private 1st Class");
  strcpy(traderTitle[1],"Private");
  strcpy(traderTitle[2],"Lance Corporal");
  strcpy(traderTitle[3],"Corporal");
  strcpy(traderTitle[4],"Staff Sergeant");
  strcpy(traderTitle[5],"Gunnery Sergeant");
  strcpy(traderTitle[6],"1st Sergeant");
  strcpy(traderTitle[7],"Sergeant Major");
  strcpy(traderTitle[8],"Sergeant");
  strcpy(traderTitle[9],"Chief Warrant Officer");
  strcpy(traderTitle[10],"Warrant Officer");
  strcpy(traderTitle[11],"Nuisance 3rd Class");
  strcpy(traderTitle[12],"Nuisance 2nd Class");
  strcpy(traderTitle[13],"Nuisance 1st Class");
  strcpy(traderTitle[14],"Menace 3rd Class");
  strcpy(traderTitle[15],"Menace 2nd Class");
  strcpy(traderTitle[16],"Menace 1st Class");
  strcpy(traderTitle[17],"Smuggler 3rd Class");
  strcpy(traderTitle[18],"Smuggler 2nd Class");
  strcpy(traderTitle[19],"Smuggler 1st Class");
  strcpy(traderTitle[20],"Smuggler Savant");
  strcpy(traderTitle[21],"Robber");
  strcpy(traderTitle[22],"Ensign");
  strcpy(traderTitle[23],"Lieutenant J.G.");
  strcpy(traderTitle[24],"Lieutenant Commander");
  strcpy(traderTitle[25],"Lieutenant");
  strcpy(traderTitle[26],"Commander");
  strcpy(traderTitle[27],"Captain");
  strcpy(traderTitle[28],"Commodore");
  strcpy(traderTitle[29],"Rear Admiral");
  strcpy(traderTitle[30],"Vice Admiral");
  strcpy(traderTitle[31],"Fleet Admiral");
  strcpy(traderTitle[32],"Admiral");
  strcpy(traderTitle[33],"Terrorist");
  strcpy(traderTitle[34],"Infamous Pirate");
  strcpy(traderTitle[35],"Notorious Pirate");
  strcpy(traderTitle[36],"Dread Pirate");
  strcpy(traderTitle[37],"Pirate");
  strcpy(traderTitle[38],"Galactic Scourge");
  strcpy(traderTitle[39],"Enemy of the State");
  strcpy(traderTitle[40],"Enemy of the People");
  strcpy(traderTitle[41],"Enemy of Humankind");
  strcpy(traderTitle[42],"Heinous Overlord");
  strcpy(traderTitle[43],"Prime Evil");
  strcpy(traderTitle[44],"Civilian");
  strcpy(traderTitle[45],"Ambassador");
  strcpy(traderTitle[46],"Annoyance");
  
}

void addToMainWindow(char *message, int forecolor, int backcolor, int bold, int enter)
{
	char escape[2];   /* escape sequence */
	char crlf[2];   /* crlf character sequence */
	char ansicolortext[1024];  /* color we are changing text to */
	int n=0; /* count length of sprintf change */
	int foreground=0, background=0;
	char *Msg=NULL;
	int i=0;

	/* set up the escape key */
	escape[0]=27;
	escape[1]='[';

	/* set up the carriage return line feed */
	if (enter==1)
	{
		crlf[0]=13;
		crlf[1]=10;
	}
	else
	{
		crlf[0]=0;
		crlf[1]=0;
	}

	/* ANSI values for foreground and background */
	foreground=30;
	background=40;

	/* create colored text */
	n = sprintf (ansicolortext, "%c%c%d;%d;%dm%s%c%c%d;%ld;%ldm%c%c", escape[0], escape[1], bold == 1 ? 1 : 0, (forecolor+foreground), (backcolor+background), message, escape[0], escape[1], 1, (BLACK+foreground), (BLACK+background),crlf[0], crlf[1]);

	Msg = Translate_String(ansicolortext, 0, 0);

	c_write(ldisc_extern, Msg, strlen(Msg));

	if (remoteConnectionEnabled)
	{
		for (i=0; i<TOTAL_CONNECTIONS; i++)
		{
			if (theClients[i].sock>0)
			{
				if( send(theClients[i].sock, Msg, strlen(Msg), 0) < 0)
				{	// send message to the client
					printf("send() failed to send data back to client.\n");
					shutdown(theClients[i].sock, SD_SEND);
					closesocket (theClients[i].sock);
					theClients[i].sock=0;
					updateRemoteList();
				}
			}
		}
	}

	pfree(&Msg);
}

char* createANSIText(char *message, int forecolor, int backcolor, int bold, int enter)
{
	char escape[2];   /* escape sequence */
	char crlf[2];   /* crlf character sequence */
	char ansicolortext[1024];  /* color we are changing text to */
	int n; /* count length of sprintf change */
	int foreground, background;
	char *Msg;

	/* set up the escape key */
	escape[0]=27;
	escape[1]='[';

	/* set up the carriage return line feed */
	if (enter==1)
	{
		crlf[0]=13;
		crlf[1]=10;
	}
	else
	{
		crlf[0]=0;
		crlf[1]=0;
	}

	/* ANSI values for foreground and background */
	foreground=30;
	background=40;

	/* create colored text */
	n = sprintf (ansicolortext, "%c%c%d;%d;%dm%s%c%c%d;%ld;%ldm%c%c", escape[0], escape[1], bold == 1 ? 1 : 0, (forecolor+foreground), (backcolor+background), message, escape[0], escape[1], 1, (BLACK+foreground), (BLACK+background),crlf[0], crlf[1]);

	Msg = Translate_String(ansicolortext, 0, 0);

	return (Msg);
}

void attacErrorMessage(char *msg)
{
  char *tmp=NULL;

  WaitForSingleObject(MessageMutex, INFINITE);

  tmp = (char *) malloc(50 + strlen(msg) + 1);
  sprintf(tmp, "[ATTAC ERROR] %s", msg);
  addToMessageCenter(tmp, ANSI_BLUE, BLACK, TRUE);
  pfree(&tmp);

  ReleaseMutex(MessageMutex);
}

void attacInfoMessage(char *msg)
{
  char *tmp=NULL;

  WaitForSingleObject(MessageMutex, INFINITE);
  tmp = (char *) malloc(50 + strlen(msg) + 1);
  sprintf(tmp, "[ATTAC INFO] %s", msg);
  addToMessageCenter(tmp, ANSI_BLUE, BLACK, TRUE);
  pfree(&tmp);
  ReleaseMutex(MessageMutex);
}

void attacScriptMessage(char *header, char *msg, int fore, int back, int bold)
{
  char *tmp=NULL;

  WaitForSingleObject(MessageMutex, INFINITE);
  tmp = (char *) malloc(50 + strlen(header) + strlen(msg) + 1); 
  sprintf(tmp, "[%s] %s", header, msg);
  addToMessageCenter(tmp, fore, back, bold);
  pfree(&tmp);
  ReleaseMutex(MessageMutex);
}

void attacScriptNotify(char *msg)
{
  char *tmp=NULL;

  WaitForSingleObject(MessageMutex, INFINITE);
  tmp = (char *) malloc(50 + strlen(msg) + 1); 
  sprintf(tmp, "[SCRIPT NOTIFY] %s", msg);
  addToMessageCenter(tmp, ANSI_YELLOW, BLACK, TRUE);
  pfree(&tmp);
  ReleaseMutex(MessageMutex);
}

void attacFedCommMessage(char *msg, char *from)
{
	char *tmp=NULL;

	WaitForSingleObject(MessageMutex, INFINITE);

	tmp = (char *) malloc(50 + strlen(msg) + strlen(from) + 1);
	if (strcmp (from, "ME") == 0)
	{
		sprintf(tmp, "[FedComm %s] %s", from, msg);
		addToMessageCenter(tmp, ANSI_GREEN, BLACK, TRUE);
	}
	else
	{
		sprintf(tmp, "[FedComm %s] %s", from, msg);
		addToMessageCenter(tmp, ANSI_YELLOW, BLACK, TRUE);
	}

	pfree(&tmp);
	ReleaseMutex(MessageMutex);
}

void attacSubSpaceMessage(char *msg, char *from, long channel)
{
	char *tmp=NULL;

	WaitForSingleObject(MessageMutex, INFINITE);

	tmp = (char *) malloc(50 + strlen(msg) + strlen(from) + 1);
	if (strcmp (from, "ME") == 0)
	{
		sprintf(tmp, "[Private Channel - %s] %s", from, msg);
		addToMessageCenter(tmp, ANSI_GREEN, BLACK, TRUE);
	}
	else
	{
		sprintf(tmp, "[Private Channel %ld - %s] %s", channel, from, msg);
		addToMessageCenter(tmp, ANSI_YELLOW, BLACK, TRUE);
	}
	pfree(&tmp);
	ReleaseMutex(MessageMutex);
}

void attacHailMessage(char *msg, char *from)
{
  char *tmp=NULL;

  WaitForSingleObject(MessageMutex, INFINITE);

  tmp = (char *) malloc(50 + strlen(msg) + strlen(from) + 1);
  sprintf(tmp, "[Hailing %s] %s", from, msg);
  addToMessageCenter(tmp, ANSI_YELLOW, BLACK, TRUE);
  pfree(&tmp);
  ReleaseMutex(MessageMutex);
}

void attacSentMessage(char *msg)
{
  char *tmp=NULL;

  WaitForSingleObject(MessageMutex, INFINITE);

  tmp = (char *) malloc(50 + strlen(msg) + 1);
  sprintf(tmp, "[From Me] %s", msg);
  addToMessageCenter(tmp, ANSI_YELLOW, BLACK, FALSE);
  pfree(&tmp);
  ReleaseMutex(MessageMutex);
}


/* we pass in the 4 ids and a regcode. the last parameter says that if it is 0
   then use the regcode that we pass in. 
   if it is 1, then read them from a file 
*/

int isValidRegCode (char* user1, char* user2, char* user3, char* user4, char* reg, char* twxReg, int useRegTxtFile, char* userHandle)
{
	char *k_e_y=NULL;
	char *t_w_x_k_e_y=NULL;
	char *allUserIDs=NULL;
	char filename[MAX_LENGTH];
	char *token=NULL, tempString[2056];
	char *tmp2=NULL, *tmp=NULL, *s1=NULL, *s2=NULL, *s3=NULL, *s4=NULL;
	char *s5=NULL, *s6=NULL, *s7=NULL, *s8=NULL;
	char *userID1 = NULL;
	char *userID2 = NULL;
	char *userID3 = NULL;
	char *userID4 = NULL;
	char *regCode = NULL;
	char *twxRegCode = NULL;
	FILE *fp=NULL;
	int i=0;
	int returnValue=0, skipToEnd=1, checkTWXCode=-1;

	strcpy(tempString, "\n");

	if (useRegTxtFile==1)
	{
		strcpy (fullFilePath, appDir);
		strcat (fullFilePath, "reg.txt");

		strcpy(filename, fullFilePath);

		fp = fopen(filename, "r");

		if (fp==NULL){
			char bufTemp[MAX_LENGTH];

			strcpy (bufTemp, exeDir);
			strcat (bufTemp, "reg.txt");

			strcpy(filename, bufTemp);
			fp = fopen(filename, "r");
			//if we found the file
			if (fp != NULL) {
				//close it
				fclose (fp);
				//move it
				MoveFile(filename, fullFilePath);
				//reopen it in the proper location
				strcpy(filename, fullFilePath);
				fp = fopen(filename, "r");
			}
		}
		strcpy(filename, fullFilePath);
		if (fp==NULL)
		{
			sprintf (data, "No reg.txt found at %s", fullFilePath);
			outputData(data);
			attacInfoMessage ("Invalid Registration Key");
			set_title(NULL, "ATTAC (Unregistered)");
			validRegCode=0;
			isRegistered=0;
			validTWXSyncRegCode=0;
			return (0);
		}
		else
		{
			for (i=0; i < (int) strlen (tempString); i++)
				tempString[i]=0;
			i=0;
			//count bytes by reading thru once
			while (!feof(fp))
			{
				fscanf(fp, "%c", &tempString[i]);
				i++;
			}
			tempString[i-1]=0;

			fclose (fp);

			i=0;
			token = strtok(tempString, "*");
			while(token != NULL)
			{
				if (i==0) {
					if (strcmp (token, "-1")!=0)
						userID1 = (char*) strdup(token);
					else
						userID1 = (char*) strdup("");
				}
				else if (i==1) {
					if (strcmp (token, "-1")!=0)
						userID2 = (char*) strdup(token);
					else
						userID2 = (char*) strdup("");
				}
				else if (i==2) {
					if (strcmp (token, "-1")!=0)
						userID3 = (char*) strdup(token);
					else
						userID3 = (char*) strdup("");
				}
				else if (i==3) {
					if (strcmp (token, "-1")!=0)
						userID4 = (char*) strdup(token);
					else
						userID4 = (char*) strdup("");
				}
				else if (i==4) {
					if (strcmp (token, "-1")!=0)
						regCode = (char*) strdup(token);
					else
						regCode = (char*) strdup("");
				}
				else if (i==5) {
					if (strcmp (token, "-1")!=0)
						twxRegCode = (char*) strdup(token);
					else
						twxRegCode = (char*) strdup("");
				}
				i++;

				token = strtok(NULL,"*");
			}
			/* if we dont have all the user handles... move the reg code
			to the proper  variable and allocate memory for the empty
			user handles 
			*/

			if (i<=5)
			{
				if ((i-1)==1)
				{
					regCode = (char *)strdup(userID2);
					regCode[strlen(userID2)]=0;
 					strcpy(userID2, "");
					userID3 = (char *)strdup("");
					userID4 = (char *)strdup("");
				}
				if ((i-1)==2)
				{
					regCode = (char *)strdup(userID3);
					regCode[strlen(userID3)]=0;
					strcpy(userID3, "");
					userID4 = (char *)strdup("");
				}
				if ((i-1)==3)
				{
					regCode = (char *)strdup(userID4);
					regCode[strlen(userID4)]=0;
					strcpy(userID4, "");
				}
				twxRegCode = (char *) strdup ("");

				//rewrite the reg code file
				strcpy (fullFilePath, appDir);
				strcat (fullFilePath, "reg.txt");
				//create reg.txt file
				fp = fopen (fullFilePath, "w");
				if (fp ==NULL)
					printf ("Error opening %s to create registration\n", fullFilePath);
				else
				{
					fprintf (fp, "%s", userID1);
					fprintf (fp, "*");
					if (strcmp(userID2, "") != 0)
						fprintf (fp, "%s", userID2);
					else
						fprintf (fp, "-1");
					fprintf (fp, "*");
					if (strcmp(userID3, "") != 0)
						fprintf (fp, "%s", userID3);
					else
						fprintf (fp, "-1");
					fprintf (fp, "*");
					if (strcmp(userID4, "") != 0)
						fprintf (fp, "%s", userID4);
					else
						fprintf (fp, "-1");
					fprintf (fp, "*");
					fprintf (fp, "%s", regCode);
					fprintf (fp, "*");
					if (strcmp(twxRegCode, "") != 0)
						fprintf (fp, "%s", twxRegCode);
					else
						fprintf (fp, "-1");
					
					fprintf (fp, "*");
					fclose(fp);
				}
				//end rewrite reg code file
			}
		} 
	}
	else
	{
		/* if we dont read from file. allocate memory for handles and reg code */
		userID1 = (char *) strdup(user1);
		userID2 = (char *) strdup(user2);
		userID3 = (char *) strdup(user3);
		userID4 = (char *) strdup(user4);
		regCode = (char *) strdup(reg);
		twxRegCode = (char *) strdup(twxReg);
	}

	k_e_y = (char*) malloc (20);
	strcpy(k_e_y, "9182xA7F3x99dex223b");

	t_w_x_k_e_y = (char*) malloc (20);
	strcpy(t_w_x_k_e_y, "0203948596839384859");

	strcpy (theUserId, userID1);
	strcat (theUserId, "/");
	strcat (theUserId, userID2);
	strcat (theUserId, "/");
	strcat (theUserId, userID3);
	strcat (theUserId, "/");
	strcat (theUserId, userID4);
	/* allocate memory for the concatenated handles */
	allUserIDs = (char*) malloc (strlen(userID1) +
	  strlen(userID2) +
	  strlen(userID3) + 
	  strlen(userID4) + 1);

	/* if reg code is empty or handle1 is empty, return 0 */
	if (strcmp(userID1, "")==0)
	{
		attacInfoMessage ("Invalid Registration Key");
		set_title(NULL, "ATTAC (Unregistered)");
		validRegCode=0;
		isRegistered=0;
		validTWXSyncRegCode=0;
		returnValue = 0;
		skipToEnd=0;
	}
	if (strcmp(regCode, "")==0)
	{
		attacInfoMessage ("Invalid Registration Key");
		set_title(NULL, "ATTAC (Unregistered)");
		validRegCode=0;
		isRegistered=0;
		validTWXSyncRegCode=0;
		returnValue = 0;
		skipToEnd=0;
	}

	/* created concatenated handles */
	strcpy (allUserIDs, "");
	strcat (allUserIDs, userID1);
	strcat (allUserIDs, userID2);
	strcat (allUserIDs, userID3);
	strcat (allUserIDs, userID4);

	/* be sure to NULL-terminate */
	allUserIDs[strlen(allUserIDs)] = 0; 

//	printf ("allUserIDs: %s\n", allUserIDs);
	if (skipToEnd==1)
	{
		/* decide whether or not to register the handles */
		tmp = encrypt_string(k_e_y, allUserIDs);
		tmp2 = encrypt_string(t_w_x_k_e_y, allUserIDs);
		checkTWXCode = strcmp(tmp2, twxRegCode);
		if (strcmp(tmp, regCode)==0)
		{
			/* if there is no handle to match, validate else
			match the handle versus the user ids. use strstr so it matches with
			a substring */
			if (userHandle==NULL || strcmp(userHandle, "")==0)
			{
				if (1 || checkTWXCode==0) {
					attacInfoMessage ("Valid Registration Key - TWXSync enabled");
					set_title(NULL, "ATTAC (Registered) - TWXSync enabled");
					validTWXSyncRegCode=1;
				}
				else {
					attacInfoMessage ("Valid Registration Key");
					set_title(NULL, "ATTAC (Registered)");
					validTWXSyncRegCode=0;
				}

				validRegCode=1;
				isRegistered=1;
				returnValue = 1;
			}
			else
			{
				userID1 = (char *)strlwr(userID1);
				userID2 = (char *)strlwr(userID2);
				userID3 = (char *)strlwr(userID3);
				userID4 = (char *)strlwr(userID4);
				userHandle = (char *)strlwr(userHandle);

				s1 = (char *)strdelimit(userID1, '.', ' ');
				s2 = (char *)strdelimit(userID2, '.', ' ');
				s3 = (char *)strdelimit(userID3, '.', ' ');
				s4 = (char *)strdelimit(userID4, '.', ' ');

				s5 = (char *)strdelimit(userID1, ' ', '.');
				s6 = (char *)strdelimit(userID2, ' ', '.');
				s7 = (char *)strdelimit(userID3, ' ', '.');
				s8 = (char *)strdelimit(userID4, ' ', '.');

				// check for a portion of the handle and also replace the periods
				// with spaces and check that too
				if (Instr(1, userHandle, userID1) ||
				Instr(1, userHandle, userID2) ||
				Instr(1, userHandle, userID3) ||
				Instr(1, userHandle, userID4) ||
				Instr(1, userHandle, s1) ||
				Instr(1, userHandle, s2) ||
				Instr(1, userHandle, s3) ||
				Instr(1, userHandle, s4) ||
				Instr(1, userHandle, s5) ||
				Instr(1, userHandle, s6) ||
				Instr(1, userHandle, s7) ||
				Instr(1, userHandle, s8)
				)
				{

					if (1 || checkTWXCode==0) {
						attacInfoMessage("Valid Handle. Registration Validated - TWXSync enabled");
						set_title(NULL, "ATTAC (Registered) - TWXSync enabled");
						validTWXSyncRegCode=1;
					}
					else {
						attacInfoMessage("Valid Handle. Registration Validated");
						set_title(NULL, "ATTAC (Registered)");
						validTWXSyncRegCode=0;
					}
				
					validRegCode=1;
					isRegistered=1;
					returnValue = 2;
				}
				else
				{
					attacInfoMessage ("User Handles Do Not Match");
					set_title(NULL, "ATTAC (Unregistered)");
					validRegCode=0;
					isRegistered=0;
					checkTWXCode=-1;
					validTWXSyncRegCode=0;
					returnValue = -1;
				}
				pfree(&s1);
				pfree(&s2);
				pfree(&s3);
				pfree(&s4);
				pfree(&s5);
				pfree(&s6);
				pfree(&s7);
				pfree(&s8);
			}
		}
		else
		{
			attacInfoMessage ("Invalid Registration Key");
			set_title(NULL, "ATTAC (Unregistered)");
			validRegCode=0;
			isRegistered=0;
			validTWXSyncRegCode=0;
			returnValue = 0;
		}
		pfree(&tmp);
		pfree(&tmp2);
	}
	pfree(&k_e_y);
	pfree(&t_w_x_k_e_y);
	pfree(&allUserIDs);
	pfree(&userID1);
	pfree(&userID2);
	pfree(&userID3);
	pfree(&userID4);
	pfree(&regCode);
	pfree(&twxRegCode);
	return (returnValue);
}

char* strdelimit (char *string, char delimiter, char new_delimiter)
{
	int i=0;
	char *tmp=NULL;

	tmp = (char *) strdup (string);

	for (i=0; i< (int) strlen(tmp); i++)
	{
		if (tmp[i]==delimiter)
			tmp[i]=new_delimiter;
	}
	return(tmp);
}

/**
 * Is 1 indexed, not 0 indexed
 */
int isValidSectorOneOffset(long theSector) {

	if (theSector <= 0 ||  theSector>header.sector_cnt)
		return 0;
	else 
		return 1;
}

/**
 * Is 1 indexed, not 0 indexed
 */
int isValidSectorZeroOffset(long theSector) {

	if (theSector < 0 ||  theSector>=header.sector_cnt)
		return 0;
	else 
		return 1;
}


/**
 * Is 1 indexed, not 0 indexed
 */
int isValidSectorOneOffsetString(char* theSector) {
	if (theSector==NULL)
		return 0;
	else
		return isValidSectorOneOffset(atoi(theSector));
}

/**
 * Is 1 indexed, not 0 indexed
 */
int isValidSectorZeroOffsetString(char* theSector) {
	if (theSector==NULL)
		return 0;
	else
		return isValidSectorZeroOffset(atoi(theSector));
}


long findNearestFigs (long theSector)
{
	long popSector=0;
	long sector_long=0;
	long returnVal=0;
	long maxDepth=1000000, level=0;

	if (isConnected==0 && isOffline==0)
	{
		attacErrorMessage("No Game Selected. Click File->Connect. Then select a game. Then Work Offline or Connect");
		return 0;
	}


	/*push the first sector onto the queue */
	pushQueue (theSector);

	while (!isQueueEmpty())
	{
		popSector = popQueue();
		sector_long = popSector;

		/*
		printf ("SECTOR FIGS: %d\n", sectors[sector_long-1].ftrs);
		printf ("SECTOR FIGS OWNER: %d\n", sectors[sector_long-1].ftr_owner);
		printf ("PLAYER 0 CORP: %d\n", players[0].corpNumber);
		*/

		if (sectors[sector_long-1].ftrs>0 && 
		(sectors[sector_long-1].ftr_owner==0 ||
		(sectors[sector_long-1].ftr_owner-CORP_CONSTANT)==players[0].corpNumber))
		{
			//if they do check if they are yours or they belong to your corp
			returnVal = sector_long;

			//empty out the queue since we found the sector
			while (!isQueueEmpty())
			{
				popSector = popQueue();
			}
		}
		else
		{
			//didnt find our sector so push the children onto the queue
			level++;
			//  printf ("LEVEL %d\n", level);
			if (sectors[sector_long-1].warp_sect[0] > 0)
				pushQueue (sectors[sector_long-1].warp_sect[0]);
			if (sectors[sector_long-1].warp_sect[1] > 0)
				pushQueue (sectors[sector_long-1].warp_sect[1]);
			if (sectors[sector_long-1].warp_sect[2] > 0)
				pushQueue (sectors[sector_long-1].warp_sect[2]);
			if (sectors[sector_long-1].warp_sect[3] > 0)
				pushQueue (sectors[sector_long-1].warp_sect[3]);
			if (sectors[sector_long-1].warp_sect[4] > 0)
				pushQueue (sectors[sector_long-1].warp_sect[4]);
			if (sectors[sector_long-1].warp_sect[5] > 0)
				pushQueue (sectors[sector_long-1].warp_sect[5]);
		}
		if (level>maxDepth)
		{
			//empty out the queue since we found the sector
			while (!isQueueEmpty())
			{
				popSector = popQueue();
				returnVal=0;	      
			}
			break;
		}
	}

	queueEmpty();
	return (returnVal); 
}


long Get_Time()
{
	time_t data;

	time(&data);

	return data;
}

void parseTraderListing (char *str)
{
	//actual amount +1 for NULL char
	char autonum[4];
	char randAndAlignment[22];
	char exp[22], align[22];
	char *m_exp, *m_align;
	char traderCorp[3];
	char traderName[40], buffer[100];
	int i=0, j=0, k=0, l=0, m=0;
	int previousAutoNumber=0;
	char *token=NULL, *tmp=NULL, *tmp1=NULL;
	struct htab *curhash=NULL;
	
	if (strlen(str)>30)
	{
		if (strlen(str)>80)
		{
			tmp = left (str, 80);
			pfree(&str);
			str = (char *) strdup (tmp);
			pfree (&tmp);
		}
		tmp = (char *) left (str, 20);
		//if we have a split screen line
		if (strcmp (tmp, "                    ")==0)
		{
			pfree(&tmp);
		}
		else
		{
			for (i=0; i< (int)strlen(str); i++)
			{
				if (i>=0 && i<=2)
					autonum[j++]=str[i];
				else if (i>=4 && i<=24)
					randAndAlignment[k++]=str[i];
				else if (i>=26 && i<=27)
					traderCorp[l++]=str[i];
				else if (i>=29 && i<=59)
					traderName[m++]=str[i];
			}
		
			autonum[j]=0;
			randAndAlignment[k]=0;
			traderCorp[l]=0;
			traderName[m]=0;

			i=m;
			while(i>0 && (traderName[i]==0 || traderName[i]==' '))
			{
				i--;
			}
			
			m=i+1;

			traderName[m]=0;

			token = strtok(randAndAlignment, " ");
			strcpy (exp, token);
			while (token != NULL)
			{
				token = strtok(NULL, " ");
				if (token != NULL)
					strcpy (align, token);
			}

			m_exp = UNCOMMA(strdup(exp));
			m_align = UNCOMMA(strdup(align));

			curhash=findhash(traderName);

			//playerHashTable
			/* if we didn't find anything, insert the new key and value */
			if(curhash == NULL)
			{
				for(i=0; i< MAX_PLAYERS; i++)
				{
					if (players[i].autonum==-1)
						break;
				}
//				sprintf (data, "new user: %s in spot %d", traderName, i);
//				outputData(data);
				players[i].autonum=i;
				tmp1 = left (traderName, 40);
				strcpy(players[i].name, tmp1);
				pfree(&tmp1);
				if (strcmp (traderCorp, "**") != 0)
					players[i].corpNumber=atoi(traderCorp);
				players[i].experience = atol(m_exp);
				players[i].alignment = atol(m_align);
				players[i].last_update=ATTAC_Time();
				
				writeSetReady();

				sprintf (buffer, "%d", i);

				curhash = addhash(traderName, buffer);
//				sprintf(data, "Added \'%s\' to \'%s\'", curhash->data, curhash->key);
//				outputData(data);
				previousAutoNumber=i;

				//start TWXSync
				buildSendTWXData_WaitForData(INFINITE);
				dataMessage = buildSendTWXData(players[i].autonum, TRADER_DATA);
				postSendMessageToTWXSync(threadId, strdup(dataMessage));
				free(dataMessage);
				buildSendTWXData_ReleaseData();
				//end TWXSync
				
			}
			else
			{
//				sprintf (data, "user found %s at %d", players[atoi(curhash->data)].name, players[atoi(curhash->data)].autonum);
//				outputData(data);
				if (strcmp (traderCorp, "**") != 0)
					players[atoi(curhash->data)].corpNumber = atoi(traderCorp);

				players[atoi(curhash->data)].experience = atol(m_exp);
				players[atoi(curhash->data)].alignment = atol(m_align);
				players[atoi(curhash->data)].last_update=ATTAC_Time();
				previousAutoNumber = atoi(curhash->data);

				//start TWXSync
				buildSendTWXData_WaitForData(INFINITE);
				dataMessage = buildSendTWXData(atoi(curhash->data), TRADER_DATA);
				postSendMessageToTWXSync(threadId, strdup(dataMessage));
				free(dataMessage);
				buildSendTWXData_ReleaseData();
				//end TWXSync

			}

			pfree(&m_exp);
			pfree(&m_align);
			pfree(&tmp);
		}
		

//		printf ("exp %s\n", exp);
//		printf ("align %s\n", align);
//		printf ("traderCorp %s\n", traderCorp);
//		printf ("traderName %s\n\n\n", traderName);
	}
}


//sscanf(str, "%02x%02x%02x%02x%02x%02x", &key[0], &key[1] ...);
char* EncryptCommMessage(char *text, char *name)
{
	unsigned char key[7];
	unsigned char tempKey[9];
	char *returnVal=NULL;
	int len=0, i=0;
	
	sscanf (header.fedCommKey, "%02x%02x%02x%02x%02x%02x%02x%02x%02x", &tempKey[0], &tempKey[1], &tempKey[2], &tempKey[3], &tempKey[4], &tempKey[5], &tempKey[6], &tempKey[7], &tempKey[8]);

	//store as size 18, convert it to 14
	for (i =0; i<7; i++)
		key[i] = tempKey[i];

	// EncryptMessage
//	printf ("name: %s\n", name);
	len = TWC_EncryptMessage ((unsigned char *)text,  strlen(text), key, name);

	returnVal = (char *) malloc (strlen (text) +1);
	strcpy (returnVal, text);


	return(returnVal);	
}

char* DecryptCommMessage(char *text, char *name)
{
	unsigned char key[7];
	unsigned char tempKey[9];
	char *returnVal=NULL;
	int len=0, i=0;

	sscanf (header.fedCommKey, "%02x%02x%02x%02x%02x%02x%02x%02x%02x", &tempKey[0], &tempKey[1], &tempKey[2], &tempKey[3], &tempKey[4], &tempKey[5], &tempKey[6], &tempKey[7], &tempKey[8]);

	//store as size 18, convert it to 14
	for (i =0; i<7; i++)
		key[i] = tempKey[i];

	// DecryptMessage

	len = TWC_DecryptMessage ((unsigned char *)text,  strlen(text), key, name);

	if (len==-1)
	{
		returnVal = (char *) malloc (strlen (text) +1);
		strcpy (returnVal, text);
	}
	else
	{
		returnVal = (char *) malloc (strlen (text) + strlen ("[Decrypted] ") +1);
		strcpy (returnVal, "[Decrypted] ");
		strcat (returnVal, text);
	}

	return(returnVal);
}

void pfree (void **x)
{
    WaitForSingleObject(PFreeMutex, INFINITE);
    if (*x==NULL) {
		#ifdef ATTAC_CODE
				attacScriptNotify("THERE IS A FREE MEMORY ERROR");
		#else
				printf ("THERE IS A FREE MEMORY ERROR");
		#endif
	}
    else {
		free(*x);
	}
    *x = NULL;
    ReleaseMutex(PFreeMutex);
}
