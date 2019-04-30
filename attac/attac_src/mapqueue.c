// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

# include <stdio.h>
# include <stdlib.h>
#include <string.h>

#include "attacGlobal.h"
#include "mapqueue.h"
#include "util.h"

#ifdef TEST

int main(void)
{
    int i;
	char *x, t[10];
    
    x = popMapQueue();
	if (x != NULL)
		pfree(&x);
    printMapQueue();
    
    for(i=34; i<48; i++){
		sprintf (t, "%d", i);
        pushMapQueue(t);
    }
    printMapQueue();

    x = popMapQueue();
	if (x != NULL)
		pfree(&x);
    printMapQueue();
    pushMapQueue("78");
    printMapQueue();


    x = popMapQueue();
	if (x != NULL)
		pfree(&x);
    x = popMapQueue();
	if (x != NULL)
		pfree(&x);
    printMapQueue();


    pushMapQueue("89");
    pushMapQueue("99");
    pushMapQueue("100");
    printMapQueue();
    
    for(i=23; i<35; i++){
        x = popMapQueue();
		if (x != NULL)
			pfree(&x);
    }
    printMapQueue();


    
    return 0;
}
#endif

char mapqueue[MAX_SIZE][MAX_MAP_LENGTH];
long firstMapElement        = 0;
long firstMapFree           = 0;
int isMapFull              = FALSE;
int isMapEmpty             = TRUE;


void pushMapQueue( char * i )
{
    if(isMapFull){
        printf("Can't push %s since the mapqueue is full\n",i);
    } else {
        strcpy (mapqueue[firstMapFree++] , i);
        firstMapFree           = firstMapFree % MAX_SIZE;
        isMapEmpty             = FALSE;
        isMapFull              = firstMapFree == firstMapElement;
    }
}

char* popMapQueue(void)
{
	char* retValue;
    if(isMapEmpty){
        printf("Can't pop a value since the mapqueue is empty\n");
        return NULL;
    } else {
        retValue = (char *) strdup (mapqueue[firstMapElement++]);
        firstMapElement    = firstMapElement % MAX_SIZE;
        isMapFull          = FALSE;
        isMapEmpty         = firstMapFree == firstMapElement;
        return retValue;
    }
}


void printMapQueue(){
    if(!isMapEmpty){
        long cur = firstMapElement;
        do{
            printf("%s ",mapqueue[cur++]);
            cur = cur % MAX_SIZE;
        } while(cur != firstMapFree);
    } else {
        printf("mapqueue is empty");
    }
    printf("\n");
}


int isMapQueueEmpty()
{
	return (firstMapFree == firstMapElement);
}

void queueMap2Empty()
{
	char *x;

	while (!isMapEmpty)
	{
		x = popMapQueue();
		if (x != NULL)
		{
			#ifdef P_FREE
				pfree(&x);           // free present block        
			#else
				free(x);           // free present block        
			#endif		
		}
	}
}
