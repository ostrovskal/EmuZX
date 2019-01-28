
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "BaseOptionsDlg.h"

IMPLEMENT_DYNAMIC(CBaseOptionsDlg, CDialog)

#define ID_PROP_BK_COLOR	0
#define ID_PROP_COLOR		1
#define ID_PROP_BOLD		2
#define ID_PROP_UNDERLINE	3

void CBaseOptionsDlg::SetOptElems(BYTE* grp, PROPERTY** dst_opt, int count, int is_grp)
{
	CString str;
	long val1, val2, val3, i_grp = 0;
	for(int i = 0 ; i < count ; i += 4)
	{
		if(is_grp && i_grp >= 0 && i_grp < 8)
		{
			GROUP_KEYWORD* g(cfg->GetCodeEditorGroup(i_grp));
			i_grp++;
			val1 = g->c_bk_text;
			val2 = g->c_text;
			val3 = g->style;
		}
		else
		{
			val1 = *(long*)(grp + (i - i_grp * 4) * 3 + 4);
			val2 = *(long*)(grp + (i - i_grp * 4) * 3);
			val3 = *(long*)(grp + (i - i_grp * 4) * 3 + 8);
		}
		dst_opt[i] = new PROPERTY;
		dst_opt[i]->name = "Фон текста";
		dst_opt[i]->opt_val = "Выбрать";
		dst_opt[i]->tip = "Цвет фона текста";
		dst_opt[i]->flags = FOPT_COLOR;
		dst_opt[i]->lVal = val1;

		dst_opt[i + 1] = new PROPERTY;
		dst_opt[i + 1]->name = "Текст";
		dst_opt[i + 1]->opt_val = "Выбрать";
		dst_opt[i + 1]->tip = "Цвет текста";
		dst_opt[i + 1]->flags = FOPT_COLOR;
		dst_opt[i + 1]->lVal = val2;

		dst_opt[i + 2] = new PROPERTY;
		dst_opt[i + 2]->name = "Bold";
		dst_opt[i + 2]->opt_val = "";
		dst_opt[i + 2]->tip = "Полужирное начертание текста";
		dst_opt[i + 2]->flags = FOPT_BOOL;
		dst_opt[i + 2]->lVal = (val3 == SI_BOLD || val3 == SI_BOLD_UNDERLINE);

		dst_opt[i + 3] = new PROPERTY;
		dst_opt[i + 3]->name = "Underline";
		dst_opt[i + 3]->opt_val = "";
		dst_opt[i + 3]->tip = "Подчеркнутое начертание текста";
		dst_opt[i + 3]->flags = FOPT_BOOL;
		dst_opt[i + 3]->lVal = (val3 == SI_UNDERLINE || val3 == SI_BOLD_UNDERLINE);
	}
}

CBaseOptionsDlg::CBaseOptionsDlg(CWnd* pParent /*=NULL*/) : CDialog(CBaseOptionsDlg::IDD, pParent)
{
	opt = NULL;
	cur_wnd = -1;
	cur_elem = -1;
	cur_grp_keys = NULL;
	cfg = theApp.frm->GetConfiguration();
	// имена шрифтов и их размеры
	fontNames[TE_OUTPUT] = cfg->GetNameFont(TE_OUTPUT);
	fontNames[TE_FIND] = cfg->GetNameFont(TE_FIND);
	fontNames[TE_EDITOR] = cfg->GetNameFont(TE_EDITOR);
	fontNames[TE_TEXT] = cfg->GetNameFont(TE_TEXT);
	fontNames[TE_BINARY] = cfg->GetNameFont(TE_BINARY);

	fontSizes[TE_OUTPUT] = cfg->GetSizeFont(TE_OUTPUT);
	fontSizes[TE_FIND] = cfg->GetSizeFont(TE_FIND);
	fontSizes[TE_EDITOR] = cfg->GetSizeFont(TE_EDITOR);
	fontSizes[TE_TEXT] = cfg->GetSizeFont(TE_TEXT);
	fontSizes[TE_BINARY] = cfg->GetSizeFont(TE_BINARY);
	// опции элементов
	SetOptElems((BYTE*)cfg->GetStdGroup(TE_OUTPUT), opt_elems_output, 24, FALSE);
	SetOptElems((BYTE*)cfg->GetStdGroup(TE_FIND), opt_elems_find, 24, FALSE);
	SetOptElems((BYTE*)cfg->GetStdGroup(TE_TEXT), opt_elems_text, 24, FALSE);
	SetOptElems((BYTE*)cfg->GetStdGroup(TE_BINARY), opt_elems_binary, 24, FALSE);
	SetOptElems((BYTE*)cfg->GetStdGroup(TE_EDITOR), opt_elems_editor, 60, TRUE);
	// сделать копиии ключевых слов
	for(int i = 0 ; i < 8 ; i++)
	{
		GROUP_KEYWORD* g(cfg->GetCodeEditorGroup(i));
		for(int j = 0 ; j < g->keys.GetSize() ; j++)
			grp_keys[i].Add(g->keys[j]);
	}
}

void CBaseOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_WINDOW, m_combo_wnd);
	DDX_Control(pDX, IDC_COMBO_FONT, m_combo_font);
	DDX_Control(pDX, IDC_COMBO_SIZE_FONT, m_combo_size);
	DDX_Control(pDX, IDC_LIST_ELEMS, m_list_elems);
	DDX_Control(pDX, IDC_LIST_KEYWORDS, m_list_keys);
	DDX_Control(pDX, IDC_STATIC_SAMPLE, m_sample);
	DDX_Control(pDX, IDC_LIST_PARAMETERS, m_grid_pars_elem);
	DDX_Control(pDX, IDC_EDIT_NEW_KEYWORD, m_new_keyword);
	DDX_Control(pDX, IDC_BUTTON_ADD_KEYWORD, m_add_keyword);
	DDX_Control(pDX, IDC_BUTTON_DEL_KEYWORD, m_del_keyword);
	DDX_Control(pDX, IDC_EDIT_DEF_FOLDER, m_def_folder);
	DDX_Control(pDX, IDC_EDIT_DEF_FOLDER_INCLUDE, m_def_folder_inc);
	DDX_Control(pDX, IDC_EDIT_DEF_FOLDER_LIB, m_def_folder_lib);
}

BEGIN_MESSAGE_MAP(CBaseOptionsDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_WINDOW, &CBaseOptionsDlg::OnCbnSelchangeComboWindow)
	ON_CBN_SELCHANGE(IDC_COMBO_FONT, &CBaseOptionsDlg::OnCbnSelchangeComboFont)
	ON_CBN_SELCHANGE(IDC_COMBO_SIZE_FONT, &CBaseOptionsDlg::OnCbnSelchangeComboSizeFont)
	ON_LBN_SELCHANGE(IDC_LIST_ELEMS, &CBaseOptionsDlg::OnLbnSelchangeListElems)
	ON_EN_UPDATE(IDC_EDIT_NEW_KEYWORD, &CBaseOptionsDlg::OnEnUpdateEditNewKeyword)
	ON_BN_CLICKED(IDC_BUTTON_ADD_KEYWORD, &CBaseOptionsDlg::OnBnClickedButtonAddKeyword)
	ON_BN_CLICKED(IDC_BUTTON_DEL_KEYWORD, &CBaseOptionsDlg::OnBnClickedButtonDelKeyword)
	ON_LBN_SELCHANGE(IDC_LIST_KEYWORDS, &CBaseOptionsDlg::OnLbnSelchangeListKeywords)
	ON_EN_UPDATE(IDC_EDIT_DEF_FOLDER, &CBaseOptionsDlg::OnEnUpdateEditDefFolder)
	ON_EN_UPDATE(IDC_EDIT_DEF_FOLDER_INCLUDE, &CBaseOptionsDlg::OnEnUpdateEditDefFolderInc)
	ON_EN_UPDATE(IDC_EDIT_DEF_FOLDER_LIB, &CBaseOptionsDlg::OnEnUpdateEditDefFolderLib)
	ON_REGISTERED_MESSAGE(WM_GRID_CHANGE_PROPERTY, &CBaseOptionsDlg::OnGridChangeProperty)
END_MESSAGE_MAP()

BOOL CBaseOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_grid_pars_elem.SetBoolLabels("Да", "Нет");
	m_grid_pars_elem.SetVSDotNetLook();
	m_grid_pars_elem.EnableHeaderCtrl(FALSE);
	m_grid_pars_elem.EnableDescriptionArea();
	m_grid_pars_elem.MarkModifiedProperties();
	m_grid_pars_elem.SetDescriptionRows(2);
	HDITEM item;
	item.mask = HDI_WIDTH;
	item.cxy = 80;
	m_grid_pars_elem.GetHeaderCtrl().SetItem(0, &item);
	m_combo_font.Setup(RASTER_FONTTYPE | TRUETYPE_FONTTYPE, RUSSIAN_CHARSET, FIXED_PITCH);
	m_combo_wnd.SetCurSel(2);
	m_def_folder.EnableFolderBrowseButton();
	m_def_folder_inc.EnableFolderBrowseButton();
	m_def_folder.SetWindowText(cfg->GetDefaultProjectFolder());
	m_def_folder_inc.EnableFolderBrowseButton();
	m_def_folder_inc.SetWindowText(cfg->GetDefaultIncludeFolder());
	m_def_folder_lib.EnableFolderBrowseButton();
	m_def_folder_lib.SetWindowText(cfg->GetDefaultLibraryFolder());
	OnCbnSelchangeComboWindow();
	return TRUE;
}

void CBaseOptionsDlg::OnCbnSelchangeComboFont()
{
	int sel(m_combo_font.GetCurSel());
	if(sel != -1)
	{
		CString str;
		m_combo_font.GetLBText(sel, str);
		fontNames[cur_wnd] = str;
		m_sample.SetDrawParams(NULL, NULL, str, NULL, NULL, NULL, NULL);
	}
}

void CBaseOptionsDlg::OnCbnSelchangeComboSizeFont()
{
	int sel(m_combo_size.GetCurSel());
	if(sel != -1)
	{
		CString str;
		m_combo_size.GetLBText(sel, str);
		int sz(atol(str));
		fontSizes[cur_wnd] = sz;
		m_sample.SetDrawParams(NULL, NULL, NULL, &sz, NULL, NULL, NULL);
	}
}

void CBaseOptionsDlg::OnCbnSelchangeComboWindow()
{
	String str;
	cur_wnd = m_combo_wnd.GetCurSel();
	int sel_nf(m_combo_font.FindString(-1, fontNames[cur_wnd]));
	int sel_sf(m_combo_size.FindString(-1, str.format("%d", fontSizes[cur_wnd])));
	m_combo_size.SetCurSel(sel_sf == -1 ? 0 : sel_sf);
	m_combo_font.SetCurSel(sel_nf == -1 ? 0 : sel_nf);
	OnCbnSelchangeComboFont();
	OnCbnSelchangeComboSizeFont();
	m_list_elems.ResetContent();

	switch(cur_wnd)
	{
		case TE_EDITOR:
			m_list_elems.AddString("Группа1");
			m_list_elems.AddString("Группа2");
			m_list_elems.AddString("Группа3");
			m_list_elems.AddString("Группа4");
			m_list_elems.AddString("Группа5");
			m_list_elems.AddString("Группа6");
			m_list_elems.AddString("Группа7");
			m_list_elems.AddString("Группа8");
		case TE_OUTPUT:
		case TE_FIND:
		case TE_TEXT:
			m_list_elems.AddString("Выделенный текст");
			m_list_elems.AddString("Строки");
			m_list_elems.AddString("Цифры");
			m_list_elems.AddString("Знаки");
			m_list_elems.AddString("Комментарии");
			m_list_elems.AddString("Текст");
			if(cur_wnd == TE_EDITOR)
				m_list_elems.AddString("Закладки");
			break;
		case TE_BINARY:
			m_list_elems.AddString("Выделенный текст");
			m_list_elems.AddString("Содержимое");
			m_list_elems.AddString("Значения");
			m_list_elems.AddString("Регистры");
			m_list_elems.AddString("Адрес");
			m_list_elems.AddString("Текст");
			break;
		default:
			return;

	}
	m_list_elems.SetCurSel(0);
	m_list_elems.SetFocus();
	OnLbnSelchangeListElems();
}

void CBaseOptionsDlg::OnLbnSelchangeListElems()
{
	BOOL is_grp;
	// стереть из ключевых слов и новых слов и из сетки параметров
	m_list_keys.ResetContent();
	// установить по новой
	cur_elem = m_list_elems.GetCurSel();
	switch(cur_wnd)
	{
		case TE_OUTPUT:
			opt = opt_elems_output;
			break;
		case TE_FIND:
			opt = opt_elems_find;
			break;
		case TE_EDITOR:
			opt = opt_elems_editor;
			break;
		case TE_TEXT:
			opt = opt_elems_text;
			break;
		case TE_BINARY:
			opt = opt_elems_binary;
			break;
	}
	is_grp = (cur_wnd == TE_EDITOR && cur_elem < 8);
	m_new_keyword.EnableWindow(is_grp);
	m_add_keyword.EnableWindow(is_grp && m_str_keyword != "");
	m_del_keyword.EnableWindow(FALSE);
	if((cur_grp_keys = (is_grp ? &grp_keys[cur_elem] : NULL)))
	{
		// в список поместить слова из выбранной группы
		for(int i = 0 ; i < cur_grp_keys->GetSize() ; i++)
			m_list_keys.AddString(cur_grp_keys->GetAt(i));
	}
	// сформировать в гриде всю структуру опций
	m_grid_pars_elem.RemoveAll();
	m_grid_pars_elem.AddProperty(theApp.GetGridProperty(opt[cur_elem * 4], ID_PROP_BK_COLOR));
	m_grid_pars_elem.AddProperty(theApp.GetGridProperty(opt[cur_elem * 4 + 1], ID_PROP_COLOR));
	m_grid_pars_elem.AddProperty(theApp.GetGridProperty(opt[cur_elem * 4 + 2], ID_PROP_BOLD));
	m_grid_pars_elem.AddProperty(theApp.GetGridProperty(opt[cur_elem * 4 + 3], ID_PROP_UNDERLINE));
	PROPERTY* pp(opt[cur_elem * 4 + 3]);
	m_sample.SetDrawParams((COLORREF*)&opt[cur_elem * 4]->lVal, (COLORREF*)&opt[cur_elem * 4 + 1]->lVal, NULL, NULL, (bool*)&opt[cur_elem * 4 + 2]->lVal, NULL, (bool*)&opt[cur_elem * 4 + 3]->lVal);
}

static char m_arr_incorrect_syms[] = {'!', '\'', '#', '%', '^', '&', '*', '(', ')', '-', '=', '+', ';', ':', '\'', '{', '}', '[', ']', '/', '\\', '|', '<', '>', ',', '.', '?', '№'};

bool IsCorrect(const CString& str)
{
	if(str != "")
	{
		CString str1("1234567890");
		if(str1.Find(str[0]) == -1)
		{
			for(int i = 0 ; i < sizeof(m_arr_incorrect_syms) ; i++)
			{
				if(str.Find(m_arr_incorrect_syms[i]) != -1)
					return false;
			}
			return true;
		}
	}

	return false;
}

void CBaseOptionsDlg::OnEnUpdateEditNewKeyword()
{
	m_new_keyword.GetWindowText(m_str_keyword);
	m_str_keyword.MakeLower();
	// проверить на коррекность - чтобы не было лишних символов
	m_add_keyword.EnableWindow(IsCorrect(m_str_keyword));
}

void CBaseOptionsDlg::OnBnClickedButtonAddKeyword()
{
	// проверить - такое ключевое слово уже есть в списке
	if((m_list_keys.FindString(0, m_str_keyword) != -1) || cfg->IsExistKeyword(m_str_keyword))
	{
		MessageBox("Ошибка. Такое ключевое слово уже есть в списке!", ASM_STUDIO, MB_ICONERROR);
	}
	else
	{
		cur_grp_keys->Add(m_str_keyword);
		m_list_keys.AddString(m_str_keyword);
		m_new_keyword.SetWindowText("");
	}
	m_new_keyword.SetFocus();
}

void CBaseOptionsDlg::OnBnClickedButtonDelKeyword()
{
	int sel(m_list_keys.GetCurSel());
	m_list_keys.DeleteString(sel);
	cur_grp_keys->RemoveAt(sel);
	m_list_keys.SetCurSel(sel);
	m_list_keys.SetFocus();
	OnLbnSelchangeListKeywords();
}

void CBaseOptionsDlg::OnLbnSelchangeListKeywords()
{
	int sel(m_list_elems.GetCurSel());
	m_del_keyword.EnableWindow(sel != -1);
}

void SetStdGrp(PROPERTY** opt, STD_GROUP* grp)
{
	grp->c_bk_sel_text = opt[0]->lVal;
	grp->c_sel_text = opt[1]->lVal;
	grp->s_sel_text = (opt[2]->lVal * SI_BOLD | opt[3]->lVal * SI_UNDERLINE);

	grp->c_bk_string = opt[4]->lVal;
	grp->c_string = opt[5]->lVal;
	grp->s_string = (opt[6]->lVal * SI_BOLD | opt[7]->lVal * SI_UNDERLINE);

	grp->c_bk_number = opt[8]->lVal;
	grp->c_number = opt[9]->lVal;
	grp->s_number = (opt[10]->lVal * SI_BOLD | opt[11]->lVal * SI_UNDERLINE);

	grp->c_bk_ops = opt[12]->lVal;
	grp->c_ops = opt[13]->lVal;
	grp->s_ops = (opt[14]->lVal * SI_BOLD | opt[15]->lVal * SI_UNDERLINE);

	grp->c_bk_comment = opt[16]->lVal;
	grp->c_comment = opt[17]->lVal;
	grp->s_comment = (opt[18]->lVal * SI_BOLD | opt[19]->lVal * SI_UNDERLINE);

	grp->c_bk_text = opt[20]->lVal;
	grp->c_text = opt[21]->lVal;
	grp->s_text = (opt[22]->lVal * SI_BOLD | opt[23]->lVal * SI_UNDERLINE);
}

void CBaseOptionsDlg::OnOK()
{
	CDocumentCommon* doc;
	STD_GROUP* grp;
	// установить все параметры
	// имена шрифтов и их размеры
	cfg->SetNameFont(TE_OUTPUT, fontNames[TE_OUTPUT]);
	cfg->SetNameFont(TE_FIND, fontNames[TE_FIND]);
	cfg->SetNameFont(TE_TEXT, fontNames[TE_TEXT]);
	cfg->SetNameFont(TE_EDITOR, fontNames[TE_EDITOR]);
	cfg->SetNameFont(TE_BINARY, fontNames[TE_BINARY]);
	cfg->SetSizeFont(TE_OUTPUT, fontSizes[TE_OUTPUT]);
	cfg->SetSizeFont(TE_FIND, fontSizes[TE_FIND]);
	cfg->SetSizeFont(TE_TEXT, fontSizes[TE_TEXT]);
	cfg->SetSizeFont(TE_EDITOR, fontSizes[TE_EDITOR]);
	cfg->SetSizeFont(TE_BINARY, fontSizes[TE_BINARY]);
	// стандартные структуры
	SetStdGrp(opt_elems_output, cfg->GetStdGroup(TE_OUTPUT));
	SetStdGrp(opt_elems_find, cfg->GetStdGroup(TE_FIND));
	SetStdGrp(opt_elems_text, cfg->GetStdGroup(TE_TEXT));
	SetStdGrp(opt_elems_binary, cfg->GetStdGroup(TE_BINARY));
	grp = cfg->GetStdGroup(TE_EDITOR);
	SetStdGrp(&opt_elems_editor[32], grp);
	// закладки редактора кода
	grp->c_bk_bookmark = opt[56]->lVal;
	grp->c_bookmark = opt[57]->lVal;
	grp->s_bookmark = (opt[58]->lVal * SI_BOLD | opt[59]->lVal * SI_UNDERLINE);
	// группы ключевых слов
	for(int i = 0 ; i < 32 ; i += 4)
	{
		GROUP_KEYWORD* key_grp(cfg->GetCodeEditorGroup(i / 4));
		key_grp->c_bk_text = opt_elems_editor[i]->lVal;
		key_grp->c_text = opt_elems_editor[i + 1]->lVal;
		key_grp->style = (opt_elems_editor[i + 2]->lVal * SI_BOLD | opt_elems_editor[i + 3]->lVal * SI_UNDERLINE);
		// обновить ключевые слова
		key_grp->keys.RemoveAll();
		for(int j = 0 ; j < grp_keys[i / 4].GetSize() ; j++)
			key_grp->keys.Add(grp_keys[i / 4].GetAt(j));
	}
	theApp.frm->GetPanelFind()->UpdateOptions();
	theApp.frm->GetPanelOutput()->UpdateOptions();
	if((doc = theApp.frm->GetDocument()))
		doc->UpdateAllViews(NULL);
	CDialog::OnOK();
}

void CBaseOptionsDlg::OnEnUpdateEditDefFolderInc()
{
	CString str;
	m_def_folder_inc.GetWindowText(str);
	cfg->SetDefaultIncludeFolder(str);
}

void CBaseOptionsDlg::OnEnUpdateEditDefFolderLib()
{
	CString str;
	m_def_folder_lib.GetWindowText(str);
	cfg->SetDefaultLibraryFolder(str);
}

void CBaseOptionsDlg::OnEnUpdateEditDefFolder()
{
	CString str;
	m_def_folder.GetWindowText(str);
	cfg->SetDefaultProjectFolder(str);
}

LRESULT CBaseOptionsDlg::OnGridChangeProperty(WPARAM wParam, LPARAM lParam)
{
	COleVariant val(((CMFCPropertyGridProperty*)lParam)->GetValue());
	PROPERTY* prop(opt[cur_elem * 4 + wParam]);

	switch(val.vt)
	{
		case VT_BOOL:
			prop->lVal = (val.boolVal == VARIANT_TRUE);
			break;
		case VT_I4:
			prop->lVal = val.lVal;
			break;
	}
	switch(wParam)
	{
		case ID_PROP_BK_COLOR:
			m_sample.SetDrawParams((COLORREF*)&prop->lVal, NULL, NULL, NULL, NULL, NULL, NULL);
			break;
		case ID_PROP_COLOR:
			m_sample.SetDrawParams(NULL, (COLORREF*)&prop->lVal, NULL, NULL, NULL, NULL, NULL);
			break;
		case ID_PROP_BOLD:
			m_sample.SetDrawParams(NULL, NULL, NULL, NULL, (bool*)&prop->lVal, NULL, NULL);
			break;
		case ID_PROP_UNDERLINE:
			m_sample.SetDrawParams(NULL, NULL, NULL, NULL, NULL, NULL, (bool*)&prop->lVal);
			break;
	}

	return 0;
}
