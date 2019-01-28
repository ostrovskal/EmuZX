
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ArchiveOptionsDlg.h"

IMPLEMENT_DYNAMIC(CArchiveOptionsDlg, CDialog)

void CArchiveOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_AVTO_BACKUP, m_backup);
	DDX_Control(pDX, IDC_LIST_MODULES, m_list_modules);
	DDX_Control(pDX, IDC_EDIT_NAME, m_module_name);
	DDX_Control(pDX, IDC_EDIT_PROG_ID, m_module_prog_id);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_add);
	DDX_Control(pDX, IDC_BUTTON_REMOVE, m_remove);
}

BEGIN_MESSAGE_MAP(CArchiveOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CArchiveOptionsDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CArchiveOptionsDlg::OnBnClickedButtonRemove)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CArchiveOptionsDlg::OnEnUpdateEdit)
	ON_EN_UPDATE(IDC_EDIT_PROG_ID, &CArchiveOptionsDlg::OnEnUpdateEdit)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_MODULES, &CArchiveOptionsDlg::OnLvnItemchangingListModules)
	ON_BN_CLICKED(IDC_CHECK_AVTO_BACKUP, &CArchiveOptionsDlg::OnBnClickedCheckAvtoBackup)
END_MESSAGE_MAP()

void CArchiveOptionsDlg::OnBnClickedButtonAdd()
{
	CPanelArchiveView::ACTIVEX* ax;
	if((ax = theApp.frm->GetPanelArchive()->AddResourceModule(name, progID)))
	{
		int index(m_list_modules.GetItemCount());
		m_list_modules.InsertItem(index, ax->name);
		m_list_modules.SetItemText(index, 1, ax->desc);
		m_list_modules.SetItemText(index, 2, ax->progID);
		m_list_modules.SetItemText(index, 3, ax->ext);
	}
	else
	{
		MessageBox("Модуль не найден или уже существует в системе!", "Ошибка", MB_ICONERROR);
	}
}

void CArchiveOptionsDlg::OnBnClickedButtonRemove()
{
	CPanelArchiveView::ACTIVEX* ax;
	CPanelArchiveView* pa(theApp.frm->GetPanelArchive());
	if((ax = pa->EnumResourceModules(cur_item)))
	{
		pa->RemoveResourceModule(ax->progID);
		m_list_modules.DeleteItem(cur_item);
		m_module_prog_id.SetWindowText("");
		m_module_name.SetWindowText("");
		cur_item = -1;
	}
}

void CArchiveOptionsDlg::OnEnUpdateEdit()
{
	m_module_prog_id.GetWindowText(progID);
	m_module_name.GetWindowText(name);
	m_add.EnableWindow(progID != "" && name != "");
	m_remove.EnableWindow(progID != "" && name != "");
}

void CArchiveOptionsDlg::OnLvnItemchangingListModules(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	cur_item = pNMLV->iItem;
	m_module_name.SetWindowText(m_list_modules.GetItemText(cur_item, 0));
	m_module_prog_id.SetWindowText(m_list_modules.GetItemText(cur_item, 2));
	*pResult = 0;
}

BOOL CArchiveOptionsDlg::OnInitDialog()
{
	size_t index(0);
	CPanelArchiveView::ACTIVEX* ax;
	LVITEM li;

	CDialog::OnInitDialog();

	m_backup.SetCheck(theApp.frm->GetConfiguration()->IsBackupArchive());

	m_list_modules.InsertColumn(0, "Расширение", LVCFMT_LEFT, 90);
	m_list_modules.InsertColumn(0, "ProgID", LVCFMT_LEFT, 90);
	m_list_modules.InsertColumn(0, "Описание", LVCFMT_LEFT, 100);
	m_list_modules.InsertColumn(0, "Имя", LVCFMT_LEFT, 120);

	li.mask = LVIF_TEXT;

	while((ax = theApp.frm->GetPanelArchive()->EnumResourceModules(index)))
	{
		m_list_modules.InsertItem(index, ax->name);
		m_list_modules.SetItemText(index, 1, ax->desc);
		m_list_modules.SetItemText(index, 2, ax->progID);
		m_list_modules.SetItemText(index, 3, ax->ext);
		index++;
	}

	return TRUE;
}

void CArchiveOptionsDlg::OnBnClickedCheckAvtoBackup()
{
	theApp.frm->GetConfiguration()->SetBackupArchive(m_backup.GetCheck());
}
