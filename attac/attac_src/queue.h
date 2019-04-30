#ifndef __queue_h__
#define __queue_h__


# define TRUE       1
# define FALSE      0
# define MAX_SIZE   1000000

void pushQueue( long i );
void printQueue( void );
long popQueue( void );
int isQueueEmpty();
void queueEmpty();

extern long queue[MAX_SIZE];
extern long firstElement;
extern long firstFree;
extern int isFull;
extern int isEmpty;

#endif
