
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "NewProjectDlg.h"

IMPLEMENT_DYNAMIC(CNewProjectDlg, CDialog)

void CNewProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_EDIT_FOLDER, m_folder);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_LIST_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_FOLDER, m_s_folder);
	DDX_Control(pDX, IDC_STATIC_SOLUTION, m_s_sln);
	DDX_Control(pDX, IDC_EDIT_NAME_SOLUTION, m_sln);
}

BEGIN_MESSAGE_MAP(CNewProjectDlg, CDialog)
	ON_EN_UPDATE(IDC_EDIT_FOLDER, &CNewProjectDlg::OnEnUpdateEditFolder)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CNewProjectDlg::OnEnUpdateEditName)
	ON_EN_UPDATE(IDC_EDIT_NAME_SOLUTION, &CNewProjectDlg::OnEnUpdateEditNameSolution)
END_MESSAGE_MAP()

BOOL CNewProjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	size_t index(0);
	WIZARD* wzd;
	LVITEMA item;

	if(action == ACTION_NEW_PROJECT)
	{
		m_sln.ShowWindow(SW_NORMAL);
		m_s_sln.ShowWindow(SW_NORMAL);
	}
	img_list.Create(32, 32, ILC_MASK | ILC_COLOR24, 0, 0);
	// установить элементы списка
	item.mask = LVIF_TEXT | LVIF_INDENT | LVIF_IMAGE;
	item.iIndent = 2;
	item.iSubItem = 0;
	while((wzd = theApp.frm->GetConfiguration()->EnumWizards(index)))
	{
		img_list.Add(wzd->hIcon);
		item.pszText = (char*)(const char*)wzd->name;
		item.iItem = index - 1;
		item.iImage = index - 1;
		m_type.InsertItem(&item);
	}
	m_type.SetImageList(&img_list, LVSIL_NORMAL);
	m_type.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	m_folder.EnableFolderBrowseButton();
	folder = theApp.frm->GetConfiguration()->GetDefaultProjectFolder();
	m_folder.SetWindowText(folder);
	return TRUE;
}

void CNewProjectDlg::OnEnUpdateEditFolder()
{
	m_folder.GetWindowText(folder);
	m_ok.EnableWindow(name != "" && folder != "" && name_sln != "");
}

void CNewProjectDlg::OnEnUpdateEditName()
{
	m_name.GetWindowText(name);
	m_sln.SetWindowText(name);
	m_ok.EnableWindow(name != "" && folder != "");
}

void CNewProjectDlg::OnOK()
{
	wzd = NULL;
	POSITION pos(m_type.GetFirstSelectedItemPosition());
	if(pos)
		wzd = theApp.frm->GetConfiguration()->GetWizard(m_type.GetItemText(m_type.GetNextSelectedItem(pos), 0));
	CDialog::OnOK();
}

void CNewProjectDlg::OnEnUpdateEditNameSolution()
{
	m_sln.GetWindowText(name_sln);
	m_ok.EnableWindow(name != "" && folder != "" && (action == ACTION_NEW_PROJECT ? name_sln != "" : TRUE));
}
