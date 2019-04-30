
#include <signal.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "TWBFind.h"
#include "TWRemaps.hpp"
#include "TWTypes.hpp"
#include "Globals.hpp"
#include "TWSector.hpp"
#include "TWWarp.hpp"
#include "TWBubble.hpp"
#include "AbEnds.hpp"

TWATTAC_API void findATTACBubbles (int argc, char* argv[], TWX_SECTOR *sectors, char *bubbleString)
{
    char sectFileName[256], outFileName[256], argtype[3], argdata[256],
         argtemp[256], binFileName[256], plotFileName[256], auxFileName[256];
	char *returnVal=NULL, *tempReturnVal=NULL, *tempReturnVal2=NULL;
    USHORT numBubbles, i;
    Bool readBinData=false, writeBinData=false,
         readWarpData=false, readPlotData=false,
         doBubbleSearch=true, readAuxData=false;
    Bool minSet=false, maxSet=false, sNumSet=false, eMultSet=false,
         fBinSet=false, fSectSet=false, fPlotSet=false, fOutSet=false,
         tunSet=false, searchSet=false, entSet=false, aUnexSet=false,
         auxSet=false, fakeSet=false, bubMaxSet=false, gobSet=false;
    #if (defined(__OS2__) || defined(__EMX__)) // priority options
    Bool priSet=false, deltaSet=false;
    #endif
    ifstream sectFile;
    ifstream plotFile;
    ofstream outFile;
    fstream binFile;
    fstream auxFile;

    TWBubbleSet bubbleSet, keepSet;

//    signal(SIGINT, AbortCatcher);

	returnVal = (char *) strdup (" ");
    strcpy(sectFileName, "");
    strcpy(binFileName, "");
    strcpy(auxFileName, "");
    strcpy(plotFileName, "");
    strcpy(outFileName, "bubbles.lst");


    if (argc==1)
	{
		if (returnVal != NULL)
			free(returnVal);
        returnVal = BadUsage("", USAGE_HELP);
		strcpy (bubbleString, returnVal);
		free (returnVal);
		return;
	}



    for (i=1; i<argc; i++)
        {
        memset(argtype, 0, sizeof(argtype));
        memset(argdata, 0, sizeof(argdata));
        strncpy(argtype, argv[i], 2);
        strcpy(argdata, argv[i]+2);
        if (!strcmpi(argtype, "/t") || !strcmpi(argtype, "-t"))
            {
            if (tunSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::tunnelEntrance=true;
            tunSet=true;
            }
        else if (!strcmpi(argtype, "/u") || !strcmpi(argtype, "-u"))
            {
            if (searchSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            doBubbleSearch=false;
            searchSet=true;
            }
        else if (!strcmpi(argtype, "/a") || !strcmpi(argtype, "-a"))
            {
            if (aUnexSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::avoidUnexplored=true;
            aUnexSet=true;
            }
        else if (!strcmpi(argtype, "/f") || !strcmpi(argtype, "-f"))
            {
            if (fakeSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::bogusBubbles=true;
            fakeSet=true;
            }
        else if (!strcmpi(argtype, "/g") || !strcmpi(argtype, "-g"))
            {
            if (gobSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::gobbleMemory=true;
            gobSet=true;
            }
        else if (!strcmpi(argtype, "/m") || !strcmpi(argtype, "-m"))
            {
            if (minSet)
				{
					if (returnVal != NULL)
						free(returnVal);
					returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
					strcpy (bubbleString, returnVal);
					free (returnVal);
					return;
				}
            Globals::minSectors=(USHORT)atoi(argdata);
            if (Globals::minSectors==0)
				{
					if (returnVal != NULL)
						free(returnVal);
					returnVal = BadUsage(argdata, BAD_MIN_SECTORS);
					strcpy (bubbleString, returnVal);
					free (returnVal);
					return;
				}
            minSet=true;
            }
        else if (!strcmpi(argtype, "/x") || !strcmpi(argtype, "-x"))
            {
            if (maxSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::maxSectors=(USHORT)atoi(argdata);
            if (Globals::maxSectors==0)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argdata, BAD_MAX_SECTORS);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            maxSet=true;
            }
        else if (!strcmpi(argtype, "/z") || !strcmpi(argtype, "-z"))
            {
            if (eMultSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::entMultiplier=(float)atof(argdata);
            if (Globals::entMultiplier<2.0 || Globals::entMultiplier>1000.0)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argdata, BAD_ENT_MULTIPLIER);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            eMultSet=true;
            }
        else if (!strcmpi(argtype, "/s") || !strcmpi(argtype, "-s"))
            {
            if (fSectSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            memset(sectFileName, 0, sizeof(sectFileName));
            strcpy(sectFileName, argdata);
            fSectSet=true;
            }
        else if (!strcmpi(argtype, "/b") || !strcmpi(argtype, "-b"))
            {
            if (fBinSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            memset(binFileName, 0, sizeof(binFileName));
            strcpy(binFileName, argdata);
            fBinSet=true;
            }
        else if (!strcmpi(argtype, "/c") || !strcmpi(argtype, "-c"))
            {
            if (auxSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            memset(auxFileName, 0, sizeof(auxFileName));
            strcpy(auxFileName, argdata);
            auxSet=true;
            }
        else if (!strcmpi(argtype, "/w") || !strcmpi(argtype, "-w"))
            {
            if (fPlotSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            memset(plotFileName, 0, sizeof(plotFileName));
            strcpy(plotFileName, argdata);
            fPlotSet=true;
            }
        else if (!strcmpi(argtype, "/o") || !strcmpi(argtype, "-o"))
            {
            if (fOutSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            memset(outFileName, 0, sizeof(outFileName));
            strcpy(outFileName, argdata);
            fOutSet=true;
            }
        else if (!strcmpi(argtype, "/n") || !strcmpi(argtype, "-n"))
            {
            if (sNumSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::totalSectors=(USHORT)atoi(argdata);
            if (Globals::totalSectors==0)
				{
					if (returnVal != NULL)
						free(returnVal);
					returnVal = BadUsage(argdata, BAD_TOTAL_SECTORS);
					strcpy (bubbleString, returnVal);
					free (returnVal);
					return;
				}
            sNumSet=true;
            }
        else if (!strcmpi(argtype, "/e") || !strcmpi(argtype, "-e"))
            {
            if (entSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::maxEntrances=(USHORT)atoi(argdata);
            if (Globals::maxEntrances==0 || Globals::maxEntrances>50)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argdata, BAD_MAX_ENTRANCES);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            entSet=true;
            }
        else if (!strcmpi(argtype, "/r") || !strcmpi(argtype, "-r"))
            {
            if (bubMaxSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::maxBubbles=(ULONG)atol(argdata);
            if (Globals::maxBubbles==0)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argdata, BAD_MAX_BUBBLES);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}

            bubMaxSet=true;
            }
        #if (defined(__OS2__) || defined(__EMX__)) // priority options
        else if (!strcmpi(argtype, "/p") || !strcmpi(argtype, "-p"))
            {
            if (priSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            Globals::pclass=(ULONG)atoi(argdata);
            if (Globals::pclass==0 || Globals::pclass>4)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argdata, BAD_PRIORITY_CLASS);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            priSet=true;
            }
        else if (!strcmpi(argtype, "/d") || !strcmpi(argtype, "-d"))
            {
            if (deltaSet)
			{
				if (returnVal != NULL)
					free(returnVal);
                returnVal = BadUsage(argv[i], BAD_DUPE_OPTION);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
            if (!strcmpi(argdata, "0"))
                {
                Globals::pdelta=0;
                }
            else
                {
					Globals::pdelta=(LONG)atoi(argdata);
					if (Globals::pdelta==0 || Globals::pdelta<0 || Globals::pdelta>31)
					{
						if (returnVal != NULL)
							free(returnVal);
						returnVal = BadUsage(argdata, BAD_PRIORITY_DELTA);
						strcpy (bubbleString, returnVal);
						free (returnVal);
						return;
					}
                }
            deltaSet=true;
            }
        #endif
        else if (!strcmpi(argtype, "/h") || !strcmpi(argtype, "-h") ||
                    !strcmpi(argtype, "/?") || !strcmpi(argtype, "-?"))
            {
				if (returnVal != NULL)
					free(returnVal);
				returnVal = BadUsage("", USAGE_HELP);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
            }
        else
		{
			if (returnVal != NULL)
				free(returnVal);
			returnVal = BadUsage(argtype, BAD_PARAMETER);
			strcpy (bubbleString, returnVal);
			free (returnVal);
			return;
		}
    }

    if (strcmp(auxFileName, "")!=0)
    {
        readAuxData=true;
        auxFile.open(auxFileName, ios::nocreate | ios::in | ios::binary);
        if (auxFile.fail())
		{
			if (returnVal != NULL)
				free(returnVal);
			returnVal = BadUsage(auxFileName, BAD_SECTOR_FILE);
			strcpy (bubbleString, returnVal);
			free (returnVal);
			return;
		}
	}

    if (strcmp(binFileName, "")!=0)
        {
        writeBinData=true;
        binFile.open(binFileName, ios::nocreate | ios::in | ios::binary);
        if (!binFile.fail())
            readBinData=true;
        }

    if (readBinData)
        {
        TWBinaryHeader bh;
        binFile.read((char*)&bh, sizeof(bh));
        if (bh.sig1!=_TWBSIG_1_ || bh.sig2!=_TWBSIG_2_)
		{
			if (returnVal != NULL)
				free(returnVal);
			returnVal = FileError(INVALID_BINARY);
			strcpy (bubbleString, returnVal);
			free (returnVal);
			return;
		}
        if (bh.majorVer<_TWBMIN_MAJOR_)
		{
			if (returnVal != NULL)
				free(returnVal);
			returnVal = FileError(INVALID_BINARY);
			strcpy (bubbleString, returnVal);
			free (returnVal);
			return;
		}
        else if (bh.majorVer==_TWBMIN_MAJOR_)
            {
            if (bh.minorVer<_TWBMIN_MINOR_)
				{
					if (returnVal != NULL)
						free(returnVal);
					returnVal = FileError(INVALID_BINARY);
					strcpy (bubbleString, returnVal);
					free (returnVal);
					return;
				}
            }
        if (bh.sectorCount<100 || bh.sectorCount>65535)
			{
				if (returnVal != NULL)
					free(returnVal);
				returnVal = FileError(INVALID_BINARY);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
        else if (bh.sectorCount!=Globals::totalSectors)
            {
            Globals::totalSectors=bh.sectorCount;
            if (!Globals::sectors.setNumberOfSectors(Globals::totalSectors))
                OutOfMemory();
				{
					if (returnVal != NULL)
						free(returnVal);
					returnVal = OutOfMemory();
					strcpy (bubbleString, returnVal);
					free (returnVal);
					return;
				}
            }
        }

    if (readAuxData)
        {
        TWBinaryHeader bh;
        auxFile.read((char*)&bh, sizeof(bh));
        if (bh.sig1!=_TWBSIG_1_ || bh.sig2!=_TWBSIG_2_)
			{
				if (returnVal != NULL)
					free(returnVal);
				returnVal = FileError(INVALID_BINARY);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
        if (bh.majorVer<_TWBMIN_MAJOR_)
			{
				if (returnVal != NULL)
					free(returnVal);
				returnVal = FileError(INVALID_BINARY);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
        else if (bh.majorVer==_TWBMIN_MAJOR_)
            {
            if (bh.minorVer<_TWBMIN_MINOR_)
				{
					if (returnVal != NULL)
						free(returnVal);
					returnVal = FileError(INVALID_BINARY);
					strcpy (bubbleString, returnVal);
					free (returnVal);
					return;
				}
            }
        if (!readBinData)
            {
            if (bh.sectorCount<100 || bh.sectorCount>65535)
				{
					if (returnVal != NULL)
						free(returnVal);
					returnVal = FileError(INVALID_BINARY);
					strcpy (bubbleString, returnVal);
					free (returnVal);
					return;
				}
            else if (bh.sectorCount!=Globals::totalSectors)
                {
                Globals::totalSectors=bh.sectorCount;
                if (!Globals::sectors.setNumberOfSectors(Globals::totalSectors))
					{
						if (returnVal != NULL)
							free(returnVal);
						returnVal = OutOfMemory();
						strcpy (bubbleString, returnVal);
						free (returnVal);
						return;
					}
                }
            }
        else
            {
            if (bh.sectorCount!=Globals::totalSectors)
				{
					if (returnVal != NULL)
						free(returnVal);
					returnVal = FileError(INVALID_BINARY);
					strcpy (bubbleString, returnVal);
					free (returnVal);
					return;
				}
            }
        }

    if (Globals::maxEntrances>Globals::maxSectors ||
         Globals::maxEntrances>(Globals::maxSectors-Globals::minSectors))
        {
        memset(argtemp, 0, sizeof(argtemp));
        sprintf(argtemp, "%u", Globals::maxEntrances);
		if (returnVal != NULL)
			free(returnVal);
        returnVal = BadUsage(argtemp, BAD_MAX_ENTRANCES);
		strcpy (bubbleString, returnVal);
		free (returnVal);
		return;
        }
    if (Globals::minSectors>Globals::maxSectors ||
         Globals::minSectors>Globals::totalSectors ||
         Globals::minSectors<2)
        {
        memset(argtemp, 0, sizeof(argtemp));
        sprintf(argtemp, "%u", Globals::minSectors);
		if (returnVal != NULL)
			free(returnVal);
        returnVal = BadUsage(argtemp, BAD_MIN_SECTORS);
		strcpy (bubbleString, returnVal);
		free (returnVal);
		return;

        }
    if (Globals::maxSectors>Globals::totalSectors || Globals::maxSectors<2)
        {
        memset(argtemp, 0, sizeof(argtemp));
        sprintf(argtemp, "%u", Globals::maxSectors);
		if (returnVal != NULL)
			free(returnVal);
        returnVal = BadUsage(argtemp, BAD_MAX_SECTORS);
		strcpy (bubbleString, returnVal);
		free (returnVal);
		return;

        }
/*we dont need an input file, force it to be true*/
	readWarpData=true;
/*
    if (strcmp(sectFileName, "")!=0)
        {
        readWarpData=true;
        sectFile.open(sectFileName, ios::nocreate | ios::in);
        if (sectFile.fail()!=0)
            BadUsage(sectFileName, BAD_SECTOR_FILE);
        }
*/

	

    if (strcmp(plotFileName, "")!=0)
        {
        readPlotData=true;
        plotFile.open(plotFileName, ios::nocreate | ios::in);
        if (plotFile.fail()!=0)
			{
				if (returnVal != NULL)
					free(returnVal);
				returnVal = BadUsage(plotFileName, BAD_PLOT_FILE);
				strcpy (bubbleString, returnVal);
				free (returnVal);
				return;
			}
        }

    if (!readPlotData && !readWarpData && !readBinData && !readAuxData)
		{
			if (returnVal != NULL)
				free(returnVal);
			returnVal = BadUsage("", NO_SECTOR_DATA_FILE);
			strcpy (bubbleString, returnVal);
			free (returnVal);
			return;
		}


    if (readBinData && !readPlotData && !readWarpData && !readAuxData)
        {
        writeBinData=false;
        if (!doBubbleSearch)
            {
//            cout << endl << "Nothing to do!" << endl;
            return;
            }
        }

 //   if (doBubbleSearch)
 //       {
//        outFile.open(outFileName, ios::trunc | ios::out);
  //      if (outFile.fail()!=0)
    //        BadUsage(outFileName, BAD_REPORT_FILE);
   //     }

    if (!Globals::sectors.setNumberOfSectors(Globals::totalSectors))
		{
			if (returnVal != NULL)
				free(returnVal);
			returnVal = OutOfMemory();
			strcpy (bubbleString, returnVal);
			free (returnVal);
			return;
		}


    DisableCursor();

//        cout << endl << Globals::totalSectors << flush;
    if (readBinData)
        {
//        cout << endl << "Reading binary data for sector:      " << flush;
        Globals::sectors.readBinaryData(binFile);
//        cout << endl;
        binFile.close();
        }

    if (readAuxData)
        {
//        cout << endl << "Reading auxiliary binary data for sector:      " << flush;
        Globals::sectors.readBinaryData(auxFile);
//        cout << endl;
        auxFile.close();
        }

    if (readPlotData)
        {
//        cout << endl << "Reading zero-turn plot data for path:                " << flush;
        Globals::sectors.readPlotData(plotFile);
//        cout << endl;
        plotFile.close();
        }

    if (readWarpData)
        {
//        cout << endl << "Reading warp data for sector:      " << flush;
//        Globals::sectors.readWarpData(sectFile);
		  Globals::sectors.readSectorData(sectors);
//        cout << endl;
        sectFile.close();
        }

    if (writeBinData)
        {
        binFile.open(binFileName, ios::trunc | ios::out | ios::binary);
        if (binFile.fail())
            FileError(BINARY_WRITE);
//        cout << endl << "Writing binary data for sector:      " << flush;
        Globals::sectors.writeBinaryData(binFile);
//        cout << endl;
        binFile.close();
        }

    if (doBubbleSearch)
        {
//        cout << endl << "Setting one-way warp status for sector:        " << flush;
        Globals::sectors.setOneWayStatus();
//        cout << endl;

        #if (defined(__OS2__) || defined(__EMX__))
        DosSetPriority(2, Globals::pclass, Globals::pdelta, 0);
        #endif

        //cout << endl << "Prospecting potential bubbles with entrance :        " << flush;
        Globals::sectors.findBubbles(bubbleSet);
//        cout << endl;

//       if (bubbleSet.numberOfBubbles()>1)
//            {
//            cout << endl << "Trimming " << bubbleSet.numberOfBubbles() << " potential bubbles..." << flush;
//            cout << endl;
//            }

        bubbleSet.trimBubblesTo(keepSet);

        numBubbles=keepSet.numberOfBubbles();
//        cout << endl << "Number of bubbles found: " << numBubbles << endl << flush;

        for (i=0; i<numBubbles; i++)
		{
			tempReturnVal = keepSet[i].makeReturnStringForATTAC();
			tempReturnVal2 = (char *) malloc (strlen (returnVal)+strlen (tempReturnVal)+1);
			strcpy (tempReturnVal2, returnVal);
			strcat (tempReturnVal2, tempReturnVal);
			tempReturnVal2[strlen (returnVal)+strlen (tempReturnVal)]=0;
			free(returnVal);
			free(tempReturnVal);
			returnVal = (char *) strdup (tempReturnVal2);
			free(tempReturnVal2);
		}
//		cout << endl << sectors[0].warp_sect[1] << endl << flush;
//        outFile.close();
	}

    #ifdef DEBUG_ENABLED
    Globals::debug.close();
    #endif
	strcpy (bubbleString, returnVal);
	free(returnVal);
    //return returnVal;
}

void freeBubbleString (char *val)
{
	free(val);
}

extern "C" void AbortCatcher(int sigstuff)
{
    cout << endl << endl << "Aborting." << endl;
    exit(1);
}