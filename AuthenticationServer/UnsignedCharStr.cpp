#include <windows.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <winbase.h>


#include "UnsignedCharStr.h"
#include "AuthenticationServer.h"


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
