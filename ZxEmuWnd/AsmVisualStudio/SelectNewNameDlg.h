
#pragma once

class CSelectNewNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectNewNameDlg)
public:
	CSelectNewNameDlg(const CString& titl, const CString& e_names, const CString& n_names, CWnd* pParent = NULL) : CDialog(CSelectNewNameDlg::IDD, pParent), title(titl), exist_names(e_names), new_names(n_names) {}
	virtual ~CSelectNewNameDlg() {}
	enum {IDD = IDD_DIALOG_SELECT_NAME};
	afx_msg void OnCbnSelchangeComboSelect();
	virtual BOOL OnInitDialog();
	CString name;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	CString exist_names, new_names, title;
	CComboBox m_name;
	CButton m_ok;
	DECLARE_MESSAGE_MAP()
};
