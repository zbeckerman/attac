#if !defined(__XML_H__)
#define __XML_H__

#include "3PP/expat/expat.h"

void XMLCALL startElement(void *userData, const XML_Char *name, const XML_Char **atts);

void XMLCALL endElement(void *userData, const XML_Char *name);

/* Lexicographically comparing UTF-8 encoded attribute values,
is equivalent to lexicographically comparing based on the character number. */

int attcmp(const void *att1, const void *att2);


void XMLCALL characterData(void *userData, const XML_Char *s, int len);


CTWServer* parseXML(char *xmlData, int len);

int hasCharacters (char buffer[]);
char * createOrAppendString (char ** destination, char *newValue);

#endif 
