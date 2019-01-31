
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "AddResourceDlg.h"

IMPLEMENT_DYNAMIC(CAddResourceDlg, CDialog)

void CAddResourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PATH, m_path);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_type);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CAddResourceDlg, CDialog)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CAddResourceDlg::OnEnUpdateEditName)
	ON_EN_UPDATE(IDC_EDIT_PATH, &CAddResourceDlg::OnEnUpdateEditName)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CAddResourceDlg::OnCbnSelchangeComboType)
END_MESSAGE_MAP()

void CAddResourceDlg::OnEnUpdateEditName()
{
	m_path.GetWindowText(path);
	m_name.GetWindowText(name);
	if(!Utils::is_correctName(name, NULL)) name = "";
	m_ok.EnableWindow(path != "" && name != "");
}

void CAddResourceDlg::OnCbnSelchangeComboType()
{
	m_path.SetWindowText("");
	OnEnUpdateEditName();
}

BOOL CAddResourceDlg::OnInitDialog()
{
	CPanelArchiveView::ACTIVEX* ax;
	size_t index(0);

	CDialog::OnInitDialog();

	while((ax = theApp.frm->GetPanelArchive()->EnumResourceModules(index)))
	{
		m_type.AddString(ax->desc);
		index++;
	}

	m_path.EnableFolderBrowseButton();
	m_type.SetCurSel(0);
	OnCbnSelchangeComboType();

	return TRUE;
}

void CAddResourceDlg::OnOK()
{
	CPanelArchiveView::ACTIVEX* ax;
	size_t index(m_type.GetCurSel());
	if((ax = theApp.frm->GetPanelArchive()->EnumResourceModules(index)))
		name += ax->ext;
	CDialog::OnOK();
}
