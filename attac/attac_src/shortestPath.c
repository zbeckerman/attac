// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>

#include "attacGlobal.h"
#include "attacUtils.h"
#include "util.h"

const int warpsPerSector = 6;
const int infinity = 255;

unsigned int *ShortestPathToAll(long startSector)
{
  int i;
  unsigned int *distances;
  long *predecessors;
  long *workingSet;
  int workingSetSize = 0;
  long targetSector;
  long minDistance;
  long nearestSector = -1;
  long posInWorkingSet;

  distances = (int *) malloc((header.sector_cnt) * sizeof(int));

  if (startSector<=0)
    startSector=1;
  
  predecessors = 
    (long*) malloc((header.sector_cnt * warpsPerSector) * sizeof(long));
  workingSet =
    (long*) malloc((header.sector_cnt) * sizeof(long));


  // data reset
  for( i=0; i<header.sector_cnt; i++ )
    {
      distances[i] = infinity;
      predecessors[i] = -1;
      workingSet[i] = 0;
    }
  
  distances[startSector-1] = 0;
  workingSet[workingSetSize++] = startSector-1;
  
  while( workingSetSize != 0 )
    {
      minDistance = infinity;
      
      // extract cheapest sector
      posInWorkingSet = 0;
      // extracting the cheapest sector would be more efficient if we did not have to compare the 
      // entire working set all the time. A priority queue would be optimal, but I'm to lazy to make one
      for( i=0; i<workingSetSize; i++ )
	{
	  if( distances[workingSet[i]] < (unsigned int) minDistance )
	    {
	      minDistance = distances[workingSet[i]];
	      nearestSector = workingSet[i];
	      posInWorkingSet = i;
	    }
	}
      workingSetSize--;
      memmove( &workingSet[posInWorkingSet], &workingSet[posInWorkingSet + 1], (workingSetSize - posInWorkingSet) * sizeof(long));
      
      assert( minDistance != infinity );
      
      // relax all sectors adjacent to the nearest sector
      for( i=0; i<warpsPerSector; i++ )
	{  
	  targetSector = (sectors[nearestSector].warp_sect[i]) - 1;
	  if( targetSector == -1 )
	    break;
	  
	  if( distances[targetSector] > distances[nearestSector] + 1 ) 
	    {
	      // new node? if it's new then it's not in the working set
	      if( distances[targetSector] == (unsigned int) infinity )
		workingSet[workingSetSize++] = targetSector;
	      
	      distances[targetSector] = distances[nearestSector] + 1;
	      predecessors[targetSector] = nearestSector;
	    }
	}
    }

  assert( workingSetSize == 0 );
  
	#ifdef P_FREE
		pfree(&workingSet);
		pfree(&predecessors);
	#else
		free(workingSet);
		free(predecessors);
	#endif		


  return distances;
}
