// Expat.cpp: implementation of the CExpat class.
//

#include "Common.h"
#include "Expat.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CExpat::CExpat()
{
	m_depth=0;
	m_emptyTag=0;

	m_parser=XML_ParserCreate(NULL);
	XML_SetUserData(m_parser, this);
	XML_SetElementHandler(m_parser, &StartElement, &EndElement);
	XML_SetCharacterDataHandler(m_parser, &CharacterData);

	m_userData=NULL;
	m_pStartElementFunc=NULL;
	m_pEndElementFunc=NULL;
	m_pCharacterDataFunc=NULL;
}

CExpat::~CExpat()
{
	XML_ParserFree(m_parser);
	m_parser = NULL;
}

/**
 * Gets the handle for this Expat parser.
 *
 * @return The Expat handle
 */
long CExpat::GetHandle()
{
	return (long)this;
}

/**
 * Gets the Expat object from a given handle.
 *
 * @param The Expat handle
 * @return The Expat object
 */
CExpat *CExpat::GetObject(long hExpat)
{
	return (CExpat *)hExpat;
}

void CExpat::Init(void *userData,
				  EXPAT_StartElementFunc pStartElementFunc,
				  EXPAT_EndElementFunc pEndElementFunc,
				  EXPAT_CharacterDataFunc pCharacterDataFunc)
{
	m_userData = userData;
	m_pStartElementFunc=pStartElementFunc;
	m_pEndElementFunc=pEndElementFunc;
	m_pCharacterDataFunc=pCharacterDataFunc;
}

int CExpat::ProcessFile(const char *filename)
{
	/* passing NULL for filename means read intput from stdin */
	FILE *fd=NULL;
	char buffer[MAX_BUF_LEN];

	if (filename != NULL) {
		if ((fd = fopen(filename, "rb")) == NULL ) {
			sprintf(s_error_msg, "Could not open file '%s'", filename);
			return -1;
		}
	}

	for (;;) {
		int isFinal;
		int len;

		len = fread(buffer, 1, MAX_BUF_LEN, fd);
		if (ferror(fd)) {
			sprintf(s_error_msg, "File read error");
			return -1;
		}
		isFinal = feof(fd);

		if (ProcessData(buffer, len, isFinal) != 0) {
			return -1;
		}

		if (isFinal) break;
	}

	fclose (fd);

	return 0;
}

int CExpat::ProcessData(char *data, int len, int isFinal)
{
	/*
	{
		FILE *ff;
		ff = fopen ("c:\\ff.xml", "w");
		if (ff!= NULL) {
			fprintf (ff,"%s",data);
			fclose(ff);
		}
	}
*/
	if (XML_Parse(m_parser, data, len, isFinal) == XML_STATUS_ERROR) {
		sprintf(s_error_msg, "XML parse error at line %d: %s",
			XML_GetCurrentLineNumber(m_parser),
			XML_ErrorString(XML_GetErrorCode(m_parser)));
		MessageBox(NULL, s_error_msg, "XML", MB_YESNO);
		return -1;
	}

	return 0;
}

void XMLCALL CExpat::StartElement(void *userData, const XML_Char *name, const XML_Char **atts)
{

	CExpat *pExpat = (CExpat*)userData;

	if (pExpat->m_pStartElementFunc == NULL) return;

	if (pExpat->m_depth < MAX_TAG_DEPTH) {
		pExpat->m_tags[pExpat->m_depth]=(char *)strdup(name);

		if (pExpat->m_pStartElementFunc != NULL) {
			// Call user function
			pExpat->m_pStartElementFunc(pExpat->m_userData,
				pExpat->m_tags[pExpat->m_depth], atts);
		}

		pExpat->m_depth++;
	}

	//checking for empty tags
	pExpat->m_emptyTag=1;

}

void XMLCALL CExpat::EndElement(void *userData, const XML_Char *name)
{

	CExpat *pExpat = (CExpat*)userData;

	if (pExpat->m_pEndElementFunc == NULL) return;

	if (pExpat->m_depth > 0) {
		pExpat->m_depth--;

		if (pExpat->m_pEndElementFunc != NULL) {
			// Call user function
			pExpat->m_pEndElementFunc(pExpat->m_userData,
				pExpat->m_tags[pExpat->m_depth]);
		}

		//it is only 1 if the node has no data. so stay at the same level
		//dont free the memory and then character call will free it.
		if (pExpat->m_emptyTag==1) {
			pExpat->m_emptyTag=2;
			pExpat->m_depth++;
		}
		else {
			pExpat->m_emptyTag=0;

			pfree((void **)&pExpat->m_tags[pExpat->m_depth]);
			pExpat->m_tags[pExpat->m_depth]=NULL;
		}
	}

}

void XMLCALL CExpat::CharacterData(void *userData, const XML_Char *data, int len)
{
	CExpat *pExpat = (CExpat*)userData;
	char buffer[100];
	int i;

	if (len==0 || pExpat->m_pCharacterDataFunc == NULL) 	return;


	// We don't want these characters
	for (i=0; len > 0; len--, data++) {
		switch (*data) {
		case '<':
			break;
		case '>':
			break;
		case 13:
			break;
		case 9:
		case 10:
			break;
		default:
			buffer[i++]=*data;
			break;
		}
		if (i == 100) break;
	}
	// null out the string
	buffer[i]=0;
//
//	if (pExpat->m_pCharacterDataFunc != NULL) {
//		// Call user function
//		pExpat->m_pCharacterDataFunc(pExpat->m_userData, pExpat->m_tags[pExpat->m_depth-1], buffer);
//	}
//
	//when there was no data. free the memory and decrement the count
	if (pExpat->m_emptyTag==2) {
		pExpat->m_depth--;

		pfree((void **)&pExpat->m_tags[pExpat->m_depth]);
		pExpat->m_tags[pExpat->m_depth]=NULL;
	}

	//found data
	pExpat->m_emptyTag=0;
	
}
