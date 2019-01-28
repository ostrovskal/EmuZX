
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"
#include "MainFrm.h"
#include "SplashWnd.h"

class CAsmVisualStudioApp : public CWinAppEx
{
public:
	CAsmVisualStudioApp() : frm(NULL) {}
	CMainFrame* frm;
	CMultiDocTemplate* docTemplAsm;
	CMultiDocTemplate* docTemplTxt;
	CMultiDocTemplate* docTemplBin;
	CMultiDocTemplate* docTemplOgx;
	virtual BOOL InitInstance();
	virtual void PreLoadState();
	afx_msg void OnAppAbout();
	CMFCPropertyGridProperty* GetGridProperty(PROPERTY* o, DWORD_PTR dwData);
	virtual BOOL SaveAllModified();
	DECLARE_MESSAGE_MAP()
	CSplashWnd* splash;
};

extern CAsmVisualStudioApp theApp;
