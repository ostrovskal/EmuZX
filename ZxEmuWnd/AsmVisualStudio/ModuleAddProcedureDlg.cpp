
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ModuleAddProcedureDlg.h"

IMPLEMENT_DYNAMIC(CModuleAddProcedureDlg, CDialog)

void CModuleAddProcedureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_USES, m_uses);
	DDX_Control(pDX, IDC_EDIT_NAME_FIELD, m_name_field);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_comment);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_add);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_del);
	DDX_Control(pDX, IDC_LIST_FIELDS, m_fields);
	DDX_Control(pDX, IDC_COMBO_TYPE_FIELD, m_type_field);
	DDX_Control(pDX, IDC_COMBO_ACCESS, m_access);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CModuleAddProcedureDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CModuleAddProcedureDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CModuleAddProcedureDlg::OnBnClickedButtonDel)
	ON_LBN_SELCHANGE(IDC_LIST_FIELDS, &CModuleAddProcedureDlg::OnLbnSelchangeListFields)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CModuleAddProcedureDlg::OnEnUpdateEditName)
	ON_EN_UPDATE(IDC_EDIT_NAME_FIELD, &CModuleAddProcedureDlg::OnEnUpdateEditNameField)
END_MESSAGE_MAP()

static char m_arr_incorrect_syms[] = {'!', '\'', '#', '%', '^', '&', '*', '(', ')', '-', '=', '+', ';', ':', '\'', '{', '}', '[', ']', '/', '\\', '|', '<', '>', ',', '.', '?', '№'};
static const char* m_arr_keywords[] = 
{
	"proc", "ends", "struct", "endp", "proto", "public",
	"private", "flat", "stdcall", "dword", "word", "byte",
	"dd", "dw", "db", "dup", "sizeof"
};

bool CModuleAddProcedureDlg::IsCorrectName(const CString& str)
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

bool CModuleAddProcedureDlg::IsDuplicateParams()
{
	CString str;
	for(int i = 0 ; i < m_fields.GetCount() ; i++)
	{
		m_fields.GetText(i, str);
		if(name_field == str.Left(str.Find(':')))
			return false;
	}
	return true;
}

void CModuleAddProcedureDlg::OnBnClickedButtonAdd()
{
	CString str;
	m_type_field.GetLBText(m_type_field.GetCurSel(), str);
	m_fields.AddString(name_field + ":" + str);
	m_name_field.SetWindowText("");
	m_name_field.SetFocus();
	OnEnUpdateEditNameField();
}

void CModuleAddProcedureDlg::OnBnClickedButtonDel()
{
	m_fields.DeleteString(cur_sel);
	OnLbnSelchangeListFields();
}

void CModuleAddProcedureDlg::OnLbnSelchangeListFields()
{
	m_del.EnableWindow((cur_sel = m_fields.GetCurSel()) != -1);
}

void CModuleAddProcedureDlg::OnEnUpdateEditName()
{
	m_name.GetWindowText(name);
	m_ok.EnableWindow(IsCorrectName(name));
}

void CModuleAddProcedureDlg::OnEnUpdateEditNameField()
{
	m_name_field.GetWindowText(name_field);
	m_add.EnableWindow(IsCorrectName(name_field) && IsDuplicateParams());
}

BOOL CModuleAddProcedureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_type_field.SetCurSel(0);
	m_access.SetCurSel(0);
	return TRUE;
}

void CModuleAddProcedureDlg::OnOK()
{
	CString access, uses, params, comment;
	m_comment.GetWindowText(comment);
	if(comment != "")
		text = "; " + comment + "\r\n";
	m_uses.GetWindowText(uses);
	m_access.GetLBText(m_access.GetCurSel(), access);
	// сформировать полный текст процедуры
	text += name + " proc " + access;
	for(int i = 0 ; i < m_fields.GetCount() ; i++)
	{
		CString str;
		m_fields.GetText(i, str);
		if(params != "")
			params += ", ";
		params += str;
	}
	if(params != "" && uses != "")
		uses += ", ";
	if(uses != "")
		text += " " + uses;
	text += " " + params + "\r\n\t\tret\r\n" + name + " endp\r\n\r\n";
	CDialog::OnOK();
}
