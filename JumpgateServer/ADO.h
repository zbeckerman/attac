// ADO.h: interface for the CADO class.
//

#if !defined(AFX_LIST_H__66142D85_C3238C9EE2028__INCLUDED_)
#define AFX_LIST_H__66142D85_C3238C9EE2028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#import "c:\Program Files\Common Files\System\ADO\msado15.dll" \
              rename("EOF", "EndOfFile")

typedef ADODB::_RecordsetPtr	RecPtr;
typedef ADODB::_ConnectionPtr	CnnPtr; 

class CADO
{
protected:

public:
	CADO(char *adoString);
	virtual ~CADO();

	char* getADOString();
	void Connect(char *userName, char *password);
	void Disconnect();
	bool Execute(char* cmdStr);
	RecPtr ExecuteWithReturn(char* CmdStr);
	char* GetErrorMessage();
	bool isConnected ();

private:
	void ErrorHandler(_com_error &e, char* ErrStr);

	///////////////////
	// ADO properties;
	//
	CnnPtr m_Cnn;	// ADO Connection object;
	char *m_adoString;
	bool m_bConnected;					// Flag that indicates when connected;
	char m_ErrStr[500];
};

#endif // !defined(AFX_LIST_H__66142D85_C322_428E_A76D_EB38C9EE2028__INCLUDED_)
