#ifndef _TWWARP_HPP_
#define _TWWARP_HPP_

#include "TWTypes.hpp"


class TWWarp
{
    public:

    inline TWWarp()
    {
        sect=0;
        oneWay=false;
    }

    inline TWWarp(USHORT sectNum)
    {
        sect=sectNum;
        oneWay=false;
    }

    inline Bool isOneWay(void) const
    {
        return oneWay;
    }

    inline void setOneWay(Bool isOneWay)
    {
        oneWay=isOneWay;
    }

    inline USHORT sector(void) const
    {
        return sect;
    }

    inline void setSector(USHORT sectNum)
    {
        sect=sectNum;
    }

    private:

    USHORT sect;
    Bool oneWay;
};

#endif

