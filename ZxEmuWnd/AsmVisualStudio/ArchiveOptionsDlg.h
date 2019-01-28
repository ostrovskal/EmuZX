
#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CArchiveOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CArchiveOptionsDlg)
public:
	CArchiveOptionsDlg(CWnd* pParent = NULL) : CDialog(CArchiveOptionsDlg::IDD, pParent), cur_item(-1) {}
	virtual ~CArchiveOptionsDlg() {}
	enum { IDD = IDD_DIALOG_ARCHIVE_OPTIONS};
protected:
	CString progID, name;
	size_t cur_item;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnEnUpdateEdit();
	afx_msg void OnLvnItemchangingListModules(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	CEdit m_module_name;
	CEdit m_module_prog_id;
	CButton m_add;
	CButton m_remove;
	CButton m_backup;
	CListCtrl m_list_modules;
public:
	afx_msg void OnBnClickedCheckAvtoBackup();
};
