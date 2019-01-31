
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "SelectNewNameDlg.h"

IMPLEMENT_DYNAMIC(CSelectNewNameDlg, CDialog)

void CSelectNewNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SELECT, m_name);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CSelectNewNameDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT, &CSelectNewNameDlg::OnCbnSelchangeComboSelect)
END_MESSAGE_MAP()

BOOL CSelectNewNameDlg::OnInitDialog()
{
	Array<String> arr_exist;
	Array<String> arr_new;

	CDialog::OnInitDialog();

	SetWindowText(title);
	Utils::explode(",", exist_names, arr_exist);
	Utils::explode(",", new_names, arr_new);

	for(size_t i = 0 ; i < arr_new.size() ; i++)
	{
		if(arr_exist.find(arr_new[i]) == -1)
			m_name.AddString(arr_new[i]);
	}
	m_name.SetCurSel(0);
	OnCbnSelchangeComboSelect();
	return TRUE;
}

void CSelectNewNameDlg::OnCbnSelchangeComboSelect()
{
	int sel(m_name.GetCurSel());
	m_ok.EnableWindow(sel != -1);
	if(sel != -1)
		m_name.GetLBText(sel, name);
}
