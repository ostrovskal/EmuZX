
#pragma once

#include "afxwin.h"
#include "Configuration.h"
#include "StaticSample.h"
#include "AsmGridCtrl.h"

class CBaseOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CBaseOptionsDlg)
public:
	CBaseOptionsDlg(CWnd* pParent = NULL);
	virtual ~CBaseOptionsDlg() {}
	enum {IDD = IDD_DIALOG_BASE_OPTIONS};
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboWindow();
	afx_msg void OnCbnSelchangeComboFont();
	afx_msg void OnCbnSelchangeComboSizeFont();
	afx_msg void OnLbnSelchangeListElems();
	afx_msg void OnEnUpdateEditNewKeyword();
	afx_msg void OnBnClickedButtonAddKeyword();
	afx_msg void OnBnClickedButtonDelKeyword();
	afx_msg void OnLbnSelchangeListKeywords();
	afx_msg void OnEnUpdateEditDefFolder();
	afx_msg void OnEnUpdateEditDefFolderInc();
	afx_msg void OnEnUpdateEditDefFolderLib();
	afx_msg LRESULT OnGridChangeProperty(WPARAM wParam, LPARAM lParam);
protected:
	void SetOptElems(BYTE* grp, PROPERTY** dst_opt, int count, int i_grp);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	int cur_wnd;
	int cur_elem;
	long fontSizes[5];
	CString m_str_keyword;
	CString fontNames[5];
	PROPERTY** opt;
	CStringArray grp_keys[8];
	CStringArray* cur_grp_keys;
	PROPERTY* opt_elems_output[24];
	PROPERTY* opt_elems_binary[24];
	PROPERTY* opt_elems_find[24];
	PROPERTY* opt_elems_text[24];
	PROPERTY* opt_elems_editor[60];
	CEdit m_new_keyword;
	CButton m_add_keyword;
	CButton m_del_keyword;
	CListBox m_list_elems;
	CListBox m_list_keys;
	CComboBox m_combo_wnd;
	CComboBox m_combo_size;
	CAsmGridCtrl m_grid_pars_elem;
	CStaticSample m_sample;
	CMFCFontComboBox m_combo_font;
	CMFCEditBrowseCtrl m_def_folder;
	CMFCEditBrowseCtrl m_def_folder_inc;
	CMFCEditBrowseCtrl m_def_folder_lib;
	CConfiguration* cfg;
	DECLARE_MESSAGE_MAP()
};
