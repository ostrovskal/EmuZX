
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "Project.h"
#include "OrderBuildProjectsDlg.h"

IMPLEMENT_DYNAMIC(COrderBuildProjectsDlg, CDialog)

void COrderBuildProjectsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROJECTS, m_list);
	DDX_Control(pDX, IDC_BUTTON_UP, m_up);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_down);
}

BEGIN_MESSAGE_MAP(COrderBuildProjectsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_UP, &COrderBuildProjectsDlg::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &COrderBuildProjectsDlg::OnBnClickedButtonDown)
	ON_LBN_SELCHANGE(IDC_LIST_PROJECTS, &COrderBuildProjectsDlg::OnLbnSelchangeListProjects)
END_MESSAGE_MAP()

BOOL COrderBuildProjectsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	size_t index(0);
	CProject* prj;
	
	CSolution* sln(theApp.frm->GetSolution());
	while((prj = sln->EnumProjects(index)))
		m_list.AddString(prj->GetName());
	return TRUE;
}

void COrderBuildProjectsDlg::MoveItem(int dx)
{
	CString str;
	m_list.GetText(sel, str);
	m_list.DeleteString(sel);
	sel += dx;
	m_list.InsertString(sel, str);
	m_list.SetCurSel(sel);
	OnLbnSelchangeListProjects();
}

void COrderBuildProjectsDlg::OnBnClickedButtonUp()
{
	MoveItem(-1);
}

void COrderBuildProjectsDlg::OnBnClickedButtonDown()
{
	MoveItem(1);
}

void COrderBuildProjectsDlg::OnLbnSelchangeListProjects()
{
	sel = m_list.GetCurSel();
	m_up.EnableWindow(sel > 0);
	m_down.EnableWindow(sel < (m_list.GetCount() - 1));
}

void COrderBuildProjectsDlg::OnOK()
{
	CString str;
	int count(m_list.GetCount());
	Array<CString> arr;

	for(int i = 0 ; i < count ;i++)
	{
		m_list.GetText(i, str);
		arr.add(str);
	}
	theApp.frm->GetSolution()->ReorderProjects(arr);

	CDialog::OnOK();
}
