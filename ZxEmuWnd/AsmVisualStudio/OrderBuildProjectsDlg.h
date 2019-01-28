
#pragma once

#include "resource.h"

class COrderBuildProjectsDlg : public CDialog
{
	DECLARE_DYNAMIC(COrderBuildProjectsDlg)
public:
	COrderBuildProjectsDlg(CWnd* pParent = NULL) : CDialog(COrderBuildProjectsDlg::IDD, pParent), sel(-1) {}
	virtual ~COrderBuildProjectsDlg() {}
	enum {IDD = IDD_DIALOG_ORDER_PROJECTS};
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnLbnSelchangeListProjects();
protected:
	int sel;
	CListBox m_list;
	CListBox lsy;
	CButton m_up;
	CButton m_down;
	void MoveItem(int dx);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
