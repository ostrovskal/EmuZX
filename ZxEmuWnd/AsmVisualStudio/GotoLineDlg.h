
#pragma once

#include "afxwin.h"
#include "resource.h"

class CGotoLineDlg : public CDialog
{
	DECLARE_DYNAMIC(CGotoLineDlg)
public:
	enum {IDD = IDD_DIALOG_GOTO_LINE};
	CGotoLineDlg(CWnd* pParent = NULL) : CDialog(CGotoLineDlg::IDD, pParent) {}
	virtual ~CGotoLineDlg() {}
	afx_msg void OnEnUpdateEditLine();
	long line;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	CString str_num;
	CButton m_ok;
	CEdit m_lines;
	DECLARE_MESSAGE_MAP()
};
