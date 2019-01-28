
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ShowUnOpenProjectDlg.h"

IMPLEMENT_DYNAMIC(CShowUnOpenProjectDlg, CDialog)

void CShowUnOpenProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LIST_PROJECTS, m_list);
	DDX_Control(pDX, IDC_STATIC_LIST, m_slist);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CShowUnOpenProjectDlg, CDialog)
END_MESSAGE_MAP()

BOOL CShowUnOpenProjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(title);
	m_slist.SetWindowText(name_list);
	m_ok.SetWindowText(name_ok);

	if(arr_prj)
	{
		for(int i = 0 ; i < arr_prj->GetSize() ; i++)
			m_list.AddString(arr_prj->GetAt(i));
	}
	return TRUE;
}
