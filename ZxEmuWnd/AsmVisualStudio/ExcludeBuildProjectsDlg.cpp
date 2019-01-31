
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ExcludeBuildProjectsDlg.h"

IMPLEMENT_DYNAMIC(CExcludeBuildProjectsDlg, CDialog)

void CExcludeBuildProjectsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROJECTS, m_list);
}

BEGIN_MESSAGE_MAP(CExcludeBuildProjectsDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROJECTS, &CExcludeBuildProjectsDlg::OnLvnItemchangedListProjects)
END_MESSAGE_MAP()

BOOL CExcludeBuildProjectsDlg::OnInitDialog()
{
	size_t index_p(0), iItem(0);
	CProject* prj;
	CConfig* cfg;
	LVITEM item;

	CDialog::OnInitDialog();

	ListView_SetExtendedListViewStyle(m_list, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);

	item.mask = LVIF_PARAM;
	item.iSubItem = 0;
	item.cColumns = 0;

	m_list.InsertColumn(0, "#", 0, 35);
	m_list.InsertColumn(1, "Проект", 0, 150);
	m_list.InsertColumn(2, "Конфигурация", 0, 130);
	m_list.InsertColumn(3, "Платформа", 0, 126);
	CSolution* sln(theApp.frm->GetSolution());
	// перечислить проекты
	while((prj = sln->EnumProjects(index_p)))
	{
		// перечислить платформы
		Array<CPlatform*>* plts(sln->GetAllPlatforms());
		for(size_t i = 0 ; i < plts->size() ; i++)
		{
			CPlatform* plt(plts->get(i));
			size_t j(0);
			// перечислить конфигурации
			while((cfg = plt->EnumConfigs(j)))
			{
				// добавляем элемент
				item.iItem = iItem;
				item.lParam = (LPARAM)cfg;
				m_list.InsertItem(&item);
				m_list.SetItemText(iItem, 1, prj->GetName());
				m_list.SetItemText(iItem, 2, cfg->GetName());
				m_list.SetItemText(iItem, 3, plt->GetName());
				m_list.SetItemState(iItem, UINT((int(cfg->IsProjectExclude(prj)) + 1) << 12),  LVIS_STATEIMAGEMASK);
				iItem++;
			}
		}
	}
	return TRUE;
}

void CExcludeBuildProjectsDlg::OnLvnItemchangedListProjects(NMHDR *pNMHDR, LRESULT *pResult)
{
	CConfig* cfg;
	CProject* prj;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	BOOL bPrevState = (BOOL)(((pNMListView->uOldState & LVIS_STATEIMAGEMASK) >> 12) - 1);
	if(bPrevState >= 0)
	{
		BOOL bNewState = (BOOL)(((pNMListView->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1);
		cfg = (CConfig*)pNMListView->lParam;
		prj = sln->GetProject(m_list.GetItemText(pNMListView->iItem, 1));
		if(cfg->IsProjectExclude(prj) != bNewState)
			cfg->SetProjectExclude(prj, bNewState);
	}
	*pResult = 0;
}
