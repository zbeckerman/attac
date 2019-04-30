// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>


#include "attacGlobal.h"
#include "../PUTTY.H"
#include "util.h"
#include "statusthread.h"
#include "parsedatathread.h"
#include "rexx.h"
#include "structs.h"
#include "linklist.h"
#include "info.h"
#include "attacGUI.h"
#include "attacUtils.h"
#include "win_res.h"
#include "remoteConnection.h"
#include "writeMemory.h"
#include "twxSync.h"


HANDLE tempCurrentSectorMutex;
HANDLE REXXThreadsMutex;
HANDLE parseThreadsMutex;
HANDLE ParseDataWaitEvent;
int parseDataWaitCount=0;
char parseDataWaitArray[MAX_LENGTH][MAX_AMOUNT];
char parseDataLastLine[MAX_LENGTH];
int parseDataWaitThreadResult=0;
int debugMe=1;
int remote=0;
int waitForParseDataMutex=0;
char data[MAX_LENGTH];
int parserTest[LIMPIT_COUNT_MAX];

char *dataMessage = NULL;

// this function is called by a new thread
void ParseDataThreadProc( void *dummy )
{
	int result, result1, do_what;
	char *last_line=NULL;
	int i, j, tmpInt;
	char *token=NULL;
	char *from=NULL;
	long tempVariable;
	long tempVariable1;
	char *tempString=NULL;
	char tempBuffer[MAX_LENGTH*2];
	char announcement[100], tChar[1];
	long timeout;
	long limpitList[LIMPIT_COUNT_MAX];
	long tempCurrentSector=0;
	long tempLong=0;
	HWND hlist;
	FILE *fp=NULL;

	sprintf (announcement, "ATTAC TCP/IP Helper (%s) http://www.tw-attac.com", ver);
	
	ParseDataWaitEvent = CreateEvent(NULL, 0, 0, NULL);

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

	while (1)
	{
			timeout = pSCRIPT_WAIT_MUX(INFINITE,
						"StarDock is located in sector", 
						"Your fighters: ",
						"One turn deducted",
						"you receive",
						"you LOSE",
						"alignment went down by",
						"Incoming transmission from ",
						"Continuing transmission from ",
						"Deployed  Fighter  Scan",
						"You have ",
						"You lost ",
						"Fuel Ore do you want to buy",
						"Organics do you want to buy",
						"Equipment do you want to buy",
						"Fuel Ore do you want to sell",
						"Organics do you want to sell",
						"Equipment do you want to sell",
						"Agreed,",
						"empty cargo",
						"want to jettison all cargo? (Y/N)",
						"Still interested ?",
						"credits, interested?",
						"Stop in this sector",
						"TransWarp Locked.",
						"clear ALL avoids? (Y/N)",
						"time/date stamp? (Y/N)",
						"trade ships with you?",
						"engage a Cloaking Device?",
						"Citadel command (?=help)",
						"<StarDock> Where to?",
						"Planet command (?=help)",
						"ARE YOU SURE CAPTAIN? (Y/N)",
						"No locating beam found for sector",
						"<Quit>",
						"Trade Wars 2002 Trader Rankings",
						"Activated  Limpet  Scan",
						"will now be avoided",
						"Avoided sectors Cleared.",
						" has been cleared and ",
						"have marked to Avoid>",
						"<List Avoided Sectors>",
						"ENDINTERROG",
						NULL);
		
			
		do_what  = result = parseDataWaitThreadResult;
	//	printf ("parseData: %d\n", do_what);
		if (do_what == 1 && passToREXX==1 && parserTest[do_what]==1)
		{
			parseThreads_WaitForData(0);
			outputData("start 1");

			timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, ".");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;

			last_line = pSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			tempLong = atol(last_line);

			  sprintf (data, "SD1: %ld", tempLong);
			  outputData(data);

			if (tempLong>0 &&  tempLong <=header.sector_cnt)
			{
				sectors[tempLong-1].info=9;
				header.stardock = tempLong;
				sectors[tempLong-1].sector_update = ATTAC_Time();
				writeSetReady();
			}

			#ifdef P_FREE
				pfree(&last_line);           // free present block        
			#else
				free(last_line);           // free present block        
			#endif		

			outputData("end 1");
			parseThreads_ReleaseData();
		}
		else if (do_what == 2 && REXX_RunningScript==0 && runningTWXScript==0 && passToREXX==1 && parserTest[do_what]==1)
		{  
			parseThreads_WaitForData(0);
			outputData("start 2");

			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector;
			ReleaseMutex(tempCurrentSectorMutex);

			if (tempCurrentSector<=0 || tempCurrentSector >header.sector_cnt)  //check universe size
			{
				DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector, "b");
				goto skip_to_next_statement_parsedata;
			}
			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " vs. theirs: ","\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			result = parseDataWaitThreadResult;

			if (result==1)
			{
				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\r", "\n", NULL);
				last_line = pSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				tempVariable = atol(last_line);

				if (tempCurrentSector>0 && tempCurrentSector<=header.sector_cnt)
					update_status_bar();

				#ifdef P_FREE
					pfree(&last_line);           // free present block        
				#else
					free(last_line);           // free present block        
				#endif		
			}	

			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Option? (A,D,I,R,P,?)", "Option? (A,D,I,R,?)", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;

			result1 = parseDataWaitThreadResult;

			if (result1==1)
			{
				//only kill your enemy's toll figs
				if (header.tollTolls==TOLLSFIGPAY)
					attac_send("py");
				else if (header.tollTolls==TOLLSFIGKILL && isEnemy ()==1)
				{
					if (tempVariable<=MAX_FIGS_TO_ATTACK)
					{
						attac_send("ay99");
						attac_send_char('\015');
					}
					else
					{
						sprintf (tempBuffer, "Not attacking figs since there are more than %d of them", MAX_FIGS_TO_ATTACK);
						attacInfoMessage(tempBuffer);
					}
				}
				else if (header.tollTolls==TOLLSFIGKILL && isEnemy ()==0)
				{
					attac_send("py");
				}
			}
			else if (result1==2)
			{
				//only kill your enemy's toll figs
				if (isEnemy ()==1)
				{
					if (header.defensiveTolls==DEFENSIVEFIGKILL)
					{
						if (tempVariable<=MAX_FIGS_TO_ATTACK)
						{
							attac_send("ay99");
							attac_send_char('\015');
						}
						else
						{
							sprintf (tempBuffer, "Not attacking figs since there are more than %d of them", MAX_FIGS_TO_ATTACK);
							attacInfoMessage(tempBuffer);
						}
					}
				}
			}

			outputData("end 2");
			parseThreads_ReleaseData();
		}
		else if (do_what == 3 && passToREXX==1 && parserTest[do_what]==1)
		{
			parseThreads_WaitForData(0);
			outputData("start 3");
			timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, ",");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "turns left");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			last_line = pSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			if (Instr(1, last_line, "Unlimited")>0)
				numTurns =-1;
			else
				numTurns = atol(last_line);

			#ifdef P_FREE
				pfree(&last_line);           // free present block        
			#else
				free(last_line);           // free present block        
			#endif		
			update_status_bar();
			outputData ("end 3");
			parseThreads_ReleaseData();
		}
		else if (do_what == 4 && passToREXX==1 && parserTest[do_what]==1)
		{
			parseThreads_WaitForData(0);
			outputData ("start 4");
			timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "experience point(s)");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			last_line = pSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			tempLong = atol(last_line);
			numExp += tempLong;
			#ifdef P_FREE
				pfree(&last_line);           // free present block        
			#else
				free(last_line);           // free present block        
			#endif		
			update_status_bar();
				
			outputData ("end 4");
			parseThreads_ReleaseData();
		}
		else if (do_what==5 && passToREXX==1 && parserTest[do_what]==1)
		{
			parseThreads_WaitForData(0);
			outputData ("start 5");
			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "sight","experience point(s)", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			if (result>1)
			{
				last_line = pSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				tempLong = atoi(last_line);
				numExp -= tempLong;
				#ifdef P_FREE
					pfree(&last_line);           // free present block        
				#else
					free(last_line);           // free present block        
				#endif		
				update_status_bar();
			}
			outputData ("end 5");
			parseThreads_ReleaseData();
		}
		else if (do_what == 6 && passToREXX==1 && parserTest[do_what]==1)
		{
			parseThreads_WaitForData(0);
			outputData ("start 6");
			timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "point(s)");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			last_line = pSCRIPT_LAST_LINE(1);
			last_line = UNCOMMA(last_line);
			tempLong = atol(last_line);
			numAlign -= tempLong;
			#ifdef P_FREE
				pfree(&last_line);           // free present block        
			#else
				free(last_line);           // free present block        
			#endif		
			update_status_bar();
			outputData ("end 6");
			parseThreads_ReleaseData();
		}
		else if ((do_what == 7 && parserTest[do_what]==1) || (do_what == 8 && parserTest[do_what]==1))
		{
			parseThreads_WaitForData(0);
			outputData ("start 7,8");
			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, ":\r","Federation comm-link:","on channel", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			result = parseDataWaitThreadResult;

			if (result==1)
			{
				from = pSCRIPT_LAST_LINE(1);

				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\n", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\n", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				last_line = pSCRIPT_LAST_LINE(2);

				if (strlen(last_line)>0 && last_line[0]=='!')
				{
					if (strlen(last_line)<128)
					{
						strcpy (tempBuffer, last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
						last_line = (char*) malloc (128);
						strcpy (last_line, tempBuffer);
					}

					tempString = DecryptCommMessage(last_line, from);
				}
				else 
					tempString = (char *) strdup (last_line);

//				sprintf (data, "HAIL: %s: %s", from, last_line);
//				outputData(data);
				attacHailMessage(tempString, from);
				#ifdef P_FREE
					pfree(&last_line);
					pfree(&from);
					pfree(&tempString);
				#else
					free(last_line);
					free(from);
					free(tempString);
				#endif		
			}
			else if (result==2)
			{
				from = pSCRIPT_LAST_LINE(25);

				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\n", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\n", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				last_line = pSCRIPT_LAST_LINE(2);
				

				if (strlen(last_line)>0 && last_line[0]=='!')
				{
					if (strlen(last_line)<128)
					{
						strcpy (tempBuffer, last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
						last_line = (char*) malloc (128);
						strcpy (last_line, tempBuffer);
					}

					tempString = DecryptCommMessage(last_line, from);
				}
				else 
					tempString = (char *) strdup (last_line);

//				printf ("FED: %s: %s\n", from, tempString);

				attacFedCommMessage(tempString, from);
				#ifdef P_FREE
					pfree(&last_line);
					pfree(&from);
					pfree(&tempString);
				#else
					free(last_line);
					free(from);
					free(tempString);
				#endif		
			}
			else if (result==3)
			{
				from = pSCRIPT_LAST_LINE(11);

				timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, ":");
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				last_line = pSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				tempLong = atol(last_line);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif		

				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\n", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "\n", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				last_line = pSCRIPT_LAST_LINE(2);

				if (strlen(last_line)>0 && last_line[0]=='!')
				{
					if (strlen(last_line)<128)
					{
						strcpy (tempBuffer, last_line);
						#ifdef P_FREE
							pfree(&last_line);
						#else
							free(last_line);
						#endif		
						last_line = (char*) malloc (128);
						strcpy (last_line, tempBuffer);
					}

					tempString = DecryptCommMessage(last_line, from);
				}
				else 
					tempString = (char *) strdup (last_line);

//				printf ("SUBSPACE: %ld: %s %s\n", tempLong, from, tempString);
//				sprintf (data, "SUBSPACE: %ld: %s %s", tempLong, from, last_line);
//				outputData(data);
				attacSubSpaceMessage(tempString, from, tempLong);
				#ifdef P_FREE
					pfree(&last_line);
					pfree(&from);
					pfree(&tempString);
				#else
					free(last_line);
					free(from);
					free(tempString);
				#endif		
			}

			outputData ("end 7,8");
			parseThreads_ReleaseData();
		}
		else if(do_what==9 && passToREXX==1 && parserTest[do_what]==1)
		{
			parseThreads_WaitForData(0);
			outputData ("start 9");

			WaitForSingleObject(tempCurrentSectorMutex, INFINITE);
			tempCurrentSector = currentSector;
			ReleaseMutex(tempCurrentSectorMutex);

			if (tempCurrentSector <=0 || tempCurrentSector >header.sector_cnt)  //check universe size
			{
				DISPLAY_UNIVERSE_SIZE_ERROR(tempCurrentSector, "j");
				goto skip_to_next_statement_parsedata;
			}

//			sprintf (data, "SHOWING FIGS");
//			outputData(data);

			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Command", "==============\r", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			//clear out all old figs in db

			for (i=0; i<header.sector_cnt; i++)
			{
				//if the figs belong to me
				if (sectors[i].ftr_owner==0 || (sectors[i].ftr_owner-CORP_CONSTANT)==players[0].corpNumber)
				{
//					sprintf (data, "sector %d", i+1);
//					outputData(data);
					sectors[i].ftrs=-1;
					sectors[i].ftr_type=-1;
					sectors[i].ftr_owner=-1;
				}
			}

			//move to the right place :)
			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Command", "\n", "\r", "No fighters deployed", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			result = parseDataWaitThreadResult;

			while (result==2 || result==3)
			{
				last_line = pSCRIPT_LAST_LINE(1);
				
//				sprintf (data, "FIG Last Line: %s", last_line);
//				outputData(data);
				parseAndEnterUserFigs(last_line);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif		
				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Command", "\n", "\r", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				result = parseDataWaitThreadResult;
			}

			writeSetReady();
			outputData ("end 9");
			parseThreads_ReleaseData();
		}
		else if(do_what==10 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			// the newline stuff is to make sure we get kicked out of this wait
			// if it really isnt credits
			outputData ("start 10");

			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " credits and "," fighter(s)","\r","\n","free", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			result = parseDataWaitThreadResult;

			if (result==1)
			{
				last_line = pSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				numCredits = atol(last_line);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif

				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " empty cargo holds","\r","\n","free", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;

				last_line = pSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				tmpInt = atol(last_line);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif

				if (tmpInt!=(int)numEmptyHolds)
				{

					//change amount in holds on GUI
					if (buyingSelling==1)
					{
						numFuel+=buyingSellingAmount;
						numEmptyHolds-=buyingSellingAmount;
						//undo the GUI variables when you port
						buyingSelling = 0;
						buyingSellingAmount = 0;
					}
					else if (buyingSelling==2)
					{
						numOrgs+=buyingSellingAmount;
						numEmptyHolds-=buyingSellingAmount;
						//undo the GUI variables when you port
						buyingSelling = 0;
						buyingSellingAmount = 0;
					}
					else if (buyingSelling==3)
					{
						numEquip+=buyingSellingAmount;
						numEmptyHolds-=buyingSellingAmount;
						//undo the GUI variables when you port
						buyingSelling = 0;
						buyingSellingAmount = 0;
					}
					else if (buyingSelling==4)
					{
						numFuel-=buyingSellingAmount;
						numEmptyHolds+=buyingSellingAmount;
						//undo the GUI variables when you port
						buyingSelling = 0;
						buyingSellingAmount = 0;
					}
					else if (buyingSelling==5)
					{
						numOrgs-=buyingSellingAmount;
						numEmptyHolds+=buyingSellingAmount;
						//undo the GUI variables when you port
						buyingSelling = 0;
						buyingSellingAmount = 0;
					}
					else if (buyingSelling==6)
					{
						numEquip-=buyingSellingAmount;
						numEmptyHolds+=buyingSellingAmount;
						//undo the GUI variables when you port
						buyingSelling = 0;
						buyingSellingAmount = 0;
					}
				}
			update_status_bar();

			}
			else if (result==2)
			{
				last_line = pSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				numFigs = atol(last_line);
//					sprintf (data, "numFigs: %ld", numFigs);
//					outputData(data);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif
				update_status_bar();
			}
			outputData ("end 10");
			parseThreads_ReleaseData();
		}	      
		else if(do_what==11 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 11");

			// the newline stuff is to make sure we get kicked out of this wait
			// if it really isnt figs

			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " fighter(s)","\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			result = parseDataWaitThreadResult;
			if (result==1)
			{
				last_line = pSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				//subtract that number of figs from the amount
				numFigs -= atol(last_line);
//				sprintf (data, "numFigs: %ld", numFigs);
//				outputData(data);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif
				update_status_bar();
			}
			outputData("end 11");
			parseThreads_ReleaseData();
		}	      
		else if(do_what==12 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			buyingSelling=1;
			parseThreads_ReleaseData();
		}	      
		else if(do_what==13 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			buyingSelling=2;
			parseThreads_ReleaseData();
		}	      
		else if(do_what==14 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			buyingSelling=3;
			parseThreads_ReleaseData();
		}	      
		else if(do_what==15 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			buyingSelling=4;
			parseThreads_ReleaseData();
		}	      
		else if(do_what==16 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			buyingSelling=5;
			parseThreads_ReleaseData();
		}	      
		else if(do_what==17 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			buyingSelling=6;
			parseThreads_ReleaseData();
		}	      
		else if(do_what==18 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 18");
			// get amount to buy/sell
			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, " units.","\r", "\n", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			result = parseDataWaitThreadResult;
			if (result==1)
			{
				last_line = pSCRIPT_LAST_LINE(1);
				last_line = UNCOMMA(last_line);
				//subtract that number of figs from the amount
				tmpInt = atoi(last_line);
				if (tmpInt>0 && tmpInt<=32000)
					buyingSellingAmount = tmpInt;
//				sprintf (data, "buyingSellingAmount: %d", buyingSellingAmount);
//				outputData(data);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif
			}
			outputData ("end 18");
			parseThreads_ReleaseData();
		}	      
		else if(do_what==19 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 19");
			//change amount in holds on GUI
			if (buyingSelling==1)
			{
				numFuel+=buyingSellingAmount;
				numEmptyHolds-=buyingSellingAmount;
				update_status_bar();
				//undo the GUI variables when you port
				buyingSelling = 0;
				buyingSellingAmount = 0;
			}
			else if (buyingSelling==2)
			{
				numOrgs+=buyingSellingAmount;
				numEmptyHolds-=buyingSellingAmount;
				update_status_bar();
				//undo the GUI variables when you port
				buyingSelling = 0;
				buyingSellingAmount = 0;
			}
			else if (buyingSelling==3)
			{
				numEquip+=buyingSellingAmount;
				numEmptyHolds-=buyingSellingAmount;
				update_status_bar();
				//undo the GUI variables when you port
				buyingSelling = 0;
				buyingSellingAmount = 0;
			}
			else if (buyingSelling==4)
			{
				numFuel-=buyingSellingAmount;
				numEmptyHolds+=buyingSellingAmount;
				update_status_bar();
				//undo the GUI variables when you port
				buyingSelling = 0;
				buyingSellingAmount = 0;
			}
			else if (buyingSelling==5)
			{
				numOrgs-=buyingSellingAmount;
				numEmptyHolds+=buyingSellingAmount;
				update_status_bar();
				//undo the GUI variables when you port
				buyingSelling = 0;
				buyingSellingAmount = 0;
			}
			else if (buyingSelling==6)
			{
				numEquip-=buyingSellingAmount;
				numEmptyHolds+=buyingSellingAmount;
				update_status_bar();
				//undo the GUI variables when you port
				buyingSelling = 0;
				buyingSellingAmount = 0;
			}
			outputData ("end 19");
			parseThreads_ReleaseData();
		}	      
		else if(do_what==20 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 20");

			// what happens when you jettison
			timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "Command [TL=");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			numFuel=0;
			numOrgs=0;
			numEquip=0;
			numColos=0;
			numEmptyHolds=numTotalHolds;
			update_status_bar();
			outputData ("end 20");
			parseThreads_ReleaseData();
		}	      
		else if((do_what==21 || do_what==22 || do_what==23 || do_what == 24 || do_what==25 || do_what==26 || do_what==27 || do_what==28) && passToREXX==1)
		{
			parseThreads_WaitForData(0);
			validDoubleY=1;
			parseThreads_ReleaseData();
		}
		else if(do_what==29 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
//			sprintf (data, "citadel");
//			outputData(data);

			commandPrompt=5;
			parseThreads_ReleaseData();
		}
		else if(do_what==30 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
//			sprintf (data, "sd");
//			outputData(data);
			commandPrompt=6;
			parseThreads_ReleaseData();
		}
		else if(do_what==31 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
//			sprintf (data, "on planet");
//				outputData(data);
			commandPrompt=4;
			parseThreads_ReleaseData();
		}
		else if(do_what==32 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 32");

			if (header.selfDestructProtection==1 && validRegCode==1)
				attac_send("n");

			outputData ("end 32");
			parseThreads_ReleaseData();
		}
		else if(do_what==33 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 33");

			if (header.blindTwarpProtection==1 && validRegCode==1)
				attac_send("n");

			outputData ("end 33");
			parseThreads_ReleaseData();
		}
		else if(do_what==34 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 34");

			do_writeMemory();

			outputData ("end 34");
			parseThreads_ReleaseData();
		}
		else if(do_what==35 && passToREXX==1 && parserTest[do_what]==1)
		{
			parseThreads_WaitForData(0);
			outputData ("start 35");

			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Computer", "--- --------------------- -- ------------------------------ ------------------", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;

			//move to the right place :)
			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Computer", "\n", "\r", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;

			result = parseDataWaitThreadResult;

			while (result==2 || result==3)
			{
				last_line = pSCRIPT_LAST_LINE(1);
				
				if (strcmp (last_line, "") != 0 && strcmp (last_line, " ") != 0)
					parseTraderListing(last_line);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif
				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Computer", "\n", "\r", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;

				result = parseDataWaitThreadResult;
			}

			writeSetReady();
			outputData ("end 35");
			parseThreads_ReleaseData();
		}
		else if(do_what==36 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 36");

			if (showlimpitscanner != NULL && enableLimpitScanner==1)
			{
				timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "========================");
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;

				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Total","No Active Limpet mines detected", "Personal", "Corporate", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				result1 = parseDataWaitThreadResult;

				for (i=0; i< LIMPIT_COUNT_MAX; i++)
					limpitList[i]=0;
				i=0;
				while (result1 ==3 || result1==4)
				{
					hlist = GetDlgItem(showlimpitscanner, IDC_LIMPITSCANNER_LIST);

					last_line = pSCRIPT_LAST_LINE(0);
					last_line = CLEANLINE(last_line);

					strcpy (tempBuffer, last_line);
					#ifdef P_FREE
						pfree(&last_line);
					#else
						free(last_line);
					#endif

				
					token = strtok(tempBuffer," ");
					limpitList[i++]=atol(token);

					timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Total","No Active Limpet mines detected", "Personal", "Corporate", NULL);
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_parsedata;
					result1 = parseDataWaitThreadResult;		
				}
				for (i=0; i<LIMPIT_COUNT_MAX;i++)
					if (limpitList[i]>0)
						updateLimpitScanner(hlist, limpitList[i]);
					
				if (enableLimpitScanner==1)
					attac_send ("k2");
			}
			outputData ("end 36");
			parseThreads_ReleaseData();
		}
		else if(do_what==37 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 37");

			last_line = pSCRIPT_LAST_LINE(0);

			tempString = right (last_line, 32);

			tempVariable = Instr (1, tempString, "Sector") +6;
			tempVariable1 = Instr (1, tempString, "will now be avoided") -1;
			for (i=0, j=tempVariable; i<tempVariable1 - tempVariable; i++, j++)
				tempBuffer[i] = tempString[j];
			tempBuffer[i]=0;
			if (atol(tempBuffer)>0 && atol(tempBuffer)<=header.sector_cnt)
				sectors[atol(tempBuffer)-1].avoided=1;

			#ifdef P_FREE
				pfree(&last_line);
				pfree(&tempString);
			#else
				free(last_line);
				free(tempString);
			#endif

			outputData ("end 37");
			parseThreads_ReleaseData();
		}
		else if(do_what==38 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 38");

			for (i=0; i<header.sector_cnt; i++)
				sectors[i].avoided=0;

			outputData ("end 38");
			parseThreads_ReleaseData();
		}
		else if(do_what==39 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 39");
			//clear avoid
			last_line = pSCRIPT_LAST_LINE(23);
			last_line = UNCOMMA(last_line);

			strcpy(tempBuffer, "");
			for (i=strlen(last_line), j=0; i>0 && last_line[i] != 10 && last_line[i] != 13; i--)
			{
				sprintf (tChar, "%c", last_line[i]); 
				strcat (tempBuffer, tChar);
			}
			strrev(tempBuffer);
			if (atol(tempBuffer)>0 && atol(tempBuffer) <=header.sector_cnt)
				sectors[atol(tempBuffer)-1].avoided=0;

			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif


			outputData ("end 39");
			parseThreads_ReleaseData();
		}
		else if(do_what==40 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 40");

			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "No Sectors are currently being avoided.", "Do you wish to clear some avoids?",NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;

			result = parseDataWaitThreadResult;
			if (result>1)
			{
				last_line = pSCRIPT_LAST_LINE(33);
				strcpy (tempBuffer, last_line);
				#ifdef P_FREE
					pfree(&last_line);
				#else
					free(last_line);
				#endif

				for (i=0; i< header.sector_cnt; i++)
					sectors[i].avoided=0;

				token = strtok(tempBuffer," \n\r");
				while(token != NULL && strlen(token)>0)
				{
					if (atol(token)>0 && atol(token)<=header.sector_cnt)
						sectors[atol(token)-1].avoided=1;
					//move to next sector
					token = strtok(NULL," \n\r");
				}	  

			}
			//only display a message once every two weeks
			if ((ATTAC_Time() > (header.lastAnnouncementStartup + 1209600)) && (header.announcementStartup==1))
			{
				header.lastAnnouncementStartup = ATTAC_Time();
				writeSetReady();

				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Command [TL=","Computer command [TL=","Planet command (?", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				result = parseDataWaitThreadResult;
				Keyboard_Locked=1;
				if (result==1)
				{
					attac_send("CA");
					attac_send(announcement);
					attac_send_char('\015');
					timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "send universally?");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_parsedata;
					attac_send("YQ");
				}
				else if (result==2)
				{
					attac_send("QCA");
					attac_send(announcement);
					attac_send_char('\015');
					timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "send universally?");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_parsedata;
					attac_send("YQ");
				}
				else if (result==3)
				{
					attac_send("CCA");
					attac_send(announcement);
					attac_send_char('\015');
					timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "send universally?");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_parsedata;
					attac_send("YQQ");
				}
			}
			timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Command [TL=","Computer command [TL=","Planet command (?", NULL);
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;
			result = parseDataWaitThreadResult;
			Keyboard_Locked=1;
			if (result==1 && header.updateInfoBarStartup==1)
			{
				attac_send("cn");
				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Mode     - Compact","Mode     - Long", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				result1 = parseDataWaitThreadResult;

				if (result1==1)
				{
					attac_send("a");
					timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "Settings command");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_parsedata;
				}
				attac_send("qq");
				timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "Command [TL=");
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				attac_send_char('i');
			}
			else if (result==2 && header.updateInfoBarStartup==1)
			{
				attac_send("cn");
				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Mode     - Compact","Mode     - Long", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				result1 = parseDataWaitThreadResult;
				if (result1==1)
				{
					attac_send("a");
					timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "Settings command");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_parsedata;
				}
				attac_send("qq");
				timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "Command [TL=");
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				attac_send("icq");
			}
			else if (result==3 && header.updateInfoBarStartup==1)
			{
				attac_send("ccn");
				timeout = pSCRIPT_WAIT_MUX(TIMEOUT_AMOUNT, "Mode     - Compact","Mode     - Long", NULL);
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				result1 = parseDataWaitThreadResult;
				if (result1==1)
				{
					attac_send("a");
					timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "Settings command");
					if (timeout == TIMEOUT_CONSTANT)
						goto skip_to_next_statement_parsedata;
				}
				attac_send("qq");
				timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "Citadel command");
				if (timeout == TIMEOUT_CONSTANT)
					goto skip_to_next_statement_parsedata;
				attac_send("iq");
			}
		
			Keyboard_Locked=0;
			
			outputData ("end 40");
			parseThreads_ReleaseData();
		}
		else if(do_what==41 && passToREXX==1 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 41");

			timeout = pSCRIPT_WAIT(TIMEOUT_AMOUNT, "Computer command [TL=");
			if (timeout == TIMEOUT_CONSTANT)
				goto skip_to_next_statement_parsedata;

			last_line = pSCRIPT_LAST_LINE(21);
			strcpy (tempBuffer, last_line);
			#ifdef P_FREE
				pfree(&last_line);
			#else
				free(last_line);
			#endif
			
			for (i=0; i< header.sector_cnt; i++)
				sectors[i].avoided=0;

			token = strtok(tempBuffer," \n\r");
			while(token != NULL && strlen(token)>0)
			{
				if (atol(token)>0 && atol(token)<=header.sector_cnt)
					sectors[atol(token)-1].avoided=1;
				//move to next sector
				token = strtok(NULL," \n\r");
			}	  

			outputData ("end 41");
			parseThreads_ReleaseData();
		}
		else if(do_what==42 && parserTest[do_what]==1)
		{   
			parseThreads_WaitForData(0);
			outputData ("start 42");

			//start TWXSync
			buildSendTWXData_WaitForData(INFINITE);
			dataMessage = sendAllNewTWXSyncData();
			postSendMessageToTWXSync(threadId, strdup(dataMessage));
			#ifdef P_FREE
				pfree(&dataMessage);
			#else
				free(dataMessage);
			#endif
			buildSendTWXData_ReleaseData();
			//end TWXSync

			outputData ("end 42");
			parseThreads_ReleaseData();
		}



skip_to_next_statement_parsedata:
		passToREXX=1;
		parseThreads_ReleaseData();

	}
	attacErrorMessage("PARSE THREAD HAS ENDED. Please report this error to tradewars@hotmail.com");
	CloseHandle(ParseDataWaitEvent);

	_endthread();
}



long pSCRIPT_WAIT(long timeout, char arg[])
{
	long returnV;
	returnV = timeout = pSCRIPT_WAIT_MUX(timeout, arg, NULL);
	return returnV;
}

long pSCRIPT_WAIT_MUX(long timeout, ...)
{
	va_list ap;
	char *p=NULL;
	int Found, I, pos;
	long tempLong;
	long returnVal;

	parseDataWaitCount=0;

	va_start (ap, timeout);
	while ((p = va_arg(ap, char *)) != NULL)
	{
		strcpy(parseDataWaitArray[parseDataWaitCount], p);
		parseDataWaitCount++;
	}
	va_end (ap);

    pos = strlen(DSM_BufferParseData);

    Found = -1;

	for (I = 0; I< parseDataWaitCount; I++)
	{
	    tempLong = Instr(1, DSM_BufferParseData, parseDataWaitArray[I]);

        if ((tempLong != 0) && (tempLong < pos))
		{
			pos = tempLong;
            Found = I;
		}
	}

    if (Found != -1)
	{
		Update_LastLine_ParseData(Instr(1, DSM_BufferParseData, parseDataWaitArray[Found]) + strlen(parseDataWaitArray[Found]) - 1);
    	parseDataWaitThreadResult=Found+1;
	    
		SetEvent(ParseDataWaitEvent);
		return 0;
    }

	//reset the event state to non-signaled
	ResetEvent(ParseDataWaitEvent);

	//tell the main thread to continue parsing
	returnVal = WaitForSingleObject(ParseDataWaitEvent, timeout);
	if (returnVal==WAIT_TIMEOUT)
	{
		printf ("!!! PARSE THREAD TIMEOUT !!!\n");
//		sprintf (data, "!!! PARSE THREAD TIMEOUT !!!");
//		outputData(data);
		return 640;
	}

	return 1;
}

char* pSCRIPT_LAST_LINE(int deletion)
{
	char *tmpPointer=NULL;
	if (strlen (parseDataLastLine)==0)
		tmpPointer = (char*) strdup(" ");
	else
	{
		if ((int)strlen(parseDataLastLine)>= deletion)
			parseDataLastLine[strlen(parseDataLastLine) - deletion] = 0;
		tmpPointer = (char*) strdup (parseDataLastLine);
	}
	return (tmpPointer);
}



char * UNCOMMA (char *tmp1)
{
        int i=-1, j=0;
        char *tmpS=NULL;
        char *t=NULL;
        WaitForSingleObject(UnCommaMutex, INFINITE);
        if (strcmp (tmp1, " ")==0)
        {
                ReleaseMutex(UnCommaMutex);
                return(tmp1);
        }
        tmpS = (char *)malloc (strlen(tmp1)+1);
        while (tmp1[++i] != '\0')
        {
                if (tmp1[i] != ' ' && tmp1[i] != ',')
                        tmpS[j++] = tmp1[i];
        }
        // MWM: Fast than strdup since we know the size
        t = (char *)malloc(j+1);
        for (i = 0; i < j; t[i] = tmpS[i], i++);
		t[j]=0;
        
		#ifdef P_FREE
	        pfree(&tmpS);
		    pfree(&tmp1);
		#else
	        free(tmpS);
		    free(tmp1);
		#endif
        ReleaseMutex(UnCommaMutex);
        return(t);
}


char * CLEANLINE (char *tmp1)
{
	int i=-1, j=0;
	char *tmpS=NULL;
	char *t=NULL;
	int startchar=0;

	WaitForSingleObject(CleanLineMutex, INFINITE);
	if (strcmp (tmp1, " ")==0)
	{
		ReleaseMutex(CleanLineMutex);
		return(tmp1);
	}

	tmpS = (char *)malloc (strlen(tmp1)+1);
        
	while (tmp1[++i] != '\0')
    {
		if (tmp1[i] != ' ' && tmp1[i] != '\n' && tmp1[i] != '\r')
			startchar=1;

		if (tmp1[i] != '\n' && startchar==1)
			tmpS[j++] = tmp1[i];
    }

    t = (char *)malloc(j+1);
    for (i = 0; i < j; t[i] = tmpS[i], i++);
	t[j]=0;
    
	#ifdef P_FREE
	    pfree(&tmpS);
		pfree(&tmp1);
	#else
	    free(tmpS);
		free(tmp1);
	#endif

	ReleaseMutex(CleanLineMutex);

	return(t);
}

void Update_LastLine_ParseData(long pos)
{
	char *tmp=NULL;

	if (pos>=MAX_LENGTH-1)
		tmp = left(DSM_BufferParseData, MAX_LENGTH);
	else
		tmp = left(DSM_BufferParseData, pos);
	strcpy (parseDataLastLine, tmp);
	#ifdef P_FREE
		pfree(&tmp);
	#else
	    free(tmp);
	#endif

	if (strlen(DSM_BufferParseData) - pos>=MAX_LENGTH-1)
		tmp = right(DSM_BufferParseData, MAX_LENGTH);
	else
		tmp = right(DSM_BufferParseData, strlen(DSM_BufferParseData) - pos);
	strcpy (DSM_BufferParseData, tmp);
	#ifdef P_FREE
		pfree(&tmp);
	#else
	    free(tmp);
	#endif
}

	 
void outputData(char str[])
{
	char crlf[2];
	char buf[128], *Msg=NULL;

	WaitForSingleObject(SyncDataMutex, INFINITE);
	if (debugMe==1)
	{
		if (remote==1)
		{
			if (theClients[1].sock>0)
			{
				crlf[0]=13;
				crlf[1]=10;
				sprintf (buf, "%s%c%c", str, crlf[0], crlf[1]);
				Msg = Translate_String(buf, 0, 0);

				send(theClients[1].sock, Msg, strlen(Msg), 0);
				#ifdef P_FREE
					pfree(&Msg);
				#else
					free(Msg);
				#endif
			}
		}
		else
			printf ("%s\n", str);
	}

	ReleaseMutex(SyncDataMutex);
}


/*
void free (void **x)
{
        WaitForSingleObject(freeMutex, INFINITE);
        if (*x==NULL)
                printf ("THERE IS A FREE MEMORY ERROR\n");
        else
                pfree(*x);
        x = NULL;
        ReleaseMutex(freeMutex);
}
*/
long Parse_WaitForREXXData(long timeout)
{
	int result;

	if (timeout == 0)
		timeout = INFINITE;
	result = WaitForSingleObject(REXXThreadsMutex, timeout);

	return result;
}

long Parse_ReleaseREXXData()
{
	int result;

	result = ReleaseMutex(REXXThreadsMutex);

	return result;
}

long parseThreads_WaitForData(long timeout)
{
	int result;

	if (timeout == 0)
		timeout = 50000;
	result = WaitForSingleObject(parseThreadsMutex, timeout);

	return result;
}

long parseThreads_ReleaseData()
{
	int result;

	result = ReleaseMutex(parseThreadsMutex);

	return result;
}
