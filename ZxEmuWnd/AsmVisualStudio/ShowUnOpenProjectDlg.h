
#pragma once

#include "resource.h"
#include "afxwin.h"

class CShowUnOpenProjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowUnOpenProjectDlg)
public:
	CShowUnOpenProjectDlg(const char* titl, const char* nm_list, const char* nm_ok, CStringArray* arr, CWnd* pParent = NULL) : CDialog(CShowUnOpenProjectDlg::IDD, pParent), title(titl), name_list(nm_list), name_ok(nm_ok), arr_prj(arr) {}
	virtual ~CShowUnOpenProjectDlg() {}
	enum {IDD = IDD_DIALOG_SHOW_UN_OPEN_PROJECT};
	virtual BOOL OnInitDialog();
protected:
	CString name_ok, name_list, title;
	CListBox m_list;
	CStatic m_slist;
	CButton m_ok;
	CStringArray* arr_prj;
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};
