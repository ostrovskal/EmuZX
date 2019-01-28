
#pragma once

class CModuleAddStructureDlg : public CDialog
{
	DECLARE_DYNAMIC(CModuleAddStructureDlg)
public:
	CModuleAddStructureDlg(CWnd* pParent = NULL) : CDialog(CModuleAddStructureDlg::IDD, pParent), cur_sel(-1) {}
	virtual ~CModuleAddStructureDlg() {}
	enum {IDD = IDD_DIALOG_ADD_MODULE_STRUCTURE};
	virtual BOOL OnInitDialog();
	CString text;
protected:
	int cur_sel;
	CString name, name_field;
	CEdit m_name;
	CEdit m_name_field;
	CEdit m_comment;
	CEdit m_value_field;
	CButton m_add;
	CButton m_del;
	CButton m_ok;
	CListBox m_fields;
	CComboBox m_type_field;
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
