
#pragma once

class CAddNewNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddNewNameDlg)
public:
	CAddNewNameDlg(const char* titl, const char* val, CWnd* pParent = NULL) : CDialog(CAddNewNameDlg::IDD, pParent), title(titl), value(val) {}
	virtual ~CAddNewNameDlg() {}
	enum {IDD = IDD_DIALOG_INPUT_NAME};
	afx_msg void OnEnUpdateEditName();
	virtual BOOL OnInitDialog();
	CString name;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	CString title, value;
	CEdit m_name;
	CButton m_ok;
	DECLARE_MESSAGE_MAP()
};
