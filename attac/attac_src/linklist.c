// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>    
#include <malloc.h>
#include <windows.h>

#include "attacGlobal.h"
#include "../PUTTY.H"
#include "rexx.h"
#include "linklist.h"


//#define TEST_CODE 1
//dont want this defined when we link with attac
#ifdef TEST_CODE
int main()
{
	int size=10, i;

	listPlanets = (planetListPtr**) malloc (size * sizeof (planetListPtr));

	for (i=0; i<size; i++)
		listPlanets[i]=NULL;


	insert_link_list_node("I", 0);
	insert_link_list_node("Love", 0);
	insert_link_list_node("Laurie", 0);

	print_link_list(0);

	delete_link_list(0);

	insert_link_list_node("TEST", 0);

	print_link_list(0);

	delete_link_list(0);

	insert_link_list_node("11", 0);
	insert_link_list_node("22", 0);
	insert_link_list_node("33", 0);
	insert_link_list_node("44", 0);
	insert_link_list_node("55", 0);

	print_link_list(0);

	delete_link_list(0);

	#ifdef P_FREE
		pfree(&listPlanets);
	#else
		free(listPlanets);
	#endif		

	return (0);
}

#endif

void insert_link_list_node(char *str, long i)
{
	planetListPtr tmp, tmp1;
	char *tStr;

	tStr = left (str, 69);
	tStr[strlen(str)]='\x0';
	if (listPlanets[i]==NULL)	//first node
	{
		/* the first record is always a special case */
		tmp = (struct planetList *)malloc(sizeof(struct planetList));
		strcpy(tmp->name,tStr);
		tmp->next = NULL;
		listPlanets[i] = (planetListPtr*)tmp;
	}
	else
	{
		tmp = (struct planetList *)malloc(sizeof(struct planetList));
		strcpy(tmp->name,tStr);
		tmp->next = NULL;   // point this "next" to NULL        
		
		//slide to the end of the list
		tmp1 = (planetListPtr) listPlanets[i]; 

		while (tmp1->next != NULL)
			tmp1 = tmp1->next;
		
		tmp1->next = tmp;        // this is now the prior record     
	}

	#ifdef P_FREE
		pfree(&tStr);
	#else
		free(tStr);
	#endif		
}

void print_link_list (long i)
{
	planetListPtr tmp;

	tmp = (planetListPtr) listPlanets[i];
	while (tmp != NULL)
	{
		printf("%s\n", tmp->name);
		tmp = tmp->next;
	} 
}

char * get_link_list_nth_data(long i, int nth)
{
	planetListPtr tmp;
	int count=0;
	char *strTmp;

	strTmp = (char *) malloc (70);
	tmp = (planetListPtr) listPlanets[i];
	while (tmp != NULL && count<=nth)
	{
		strcpy (strTmp, tmp->name);
		tmp = tmp->next;
		count++;
	} 
	return (strTmp);
}

int get_link_list_length(long i)
{
	planetListPtr tmp;
	int count=0;

	tmp = (planetListPtr) listPlanets[i];
	while (tmp != NULL)
	{
		count++;
		tmp = tmp->next;
	} 
	return (count);
}
void delete_link_list (long i)
{
	planetListPtr tmp, tmp1;

	tmp = (planetListPtr) listPlanets[i];
	while (tmp != NULL)  // quit when next is NULL  
	{
		tmp1 = tmp;
		tmp = tmp->next;   // next block of data        
		#ifdef P_FREE
			pfree(&tmp1);           // free present block        
		#else
			free(tmp1);           // free present block        
		#endif		

	} 
	listPlanets[i]=NULL;
}
