// List.h: interface for the CTWServer class.
//

#if !defined(AFX_LIST_H__66142D85_C322_428E_A76D_EB3E2028__INCLUDED_)
#define AFX_LIST_H__66142D85_C322_428E_A76D_EB3E2028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "List.h"
#include "TWGame.h"
#define MAX_TAG_DEPTH 20

class CTWServer
{
protected:

public:
	CTWServer();
	virtual ~CTWServer();


	//xml stuff
	int GetDepth();
	void SetDepth(int d);
	char* insertIntoDatabase ();
	char* updateInDatabase ();
	bool existsInDatabase ();
	bool insertOrUpdateData();
	void addGame();
	CTWGame* getLastGame();

	//xml parsing stuff
	char *m_tags[MAX_TAG_DEPTH];
	int m_depth;
	int m_emptyTag;


public:
	CList *m_twGames;

	char *twgsKey;
	char *twgsRegcode;
	char *tradewarsKey;
	char *tradewarsRegcode;
	char *tradewarsGoldCode;
	char *serverName;
	char *ipAddress;
	char *Domain;
	char *siteOp;
	char *version;
	char *website;
	char *ftp;
	char *email;
	char *icq;
	char *port;
	char *nodes;
	char *externTime;
	char *description;
	char *graphic;
	char *notes;
	char *connections;
	char *connectionStart;
	char *bbsFront;
	char *lastUpdatedWhen;
	char *lastUpdatedBy;


};

#endif // !defined(AFX_LIST_H__66142D85_C322_428E_A76D_EB38C9EE2028__INCLUDED_)
