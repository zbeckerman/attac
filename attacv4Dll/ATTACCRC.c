// ATTACCRC.cpp : Defines the initialization routines for the DLL.
//

#include "ATTACCRC.h"
#include "TWBFind.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int confirmCRCinTWXFile (char *file_name)
{
	FILE *myFile;
	unsigned long crc32=0;
	struct stat buf;

	myFile = fopen(file_name, "rb");
	if (myFile == 0)
	{
		return(2);
	}
	else
	{
		stat(file_name, &buf);
		crc32 = getCRC32(myFile, buf.st_size);
	}
    
	fclose(myFile);
	if (crc32==0)
		return 0;
	else
		return 1;
}


int updateCRCinTWXFile (char *file_name)
{
	FILE *myFile;
	unsigned long crc32=0;
	struct stat buf;

	myFile = fopen(file_name, "r+b");
	
	if (myFile ==0)
	{
		return(2);
	}
	else
	{
		stat(file_name, &buf);
		crc32 = getCRC32(myFile, buf.st_size);
	}

	writeCRCValue(myFile, crc32);
	
	fclose(myFile);
	return(0);
}


unsigned long CalcCRC32(void *ptr, long size)
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

unsigned long getCRC32(FILE *pFile, int length)
{
	char buf[16]; 	  
	unsigned long crc32=0;
	int i=0,n=0;

	// Calculate checksum
	fseek(pFile, 0, SEEK_SET);

	n = length/16;

	for (i=0; i<n; i++)
	{
		fread(buf, 1, 16, pFile);
		crc32=crc32^CalcCRC32(buf, 16);
	}
	// Checksum ok, import file...

	return (ntohl(crc32));
}


void writeCRCValue(FILE *pFile, unsigned long crc32)
{
	int nActual=0;
	unsigned long tmpCRC=0;
	TWX_FILE_HEADER exportHeader;
	
	fseek(pFile, 0, SEEK_SET);

	nActual = fread(&exportHeader, sizeof( TWX_FILE_HEADER), 1, pFile); 

	tmpCRC = htonl(crc32);
	exportHeader.crc32=tmpCRC;
		
	fseek(pFile, 0, SEEK_SET);
	fwrite(&exportHeader, sizeof( TWX_FILE_HEADER ), 1, pFile);
	
}