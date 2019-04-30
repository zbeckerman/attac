// Line.cpp : implementation file
//

#include "stdafx.h"
#include "Sector.h"


//Constructors
CSector::CSector()
{
}
//Destructor
CSector::~CSector()
{
}

BOOL CSector::Equals(long a_sector) {
	return  (sectorNumber == a_sector);
}
