#include <string.h>

#include "stdafx.h"
#include "JumpgateServer.h"
#include "xml.h"

void XMLCALL startElement(void *userData, const XML_Char *name, const XML_Char **atts)
{
	CTWServer *twServer = (CTWServer*)userData;

	twServer->m_tags[twServer->GetDepth()]=(char *)strdup(name); 


	//we are getting a new game here!!
	if (stricmp(name, "GameData")==0) {
		//add it to the server
		twServer->addGame();
		twServer->getLastGame()->twgsKey = (char *) strdup (twServer->twgsKey);
	}

	//start attribute stuff here

	
	//end attribute stuff here


	//increment depth of tree
	twServer->SetDepth(twServer->GetDepth()+1);

	twServer->m_emptyTag=1;
}

void XMLCALL endElement(void *userData, const XML_Char *name)
{
	CTWServer *twServer = (CTWServer*)userData;



	if (twServer->GetDepth()>0)
		twServer->SetDepth(twServer->GetDepth()-1);

	pfree((void **)&twServer->m_tags[twServer->GetDepth()]);
	twServer->m_tags[twServer->GetDepth()]=NULL;
}

/* Lexicographically comparing UTF-8 encoded attribute values,
is equivalent to lexicographically comparing based on the character number. */

int attcmp(const void *att1, const void *att2)
{
  return strcmp(*(const XML_Char **)att1, *(const XML_Char **)att2);
}


void XMLCALL characterData(void *userData, const XML_Char *s, int len)
{
	int i=0;
	long tmpLong=0;
	int tmpInt=0, tmpInt2=0;
	int offset=1;
	char staticData[256];
	CTWServer *twServer = (CTWServer*)userData;


	for (; len > 0; --len, ++s) {
		switch (*s) {
		//	case '&':
		//	      fputs("&amp;", fp);
		//	  break;
			case '<':
			//      fputs(T("&lt;"), fp);
			  break;
			case '>':
			//      fputs(T("&gt;"), fp);
			  break;
			#ifdef W3C14N
			case 13:
			#ifdef USE_FAKE_LF
			//      fputs(T("&#xD;"), fp);
			#else
			//		fputs(T("\n"), fp);
			#endif

			  break;
			#else
			//    case T('"'):
			//    fputs(T("&quot;"), fp);
			  break;
			case 9:
			case 10:
			case 13:
			#ifdef USE_FAKE_LF
			//      fprintf(fp, T("&#%d;"), *s);
			#else
			//		fputs(T("\n"), fp);
			#endif
			  break;
			#endif
			default:
			  staticData[i++] = *s;
			  break;
		}
	}
	staticData[i++]=0;

	int hasChars = hasCharacters(staticData);
	char *data = NULL;
	if (strlen (staticData)>0) {
		char *tmp = (char *) strdup (staticData);
		data = (char *) unescapeData(tmp);
	}

 	if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Version") == 0) {

		twServer->version = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Title") == 0) {

		twServer->serverName = createOrAppendString(&twServer->serverName, data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Key") == 0) {

		twServer->twgsKey = createOrAppendString(&twServer->twgsKey, data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Code") == 0) {

		twServer->twgsRegcode = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TWKey") == 0) {

		twServer->tradewarsKey = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TWCode") == 0) {

		twServer->tradewarsRegcode = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "GoldCode") == 0) {

		twServer->tradewarsGoldCode = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "SiteOp") == 0) {

		twServer->siteOp = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Address") == 0) {

		twServer->ipAddress = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Port") == 0) {

		twServer->port = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Nodes") == 0) {

		twServer->nodes = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ExternTime") == 0) {

		twServer->externTime = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Version") == 0) {

		twServer->version = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Connections") == 0) {

		twServer->connections = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ConnectionsStart") == 0) {

		twServer->connectionStart = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "BBSFront") == 0) {

		twServer->bbsFront = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "URL") == 0) {

		twServer->website = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Email") == 0) {

		twServer->email = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "FTP") == 0) {

		twServer->ftp = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ICQ") == 0) {

		twServer->icq = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Description") == 0) {

		twServer->description = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-3], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Graphic") == 0) {

		twServer->graphic = (char *) strdup (data);
	} else if (twServer->GetDepth()>=4 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Notes") == 0) {

		twServer->notes = (char *) strdup (data);
///game data
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Letter") == 0) {

		twServer->getLastGame()->Game = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Letter") == 0) {

		twServer->getLastGame()->Game = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Active") == 0) {

		twServer->getLastGame()->GameActive = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Title") == 0) {

		twServer->getLastGame()->Title = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "GameOp") == 0) {

		twServer->getLastGame()->GameOp = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Email") == 0) {

		twServer->getLastGame()->Email = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ICQ") == 0) {

		twServer->getLastGame()->ICQ = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "NextBang") == 0) {

		twServer->getLastGame()->NextBang = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Notes") == 0) {

		twServer->getLastGame()->Notes = (char *) strdup (data);
//	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
//			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
//			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
//			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MajorVersion") == 0) {
//
//		twServer->getLastGame()->MajorVersion = (char *) strdup (data);
//	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
//			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
//			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
//			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MinorVersion") == 0) {
//
//		twServer->getLastGame()->MinorVersion = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "GoldEnabled") == 0) {

		twServer->getLastGame()->GoldEnabled = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MBBSCompatibility") == 0) {

		twServer->getLastGame()->MBBSCompatibility = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "StartDay") == 0) {

		twServer->getLastGame()->StartDay = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "RandomFactor") == 0) {

		twServer->getLastGame()->RandomFactor = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "DefinedStartDay") == 0) {

		twServer->getLastGame()->DefinedStartDay = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "InternalAliens") == 0) {

		twServer->getLastGame()->InternalAliens = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "InternalFerrengi") == 0) {

		twServer->getLastGame()->InternalFerrengi = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ClosedGame") == 0) {

		twServer->getLastGame()->ClosedGame = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PasswordGame") == 0) {

		twServer->getLastGame()->PasswordGame = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PasswordEditor") == 0) {

		twServer->getLastGame()->PasswordEditor = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ShowStardock") == 0) {

		twServer->getLastGame()->ShowStardock = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ShipDelayMode") == 0) {

		twServer->getLastGame()->ShipDelayMode = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "EtherDelayMode") == 0) {

		twServer->getLastGame()->EtherDelayMode = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MaxCommands") == 0) {

		twServer->getLastGame()->MaxCommands = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TurnBase") == 0) {

		twServer->getLastGame()->TurnBase = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TimeOnline") == 0) {

		twServer->getLastGame()->TimeOnline = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "InactiveTime") == 0) {

		twServer->getLastGame()->InactiveTime = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "LastBustClearDay") == 0) {

		twServer->getLastGame()->LastBustClearDay = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "InitialFighters") == 0) {

		twServer->getLastGame()->InitialFighters = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "InitialCredits") == 0) {

		twServer->getLastGame()->InitialCredits = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "InitialHolds") == 0) {

		twServer->getLastGame()->InitialHolds = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "NewPlayerPlanets") == 0) {

		twServer->getLastGame()->NewPlayerPlanets = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "DaysTilDeletion") == 0) {

		twServer->getLastGame()->DaysTilDeletion = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ColonistRegenRate") == 0) {

		twServer->getLastGame()->ColonistRegenRate = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MaxPlanetSector") == 0) {

		twServer->getLastGame()->MaxPlanetSector = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MaxCorpMembers") == 0) {

		twServer->getLastGame()->MaxCorpMembers = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "FedSpaceShipLimit") == 0) {

		twServer->getLastGame()->FedSpaceShipLimit = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PhotonMissileDuration") == 0) {

		twServer->getLastGame()->PhotonMissileDuration = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "CloakFailPercent") == 0) {

		twServer->getLastGame()->CloakFailPercent = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "DebrisLossPercent") == 0) {

		twServer->getLastGame()->DebrisLossPercent = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TradePercent") == 0) {

		twServer->getLastGame()->TradePercent = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "StealBuy") == 0) {

		twServer->getLastGame()->StealBuy = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ProductionRate") == 0) {

		twServer->getLastGame()->ProductionRate = (char *) strdup (data);

	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MaxProductionRegen") == 0) {

		twServer->getLastGame()->MaxProductionRegen = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MultiplePhotons") == 0) {

		twServer->getLastGame()->MultiplePhotons = (char *) strdup (data);

	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ClearBustDays") == 0) {

		twServer->getLastGame()->ClearBustDays = (char *) strdup (data);

	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "StealFactor") == 0) {

		twServer->getLastGame()->StealFactor = (char *) strdup (data);

	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "RobFactor") == 0) {

		twServer->getLastGame()->RobFactor = (char *) strdup (data);

	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "RobDelay") == 0) {

		twServer->getLastGame()->RobDelay = (char *) strdup (data);

	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PortProductionMax") == 0) {

		twServer->getLastGame()->PortProductionMax = (char *) strdup (data);

	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "RadiationLifetime") == 0) {

		twServer->getLastGame()->RadiationLifetime = (char *) strdup (data);

	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "FighterLockDecay") == 0) {

		twServer->getLastGame()->FighterLockDecay = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TavernAnnouncement") == 0) {

		twServer->getLastGame()->TavernAnnouncement = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "LimpetRemoval") == 0) {

		twServer->getLastGame()->LimpetRemoval = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ReregisterShip") == 0) {

		twServer->getLastGame()->ReregisterShip = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "CitadelTransportUnit") == 0) {

		twServer->getLastGame()->CitadelTransportUnit = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "CitadelTransportUpgrade") == 0) {

		twServer->getLastGame()->CitadelTransportUpgrade = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "GenesisTorpedo") == 0) {

		twServer->getLastGame()->GenesisTorpedo = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ArmidMine") == 0) {

		twServer->getLastGame()->ArmidMine = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "LimpetMine") == 0) {

		twServer->getLastGame()->LimpetMine = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Beacon") == 0) {

		twServer->getLastGame()->Beacon = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TypeITWarp") == 0) {

		twServer->getLastGame()->TypeITWarp = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TypeIITWarp") == 0) {

		twServer->getLastGame()->TypeIITWarp = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TWarpUpgrade") == 0) {

		twServer->getLastGame()->TWarpUpgrade = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PsychicProbe") == 0) {

		twServer->getLastGame()->PsychicProbe = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PlanetScanner") == 0) {

		twServer->getLastGame()->PlanetScanner = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "AtomicDetonator") == 0) {

		twServer->getLastGame()->AtomicDetonator = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Corbomite") == 0) {

		twServer->getLastGame()->Corbomite = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "EtherProbe") == 0) {

		twServer->getLastGame()->EtherProbe = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PhotonMissile") == 0) {

		twServer->getLastGame()->PhotonMissile = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "CloakingDevice") == 0) {

		twServer->getLastGame()->CloakingDevice = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MineDisruptor") == 0) {

		twServer->getLastGame()->MineDisruptor = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "HolographicScanner") == 0) {

		twServer->getLastGame()->HolographicScanner = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "DensityScanner") == 0) {

		twServer->getLastGame()->DensityScanner = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Sectors") == 0) {

		twServer->getLastGame()->Sectors = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Users") == 0) {

		twServer->getLastGame()->Users = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Ships") == 0) {

		twServer->getLastGame()->Ships = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Ports") == 0) {

		twServer->getLastGame()->Ports = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "Planets") == 0) {

		twServer->getLastGame()->Planets = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "FerrengiHomeBase") == 0) {

		twServer->getLastGame()->FerrengiHomeBase = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "StarDockSector") == 0) {

		twServer->getLastGame()->Stardock = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "SecondPortZeroLocation") == 0) {

		twServer->getLastGame()->SecondPortZeroLocation = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ThirdPortZeroLocation") == 0) {

		twServer->getLastGame()->ThirdPortZeroLocation = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MaxCourseLength") == 0) {

		twServer->getLastGame()->MaxCourseLength = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "TournamentMode") == 0) {

		twServer->getLastGame()->TournamentMode = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "DaysToEnter") == 0) {

		twServer->getLastGame()->DaysToEnter = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "MaxTimesBlownUp") == 0) {

		twServer->getLastGame()->MaxTimesBlownUp = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ActivePlayers") == 0) {

		twServer->getLastGame()->MaxTimesBlownUp = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PercentPlayersGood") == 0) {

		twServer->getLastGame()->PercentPlayersGood = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ActivePorts") == 0) {

		twServer->getLastGame()->ActivePorts = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PortValue") == 0) {

		twServer->getLastGame()->PortValue = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ActivePlanets") == 0) {

		twServer->getLastGame()->ActivePlanets = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "PercentPlanetCitadels") == 0) {

		twServer->getLastGame()->PercentPlanetCitadels = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ActiveShips") == 0) {

		twServer->getLastGame()->ActiveShips = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ActiveCorps") == 0) {

		twServer->getLastGame()->ActiveCorps = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ActiveFigs") == 0) {

		twServer->getLastGame()->ActiveFigs = (char *) strdup (data);
	} else if (twServer->GetDepth()>=3 && stricmp(twServer->m_tags[twServer->GetDepth()-4], "JumpGatePost") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-3], "Server") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-2], "GameData") == 0 &&
			 stricmp(twServer->m_tags[twServer->GetDepth()-1], "ActiveMines") == 0) {

		twServer->getLastGame()->ActiveMines = (char *) strdup (data);
	}


	//found data
	twServer->m_emptyTag=0;
	if (data != NULL)
		pfree( (void**) &data);

}

char * createOrAppendString (char ** destination, char *newValue) {
	char *returnValue=NULL;
	if (*destination == NULL) {
		returnValue = (char *) strdup (newValue);
	} else {
		int len = strlen (*destination) + strlen (newValue) +1;
		returnValue = (char *) malloc (len);
		strcpy (returnValue, *destination);
		strcat (returnValue, newValue);
		returnValue[len-1]=0;
		free (*destination);
	}
	return returnValue;
}


int hasCharacters (char buffer[]) {
	int i=0;

	for (i=0; i<(int) strlen (buffer); i++)
		if (buffer[i] != ' ')
			return 1;

	return 0;
}
