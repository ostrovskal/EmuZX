
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ModuleAddEquDlg.h"

IMPLEMENT_DYNAMIC(CModuleAddEquDlg, CDialog)

void CModuleAddEquDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_value);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_comment);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CModuleAddEquDlg, CDialog)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CModuleAddEquDlg::OnEnUpdateEditName)
	ON_EN_UPDATE(IDC_EDIT_VALUE, &CModuleAddEquDlg::OnEnUpdateEditValue)
END_MESSAGE_MAP()

void CModuleAddEquDlg::OnOK()
{
	m_comment.GetWindowText(text);
	if(text != "")
		text = "; " + text + "\r\n";
	text += name + "\tequ\t" + value + "\r\n";
	CDialog::OnOK();
}

void CModuleAddEquDlg::OnEnUpdateEditName()
{
	m_name.GetWindowText(name);
	m_ok.EnableWindow(name != "" && value != "");
}

void CModuleAddEquDlg::OnEnUpdateEditValue()
{
	m_value.GetWindowText(value);
	m_ok.EnableWindow(value != "" && name != "");
}
