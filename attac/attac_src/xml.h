#ifndef __xml_h__
#define __xml_h__


#define XML_MAP_FILE 01
#define XML_EXTERNAL_ENTITIES 02
#define BUFFSIZE        8192
#define RECURSIVE_TAGS 50
#define T(x) x
#define NSSEP T('\001')

// prevent winsock.h being included in windows.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "./3PP/expat/expat.h"

#define XML_OFFSET 0
#define TWXSYNC_XML_IMPORT_OFFSET 1
#define TWXSYNC_XML_OFFSET 1

extern long twxSectorNumber;
extern long lastUpdated;
extern long twxVersion;
extern char twxCreatedBy[MAX_LENGTH];
extern long twxCreatedOn;
extern char traderName[MAX_LENGTH];
extern char ships[MAX_LENGTH];
extern char trader[MAX_LENGTH];
extern char user[MAX_LENGTH];
extern char helper[MAX_LENGTH];
extern char someoneElseLoginId[MAX_LENGTH];
extern int corp;
extern long finished;
extern long hops;


int isValidSector (long sectorNum);
char* import_twxv2 (char * filename, int offset);
int export_twxv2 (char *filename, long startTime, long endTime, int exportToTWXFile);
void  parseXMLImportData(int tOffset, int tDepth, char *tXMLTreeTag[], char tBuffer[]);

void XMLCALL characterData(void *userData, const XML_Char *s, int len);
const XML_Char * resolveSystemId(const XML_Char *base, const XML_Char *systemId,
                XML_Char **toFree);


int
externalEntityRefFilemap(XML_Parser parser,
                         const XML_Char *context,
                         const XML_Char *base,
                         const XML_Char *systemId,
                         const XML_Char *publicId);

int
externalEntityRefStream(XML_Parser parser,
                        const XML_Char *context,
                        const XML_Char *base,
                        const XML_Char *systemId,
                        const XML_Char *publicId);
void reportError(XML_Parser parser, const XML_Char *filename);

void processFile(const void *data, size_t size, const XML_Char *filename, void *args);
#ifdef WIN32

int isAsciiLetter(XML_Char c);

#endif /* WIN32 */



int XML_ProcessFile(XML_Parser parser, const XML_Char *filename, unsigned flags);
int processStream(const XML_Char *filename, XML_Parser parser);
void XMLCALL startElement(void *userData, const XML_Char *name, const XML_Char **atts);
void XMLCALL endElement(void *userData, const XML_Char *name);
int attcmp(const void *att1, const void *att2);
XML_Char* attributeValue(FILE *fp, const XML_Char *s);
int hasCharacters (char buffer[]);
#endif


