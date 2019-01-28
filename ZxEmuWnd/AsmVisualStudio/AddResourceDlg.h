
#pragma once

#include "afxwin.h"

class CAddResourceDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddResourceDlg)
public:
	CAddResourceDlg(CWnd* pParent = NULL) : CDialog(CAddResourceDlg::IDD, pParent) {}
	virtual ~CAddResourceDlg() {}
	enum {IDD = IDD_DIALOG_ARCHIVE_OPEN_RESOURCE};
	CString name, path;
protected:
	afx_msg void OnEnUpdateEditName();
	afx_msg void OnCbnSelchangeComboType();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	CEdit m_name;
	CButton m_ok;
	CComboBox m_type;
	CMFCEditBrowseCtrl m_path;
};
