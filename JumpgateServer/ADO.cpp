// List.cpp: implementation of the CADO class.
//

#include "stdafx.h"
#include "JumpgateServer.h"
#include "ADO.h"

//		Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\temp\portal.mdb

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CADO::CADO(char *adoString)
{
	m_adoString = (char*) strdup (adoString);
	m_bConnected=FALSE;
	m_Cnn=NULL;
}

CADO::~CADO()
{
}



char* CADO::getADOString()
{
	return m_adoString;
}


void CADO::Connect(char *userName, char *password)
{
	try
	{
		if(m_bConnected)	// If was connected previously;
		{
			m_Cnn->Close();	// Close the conncetion;
			m_bConnected = false;	// Indicate "Not Connected";
		}

		// Now trying to open ADO connection:
		HRESULT hr    = m_Cnn.CreateInstance( __uuidof( ADODB::Connection ) );
		m_bConnected = (m_Cnn->Open(m_adoString, userName, password, ADODB::adConnectUnspecified) == S_OK);
	}
	catch(_com_error error)
	{
		MessageBox(NULL, error.Description(), "Failed To Connect", MB_OK|MB_ICONERROR);
	}

}

void CADO::Disconnect()
{
	try
	{
		if(m_bConnected)	// If was connected previously;
		{
			m_Cnn->Close();	// Close the conncetion;
			m_bConnected = false;	// Indicate "Not Connected";
			m_Cnn=NULL;
		}

	}
	catch(_com_error error)
	{
		MessageBox(NULL, error.Description(), "Failed To Disconnect", MB_OK|MB_ICONERROR);
	}

}


bool CADO::Execute(char* cmdStr)
{
	try
	{
		m_Cnn->Execute(cmdStr,NULL,1);
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		return 0;
	}
	sprintf(m_ErrStr,"Success");
	return 1;
}

RecPtr CADO::ExecuteWithReturn(char* CmdStr)
{
	RecPtr t_Rec=NULL;
	try
	{
		t_Rec=m_Cnn->Execute(CmdStr,NULL,1);
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		return 0;
	}

	sprintf(m_ErrStr,"Success");
	return t_Rec;
}



char* CADO::GetErrorMessage() {
	return ((char *) strdup (m_ErrStr));
}

void CADO::ErrorHandler(_com_error &e, char* ErrStr)
{
  
	sprintf(ErrStr,"Error:\n");
	sprintf(ErrStr,"%sCode = %08lx\n",ErrStr ,e.Error());
	sprintf(ErrStr,"%sCode meaning = %s\n", ErrStr, (char*) e.ErrorMessage());
	sprintf(ErrStr,"%sSource = %s\n", ErrStr, (char*) e.Source());
	sprintf(ErrStr,"%sDescription = %s",ErrStr, (char*) e.Description());
}

bool CADO::isConnected () {
	return m_bConnected;
}