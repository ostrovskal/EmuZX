
#pragma once

#include "afxwin.h"
#include "afxcmn.h"

class CNewProjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewProjectDlg)
public:
	CNewProjectDlg(int style, CWnd* pParent = NULL) : CDialog(CNewProjectDlg::IDD, pParent), action(style), wzd(NULL) {}
	virtual ~CNewProjectDlg() {}
	enum {IDD = IDD_DIALOG_NEW_PROJECT};
	virtual BOOL OnInitDialog();
	afx_msg void OnEnUpdateEditFolder();
	afx_msg void OnEnUpdateEditName();
	afx_msg void OnEnUpdateEditNameSolution();
	WIZARD* wzd;
	CString name, folder;
	CString name_sln;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	int action;
	CEdit m_name;
	CEdit m_sln;
	CButton m_ok;
	CStatic m_s_folder;
	CStatic m_s_sln;
	CMFCListCtrl m_type;
	CImageList img_list;
	CMFCEditBrowseCtrl m_folder;
};
