
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ProjectOptionsDlg.h"
#include "AddNewConfigDlg.h"
#include "SelectNewNameDlg.h"
#include "OrderBuildProjectsDlg.h"
#include "ExcludeBuildProjectsDlg.h"

IMPLEMENT_DYNAMIC(CProjectOptionsDlg, CDialog)

void CProjectOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CONFIG, m_config);
	DDX_Control(pDX, IDC_COMBO_PLATFORM, m_platform);
	DDX_Control(pDX, IDC_BUTTON_ADD_CONFIG, m_add_cfg);
	DDX_Control(pDX, IDC_BUTTON_DEL_CONFIG, m_del_cfg);
	DDX_Control(pDX, IDC_LIST_OPT, m_opt);
	DDX_Control(pDX, IDC_COMBO_TRANS_LINK, m_combo_sel);
	DDX_Control(pDX, IDC_BUTTON_ORDER, m_order);
	DDX_Control(pDX, IDC_BUTTON_EXCLUDE, m_exclude);
	DDX_Control(pDX, IDC_EDIT_START_PARAMS, m_params);
}

BEGIN_MESSAGE_MAP(CProjectOptionsDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PLATFORM, &CProjectOptionsDlg::OnCbnSelchangeComboPlatform)
	ON_CBN_SELCHANGE(IDC_COMBO_CONFIG, &CProjectOptionsDlg::OnCbnSelchangeComboConfig)
	ON_CBN_SELCHANGE(IDC_COMBO_TRANS_LINK, &CProjectOptionsDlg::OnCbnSelchangeComboTransLink)
	ON_BN_CLICKED(IDC_BUTTON_ADD_CONFIG, &CProjectOptionsDlg::OnBnClickedButtonAddConfig)
	ON_BN_CLICKED(IDC_BUTTON_DEL_CONFIG, &CProjectOptionsDlg::OnBnClickedButtonDelConfig)
	ON_BN_CLICKED(IDC_BUTTON_ORDER, &CProjectOptionsDlg::OnBnClickedButtonOrder)
	ON_BN_CLICKED(IDC_BUTTON_EXCLUDE, &CProjectOptionsDlg::OnBnClickedButtonExclude)
	ON_REGISTERED_MESSAGE(WM_GRID_CHANGE_PROPERTY, &CProjectOptionsDlg::OnGridChangeProperty)
END_MESSAGE_MAP()

void CProjectOptionsDlg::OnCbnSelchangeComboPlatform()
{
	CConfig* cfg;
	int sel(m_platform.GetCurSel());
	plt = NULL;

	m_add_cfg.EnableWindow(sel != -1);
	m_del_cfg.EnableWindow(sel != -1);
	m_config.ResetContent();
	m_str_configs = "";
	if(sel != -1)
	{
		CString str;
		m_platform.GetLBText(sel, str);
		// по текущей платформе установить все ее конфигурации
		if((plt = sln->GetPlatform(str)))
		{
			size_t i(0);
			while((cfg = plt->EnumConfigs(i)))
			{
				m_config.AddString(cfg->GetName());
				if(m_str_configs != "")
					m_str_configs += ",";
				m_str_configs += cfg->GetName();
			}
		}
	}
	HDITEM item;
	item.mask = HDI_WIDTH;
	item.cxy = 200;
	m_opt.GetHeaderCtrl().SetItem(0, &item);
	m_combo_sel.SetCurSel(0);
	m_config.SetCurSel(0);
	OnCbnSelchangeComboConfig();
}

void CProjectOptionsDlg::OnCbnSelchangeComboConfig()
{
	int sel(m_config.GetCurSel());
	m_del_cfg.EnableWindow(sel != -1);
	cfg = NULL;
	if(plt && sel != -1)
	{
		CString str;
		m_config.GetLBText(sel, str);
		cfg = plt->GetConfig(str);
	}
	OnCbnSelchangeComboTransLink();
}

BOOL CProjectOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// параметры грида
	m_opt.SetBoolLabels("Да", "Нет");
	m_opt.SetVSDotNetLook();
	m_opt.EnableHeaderCtrl(TRUE, "Ключь", "Значение");
	m_opt.EnableDescriptionArea();
	m_opt.MarkModifiedProperties();
	m_params.SetWindowText(sln->GetCurrentProject()->GetStartParams());
	// установить все допустимые платформы
	Array<CPlatform*>* plts(sln->GetAllPlatforms());
	for(size_t i = 0 ; i < plts->size() ; i++)
		m_platform.AddString(plts->get(i)->GetName());
	m_combo_sel.SetCurSel(0);
	m_platform.SetCurSel(0);
	OnCbnSelchangeComboPlatform();
	return TRUE;
}

void CProjectOptionsDlg::OnCbnSelchangeComboTransLink()
{
	m_opt.RemoveAll();
	if(plt && cfg)
	{
		opt = cfg->GetAllOptions(m_combo_sel.GetCurSel() == 0);
		// сформировать в гриде всю структуру опций
		for(size_t j = 0 ; j < opt->size() ; j++)
		{
			if(!(opt->get(j)->flags&FOPT_ALWAYS))
				m_opt.AddProperty(theApp.GetGridProperty(opt->get(j), j));
		}
	}
}

void CProjectOptionsDlg::OnBnClickedButtonAddConfig()
{
	CAddNewConfigDlg dlg(m_str_configs);
	if(dlg.DoModal() == IDOK)
	{
		CConfig* cfg(plt->GetConfig(dlg.name_copy));
		if(cfg)
		{
			if(plt->AddConfig(dlg.name, *cfg->GetAllOptions(true), *cfg->GetAllOptions(false)))
			{
				plt->SetModify();
				m_config.AddString(dlg.name);
				m_config.SetCurSel(m_config.GetCount() - 1);
				OnCbnSelchangeComboConfig();
			}
		}
	}
}

void CProjectOptionsDlg::OnBnClickedButtonDelConfig()
{
	if(MessageBox("Вы действительно хотите удалить конфигурацию?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		plt->DeleteConfig(cfg->GetName());
		plt->SetModify();
		m_config.DeleteString(m_config.GetCurSel());
		m_config.SetCurSel(m_config.GetCount() - 1);
		OnCbnSelchangeComboConfig();
	}
}

void CProjectOptionsDlg::OnBnClickedButtonOrder()
{
	COrderBuildProjectsDlg dlg;
	dlg.DoModal();
}

LRESULT CProjectOptionsDlg::OnGridChangeProperty(WPARAM wParam, LPARAM lParam)
{
	COleVariant val(((CMFCPropertyGridProperty*)lParam)->GetValue());
	PROPERTY* prop(opt->get(wParam));
	switch(val.vt)
	{
		case VT_BOOL:
			prop->lVal = (val.boolVal == VARIANT_TRUE);
			break;
		case VT_I4:
			prop->lVal = val.lVal;
			break;
		case VT_BSTR:
			prop->opt = val.bstrVal;
			break;
	}

	sln->SetModify();

	return 0;
}

void CProjectOptionsDlg::OnCancel()
{
	theApp.frm->UpdatePlatforms();
	CDialog::OnCancel();
}

void CProjectOptionsDlg::OnOK()
{
	CString str;
	m_params.GetWindowText(str);
	sln->GetCurrentProject()->SetStartParams(str);
	theApp.frm->UpdatePlatforms();
	CDialog::OnOK();
}

void CProjectOptionsDlg::OnBnClickedButtonExclude()
{
	CExcludeBuildProjectsDlg dlg;
	dlg.DoModal();
}
