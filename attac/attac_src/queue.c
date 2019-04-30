# include <stdio.h>
# include <stdlib.h>

#include "attacGlobal.h"
#include "queue.h"

#ifdef TEST

int main(void)
{
    int i;
    
    popQueue();
    printQueue();
    
    for(i=34; i<48; i++){
        pushQueue(i);
    }
    printQueue();


    popQueue();
    printQueue();
    pushQueue(78);
    printQueue();


    popQueue();
    popQueue();
    printQueue();


    pushQueue(89);
    pushQueue(99);
    pushQueue(100);
    printQueue();
    
    for(i=23; i<35; i++){
        popQueue();
    }
    printQueue();


    
    return 0;
}
#endif

long queue[MAX_SIZE];
long firstElement        = 0;
long firstFree           = 0;
int isFull              = FALSE;
int isEmpty             = TRUE;


void pushQueue( long i )
{
    if(isFull){
        printf("Can't push %ld since the queue is full\n",i);
    } else {
        queue[firstFree++]  = i;
        firstFree           = firstFree % MAX_SIZE;
        isEmpty             = FALSE;
        isFull              = firstFree == firstElement;
    }
}


long popQueue(){
    if(isEmpty){
        printf("Can't pop a value since the queue is empty\n");
        return 0;
    } else {
        long retValue    = queue[firstElement++];
        firstElement    = firstElement % MAX_SIZE;
        isFull          = FALSE;
        isEmpty         = firstFree == firstElement;
        return retValue;
    }
}


void printQueue(){
    if(!isEmpty){
        long cur = firstElement;
        do{
            printf("%3ld ",queue[cur++]);
            cur = cur % MAX_SIZE;
        } while(cur != firstFree);
    } else {
        printf("Queue is empty");
    }
    printf("\n");
}


int isQueueEmpty()
{
	return (firstFree == firstElement);
}

void queueEmpty()
{
	long x;

	while (!isEmpty)
		x = popQueue();
}
