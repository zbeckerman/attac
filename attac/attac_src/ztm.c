// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "util.h"
#include "ztm.h"
#include "rexx.h"

int parseZTMData=0;
int GetZTMData=0;
int currentZTM=0;
int forwardZTM=0;
int backwardZTM=0;
int oneWayZTM=0;
int randomZTM=0;

char oneWayWarps[MAX_ONE_WAY_WARPS][MAX_ONE_WAY_WARP_LENGTH];
int oneWayWarpCount=0;
int totalOneWayWarpCount=0;



int isOneWayWarp(long sector, long warpTo)
{
  if (sector==0 || warpTo==0 || sectors[warpTo-1].warps==0 || sectors[warpTo-1].warp_sect[0]==0)
    return (1);   //unsure if it is one way or not

  if (sectors[warpTo-1].warp_sect[0] == sector || 
      sectors[warpTo-1].warp_sect[1] == sector || 
      sectors[warpTo-1].warp_sect[2] == sector || 
      sectors[warpTo-1].warp_sect[3] == sector || 
      sectors[warpTo-1].warp_sect[4] == sector || 
      sectors[warpTo-1].warp_sect[5] == sector)
    return (0);   //regular warp
  else
    return (2);   //one way warp
}


void startZTM()
{
	int j=0;
	char tmp;
	char tmp2[2], *t;
	char buffer[20];
	long i;

	for (i=0; i<MAX_ONE_WAY_WARPS; i++)
		strcpy (oneWayWarps[i], "");

	for (i=0; i<header.sector_cnt; i++)
	{
		for (j=0; j<sectors[i].warps; j++)
		{
			if (isOneWayWarp(i+1, sectors[i].warp_sect[j])==2)
			{
				sprintf (buffer, "%ld:%ld", sectors[i].warp_sect[j], i+1);
				if (strlen (buffer)>MAX_ONE_WAY_WARP_LENGTH-1)
				{
					strcpy (oneWayWarps[oneWayWarpCount++], buffer);
				}
				else
				{
					t = (char*) left (buffer, MAX_ONE_WAY_WARP_LENGTH-1);
					strcpy (oneWayWarps[oneWayWarpCount++], t);
					free (t);
				}
				printf ("%s\n", buffer);
			}
		}
	}

	totalOneWayWarpCount = oneWayWarpCount;

	tmp2[0] = 'c';
	tmp2[1] = 0;
	attac_send(tmp2);

	for (i = 200; i <= 205; i++)
	{
		tmp = (char) i;
		attac_send_char(tmp);
	}

	srand ( time(NULL) );

	parseZTMData = 2;		//set it to 2 till you get into interrogation then back to 1
	strcpy(DSM_Buffer,"");

}

void parseZTMBufferData (char *str)
{
	char *token=NULL, tempString[2056], *csector;
	long sector=0, previousSector=0;

	strcpy(tempString, str);

	token = strtok(tempString, "\n\r");
	while(token != NULL)
	{
		if (Instr(1, token, "TO")==0 &&
			Instr(1, token, "FM")==0 &&
			Instr(1, token, ">")>1)
		{
			strcpy(tempString, token);

			token = strtok(NULL, "\n\r");
			if (token != NULL)
			{
				if (Instr(1, token, "TO")==0 &&
					Instr(1, token, "FM")==0 &&
					Instr(1, token, ">")>1)
				{
					strcat(tempString, token);
					token = strtok(NULL, "\n\r");
					if (token != NULL)
					{
						if (Instr(1, token, "TO")==0 &&
							Instr(1, token, "FM")==0 &&
							Instr(1, token, ">")>1)
						{
							strcat(tempString, token);
							token = strtok(NULL, "\n\r");
							if (token != NULL)
							{
								if (Instr(1, token, "TO")==0 &&
									Instr(1, token, "FM")==0 &&
									Instr(1, token, ">")>1)
								{
									strcat(tempString, token);
								}
							}
						}
					}
				}

			}
			break;
		}
		token = strtok(NULL,"\n\r");
	}

	sector=0;
	previousSector=0;

	token = strtok(tempString, ">:");
	while(token != NULL)
	{
		csector = removeParens (token);


		if (sector != 0)
			previousSector = sector;
		sector = atol(csector);
		free (csector);

		if (sector >0 && sector <= header.sector_cnt && previousSector >0 && previousSector <= header.sector_cnt)
		{
			addWarpToSector (sector, previousSector);
		}
		token = strtok(NULL,">:");
	}


}

void addWarpToSector (long otherSector, long sector)
{
	//dont do anything if it is already there.
	if (sectors[sector-1].warp_sect[0]==otherSector)
		;
	else if (sectors[sector-1].warp_sect[1]==otherSector)
		;
	else if (sectors[sector-1].warp_sect[2]==otherSector)
		;
	else if (sectors[sector-1].warp_sect[3]==otherSector)
		;
	else if (sectors[sector-1].warp_sect[4]==otherSector)
		;
	else if (sectors[sector-1].warp_sect[5]==otherSector)
		;
	else
	{
		sectors[sector-1].warp_sect[(int)sectors[sector-1].warps] = otherSector;
		sectors[sector-1].warps++;
		sectors[sector-1].info = 11;
	}
}

char * removeParens (char * tmp)
{
	char  str[10], *tstr;
	int i, j=0;

	for (i=0; i < (int) strlen (str); i++)
		str[i]=0;

	for (i=0; i < 10 && i < (int) strlen (tmp); i++)
	{
		if (tmp[i] != '(' && tmp[i] != ')')
			str[j++] = tmp[i];
	}

	tstr = (char *) strdup (str);
	return (tstr);
}


