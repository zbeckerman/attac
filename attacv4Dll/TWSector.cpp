#if (defined(__OS2__) || defined(__EMX__))
    #define INCL_DOSPROCESS
    #include <os2.h>
#endif
#include <iostream.h>
#include <iomanip.h>
#include <stdio.h>
#if (!defined(__linux__))
    #include <conio.h>
#endif
#include <stdlib.h>

#include "TWBFind.h"
#include "TWRemaps.hpp"
#include "TWSector.hpp"

TWSector::TWSector(TWSector& copy)
{
    initData();
    sdata->numInWarps=copy.sdata->numInWarps;
    if (sdata->numInWarps>0)
        {
        sdata->inWarps = new TWWarp[sdata->numInWarps];
        if (sdata->inWarps==NULL)
            OutOfMemory();
        memcpy(sdata->inWarps, copy.sdata->inWarps, sizeof(TWWarp)*sdata->numInWarps);
        }
    sdata->numOutWarps=copy.sdata->numOutWarps;
    if (sdata->numOutWarps>0)
        {
        sdata->outWarps = new TWWarp[sdata->numOutWarps];
        if (sdata->outWarps==NULL)
            OutOfMemory();
        memcpy(sdata->outWarps, copy.sdata->outWarps, sizeof(TWWarp)*sdata->numOutWarps);
        }
    sdata->explored=copy.sdata->explored;
}

TWSector& TWSector::operator=(const TWSector& assign)
{
    if (this==&assign)
        return *this;
    nixData();
    initData();
    sdata->numInWarps=assign.sdata->numInWarps;
    sdata->numOutWarps=assign.sdata->numOutWarps;
    if (sdata->numInWarps>0)
        {
        sdata->inWarps = new TWWarp[sdata->numInWarps];
        if (sdata->inWarps==NULL)
            OutOfMemory();
        memcpy(sdata->inWarps, assign.sdata->inWarps, sizeof(TWWarp)*sdata->numInWarps);
        }
    if (sdata->numOutWarps>0)
        {
        sdata->outWarps = new TWWarp[sdata->numOutWarps];
        if (sdata->outWarps==NULL)
            OutOfMemory();
        memcpy(sdata->outWarps, assign.sdata->outWarps, sizeof(TWWarp)*sdata->numOutWarps);
        }
    sdata->explored=assign.sdata->explored;
    return *this;
}

Bool TWSector::addInWarp(const TWWarp& newSect)
{
    if (sdata->numInWarps==0)
        {
        sdata->inWarps = new TWWarp[1];
        if (sdata->inWarps==NULL)
            return false;
        sdata->numInWarps++;
        sdata->inWarps[0]=newSect;
        }
    else
        {
        if (!this->hasInWarp(newSect.sector()))
            {
            TWWarp *tempArray = new TWWarp[sdata->numInWarps+1];
            if (tempArray==NULL)
                return false;
            memcpy(tempArray, sdata->inWarps, sizeof(TWWarp)*sdata->numInWarps);
            tempArray[sdata->numInWarps]=newSect;
            sdata->numInWarps++;
            delete[] sdata->inWarps;
            sdata->inWarps=tempArray;
            }
        }
    return true;
}

Bool TWSector::addOutWarp(const TWWarp& newSect)
{
    if (sdata->numOutWarps==0)
        {
        sdata->outWarps = new TWWarp[1];
        if (sdata->outWarps==NULL)
            return false;
        sdata->numOutWarps++;
        sdata->outWarps[0]=newSect;
        }
    else
        {
        if (!this->hasOutWarp(newSect.sector()))
            {
            if (this->numberOfOutWarps()>5)
                FileError(INCOMPATIBLE_INFO);
            TWWarp *tempArray = new TWWarp[sdata->numOutWarps+1];
            if (tempArray==NULL)
                return false;
            memcpy(tempArray, sdata->outWarps, sizeof(TWWarp)*sdata->numOutWarps);
            tempArray[sdata->numOutWarps]=newSect;
            sdata->numOutWarps++;
            delete[] sdata->outWarps;
            sdata->outWarps=tempArray;
            }
        }
    return true;
}

// Add your compiler's define if it optimizes better with the
// following functions inlined.  Be sure to modify the header
// accordingly.

#if (!defined(__IBMCPP__) && !defined(__TCPLUSPLUS__))
Bool TWSector::hasInWarp(USHORT testSect) const
{
    if (sdata->numInWarps==0)
        return false;
    for (USHORT i=0; i<sdata->numInWarps; i++)
        {
        if (sdata->inWarps[i].sector()==testSect)
            return true;
        }
    return false;
}

Bool TWSector::hasOutWarp(USHORT testSect) const
{
    if (sdata->numOutWarps==0)
        return false;
    for (USHORT i=0; i<sdata->numOutWarps; i++)
        {
        if (sdata->outWarps[i].sector()==testSect)
            return true;
        }
    return false;
}
#endif

void TWSectorArray::readSectorData(TWX_SECTOR *sectors)
{
    char tokens[7][10];
    USHORT i, j, scount=0, lastSector=0;
    Bool noMoreTokens;
    USHORT lineNum=0, numTokens, sectNum=0, currWarpSect;
    TWWarp currWarp;

    lineNum++;
    noMoreTokens=false;
    numTokens=0;
    for (i=0; i<7; i++)
	{
        memset(tokens[i], 0, sizeof(tokens[i]));
    }

	for (i=0; i <Globals::totalSectors; i++)
	{
		numTokens=0;
		sprintf (tokens[numTokens], "%d", i+1);
		numTokens++;
		for (j=0; j<sectors[i].warps && j<6; j++)
		{
			sprintf (tokens[numTokens], "%ld", sectors[i].warp_sect[j]);
			numTokens++;
		}

		sectNum=(USHORT)atoi(tokens[0]);

//		cout << "\b\b\b\b\b" << setw(5) << sectNum << flush;

		if (sectNum==0)
			continue;
		if (sectNum>Globals::totalSectors)
			BadSector(tokens[0], lineNum, BADSECT_TOO_LARGE);
		if (sectNum<=lastSector)
			BadSector(tokens[0], lineNum, BADSECT_DUP_DATA);

		sects[sectNum-1].setExplored(true);

		for (j=1; j<numTokens; j++)
		{
			currWarpSect=(USHORT)atoi(tokens[j]);
			if (currWarpSect>0 && currWarpSect <=Globals::totalSectors)
			{
				currWarp.setSector(currWarpSect);
				currWarp.setOneWay(false);
				if (!sects[sectNum-1].addOutWarp(currWarp))
					OutOfMemory();
				currWarp.setSector(sectNum);
				if (!sects[currWarpSect-1].addInWarp(currWarp))
					OutOfMemory();
			}
		}
	}
}


void TWSectorArray::readWarpData(ifstream& sectFile)
{
    char sectLine[256], tokens[7][10], *toktemp;
    USHORT i, scount=0, lastSector=0;
    Bool noMoreTokens;
    USHORT lineNum=0, numTokens, sectNum=0, currWarpSect;
    TWWarp currWarp;

    sectFile.seekg(0);
    while (!sectFile.eof())
        {
        sectFile.getline(sectLine, 255);
        if (sectFile.eof())
            {
            cout << "\b\b\b\b\b" << setw(5) << sectNum << flush;
            break;
            }
        if (sectFile.fail())
            FileError(SECTOR_READ);
        lineNum++;
        noMoreTokens=false;
        numTokens=0;
        for (i=0; i<7; i++)
            {
            memset(tokens[i], 0, sizeof(tokens[i]));
            }
        toktemp=strtok(sectLine, " ");
        if (toktemp==NULL)
            continue;
        strcpy(tokens[numTokens], toktemp);
        numTokens++;
        while (!noMoreTokens)
            {
            toktemp=strtok(NULL, " ");
            if (toktemp==NULL)
                noMoreTokens=true;
            else
                {
                strcpy(tokens[numTokens], toktemp);
                numTokens++;
                }
            }
        sectNum=(USHORT)atoi(tokens[0]);
        if (sectNum==0)
            continue;
        if (sectNum>Globals::totalSectors)
            BadSector(tokens[0], lineNum, BADSECT_TOO_LARGE);
        if (sectNum<=lastSector)
            BadSector(tokens[0], lineNum, BADSECT_DUP_DATA);

        scount++;
        if (scount%50==0)
            {
            cout << "\b\b\b\b\b" << setw(5) << sectNum << flush;
            }
        sects[sectNum-1].setExplored(true);
        for (i=1; i<numTokens; i++)
            {
            currWarpSect=(USHORT)atoi(tokens[i]);
            if (currWarpSect==0)
                BadSector(tokens[i], lineNum, BADSECT_NOT_INTEGER);
            if (currWarpSect>Globals::totalSectors)
                BadSector(tokens[i], lineNum, BADSECT_TOO_LARGE);
            currWarp.setSector(currWarpSect);
            currWarp.setOneWay(false);
            if (!sects[sectNum-1].addOutWarp(currWarp))
                OutOfMemory();
            currWarp.setSector(sectNum);
            if (!sects[currWarpSect-1].addInWarp(currWarp))
                OutOfMemory();
            }
        }
}

void TWSectorArray::readBinaryData(fstream& binFile)
{
    TWBinarySectorData bsdata;
    USHORT i, j;
    char numTemp[32];

    binFile.seekg(16);
    for (i=0; i<Globals::totalSectors; i++)
        {
        binFile.read((char*)&bsdata, sizeof(bsdata));
        if (binFile.fail())
            FileError(SECTOR_READ);

        if ((i+1)%50==0)
            {
            cout << "\b\b\b\b\b" << setw(5) << (i+1) << flush;
            }

        sects[i].setExplored(bsdata.explored);
        for (j=0; j<MAX_WARPS; j++)
            {
            if (bsdata.outWarps[j]>Globals::totalSectors)
                {
                sprintf(numTemp, "%u", bsdata.outWarps[j]);
                BadSector(numTemp, i+1, BADSECT_TOO_LARGE);
                }
            if (bsdata.outWarps[j]!=0)
                {
                if (!sects[i].addOutWarp(TWWarp((USHORT)bsdata.outWarps[j])))
                    OutOfMemory();
                if (!sects[bsdata.outWarps[j]-1].addInWarp(TWWarp((USHORT)(i+1))))
                    OutOfMemory();
                }
            }
        }
}

void TWSectorArray::writeBinaryData(fstream& binFile)
{
    TWBinarySectorData bsdata;
    USHORT i, j;
    TWBinaryHeader bh;

    memset(&bh, 0, sizeof(bh));
    bh.sig1=_TWBSIG_1_;
    bh.sig2=_TWBSIG_2_;
    bh.majorVer=Globals::majorVersion;
    bh.minorVer=Globals::minorVersion;
    bh.sectorCount=(ULONG)Globals::totalSectors;
    binFile.write((char*)&bh, sizeof(bh));
    if (binFile.fail())
        FileError(BINARY_WRITE);

    for (i=0; i<Globals::totalSectors; i++)
        {
        memset(&bsdata, 0, sizeof(bsdata));

        if ((i+1)%50==0)
            {
            cout << "\b\b\b\b\b" << setw(5) << (i+1) << flush;
            }

        bsdata.explored=sects[i].isExplored();
        for (j=0; j<sects[i].numberOfOutWarps(); j++)
            {
            bsdata.outWarps[j]=(USHORT)sects[i].outWarpAtPosition(j).sector();
            }

        binFile.write((char*)&bsdata, sizeof(bsdata));
        if (binFile.fail())
            FileError(BINARY_WRITE);
        }
}

void TWSectorArray::readPlotData(ifstream& plotFile)
{
    char plotLine[256], sectString[32], *toktemp;
    const char tokStr[]=" > ";
    USHORT lineNum=0, startSect, endSect, sectNum;
    USHORT prevSect;
    Bool plotDone, startMatched, currExp;

    plotFile.seekg(0);
    while (!plotFile.eof())
        {
        plotFile.getline(plotLine, 255);

        if (plotFile.eof())
            break;

        if (plotFile.fail())
            FileError(SECTOR_READ);
        lineNum++;

        if (!strcmp(plotLine+strlen(plotLine)-1, "\r"))
            plotLine[strlen(plotLine)-1]=0x00; // Nix any CR

        toktemp=strtok(plotLine, tokStr);
        if (toktemp==NULL)
            continue;
        if (strcmp(toktemp, "FM")!=0)
            continue;
        else
            {
            toktemp=strtok(NULL, tokStr);
            if (toktemp==NULL)
                BadSector("N/A", lineNum, BADSECT_MALFORMED_PLOT);

            startSect=(USHORT)atoi(toktemp);
            if (startSect==0)
                BadSector(toktemp, lineNum, BADSECT_NOT_INTEGER);
            if (startSect>Globals::totalSectors)
                BadSector(toktemp, lineNum, BADSECT_TOO_LARGE);

            memset(plotLine, 0, sizeof(plotLine));
            plotFile.getline(plotLine, 255);
            lineNum++;

            if (plotFile.eof())
                break;
            if (plotFile.fail())
                FileError(SECTOR_READ);

            if (!strcmp(plotLine+strlen(plotLine)-1, "\r"))
                plotLine[strlen(plotLine)-1]=0x00; // Nix any CR

            toktemp=strtok(plotLine, tokStr);
            if (strcmp(toktemp, "TO")!=0)
                break;

            toktemp=strtok(NULL, tokStr);
            endSect=(USHORT)atoi(toktemp);
            if (endSect==0)
                BadSector(toktemp, lineNum, BADSECT_NOT_INTEGER);
            if (endSect>Globals::totalSectors)
                BadSector(toktemp, lineNum, BADSECT_TOO_LARGE);

            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << setw(5) << startSect
                 << " -> " << setw(5) << endSect << flush;

            plotDone=false;
            startMatched=false;
            while (!plotDone)
                {
                memset(plotLine, 0, sizeof(plotLine));
                plotFile.getline(plotLine, 255);
                lineNum++;
                if (plotFile.fail())
                    FileError(SECTOR_READ);

                if (!strcmp(plotLine+strlen(plotLine)-1, "\r"))
                    plotLine[strlen(plotLine)-1]=0x00; // Nix any CR

                toktemp=strtok(plotLine, tokStr);
                if (toktemp==NULL)
                    continue;

                if (!strcmp(toktemp, "***"))
                    break;
                if (!startMatched)
                    {
                    sectNum=(USHORT)atoi(toktemp);
                    if (sectNum==0)
                        BadSector(toktemp, lineNum, BADSECT_NOT_INTEGER);
                    if (sectNum>Globals::totalSectors)
                        BadSector(toktemp, lineNum, BADSECT_TOO_LARGE);
                    if (sectNum!=startSect)
                        BadSector("N/A", lineNum, BADSECT_MALFORMED_PLOT);
                    startMatched=true;
                    prevSect=sectNum;
                    }
                else
                    {
                    memset(sectString, 0, sizeof(sectString));
                    if (toktemp[0]=='(')
                        {
                        if (toktemp[strlen(toktemp)-1]!=')')
                            BadSector(toktemp, lineNum, BADSECT_MALFORMED_PLOT);
                        strncpy(sectString, toktemp+1, strlen(toktemp)-2);
                        currExp=false;
                        }
                    else
                        {
                        currExp=true;
                        strcpy(sectString, toktemp);
                        }
                    sectNum=(USHORT)atoi(sectString);
                    if (sectNum==0)
                        BadSector(sectString, lineNum, BADSECT_NOT_INTEGER);
                    if (sectNum>Globals::totalSectors)
                        BadSector(toktemp, lineNum, BADSECT_TOO_LARGE);
                    sects[sectNum-1].setExplored(currExp);
                    if (!sects[sectNum-1].addInWarp(TWWarp(prevSect)))
                        OutOfMemory();
                    if (!sects[prevSect-1].addOutWarp(TWWarp(sectNum)))
                        OutOfMemory();
                    prevSect=sectNum;
                    if (sectNum==endSect)
                        plotDone=true;
                    }
                do
                    {
                    toktemp=strtok(NULL, tokStr);
                    if (toktemp==NULL)
                        continue;

                    memset(sectString, 0, sizeof(sectString));
                    if (toktemp[0]=='(')
                        {
                        if (toktemp[strlen(toktemp)-1]!=')')
                            BadSector(toktemp, lineNum, BADSECT_MALFORMED_PLOT);
                        strncpy(sectString, toktemp+1, strlen(toktemp)-2);
                        currExp=false;
                        }
                    else
                        {
                        currExp=true;
                        strcpy(sectString, toktemp);
                        }
                    sectNum=(USHORT)atoi(sectString);
                    if (sectNum==0)
                        BadSector(sectString, lineNum, BADSECT_NOT_INTEGER);
                    sects[sectNum-1].setExplored(currExp);
                    if (!sects[sectNum-1].addInWarp(TWWarp(prevSect)))
                        OutOfMemory();
                    if (!sects[prevSect-1].addOutWarp(TWWarp(sectNum)))
                        OutOfMemory();
                    prevSect=sectNum;
                    if (sectNum==endSect)
                        plotDone=true;
                    } while (toktemp!=NULL);
                }
            }
        }
}
void TWSectorArray::setOneWayStatus(void)
{
    UCHAR warps;
    USHORT i, j, sectNum;
    TWWarp currWarp;

    for (i=0; i<Globals::totalSectors; i++)
        {
//        if ((i+1)%50==0 || (i+1)==Globals::totalSectors)
//            cout << "\b\b\b\b\b" << setw(5) << (i+1) << flush;
        warps=sects[i].numberOfInWarps();
        for (j=0; j<warps; j++)
            {
            currWarp=sects[i].inWarpAtPosition(j);
            sectNum=currWarp.sector();
            if (sects[i].hasOutWarp(sectNum))
                sects[i].inWarpAtPosition(j).setOneWay(false);
            else
                {
                if (sects[i].numberOfOutWarps()!=0)
                    sects[i].inWarpAtPosition(j).setOneWay(true);
                }
            }
        warps=sects[i].numberOfOutWarps();
        for (j=0; j<warps; j++)
            {
            currWarp=sects[i].outWarpAtPosition(j);
            sectNum=currWarp.sector();
            if (sects[i].hasInWarp(sectNum))
                sects[i].outWarpAtPosition(j).setOneWay(false);
            else
                sects[i].outWarpAtPosition(j).setOneWay(true);
            }
        }
}

void TWSectorArray::findBubbles(TWBubbleSet &bubbleSet)
{
    for (i=0; i<Globals::totalSectors; i++)
        {
        #if (defined(__OS2__) || defined(__EMX__))
        if (Globals::pclass==3)
            {
            if ((i+1)%100==0)
                DosSleep(1);
            }
        #endif
        #ifdef __linux__
        if ((i+1)%50==0)
        #endif
        while (_kbhit())
            {
            if (_getch()==32)
                {
                fflush(stdin);
                return;
                }
            }
//        if ((i+1)%10==0 || (i+1)==Globals::totalSectors)
//            cout << "\b\b\b\b\b" << setw(5) << (i+1) << flush;
        warps=sects[i].numberOfOutWarps();
        entrance=i+1;
        if (Globals::tunnelEntrance)
            {
            for (uniIterator=0; uniIterator<warps; uniIterator++)
                {
                currBubble = new TWBubble(entrance);
                if (currBubble==NULL)
                    OutOfMemory();
                if (!currBubble->addSector(sects[i].outWarpAtPosition(uniIterator).sector()))
                    OutOfMemory();
                bubbleCursor=1;
                uniSects = new USHORT[warps];
                if (uniSects==NULL)
                    OutOfMemory();
                numUniSects=0;
                for (j=0; j<warps; j++)
                    {
                    if (j!=uniIterator)
                        {
                        sectNum=sects[i].outWarpAtPosition(j).sector();
                        uniSects[numUniSects]=sectNum;
                        numUniSects++;
                        }
                    }
                if (!traverseBubble(bubbleSet))
                    {
                    return;
                    }
                }
            }
        else
            {
            uniMasks=(UCHAR)ipow(2L, (ULONG)warps);
            for (uniIterator=1; uniIterator<(uniMasks-1); uniIterator++)
                {
                currBubble = new TWBubble(entrance);
                if (currBubble==NULL)
                    OutOfMemory();
                bubbleCursor=1;
                uniSects = new USHORT[warps];
                if (uniSects==NULL)
                    OutOfMemory();
                numUniSects=0;
                for (j=0; j<warps; j++)
                    {
                    sectNum=sects[i].outWarpAtPosition(j).sector();
                    if ((uniIterator<<(31-j)) >> 31)
                        {
                        uniSects[numUniSects]=sectNum;
                        numUniSects++;
                        }
                    else
                        {
                        if (!currBubble->addSector(sectNum))
                            {
                            OutOfMemory();
                            }
                        }
                    }
                if (!traverseBubble(bubbleSet))
                    {
                    return;
                    }
                }
            }
        }
}

Bool TWSectorArray::addWarps(USHORT previous, USHORT current)
{
    UCHAR numWarps, i, j;
    TWWarp currWarp;
    USHORT sectNum;
    numWarps=sects[current-1].numberOfOutWarps();
    if ((currBubble->numberOfSectors()+numWarps)>(Globals::maxSectors+Globals::maxEntrances))
        {
        return false;
        }
    for (i=0; i<numWarps; i++)
        {
        currWarp=sects[current-1].outWarpAtPosition(i);
        sectNum=currWarp.sector();
        if (Globals::avoidUnexplored)
            {
            if (!sects[sectNum-1].isExplored())
                return false;
            }
        else
            {
            if (sects[sectNum-1].numberOfOutWarps()==0)
                return false;
            }
        if (currWarp.isOneWay())
            {
            if (!currBubble->addBackDoor(current))
                OutOfMemory();
            continue;
            }
        if (sectNum==previous)
            continue;
        if (currBubble->hasSector(sectNum))
            continue;
        for (j=0; j<numUniSects; j++)
            {
            if (sectNum==uniSects[j])
                return false;
            }
        if (!currBubble->addSector(sectNum))
            {
            OutOfMemory();
            }
        }
    numWarps=sects[current-1].numberOfInWarps();
    if ((currBubble->numberOfSectors()+numWarps)>(Globals::maxSectors+Globals::maxEntrances))
        {
        return false;
        }
    for (i=0; i<numWarps; i++)
        {
        currWarp=sects[current-1].inWarpAtPosition(i);
        sectNum=currWarp.sector();
        if (sectNum==previous)
            continue;
        if (currBubble->hasSector(sectNum))
            continue;
        for (j=0; j<numUniSects; j++)
            {
            if (sectNum==uniSects[j])
                return false;
            }
        if (!Globals::bogusBubbles)
            {
            if (!currBubble->addSector(sectNum))
                {
                OutOfMemory();
                }
            }
        else
            currBubble->setBogus(true);
        }
    return true;
}

Bool TWSectorArray::traverseBubble(TWBubbleSet& bubbleSet)
{
    previous=entrance;
    bubbleGood=true;
    bubbleDone=false;
    while (!bubbleDone)
        {
        sectNum=currBubble->sectorAtPosition(bubbleCursor);
        if (sectNum==0)
            break;
        if (Globals::avoidUnexplored)
            {
            if (!sects[sectNum-1].isExplored())
                {
                bubbleGood=false;
                break;
                }
            }
        else
            {
            if (sects[sectNum-1].numberOfOutWarps()==0 &&
                !sects[sectNum-1].isExplored())
                {
                bubbleGood=false;
                break;
                }
            }
        if (!addWarps(previous, sectNum))
            {
            bubbleDone=true;
            bubbleGood=false;
            }
        else
            {
            if (Globals::maxEntrances>1)
                {
                if ((bubbleCursor+1>(USHORT)(currBubble->numberOfSectors() -
                     (bubbleCursor))*Globals::entMultiplier) &&
                     ((bubbleCursor+1)>Globals::minSectors) &&
                     (currBubble->numberOfSectors()-(bubbleCursor+1) <
                     Globals::maxEntrances))
                    {
                    tempBubble = new TWBubble(*currBubble);
                    if (tempBubble==NULL)
                        OutOfMemory();
                    tempCursor=bubbleCursor;
                    while ((tempCursor+1)<tempBubble->numberOfSectors())
                        {
                        tempCursor++;
                        tsect=tempBubble->sectorAtPosition(tempCursor);
                        if (!tempBubble->addEntrance(tsect))
                            {
                            OutOfMemory();
                            }
                        }
                    if (Globals::bogusBubbles)
                        {
                        if (tempBubble->isBogus())
                            {
                            if (!bubbleSet.addBubble(*tempBubble))
                                {
                                maxBubblesMet();
                                return false;
                                }
                            }
                        else
                            {
                            delete tempBubble;
                            tempBubble=0;
                            }
                        }
                    else
                        {
                        if (!bubbleSet.addBubble(*tempBubble))
                            {
                            maxBubblesMet();
                            return false;
                            }
                        }
                    }
                }
            }
        previous=sectNum;
        bubbleCursor++;
        if ((bubbleCursor+1)>currBubble->numberOfSectors())
            bubbleDone=true;
        }
    if (bubbleGood && currBubble->numberOfSectors()>Globals::minSectors)
        {
        if (Globals::bogusBubbles)
            {
            if (currBubble->isBogus())
                {
                if (!bubbleSet.addBubble(*currBubble))
                    {
                    maxBubblesMet();
                    return false;
                    }
                }
            else
                {
                delete currBubble;
                currBubble=0;
                }
            }
        else
            {
            if (!bubbleSet.addBubble(*currBubble))
                {
                maxBubblesMet();
                return false;
                }
            }
        }
    else
        {
        delete currBubble;
        currBubble=0;
        }
    delete[] uniSects;
    uniSects=0;
    return true;
}
