
#pragma once

class CAddNewConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddNewConfigDlg)
public:
	CAddNewConfigDlg(const CString& copy_names, CWnd* pParent = NULL);
	virtual ~CAddNewConfigDlg() {}
	enum {IDD = IDD_DIALOG_ADD_CONFIG};
	afx_msg void OnCbnSelchangeComboSelect();
	afx_msg void OnEnUpdateEditNewName();
	virtual BOOL OnInitDialog();
	CString name;
	CString name_copy;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	CEdit m_name;
	CButton m_ok;
	CComboBox m_configs;
	Array<String> arr_copy;
	DECLARE_MESSAGE_MAP()
};
