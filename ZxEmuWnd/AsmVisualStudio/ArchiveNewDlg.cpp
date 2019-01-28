
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ArchiveNewDlg.h"

IMPLEMENT_DYNAMIC(CArchiveNewDlg, CDialog)

void CArchiveNewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PATH, m_path);
	DDX_Control(pDX, IDC_EDIT_PATH_LST, m_path_lst);
	DDX_Control(pDX, IDC_EDIT_SIGNATURE, m_sign);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CArchiveNewDlg, CDialog)
	ON_EN_UPDATE(IDC_EDIT_PATH, &CArchiveNewDlg::OnEnUpdateEditPath)
	ON_EN_UPDATE(IDC_EDIT_PATH_LST, &CArchiveNewDlg::OnEnUpdateEditPathLst)
	ON_EN_UPDATE(IDC_EDIT_SIGNATURE, &CArchiveNewDlg::OnEnUpdateEditSignature)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CArchiveNewDlg::OnEnUpdateEditName)
END_MESSAGE_MAP()

void CArchiveNewDlg::OnUpdate()
{
	m_ok.EnableWindow(folder_arch != "" && folder_lst != "" && name != "" && signature != "");
}

void CArchiveNewDlg::OnEnUpdateEditPath()
{
	m_path.GetWindowText(folder_arch);
	OnUpdate();
}

void CArchiveNewDlg::OnEnUpdateEditPathLst()
{
	m_path_lst.GetWindowText(folder_lst);
	OnUpdate();
}

void CArchiveNewDlg::OnEnUpdateEditSignature()
{
	m_sign.GetWindowText(signature);
	OnUpdate();
}

void CArchiveNewDlg::OnEnUpdateEditName()
{
	m_name.GetWindowText(name);
	OnUpdate();
}

BOOL CArchiveNewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	folder_arch = theApp.frm->GetConfiguration()->GetDefaultProjectFolder();
	folder_lst = folder_arch;
	m_path.EnableFolderBrowseButton();
	m_path_lst.EnableFolderBrowseButton();
	m_path.SetWindowText(folder_arch);
	m_path_lst.SetWindowText(folder_lst);
	OnUpdate();
	return TRUE;
}
