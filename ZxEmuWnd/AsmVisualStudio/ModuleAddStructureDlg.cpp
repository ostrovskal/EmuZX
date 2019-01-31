
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ModuleAddStructureDlg.h"

IMPLEMENT_DYNAMIC(CModuleAddStructureDlg, CDialog)

void CModuleAddStructureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_NAME_FIELD, m_name_field);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_comment);
	DDX_Control(pDX, IDC_EDIT_VALUE_FIELD, m_value_field);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_add);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_del);
	DDX_Control(pDX, IDC_LIST_FIELDS, m_fields);
	DDX_Control(pDX, IDC_COMBO_TYPE_FIELD, m_type_field);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CModuleAddStructureDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CModuleAddStructureDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CModuleAddStructureDlg::OnBnClickedButtonDel)
	ON_LBN_SELCHANGE(IDC_LIST_FIELDS, &CModuleAddStructureDlg::OnLbnSelchangeListFields)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CModuleAddStructureDlg::OnEnUpdateEditName)
	ON_EN_UPDATE(IDC_EDIT_NAME_FIELD, &CModuleAddStructureDlg::OnEnUpdateEditNameField)
END_MESSAGE_MAP()

static char m_arr_incorrect_syms[] = {'!', '\'', '#', '%', '^', '&', '*', '(', ')', '-', '=', '+', ';', ':', '\'', '{', '}', '[', ']', '/', '\\', '|', '<', '>', ',', '.', '?', '№'};
static const char* m_arr_keywords[] = 
{
	"proc", "ends", "struct", "endp", "proto", "public",
	"private", "flat", "stdcall", "dword", "word", "byte",
	"dd", "dw", "db", "dup", "sizeof"
};

bool CModuleAddStructureDlg::IsCorrectName(const CString& str)
{
	if(str != "")
	{
		CString str1("1234567890");
		int i;
		if(str1.Find(str[0]) == -1)
		{
			for(i = 0 ; i < sizeof(m_arr_incorrect_syms) ; i++)
			{
				if(str.Find(m_arr_incorrect_syms[i]) != -1)
					return false;
			}
			// проверить на ключевые слова
			for(i = 0 ; i < 17 ; i++)
			{
				if(str.CompareNoCase(m_arr_keywords[i]) == 0)
					return false;
			}
			return true;
		}
	}

	return false;
}

bool CModuleAddStructureDlg::IsDuplicateParams()
{
	CString str;
	for(int i = 0 ; i < m_fields.GetCount() ; i++)
	{
		m_fields.GetText(i, str);
		int p1(str.Find(' ') + 1);
		int p2(str.ReverseFind(' '));
		if(name_field == str.Mid(p1, p2 - p1))
			return false;
	}
	return true;
}

BOOL CModuleAddStructureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_type_field.SetCurSel(0);
	return TRUE;
}

void CModuleAddStructureDlg::OnOK()
{
	CString params, comment;
	m_comment.GetWindowText(comment);
	if(comment != "")
		text = "; " + comment + "\r\n";
	// сформировать полный текст структуры
	text += name + " struct\r\n";
	for(int i = 0 ; i < m_fields.GetCount() ; i++)
	{
		CString str;
		m_fields.GetText(i, str);
		params += str + "\r\n";
	}
	params.Replace(' ', '\t');
	text += params + name + " ends\r\n\r\n";
	CDialog::OnOK();
}

void CModuleAddStructureDlg::OnBnClickedButtonAdd()
{
	CString str, str1;
	m_type_field.GetLBText(m_type_field.GetCurSel(), str);
	m_value_field.GetWindowText(str1);
	if(str1 == "")
		str1 = "?";
	m_fields.AddString(str + " " + name_field + " " + str1);
	m_name_field.SetWindowText("");
	m_name_field.SetFocus();
	m_ok.EnableWindow(IsCorrectName(name) && m_fields.GetCount() > 0);
	OnEnUpdateEditNameField();
}

void CModuleAddStructureDlg::OnBnClickedButtonDel()
{
	m_fields.DeleteString(cur_sel);
	OnLbnSelchangeListFields();
}

void CModuleAddStructureDlg::OnLbnSelchangeListFields()
{
	m_del.EnableWindow((cur_sel = m_fields.GetCurSel()) != -1);
}

void CModuleAddStructureDlg::OnEnUpdateEditName()
{
	m_name.GetWindowText(name);
	m_ok.EnableWindow(IsCorrectName(name) && m_fields.GetCount() > 0);
}

void CModuleAddStructureDlg::OnEnUpdateEditNameField()
{
	m_name_field.GetWindowText(name_field);
	m_add.EnableWindow(IsCorrectName(name_field) && IsDuplicateParams());
}
