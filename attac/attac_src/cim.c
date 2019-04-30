// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "util.h"
#include "cim.h"
#include "rexx.h"

int parseCIMData=0;
int GetCIMData=3;
long lastCimRunTime;

void startCIM()
{
  int i;
  char tmp;
  char tmp2[2];


  tmp2[0] = 'c';
  tmp2[1] = 0;
  attac_send(tmp2);
  lastCimRunTime=Get_Time();
  header.lastTimeCIMRun = lastCimRunTime;

  for (i = 200; i <= 205; i++)
  {
    tmp = (char) i;
    attac_send_char(tmp);
  }

  parseCIMData = 1;
  
  strcpy (DSM_Buffer,"");
  strcpy (DSM_BufferParseData,"");
  strcpy (DSM_BufferStatus,"");

  if (GetCIMData == 1 || GetCIMData == 3)
  {
    attac_send_char('i');
	CIMParseType = 1;
  }
  if (GetCIMData == 2)
  {
    attac_send_char('r');
	CIMParseType = 2;
  }


}

int parseCIMFromTextFile(char * filename)
{
	char tmpCIMBuffer[MAX_LENGTH];
	int countParens=0;
	int CIMParseType=1;
	FILE *fp;
	char ch;
	char buffer[5];
	char *tmp;

	fp = fopen (filename, "r");
	
	strcpy (tmpCIMBuffer, "");
	ch = fgetc( fp );
	while (ch != EOF)
	{	

		sprintf (buffer, "%c", (char) ch);
		strcat (tmpCIMBuffer, buffer);
		if (strlen (tmpCIMBuffer) > (MAX_LENGTH-5))
		{
			tmp = (char *) right (tmpCIMBuffer, 50);
			strcpy (tmpCIMBuffer, "");
			strcpy (tmpCIMBuffer, tmp);
			#ifdef P_FREE
				pfree(&tmp);
			#else
				free(tmp);
			#endif		
		}

		if (Instr (1, tmpCIMBuffer, "\n") || Instr (1, tmpCIMBuffer, ": "))
		{
			if (Instr (1, tmpCIMBuffer, "   1    2"))
				CIMParseType = 1;
			if (Instr (1, tmpCIMBuffer, "    1     2"))
				CIMParseType = 1;
			if (Instr (1, tmpCIMBuffer, "%"))
				CIMParseType = 2;

			if (Instr (1, tmpCIMBuffer, "Computer"))
			{	
				countParens=0;
			}
			else if (Instr (1, tmpCIMBuffer, ":"))
			{
				countParens++;

				if (GetCIMData==3 && countParens==2)
				{
					CIMParseType = 2;
				}
				else if (GetCIMData==3 && countParens>2)
				{
					parseCIMData=0;
					writeMemory();
				}
				else if (GetCIMData<3 && countParens>1)
				{
					parseCIMData=0;
					writeMemory();
				}

			}
			else if (isalpha(tmpCIMBuffer[0]))
				;
			else if (tmpCIMBuffer[0]==13)
				;
			else if (tmpCIMBuffer[0]=='\n')
				;
			else
			{	
				tmp = (char *) strdup (tmpCIMBuffer);
				parseCIMInformation(CIMParseType, tmp);
				#ifdef P_FREE
					pfree(&tmp);
				#else
					free(tmp);
				#endif		
			}
			strcpy(tmpCIMBuffer,"");
		}
		ch = fgetc( fp );
	}

	if (fp !=NULL)
		fclose(fp);

	return 1;
}
