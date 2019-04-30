#ifndef _H_BLOWFISH
#define _H_BLOWFISH

#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#define MAXKEYBYTES 56          /* 448 bits */
#define bf_N             16
#define noErr            0
#define DATAERROR         -1
#define KEYBYTES         8

#define UBYTE_08bits  unsigned char
#define UWORD_16bits  unsigned short

#define nmalloc(x) n_malloc((x),__FILE__,__LINE__)

#define SIZEOF_INT 2
#define SIZEOF_LONG 4


#if SIZEOF_INT==4
#  define UWORD_32bits  unsigned int
#else
#  if SIZEOF_LONG==4
#  define UWORD_32bits  unsigned long
#  endif
#endif

/* choose a byte order for your hardware */

#ifdef WORDS_BIGENDIAN
/* ABCD - big endian - motorola */
union aword {
  UWORD_32bits word;
  UBYTE_08bits byte [4];
  struct {
    unsigned int byte0:8;
    unsigned int byte1:8;
    unsigned int byte2:8;
    unsigned int byte3:8;
  } w;
};
#endif  /* WORDS_BIGENDIAN */

#ifndef WORDS_BIGENDIAN
/* DCBA - little endian - intel */
union aword {
  UWORD_32bits word;
  UBYTE_08bits byte [4];
  struct {
    unsigned int byte3:8;
    unsigned int byte2:8;
    unsigned int byte1:8;
    unsigned int byte0:8;
  } w;
};
#endif  /* !WORDS_BIGENDIAN */




/* define the boxes. 
   from bf_tab.h */

extern UWORD_32bits initbf_P[bf_N + 2];
extern UWORD_32bits initbf_S[4][256];

char *encrypt_string (char *key, char *str);
char *decrypt_string (char *key, char *str);
char* getFileAndDecrypt (char* file);
char * getFileWithoutDecrypt (char* file);

#endif
