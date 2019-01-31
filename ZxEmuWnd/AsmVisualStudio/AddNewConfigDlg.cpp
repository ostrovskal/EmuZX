
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "AddNewConfigDlg.h"

IMPLEMENT_DYNAMIC(CAddNewConfigDlg, CDialog)

CAddNewConfigDlg::CAddNewConfigDlg(const CString& copy_names, CWnd* pParent /*=NULL*/) : CDialog(CAddNewConfigDlg::IDD, pParent)
{
	Utils::explode(",", copy_names, arr_copy);
}

void CAddNewConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CONFIGS, m_configs);
	DDX_Control(pDX, IDC_EDIT_NEW_NAME, m_name);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CAddNewConfigDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_CONFIGS, &CAddNewConfigDlg::OnCbnSelchangeComboSelect)
	ON_EN_UPDATE(IDC_EDIT_NEW_NAME, &CAddNewConfigDlg::OnEnUpdateEditNewName)
END_MESSAGE_MAP()

BOOL CAddNewConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	for(size_t i = 0 ; i < arr_copy.size() ; i++)
		m_configs.AddString(arr_copy[i]);
	m_configs.SetCurSel(0);
	OnCbnSelchangeComboSelect();
	return TRUE;
}

void CAddNewConfigDlg::OnCbnSelchangeComboSelect()
{
	int sel(m_configs.GetCurSel());
	if(sel != -1)
	{
		m_configs.GetLBText(sel, name_copy);
	}
}

void CAddNewConfigDlg::OnEnUpdateEditNewName()
{
	m_name.GetWindowText(name);
	m_ok.EnableWindow(name != "" && name_copy != "");
}

void CAddNewConfigDlg::OnOK()
{
	// проверить новая конфигурации уже есть?
	if(arr_copy.find(String(name)) != -1)
	{
		MessageBox("Конфигурация с таким именем уже есть. Повторите ввод!", ASM_STUDIO,  MB_ICONERROR);
		return;
	}
	CDialog::OnOK();
}
