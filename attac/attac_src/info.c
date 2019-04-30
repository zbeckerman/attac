 /* Status Info for ATTAC v4.0 
   Michael C. Welsh
   06/15/2002
   RagingMF@attbi.com
*/

// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>	/* for sprintf() */
#include <malloc.h>
#include <string.h>
#include <windows.h>
#include <math.h>

#include "attacGlobal.h"
#include "info.h"
#include "win_res.h"
#include "twxSync.h"
#include "../PUTTY.H"
#include "util.h"
#include "parsedatathread.h"
#include "rexx.h"

// Owner draw garp
#define XBITMAP 80 
#define YBITMAP 20 
#define BUFFER MAX_PATH 

long countMax=0;
HANDLE StatusBarMutex;

void update_status_bar()
{

	HWND hwnd;
	char buffer2[50];
	char *tmp;

	WaitForSingleObject(StatusBarMutex, INFINITE);

	if (showinfocenter == NULL) {
		ReleaseMutex(StatusBarMutex);
		return;
	  }

	// Sector
	SetDlgItemInt(showinfocenter, IDC_SECTOR, (UINT) currentSector, 0);

	// Turns
	hwnd = GetDlgItem(showinfocenter, IDC_TURNS);
	if (hwnd) {
		if (numTurns==-1)
			sprintf (buffer2, "99,999");
		else
		{
			tmp = addCommaToNumber(numTurns, 1);
			sprintf (buffer2, "%s", tmp);
			#ifdef P_FREE
				pfree(&tmp);
			#else
				free(tmp);
			#endif		
		}

		SetWindowText(hwnd, buffer2);
	}

  // Exp
	hwnd = GetDlgItem(showinfocenter, IDC_EXP);
	if (hwnd) {
		tmp = addCommaToNumber(numExp, 1);
		sprintf (buffer2, "%s", tmp);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		
		
		SetWindowText(hwnd, buffer2);
	}

	// Alignment
	hwnd = GetDlgItem(showinfocenter, IDC_ALIGN);
	if (hwnd) {
		tmp = addCommaToNumber(numAlign, 1);
		sprintf (buffer2, "%s", tmp);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		

		SetWindowText(hwnd, buffer2);
	}	

	// Credits
	hwnd = GetDlgItem(showinfocenter, IDC_CREDITS);
	if (hwnd) {
		tmp = addCommaToNumber(numCredits, 1);
		sprintf (buffer2, "%s", tmp);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		

		SetWindowText(hwnd, buffer2);
	}
	
	// Figs
	hwnd = GetDlgItem(showinfocenter, IDC_FIGS);
	if (hwnd) {
		tmp = addCommaToNumber(numFigs, 1);
		sprintf (buffer2, "%s", tmp);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		

		SetWindowText(hwnd, buffer2);
	}

    // Shields
	hwnd = GetDlgItem(showinfocenter, IDC_SHIELDS);
	if (hwnd) {
		tmp = addCommaToNumber(numShields, 1);
		sprintf (buffer2, "%s", tmp);
		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		

		SetWindowText(hwnd, buffer2);
	}

	// Total Holds
	SetDlgItemInt(showinfocenter, IDC_MAXHOLDS, numTotalHolds, 0);

	// Fuel
	SetDlgItemInt(showinfocenter, IDC_FUEL, numFuel, 0);

	// Orgs
	SetDlgItemInt(showinfocenter, IDC_ORGS, numOrgs, 0);
	
	// Equipment
	SetDlgItemInt(showinfocenter, IDC_EQUIP, numEquip, 0);
	
	// Empty
	SetDlgItemInt(showinfocenter, IDC_EMPTY, numEmptyHolds, 0);

	// Colos
	SetDlgItemInt(showinfocenter, IDC_COLOS, numColos, 0);
  
	// Cloaks
	SetDlgItemInt(showinfocenter, IDC_CLOAKS, numCloaks, 0);
  
	// Photons
	SetDlgItemInt(showinfocenter, IDC_PHOTON, numPhotons, 0);
  
	// Probes
	SetDlgItemInt(showinfocenter, IDC_PROBES, numProbes, 0);


	// Mines
	hwnd = GetDlgItem(showinfocenter, IDC_MINES);
	if (hwnd) {
		sprintf (buffer2, "%ld/%ld", numAMines, numLMines);
		SetWindowText(hwnd, buffer2);
	}

	// twxsync packets
	hwnd = GetDlgItem(showinfocenter, IDC_TWXSYNC_PACKET_INFO);
	if (hwnd) {
		sprintf (buffer2, "%ld(s)/%ld(r)", twxSyncPacketsSent, twxSyncPacketsReceived);
		SetWindowText(hwnd, buffer2);
	}
	

	ReleaseMutex(StatusBarMutex);

}

COLORREF GetColorForText(HDC hdc, HWND hwnd)
{
	char str[INFO_LENGTH];
//	int strLength = GetWindowTextLength(hwnd);
	COLORREF color = GREEN;
	
	if (ctlAlign == hwnd) 
	{
		// if alignment is less than 0, make it red
		GetWindowText(hwnd, str, INFO_LENGTH);

		if (strlen(str)>0 && str[0] == '-')
			color = RED;
	}
	else if (ctlTurns == hwnd) 
	{
		// if alignment is less than 0, make it red
		GetWindowText(hwnd, str, INFO_LENGTH);

		if (atol(str) < 50)
			color = RED;
	}

	// if they have 0, BLUE
	if (ctlFigs == hwnd || ctlShields == hwnd || ctlTurns == hwnd || ctlCredits == hwnd) 
	{
		GetWindowText(hwnd, str, INFO_LENGTH);

		if (strlen(str)>0 && str[0] == '0')
			color = BLUE;
	}

	if (ctlTurns == hwnd) 
	{
		GetWindowText(hwnd, str, INFO_LENGTH);

		if (atol(str) <= 30)
			color = RED;
	}

	return color;
}

COLORREF GetColorForTextFromAnsiString(char str[])
{
	char *tmp, *q, *end;
	
	COLORREF ret = YELLOW;
	int code;

	tmp = (char *) strdup(str);

	do {
		if (!str)
			break;
	
		q = strstr(tmp, "\\e[");
		if (!q) 
			break;
	
		end = strstr(q, "m");
		if (!end) 
			break;
	
		*end = '\0';
		q += 3;
	
		/* at this point, q should contain a ; delimited color sequence */
		end = strtok(q, ";");
		if (!end) 
			break;

		code = atoi(end);
	
		switch (code) 
		{
			case ANSI_RED: 
				ret = RED;
				break;
			case ANSI_GREEN: 
				ret = GREEN;
				break;
			case ANSI_YELLOW: 
				ret = YELLOW;
				break;
			case ANSI_BLUE: 
				ret = BLUE;
				break;
			case ANSI_MAGENTA: 
				ret = MAGENTA;
				break;
			case ANSI_CYAN: 
				ret = CYAN;
				break;
			case ANSI_WHITE: 
				ret = WHITE;
				break;
		}
	} while(0);

	#ifdef P_FREE
		pfree(&tmp);
	#else
		free(tmp);
	#endif		

	return ret;
}

char* StripAnsiFromString(char str[])
{
	char *newstr;
	char *tmp;
	if (!str)
	{
		newstr = (char *) strdup (" ");
		return (newstr);
	}

	tmp = (char *) strdup(str);

	if (Instr (1, tmp, "\\e["))
	{
		if (strlen(tmp)>6)
			newstr = (char *) right (tmp,  strlen(tmp)-6);
		else
			newstr = (char *) strdup (tmp);

		#ifdef P_FREE
			pfree(&tmp);
		#else
			free(tmp);
		#endif		
		tmp = newstr;
	}

	return tmp;
}

BOOL attacMessage(char* msg, COLORREF color)
{
	return PostMessage(showmessagecenter, WM_MSG, (WPARAM) msg, (LPARAM) color);
}

void addToMessageCenter(char* msg, int fgcolor, int bgcolor, BOOL flag)
{
	COLORREF color = YELLOW;
	char* msg2=NULL;
	char *t=NULL;
	int tLen=0, i=0;
	const char cont[]="[Continued] ";
	
	for (i=0; i< ceil((double) ( (double) strlen(msg)/MAX_COLUMNS)); i++) {
		char dest[MAX_COLUMNS+1];

		strncpy (dest, msg+(MAX_COLUMNS*i), MAX_COLUMNS);
		dest[MAX_COLUMNS]=0;
		dest[strlen(dest)]=0;

		msg2 = (char*) malloc(strlen(dest)+32);

		sprintf(msg2, "\\e[%dm", fgcolor);
		if (i>0) {
			t = (char *) malloc(strlen(cont)+strlen(msg2)+strlen(dest)+1);
			strcpy(t, msg2);
			strcat(t, cont);
			t[strlen(t)]=0;
			#ifdef P_FREE
				pfree(&msg2);
			#else
				free(msg2);
			#endif		
			msg2=t;
		}
		strcat(msg2, dest);

		tLen = strlen(msg2);
		if (tLen>0)
			msg2[tLen]=0;

		attacMessage(msg2, color);
	}
	// free memory from malloc of msg2 in callback
}

void setScriptMessage(char* msg)
{
	HWND hwnd;

	// script count
	hwnd = GetDlgItem(showinfocenter, IDC_SCRIPT);
	if (hwnd) {
		SetWindowText(hwnd, msg);
	}
}

char* addCommaToNumber(long number, int shrink)
{
	char buffer[50], numStr[30], tmpStr[30];
	int level;
	long shift;
	long absnum = number;
	int sign = 0, i, j=0;
	long major, minor;
	char *tmp;
	char levels[] = {' ', 'k', 'M', 'B', 'T'};

	if (number < 0)
	{
		absnum = -number;
		sign = 1;
	}

	if (absnum >= 100000 && shrink==1)
	{
		level = 1;
		shift = 1000;
		while (absnum / shift > 1000)
		{
			level++;
			shift *= 1000;
		}

		major = absnum / shift;
		sprintf (tmpStr, "%ld", major);

		sprintf (numStr, "%ld", absnum);
		for (i=(int)strlen(tmpStr); i< (int) strlen(numStr)-1 && i<(int)strlen(tmpStr)+2 && i<20; i++)
			numStr[j++] = numStr[i];

		numStr[j]=0;

		minor = atol(numStr);

		if (sign)
		{
			if (minor != 0)
				sprintf(buffer, "-%ld.%s%c", major, numStr, levels[level]);
			else
				sprintf(buffer, "-%ld%c", major, levels[level]);
		}
		else
		{
			if (minor != 0)
				sprintf(buffer, "%ld.%s%c", major, numStr, levels[level]);
			else
				sprintf(buffer, "%ld%c", major, levels[level]);
		}
	}
	else
	{
		sprintf(buffer, "%ld", number);
	}

	tmp = (char*) strdup (buffer);

	return (tmp);
}
