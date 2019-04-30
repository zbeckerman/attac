
#if defined( _WINDOWS )
#include <windows.h>
#endif

#include "JumpgateServer.h"
#include "zlibengn.h"
#include "UnsignedCharStr.h"

//
// The constructor initializes a couple of members
// of the z_stream class.  See the Zlib documentation
// for details on what those members do
//

CUnsignedCharStr::CUnsignedCharStr()
{
	appendMutex = CreateMutex (NULL, 0, NULL);

	unsignedCharPtr = (unsigned char *) malloc (1);
	memset(unsignedCharPtr, 0, 1);
	length=0;
}

CUnsignedCharStr::CUnsignedCharStr(char *str)
{
	long len = strlen (str);

	appendMutex = CreateMutex (NULL, 0, NULL);

	unsignedCharPtr = (unsigned char *) malloc (1);
	memset(unsignedCharPtr, 0, 1);
	length=0;

	appendString((unsigned char *) str, len);

}


CUnsignedCharStr::~CUnsignedCharStr()
{
	WaitForSingleObject(appendMutex, INFINITE);

	if (unsignedCharPtr!= NULL)
		pfree((void**) &unsignedCharPtr);
	length=0;

	ReleaseMutex(appendMutex);
	CloseHandle(appendMutex);
}

long CUnsignedCharStr::getLength() {
	return length;
}

unsigned char* CUnsignedCharStr::getString() {
	WaitForSingleObject(appendMutex, INFINITE);
	unsigned char * tmp = (unsigned char *) malloc (length+1);
	memcpy (tmp, unsignedCharPtr, length);

	tmp[length]=0;
	ReleaseMutex(appendMutex);
	return tmp;
}

int CUnsignedCharStr::endsWithString(char *str) {
	WaitForSingleObject(appendMutex, INFINITE);
	
	long len = strlen(str);
	int result = memcmp( unsignedCharPtr+(length-len), str, len);

	ReleaseMutex(appendMutex);
	return (result);
}


int CUnsignedCharStr::trimStringRight(int count) {
	WaitForSingleObject(appendMutex, INFINITE);

	long newSize = length - count;
	if (newSize>0) {
		unsigned char *tmp = (unsigned char *) malloc (newSize + 1);
		memcpy (tmp, unsignedCharPtr, length-count);

		tmp[newSize]=0;
		pfree((void **)&unsignedCharPtr);
		unsignedCharPtr = tmp;
		length -= count;
	}
	ReleaseMutex(appendMutex);
	return 0;
}

int CUnsignedCharStr::appendString(unsigned char *str, long strLength) {
	WaitForSingleObject(appendMutex, INFINITE);

	long newSize = length + strLength;
	if (strLength>0) {
		unsigned char *tmp = (unsigned char *) malloc (newSize + 1);
		memcpy (tmp, unsignedCharPtr, length);
		memcpy (tmp+length, str, strLength);

		tmp[newSize]=0;
		pfree((void **)&unsignedCharPtr);
		unsignedCharPtr = tmp;
		length += strLength;
	}
	ReleaseMutex(appendMutex);
	return strLength;
}


CUnsignedCharStr::deleteString()
{
	WaitForSingleObject(appendMutex, INFINITE);
	pfree((void**) &unsignedCharPtr);
	unsignedCharPtr = (unsigned char *) malloc (1);
	memset(unsignedCharPtr, 0, 1);
	length=0;
	ReleaseMutex(appendMutex);
}


int CUnsignedCharStr::encryptString()
{
	int i;

	WaitForSingleObject(appendMutex, INFINITE);

	unsigned char *tmp = (unsigned char *) malloc (length + 1);

	for(i=0; i<length; i++)
		tmp[i] = (unsigned char)((int)unsignedCharPtr[i] ^ (int)TRADEWARS[i%TRADEWARS_LEN]);

	tmp[length]=0;
	pfree((void **)&unsignedCharPtr);
	unsignedCharPtr = tmp;

	ReleaseMutex(appendMutex);
	return length;
}

int CUnsignedCharStr::decryptString()
{
	int i;

	WaitForSingleObject(appendMutex, INFINITE);

	unsigned char *tmp = (unsigned char *) malloc (length + 1);

	for(i=0; i<length; i++)
		tmp[i] = (unsigned char)((int)unsignedCharPtr[i] ^ (int)TRADEWARS[i%TRADEWARS_LEN]);

	tmp[length]=0;
	pfree((void **)&unsignedCharPtr);
	unsignedCharPtr = tmp;

	ReleaseMutex(appendMutex);
	return length;
}

unsigned long CUnsignedCharStr::crc32() {
  char buf[16]; 	  
  unsigned long crc32=0;
  int i=0,n=0;

  n = length/16;

  //if i am exactly divisible by 16, just iterate the exact number of times
  if (length % 16==0)
	  n=n-1;

  for (i=0; i<=n; i++)
  {
	int len=16;
	//initialize memory
	memset(&buf, 0, sizeof(buf));

	//only copy what we have
	if (i==n) {
		//last iteration
		len = length - (n*16);
	}
    memcpy (buf, unsignedCharPtr+(i*16), len);
    crc32=crc32^CalcCRC32(buf, 16);
  }
  return crc32;
}

unsigned long CUnsignedCharStr::CalcCRC32(void *ptr, long size)
{
  unsigned long crc32=0;
  unsigned long *lp;
  int i,n;

  lp=(unsigned long *)ptr;
  n=size/4;
  for (i=0; i<n; i++)
  {
    crc32=crc32^lp[i];
  }
  return crc32;
}

unsigned char* CUnsignedCharStr::getCrc32FromString(int count) {
	WaitForSingleObject(appendMutex, INFINITE);
	
	unsigned char *tmpCrc32 = (unsigned char *) malloc (length-count-CRC_32_LEN + 1);

	if (length-count>=0)
		memcpy (tmpCrc32, unsignedCharPtr+(count+CRC_32_LEN), length-count-CRC_32_LEN);

	tmpCrc32[length-count-CRC_32_LEN]=0;
	ReleaseMutex(appendMutex);

	return tmpCrc32;
}


int CUnsignedCharStr::hasString(char  str[]) {
	WaitForSingleObject(appendMutex, INFINITE);
	
	int result=-1;
	long len = strlen(str);
	for (int i=length-len; i>=0 && result!=0; i--) {
		result = memcmp( unsignedCharPtr+( i), str, len);
	}

	ReleaseMutex(appendMutex);
	if (i==-1)
		return -1;
	else if (result==-1)
		return -1;
	else
		return i+1;
}

int CUnsignedCharStr::confirmCrc32(unsigned char *str) {
	char buffer[64];
	sprintf (buffer, "%lu", crc32());
	if (strcmp (buffer, (char *)str)==0)
		return 1;
	else
		return 0;
}
