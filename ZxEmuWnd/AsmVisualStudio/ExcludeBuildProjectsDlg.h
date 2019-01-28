
#pragma once

#include "resource.h"
#include "AsmVisualStudio.h"
#include "Project.h"

class CExcludeBuildProjectsDlg : public CDialog
{
	DECLARE_DYNAMIC(CExcludeBuildProjectsDlg)
public:
	CExcludeBuildProjectsDlg(CWnd* pParent = NULL) : CDialog(CExcludeBuildProjectsDlg::IDD, pParent), sln(theApp.frm->GetSolution()) {}
	virtual ~CExcludeBuildProjectsDlg() {}
	enum {IDD = IDD_DIALOG_EXCLUDE_BUILD};
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedListProjects(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CSolution* sln;
	CListCtrl m_list;
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};
