
#pragma once
#include "afxwin.h"

class CArchiveNewDlg : public CDialog
{
	DECLARE_DYNAMIC(CArchiveNewDlg)
public:
	CArchiveNewDlg(CWnd* pParent = NULL) : CDialog(CArchiveNewDlg::IDD, pParent) {}
	virtual ~CArchiveNewDlg() {}
	enum {IDD = IDD_DIALOG_ARCHIVE_NEW};
	virtual BOOL OnInitDialog();
	CString folder_arch, folder_lst;
	CString name, signature;
protected:
	void OnUpdate();
	afx_msg void OnEnUpdateEditPath();
	afx_msg void OnEnUpdateEditPathLst();
	afx_msg void OnEnUpdateEditSignature();
	afx_msg void OnEnUpdateEditName();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
	CMFCEditBrowseCtrl m_path;
	CMFCEditBrowseCtrl m_path_lst;
	CEdit m_sign;
	CEdit m_name;
	CButton m_ok;
};
