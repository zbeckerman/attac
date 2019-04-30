#ifndef __ztm_h__
#define __ztm_h__

#define MAX_ONE_WAY_WARPS 10000
#define MAX_ONE_WAY_WARP_LENGTH 10

void startZTM();


extern int currentZTM;
extern int forwardZTM;
extern int backwardZTM;
extern int oneWayZTM;
extern int randomZTM;
extern char oneWayWarps[MAX_ONE_WAY_WARPS][MAX_ONE_WAY_WARP_LENGTH];
extern int oneWayWarpCount;
extern int totalOneWayWarpCount;

int isOneWayWarp (long x, long y);
void parseZTMBufferData(char *);
char * removeParens (char * tmp);
void addWarpToSector (long sector, long previousSector);

#endif
