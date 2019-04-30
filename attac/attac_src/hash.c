#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <malloc.h>

#include "attacGlobal.h"
#include "hash.h"
#include "util.h"

struct htab *hashtab[HASHELEMENTS];

unsigned int hash (char *s) {
   int i;			// Our temporary variables
   int n;			// Our temporary variables
   unsigned int hashval;
   unsigned int ival;
   char *p;			// p lets us reference the integer value as a
                                // character array instead of an integer.  This
                                // is actually pretty bad style even though it
                                // works.  You should try a union if you know
                                // how to use them.

   p = (char *) &ival;		// Make p point to i, without the type cast you					// should get a warning.
 
   hashval = ival = 0;		// Initialize our variables

   // Figure out how many characters are in an integer the correct answer is 4 (on an i386), but this should be more cross platform.
   n = (int) (((log10((double)(UINT_MAX)) / log10(2.0))) / CHAR_BIT) + 0.5;

   // loop through s four characters at a time
   for(i = 0; i < (int) strlen(s); i += n) {
      // voodoo to put the string in an integer don't try and use strcpy, it
      // is a very bad idea and you will corrupt something.
      strncpy(p, s + i, n);
      // accumulate our values in hashval
      hashval += ival;
   }

   // divide by the number of elements and return our remainder
   return hashval % HASHELEMENTS;
}

struct htab *addhash (char *key, char *data) {
   struct htab *newhash;
   struct htab *curhash;
   unsigned int i, hashval;

   newhash = (struct htab *)(malloc(sizeof(struct htab)));
   if (newhash == NULL) {
   }

   for(i = 0; i <= strlen(key); i++) {
      newhash->key[i] = key[i];
   }
   for(i = 0; i <= strlen(key); i++) {
      newhash->data[i] = data[i];
   }

   hashval = hash(key);

   if (hashtab[hashval] == NULL) {
      hashtab[hashval] = newhash;
      hashtab[hashval]->parent = NULL;
      hashtab[hashval]->child = NULL;
   }
   else {
      curhash=hashtab[hashval];
      while(curhash->child != NULL) {
         curhash=curhash->child;
      }
      curhash->child = newhash;
      newhash->child = NULL;
      newhash->parent = curhash;
   }
   
   return newhash;
}

struct htab *findhash(char *key) {
   unsigned int hashval;
   struct htab *curhash;
   
   hashval = 0;

   hashval = hash(key);

   
   if (hashtab[hashval] == NULL) {
      return NULL;
   }
 
   if (!strcmp((hashtab[hashval]->key), (key))) {
      curhash = hashtab[hashval];
      return curhash; 
   }
   else {
      if (hashtab[hashval]->child == NULL) {
         return NULL;
      }

      curhash = hashtab[hashval]->child;


      if (!strcmp((curhash->key), (key))) {
         return curhash;
      }

      while (curhash->child != NULL) {
         if (!strcmp((curhash->key), (key))) {
            return curhash;
         }
         curhash = curhash->child;
      }
      if (!strcmp((curhash->key), (key))) {
         return curhash;
      }
      else {
         return NULL;
     }
   }
}

int delhash(char *key) {
   unsigned int hashval;
   struct htab *curhash;
   
   hashval = 0;

   hashval = hash(key);

   
   if (hashtab[hashval] == NULL) {
      return 0;
   }
 
   if (!strcmp((hashtab[hashval]->key), (key))) {
      curhash = hashtab[hashval];
      hashtab[hashval] = curhash->child;
		#ifdef P_FREE
			pfree(&curhash);
		#else
			free(curhash);
		#endif		
      return 1; 
   }
   else {
      if (hashtab[hashval]->child == NULL) {
         return 0;
      }

      curhash = hashtab[hashval]->child;


      if (!strcmp((curhash->key), (key))) {
         curhash->parent->child = curhash->child;
         if (curhash->child != NULL) {
            curhash->child->parent = curhash->parent;
         }
		#ifdef P_FREE
			pfree(&curhash);
		#else
			free(curhash);
		#endif		
         return 1; 
      }

      while (curhash->child != NULL) {
         if (!strcmp((curhash->key), (key))) {
            curhash->parent->child = curhash->child;
            if (curhash->child != NULL) {
               curhash->child->parent = curhash->parent;
            }
			#ifdef P_FREE
				pfree(&curhash);
			#else
				free(curhash);
			#endif		
            return 1; 
         }
         curhash = curhash->child;
      }
      if (!strcmp((curhash->key), (key))) {
         curhash->parent->child = curhash->child;
         if (curhash->child != NULL) {
            curhash->child->parent = curhash->parent;
         }
		#ifdef P_FREE
			pfree(&curhash);
		#else
			free(curhash);
		#endif		
         return 1; 
      }
      else {
         return 0;
     }
   }
}

void hashprofile(int profile) {
   int max, total, i, nodes, used;
   double avg;
   struct htab *curhash;
  
   max = total = i = nodes = used = 0;
   avg = 0;
 
   if (profile > 0) {
      for (i = 0; i < HASHELEMENTS; i++) {
         if (hashtab[i] != NULL) {
            used++;
            nodes = 0;
            curhash = hashtab[i];
            while(curhash != NULL) {
               nodes++;
               if (profile > 3) {
                  fprintf(stderr, "%s (%s)\n", curhash->key, curhash->data);
               }
               curhash = curhash->child; 
            }
            if (nodes != 0) total += nodes; 
            if (nodes > max) {
               max = nodes;
            }
            if (profile > 1) {
               fprintf(stderr, "i: %d\n", i); 
            }
         }
      }
      avg = (double)(total) / (double)(used);
      fprintf(stderr, "total: %d\n", total);
      fprintf(stderr, "max: %d\n", max);
      fprintf(stderr, "used: %f\n", 100 * ((double)(used) / (double)(HASHELEMENTS)));
      fprintf(stderr, "average: %f\n", avg);
   }
}
