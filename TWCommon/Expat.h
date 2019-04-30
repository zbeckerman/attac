// Expat.h: interface for the CExpat class.
//

#if !defined(AFX_EXPAT_H__AA8A2738_C4A8_49B0_BF1E_580937343B1F__INCLUDED_)
#define AFX_EXPAT_H__AA8A2738_C4A8_49B0_BF1E_580937343B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TWCommon.h"
#include "3PP/expat/expat.h"


typedef void (*EXPAT_StartElementFunc)(void *userData, const char *tag, const char **atts);
typedef void (*EXPAT_EndElementFunc)(void *userData, const char *tag);
typedef void (*EXPAT_CharacterDataFunc)(void *userData, const char *tag, const char *data);

class TWCOMMON_CLASS CExpat
{
public:
	CExpat();
	virtual ~CExpat();

	long GetHandle();
	static CExpat *GetObject(long hExpat);

	void Init(void *userData,
			  EXPAT_StartElementFunc pStartElementFunc,
			  EXPAT_EndElementFunc pEndElementFunc,
			  EXPAT_CharacterDataFunc pCharacterDataFunc);

	int ProcessFile(const char *filename);
	int ProcessData(char *data, int len, int isFinal);
	int m_emptyTag;
protected:
	static void XMLCALL StartElement(void *userData, const XML_Char *name, const XML_Char **atts);
	static void XMLCALL EndElement(void *userData, const XML_Char *name);
	static void XMLCALL CharacterData(void *userData, const XML_Char *data, int len);

private:
	XML_Parser m_parser;
	char *m_tags[MAX_TAG_DEPTH];
	int m_depth;
	//expat state

	void *m_userData;
	EXPAT_StartElementFunc m_pStartElementFunc;
	EXPAT_EndElementFunc m_pEndElementFunc;
	EXPAT_CharacterDataFunc m_pCharacterDataFunc;
};

#endif // !defined(AFX_EXPAT_H__AA8A2738_C4A8_49B0_BF1E_580937343B1F__INCLUDED_)
