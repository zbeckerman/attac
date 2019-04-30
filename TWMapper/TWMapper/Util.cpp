#include <stdio.h>
#include <windows.h>
#include <string.h>

#include "stdafx.h"
#include "Util.h"
#include "Sector.h"
#include "Queue.h"

#define CR 13            /* Decimal code of Carriage Return char */
#define LF 10            /* Decimal code of Line Feed char */
#define EOF_MARKER 26    /* Decimal code of DOS end-of-file marker */
#define MAX_REC_LEN 1024 /* Maximum size of input buffer */

char * readLineFromFile(FILE *fp) ;

//Constructors
CUtil::CUtil()
{
}

//Destructor
CUtil::~CUtil()
{
}

CObList* CUtil::loadMapFromSector (long sectorNum, int maxSectors) {
	int currentLevel=0;
	CQueue *queue = new CQueue();
	CObList *returnList = new CObList();
	CObList *sectorList = this->getSectorsFromDatabase();

	CSector* sector = getSectorFromList (sectorList, sectorNum);
	queue->push(sector);

	while (!queue->isQueueEmpty() && currentLevel<maxSectors) {
		CSector* popSector = queue->pop();
		if (popSector->sectorNumber==1)
			popSector->stardock=TRUE;

		returnList->InsertAfter(returnList->GetTailPosition(), (CObject*) popSector);
		for (int i=0; i<popSector->sectorCount; i++) {
			CSector* warpSector = getSectorFromList (sectorList, popSector->warps[i]);
			queue->push(warpSector);
		}
		currentLevel++;
	}

	return returnList;
}

CSector* CUtil::getSectorFromList (CObList* theList, long sectorNum) {
	int count = (int) theList->GetCount();
	for (int i=0; i<count; i++) {
		CSector* sector = (CSector*) theList->GetAt(theList->FindIndex(i));
		if (sector->Equals(sectorNum))
			return sector;
	}
	return NULL;
}

CObList* CUtil::getSectorsFromDatabase () {
	CObList *sectorList = new CObList();
	FILE *fp=NULL;
	char *readLine=NULL;
	char *token;
	fp = fopen ("cim.txt", "r");
	int i=0;

	
	while (1)
	{
		if (fp==NULL)
			break;
		/* Read the next line with the appropriate read function */
		 readLine = (char *) readLineFromFile(fp); 

		 if (strcmp(readLine, "")==0) {
			 free(readLine);
			 break;
		 }

 		 printf ("%s\n",readLine);
		 token = strtok (readLine, " ");
		CSector *sector = NULL;
		 if (atol(token)>0) {
			i=0;
			sector = new CSector();
			while (token != NULL) {
				if (i==0)
					sector->sectorNumber=atol(token);
				if (i==1)
					sector->warps[0]=atol(token);
				if (i==2)
					sector->warps[1]=atol(token);
				if (i==3)
					sector->warps[2]=atol(token);
				if (i==4)
					sector->warps[3]=atol(token);
				if (i==5)
					sector->warps[4]=atol(token);
				if (i==6)
					sector->warps[5]=atol(token);

				sector->sectorCount=i;
				i++;
				token = strtok (NULL, " ");
			}
		 }
		
		 if (sector!=NULL)
			 sectorList->AddTail((CObject*)sector);
		 free(readLine);
	}
	if (fp != NULL)
		fclose (fp);
	return sectorList;

}


/***************************************/
/* Use:       Read next line of text   */
/*            file with fgetc          */
/*                                     */
/* Arguments: FILE *input              */
/*              Pointer to input file  */
/*            char *output             */
/*              Read buffer            */
/*                                     */
/* Return:    int                      */
/*              <0 = error             */
/*               0 = end of file       */
/*               1 = line read okay    */
/***************************************/
char * readLineFromFile(FILE *fp) 
{
	char buffer[MAX_REC_LEN];
	int i=0;

	while (1) {
		int c = fgetc (fp);

		if (c==LF || c==EOF) {
			break;
		}
			
		if (c!=CR) {
			buffer[i++]=c;
		}
	}
	buffer[i]=0;
	return strdup (buffer);
} 

