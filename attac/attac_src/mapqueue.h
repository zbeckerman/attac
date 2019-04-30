#ifndef __mapqueue_h__
#define __mapqueue_h__


# define TRUE       1
# define FALSE      0
# define MAX_SIZE   2000
#define MAX_MAP_LENGTH 256

void pushMapQueue( char * i );
void printMapQueue( void );
char* popMapQueue(void);
int isMapQueueEmpty();
void queueMapEmpty();
void queueMap2Empty();

extern char mapqueue[MAX_SIZE][MAX_MAP_LENGTH];
extern long firstMapElement;
extern long firstMapFree;
extern int isMapFull;
extern int isMapEmpty;

#endif
