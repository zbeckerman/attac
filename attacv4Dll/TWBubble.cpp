#include <stdio.h>
#include <iomanip.h>
#include <memory.h>

#include "TWBFind.h"
#include "TWBubble.hpp"
#include "TWSector.hpp"
#include "TWWarp.hpp"
#include "AbEnds.hpp"

TWBubble::TWBubble(TWBubble& copy)
{
    initData();
    bd->numSectors=copy.bd->numSectors;
    bd->numEnts=copy.bd->numEnts;
    bd->numBD=copy.bd->numBD;
    bd->bogus=copy.bd->bogus;
    if (!Globals::gobbleMemory)
        {
        if (bd->numSectors>0)
            {
            bd->sectors = new USHORT[bd->numSectors];
            if (bd->sectors==NULL)
                OutOfMemory();
            }
        if (bd->numEnts>0)
            {
            bd->entrances = new USHORT[bd->numEnts];
            if (bd->entrances==NULL)
                OutOfMemory();
            }
        }

    if (bd->numSectors>0)
        memcpy(bd->sectors, copy.bd->sectors, sizeof(USHORT)*bd->numSectors);
    if (bd->numEnts>0)
        memcpy(bd->entrances, copy.bd->entrances, sizeof(USHORT)*bd->numEnts);

    if (bd->numBD>0)
        {
        bd->back = new USHORT[bd->numBD];
        if (bd->back==NULL)
            OutOfMemory();
        memcpy(bd->back, copy.bd->back, sizeof(USHORT)*bd->numBD);
        }
}

TWBubble& TWBubble::operator=(const TWBubble& assign)
{
    if (this==&assign)
        return *this;

    if (assign.bd==NULL)
        {
        nixData();
        return *this;
        }

    if (bd==NULL)
        {
        initData();
        }

    if (bd->numBD>0)
        delete[] bd->back;

    if (!Globals::gobbleMemory)
        {
        if (bd->numSectors>0)
            delete[] bd->sectors;
        if (bd->numEnts>0)
            delete[] bd->entrances;
        }

    bd->numSectors=assign.bd->numSectors;
    bd->numEnts=assign.bd->numEnts;
    bd->numBD=assign.bd->numBD;
    bd->bogus=assign.bd->bogus;

    if (!Globals::gobbleMemory)
        {
        if (bd->numSectors>0)
            {
            bd->sectors = new USHORT[bd->numSectors];
            if (bd->sectors==NULL)
                OutOfMemory();
            }
        if (bd->numEnts>0)
            {
            bd->entrances = new USHORT[bd->numEnts];
            if (bd->entrances==NULL)
                OutOfMemory();
            }
        }

    if (bd->numBD>0)
        {
        bd->back = new USHORT[bd->numBD];
        if (bd->back==NULL)
            OutOfMemory();
        memcpy(bd->back, assign.bd->back, sizeof(USHORT)*bd->numBD);
        }

    if (bd->numSectors>0)
        memcpy(bd->sectors, assign.bd->sectors, sizeof(USHORT)*bd->numSectors);
    if (bd->numEnts>0)
        memcpy(bd->entrances, assign.bd->entrances, sizeof(USHORT)*bd->numEnts);

    return *this;
}

Bool TWBubble::operator==(const TWBubble& comp) const
{
    if (bd->numSectors!=comp.bd->numSectors ||
        bd->numBD!=comp.bd->numBD ||
        bd->numEnts!=comp.bd->numEnts ||
        bd->bogus!=comp.bd->bogus)
        return false;
    for (USHORT i=0; i<bd->numSectors; i++)
        {
        if (bd->sectors[i]!=comp.bd->sectors[i])
            {
            return false;
            }
        }
    return true;
}

Bool TWBubble::containsBubble(const TWBubble& sBub) const
{
    if (bd->numSectors<sBub.bd->numSectors)
        return false;
    for (USHORT i=0; i<sBub.bd->numSectors; i++)
        {
        if (!hasSector(sBub.bd->sectors[i]))
            return false;
        }
    if (bd->numEnts>sBub.bd->numEnts)
        return false;
    else
        return true;
}

Bool TWBubble::addSector(USHORT newSect)
{
    if (bd==NULL)
        initData();

    if (!Globals::gobbleMemory)
        {
        if (bd->numSectors==0)
            {
            bd->sectors = new USHORT[1];
            if (bd->sectors==NULL)
                return false;
            }
        else
            {
            USHORT *tempArray = new USHORT[bd->numSectors+1];
            if (tempArray==NULL)
                return false;
            memcpy(tempArray, bd->sectors, sizeof(USHORT)*bd->numSectors);
            delete[] bd->sectors;
            bd->sectors=tempArray;
            }
        }
    if ((bd->numSectors+1)>(Globals::maxSectors+Globals::maxEntrances))
        return false;
    bd->sectors[bd->numSectors]=newSect;
    bd->numSectors++;
    return true;
}

Bool TWBubble::addEntrance(USHORT newEnt)
{
    if (newEnt==0)
        return true;
    if (bd==NULL)
        initData();

    if (!Globals::gobbleMemory)
        {
        if (bd->numEnts==0)
            {
            bd->entrances = new USHORT[1];
            if (bd->entrances==NULL)
                return false;
            }
        else
            {
            USHORT *tempArray = new USHORT[bd->numEnts+1];
            if (tempArray==NULL)
                return false;
            memcpy(tempArray, bd->entrances, sizeof(USHORT)*bd->numEnts);
            delete[] bd->entrances;
            bd->entrances=tempArray;
            }
        }
    if ((bd->numEnts+1)>Globals::maxEntrances)
        return false;
    bd->entrances[bd->numEnts]=newEnt;
    bd->numEnts++;
    return true;
}

// Add the define for your compiler if it optimizes better with
// the following functions inlined.  Make sure to also change
// the header file accordingly.

#if (!defined(__IBMCPP__) && !defined(__TCPLUSPLUS__))
Bool TWBubble::hasSector(USHORT sectNum) const
{
    if (bd->numSectors==0)
        return false;
    for (USHORT i=0; i<bd->numSectors; i++)
        {
        if (bd->sectors[i]==sectNum)
            return true;
        }
    return false;
}

Bool TWBubble::hasEntrance(USHORT sectNum) const
{
    if (bd->numEnts==0)
        return false;
    for (USHORT i=0; i<bd->numEnts; i++)
        {
        if (bd->entrances[i]==sectNum)
            return true;
        }
    return false;
}

Bool TWBubble::isBackDoor(USHORT sectNum) const
{
    if (bd->numBD==0)
        return false;
    for (USHORT i=0; i<bd->numBD; i++)
        {
        if (bd->back[i]==sectNum)
            return true;
        }
    return false;
}
#endif

Bool TWBubble::addBackDoor(USHORT sectNum)
{
    if (bd==NULL)
        initData();
    if (bd->numBD==0)
        {
        bd->back = new USHORT[1];
        if (bd->back==NULL)
            return false;
        bd->back[0]=sectNum;
        bd->numBD++;
        }
    else
        {
        USHORT *tempArray = new USHORT[bd->numBD+1];
        if (tempArray==NULL)
            return false;
        memcpy(tempArray, bd->back, sizeof(USHORT)*bd->numBD);
        tempArray[bd->numBD]=sectNum;
        bd->numBD++;
        delete[] bd->back;
        bd->back=tempArray;
        }
    return true;
}

Bool TWBubble::isBogus(void)
{
    if (!bd->bogus)
        return false;
    TWSector tsect;
    TWWarp twarp;
    for (int i=1; i<bd->numSectors; i++)
        {
        if (bd->numEnts>0)
            {
            if (hasEntrance(bd->sectors[i]))
                continue;
            }
        tsect=Globals::sectors[bd->sectors[i]-1];
        for (int j=0; j<tsect.numberOfInWarps(); j++)
            {
            twarp=tsect.inWarpAtPosition(j);
            if (twarp.isOneWay())
                {
                if (!hasSector(twarp.sector()))
                    return true;
                }
            }
        }
    return false;
}
char * TWBubble::makeReturnStringForATTAC() const
{
    USHORT sectNum, i;
    UCHAR sectsOnLine=0;
    TWSector tsect;
    TWWarp currWarp;
	
	char *returnVal;
	char buffer[10000], tempBuffer[1024];

	strcpy (buffer, "");
	//size
	sprintf (tempBuffer, "%d", bd->numSectors-bd->numEnts);
	strcat (buffer, tempBuffer);
	strcat (buffer, ":");

    if (bd->numEnts==1)
	{
		//entrance
		sprintf (tempBuffer, "%d", bd->sectors[0]);
		strcat (buffer, tempBuffer);
		strcat (buffer, ":");
	}

	for (i=0; i<bd->numSectors; i++)
	{
        sectNum=bd->sectors[i];

		//sector
		sprintf (tempBuffer, "%d", sectNum);
		strcat (buffer, tempBuffer);
		strcat (buffer, ":");

	}
	if (strlen(buffer)>0)
		buffer[strlen(buffer)-1]=';';
	else
		strcpy (buffer, "1");

	returnVal = (char *) strdup (buffer);
	return (returnVal);
}

void TWBubble::writeTo(ofstream& outFile) const
{
    USHORT sectNum, i, j;
    UCHAR warps, sectsOnLine=0;
    TWSector tsect;
    TWWarp currWarp;

    outFile << (bd->numSectors-bd->numEnts) << "-sector ";
    if (Globals::bogusBubbles)
        outFile << "bogus ";
    outFile << "bubble with ";
    if (bd->numEnts==1)
        {
        outFile << "entrance at " << bd->sectors[0] << "." << endl << endl;
        }
    else
        {
        outFile << bd->numEnts << " entrances at:" << endl;
        for (i=0; i<bd->numEnts; i++)
            {
            sectsOnLine++;
            outFile << setw(6) << bd->entrances[i];
            if (sectsOnLine==12 || i==(bd->numEnts-1))
                {
                sectsOnLine=0;
                outFile << endl;
                }
            }
        outFile << endl;
        }
    outFile << "Sectors in bubble:" << endl;;
    sectsOnLine=0;
    for (i=1; i<bd->numSectors; i++)
        {
        sectNum=bd->sectors[i];
        if (hasEntrance(sectNum))
            {
            if (i==(bd->numSectors-1) && sectsOnLine!=0)
                outFile << endl;
            continue;
            }
        else if (isBackDoor(sectNum))
            {
            outFile << "(" << sectNum << ") ";
            }
        else
            outFile << " " << sectNum << "  ";
        sectsOnLine++;
        if (sectsOnLine==10 || i==(bd->numSectors-1))
            {
            sectsOnLine=0;
            outFile << endl;
            }
        }
    outFile << endl;
    for (i=0; i<bd->numSectors; i++)
        {
        sectNum=bd->sectors[i];
        tsect=Globals::sectors[sectNum-1];
        outFile << "Sector " << setw(5) << sectNum << "  -  ";
        warps=tsect.numberOfInWarps();
        for (j=0; j<warps; j++)
            {
            currWarp=tsect.inWarpAtPosition(j);
            if (!currWarp.isOneWay())
                continue;
            outFile << "<" << setw(5) << currWarp.sector() << "> ";
            }
        warps=tsect.numberOfOutWarps();
        for (j=0; j<warps; j++)
            {
            currWarp=tsect.outWarpAtPosition(j);
            if (currWarp.isOneWay())
                {
                outFile << "(";
                }
            else if (hasEntrance(currWarp.sector()))
                {
                outFile << "{";
                }
            else if (!Globals::sectors[currWarp.sector()-1].isExplored())
                {
                outFile << "[";
                }
            else
                outFile << " ";
            outFile << setw(5) << currWarp.sector();
            if (currWarp.isOneWay())
                {
                outFile << ") ";
                }
            else if (hasEntrance(currWarp.sector()))
                {
                outFile << "}";
                }
            else if (!Globals::sectors[currWarp.sector()-1].isExplored())
                {
                outFile << "]";
                }
            else
                outFile << "  ";
            }
        outFile << endl;
        }
    outFile << endl
              << "----------------------------------------"
              << "----------------------------------------"
              << endl << endl;
    if (outFile.fail())
        FileError(REPORT_WRITE);
}

TWBubbleSet& TWBubbleSet::operator=(const TWBubbleSet& assign)
{
    if (this==&assign)
        return *this;

    for (int i=0; i<assign.numBubbles; i++)
        {
        *bubbles[i]=*assign.bubbles[i];
        }

    numBubbles=assign.numBubbles;

    return *this;
}

void TWBubbleSet::trimBubblesTo(TWBubbleSet& keepSet)
{
    USHORT i, j;
    Bool biggestBubble;

    for (i=0; i<numBubbles; i++)
        {
        biggestBubble=true;
        for (j=0; j<numBubbles; j++)
            {
            if (i==j)
                continue;
            if (*bubbles[i]==*bubbles[j])
                {
                if (j<i)
                    {
                    continue;
                    }
                else
                    {
                    biggestBubble=false;
                    break;
                    }
                }
            else if (bubbles[j]->containsBubble(*bubbles[i]))
                {
                biggestBubble=false;
                break;
                }
            }
        if (biggestBubble)
            {
            TWBubble *tbub = new TWBubble(*bubbles[i]);
            keepSet.addBubble(*tbub);
            }
        }
}

