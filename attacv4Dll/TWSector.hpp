#ifndef _TWSECTOR_HPP_
#define _TWSECTOR_HPP_

#include <string.h>
#include <fstream.h>

#include "TWTypes.hpp"
#include "TWBubble.hpp"
#include "TWWarp.hpp"
#include "AbEnds.hpp"



typedef struct TWSectorData
{
    TWWarp *inWarps;
    TWWarp *outWarps;
    UCHAR numInWarps;
    UCHAR numOutWarps;
    Bool explored;
};

typedef struct TWBinaryHeader
{
    ULONG sig1;
    ULONG sig2;
    ULONG reserved;
    UCHAR majorVer;
    UCHAR minorVer;
    USHORT sectorCount;
};

typedef struct TWBinarySectorData
{
    UCHAR explored;
    UCHAR reserved[3];
    USHORT outWarps[MAX_WARPS];
};

class TWSector
{
    public:

    inline TWSector()
    {
        initData();
    }

    inline ~TWSector()
    {
        nixData();
    }

    TWSector(TWSector& copy);
    TWSector& operator=(const TWSector& assign);

    inline UCHAR numberOfInWarps(void) const
    {
        return sdata->numInWarps;
    }
    inline UCHAR numberOfOutWarps(void) const
    {
        return sdata->numOutWarps;
    }

    inline TWWarp& inWarpAtPosition(UCHAR index)
    {
        return sdata->inWarps[index];
    }
    inline TWWarp& outWarpAtPosition(UCHAR index)
    {
        return sdata->outWarps[index];
    }

    Bool addInWarp(const TWWarp& addition);
    Bool addOutWarp(const TWWarp& addition);

    inline Bool isExplored(void) const
    {
        return sdata->explored;
    }

    inline void setExplored(Bool expState)
    {
        sdata->explored=expState;
    }

// Add the define for your compiler if it optimizes better with the
// following functions inlined.  Make sure to modify the .CPP file
// accordingly.

    #if (defined(__IBMCPP__) || defined(__TCPLUSPLUS__))
    inline Bool TWSector::hasInWarp(USHORT testSect) const
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

    inline Bool TWSector::hasOutWarp(USHORT testSect) const
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
    #else
    Bool hasInWarp(USHORT testSect) const;
    Bool hasOutWarp(USHORT testSect) const;
    #endif

    private:

    inline void initData(void)
    {
        sdata = new TWSectorData;
        if (sdata==NULL)
            OutOfMemory();
        sdata->numInWarps=0;
        sdata->numOutWarps=0;
        sdata->inWarps=0;
        sdata->outWarps=0;
        sdata->explored=false;
    }

    inline void nixData(void)
    {
        if (sdata->numInWarps>0)
            delete[] sdata->inWarps;
        if (sdata->numOutWarps>0)
            delete[] sdata->outWarps;
        delete sdata;
        sdata=0;
    }

    TWSectorData *sdata;
};

class TWSectorArray
{
    public:

    inline TWSectorArray(USHORT ns)
    {
        numSects=ns;
        sects = new TWSector[numSects];
        if (sects==NULL)
            OutOfMemory();
    }

    inline TWSectorArray()
    {
        numSects=0;
        sects=0;
    }

    inline ~TWSectorArray()
    {
        if (numSects>0)
            delete[] sects;
    }

    inline Bool setNumberOfSectors(USHORT ns)
    {
        if (numSects>0)
            delete[] sects;
        numSects=ns;
        sects = new TWSector[numSects];
        if (sects==NULL)
            return false;
        else
            return true;
    }

    inline USHORT numberOfSectors(void) const
    {
        return numSects;
    }

    inline TWSector& operator[](USHORT index)
    {
        return sects[index];
    }

    inline TWSector& sectorAtPosition(USHORT index)
    {
        return sects[index];
    }

    void readWarpData(ifstream &dataStream);
	void readSectorData(TWX_SECTOR *sectors);
    void readPlotData(ifstream &dataStream);
    void readBinaryData(fstream &dataStream);
    void writeBinaryData(fstream &dataStream);
    void setOneWayStatus(void);
    void findBubbles(TWBubbleSet &bubbleSet);
    Bool traverseBubble(TWBubbleSet &bubbleSet);

    private:

    Bool addWarps(USHORT previous, USHORT sectNum);

    inline void maxBubblesMet(void)
    {
        cout << endl << endl << "Maximum bubbles (" << Globals::maxBubbles
             << ") exceeded, terminating search." << endl << endl
             << "Either adjust parameters to find less bubbles, or use the"
             << endl << "/R switch to reserve more storage." << endl << flush;
        if (uniSects!=NULL)
            delete[] uniSects;
    }

    TWSector *sects;
    USHORT numSects, *uniSects, i, j, entrance, previous, bubbleCursor,
             tempCursor, sectNum, tsect;
    UCHAR numUniSects, warps, uniMasks;
    TWBubble *currBubble, *tempBubble;
    ULONG uniIterator;
    Bool bubbleGood, bubbleDone;
};

#endif
