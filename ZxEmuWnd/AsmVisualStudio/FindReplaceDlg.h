
#pragma once

#include "afxwin.h"
#include "resource.h"
#include "OgxTextEditor.h"

class CFindReplaceDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindReplaceDlg)
public:
	enum Styles
	{
		STYLE_FIND,
		STYLE_REPLACE,
		STYLE_FIND_IN_FILES,
		STYLE_REPLACE_IN_FILES
	};
	enum ActionTypes
	{
		ACT_BOOKMARK,
		ACT_BOOKMARK_ALL,
		ACT_FIND,
		ACT_FIND_ALL,
		ACT_REPLACE,
		ACT_REPLACE_ALL
	};
	enum BarTypes
	{
		BAR_UNDEF = -1,
		BAR_SELECTION,
		BAR_CURRENT_DOCUMENT,
		BAR_ALL_OPENED_DOCUMENTS,
		BAR_CURRENT_PROJECT,
		BAR_SOLUTION
	};
	CFindReplaceDlg(CWnd* pParent = NULL) : CDialog(CFindReplaceDlg::IDD, pParent), cur_bar(BAR_UNDEF), index_prj(0), index_elem(0) {}
	virtual ~CFindReplaceDlg() {}
	enum {IDD = IDD_DIALOG_FIND_REPLACE};
	afx_msg void OnBnClickedButtonFindNext();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonBookmark();
	afx_msg void OnBnClickedButtonReplaceAll();
	afx_msg void OnBnClickedButtonBookmarkAll();
	afx_msg void OnBnClickedButtonFindAll();
	afx_msg void OnBnClickedButtonReplace();
	afx_msg void OnCbnEditupdateComboFind();
	afx_msg void OnCbnSelchangeBar();
	afx_msg void OnCbnSelChangeComboFind();
	void Show(Styles type, const CString& find_text, long is_sel);
	void Initialize(const CString& finds, const CString& repls, long is_case, long is_hide, long is_word);
	void Uninitialize(CString& finds, CString& repls, long& is_case, long& is_hide, long& is_word);
	void OnAction(int type, int nBar);
	CString str_find;
protected:
	bool is_result;
	int cur_bar;
	size_t index_prj;
	size_t index_elem;
	void UpdateAll();
	bool FindReplaceToAction(int type, CDocumentCommon* doc, CElement* elem, long flags, bool is_begin);
	void SetTitlePanelFind(int nAction, const CString& fnd, const CString& rpl);
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
	CString str_replace;
	Styles style;
	CComboBox m_replace_text;
	CComboBox m_bar_find_replace;
	CComboBox m_find_text;
	CButton m_check_case;
	CButton m_check_word;
	CButton m_check_hide;
	CStatic m_replace_static;
	CStatic m_static_bar;
	CButton m_bookmark;
	CButton m_bookmark_all;
	CButton m_replace;
	CButton m_replace_all;
	CButton m_find_next;
	CButton m_find_all;
	virtual void OnOK();
	virtual void OnCancel();
};
