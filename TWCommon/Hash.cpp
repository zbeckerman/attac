// List.cpp: implementation of the CHash class.
//

#include <windows.h>


#include <string.h>
#include <limits.h>
#include <math.h>
#include "Hash.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CHash::CHash()
{
	int i=0;
	m_nCount=0;
	for (i=0; i<HASHELEMENTS; i++) {
		m_pHashTable[i]=NULL;
	}
}

CHash::~CHash()
{
}

//////////////////////////////////////////////////////////////////////

unsigned int CHash::Hash (char *key) {
   int i;			// Our temporary variables
   int n;			// Our temporary variables
   unsigned int hashValue;
   unsigned int ival;
   char *p;			// p lets us reference the integer value as a
                                // character array instead of an integer.  This
                                // is actually pretty bad style even though it
                                // works.  You should try a union if you know
                                // how to use them.

   p = (char *) &ival;		// Make p point to i, without the type cast you	
							// should get a warning.
 
   hashValue = ival = 0;		// Initialize our variables

   // Figure out how many characters are in an integer the correct answer is 4 (on an i386), but this should be more cross platform.
   n = (int) (((log10((double)(UINT_MAX)) / log10(2.0))) / CHAR_BIT) + 0.5;

   // loop through s four characters at a time
   for(i = 0; i < (int) strlen(key); i += n) {
      // voodoo to put the string in an integer don't try and use strcpy, it
      // is a very bad idea and you will corrupt something.
      strncpy(p, key + i, n);
      // accumulate our values in hashValue
      hashValue += ival;
   }

   // divide by the number of elements and return our remainder
   return hashValue % HASHELEMENTS;
}


int CHash::Add(char *key, char *data)
{
	CHashItem *pItem;
	CHashItem *pItem2;
	
	unsigned int hashValue;

	pItem=new CHashItem();
	pItem->key=strdup (key);
	pItem->data=strdup (data);

	hashValue = Hash(key);

	if (m_pHashTable[hashValue]==NULL) {
		m_pHashTable[hashValue] = pItem;
		m_pHashTable[hashValue]->parent = NULL;
		m_pHashTable[hashValue]->child = NULL;
	} else {
      pItem2=m_pHashTable[hashValue];
      while(pItem2->child != NULL) {
         pItem2=pItem2->child;
      }
      pItem2->child = pItem;
      pItem->child = NULL;
      pItem->parent = pItem2;
	}
	return m_nCount++;
}

void CHash::Remove(char *key)
{
   unsigned int hashval;
   CHashItem *pItem;
   
   hashval = 0;

   hashval = Hash(key);

   if (m_pHashTable[hashval] == NULL) {
      return;
   }
 
	m_nCount--;

   if (!strcmp((m_pHashTable[hashval]->key), (key))) {
      pItem = m_pHashTable[hashval];
      m_pHashTable[hashval] = pItem->child;
	  free(pItem->key);
	  free(pItem->data);
      free(pItem);
      return; 
   }
   else {
      if (m_pHashTable[hashval]->child == NULL) {
         return;
      }

      pItem = m_pHashTable[hashval]->child;


      if (!strcmp((pItem->key), (key))) {
         pItem->parent->child = pItem->child;
         if (pItem->child != NULL) {
            pItem->child->parent = pItem->parent;
	        }
		 free(pItem->key);
		 free(pItem->data);
         free(pItem);
         return; 
      }

      while (pItem->child != NULL) {
         if (!strcmp((pItem->key), (key))) {
            pItem->parent->child = pItem->child;
            if (pItem->child != NULL) {
               pItem->child->parent = pItem->parent;
            }
			free(pItem->key);
			free(pItem->data);
            free(pItem);
            return; 
         }
         pItem = pItem->child;
      }
      if (!strcmp((pItem->key), (key))) {
         pItem->parent->child = pItem->child;
         if (pItem->child != NULL) {
            pItem->child->parent = pItem->parent;
         }
	     free(pItem->key);
		 free(pItem->data);
         free(pItem);
         return; 
      }
      else {
         return;
     }
   }

  
}

int CHash::GetCount()
{
	return m_nCount;
}

/**
  * Caller frees memory of char
  */
char* CHash::FindValue(char *key)
{
   unsigned int hashval;
   CHashItem *pItem;
   
   hashval = 0;

   hashval = Hash(key);

   
   if (m_pHashTable[hashval] == NULL) {
      return NULL;
   }
 
   if (!strcmp((m_pHashTable[hashval]->key), (key))) {
      pItem = m_pHashTable[hashval];
      return strdup (pItem->data); 
   }
   else {
      if (m_pHashTable[hashval]->child == NULL) {
         return NULL;
      }

      pItem = m_pHashTable[hashval]->child;


      if (!strcmp((pItem->key), (key))) {
         return strdup (pItem->data); 
      }

      while (pItem->child != NULL) {
         if (!strcmp((pItem->key), (key))) {
            return strdup (pItem->data); 
         }
         pItem = pItem->child;
      }
      if (!strcmp((pItem->key), (key))) {
         return strdup (pItem->data); 
      }
      else {
         return NULL;
     }
   }
}


/*
	CHash *t = new CHash();
	
	t->Add("test1", "rrrr1");
	t->Add("test2", "rrrr2");
	t->Add("test3", "rrrr3");
	t->Add("test4", "rrrr4");
	t->Add("test5", "rrrr5");

	char *a = t->FindValue ("test1");

	int dd = t->GetCount();

	t->Remove("test1");

	char *aa = t->FindValue ("test1");

	int ddd = t->GetCount();
	
*/