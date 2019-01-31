
#pragma once

class CModuleAddProcedureDlg : public CDialog
{
	DECLARE_DYNAMIC(CModuleAddProcedureDlg)
public:
	CModuleAddProcedureDlg(CWnd* pParent = NULL) : CDialog(CModuleAddProcedureDlg::IDD, pParent), cur_sel(-1) {}
	virtual ~CModuleAddProcedureDlg() {}
	enum {IDD = IDD_DIALOG_ADD_MODULE_PROCEDURE};
	virtual BOOL OnInitDialog();
	CString text;
protected:
	CString name_field, name;
	int cur_sel;
	CEdit m_name;
	CEdit m_uses;
	CEdit m_name_field;
	CEdit m_comment;
	CButton m_add;
	CButton m_del;
	CButton m_ok;
	CListBox m_fields;
	CComboBox m_type_field;
	CComboBox m_access;
	bool IsDuplicateParams();
	bool IsCorrectName(const CString& str);
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnLbnSelchangeListFields();
	afx_msg void OnEnUpdateEditName();
	afx_msg void OnEnUpdateEditNameField();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
