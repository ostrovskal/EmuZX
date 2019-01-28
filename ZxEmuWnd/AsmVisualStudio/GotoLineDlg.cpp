
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "GotoLineDlg.h"

IMPLEMENT_DYNAMIC(CGotoLineDlg, CDialog)

void CGotoLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LINE, m_lines);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CGotoLineDlg, CDialog)
	ON_EN_UPDATE(IDC_EDIT_LINE, &CGotoLineDlg::OnEnUpdateEditLine)
END_MESSAGE_MAP()

void CGotoLineDlg::OnEnUpdateEditLine()
{
	m_lines.GetWindowText(str_num);
	m_ok.EnableWindow(str_num != "");
}

void CGotoLineDlg::OnOK()
{
	line = atoi(str_num);
	CDialog::OnOK();
}
