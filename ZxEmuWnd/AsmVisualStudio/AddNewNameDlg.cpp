
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "AddNewNameDlg.h"

IMPLEMENT_DYNAMIC(CAddNewNameDlg, CDialog)

void CAddNewNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CAddNewNameDlg, CDialog)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CAddNewNameDlg::OnEnUpdateEditName)
END_MESSAGE_MAP()

void CAddNewNameDlg::OnEnUpdateEditName()
{
	m_name.GetWindowText(name);
	m_ok.EnableWindow(name != "");
}

BOOL CAddNewNameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(title);
	m_name.SetWindowText(value);
	m_name.SetSel(0, value.GetLength());
	return TRUE;
}
