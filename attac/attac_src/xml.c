// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#ifdef _MSC_VER
//#include <io.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "attacGlobal.h"
#include "attacUtils.h"
#include "util.h"
#include "xml.h"
#include "structs.h"
#include "twxSync.h"
#include "../PUTTY.h"
#include "./3PP/expat/expat.h"

typedef struct {
  XML_Parser parser;
  int *retPtr;
} PROCESS_ARGS;

int Depth=0;
char *XMLTreeTag[RECURSIVE_TAGS];

long twxSectorNumber=0;
long lastUpdated=0;
long twxVersion=0;
char twxCreatedBy[MAX_LENGTH];
long twxCreatedOn=0;
char traderName[MAX_LENGTH];
char ships[MAX_LENGTH];
char trader[MAX_LENGTH];
char user[MAX_LENGTH];
char helper[MAX_LENGTH];
char someoneElseLoginId[MAX_LENGTH];

int corp=0;
long finished=0;
long hops=0;

void reportError(XML_Parser parser, const XML_Char *filename)
{
  int code = XML_GetErrorCode(parser);
  char buffer[MAX_LENGTH];
  const XML_Char *message = XML_ErrorString(code);
  if (message)
    sprintf(buffer, T("%s:%d:%d: %s\n"),
             filename,
             XML_GetErrorLineNumber(parser),
             XML_GetErrorColumnNumber(parser),
             message);
  else
    sprintf(buffer, T("%s: (unknown message %d)\n"), filename, code);
  attacErrorMessage(buffer);
}

#ifdef WIN32

int isAsciiLetter(XML_Char c)
{
  return (T('a') <= c && c <= T('z')) || (T('A') <= c && c <= T('Z'));
}

#endif /* WIN32 */



int processStream(const XML_Char *filename, XML_Parser parser)
{
  /* passing NULL for filename means read intput from stdin */
  FILE * fd; 
  char Buff[BUFFSIZE];
  char buffer[MAX_LENGTH];

  if (filename != NULL) {
   	  if( (fd = fopen(filename, "rb")) == NULL ) {
		  perror(filename);
      return -1;
    }
  }

   for (;;) {
    int done;
    int len;

    len = fread(Buff, 1, BUFFSIZE, fd);
    if (ferror(stdin)) {
      sprintf(buffer, "Read error\n");
	  attacErrorMessage(buffer);
      return -1;
    }
    done = feof(fd);

    if (XML_Parse(parser, Buff, len, done) == XML_STATUS_ERROR) {
      sprintf(buffer, "Parse error at line %d:\n%s\n",
              XML_GetCurrentLineNumber(parser),
              XML_ErrorString(XML_GetErrorCode(parser)));
	  attacErrorMessage(buffer);
      return -1;
    }

    if (done)
      break;
  }
  return 0;
}

void XMLCALL startElement(void *userData, const XML_Char *name, const XML_Char **atts)
{
	char * escapedName = NULL;
	char * unescapedName = NULL;
 
  XMLTreeTag[Depth] = (char *) strdup (name);

  while (*atts) {
	//printf ("attribute: %s\n", *atts);
	if (strcmp (*atts,"nr")==0)
		twxSectorNumber = atol((*(atts+1)));
	else if (strcmp (*atts,"updated")==0) {
		lastUpdated = atol((*(atts+1)));
		if (strcmp(name,"Trader") ==0) {
			int n = getTradeNumber (trader);
			if (n>=0 && strcmp (trader, "") != 0 && strcmp(players[n].name, "")!=0) {
				players[n].last_update = lastUpdated;
			}
		}
	}
	else if (strcmp (*atts,"discovered")==0)
		lastUpdated = atol((*(atts+1)));
	else if (strcmp (*atts,"name")==0) {
		escapedName = strdup ((char *) (*(atts+1)));
		unescapedName = unescapeData(escapedName);
		strcpy (traderName, unescapedName);
		pfree((void**) &unescapedName);

	}
	else if (strcmp (*atts,"ships")==0)
		strcpy (ships, (char *) (*(atts+1)));
	else if (strcmp (*atts,"corp")==0)
		if (strcmp((*(atts+1)), "true")==0 || strcmp((*(atts+1)), "True")==0)
			corp= 1;
		else
			corp=0;
	else if (strcmp (*atts,"finished")==0)
		finished= atol((*(atts+1)));
	else if (strcmp (*atts,"hops")==0)
		hops= atol((*(atts+1)));
	else if (strcmp (*atts,"version")==0)
		twxVersion= atol((*(atts+1)));
	else if (strcmp(name,"LastBust") ==0 && strcmp (*atts,"trader")==0) {
		escapedName = strdup ((char *) (*(atts+1)));
		unescapedName = unescapeData(escapedName);
		strcpy (trader, unescapedName);
		pfree((void**) &unescapedName);

		if (strcmp(name,"Trader") ==0) {
			int n = getTradeNumber (trader);
			if (n>=0  && strcmp (trader, "") != 0) {
				strcpy (players[n].name, trader);
			}
		}
    }
	atts++;
  }


  Depth++;
}

void XMLCALL endElement(void *userData, const XML_Char *name)
{

	Depth--;

	#ifdef P_FREE
		pfree(&XMLTreeTag[Depth]);
	#else
		free(XMLTreeTag[Depth]);
	#endif	


}

/* Lexicographically comparing UTF-8 encoded attribute values,
is equivalent to lexicographically comparing based on the character number. */

int
attcmp(const void *att1, const void *att2)
{
  return strcmp(*(const XML_Char **)att1, *(const XML_Char **)att2);
}


void XMLCALL characterData(void *userData, const XML_Char *s, int len)
{
	char buffer[MAX_LENGTH];
	int i=0;

	char *offset = userData;
  for (; len > 0; --len, ++s) {
	  switch (*s) {
//    case T('&'):
//      fputs(T("&amp;"), fp);
      break;
    case T('<'):
//      fputs(T("&lt;"), fp);
      break;
    case T('>'):
//      fputs(T("&gt;"), fp);
      break;
#ifdef W3C14N
    case 13:
#ifdef USE_FAKE_LF
//      fputs(T("&#xD;"), fp);
#else
//		fputs(T("\n"), fp);
#endif

      break;
#else
//    case T('"'):
  //    fputs(T("&quot;"), fp);
      break;
    case 9:
    case 10:
    case 13:
#ifdef USE_FAKE_LF
//      fprintf(fp, T("&#%d;"), *s);
#else
//		fputs(T("\n"), fp);
#endif
      break;
#endif
    default:
	  buffer[i++] = *s;
      break;
    }
  }
  buffer[i++]=0;

  parseXMLImportData(atoi(offset), Depth, XMLTreeTag, buffer);
}


void  parseXMLImportData(int tOffset, int tDepth, char *tXMLTreeTag[], char tBuffer[]) {
	int i=0;
	long tmpLong=0;
	int tmpInt=0, tmpInt2=0;

	if (tDepth>(0+tOffset) && (strcmp (tXMLTreeTag[0+tOffset],"TWX")==0 || strcmp (tXMLTreeTag[0+tOffset],"Update")==0) && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
		if (tDepth>(1+tOffset) && strcmp (tXMLTreeTag[1+tOffset],"Header")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
			if (tDepth>(2+tOffset) && strcmp (tXMLTreeTag[2+tOffset],"Created")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Date")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
					twxCreatedOn = atol (tBuffer);
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"By")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
					strcpy(twxCreatedBy, tBuffer);
				}		
			}	
			if (tDepth>(2+tOffset) && strcmp (tXMLTreeTag[2+tOffset],"Game")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Name")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Host")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Port")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Version")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"MBBS")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Gold")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Sectors")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"StarDock")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
					tmpLong = atol(tBuffer);
					if (tmpLong>=-1 && tmpLong < header.sector_cnt)
						header.stardock = tmpLong;
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Class0Port")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
					tmpLong = atol(tBuffer);
					if (tmpLong>=-1 && tmpLong < header.sector_cnt)
						header.cls0port_alpha = tmpLong;
				}		
			}		
			
		}
		if (tDepth>(1+tOffset) && strcmp (tXMLTreeTag[1+tOffset],"TraderData")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
			if (tDepth>(2+tOffset) && strcmp (tXMLTreeTag[2+tOffset],"Trader")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				tmpInt = getTradeNumber (traderName);
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Experience")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
					players[tmpInt].experience = atol(tBuffer);
					players[tmpInt].last_update = lastUpdated;
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Alignment")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
					tmpLong = atol(tBuffer);
					if (tmpLong>=-1 && tmpLong<5000000)
						players[tmpInt].alignment = atol(tBuffer);
					
					players[tmpInt].last_update = lastUpdated;
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Corporation")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
					tmpInt2 = atoi(tBuffer);
					if (tmpInt2>=-1 && tmpInt2<20000)
						players[tmpInt].corpNumber = tmpInt2;
					players[tmpInt].last_update = lastUpdated;
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Rank")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
					players[tmpInt].last_update = lastUpdated;
				}		
			}	
		}
		if (tDepth>(1+tOffset) && strcmp (tXMLTreeTag[1+tOffset],"CorporationData")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
			if (tDepth>(2+tOffset) && strcmp (tXMLTreeTag[2+tOffset],"Corporation")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Name")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"CEO")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Corporation")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				}		
			}	
		}
		if (tDepth>(1+tOffset) && strcmp (tXMLTreeTag[1+tOffset],"ShipData")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
			//not storing this
		}
		if (tDepth>(1+tOffset) && strcmp (tXMLTreeTag[1+tOffset],"PlanetData")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
			//not storing this
		}
		if (tDepth>(1+tOffset) && strcmp (tXMLTreeTag[1+tOffset],"SectorData")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
			if (tDepth>(2+tOffset) && strcmp (tXMLTreeTag[2+tOffset],"Sector")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
				//if there is no sector data added, then add it.
				if (sectors[twxSectorNumber-1].info==-1) {
					sectors[twxSectorNumber-1].info=10;
				}
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Space")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
				}
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Beacon")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
			
				}
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Warps")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Warp")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						int foundWarp=0;

						tmpLong = atol(tBuffer);
						i=0;
						//check if the warp exists
						for (i=0; foundWarp==0 && i<6; i++) {
							if 	(sectors[twxSectorNumber-1].warp_sect[i] == tmpLong) {
								foundWarp=1;
							}
						}
						//means the warp was no there
						if (foundWarp==0) {
							sectors[twxSectorNumber-1].warp_sect[sectors[twxSectorNumber-1].warps++]=tmpLong;
							sectors[twxSectorNumber-1].sector_update = lastUpdated;
						}
					}
				}
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Port")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].port_update) {
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Name")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Class")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						tmpInt = atoi(tBuffer);
						if (tmpInt>-10 && tmpInt <40)
							sectors[twxSectorNumber-1].info=tmpInt;
						sectors[twxSectorNumber-1].port_update = lastUpdated;
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"UnderConstruction")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						tmpInt = atoi(tBuffer);
						if (tmpInt>-10 && tmpInt <40) {
							if (sectors[twxSectorNumber-1].info<UNDER_CONSTRUCTION)
								sectors[twxSectorNumber-1].info+=UNDER_CONSTRUCTION;
						}
						sectors[twxSectorNumber-1].port_update = lastUpdated;
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Destroyed")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						tmpInt = atoi(tBuffer);
						if (tmpInt>-10 && tmpInt <40) {
							sectors[twxSectorNumber-1].info=12;
						sectors[twxSectorNumber-1].port_update = lastUpdated;
						}
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"FuelOre")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						if (tDepth>(5+tOffset) && strcmp (tXMLTreeTag[5+tOffset],"Amount")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
							tmpLong = atol(tBuffer);
							if (tmpLong>=-1 && tmpLong <= 1000000)
								sectors[twxSectorNumber-1].port_amt[0] = tmpLong;
							sectors[twxSectorNumber-1].port_update = lastUpdated;
						}
						if (tDepth>(5+tOffset) && strcmp (tXMLTreeTag[5+tOffset],"Percentage")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
							tmpInt = atoi(tBuffer);
							if (tmpInt>=-1 && tmpInt <= 100)
								sectors[twxSectorNumber-1].port_per[0] = tmpInt;
							sectors[twxSectorNumber-1].port_update = lastUpdated;
						}
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Organics")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						if (tDepth>(5+tOffset) && strcmp (tXMLTreeTag[5+tOffset],"Amount")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
							tmpLong = atol(tBuffer);
							if (tmpLong>=-1 && tmpLong <= 1000000)
								sectors[twxSectorNumber-1].port_amt[1] = tmpLong;
							sectors[twxSectorNumber-1].port_update = lastUpdated;
						}
						if (tDepth>(5+tOffset) && strcmp (tXMLTreeTag[5+tOffset],"Percentage")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
							tmpInt = atoi(tBuffer);
							if (tmpInt>=-1 && tmpInt <= 100)
								sectors[twxSectorNumber-1].port_per[1] = tmpInt;
							sectors[twxSectorNumber-1].port_update = lastUpdated;
						}
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Equipment")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						if (tDepth>(5+tOffset) && strcmp (tXMLTreeTag[5+tOffset],"Amount")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
							tmpLong = atol(tBuffer);
							if (tmpLong>=-1 && tmpLong <= 1000000)
								sectors[twxSectorNumber-1].port_amt[2] = tmpLong;
							sectors[twxSectorNumber-1].port_update = lastUpdated;
						}
						if (tDepth>(5+tOffset) && strcmp (tXMLTreeTag[5+tOffset],"Percentage")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
							tmpInt = atoi(tBuffer);
							if (tmpInt>=-1 && tmpInt <= 100)
								sectors[twxSectorNumber-1].port_per[2] = tmpInt;
							sectors[twxSectorNumber-1].port_update = lastUpdated;
						}
					}
				}
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Fighters")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Amount")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						tmpLong = atol(tBuffer);
						if (tmpLong>=-1 && tmpLong <= 10000000000)
							sectors[twxSectorNumber-1].ftrs = tmpLong;
						sectors[twxSectorNumber-1].sector_update = lastUpdated;
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Type")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						if (strcmp (tBuffer, "Toll")==0)
							sectors[twxSectorNumber-1].ftr_type = 1;
						else if (strcmp (tBuffer, "Offensive")==0)
							sectors[twxSectorNumber-1].ftr_type = 2;
						else if (strcmp (tBuffer, "Defensive")==0)
							sectors[twxSectorNumber-1].ftr_type = 3;
						else if (strcmp (tBuffer, "Mercenary")==0)
							sectors[twxSectorNumber-1].ftr_type = 0;
						else 
							sectors[twxSectorNumber-1].ftr_type = -1;
						sectors[twxSectorNumber-1].sector_update = lastUpdated;
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Owner")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						tmpInt = getTradeNumber (traderName);
						if (corp==1)
							sectors[twxSectorNumber-1].ftr_owner = atol(tBuffer)+CORP_CONSTANT;
						else
							sectors[twxSectorNumber-1].ftr_owner = tmpInt;
						sectors[twxSectorNumber-1].sector_update = lastUpdated;
					}
				}
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"ArmidMines")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Amount")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						tmpInt = atoi(tBuffer);
						if (tmpInt>=-1 && tmpInt<= 10000)
							sectors[twxSectorNumber-1].armids = tmpInt;
						sectors[twxSectorNumber-1].sector_update = lastUpdated;
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Owner")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						tmpInt = getTradeNumber (traderName);
						sectors[twxSectorNumber-1].ftr_owner = tmpInt + (corp * CORP_CONSTANT);
						sectors[twxSectorNumber-1].sector_update = lastUpdated;
					}
				}
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"LimpetMines")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Amount")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						tmpInt = atoi(tBuffer);
						if (tmpInt>=-1 && tmpInt<= 10000)
							sectors[twxSectorNumber-1].limpets = tmpInt;
						sectors[twxSectorNumber-1].sector_update = lastUpdated;
					}
					if (tDepth>(4+tOffset) && strcmp (tXMLTreeTag[4+tOffset],"Owner")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer)) {
						tmpInt = getTradeNumber (traderName);
						sectors[twxSectorNumber-1].ftr_owner = tmpInt + (corp * CORP_CONSTANT);
						sectors[twxSectorNumber-1].sector_update = lastUpdated;
					}
				}
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Density")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
					tmpLong = atol(tBuffer);
					if (tmpLong>=-1 && tmpLong <= 10000000000)
						sectors[twxSectorNumber-1].density = tmpLong;		
					sectors[twxSectorNumber-1].sector_update = lastUpdated;
				}
//zzzz
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"LastBust")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
					tmpLong = atol(tBuffer);
					//make sure it was me that busted
					if (tmpLong>=-1) {
						int traderNumber = getTradeNumberWithoutAdding(trader);
						//check the trader name. 0 means that it is me
						//greater than 1 means it isnt
						sectors[twxSectorNumber-1].playerIdBusted = traderNumber;	
						sectors[twxSectorNumber-1].lastBusted = tmpLong;	
						sectors[twxSectorNumber-1].sector_update = lastUpdated;
					}
				}


				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"Anomaly")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
			
				}
				if (tDepth>(3+tOffset) && strcmp (tXMLTreeTag[3+tOffset],"NavHaz")==0 && isValidSector(twxSectorNumber) && hasCharacters(tBuffer) && lastUpdated >= sectors[twxSectorNumber-1].sector_update) {
					tmpInt = atoi(tBuffer);
					if (tmpInt>=-1 && tmpInt<= 10000)
						sectors[twxSectorNumber-1].navhaz = tmpInt;
					sectors[twxSectorNumber-1].sector_update = lastUpdated;
				}
			}
		}
	}
}

int hasCharacters (char buffer[]) {
	int i=0;
	
	if (twxSectorNumber<0 || twxSectorNumber >=header.sector_cnt)
		return 0;

	for (i=0; i<(int) strlen (buffer); i++)
		if (buffer[i] != ' ')
			return 1;

	return 0;
}

int isValidSector (long sectorNum) {
	if (sectorNum-1>=0 && sectorNum-1 <header.sector_cnt)
		return 1;
	else
		return 0;
}


char* import_twxv2 (char * filename, int offsetInt) {
	FILE *outFp=NULL;
	char *outputData;
    XML_Parser parser;
	int result=0;
	time_t myTime;
	char bufTime[40];
	char tmpBuffer[MAX_LENGTH];
    char *offset=NULL;

	sprintf (bufTime, "%d", offsetInt);
	offset = (char *) strdup (bufTime);

	parser = XML_ParserCreate(NULL);
	
	XML_SetUserData(parser, offset);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, characterData);
	result = processStream(filename, parser);
	
	#ifdef P_FREE
		pfree(&offset);
	#else
		free(offset);
	#endif	

	if (result==0) {
		myTime = twxCreatedOn;
		strcpy(bufTime, ctime(&myTime));
		bufTime[strlen(bufTime)-1]=0;

		sprintf (tmpBuffer, "TWX v%d file %s created by %s on %s has been imported", twxVersion, filename, twxCreatedBy, bufTime);
		outputData = (char *) strdup ((char *)tmpBuffer);
	} else {
		outputData = (char *) strdup (" ");
	}

	XML_ParserFree(parser);

	return outputData;
}

int export_twxv2 (char *filename, long startTime, long endTime, int exportToTWXSync) {
	FILE *fp=NULL;
	long i=0, j=0;
	char *unescapedName = NULL;
	char *escapedName = NULL;

	fp = fopen (filename, "wt");
	if (fp==NULL) {
		return (-1);
	}

	if (exportToTWXSync==0) {
		fprintf (fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		fprintf (fp, "<TWX version=\"2\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://www.tw-attac.com/TWX.xsd\">\n");
	}


	
	//header
    fprintf (fp, "  <Header>\n");
    fprintf (fp, "    <Created>\n");
    fprintf (fp, "      <Date>%ld</Date>\n", Get_Time());
    fprintf (fp, "      <By>ATTAC v%s.%s.%s</By>\n", releaseNumber, majorNumber, minorNumber);
    fprintf (fp, "    </Created>\n");
    fprintf (fp, "    <Game>\n");
    fprintf (fp, "      <Name>%s</Name>\n", header.name);
    fprintf (fp, "      <Host>%s</Host>\n", header.address);
    fprintf (fp, "      <Port>%d</Port>\n", header.port);
//    fprintf (fp, "      <Version>%s</Version>\n", header.twVersion);
//    fprintf (fp, "      <MBBS>%s</MBBS>\n", header.isMBBS);
//    fprintf (fp, "      <Gold>%s</Gold>\n", header.isGoldVersion);
    fprintf (fp, "      <Sectors>%ld</Sectors>\n", header.sector_cnt);
	if (header.stardock>0 && isValidSector(header.stardock)) {
	    fprintf (fp, "      <StarDock>%ld</StarDock>\n", header.stardock);

	}
	if (header.cls0port_sol>0 && isValidSector(header.cls0port_sol)) {
		fprintf (fp, "      <Class0Port>%ld</Class0Port>\n", header.cls0port_sol);
	}
	if (header.cls0port_alpha>0 && isValidSector(header.cls0port_alpha)) {
		fprintf (fp, "      <Class0Port>%ld</Class0Port>\n", header.cls0port_alpha);
	}
	if (header.cls0port_rylos>0 && isValidSector(header.cls0port_rylos)) {
	    fprintf (fp, "      <Class0Port>%ld</Class0Port>\n", header.cls0port_rylos);
	}
    fprintf (fp, "    </Game>\n");
    fprintf (fp, "  </Header>\n");
	//trader data
    fprintf (fp, "  <TraderData>\n");

	for (i=0; i<MAX_PLAYERS; i++) {
		if (players[i].autonum >=0 && players[i].last_update >= startTime && players[i].last_update <= endTime) {
			unescapedName = strdup ((char *) players[i].name);
			escapedName = escapeData(unescapedName);

			fprintf (fp, "  <Trader name=\"%s\" updated=\"%ld\">\n", escapedName, players[i].last_update);

			pfree((void**) &escapedName);
			
			if (players[i].experience>0) {
				fprintf (fp, "	  <Experience>%ld</Experience>\n", players[i].experience);
			}
			if (players[i].alignment>0) {
				fprintf (fp, "	  <Alignment>%ld</Alignment>\n", players[i].alignment);
			}
			if (players[i].corpNumber>0) {
				fprintf (fp, "	  <Corporation>%ld</Corporation>\n", players[i].corpNumber);
			}
//		    fprintf (fp, "	  <Rank>%ld</Rank>\n");
			fprintf (fp, "  </Trader>\n");
		}
	}
    
	fprintf (fp, "  </TraderData>\n");


    fprintf (fp, "  <SectorData>\n");

	for (i=0; i<header.sector_cnt; i++) {
		if (sectors[i].info>=0 &&
				(sectors[i].sector_update >= startTime && sectors[i].sector_update <= endTime ||
				sectors[i].port_update >= startTime && sectors[i].port_update <= endTime )
				)
		{
			fprintf (fp, "  <Sector nr=\"%ld\" updated=\"%ld\">\n", i+1, sectors[i].sector_update);
//			if (strcmp (sectors[i].space, "")==0) {
//			    fprintf (fp, "	  <Space>%ld</Space>\n", sectors[i].space);
//		}
//			if (strcmp (sectors[i].beacon, "")==0) {
//			    fprintf (fp, "	  <Beacon>%ld</Beacon>\n", sectors[i].space);
//		}


			//sector warps
			if (sectors[i].sector_update >= startTime && sectors[i].sector_update <= endTime) {
				fprintf (fp, "	  <Warps>\n");
				for (j=0; j<sectors[i].warps && j<6; j++) {
					if (isValidSector(sectors[i].warp_sect[j]))
						fprintf (fp, "	    <Warp>%ld</Warp>\n", sectors[i].warp_sect[j]);
				}
				fprintf (fp, "	  </Warps>\n");
			}
			
			//port data
			if (sectors[i].port_update >= startTime && sectors[i].port_update <= endTime) {
				if ((sectors[i].info>0 && sectors[i].info < 10) || sectors[i].info > UNDER_CONSTRUCTION) {
					fprintf (fp, "	  <Port updated=\"%ld\">\n", sectors[i].port_update);
	//				fprintf (fp, "	    <Name>%s</Name>\n", sectors[i].portname);
					fprintf (fp, "	    <Class>%d</Class>\n", sectors[i].info % UNDER_CONSTRUCTION);
					//how do you deal with underconstruction
					if (sectors[i].info > UNDER_CONSTRUCTION) {
						fprintf (fp, "	    <UnderConstruction>true</UnderConstruction>\n");

					}
					if (sectors[i].info ==12) {
						fprintf (fp, "	    <Destroyed>true</Destroyed>\n");
				
					}
					//fuel ore
					if (sectors[i].port_amt[0]>=0 || sectors[i].port_per[0]>=0) {
						fprintf (fp, "	    <FuelOre>\n");
			
						if (sectors[i].port_amt[0]>=0) {
							fprintf (fp, "	      <Amount>%ld</Amount>\n", sectors[i].port_amt[0]);
						}
						if (sectors[i].port_per[0]>=0) {
							fprintf (fp, "	      <Percentage>%d</Percentage>\n", sectors[i].port_per[0]);
						}
						fprintf (fp, "	    </FuelOre>\n");
					}
					//organics
					if (sectors[i].port_amt[1]>=0 || sectors[i].port_per[1]>=0) {
						fprintf (fp, "	    <Organics>\n");
						if (sectors[i].port_amt[1]>=0) {
							fprintf (fp, "	      <Amount>%ld</Amount>\n", sectors[i].port_amt[1]);
						}
						if (sectors[i].port_per[1]>=0) {
							fprintf (fp, "	      <Percentage>%d</Percentage>\n", sectors[i].port_per[1]);
						}
						fprintf (fp, "	    </Organics>\n");
					}
					//equipment
					if (sectors[i].port_amt[2]>=0 || sectors[i].port_per[2]>=0) {
						fprintf (fp, "	    <Equipment>\n");
						if (sectors[i].port_amt[2]>=0) {
							fprintf (fp, "	      <Amount>%ld</Amount>\n", sectors[i].port_amt[2]);
						}
						if (sectors[i].port_per[2]>=0) {
							fprintf (fp, "	      <Percentage>%d</Percentage>\n", sectors[i].port_per[2]);
						}
						fprintf (fp, "	    </Equipment>\n");
					}
					fprintf (fp, "	  </Port>\n");
				}
			}

			//fighters data
			if (sectors[i].sector_update >= startTime && sectors[i].sector_update <= endTime) {
				if (sectors[i].ftrs>0) {
					fprintf (fp, "	  <Fighters>\n");
					fprintf (fp, "	    <Amount>%ld</Amount>\n", sectors[i].ftrs);
					if (sectors[i].ftr_type>=0) {
						if (sectors[i].ftr_type==0) {
							fprintf (fp, "	    <Type>Mercenary</Type>\n");
						}
						else if (sectors[i].ftr_type==1) {
							fprintf (fp, "	    <Type>Toll</Type>\n");
						}
						else if (sectors[i].ftr_type==2) {
							fprintf (fp, "	    <Type>Offensive</Type>\n");
						}
						else if (sectors[i].ftr_type==3) {
							fprintf (fp, "	    <Type>Defensive</Type>\n");
						}
					}
					if (sectors[i].ftr_owner>0) {
						if (sectors[i].ftr_owner>CORP_CONSTANT) {
							fprintf (fp, "	    <Owner corp=\"true\">%d</Owner>\n", sectors[i].ftr_owner-CORP_CONSTANT);
						}
						else {
							if (sectors[i].ftr_owner>=0 && sectors[i].ftr_owner<MAX_PLAYERS) {
								fprintf (fp, "	    <Owner corp=\"false\">%s</Owner>\n", players[sectors[i].ftr_owner].name);
							}
						}
					}
					fprintf (fp, "	  </Fighters>\n");
				}
				//armid mines
				if (sectors[i].armids>0) {
					fprintf (fp, "	  <ArmidMines>\n");
					fprintf (fp, "	    <Amount>%ld</Amount>\n", sectors[i].armids);
					if (sectors[i].armid_owner>0) {
						if (sectors[i].armid_owner>CORP_CONSTANT) {
							fprintf (fp, "	    <Owner corp=\"true\">%d</Owner>\n", sectors[i].armid_owner-CORP_CONSTANT);
						}
						else {
							if (sectors[i].armid_owner>=0 && sectors[i].armid_owner<MAX_PLAYERS) {
								fprintf (fp, "	    <Owner corp=\"false\">%s</Owner>\n", players[sectors[i].armid_owner].name);
							}
						}

					}
					fprintf (fp, "	  </ArmidMines>\n");
				}

				//limpit mines
				if (sectors[i].limpets>0) {
					fprintf (fp, "	  <LimpetMines>\n");
					fprintf (fp, "	    <Amount>%ld</Amount>\n", sectors[i].limpets);
					if (sectors[i].limpet_owner>0) {
						if (sectors[i].limpet_owner>CORP_CONSTANT) {
							fprintf (fp, "	    <Owner corp=\"true\">%d</Owner>\n", sectors[i].limpet_owner-CORP_CONSTANT);
						}
						else {
							if (sectors[i].limpet_owner>=0 && sectors[i].limpet_owner<MAX_PLAYERS) {
								fprintf (fp, "	    <Owner corp=\"false\">%s</Owner>\n", players[sectors[i].limpet_owner].name);
							}
						}
					}
					fprintf (fp, "	  </LimpetMines>\n");
				}

				//last busted
				if (sectors[i].lastBusted>0) {
					fprintf (fp, "	  <LastBust trader=\"%s\">%ld</LastBust>\n", players[sectors[i].playerIdBusted].name, sectors[i].lastBusted);
				}


				//density
				if (sectors[i].density>0) {
					fprintf (fp, "	  <Density>%ld</Density>\n", sectors[i].density);
				}
				//Anomaly
				if (sectors[i].anom>=0) {
					if (sectors[i].anom==1) {
						fprintf (fp, "	  <Anomaly>true</Anomaly>\n");
					}
					else {
						fprintf (fp, "	  <Anomaly>false</Anomaly>\n");
					}
				}
				//navhaz
				if (sectors[i].navhaz>0) {
					fprintf (fp, "	  <NavHaz>%d</NavHaz>\n", sectors[i].navhaz);
				}
			}

			fprintf (fp, "  </Sector>\n");
		}
	}
    
	fprintf (fp, "  </SectorData>\n");

	if (exportToTWXSync==0) {
		fprintf (fp, "</TWX>\n");
	}

	fclose (fp);

	return (0);
	
}

