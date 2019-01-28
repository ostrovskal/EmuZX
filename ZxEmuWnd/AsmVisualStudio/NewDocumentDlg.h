
#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CNewDocumentDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewDocumentDlg)
public:
	CNewDocumentDlg(int style, CWnd* pParent = NULL) : CDialog(CNewDocumentDlg::IDD, pParent), action(style) {}
	virtual ~CNewDocumentDlg() {}
	enum {IDD = IDD_DIALOG_NEW_ELEMENT};
	virtual BOOL OnInitDialog();
	afx_msg void OnEnUpdateEditFolder();
	afx_msg void OnEnUpdateEditName();
	afx_msg void OnEnUpdateEditNameSolution();
	CString name, folder, ext;
	CString name_sln;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	int action;
	CEdit m_name;
	CEdit m_sln;
	CButton m_ok;
	CStatic m_s_sln;
	CStatic m_s_type;
	CStatic m_s_name;
	CStatic m_s_folder;
	CMFCListCtrl m_type;
	CImageList img_list;
	CMFCEditBrowseCtrl m_folder;
};
