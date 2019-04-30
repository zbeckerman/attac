// MouseSel.h : main header file for the MouseSel application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CMouseSelApp:
// See MouseSel.cpp for the implementation of this class
//

class CMouseSelApp : public CWinApp
{
public:
	CMouseSelApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMouseSelApp theApp;