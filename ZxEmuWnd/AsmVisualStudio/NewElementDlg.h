
#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CNewElementDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewElementDlg)
public:
	enum Types
	{
		TYPE_FILE_SOURCE	= 0,
		TYPE_FILE_HEADER	= 1,
		TYPE_FILE_TEXT		= 2,
		TYPE_PROJECT_EXE32	= 0,
		TYPE_PROJECT_DLL32	= 1,
		TYPE_PROJECT_EXE64	= 2,
		TYPE_PROJECT_DLL64	= 3
	};
	CNewElementDlg(int style, CWnd* pParent = NULL) : CDialog(CNewElementDlg::IDD, pParent), action(style), elems(NULL) {}
	virtual ~CNewElementDlg() {}
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
	char** elems;
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
