// Common.cpp

#include <time.h>
#include "Common.h"

TWX_FILE_HEADER headerArray[MAX_GAMES];
TWX_SECTOR *sectorsArray[MAX_GAMES];
TWX_PLAYER *playersArray[MAX_GAMES];
TWX_PLANET *planetsArray[MAX_GAMES];

CHash *traderHashArray[MAX_GAMES];


//////////////////////////////////////////////////////////////////////////
// Common variables
char s_error_msg[MAX_BUF_LEN*4];
int gameCount=0;
HANDLE SyncUsersMutex;
HANDLE ListOfConnectedUsersMutex;
HANDLE freeMutex;
HANDLE parseSafeMutex;
HANDLE zlibEngineMutex;
char appDir[MAX_LENGTH]="c:\\";
const char gameExtension[5]=".srv";

//version of software
int releaseVersion=1;
int majorVersion=0;
int minorVersion=5;

//////////////////////////////////////////////////////////////////////////
// Common functions

/**
 * Base 64 encoding of string.
 *
 * @param data The string to encode
 * @param n The length of the string
 */
void TWC_Encode64(unsigned char *data, int n)
{
	unsigned char buf[256];
	unsigned char *src_ptr;
	unsigned char *dst_ptr;

	src_ptr=data;
	dst_ptr=buf;
	while (n > 0) {
		*dst_ptr++='0'+(src_ptr[0]>>2);
		*dst_ptr++='0'+((src_ptr[0]<<4)&0x30)+(src_ptr[1]>>4);
		*dst_ptr++='0'+((src_ptr[1]<<2)&0x3C)+(src_ptr[2]>>6);
		*dst_ptr++='0'+(src_ptr[2]&0x3F);
		src_ptr+=3;
		n--;
	}
	*dst_ptr=0;
	strcpy((char *)data, (char *)buf);
}

/**
 * Base 64 decoding of string.
 *
 * @param data The string to decode
 * @param n The length of the string
 */
void TWC_Decode64(unsigned char *data, int n)
{
	unsigned char *src_ptr;
	unsigned char *dst_ptr;
	int i;

	src_ptr=data;
	dst_ptr=data;
	while (n > 0) {
		for (i=0; i<4; i++) {
			src_ptr[i]-='0';
		}
		*dst_ptr++=(src_ptr[0]<<2)+(src_ptr[1]>>4);
		*dst_ptr++=(src_ptr[1]<<4)+(src_ptr[2]>>2);
		*dst_ptr++=(src_ptr[2]<<6)+src_ptr[3];
		src_ptr+=4;
		n--;
	}
}


long Get_Time()
{
	time_t data;

	time(&data);

	return data;
}