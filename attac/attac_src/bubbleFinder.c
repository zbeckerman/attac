// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#include "attacGlobal.h"
#include "attacGUI.h"
#include "../PUTTY.H"
#include "rexx.h"
#include "bubbleFinder.h"
#include "win_res.h"
#include "structs.h"
#include "../../attacv4Dll/TWBFind.h"
 
char* bubbleCharDataStorage=NULL;
int bubbleOffset=250;

void findBubblesThreadProc(void *dummy)
{

	char *bufferVar[100], buf[20], buf1[20], bubbleChar[MAX_LENGTH*2];
	char *token, *x;
	char bubble_sectors[MAX_LENGTH*3][20];
	int pos, i, j, count;
	HWND hlist;

	hlist = GetDlgItem(showbubblefinder, IDC_BUBBLE_FINDER_LIST_ENTRANCE);

	bufferVar[0] = (char *) strdup ("programname");

	GetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_MIN, buf1, sizeof(buf1) - 1);
	sprintf (buf, "/m%d", atoi(buf1));
	bufferVar[1] = (char *) strdup (buf);

	GetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_MAX, buf1, sizeof(buf1) - 1);
	sprintf (buf, "/x%d", atoi(buf1));
	bufferVar[2] = (char *) strdup (buf);

	GetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_ENTRANCE_MULT, buf1, sizeof(buf1) - 1);
	sprintf (buf, "/z%d", atoi(buf1));
	bufferVar[3] = (char *) strdup (buf);

	GetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_MAX_ENTRANCES, buf1, sizeof(buf1) - 1);
	sprintf (buf, "/e%d", atoi(buf1));
	bufferVar[4] = (char *) strdup (buf);


	sprintf (buf, "/n%ld", header.sector_cnt);
	bufferVar[5] = (char *) strdup (buf);

	if (IsDlgButtonChecked(showbubblefinder, IDC_BUBBLE_FINDER_MEMORY_CHECK))
		bufferVar[6] = (char *) strdup ("/g");
	else
		bufferVar[6] = (char *) strdup (" ");

//	bufferVar[7] = (char *) strdup ("/r5000");


//	printf ("%s %s %s %s %s %s %s\n\n", bufferVar[0], bufferVar[1], bufferVar[2], bufferVar[3], bufferVar[4], bufferVar[5], bufferVar[6]);

	if (bubbleCharDataStorage != NULL)
	{
		pfree (&bubbleCharDataStorage);	//we free the temp storage. this is freed when we close the bubble finder window
		bubbleCharDataStorage=NULL;
	}

	bubbleCharDataStorage = (char *) malloc(500000);
	
	EnableWindow( GetDlgItem(showbubblefinder,IDOK), FALSE ) ; // disable a control
	EnableWindow( GetDlgItem(showbubblefinder,IDCANCEL), FALSE ) ; // disable a control 
	SetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_STATUS, "Status: Running Bubble Finder");
	

	findATTACBubbles (6, bufferVar, sectors, bubbleCharDataStorage);

	x = (char *) strdup (bubbleCharDataStorage);
	#ifdef P_FREE
		pfree(&bubbleCharDataStorage);
	#else
		free(bubbleCharDataStorage);
	#endif
	bubbleCharDataStorage = x;


	if (Instr(1, bubbleCharDataStorage, "ERROR"))
	{
		SetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_STATUS, bubbleCharDataStorage);
	}
	else
	{
		j=0;
		count=0;
		for (i=0; i<(int) strlen(bubbleCharDataStorage); i++)
		{
			if (bubbleCharDataStorage[i]==';')
			{
				bubbleChar[j]=0;
				j=0;
				pos=0;
				token = strtok(bubbleChar, ":");
				while (token != NULL)
				{
					strcpy (bubble_sectors[pos], token);
					token = strtok(NULL, ":");
					pos++;
				}	
				listview_insert_bubble_entrances(hlist, count++, bubble_sectors[1], bubble_sectors[0]);
			}
			else
				bubbleChar[j++] = bubbleCharDataStorage[i];
		}
		SetDlgItemText(showbubblefinder, IDC_BUBBLE_FINDER_STATUS, "Status: Idle");
	}
	pfree (&bufferVar[0]);
	pfree (&bufferVar[1]);
	pfree (&bufferVar[2]);
	pfree (&bufferVar[3]);
	pfree (&bufferVar[4]);
	pfree (&bufferVar[5]);
	pfree (&bufferVar[6]);
//	pfree (&bufferVar[7]);

	
	EnableWindow( GetDlgItem(showbubblefinder,IDOK), TRUE ) ; // enable a control 
	EnableWindow( GetDlgItem(showbubblefinder,IDCANCEL), TRUE ) ; // enable a control 

	_endthread();
	return;
}
