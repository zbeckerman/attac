#ifndef _TWBUBBLE_HPP_
#define _TWBUBBLE_HPP_

#include <fstream.h>
#include "TWTypes.hpp"
#include "Globals.hpp"
#include "AbEnds.hpp"

struct TWBubbleData
{
    USHORT numSectors;
    USHORT *sectors;
    USHORT numEnts;
    USHORT *entrances;
    USHORT numBD;
    USHORT *back;
    Bool bogus;
};

class TWBubble
{
    public:

    inline TWBubble()
    {
        bd=0;
    }

    inline TWBubble(USHORT entrance)
    {
        initData();
        if (!Globals::gobbleMemory)
            {
            bd->sectors = new USHORT[1];
            if (bd->sectors==NULL)
                OutOfMemory();
            bd->entrances = new USHORT[1];
            if (bd->entrances==NULL)
                OutOfMemory();
            }
        bd->sectors[0]=entrance;
        bd->numSectors=1;
        bd->entrances[0]=entrance;
        bd->numEnts=1;
        bd->numBD=0;
        bd->back=0;
        bd->bogus=false;
    }

    TWBubble(TWBubble& copy);
    TWBubble& operator=(const TWBubble& assign);
    Bool operator==(const TWBubble& compare) const;

    inline ~TWBubble()
    {
        if (bd!=NULL)
            {
            if (bd->numSectors>0 || Globals::gobbleMemory)
                delete[] bd->sectors;
            if (bd->numEnts>0 || Globals::gobbleMemory)
                delete[] bd->entrances;
            if (bd->numBD>0)
                delete[] bd->back;
            delete bd;
            }
    }

    Bool isBogus(void);

    inline void setBogus(Bool newbogus)
    {
        bd->bogus=newbogus;
    }

    inline USHORT numberOfSectors(void) const
    {
        return bd->numSectors;
    }

    inline USHORT entranceAtPosition(USHORT index) const
    {
        return bd->entrances[index];
    }

    inline USHORT numberOfEntrances(void) const
    {
        return bd->numEnts;
    }

    Bool addEntrance(USHORT newEnt);
    Bool addSector(USHORT newSect);

    inline USHORT sectorAtPosition(USHORT index) const
    {
        return bd->sectors[index];
    }

    inline USHORT operator[](USHORT index) const
    {
        return bd->sectors[index];
    }

// Add the define for your compiler if it optimizes better with the
// following functions inlined.  Be sure to modify the .CPP file
// accordingly.

    #if (defined(__IBMCPP__) || defined(__TCPLUSPLUS__))
    inline Bool TWBubble::hasSector(USHORT sectNum) const
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

    inline Bool TWBubble::hasEntrance(USHORT sectNum) const
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

    inline Bool TWBubble::isBackDoor(USHORT sectNum) const
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
    #else
    Bool hasEntrance(USHORT sectNum) const;
    Bool hasSector(USHORT sectNum) const;
    Bool isBackDoor(USHORT sectNum) const;
    #endif

    Bool addBackDoor(USHORT sectNum);
    Bool containsBubble(const TWBubble& subBub) const;
    void writeTo(ofstream& outFile) const;
    char * makeReturnStringForATTAC() const;


    private:

    inline void initData(void)
    {
        bd = new TWBubbleData;
        if (bd==NULL)
            OutOfMemory();
        bd->numSectors=0;
        bd->numEnts=0;
        bd->numBD=0;
        bd->back=0;
        bd->bogus=false;

        if (Globals::gobbleMemory)
            {
            bd->sectors = new USHORT[Globals::maxSectors+Globals::maxEntrances];
            if (bd->sectors==NULL)
                OutOfMemory();
            bd->entrances = new USHORT[Globals::maxEntrances];
            if (bd->entrances==NULL)
                OutOfMemory();
            }
        else
            {
            bd->sectors=0;
            bd->entrances=0;
            }
    }

    inline void nixData(void)
    {
        if (bd!=NULL)
            {
            if (Globals::gobbleMemory)
                {
                delete[] bd->sectors;
                delete[] bd->entrances;
                }
            else
                {
                if (bd->numSectors>0)
                    delete[] bd->sectors;
                if (bd->numEnts>0)
                    delete[] bd->entrances;
                }
            if (bd->numBD>0)
                delete[] bd->back;
            delete bd;
            }
        bd=0;
    }

    TWBubbleData *bd;
};

class TWBubbleSet
{
    public:

    inline TWBubbleSet()
    {
        bubbles = new TWBubble*[Globals::maxBubbles];
        numBubbles=0;
    }

    inline ~TWBubbleSet()
    {
        for (int i=0; i<numBubbles; i++)
            {
            delete bubbles[i];
            }
        delete[] bubbles;
    }

    TWBubbleSet& operator=(const TWBubbleSet& assign);

    inline Bool addBubble(TWBubble& newBubble)
    {
        if (numBubbles>=Globals::maxBubbles)
            return false;
        bubbles[numBubbles]=&newBubble;
        numBubbles++;
        return true;
    }

    inline USHORT numberOfBubbles(void)
    {
        return numBubbles;
    }

    inline TWBubble& bubbleAtPosition(USHORT index)
    {
        return *bubbles[index];
    }

    inline TWBubble& operator[](USHORT index)
    {
        return *bubbles[index];
    }

    void trimBubblesTo(TWBubbleSet& keepSet);

    private:

    TWBubble **bubbles;
    USHORT numBubbles;
};

#endif

