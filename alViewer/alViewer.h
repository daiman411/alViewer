
// alViewer.h : main header file for the alViewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CalViewerApp:
// See alViewer.cpp for the implementation of this class
//

class CalViewerApp : public CWinAppEx
{
public:
	CalViewerApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	BOOL LoadSetting();
	BOOL RestoreSetting();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CalViewerApp theApp;
