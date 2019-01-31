
#pragma once

class CResourcePropertiesDlg : public CDialog
{
	DECLARE_DYNAMIC(CResourcePropertiesDlg)
public:
	CResourcePropertiesDlg(CWnd* pParent = NULL) : CDialog(CResourcePropertiesDlg::IDD, pParent) {}
	virtual ~CResourcePropertiesDlg() {}
	enum { IDD = IDD_DIALOG_ARCHIVE_PROP_RESOURCE};
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};
