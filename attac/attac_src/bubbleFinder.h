#ifndef __bubbleFinder_h__
#define __bubbleFinder_h__

void findBubblesThreadProc(void *dummy);

//typedef void (* my_findATTACBubbles)(int argc, char* argv[], TWX_SECTOR* sectors, char *bubbleCharDataStorage); 
//my_findATTACBubbles findATTACBubbles;

extern char * bubbleCharDataStorage;
extern int bubbleOffset;
#define MAX_BUBBLE_SECTORS 500
#endif
