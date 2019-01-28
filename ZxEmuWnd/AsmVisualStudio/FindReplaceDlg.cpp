
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "FindReplaceDlg.h"
#include "DocumentAsmEditor.h"
#include "DocumentTxtEditor.h"

IMPLEMENT_DYNAMIC(CFindReplaceDlg, CDialog)

void CFindReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FIND, m_find_text);
	DDX_Control(pDX, IDC_COMBO_REPLACE, m_replace_text);
	DDX_Control(pDX, IDC_STATIC_REPLACE, m_replace_static);
	DDX_Control(pDX, IDC_CHECK_CASE, m_check_case);
	DDX_Control(pDX, IDC_CHECK_WORD, m_check_word);
	DDX_Control(pDX, IDC_CHECK_HIDE_TEXT, m_check_hide);
	DDX_Control(pDX, IDC_COMBO_FIND_REPLACE_BAR, m_bar_find_replace);
	DDX_Control(pDX, IDC_BUTTON_BOOKMARK, m_bookmark);
	DDX_Control(pDX, IDC_BUTTON_BOOKMARK_ALL, m_bookmark_all);
	DDX_Control(pDX, IDC_BUTTON_REPLACE_ALL, m_replace_all);
	DDX_Control(pDX, IDC_BUTTON_FIND_NEXT, m_find_next);
	DDX_Control(pDX, IDC_BUTTON_REPLACE, m_replace);
	DDX_Control(pDX, IDC_BUTTON_FIND_ALL, m_find_all);
}

BEGIN_MESSAGE_MAP(CFindReplaceDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FIND_NEXT, &CFindReplaceDlg::OnBnClickedButtonFindNext)
	ON_BN_CLICKED(IDC_BUTTON_BOOKMARK, &CFindReplaceDlg::OnBnClickedButtonBookmark)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE_ALL, &CFindReplaceDlg::OnBnClickedButtonReplaceAll)
	ON_BN_CLICKED(IDC_BUTTON_BOOKMARK_ALL, &CFindReplaceDlg::OnBnClickedButtonBookmarkAll)
	ON_BN_CLICKED(IDC_BUTTON_REPLACE, &CFindReplaceDlg::OnBnClickedButtonReplace)
	ON_BN_CLICKED(IDC_BUTTON_FIND_ALL, &CFindReplaceDlg::OnBnClickedButtonFindAll)
	ON_CBN_SELCHANGE(IDC_COMBO_FIND, &CFindReplaceDlg::OnCbnSelChangeComboFind)
	ON_CBN_EDITUPDATE(IDC_COMBO_FIND, &CFindReplaceDlg::OnCbnEditupdateComboFind)
	ON_CBN_SELCHANGE(IDC_COMBO_FIND_REPLACE_BAR, &CFindReplaceDlg::OnCbnSelchangeBar)
END_MESSAGE_MAP()

bool CFindReplaceDlg::FindReplaceToAction(int type, CDocumentCommon* doc, CElement* elem, long flags, bool is_begin)
{
	long result, old_x, old_y;
	COgxTextEditor* ogx(NULL);
	CString path;
	// 1. запрос панели поиска
	CPanelFindView* pf(theApp.frm->GetPanelFind());
	// 2. запрос редактора ј’
	if(doc)
	{
		ogx = doc->GetOgx();
		path = doc->GetPathName();
	}
	else if(elem)
	{
		if(elem->doc)
		{
			ogx = elem->doc->GetOgx();
		}
		else
		{
			CFile f;
			CString text;
			// фэйковый ogx
			ogx = theApp.frm->GetFakeOgx();
			// загрузить и установить текст
			if(!f.Open(elem->path, CFile::modeRead))
				return false;
			long length((long)f.GetLength());
			f.Read(text.GetBuffer(length), length);
			text.ReleaseBuffer();
			ogx->SetText(text, SSL_VIS);
			f.Close();
		}
		path = elem->path;
	}
	if(ogx)
	{
		switch(type)
		{
			case ACT_FIND_ALL:
			case ACT_REPLACE_ALL:
			case ACT_BOOKMARK_ALL:
				// запомнить старую позицию каретки
				ogx->GetCaretPosition(&old_x, &old_y);
				ogx->SetCaretPosition(0, 0, FALSE);
				break;
		}
		// 3. цикл поиска
		while((result = ogx->FindReplace(str_find, str_replace, flags)) != -1)
		{
			// при положительном результате (что то нашли) - выполн€ем требуемые действи€
			switch(type)
			{
				case ACT_FIND:
				case ACT_REPLACE:
				case ACT_BOOKMARK:
					if(ogx == theApp.frm->GetFakeOgx())
					{
						// если что то нашли, а документ реально не открыт - то открываем его
						theApp.frm->OpenDocument(path, elem, true);
					}
					return false;
				case ACT_FIND_ALL:
				case ACT_REPLACE_ALL:
				case ACT_BOOKMARK_ALL:
					// поместить в панель поиска
					pf->AddString(path, ogx->GetString(result), result + 1);
					break;
			}
			is_result = true;
		}
		switch(type)
		{
			case ACT_FIND_ALL:
			case ACT_REPLACE_ALL:
			case ACT_BOOKMARK_ALL:
				// восстановить оригинальную позицию карет
				ogx->SetCaretPosition(old_x, old_y, FALSE);
				ogx->UpdateAll(0);
				break;
		}
	}
	return false;
}

void CFindReplaceDlg::OnAction(int type, int nBar)
{
	CProject* prj;
	CElement* elem;
	Array<CDocumentCommon*> docs;
	bool is_begin(true);
	is_result = false;
	// 1. устанавливаем режимы
	long flags(0);
	if(m_check_case.GetCheck())
		flags |= OGX_FR_REGISTER;
	if(m_check_hide.GetCheck())
		flags |= OGX_FR_HIDDEN;
	if(m_check_word.GetCheck())
		flags |= OGX_FR_WORD;
	if(type == ACT_REPLACE || type == ACT_REPLACE_ALL)
		flags |= OGX_FR_REPLACE;
	else if(type == ACT_BOOKMARK || type == ACT_BOOKMARK_ALL)
		flags |= OGX_FR_BOOKMARK;
	else if(type == ACT_FIND || type == ACT_FIND_ALL)
		flags |= OGX_FR_FIND;
	if(type == ACT_FIND_ALL || type == ACT_REPLACE_ALL || type == ACT_BOOKMARK_ALL)
		flags |= OGX_FR_COMPLEX;
	// 2. определ€ем - что ищем
	if(str_find != "" && m_find_text.FindString(-1, str_find) == CB_ERR)
		m_find_text.InsertString(0, str_find);
	m_replace_text.GetWindowText(str_replace);
	if(str_replace != "" && m_replace_text.FindString(-1, str_replace) == CB_ERR)
		m_replace_text.InsertString(0, str_replace);
	// поместить в быстрый поиск
	if(theApp.frm->GetWndQuickFind()->FindItem(str_find) != -1)
		theApp.frm->GetWndQuickFind()->AddItem(str_find);
	// 3. установить заголовок в панели поиска
	SetTitlePanelFind(type, str_find, str_replace);
	// 4. непосредственно поиск в разрезе области поиска
	switch(nBar)
	{
		case BAR_SELECTION:
			while(FindReplaceToAction(type, theApp.frm->GetDocument(), NULL, flags | OGX_FR_SELECTION, true)) {}
			break;
		case BAR_CURRENT_DOCUMENT:
			while(FindReplaceToAction(type, theApp.frm->GetDocument(), NULL, flags, true)) {}
			break;
		case BAR_ALL_OPENED_DOCUMENTS:
			theApp.frm->GetAllDocuments(&docs, NULL, NULL, false, true, false);
			for( ; index_elem < docs.size() ; index_elem++)
			{
				while(FindReplaceToAction(type, docs[index_elem], NULL, flags, is_begin)) {}
				is_begin = false;
			}
			index_elem = 0;
			break;
		case BAR_CURRENT_PROJECT:
			if((prj = theApp.frm->GetSolution()->GetCurrentProject()))
			{
				while((elem = prj->EnumElements(index_elem)))
				{
					while(FindReplaceToAction(type, NULL, elem, flags, is_begin)) {}
					is_begin = false;
				}
				index_elem = 0;
			}
			break;
		case BAR_SOLUTION:
			while((prj = theApp.frm->GetSolution()->EnumProjects(index_prj)))
			{
				while((elem = prj->EnumElements(index_elem)))
				{
					while(FindReplaceToAction(type, NULL, elem, flags, is_begin)) {}
					is_begin = false;
				}
				index_elem = 0;
			}
			index_prj = 0;
			break;
	}
	switch(type)
	{
		case ACT_REPLACE_ALL:
		case ACT_FIND_ALL:
		case ACT_BOOKMARK_ALL:
			theApp.frm->GetPanelFind()->SetFinish();
			break;
	}
//	MessageBox(is_result ? "«авершение операции!" : "Ќичего не удалось найти!", ASM_STUDIO, MB_ICONWARNING);
	SetFocus();
}

void CFindReplaceDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void CFindReplaceDlg::OnOK()
{
	ShowWindow(SW_HIDE);
}

void CFindReplaceDlg::OnBnClickedButtonFindNext()
{
	OnAction(ACT_FIND, cur_bar);
}

void CFindReplaceDlg::OnBnClickedButtonBookmark()
{
	OnAction(ACT_BOOKMARK, cur_bar);
}

void CFindReplaceDlg::OnBnClickedButtonReplaceAll()
{
	OnAction(ACT_REPLACE_ALL, cur_bar);
}

void CFindReplaceDlg::OnBnClickedButtonBookmarkAll()
{
	OnAction(ACT_BOOKMARK_ALL, cur_bar);
}

void CFindReplaceDlg::OnBnClickedButtonFindAll()
{
	OnAction(ACT_FIND_ALL, cur_bar);
}

void CFindReplaceDlg::OnBnClickedButtonReplace()
{
	OnAction(ACT_REPLACE, cur_bar);
}

void CFindReplaceDlg::Initialize(const CString& finds, const CString& repls, long is_case, long is_hide, long is_word)
{
	size_t i;
	Array<String> arr_f;
	Array<String> arr_r;
	
	Utils::explode("$$$$", finds, arr_f);
	Utils::explode("$$$$", repls, arr_r);

	size_t count_f(arr_f.size() > 64 ? 64 : arr_f.size());
	size_t count_r(arr_r.size() > 64 ? 64 : arr_r.size());

	for(i = 0 ; i < count_f ; i++)
		m_find_text.AddString(arr_f[i]);
	for(i = 0 ; i < count_r ; i++)
		m_replace_text.AddString(arr_r[i]);
	m_bar_find_replace.SetCurSel(0);
	m_check_case.SetCheck(is_case);
	m_check_hide.SetCheck(is_hide);
	m_check_word.SetCheck(is_word);
}

void CFindReplaceDlg::Uninitialize(CString& finds, CString& repls, long& is_case, long& is_hide, long& is_word)
{
	int i;
	CString str;

	int count_f(m_find_text.GetCount());
	int count_r(m_replace_text.GetCount());

	if(count_f > 64)
		count_f = 64;
	if(count_r > 64)
		count_r = 64;

	finds = "";
	repls = "";
	for(i = 0 ; i < count_f ; i++)
	{
		m_find_text.GetLBText(i, str);
		if(finds != "")
			finds += "$$$$";
		finds += str;
	}
	for(i = 0 ; i < count_r ; i++)
	{
		m_replace_text.GetLBText(i, str);
		if(repls != "")
			repls += "$$$$";
		repls += str;
	}
	is_case = m_check_case.GetCheck();
	is_hide = m_check_hide.GetCheck();
	is_word = m_check_word.GetCheck();
}

void CFindReplaceDlg::Show(Styles type, const CString& find_text, long is_sel)
{
	BOOL is(type == STYLE_REPLACE || type == STYLE_REPLACE_IN_FILES);
	m_replace_text.EnableWindow(is);
	m_replace_static.EnableWindow(is);
	m_replace.EnableWindow(is);
	m_replace_all.EnableWindow(is);
	m_bookmark.EnableWindow(type == STYLE_FIND);
	m_bookmark_all.EnableWindow(type == STYLE_FIND || type == STYLE_FIND_IN_FILES);
	m_find_all.EnableWindow(type == STYLE_FIND_IN_FILES);
	m_find_next.EnableWindow(type != STYLE_FIND_IN_FILES);
	// сформировать строки списка области
	m_bar_find_replace.ResetContent();
	m_bar_find_replace.AddString("¬ выделении");
	m_bar_find_replace.AddString("¬ текущем документе");
	m_bar_find_replace.AddString("¬о всех открытых документах");
	if(theApp.frm->GetSolution()->GetCountProjects() > 0)
	{
		m_bar_find_replace.AddString("¬о всех документах текущего проекта");
		m_bar_find_replace.AddString("¬о всех документах системы");
	}
	// выделить текущую область
	if(is_sel)
	{
		m_bar_find_replace.SetCurSel(0);
	}
	m_bar_find_replace.SetCurSel((type == STYLE_REPLACE_IN_FILES || type == STYLE_FIND_IN_FILES) ? 3 : 1);
	style = type;
	m_find_text.SetWindowText(find_text);
	m_replace_text.SetCurSel(0);
	OnCbnSelchangeBar();
	OnCbnEditupdateComboFind();
	ShowWindow(SW_SHOW);
}

void CFindReplaceDlg::OnCbnSelChangeComboFind()
{
	int i(m_find_text.GetCurSel());
	if(i != -1)
		m_find_text.GetLBText(i, str_find);
	UpdateAll();
}
void CFindReplaceDlg::OnCbnEditupdateComboFind()
{
	m_find_text.GetWindowText(str_find);
	UpdateAll();
}

void CFindReplaceDlg::UpdateAll()
{
	bool is(str_find != "");
	if(style == STYLE_REPLACE_IN_FILES || style == STYLE_REPLACE)
	{
		m_replace.EnableWindow(is);
		m_replace_all.EnableWindow(is);
		m_replace_text.EnableWindow(is);
		m_replace_static.EnableWindow(is);
	}
	if(style == STYLE_FIND)
		m_bookmark.EnableWindow(is);
	if(style == STYLE_FIND || style == STYLE_FIND_IN_FILES)
		m_bookmark_all.EnableWindow(is);
	if(style != STYLE_FIND_IN_FILES)
		m_find_next.EnableWindow(is);
	if(style == STYLE_FIND_IN_FILES)
		m_find_all.EnableWindow(is);
}

void CFindReplaceDlg::OnCbnSelchangeBar()
{
	index_prj = 0;
	index_elem = 0;
	cur_bar = m_bar_find_replace.GetCurSel();
	if(cur_bar != -1)
	{
		CString str;
		m_bar_find_replace.GetLBText(cur_bar, str);
		if(str == "¬ выделении")
			cur_bar = BAR_SELECTION;
		else if(str == "¬ текущем документе")
			cur_bar = BAR_CURRENT_DOCUMENT;
		else if(str == "¬о всех открытых документах")
			cur_bar = BAR_ALL_OPENED_DOCUMENTS;
		else if(str == "¬о всех документах текущего проекта")
			cur_bar = BAR_CURRENT_PROJECT;
		else if(str == "¬о всех документах системы")
			cur_bar = BAR_SOLUTION;
	}
	else
	{
		cur_bar = BAR_CURRENT_DOCUMENT;
	}
}

void CFindReplaceDlg::SetTitlePanelFind(int nAction, const CString& fnd, const CString& rpl)
{
	CString text, param;
	// 1. действие
	switch(nAction)
	{
		case ACT_REPLACE_ALL:
			text = "«амена подстроки \"" + fnd + "\" на \"" + rpl + "\" ";
			break;
		case ACT_FIND_ALL:
			text = "ѕоиск подстроки \"" + fnd + "\" ";
			break;
		case ACT_BOOKMARK_ALL:
			text = "ѕометка подстроки \"" + fnd + "\" ";
			break;
		default:
			return;
	}
	// 2. параметры
	if(m_check_case.GetCheck())
		param = "без учета регистра";
	if(m_check_hide.GetCheck())
		param += (param != "" ? ", в скрытом тексте" : "в скрытом тексте");
	if(m_check_word.GetCheck())
		param += (param != "" ? ", слова целиком" : "слова целиком");
	// 3. область
	switch(cur_bar)
	{
		case BAR_SELECTION:
			text += "в выделении";
			break;
		case BAR_CURRENT_DOCUMENT:
			text += "в текущем документе";
			break;
		case BAR_ALL_OPENED_DOCUMENTS:
			text += "во всех открытых документах";
			break;
		case BAR_CURRENT_PROJECT:
			text += "во всех документах текущего проекта";
			break;
		case BAR_SOLUTION:
			text += "во всех документах системы";
			break;
	}
	if(param != "")
		text += " (" + param + ")";
	theApp.frm->GetPanelFind()->SetTitle(text);
}
