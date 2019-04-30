#if !defined(AUTHORIZE_H)
#define AUTHORIZE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct passedInValues {
	//return values
	char *r_returnBuffer;
	int r_isRegDeactivated;
	char *p_userId;
	int p_releaseVersion;
	int p_majorVersion;
	int p_minorVersion;
	char *p_betaVersion;
	char *p_program;
	char *p_xmlData;
	char *p_action;
	void (*p_AuthorizeCallback)(int r_isRegDeactivated, char *r_returnBuffer);
};


#define MAX_LENGTH 1024
#define READ_WRITE_AMOUNT 1
#define SMTP_PORT 25
#define HTTP_PORT 80

#define FOUND_SMTP_NOTHING 0
#define FOUND_SMTP_200 1
#define FOUND_SMTP_211 2
#define FOUND_SMTP_214 3
#define FOUND_SMTP_220 4
#define FOUND_SMTP_235 5
#define FOUND_SMTP_250 6
#define FOUND_SMTP_334 7
#define FOUND_SMTP_354 8

#define DATE "Today\r\n"
#define INIT "data\r\n"
#define SENDNOW "\r\n.\r\n"
#define BYE "quit\r\n"	

static const char base64digitsDll[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define BAD     -1
static const char base64valDll[] = {
     BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
     BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD,
     BAD,BAD,BAD,BAD, BAD,BAD,BAD,BAD, BAD,BAD,BAD, 62, BAD,BAD,BAD, 63,
      52, 53, 54, 55,  56, 57, 58, 59,  60, 61,BAD,BAD, BAD,BAD,BAD,BAD,
     BAD,  0,  1,  2,   3,  4,  5,  6,   7,  8,  9, 10,  11, 12, 13, 14,
      15, 16, 17, 18,  19, 20, 21, 22,  23, 24, 25,BAD, BAD,BAD,BAD,BAD,
     BAD, 26, 27, 28,  29, 30, 31, 32,  33, 34, 35, 36,  37, 38, 39, 40,
      41, 42, 43, 44,  45, 46, 47, 48,  49, 50, 51,BAD, BAD,BAD,BAD,BAD
};
#define DECODE64DLL(c)  (isascii(c) ? base64valDll[c] : BAD)


//encrypt.h
#ifndef _H_BLOWFISH
#define _H_BLOWFISH

#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#define MAXKEYBYTES 56          // 448 bits 
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

// choose a byte order for your hardware 

#ifdef WORDS_BIGENDIAN
// ABCD - big endian - motorola
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
#endif  // WORDS_BIGENDIAN

#ifndef WORDS_BIGENDIAN
// DCBA - little endian - intel
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
#endif  // !WORDS_BIGENDIAN



//define the boxes. 
  // from bf_tab.h

extern UWORD_32bits initbf_P[bf_N + 2];
extern UWORD_32bits initbf_S[4][256];

char *encrypt_string (char *key, char *str);
char *decrypt_string (char *key, char *str);


#endif
//encrypt.h


void JumpgateThreadProc(void *d);
void AuthorizeRegistrationThreadProc(void *d);
int isSmtpResponseValid(char response[]);
char * getEmailBody(int isRegDeactivated, struct passedInValues *fPassedInValues);
char * getEmailSubject(char userId[]);
int invalidateRegistration (char *invalidReg, struct passedInValues *mPassedInValues);
char* getSmtpEmailInfo(int programCaller);
char* getInvalidRegistrations();

//end authorization

void to64frombits(unsigned char *out, const unsigned char *in, int inlen);
int from64tobits(char *out, const char *in);
long Instr (long startPos, char stringBeingSearched[] , char stringToFind[] );
char* right (char tmp[], int len);
char* left (char tmp[], int len);

#endif