
#pragma once

#include "AsmVisualStudio.h"
#include "Project.h"
#include "AsmGridCtrl.h"

class CProjectOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CProjectOptionsDlg)
public:
	CProjectOptionsDlg(CWnd* pParent = NULL) : CDialog(CProjectOptionsDlg::IDD, pParent), sln(theApp.frm->GetSolution()), opt(NULL), cfg(NULL), plt(NULL) {}
	virtual ~CProjectOptionsDlg() {}
	enum {IDD = IDD_DIALOG_PROJECT_OPTIONS};
	afx_msg void OnCbnSelchangeComboPlatform();
	afx_msg void OnCbnSelchangeComboConfig();
	afx_msg void OnCbnSelchangeComboTransLink();
	afx_msg void OnBnClickedButtonAddConfig();
	afx_msg void OnBnClickedButtonDelConfig();
	afx_msg void OnBnClickedButtonOrder();
	afx_msg void OnBnClickedButtonExclude();
	afx_msg LRESULT OnGridChangeProperty(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
protected:
	CString m_str_configs;
	Array<PROPERTY*>* opt;
	CPlatform* plt;
	CConfig* cfg;
	CSolution* sln;
	virtual void DoDataExchange(CDataExchange* pDX);
	CComboBox m_config;
	CComboBox m_platform;
	CButton m_add_cfg;
	CButton m_del_cfg;
	CButton m_order;
	CButton m_exclude;
	CEdit m_params;
	CComboBox m_combo_sel;
	CAsmGridCtrl m_opt;
	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
};
