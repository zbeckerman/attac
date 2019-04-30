
#pragma once

#include "Shape.h"
#include "Sector.h"
#include "Sphere.h"
#include "Queue.h"


class CUtil
{
public:
	CUtil();
	virtual ~CUtil();
	CObList* getSectorsFromDatabase ();
	CObList* loadMapFromSector (long sector, int levels);
	CSector* getSectorFromList (CObList* theList, long sector);

};
