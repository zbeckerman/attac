// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <stdarg.h>
#include <process.h>    /* _beginthread, _endthread */

#include "attacGlobal.h"
#include "attacUtils.h"
#include "util.h"
#include "parsedatathread.h"
#include "statusthread.h"
#include "rexx.h"
#include "structs.h"
#include "linklist.h"
#include "win_res.h"
#include "map.h"
#include "info.h"
#include "twxSync.h"
#include "hash.h"


HANDLE StatusWaitEvent;
HANDLE statusThreadsMutex;
int StatusWaitCount=0;
char StatusWaitArray[MAX_LENGTH][MAX_AMOUNT];
char StatusLastLine[MAX_LENGTH];
int StatusWaitThreadResult=0;
char DSM_BufferStatus[MAX_LENGTH];
int waitForStatusEvent=0;
int statusTest[LIMPIT_COUNT_MAX];
int startParsing=0;

void DISPLAY_UNIVERSE_SIZE_ERROR(int s, char where[]) 
{ 
	WaitForSingleObject(SyncDataMutex, INFINITE);
//  char *buffer = malloc(1024); 

//  sprintf(buffer, "Universe Size=%ld  CurrentSector=%d  where=%s", header.sector_cnt, (s), (where)); 
//  attacErrorMessage(buffer); 
//  free (buffer);
	ReleaseMutex(SyncDataMutex);
}


// this function is called by a new thread
void StatusThreadProc( void *dummy )
{
	int result=0, result1=0, do_what=0, do_what2=0;
	char *last_line=NULL, buff[50], *tmp=NULL, *tempString=NULL;
	int returnVal=0, traderNum=0, num=0, tempInt=0;
	long timeout=0, tempLong=0, tempCurrentSector=0;
	unsigned char PIPE[2];
	RECT rect;
	FILE *fp=NULL;
	int hasPortSector=0, breakFlag=0;
	char tempBuffer[MAX_LENGTH*2];
	int warpCount;
	int i, tmpInt;
	char *token=NULL;
	char strSector[8];

	
	PIPE[0] = 179;
	PIPE[1] = 0;

	StatusWaitEvent = CreateEvent(NULL, 0, 0, NULL);

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

	outputData ("start while threads");
	while (1)
	{
		timeout = sSCRIPT_WAIT_MUX(INFINITE,
							"<Info>",
							" Sect ",
							"Command [TL=", 
							"Auto Warping to sector ",
							"Warping to Sector ",
							"Sector  : ",
							" destroyed ",
							"Initializing...",
							"INACTIVITY WARNING:",
							"The shortest path (",
							"Fuel Ore do you want to buy",
							"Organics do you want to buy",
							"Equipment do you want to buy",
							"Fuel Ore do you want to sell",
							"Organics do you want to sell",
							"Equipment do you want to sell",
							"Game Server Administrative Mode",
							"% of max OnBoard",
							"Suddenly you're",
							"<T> Trade at this Port",
							"<Thievery>",
							"Corporate Planet Scan",
							"Personal Planet Scan",
							"Success!",
							"faint, metallic click",
							NULL);
		
		do_what  = result = StatusWaitThreadResult;

//		printf ("statusData: %d\n", do_what);

		if (passToREXX==0)
		{
			//dont do anything
			goto skip_to_next_statement_status;
		}
		else if (startParsing==0 && (do_what != 8 && do_what != 17))
		{
			//do nothing since we shouldnt be parsing data yet
			goto skip_to_next_statement_status;
		}
		else if (do_what == 1 && statusTest[do_what]==1)
		{
			statusThreads_WaitForData(0);
			outputData ("start 1 status");
			numSector = 0;
			numTurns = 0;
			numExp = 0;
			numAlign = 0;
			numCredits = 0;
			numFigs = 0;
			numShields = 0;
			numTotalHolds = 0;
			numEmptyHolds = 0;
			numFuel = 0;
			numOrgs = 0;
			numEquip = 0;
			numColos = 0;
			numCloaks = 0;
			numPhotons = 0;
			numProbes = 0;
			numAMines = 0;
			numLMines = 0;

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Trader Name    : ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = getTraderName(last_line);
			if (last_line != NULL && strlen(last_line)>0 && strlen (last_line) <40) {
				struct htab *curhash=NULL;
				curhash=findhash(last_line);
				if (curhash != NULL) {
					strcpy (curhash->data, last_line);
				}
				strcpy (players[0].name, last_line);
				players[0].last_update = ATTAC_Time();
			}
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Rank and Exp   : ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, " ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			numExp = atol(last_line);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Alignment=");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, " ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			numAlign = atol(last_line);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Corp           # ","Ship Name", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			result1 = StatusWaitThreadResult;
			if (result1==1)
			{
				timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, ",");
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_status;
				last_line = sSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				if (atoi(last_line)>-500 && atoi(last_line) <30000) {
					players[0].corpNumber=atoi(last_line);
					players[0].last_update = ATTAC_Time();
				}

				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif		
			}
			if (result1==2)
			{
				players[0].corpNumber=-1;
				players[0].last_update = ATTAC_Time();
			}

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Current Sector : ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			numSector = atol(last_line);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		
			if (numSector <= 0 || numSector > header.sector_cnt)
				goto skip_to_next_statement_status;

			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector = numSector;
			ReleaseMutex(tempCurrentSectorMutex);

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Turns left     : ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			if (strcmp(last_line, "Unlimited") == 0)
			{
				numTurns = -1;
			}
			else
			{
				last_line = UNCOMMA(last_line);
				numTurns = atol(last_line);
			}
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			do_what2 = 0;

			while (do_what2 < 14)
			{
				timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Fighters       : ",
					"Total Holds    : ",
					"Fuel Ore=",
					"Organics=",
					"Equipment=", 
					"Colonists=",
					"Empty=",
					"Shield points  : ",
					"Armid Mines  T1: ",
					"Limpet Mines T2: ",
					"Photon Missiles: ",
					"Cloaking Device: ",
					"Ether Probes   : ",
					"Credits        : ",
					"[Pause]",
					NULL);

				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_status;

				do_what2 = StatusWaitThreadResult;

				if (do_what2 == 1)       // Fighters
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numFigs = strtol(last_line, NULL, 10);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 2)  // Total Holds
				{
					timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, " ");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numTotalHolds = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 3)  // Fuel Ore
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numFuel = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 4)  // Organics
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numOrgs = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 5)  // Equipment
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numEquip = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 6)  // Colonists
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numColos = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 7)  // Empty
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numEmptyHolds = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 8)  // Shields
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numShields = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 9)  // Armid Mines
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numAMines = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 10) // Limpet Mines
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numLMines = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 11) // Photon Missiles
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numPhotons = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 12) // Cloaking Devices
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numCloaks = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 13) // Ether Probes
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " ", "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numProbes = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
				else if (do_what2 == 14) // Credits
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numCredits = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
				}
			}
/*
			sprintf (data, "numExp: %ld", numExp);
			outputData (data);
			sprintf (data, "numAlign: %ld", numAlign);
			outputData (data);
			sprintf (data, "numTurns: %ld", numTurns);
			outputData (data);
			sprintf (data, "numFigs: %ld", numFigs);
			outputData (data);
			sprintf (data, "numTotalHolds: %ld", numTotalHolds);
			outputData (data);
			sprintf (data, "numFuel: %ld", numFuel);
			outputData (data);
			sprintf (data, "numOrgs: %ld", numOrgs);
			outputData (data);
			sprintf (data, "numEquip: %ld", numEquip);
			outputData (data);
			sprintf (data, "numColos: %ld", numColos);
			outputData (data);
			sprintf (data, "numEmptyHolds: %ld", numEmptyHolds);
			outputData (data);
			sprintf (data, "numShields: %ld", numShields);
			outputData (data);
			sprintf (data, "numAMines: %ld", numAMines);
			outputData (data);
			sprintf (data, "numLMines: %ld", numLMines);
			outputData (data);
			sprintf (data, "numPhotons: %ld", numPhotons);
			outputData (data);
			sprintf (data, "numCloaks: %ld", numCloaks);
			outputData (data);
			sprintf (data, "numProbes: %ld", numProbes);
			outputData (data);
			sprintf (data, "numCredits: %ld", numCredits);
			outputData (data);
			*/
			update_status_bar();
			outputData ("end 1 status");
			statusThreads_ReleaseData();
		}
		else if (do_what == 2 && statusTest[do_what]==1)
		{
			statusThreads_WaitForData(0);
			outputData ("start 2 status");
			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;

			result = StatusWaitThreadResult;
			if (result == 1)
			{
				numSector = 0;
				numTurns = 0;
				numExp = 0;
				numAlign = 0;
				numCredits = 0;
				numFigs = 0;
				numShields = 0;
				numTotalHolds = 0;
				numEmptyHolds = 0;
				numFuel = 0;
				numOrgs = 0;
				numEquip = 0;
				numColos = 0;
				numCloaks = 0;
				numPhotons = 0;
				numProbes = 0;
				numAMines = 0;
				numLMines = 0;

				last_line = sSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				numSector = atol(last_line);

				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif		
				if (numSector <= 0 || numSector > header.sector_cnt)
					goto skip_to_next_statement_status;

				WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
				tempCurrentSector = currentSector = numSector;
				ReleaseMutex(tempCurrentSectorMutex);

				do_what2=0;
				while (do_what2 < 16)
				{
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, 
								  "Turns", 
								  "Creds", 
								  "Figs", 
								  "Shlds", 
								  "Hlds", 
								  "Ore",
								  "Org",
								  "Equ",
								  "Col",
								  "Phot",
								  "Armd",
								  "Lmpt",
								  "Clks",
								  "EPrb",
								  "Aln",
								  "Exp", 
								  "Command [TL=",
								  NULL);

					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;

					do_what2 = StatusWaitThreadResult;

					if (do_what2 == 1)        // Turns
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						if (strcmp(last_line, "Unlimited") == 0)
						{
							numTurns = -1;
						}
						else
						{
							last_line = UNCOMMA(last_line);
							numTurns = atol(last_line);
						}
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 2)   // Credits
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numCredits = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 3)   // Figs
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numFigs = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 4)   // Shields
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numShields = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 5)   // Total Holds
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numTotalHolds = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 6)   // Fuel Ore
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numFuel = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 7)   // Org
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numOrgs = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 8)   // Equip
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numEquip = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 9)   // Colos
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numColos = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 10)  // Photons
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numPhotons = atol(last_line);
						pfree(&last_line);
					}
					else if (do_what2 == 11)  // Armid
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numAMines = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 12)  // Limpet
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numLMines = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 13)  // Cloaks
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numCloaks = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 14)  // E-Probes
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numProbes = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 15)  // Alignment
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numAlign = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
					else if (do_what2 == 16)  // Exp
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, PIPE, "\r", "\n", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						numExp = atol(last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
					}
				}

				numEmptyHolds = numTotalHolds - numFuel - numOrgs - numEquip - numColos;
	/*
				sprintf (data, "numExp: %ld", numExp);
				outputData (data);
				sprintf (data, "numAlign: %ld", numAlign);
				outputData (data);
				sprintf (data, "numTurns: %ld", numTurns);
				outputData (data);
				sprintf (data, "numFigs: %ld", numFigs);
				outputData (data);
				sprintf (data, "numTotalHolds: %ld", numTotalHolds);
				outputData (data);
				sprintf (data, "numFuel: %ld", numFuel);
				outputData (data);
				sprintf (data, "numOrgs: %ld", numOrgs);
				outputData (data);
				sprintf (data, "numEquip: %ld", numEquip);
				outputData (data);
				sprintf (data, "numColos: %ld", numColos);
				outputData (data);
				sprintf (data, "numEmptyHolds: %ld", numEmptyHolds);
				outputData (data);
				sprintf (data, "numShields: %ld", numShields);
				outputData (data);
				sprintf (data, "numAMines: %ld", numAMines);
				outputData (data);
				sprintf (data, "numLMines: %ld", numLMines);
				outputData (data);
				sprintf (data, "numPhotons: %ld", numPhotons);
				outputData (data);
				sprintf (data, "numCloaks: %ld", numCloaks);
				outputData (data);
				sprintf (data, "numProbes: %ld", numProbes);
				outputData (data);
				sprintf (data, "numCredits: %ld", numCredits);
				outputData (data);
				*/
				update_status_bar();
			}
			outputData ("end 2 status");
			statusThreads_ReleaseData();
		}
		else if (do_what == 3 && statusTest[do_what]==1)
		{
			statusThreads_WaitForData(0);
			outputData ("start 3 status");

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "]:[");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "]");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			numSector = atol(last_line);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			if (numSector <=0 || numSector > header.sector_cnt)
			{
				DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector, "Status Thread=3");
				goto skip_to_next_statement_status;
			}

			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector = numSector;
			ReleaseMutex(tempCurrentSectorMutex);

			validDoubleY=0; //reset the blind twarp protection
			commandPrompt=1; //set the prompt back to command
			update_status_bar();
//			sprintf (data, "CURRENT SECTOR %ld", tempCurrentSector);
//			outputData (data);

			if (showmap != NULL && header.autoUpdateMap==1)
			{
				GetClientRect(showmap, &rect);
				sprintf (buff, "%ld", tempCurrentSector);
				SetDlgItemText(showmap, IDC_MAP_SECTOR, buff);
				buildMapDataStructure(showmap, tempCurrentSector, 3, rect.bottom, rect.right);
				InvalidateRect (showmap, &rect, FALSE);
			}
			outputData ("end 3 status");
			statusThreads_ReleaseData();
		}
		else if (do_what == 4 && statusTest[do_what]==1)
		{
			statusThreads_WaitForData(0);
			outputData ("start 4 status");
			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			numSector = atol(last_line);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			if (numSector <=0 || numSector > header.sector_cnt)
			{
				DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector , "Status Thread=4");
				goto skip_to_next_statement_status;
			}

			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector = numSector;
			ReleaseMutex(tempCurrentSectorMutex);

			update_status_bar();
//			sprintf (data, "CURRENT SECTOR %ld", tempCurrentSector);
//			outputData (data);

			if (showmap != NULL && header.autoUpdateMap==1)
			{
				GetClientRect(showmap, &rect);
				sprintf (buff, "%ld", tempCurrentSector);
				SetDlgItemText(showmap, IDC_MAP_SECTOR, buff);
				buildMapDataStructure(showmap, tempCurrentSector, 3, rect.bottom, rect.right);
				InvalidateRect (showmap, &rect, FALSE);
			}
			outputData ("end 4 status");
			statusThreads_ReleaseData();
		}
		else if (do_what == 5 && statusTest[do_what]==1)
		{
			statusThreads_WaitForData(0);
			outputData ("start 5 status");
			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			numSector = atol(last_line);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			if (numSector <=0 || numSector > header.sector_cnt)
			{
				DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector, "Status Thread=5");
				goto skip_to_next_statement_status;
			}

			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector = numSector;
			ReleaseMutex(tempCurrentSectorMutex);

			update_status_bar();
//			sprintf (data, "CURRENT SECTOR %ld", tempCurrentSector);
//			outputData (data);
//			outputData ("end 5 status");
			statusThreads_ReleaseData();
		}
		else if (do_what == 6 && statusTest[do_what]==1)
		{
			statusThreads_WaitForData(0);
			outputData ("start 6 status");
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, " in ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(3);
			last_line = UNCOMMA(last_line);

			numSector = atol(last_line);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			if (numSector <=0 || numSector > header.sector_cnt)
			{
				DISPLAY_UNIVERSE_SIZE_ERROR(numSector, "Status Thread=6");
				goto skip_to_next_statement_status;
			}

			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector = numSector;
			ReleaseMutex(tempCurrentSectorMutex);

			/* set planets, figs, haz, and mines to 0 */
			/* you MUST look at if do_what == 7 and also wipe out the data
			in that case since it is a density scan */
			sectors[tempCurrentSector-1].navhaz=0;
			sectors[tempCurrentSector-1].ftrs=0;
			sectors[tempCurrentSector-1].ftr_type=-1;
			sectors[tempCurrentSector-1].ftr_owner=-1;

			sectors[tempCurrentSector-1].armids=0;
			sectors[tempCurrentSector-1].armid_owner=-1;
			sectors[tempCurrentSector-1].limpets=0;
			sectors[tempCurrentSector-1].limpet_owner=-1;

			//remove the planets from sector
			if (listPlanets[tempCurrentSector - 1] != NULL)
				delete_link_list (tempCurrentSector-1);
	
			writeSetReady();

			breakFlag=0;
			do_what2=0;
			while (breakFlag==0)
			{
				timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Planets : ",
							"Traders : ", 
							"Ships   : ",
							"Fighters: ", 
							"NavHaz  : ",
							"Mines   : ",
							"Ports   : ", 
							"Sector  : ",
							"Warps to Sector(s) : ",
							"Probe entering sector",
							"(Under Construction -",
							NULL);

				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_status;
				do_what2 = StatusWaitThreadResult;

				if (do_what2 ==1)
				{
					/* planets */
					tmpInt=0;
					timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "\r\n");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					result = StatusWaitThreadResult;
					while (result ==1)
					{
						
						last_line = sSCRIPT_LAST_LINE(2);
						if (last_line[0] == 40 || last_line[0] == 32 || tmpInt==0)
						{
							last_line = CLEANLINE(last_line);
	//						sprintf (data, "planets: %s", last_line);
	//						outputData (data);
							if (strcmp(last_line, "")==0)
							{
								goto skip_to_next_statement_status;
							}
							tmp = left (last_line, 70);
//							printf ("planet: %s\n", tmp);
							/* throw planet in the link list */
							insert_link_list_node (tmp, tempCurrentSector-1);
							//only add onto the end of the zocwaitmux since i need the 
							//first 3 parameters in their exact spots

							#ifdef P_FREE
								pfree(&tmp);
								pfree(&last_line);
							#else
								free(tmp);
								free(last_line);
							#endif		

							timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r\n","Planets : ",
										"Traders : ", 
										"Ships   : ",
										"Fighters: ", 
										"NavHaz  : ",
										"Mines   : ",
										"Ports   : ", 
										"Sector  : ",
										"Warps to Sector(s) : ",
										"Probe entering sector",
										"(Under Construction -",
										NULL);
							if (timeout == TIMEOUT_CONSTANT)
								goto skip_to_next_statement_status;
							result = StatusWaitThreadResult;
						}
						else
						{
							/*if we dont see a planet... breakout */
							#ifdef P_FREE
								pfree(&last_line);
							#else
								free(last_line);
							#endif		
							result=-1;
						}
						tmpInt=1;
					}
					//basically throw me outta of parent MUX since i already see the WArps to sector
					do_what2 = result-1;
//					print_link_list(numSector-1);

				}
				if (do_what2 ==2)
				{ 
					// traders
					tmpInt=0;
					returnVal=1;
					while(returnVal==1)
					{
//						timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, " w/ ");
						/* first line of trader data */
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r\n","Planets : ",
									"Traders : ", 
									"Ships   : ",
									"Fighters: ", 
									"NavHaz  : ",
									"Mines   : ",
									"Ports   : ", 
									"Sector  : ",
									"Warps to Sector(s) : ",
									"Probe entering sector",
									"(Under Construction -",
									NULL);

						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;

						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						if (last_line[0] == 40 || last_line[0] == 32 || tmpInt==0)
						{
							last_line = CLEANLINE(last_line);
							//printf ("trader: %s\n", last_line);
							traderNum = parseTradeNumber(last_line);
							#ifdef P_FREE
								pfree(&last_line);
							#else
								free(last_line);
							#endif		
//							printf ("test: %d\n", traderNum);
							/* second line of trader data */
							timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "\r\n");
							if (timeout == TIMEOUT_CONSTANT)
								goto skip_to_next_statement_status;
							returnVal=StatusWaitThreadResult;

							//start TWXSync
							buildSendTWXData_WaitForData(INFINITE);
							dataMessage = buildSendTWXData(traderNum, TRADER_DATA);
							postSendMessageToTWXSync(threadId, strdup(dataMessage));
							#ifdef P_FREE
								pfree(&dataMessage);
							#else
								free(dataMessage);
							#endif		
							buildSendTWXData_ReleaseData();
							//end TWXSync
						}
						else
						{
							/*if we dont see a trader... breakout */
							#ifdef P_FREE
								pfree(&last_line);
							#else
								free(last_line);
							#endif		
							returnVal=-1;
						}
						tmpInt=1;
					}
					do_what2 = StatusWaitThreadResult-1;
				}
				if (do_what2 ==3)
				{
					/* ships */
				}
				if (do_what2 ==4)
				{ 
					/* figs */

					timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "(");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					tempLong = atol(last_line);
//					          sprintf (data, "FIGS: %ld", tempLong);
//							  outputData (data);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
					if (tempLong>=-1 && tempLong <=100000000)
						sectors[tempCurrentSector-1].ftrs=tempLong;

					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "belong to ","Rogue Mer","yours)", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					result = StatusWaitThreadResult;
					if (result==1)
					{
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Corp#","your Corp)",") [", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						result = StatusWaitThreadResult;
						if (result==1)
						{
							timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, ",");
							if (timeout == TIMEOUT_CONSTANT)
								goto skip_to_next_statement_status;
							last_line = sSCRIPT_LAST_LINE(1);
							last_line = UNCOMMA(last_line);
							num = atoi(last_line);
//									  sprintf (data, "CORP %d", num);
//									  outputData (data);
							#ifdef P_FREE
								pfree(&last_line);
							#else
								free(last_line);
							#endif		

							if (CORP_CONSTANT+num>=-10 && CORP_CONSTANT+num <=30000)
								sectors[tempCurrentSector-1].ftr_owner=CORP_CONSTANT+num;
						}
						else if (result==2)
						{
//							sprintf (data, "YOUR CORP");
//							outputData (data);
							if (players[0].corpNumber>=0)
							{
								if (CORP_CONSTANT+players[0].corpNumber>=-10 && CORP_CONSTANT+players[0].corpNumber <=30000)
									sectors[tempCurrentSector-1].ftr_owner=CORP_CONSTANT+players[0].corpNumber;
							}
							else
								sectors[tempCurrentSector-1].ftr_owner=0;
						}
						else if (result==3)
						{
							last_line = sSCRIPT_LAST_LINE(2);

							if (strlen(last_line)>0)
								if (last_line[strlen(last_line)-1]==')')
									last_line[strlen(last_line)-1]=0;

//									  sprintf (data, "USERFIGS: %s", last_line); 
//									  outputData (data);
							traderNum = getTradeNumber(last_line);
							#ifdef P_FREE
								pfree(&last_line);
							#else
								free(last_line);
							#endif		
//									  sprintf (data, "TRADERNUM: %d",traderNum); 
//									  outputData (data);
							if (traderNum>=-10 && traderNum <=10000)
								sectors[tempCurrentSector-1].ftr_owner=traderNum;
						}
					}
					else if (result==2)
					{
//							      sprintf (data, "ROGUE MERCENARY");
//								  outputData (data);
						sectors[tempCurrentSector-1].ftr_owner=1;
									
					}
					else
					{
//							      sprintf (data, "MINE");
//								  outputData (data);
						sectors[tempCurrentSector-1].ftr_owner=0;
					}

					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Toll]","Offensive]","Defensive]", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;

					result = StatusWaitThreadResult;
//					sprintf (data, "TYPE: %d", result);
//					outputData (data);
//					sprintf (data, "tempCurrentSector: %ld", tempCurrentSector);
//					outputData (data);

					if (result>=-10 && result <=10)
						sectors[tempCurrentSector-1].ftr_type=result;
					writeSetReady();

				}
				if (do_what2 ==5)
				{ 
					/* navhaz */
					timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "%");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					num = atoi(last_line);
//			        sprintf (data, "HAZ: %d", num);
//					outputData (data);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
					if (num>=-10 && num <=100)
						sectors[tempCurrentSector-1].navhaz=num;
					writeSetReady();
				}
				if (do_what2 ==6)
				{
					/* mines */
					timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "(");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					num = atoi(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Armid","Limpet", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					returnVal = StatusWaitThreadResult;
					if (returnVal==1)
					{
						if (num>=-10 && num <=10000)
							sectors[tempCurrentSector-1].armids=num;
						writeSetReady();
//						sprintf (data, "A MINES: %d", num);
//						outputData (data);
						timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "        : ","Warps to Sector", NULL);
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;

						result = StatusWaitThreadResult;
						if (result==1)
						{
							timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "(");
							if (timeout == TIMEOUT_CONSTANT)
								goto skip_to_next_statement_status;
							last_line = sSCRIPT_LAST_LINE(1);
							last_line = UNCOMMA(last_line);
							num = atoi(last_line);	
//				            sprintf (data, "L1 MINES: %d", num);
//							outputData (data);
							#ifdef P_FREE
								pfree(&last_line);
							#else
								free(last_line);
							#endif		
							if (num>=-10 && num <=10000)
								sectors[tempCurrentSector-1].limpets=num;
							writeSetReady();
						}
						if (result==2)
						{
//						          sprintf (data, "L1 MINES: 0");
//								  outputData (data);
						  goto skip_to_next_statement_status;
						}
					}
					if (returnVal==2)
					{
						if (num>=-10 && num <=10000)
							sectors[tempCurrentSector-1].limpets=num;
						writeSetReady();
					}
				}
				if (do_what2==7)
				{
					hasPortSector=1;

					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Sol,","Rylos,","Alpha Centauri,","Stargate Alpha I","<=-DANGER-=>",", Class", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					result = StatusWaitThreadResult;

					if (result==1)
					{
						sprintf (data, "SOL: %ld", tempCurrentSector);
						outputData(data);

						if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
						{
							sectors[tempCurrentSector-1].info=0;
							sectors[tempCurrentSector-1].port_update = ATTAC_Time();
							header.cls0port_sol = tempCurrentSector;
							writeSetReady();
						}
					}
					else if(result==2)
					{
						sprintf (data, "rylos: %ld", tempCurrentSector);
						outputData(data);

						if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
						{
							sectors[tempCurrentSector-1].info=0;
							sectors[tempCurrentSector-1].port_update = ATTAC_Time();
							header.cls0port_rylos = tempCurrentSector;
							writeSetReady();
						}
					}	     
					else if (result==3)
					{
  						sprintf (data, "alpha: %ld", tempCurrentSector);
						outputData(data);
						if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
							{
							sectors[tempCurrentSector-1].info=0;
							sectors[tempCurrentSector-1].port_update = ATTAC_Time();
							header.cls0port_alpha = tempCurrentSector;
							writeSetReady();
						}
					}
					else if (result==4)
					{
						sprintf (data, "sd: %ld", tempCurrentSector);
						outputData(data);
						if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
						{
							sectors[tempCurrentSector-1].info=9;
							sectors[tempCurrentSector-1].port_update = ATTAC_Time();
							header.stardock = tempCurrentSector;
							writeSetReady();
						}
					}
					else if (result==5)
					{
						sprintf (data, "destroyed port %ld", tempCurrentSector);
						outputData(data);
						if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
						{
							sectors[tempCurrentSector-1].info=12;
							sectors[tempCurrentSector-1].port_update = ATTAC_Time();
							writeSetReady();
						}
					}
					else if (result==6)
					{
						timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, ")");
						if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
						last_line = sSCRIPT_LAST_LINE(1);
						last_line = UNCOMMA(last_line);
						num = atoi(last_line);
						sprintf (data, "SECTOR: %ld --> PORT: %d", tempCurrentSector, num);
						outputData(data);
						if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
						{
							if (num>=-10 && num <=500)
							{
								sectors[tempCurrentSector-1].info=num;
							}
							sectors[tempCurrentSector-1].port_update = ATTAC_Time();
							writeSetReady();
						}

						#ifdef P_FREE
							pfree(&last_line);           // free present block        
						#else
							free(last_line);           // free present block        
						#endif		

					}


				}
				if (do_what2==8)
				{
					timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, " in ");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					numSector = atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif		
					if (numSector <= 0 || numSector > header.sector_cnt)
					  goto skip_to_next_statement_status;

					WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
					tempCurrentSector = currentSector = numSector;
					ReleaseMutex(tempCurrentSectorMutex);

					/* now wipe out the data for that sector so we get new data */
					sectors[tempCurrentSector-1].ftrs=-1;
					sectors[tempCurrentSector-1].ftr_type=-1;
					sectors[tempCurrentSector-1].ftr_owner=-1;
					sectors[tempCurrentSector-1].armids=-1;
					sectors[tempCurrentSector-1].armid_owner=-1;
					sectors[tempCurrentSector-1].limpets=-1;
					sectors[tempCurrentSector-1].limpet_owner=-1;
					sectors[tempCurrentSector-1].navhaz=-1;

					//remove the planets from sector
					if (listPlanets[tempCurrentSector - 1] != NULL)
						delete_link_list (tempCurrentSector-1);

					writeSetReady();
				}	
				if (do_what2==9) {
					breakFlag=1;
					timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r", "\n", NULL);
					if (timeout == TIMEOUT_CONSTANT)
							goto skip_to_next_statement_status;
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					strcpy(tempBuffer, last_line);

					#ifdef P_FREE
						pfree(&last_line);           // free present block        
					#else
						free(last_line);           // free present block        
					#endif		

					if (tempCurrentSector>0 && tempCurrentSector <=header.sector_cnt)
					{
						warpCount=0;
						token = strtok(tempBuffer,"-");
						while(token != NULL && strlen(token)>0)
						{
							if (token[0]=='(')
								{              //8 is len of strSector
								for (i=1;i<(int)strlen(token)-1 && i<8; i++)
									strSector[i-1]=token[i];
								strSector[i-1]=0;
								}
							else
								strcpy(strSector, token);

							sprintf (data, "sector %s", strSector);
							outputData(data);
							if (tempCurrentSector<=0 ||  tempCurrentSector>header.sector_cnt)
							{
								DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector, "status thread Thread=6a");
							}
							else if (atol(strSector)<=0 || atol(strSector) >header.sector_cnt)
							{
								DISPLAY_UNIVERSE_SIZE_ERROR(atol(strSector), "status thread Thread=6b");
							}
							else if (warpCount<0 || warpCount>5)
							{
								DISPLAY_UNIVERSE_SIZE_ERROR((long)warpCount, "status thread Thread=6c");
							}
							else
							{
						//		printf ("sector %ld has warp #%d to %ld\n", tempCurrentSector, warpCount, atol(strSector));
								sectors[tempCurrentSector-1].warp_sect[warpCount]=atol(strSector);
							}
							//move to next sector
							warpCount++;
							token = strtok(NULL,"-");
						}	  
						if (atol(strSector) <= 0 || atol(strSector) >header.sector_cnt)
						{
							DISPLAY_UNIVERSE_SIZE_ERROR(atol(strSector), "status thread Thread=6c");
						}
						else
						{
		//					printf ("warpCount: %d\n", warpCount);
							if (warpCount>=-10 && warpCount <=6)
								sectors[tempCurrentSector-1].warps = warpCount;
							sectors[tempCurrentSector-1].sector_update = ATTAC_Time();
							//say that the sector has no port
							if (sectors[tempCurrentSector-1].info==-1)
								sectors[tempCurrentSector-1].info=10;
						}
					}
				}
				else if(do_what2==11)
				{
					WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
					tempCurrentSector = currentSector;
					ReleaseMutex(tempCurrentSectorMutex);

					if (tempCurrentSector <=0 || tempCurrentSector >header.sector_cnt)  //check universe size
					{
						DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector, "k");
						goto skip_to_next_statement_status;
					}
					if (tempCurrentSector >0 && tempCurrentSector <=header.sector_cnt)
					{
						sectors[tempCurrentSector-1].info=sectors[tempCurrentSector-1].info+UNDER_CONSTRUCTION;
						sectors[tempCurrentSector-1].port_update = ATTAC_Time();

						writeSetReady();
					}
		//			sprintf (data, "PORT %ld IS REALLY UNDER CONSTRUCTION AND IS: %d", tempCurrentSector, sectors[tempCurrentSector-1].info);
		//			outputData(data);
				}	      
			}	//end while of do_what2==9


			//start TWXSync
			buildSendTWXData_WaitForData(INFINITE);
			dataMessage = buildSendTWXData(tempCurrentSector-1, SECTOR_DATA);
			postSendMessageToTWXSync(threadId, strdup(dataMessage));
			#ifdef P_FREE
				pfree(&dataMessage);
			#else
				free(dataMessage);
			#endif		
			buildSendTWXData_ReleaseData();
			//end TWXSync

			if (hasPortSector==0)
			{
				if (tempCurrentSector > 0 && tempCurrentSector <= header.sector_cnt)
					sectors[tempCurrentSector-1].info=10;

				//start TWXSync
				buildSendTWXData_WaitForData(INFINITE);
				dataMessage = buildSendTWXData(tempCurrentSector-1, PORT_DATA);
				postSendMessageToTWXSync(threadId, strdup(dataMessage));
				#ifdef P_FREE
					pfree(&dataMessage);
				#else
					free(dataMessage);
				#endif		
				buildSendTWXData_ReleaseData();
				//end TWXSync
			}
			outputData ("end 6 status");
			statusThreads_ReleaseData();
		}
		//we only put there here since the parsedata thread is already parsing
		//the incoming transmission so this is missed if we place it there
		else if(do_what==7 && statusTest[do_what]==1)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 7 status");
			// the newline stuff is to make sure we get kicked out of this wait
			// if it really isnt figs
			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " shield points and "," fighters.","\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			result = StatusWaitThreadResult;
			if (result==1)
			{
				last_line = sSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				//subtract that number of figs from the amount
				numShields -= atol(last_line);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif
				timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " fighters.","\r", "\n", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_status;
				result = StatusWaitThreadResult;
				if (result==1)
				{
					last_line = sSCRIPT_LAST_LINE(1);
					last_line = UNCOMMA(last_line);
					//subtract that number of figs from the amount
					numFigs -= atol(last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif
				}
				result=0;
			}
			else if (result==2)
			{
				last_line = sSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				//subtract that number of figs from the amount
				numFigs -= atol(last_line);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif
			}
			outputData ("end 7 status");
			update_status_bar();
			statusThreads_ReleaseData();
		}  
		else if(do_what==8 && statusTest[do_what]==1)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 8 status");
			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Hello ", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, ",");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			returnVal=0;
			startParsing=1;
			if (validRegCode==1)
			{
				returnVal = isValidRegCode("","","","","", "", 1, last_line);
				if (returnVal==2) {
					NonRegisteredREXXCount=-1;

					if (twxThreadStarted==0 && validTWXSyncRegCode==1) {
						if(_beginthread(twxSyncThreadProc, 0, NULL )==-1)
						{
							printf("Failed to create thread");
							return;
						}
					} else if (validTWXSyncRegCode==0 && strcmp(header.TWXSyncSession, "") !=0) {
						attacErrorMessage("You must obtain a seperate twxSync reg code to enable realtime data synchronization with TWX Sync. Please visit the ATTAC website for more information.");
					}
				}
			} else if (validTWXSyncRegCode==0 && strcmp(header.TWXSyncSession, "") !=0) {
				attacErrorMessage("TWX Sync will not work without a registration code. Please visit the ATTAC website for more information.");
				attacErrorMessage("http://www.tw-attac.com");
			}

			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif
			outputData ("end 8 status");
			statusThreads_ReleaseData();
		}
		else if (do_what==9 && statusTest[do_what]==1)
		{
			statusThreads_WaitForData(0);
			if (isRegistered==1)
				attac_send_char('#');
			statusThreads_ReleaseData();
		}
		else if (do_what==10 && statusTest[do_what]==1)
		{
			statusThreads_WaitForData(0);
			outputData ("start 10 status\n");
			//removed
			outputData ("end 10 status\n");
			statusThreads_ReleaseData();
		}
		else if(do_what==11 && header.autoHaggle==1 && REXX_RunningScript==0 && runningTWXScript==0 && statusTest[do_what]==1)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 11 status");

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Your offer [");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;		
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "]");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;

			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
//			sprintf (data, "2amount: %s", last_line);
//			outputData (data);
			timeout = buyHaggle(atol(last_line), 0);
//			if (timeout==640)
//			{
//				sprintf (data, "timed out");
//				outputData (data);
//			}
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif
			outputData ("end 11 status");
			statusThreads_ReleaseData();
		}	      
		else if(do_what==12 && header.autoHaggle==1 && REXX_RunningScript==0 && runningTWXScript==0 && statusTest[do_what]==1)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 12 status");

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Your offer [");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;		
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "]");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;

			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
//			sprintf (data, "2amount: %s", last_line);
//			outputData (data);
			timeout = buyHaggle(atol(last_line), 0);
//			if (timeout==640)
//			{
//				sprintf (data, "timed out");
//				outputData (data);
//			}
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif
			outputData ("end 12 status");
			statusThreads_ReleaseData();
		}	      
		else if(do_what==13 && header.autoHaggle==1 && REXX_RunningScript==0 && runningTWXScript==0  && statusTest[do_what]==1)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 13 status");

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Your offer [");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;		
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "]");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;

			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
//			sprintf (data, "2amount: %s", last_line);
//			outputData (data);
			timeout = buyHaggle(atol(last_line), 0);
//			if (timeout==640)
//			{
//				sprintf (data, "timed out");
//				outputData (data);
//			}
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif
			outputData ("end 13 status");
			statusThreads_ReleaseData();
		}	      
		else if(do_what==14 && header.autoHaggle==1 && REXX_RunningScript==0 && runningTWXScript==0  && statusTest[do_what]==1)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 14 status");

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Your offer [");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;		
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "]");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;

			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
//			sprintf (data, "2amount: %s", last_line);
//			outputData (data);
			timeout = sellHaggle(atol(last_line), 0);
//			if (timeout==640)
//			{
//				sprintf (data, "timed out");
//				outputData (data);
//			}
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif
			outputData ("end 14 status");
			statusThreads_ReleaseData();
		}	      
		else if(do_what==15 && header.autoHaggle==1 && REXX_RunningScript==0 && runningTWXScript==0  && statusTest[do_what]==1)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 15 status");

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Your offer [");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;		
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "]");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;

			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
//			sprintf (data, "2amount: %s", last_line);
//			outputData (data);
			timeout = sellHaggle(atol(last_line), 0);
//			if (timeout==640)
//			{
//				sprintf (data, "timed out");
//				outputData (data);
//			}

			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif
			outputData ("end 15 status");
			statusThreads_ReleaseData();
		}	      
		else if(do_what==16 && header.autoHaggle==1 && REXX_RunningScript==0 && runningTWXScript==0  && statusTest[do_what]==1)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 16 status");

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Your offer [");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;		
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "]");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;

			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
//			sprintf (data, "2amount: %s", last_line);
//			outputData (data);
			timeout = sellHaggle(atol(last_line), 0);
//			if (timeout==640)
//			{
//				sprintf (data, "timed out");
//				outputData (data);
//			}
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif
			outputData ("end 16 status");
			statusThreads_ReleaseData();
		}	      
		else if(do_what==17)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 17 status");
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Admins online");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;		

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "Selection");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;		
			
			//special for TWGS admin log ons. if the ID is valid, then remove the rexx restriction
			if (validRegCode)
			{
				NonRegisteredREXXCount=-1;
			}
			outputData ("end 17 status");
			statusThreads_ReleaseData();
		}
		else if (do_what ==18)  // commerce port for 
		{  
			statusThreads_WaitForData(0);
			// make sure ship name withbuying in it doesnt screw us up 
			outputData ("start 18");
			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector;
			ReleaseMutex(tempCurrentSectorMutex);

			if (tempCurrentSector <= 0 || tempCurrentSector >header.sector_cnt)  //check universe size
			{
				DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector, "h");
				goto skip_to_next_statement_status;
			}

			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Buying       ","Buying      ","Buying     ","Buying    ","Buying   ","Buying  ", "Selling      ","Selling     ","Selling    ","Selling   ","Selling  ","Selling ", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "    ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);      
			tempLong = atol(last_line);
			if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
			{
				if (tempLong>=-10 && tempLong <=1000000)
					sectors[tempCurrentSector-1].port_amt[0] = tempLong;
				writeSetReady();
			}
//			sprintf (data, "FUEL AMOUNT: %ld", tempLong);
//			outputData(data);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "%");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			num = atoi(last_line);
			if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
			{
				if (num>=-10 && num <=100)
					sectors[tempCurrentSector-1].port_per[0] = num;
				writeSetReady();
			}
//					sprintf (data, "FUEL PERCENT: %d", num);
//					outputData(data);

			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Buying       ","Buying      ","Buying     ","Buying    ","Buying   ","Buying  ", "Selling      ","Selling     ","Selling    ","Selling   ","Selling  ","Selling ", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "    ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			tempLong = atol(last_line);
			if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
			{
				if (tempLong>=-10 && tempLong <=1000000)
					sectors[tempCurrentSector-1].port_amt[1] = tempLong;
				writeSetReady();
			}
//			  sprintf (data, "ORG AMOUNT: %ld", tempLong);
//			  outputData(data);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "%");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			num = atoi(last_line);
			if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
			{
				if (num>=-10 && num <=100)
					sectors[tempCurrentSector-1].port_per[1] = num;
				writeSetReady();
			}
//				sprintf (data, "Org PERCENT: %d", num);
//				outputData(data);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Buying       ","Buying      ","Buying     ","Buying    ","Buying   ","Buying  ","Selling      ","Selling     ","Selling    ","Selling   ","Selling  ","Selling ", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "    ");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			tempLong = atol(last_line);
			if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
			{
				if (tempLong>=-10 && tempLong <=1000000)
					sectors[tempCurrentSector-1].port_amt[2] = tempLong;
				writeSetReady();
			}
//			  sprintf (data, "EQUIP AMOUNT: %ld", tempLong);
//			  outputData(data);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "%");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			last_line = sSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			num = atoi(last_line);
			if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
			{
				if (num>=-10 && num<=100)
					sectors[tempCurrentSector-1].port_per[2] = num;
				writeSetReady();
			}
//				  sprintf (data, "EQUIP PERCENT: %d", num);
//				  outputData(data);

			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif		

			//start TWXSync
			buildSendTWXData_WaitForData(INFINITE);
			dataMessage = buildSendTWXData(tempCurrentSector-1, PORT_DATA);
			postSendMessageToTWXSync(threadId, strdup(dataMessage));
			#ifdef P_FREE
				pfree(&dataMessage);
			#else
				free(dataMessage);
			#endif		
			buildSendTWXData_ReleaseData();
			//end TWXSync


			outputData ("end 18");
			statusThreads_ReleaseData();
		}
		else if(do_what==19)
		{
			statusThreads_WaitForData(0);
			outputData ("start 19");
			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector;
			ReleaseMutex(tempCurrentSectorMutex);

//			sprintf (data, "busted: %ld", tempCurrentSector);
//			outputData(data);
			if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
			{
				sectors[tempCurrentSector-1].playerIdBusted=0;
				sectors[tempCurrentSector-1].lastBusted=ATTAC_Time();
				header.lastBustedSector = tempCurrentSector;
				header.lastRobbedSector = tempCurrentSector;
			}

			//start TWXSync
			buildSendTWXData_WaitForData(INFINITE);
			dataMessage = buildSendTWXData(tempCurrentSector-1, SECTOR_DATA);
			postSendMessageToTWXSync(threadId, strdup(dataMessage));
			#ifdef P_FREE
				pfree(&dataMessage);           // free present block        
			#else
				free(dataMessage);           // free present block        
			#endif		
			buildSendTWXData_ReleaseData();
			//end TWXSync

			outputData ("end 19");
			statusThreads_ReleaseData();
		}	      
		else if(do_what==20)
		{
			statusThreads_WaitForData(0);
			outputData ("start 20");
			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector;
			ReleaseMutex(tempCurrentSectorMutex);

			if (tempCurrentSector <=0 || tempCurrentSector >header.sector_cnt)  //check universe size
			{
				DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector, "l");
				goto skip_to_next_statement_status;
			}

			//undo the GUI variables when you port
			buyingSelling = 0;
			buyingSellingAmount = 0;
			// check to make sure last bust isnt this port
			if (tempCurrentSector==header.lastBustedSector)
			{
//				sprintf (data, "JUST BUSTED IN SECTOR %ld. DONT PORT HERE", tempCurrentSector);
//				outputData(data);
				//attacErrorMessage("You were just Busted Here!!!");
				MessageBox(NULL, "You were just Busted Here!!!", "Warning", MB_OK);
			}
			else
			{
				for (i=0; i<header.sector_cnt; i++)
				{
					if ((sectors[i].lastBusted>0 && i==tempCurrentSector-1 && sectors[tempCurrentSector-1].playerIdBusted==0))
					{
//						sprintf(data, "DONT PORT HERE!@!@!@ THIS PORT IS ON YOUR BUST LIST");
//						outputData(data);
						//attacErrorMessage("DONT PORT HERE!@!@!@ THIS PORT IS ON YOUR BUST LIST");
						if (REXX_RunningScript==0 && runningTWXScript==0)
							MessageBox(NULL, "This Port is on Your Bust List!!!", "Warning", MB_OK);
						else
						{
							tempString = (char *) strdup("This Port is on Your Bust List!!!");
						    attacScriptMessage("ATTAC SCRIPT", tempString, ANSI_RED, BLACK, TRUE);
						#ifdef P_FREE
							pfree(&tempString);
						#else
							free(tempString);
						#endif
						}
						goto skip_to_next_statement_status;
					}
				}
			}

			outputData ("end 20");
			statusThreads_ReleaseData();
		}
		else if(do_what==21 && passToREXX==1)
		{    
			statusThreads_WaitForData(0);
			// check to make sure you dont try to rob a port on bust list
			outputData ("start 21");
			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector;
			ReleaseMutex(tempCurrentSectorMutex);

			if (tempCurrentSector >0 && tempCurrentSector <=header.sector_cnt)
			{
				if (tempCurrentSector==header.lastRobbedSector)
				{
					if (REXX_RunningScript==0 && runningTWXScript==0)
						MessageBox(NULL, "You just Robbed the Port Here!!!", "Warning", MB_OK);
					else
					{
						tempString = (char *) strdup("You just Robbed the Port Here!!!");
						attacScriptMessage("ATTAC SCRIPT", tempString, ANSI_RED, BLACK, TRUE);
						#ifdef P_FREE
							pfree(&tempString);
						#else
							free(tempString);
						#endif
					}

				}
				else
				{
					for (i=0; i<header.sector_cnt; i++)
					{
						if ( isBustedSector(i+1) && i==tempCurrentSector-1 && sectors[tempCurrentSector-1].playerIdBusted==0)
						{
//							sprintf(data, "DONT PORT HERE!@!@!@ THIS PORT IS ON YOUR BUST LIST");
//							outputData(data);
							//attacErrorMessage("DONT PORT HERE!@!@!@ THIS PORT IS ON YOUR BUST LIST");
							if (REXX_RunningScript==0 && runningTWXScript==0)
								MessageBox(NULL, "This Port is on Your Bust List!!!", "Warning", MB_OK);
							else
							{
								tempString = (char *) strdup("This Port is on Your Bust List!!!");
								attacScriptMessage("ATTAC SCRIPT", tempString, ANSI_RED, BLACK, TRUE);
								#ifdef P_FREE
									pfree(&tempString);
								#else
									free(tempString);
								#endif
							}							
							goto skip_to_next_statement_status;
						}
					}
				}
			}
			outputData ("end 21");
			statusThreads_ReleaseData();
		}	      
		else if((do_what==22 && passToREXX==1) || (do_what==23 && passToREXX==1))
		{   
			statusThreads_WaitForData(0);
			outputData ("start 22,23");
			
			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector;
			ReleaseMutex(tempCurrentSectorMutex);

			if (do_what==22)
			{
//				sprintf (data, "CORP PLANET SCAN - not parsed yet");
//				outputData(data);
			}
			else if (do_what==23)
			{
//				sprintf (data, "PERSONAL PLANET SCAN - not parsed yet");
//				outputData(data);
			}

			timeout = sSCRIPT_WAIT(TIMEOUT_AMOUNT, "======================================================================");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_status;
			//set the last sector scanned var
			tempInt=0;
			tempString = (char*) malloc(1000);
//FIX			//	while (TRUE)
			while (FALSE)
			{
				timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\n","Computer", "Command", "Corporate", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_status;
				result1 = parseDataWaitThreadResult;
				if (result1 != 1)
					goto skip_to_next_statement_status;
				
				timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Class","Computer","Command", "Corporate", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_status;
				result1 = parseDataWaitThreadResult;
				if (result1 != 1)
					goto skip_to_next_statement_status;	    

				last_line = sSCRIPT_LAST_LINE(1);
				strcpy (tempBuffer, last_line);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif

				token = strtok(tempBuffer," ");
				i=0;
				tempLong=0;
				strcpy (tempString, "");
				while(token != NULL)
				{
					if (i>1)
						goto skip_to_next_statement_status;
					//should never get in here more than twice since we only
					//parse two bits of data
					if (i==0)
						tempLong = atol(token);
					else if (i==1)
						strcpy(tempString, token);

					//if planet has a transporter.. bypass it
					if (i==1 && token[0]=='T')
					{
						token = strtok(NULL," ");
						strcpy(tempString, token);
					}
					token = strtok(NULL," ");
					i++;
				}

				//clear the sector of planets if it is the first planet in sector
				if (tempLong != tempInt && tempLong>0 )
				{
					//remove the planets from sector
					if (listPlanets[tempCurrentSector - 1] != NULL)
						delete_link_list (tempCurrentSector-1);
				}
				timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Level 5", "Level 6", "No Citadel","Computer","Command", "Corporate", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_status;
				result1 = parseDataWaitThreadResult;
				if (result1==1 || result1 == 2)
				{
					//		printf ("CITADEL\n");
//					tempString = g_strchug(tempString);
//					token = g_strconcat ("<<<< (?) ", tempString, " >>>> (Shielded)", NULL);
//					strcpy (tempString, token);
//					pfree(&token);
				}
				else if (result1 >=4 && result1 <=6)
					goto skip_to_next_statement_status;

				// throw planet in the link list
				if (tempLong>0 || (strcmp(tempString, "") != 0) )
				{
//					strcpy(tempBuffer, tempString);
					//		printf ("Adding planet %s to sector %d to DB\n", tempBuffer, tempLong);
//					listPlanets[tempLong-1] = 
//					g_slist_append (listPlanets[tempLong-1], tempBuffer);
				}
				timeout = sSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\n","Computer", "Command", "Corporate", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_status;
				result1 = parseDataWaitThreadResult;
				if (result1 != 1)
					goto skip_to_next_statement_status;
			}

			#ifdef P_FREE
				pfree(&tempString);
			#else
				free(tempString);
			#endif
		

			//	printf ("OUT\n");
			outputData ("end 22,23");
			statusThreads_ReleaseData();
		}
		else if(do_what==24 && passToREXX==1)
		{   
			statusThreads_WaitForData(0);
			outputData ("start 24");

			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector;
			ReleaseMutex(tempCurrentSectorMutex);

			if (tempCurrentSector<=0 ||  tempCurrentSector>header.sector_cnt)
			{
				DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector, "status thread Thread=24");
			}
			else
				header.lastRobbedSector = tempCurrentSector;

			outputData ("end 24");
			statusThreads_ReleaseData();
		}

		else if(do_what==25 && parserTest[do_what]==1)
		{   
			statusThreads_ReleaseData(0);
			outputData ("start 25");

			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector;
			ReleaseMutex(tempCurrentSectorMutex);

			sprintf (tempBuffer, "You just picked up a limpit mine in sector %ld. Go to a Class 0 Port or to Stardock to remove it", tempCurrentSector);
			attacScriptMessage("ATTAC INFO", tempBuffer, ANSI_YELLOW, BLACK, TRUE);

			outputData ("end 25");
			statusThreads_ReleaseData();
		}



	skip_to_next_statement_status:
		
		statusThreads_ReleaseData();
	}
	attacErrorMessage("STATUS THREAD HAS ENDED. Please report this error to tradewars@hotmail.com");
	CloseHandle(StatusWaitEvent);

	_endthread();
}

char * removeCRs(char *str)
{
	char *buffer=NULL;
	char *t=NULL;
	int i, j=0;

	WaitForSingleObject(RemoveCRsMutex, INFINITE);
	buffer = (char *)malloc (strlen(str)+1);

	for (i=0; i < (int) strlen (str); i++)
	{
		if (str[i]!=10 && str[i] != 13)
			buffer[j++]=str[i];
	}

	buffer[j]=0;
	t = (char *) strdup (buffer);
	#ifdef P_FREE
		pfree(&buffer);
		pfree(&str);
	#else
		free(buffer);
		free(str);
	#endif

	ReleaseMutex(RemoveCRsMutex);
	return (t);
}

long sSCRIPT_WAIT(long timeout, char arg[])
{
	long returnV;
	returnV = sSCRIPT_WAIT_MUX(timeout, arg, NULL);
	return returnV;
}

long sSCRIPT_WAIT_MUX(long timeout, ...)
{
	va_list ap;
	char *p=NULL;
	int Found, I, pos;
	long tempLong;
	long returnVal;

	StatusWaitCount=0;

	va_start (ap, timeout);
	while ((p = va_arg(ap, char *)) != NULL)
	{
		strcpy(StatusWaitArray[StatusWaitCount], p);
		StatusWaitCount++;
	}
	va_end (ap);

    pos = strlen(DSM_BufferStatus);

    Found = -1;


	for (I = 0; I< StatusWaitCount; I++)
	{
	    tempLong = Instr(1, DSM_BufferStatus, StatusWaitArray[I]);

        if ((tempLong != 0) && (tempLong < pos))
		{
			pos = tempLong;
            Found = I;
		}
	}

    if (Found != -1)
	{
		Update_LastLine_Status(Instr(1, DSM_BufferStatus, StatusWaitArray[Found]) + strlen(StatusWaitArray[Found]) - 1);
    	StatusWaitThreadResult=Found+1;
		SetEvent(StatusWaitEvent);
		return 0;
    }

	//reset the event state to non-signaled
	ResetEvent(StatusWaitEvent);

	//tell the main thread to continue parsing
	returnVal = WaitForSingleObject(StatusWaitEvent, timeout);
	
	if (returnVal==WAIT_TIMEOUT)
	{
//		printf ("!!! STATUS THREAD TIMEOUT !!!\n");
//		sprintf (data, "!!! STATUS THREAD TIMEOUT !!!");
//		outputData (data);
		return 640;
	}

	return 1;
}

char* sSCRIPT_LAST_LINE(int deletion)
{
	char *tmpPointer=NULL;
	if (strlen (StatusLastLine)==0)
		tmpPointer = (char*) strdup (" ");
	else
	{
		if ((int) strlen(StatusLastLine)>= deletion)
			StatusLastLine[strlen(StatusLastLine) - deletion] = 0;
		tmpPointer = (char*) strdup (StatusLastLine);
	}
	return (tmpPointer);
}

//UNCOMMA defined in parsedatathread.c


void Update_LastLine_Status(long pos)
{
	char *tmp=NULL;

	if (pos>=MAX_LENGTH-1)
		tmp = left(DSM_BufferStatus, MAX_LENGTH);
	else
		tmp = left(DSM_BufferStatus, pos);
	strcpy (StatusLastLine, tmp);
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif

	if (strlen(DSM_BufferStatus) - pos>=MAX_LENGTH)
		tmp = right(DSM_BufferStatus, MAX_LENGTH);
	else
		tmp = right(DSM_BufferStatus, strlen(DSM_BufferStatus) - pos);
    
	strcpy (DSM_BufferStatus, tmp);
		
	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif
}

long statusThreads_WaitForData(long timeout)
{
	int result;

	if (timeout == 0)
		timeout = 50000;
	result = WaitForSingleObject(statusThreadsMutex, timeout);

	return result;
}

long statusThreads_ReleaseData()
{
	int result;

	result = ReleaseMutex(statusThreadsMutex);

	return result;
}
