// List.cpp: implementation of the CTWServer class.
//

#include <time.h>
#include "stdafx.h"
#include "JumpgateServer.h"
#include "TWServer.h"
#include "TWGame.h"
#include "ADO.h"
#include "commctrl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CTWServer::CTWServer()
{
	time_t myTime;
	myTime = time (NULL);

	//these are allocated in a new thread so do it here
	::CoInitialize(NULL);

	twgsKey=NULL;
	twgsRegcode=NULL;
	tradewarsKey=NULL;
	tradewarsRegcode=NULL;
	tradewarsGoldCode=NULL;
	serverName=NULL;
	ipAddress=NULL;
	Domain=NULL;
	siteOp=NULL;
	version=NULL;
	website=NULL;
	ftp=NULL;
	email=NULL;
	icq=NULL;
	port=NULL;
	nodes=NULL;
	externTime=NULL;
	description=NULL;
	graphic=NULL;
	notes=NULL;
	connections=NULL;
	connectionStart=NULL;
	bbsFront=NULL;
	lastUpdatedWhen= (char *)strdup (ctime(&myTime));
	lastUpdatedBy=(char *) strdup ("Jumpgate");

	m_depth=0;
	m_emptyTag=0;

	m_twGames = new CList();
}

CTWServer::~CTWServer()
{

	pfree((void**)&twgsKey);
	pfree((void**)&twgsRegcode);
	pfree((void**)&tradewarsKey);
	pfree((void**)&tradewarsRegcode);
	pfree((void**)&tradewarsGoldCode);
	pfree((void**)&serverName);
	pfree((void**)&ipAddress);
	pfree((void**)&Domain);
	pfree((void**)&siteOp);
	pfree((void**)&version);
	pfree((void**)&website);
	pfree((void**)&ftp);
	pfree((void**)&email);
	pfree((void**)&icq);
	pfree((void**)&port);
	pfree((void**)&nodes);
	pfree((void**)&externTime);
	pfree((void**)&description);
	pfree((void**)&graphic);
	pfree((void**)&notes);
	pfree((void**)&connections);
	pfree((void**)&connectionStart);
	pfree((void**)&bbsFront);
	pfree((void**)&lastUpdatedWhen);
	pfree((void**)&lastUpdatedBy);

	//delete all our games
	for (int i=0; i<m_twGames->GetCount(); i++) {
		CTWGame *twGame = (CTWGame*) m_twGames->GetAt(i);
		delete twGame;
	}

	//now delete the list
	delete m_twGames;

	//these are allocated in a new thread so do it here
	::CoUninitialize();

}

void CTWServer::addGame() {
	CTWGame *twGame = new CTWGame();
	m_twGames->Add(twGame);
}

CTWGame* CTWServer::getLastGame() {
	if (m_twGames->GetCount()==0) {
		return NULL;
	} else {
		return (CTWGame*) m_twGames->GetAt(m_twGames->GetCount()-1);
	}
}

int CTWServer::GetDepth()
{
	return m_depth;
}

void CTWServer::SetDepth(int d)
{
	m_depth = d;
}

char* CTWServer::insertIntoDatabase() {

	char buffer[MAX_LENGTH];
	char sqlBuffer[MAX_LENGTH];
	bool returnValue = false;
	char *tempValue=NULL;
	char *replaceSQL=NULL;

	if (this->twgsKey==NULL)
		return false;


	FILE *fp;

	fp = fopen ("c:\\ado.txt","r");
	if (fp != NULL) {
		fgets (buffer, MAX_LENGTH, fp);
		fclose (fp);
	}
	CADO *cAdo = new CADO(buffer);
	cAdo->Connect("","");

	if (cAdo->isConnected()) {
		sprintf (sqlBuffer, "insert into JUMPGATE_SERVERS ( [twgsKey], [twgsRegcode], [ServerName], [IP], [Domain], [SiteOp], [Version], [Website], [FTP], [Email], [ICQ], [Port], [Nodes], [ExternTime], [Description], [Graphic], [Notes], [LastUpdatedWhen], [LastUpdatedBy]) values (");

		//key
		tempValue = fixSQLValue (this->twgsKey);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//regcode
		tempValue = fixSQLValue (this->twgsRegcode);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//servername
		tempValue = fixSQLValue (this->serverName);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//ipAddress
		tempValue = fixSQLValue (this->ipAddress);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//domain
		tempValue = fixSQLValue (this->Domain);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//siteop
		tempValue = fixSQLValue (this->siteOp);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//version
		tempValue = fixSQLValue (this->version);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//website
		tempValue = fixSQLValue (this->website);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//ftp
		tempValue = fixSQLValue (this->ftp);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//email
		tempValue = fixSQLValue (this->email);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//ICQ
		tempValue = fixSQLValue (this->icq);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//Port
		tempValue = fixSQLValue (this->port);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//Nodes
		tempValue = fixSQLValue (this->nodes);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//ExternTime
		tempValue = fixSQLValue (this->externTime);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//Description
		tempValue = fixSQLValue (this->description);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//Graphic
		tempValue = fixSQLValue (this->graphic);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//Notes
		tempValue = fixSQLValue (this->notes);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//LastUpdatedWhen
		tempValue = fixSQLValue (this->lastUpdatedWhen);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ",");

		//LastUpdatedBy
		tempValue = fixSQLValue (this->lastUpdatedBy);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		//end parentesis
		strcat (sqlBuffer, ");");


		FILE *fp1;

		fp1 = fopen ("c:\\abc.txt","w");
		if (fp1 != NULL) {
			fprintf (fp1, "%s", sqlBuffer);
			fclose (fp1);
		}


		if (cAdo->Execute(sqlBuffer)==0) {
			char *tmp = cAdo->GetErrorMessage();
			MessageBox(NULL, tmp,"Jumpgate Server",MB_ICONERROR);
			pfree((void**)&tmp);
		}

	}

	cAdo->Disconnect();

	return NULL;
	
}

char* CTWServer::updateInDatabase () {

	char buffer[MAX_LENGTH];
	char sqlBuffer[MAX_LENGTH];
	bool returnValue = false;
	char *tempValue=NULL;
	char *replaceSQL=NULL;

	if (this->twgsKey==NULL)
		return false;


	FILE *fp;

	fp = fopen ("c:\\ado.txt","r");
	if (fp != NULL) {
		fgets (buffer, MAX_LENGTH, fp);
		fclose (fp);
	}
	CADO *cAdo = new CADO(buffer);
	cAdo->Connect("","");

	if (cAdo->isConnected()) {
		sprintf (sqlBuffer, "update JUMPGATE_SERVERS set ");

		strcat (sqlBuffer, " [twgsRegcode]=");

		//regcode
		tempValue = fixSQLValue (this->twgsRegcode);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ServerName]=");

		//servername
		tempValue = fixSQLValue (this->serverName);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [IP]=");

		//ipAddress
		tempValue = fixSQLValue (this->ipAddress);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Domain]=");

		//domain
		tempValue = fixSQLValue (this->Domain);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [SiteOp]=");

		//siteop
		tempValue = fixSQLValue (this->siteOp);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Version]=");

		//version
		tempValue = fixSQLValue (this->version);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Website]=");

		//website
		tempValue = fixSQLValue (this->website);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [FTP]=");

		//ftp
		tempValue = fixSQLValue (this->ftp);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Email]=");

		//email
		tempValue = fixSQLValue (this->email);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ICQ]=");

		//ICQ
		tempValue = fixSQLValue (this->icq);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Port]=");

		//Port
		tempValue = fixSQLValue (this->port);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Nodes]=");

		//Nodes
		tempValue = fixSQLValue (this->nodes);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [ExternTime]=");

		//ExternTime
		tempValue = fixSQLValue (this->externTime);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Description]=");

		//Description
		tempValue = fixSQLValue (this->description);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Graphic]=");

		//Graphic
		tempValue = fixSQLValue (this->graphic);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [Notes]=");

		//Notes
		tempValue = fixSQLValue (this->notes);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [LastUpdatedWhen]=");

		//LastUpdatedWhen
		tempValue = fixSQLValue (this->lastUpdatedWhen);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		strcat (sqlBuffer, ", [LastUpdatedBy]=");

		//LastUpdatedBy
		tempValue = fixSQLValue (this->lastUpdatedBy);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);

		//end parentesis
		strcat (sqlBuffer, " WHERE [twgsKey]=");

		//key
		tempValue = fixSQLValue (this->twgsKey);
		strcat (sqlBuffer, tempValue);
		pfree((void **) &tempValue);


		FILE *fp1;

		fp1 = fopen ("c:\\abc.txt","w");
		if (fp1 != NULL) {
			fprintf (fp1, "%s", sqlBuffer);
			fclose (fp1);
		}


		if (cAdo->Execute(sqlBuffer)==0) {
			char *tmp = cAdo->GetErrorMessage();
			MessageBox(NULL, tmp,"Jumpgate Server",MB_ICONERROR);
			pfree((void**)&tmp);
		}

	}

	cAdo->Disconnect();

	
	return NULL;
}

bool CTWServer::existsInDatabase () {
	char buffer[MAX_LENGTH];
	char sqlBuffer[MAX_LENGTH];
	bool returnValue = false;

	if (this->twgsKey==NULL)
		return false;


	FILE *fp;

	fp = fopen ("c:\\ado.txt","r");
	if (fp != NULL) {
		fgets (buffer, MAX_LENGTH, fp);
		fclose (fp);
	}
	CADO *cAdo = new CADO(buffer);
	cAdo->Connect("","");

	if (cAdo->isConnected()) {
		char *replaceSQL = fixSQLValue (this->twgsKey);
		sprintf (sqlBuffer, "select count(*) AS theCounter from JUMPGATE_SERVERS where twgsKey=%s", replaceSQL);
		pfree((void **) &replaceSQL);

		RecPtr recordPointer = cAdo->ExecuteWithReturn(sqlBuffer);
		if (recordPointer!=NULL) {
			long counter = recordPointer->Fields->GetItem("theCounter")->GetValue();
			if (counter>0) {
				returnValue=true;
			}
		}
	}
	
	cAdo->Disconnect();

	return returnValue;
}

bool CTWServer::insertOrUpdateData() {
	char sqlBuffer[MAX_LENGTH];
	char gamesToDelete[MAX_LENGTH];
	char buffer[MAX_LENGTH];
	int i=0;

	//update Server data
	if (existsInDatabase()==true) {
		updateInDatabase();
	} else {
		insertIntoDatabase();
	}
	
	//update gate data
	for (i=0; i<m_twGames->GetCount(); i++) {
		CTWGame *twGame = (CTWGame*) m_twGames->GetAt(i);
		twGame->insertOrUpdateData();
	}


	//delete game data
	strcpy(gamesToDelete, "");
	for (i=0; i<m_twGames->GetCount(); i++) {
		CTWGame *twGame = (CTWGame*) m_twGames->GetAt(i);
		if (i>0) {
			strcat(gamesToDelete, ",");
		}
		char *tmp = fixSQLValue(twGame->Game);
		strcat(gamesToDelete, tmp);
		pfree ((void **) &tmp);
	}

	FILE *fp;

	fp = fopen ("c:\\ado.txt","r");
	if (fp != NULL) {
		fgets (buffer, MAX_LENGTH, fp);
		fclose (fp);
	}

	if (strcmp (gamesToDelete,"") !=0) {
		CADO *cAdo = new CADO(buffer);
		cAdo->Connect("","");

		if (cAdo->isConnected()) {
			char *replaceSQLTWGS = fixSQLValue (this->twgsKey);
			sprintf (sqlBuffer, "delete from JUMPGATE_GAMES where twgsKey=%s and Game not in (%s)", replaceSQLTWGS, gamesToDelete);

			pfree((void **) &replaceSQLTWGS);


			FILE *fp1;

			fp1 = fopen ("c:\\def.txt","w");
			if (fp1 != NULL) {
				fprintf (fp1, "%s", sqlBuffer);
				fclose (fp1);
			}

			if (cAdo->Execute(sqlBuffer)==0) {
				char *tmp = cAdo->GetErrorMessage();
				MessageBox(NULL, tmp,"Jumpgate Server",MB_ICONERROR);
				pfree((void**)&tmp);
			}
			
		}

		cAdo->Disconnect();
	}	

	return true;
}
