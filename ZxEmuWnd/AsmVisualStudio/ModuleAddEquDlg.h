
#pragma once

class CModuleAddEquDlg : public CDialog
{
	DECLARE_DYNAMIC(CModuleAddEquDlg)
public:
	CModuleAddEquDlg(CWnd* pParent = NULL) : CDialog(CModuleAddEquDlg::IDD, pParent) {}
	virtual ~CModuleAddEquDlg() {}
	enum {IDD = IDD_DIALOG_ADD_MODULE_EQU};
	CString text;
protected:
	CString name, value;
	CEdit m_name;
	CEdit m_value;
	CEdit m_comment;
	CButton m_ok;
	afx_msg void OnEnUpdateEditName();
	afx_msg void OnEnUpdateEditValue();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
