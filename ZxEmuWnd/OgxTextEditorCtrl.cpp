
#include "stdafx.h"
#include "OgxTextEditor.h"
#include "OgxTextEditorCtrl.h"
#include ".\ogxtexteditorctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static STD_GROUP std_grp;
static GROUP_FONTS group_fonts;

IMPLEMENT_DYNCREATE(COgxTextEditorCtrl, COleControl)

BEGIN_MESSAGE_MAP(COgxTextEditorCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CHAR()
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_HSCROLL()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COgxTextEditorCtrl, COleControl)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "SetText", dispidSetText, SetText, VT_EMPTY, VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "IsTextCopy", dispidIsTextCopy, IsTextCopy, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "IsTextCut", dispidIsTextCut, IsTextCut, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "IsTextPaste", dispidIsTextPaste, IsTextPaste, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "TextCopy", dispidTextCopy, TextCopy, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "TextCut", dispidTextCut, TextCut, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "TextPaste", dispidTextPaste, TextPaste, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "IsTextModify", dispidIsTextModify, IsTextModify, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "IsTextReadOnly", dispidIsTextReadOnly, IsTextReadOnly, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetOffsetXY", dispidGetOffsetXY, GetOffsetXY, VT_EMPTY, VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "SetGroup", dispidSetGroup, SetGroup, VT_EMPTY, VTS_I4 VTS_HANDLE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "SetStdGroup", dispidSetStdGroup, SetStdGroup, VT_EMPTY, VTS_HANDLE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetText", dispidGetText, GetText, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetTextFromPoint", dispidGetLineText, GetTextFromPoint, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "Initialize", dispidInitialize, Initialize, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "IsTextUndo", dispidIsTextUndo, IsTextUndo, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "IsTextRedo", dispidIsTextRedo, IsTextRedo, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "TextUndo", dispidTextUndo, TextUndo, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "TextRedo", dispidTextRedo, TextRedo, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GotoLine", dispidGotoLine, GotoLine, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "SelectAll", dispidSelectAll, SelectAll, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "SetStatusLine", dispidSetStatusLine, SetStatusLine, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetStatusLine", dispidGetStatusLine, GetStatusLine, VT_I4, VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetCountLines", dispidGetCountLines, GetCountLines, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetLineWords", dispidGetLineWords, GetLineWords, VT_I4, VTS_I4 VTS_HANDLE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "SetStatusNode", dispidSetStatusNode, SetStatusNode, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetPageLines", dispidGetPageLines, GetPageLines, VT_I4, VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "SetFontParams", dispidSetFontParams, SetFontParams, VT_EMPTY, VTS_HANDLE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetWordFromCaretPosition", dispidGetWordFromCaretPosition, GetWordFromCaretPosition, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "FindReplace", dispidFindReplace, FindReplace, VT_I4, VTS_BSTR VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "SetModifyText", dispidSetModifyText, SetModifyText, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "AddString", dispidAddLineText, AddString, VT_EMPTY, VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetCaretPosition", dispidGetCaretPosition, GetCaretPosition, VT_EMPTY, VTS_PI4 VTS_PI4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "UpdateAll", dispidUpdateAll, UpdateAll, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "SetCaretPosition", dispidSetCaretPosition, SetCaretPosition, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "FormatText", dispidFormatText, FormatText, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(COgxTextEditorCtrl, "GetString", dispidGetString, GetString, VT_BSTR, VTS_I4)
END_DISPATCH_MAP()

BEGIN_EVENT_MAP(COgxTextEditorCtrl, COleControl)
	EVENT_CUSTOM_ID("TextChange", eventidTextChange, TextChange, VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("TextDoubleClick", eventidTextDoubleClick, TextDoubleClick, VTS_I4)
END_EVENT_MAP()

IMPLEMENT_OLECREATE_EX(COgxTextEditorCtrl, "OGXTEXTEDITOR.OgxTextEditorCtrl.1", 0x9809c162, 0xa477, 0x4e5d, 0xae, 0xd2, 0x3c, 0x7c, 0xf6, 0xde, 0x15, 0x25)
IMPLEMENT_OLETYPELIB(COgxTextEditorCtrl, _tlid, _wVerMajor, _wVerMinor)
const IID BASED_CODE IID_DOgxTextEditor = { 0xB51BD8C3, 0x30D0, 0x4FEB, { 0x89, 0x2F, 0xA5, 0xBC, 0xF5, 0x59, 0x44, 0xE2 } };
const IID BASED_CODE IID_DOgxTextEditorEvents = { 0xC2A0982F, 0x33D8, 0x408B, { 0xB0, 0xB1, 0xC8, 0xC, 0x45, 0xB1, 0x81, 0xFC } };

static const DWORD BASED_CODE _dwOgxTextEditorOleMisc = OLEMISC_ACTIVATEWHENVISIBLE | OLEMISC_SETCLIENTSITEFIRST | OLEMISC_INSIDEOUT | OLEMISC_CANTLINKINSIDE | OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(COgxTextEditorCtrl, IDS_OGXTEXTEDITOR, _dwOgxTextEditorOleMisc)

BOOL COgxTextEditorCtrl::COgxTextEditorCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	return (bRegister ? AfxOleRegisterControlClass(AfxGetInstanceHandle(), m_clsid, m_lpszProgID, IDS_OGXTEXTEDITOR, IDB_OGXTEXTEDITOR, afxRegApartmentThreading, _dwOgxTextEditorOleMisc, _tlid, _wVerMajor, _wVerMinor) : AfxOleUnregisterClass(m_clsid, m_lpszProgID));
}

COgxTextEditorCtrl::COgxTextEditorCtrl()
{
	InitializeIIDs(&IID_DOgxTextEditor, &IID_DOgxTextEditorEvents);

	x1_sel = y1_sel = x2_sel = y2_sel = 0;
	i1_sel = i2_sel = 0;
	xoffs = yoffs = 0;
	xc = yc = xcl = 0;
	xpage = ypage = 0;
	is_modify = false;
	flags = 0;
	is_selection = false;
	language = LANG_TXT;
	memset(groups, 0, sizeof(groups));

	std_group = &std_grp;
	std_group->c_text		= RGB(160, 180, 255);
	std_group->c_sel_text	= RGB(255, 255, 255);
	std_group->c_comment	= RGB(0,255,0);
	std_group->c_number		= RGB(255,255,0);
	std_group->c_ops		= RGB(0,255,255);
	std_group->c_string		= RGB(255,0,255);

	std_group->c_bk_text	= RGB(40, 40, 40);
	std_group->c_bk_sel_text= RGB(120, 120, 120);
	std_group->c_bk_comment	= RGB(40, 40, 40);
	std_group->c_bk_number	= RGB(40, 40, 40);
	std_group->c_bk_ops		= RGB(40, 40, 40);
	std_group->c_bk_string	= RGB(40, 40, 40);

	std_group->s_text		= SI_NORMAL;
	std_group->s_sel_text	= SI_NORMAL;
	std_group->s_comment	= SI_UNDERLINE;
	std_group->s_number		= SI_NORMAL;
	std_group->s_ops		= SI_NORMAL;
	std_group->s_string		= SI_NORMAL;

	group_fonts.hFont[0] = ::CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, "Courier");
	group_fonts.hFont[1] = ::CreateFont(-14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, "Courier");
	group_fonts.hFont[2] = ::CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, "Courier");
	group_fonts.hFont[3] = ::CreateFont(-14, 0, 0, 0, FW_BOLD, FALSE, TRUE, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, "Courier");

	group_fonts.hnf = 10;
	group_fonts.wnf = 10;
	group_fonts.wbf = 10;
	group_fonts.wuf = 10;
	group_fonts.wbuf = 10;

	grp_fonts = &group_fonts;

	pen_cur.CreatePen(PS_SOLID, 1, RGB(192, 0, 0));
}

COgxTextEditorCtrl::~COgxTextEditorCtrl()
{
	int i;

	for(i = 0 ; i < undo.GetSize() ; i++)
		delete undo[i];
	for(i = 0 ; i < redo.GetSize() ; i++)
		delete redo[i];
	undo.RemoveAll();
	redo.RemoveAll();
	text.RemoveAll();
	pen_cur.DeleteObject();
}

void COgxTextEditorCtrl::SetModifyText(LONG status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	is_modify = status;
}

void COgxTextEditorCtrl::UpdateAll(LONG flags)
{
	CRect rc;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// позиция каретки
	GetClientRect(rc);
	int xp(rc.Width() / grp_fonts->wnf);
	int yp(rc.Height() / grp_fonts->hnf);
	if(xpage != xp || ypage != yp)
	{
		xpage = xp;
		ypage = yp;
	}
	SetFocus();
	UpdateCaret(pcUpdate | pcOffset | pcChange);
}

BSTR COgxTextEditorCtrl::GetWordFromCaretPosition()
{
	int x1, x2;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	asmFindWord(text[yc].line, xc, &x1, &x2);
	CString strResult(text[yc].line.Mid(x1, x2 - x1));
	return strResult.AllocSysString();
}

LONG COgxTextEditorCtrl::FindReplace(LPCTSTR find_wrd, LPCTSTR repl_wrd, LONG flags)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int fx1(0), fx2(0), y1(yc), x1(xc), x2(0), y2(text.GetSize() - 1), ii;
	// 1. определяем начальную и конечную позиции поиска(с учетом поиска в выделенном)
	if(flags & (OGX_FR_SELECTION | OGX_FR_REPLACE) && IsTextCopy())
	{
		if(!(flags & OGX_FR_COMPLEX))
		{
			x1 = x1_sel, x2 = x2_sel;
			y1 = y1_sel, y2 = y2_sel;
			if(y1_sel != y2_sel)
			{
				if(y1 > y2) {ii = y1 ; y1 = y2 ; y2 = ii;}
				x2 = 1000000;
			}
			else
			{
				if(x1 > x2) {ii = x1 ; x1 = x2 ; x2 = ii;}
			}
			if(!(flags & OGX_FR_SELECTION))
				y2 = (text.GetSize() - 1);
		}
	}
	// 2. ищем
	bool is_begin = true;
	while(y1 <= y2)
	{
		if(asmFind(find_wrd, text[y1].line, x1, x2, &fx1, &fx2, flags))
		{
			// ищем в выделенном?
			if(flags & OGX_FR_SELECTION)
			{
				// найденное находится "внутри" выделения?
				int i1(asmGetOffsetLine(this, y1) + fx1);
				int i2(asmGetOffsetLine(this, y1) + fx2);
				if(i1 < i1_sel)
				{
					x1 = fx2;
					x2 = 1000000;
					continue;
				}
				if(i2 > i2_sel)
					return -1;
			} 
			// заменяем?
			if(flags & OGX_FR_REPLACE)
			{
				bool is(false);
				if(is_begin)
				{
					if(!(flags & OGX_FR_COMPLEX))
					{
						int i1(asmGetOffsetLine(this, y1) + fx1);
						int i2(asmGetOffsetLine(this, y1) + fx2);
						is = (i1_sel == i1 && i2_sel == i2);
					}
					else
					{
						is = true;
					}
					is_begin = false;
				}
				if(is)
				{
					// заменить
					text[y1].line = text[y1].line.Left(fx1) + repl_wrd + text[y1].line.Mid(fx2);
					x1 = xc = fx1 + strlen(repl_wrd);
					// скорректировать на длину замены выделение(если оно есть)
					if(IsTextCopy())
					{
						int xx1 = x1_sel, xx2 = x2_sel;
						int yy1 = y1_sel, yy2 = y2_sel;
						if(y1_sel != y2_sel)
						{
							if(yy1 > yy2) {ii = yy1 ; yy1 = yy2 ; yy2 = ii;}
						}
						else
						{
							if(xx1 > xx2) {ii = xx1 ; xx1 = xx2 ; xx2 = ii;}
						}
						i2_sel = asmGetOffsetLine(this, yy2) + xx2;
					}
					SetModifyText(TRUE);
					if(!(flags & OGX_FR_COMPLEX))
					{
						UpdateCaret(pcPosition | pcChange | pcOffset | pcReset | pcUpdate);
						continue;
					}
				}
			}
			// помечаем?
			if(flags & OGX_FR_BOOKMARK)
			{
				text[y1].status |= SSL_BMA;
				y1++;
				if(!(flags & OGX_FR_COMPLEX))
					UpdateCaret(pcPosition | pcChange | pcOffset);
			}
			// ищем?
			if(flags & (OGX_FR_FIND | OGX_FR_REPLACE))
			{
				if(!(flags & OGX_FR_COMPLEX))
				{
					// выделяем найденное
					startSel.x = fx1;
					startSel.y = y1;
					SetSelection(startSel, CPoint(fx2, y1), true);
					UpdateCaret(pcPosition | pcOffset | pcChange);
				}
			}
			if(flags & OGX_FR_COMPLEX)
			{
				xc = fx2;
				yc = y1;
			}
			return y1;
		}
		x1 = 0;
		x2 = 1000000;
		y1++;
	}
	return -1;
}

LONG COgxTextEditorCtrl::GetCountLines()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return text.GetSize();
}

void COgxTextEditorCtrl::SetStatusLine(LONG line, LONG status, LONG update)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(line >= 0 && line < text.GetSize())
	{
		text[line].status = status;
		if(update)
		{
			UpdateCaret(pcPosition | pcOffset | pcUpdate | pcChange);
		}
	}
}

LONG COgxTextEditorCtrl::IsTextUndo()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return undo.GetSize();
}

LONG COgxTextEditorCtrl::IsTextRedo()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return redo.GetSize();
}

void COgxTextEditorCtrl::TextUndo()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(IsTextUndo())
	{
		UNDO_REDO* ur;
		int index;

		index = (undo.GetSize() - 1);
		ur = undo[index];

		ApplyUndoRedo(true, ur);

		undo.RemoveAt(index);
		redo.Add(ur);
	}
}

void COgxTextEditorCtrl::TextRedo()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(IsTextRedo())
	{
		UNDO_REDO* ur;
		int index;

		index = redo.GetSize() - 1;
		ur = redo[index];
		ApplyUndoRedo(false, ur);
		redo.RemoveAt(index);
		undo.Add(ur);
	}
}

void COgxTextEditorCtrl::SetText(LPCTSTR txt, LONG status_lines)
{
	CString str_txt(txt),ln;
	int pos = 0, p, len = str_txt.GetLength();
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	text.RemoveAll();
	while(pos < len)
	{
		p = str_txt.Find("\r\n", pos);
		if(p == -1)
			p = len;
		ln = str_txt.Mid(pos, p - pos);
		text.Add(LINE_TEXT(ln, status_lines));
		pos = p + 2;
	}
	xc = yc = 0;
	text.Add(LINE_TEXT("", status_lines));
	UpdateCaret(pcOffset | pcUpdate | pcReset | pcChange);
}

LONG COgxTextEditorCtrl::IsTextCopy()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (i1_sel != i2_sel);
}

LONG COgxTextEditorCtrl::IsTextCut()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return ((i1_sel != i2_sel) && !(flags & OGXEF_READONLY));
}

LONG COgxTextEditorCtrl::IsTextPaste()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (::IsClipboardFormatAvailable(CF_TEXT) && !(flags & OGXEF_READONLY));
}

LONG COgxTextEditorCtrl::IsTextModify()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return is_modify;
}

LONG COgxTextEditorCtrl::IsTextReadOnly()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return ((flags & OGXEF_READONLY) != FALSE);
}

void COgxTextEditorCtrl::TextCopy()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(IsTextCopy())
	{
		if(OpenClipboard())
		{
			HGLOBAL clipbuffer;

			if(EmptyClipboard())
			{
				CString ln(copy());
				WORD locale(0x419);
				// сначала поместить 0x419 - CF_LOCALE
				clipbuffer = GlobalAlloc(GMEM_DDESHARE, 2);
				memcpy(GlobalLock(clipbuffer), &locale, 2);
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_LOCALE, clipbuffer);
				// поместить в буфер обмена
				clipbuffer = GlobalAlloc(GMEM_DDESHARE, ln.GetLength() + 1);
				strcpy((char*)GlobalLock(clipbuffer), (const char*)ln);
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_TEXT, clipbuffer);
				CloseClipboard();
			}
		}
	}
}

void COgxTextEditorCtrl::TextCut()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(IsTextCut())
	{
		TextCopy();
		RemoveSelection(true);
	}
}

void COgxTextEditorCtrl::TextPaste()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(IsTextPaste())
	{
		if(OpenClipboard())
		{
			is_modify = true;
			RemoveSelection(true);
			CString buffer((const char*)GetClipboardData(CF_TEXT));
			paste(buffer);
			CloseClipboard();
			UpdateCaret(pcPosition | pcOffset | pcUpdate | pcReset | pcChange);
		}
	}
}

void COgxTextEditorCtrl::SelectAll()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	startSel.SetPoint(0,0);
	yc = text.GetSize() - 1;
	xc = text[yc].line.GetLength();
	SetSelection(startSel, CPoint(xc, yc), true);
}

LONG COgxTextEditorCtrl::GetStatusLine(LONG line)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(line >= 0 && line < text.GetSize())
		return text[line].status;
	return 0;
}

LONG COgxTextEditorCtrl::GetPageLines(LONG* lines, LONG* statuses)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int yy, y = yoffs, i;
	for(i = 0 ; i <= ypage ; i++)
	{
		lines[i] = y;
		statuses[i] = text[y].status;
		yy = asmGotoLine(this, y, gtNext);
		if(y == yy)
			break;
		y = yy;
	}

	i++;
	lines[i] = y;
	statuses[i] = 0;
	return i;
}

void COgxTextEditorCtrl::GetOffsetXY(LONG* x_offs, LONG* y_offs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(x_offs) *x_offs = xoffs;
	if(y_offs) *y_offs = yoffs;
}

void COgxTextEditorCtrl::SetGroup(LONG index, OLE_HANDLE group)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(index >= 0 && index < 8)
	{
		groups[index] = (GROUP_KEYWORD*)group;
		Invalidate();
	}
}

void COgxTextEditorCtrl::SetStdGroup(OLE_HANDLE std_grp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	std_group = (STD_GROUP*)std_grp;
	Invalidate();
}

BSTR COgxTextEditorCtrl::GetText()
{
	CString strResult;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	for(int i = 0 ; i < text.GetSize() ; i++)
		strResult += (text[i].line + "\r\n");
	return strResult.AllocSysString();
}

BSTR COgxTextEditorCtrl::GetString(LONG y)
{
	CString strResult;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(y >= 0 && y < text.GetSize())
		strResult = text[y].line;
	return strResult.AllocSysString();
}

BSTR COgxTextEditorCtrl::GetTextFromPoint(LONG y)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CPoint pt(GetCoordCaretFromPoint(CPoint(0, y)));
	CString strResult(text[pt.y].line);
	return strResult.AllocSysString();
}

void COgxTextEditorCtrl::GotoLine(LONG y, LONG is_sel)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	y--;
	if(y < 0) y = 0;
	if(y >= text.GetSize()) y = text.GetSize() - 1;
	startSel.x = 0;
	startSel.y = y;
	if(is_sel)
	{
		SetSelection(startSel, CPoint(text[y].line.GetLength(), y), true);
	}
	else
	{
		xc = 0;
		yc = y;
		UpdateCaret(pcPosition | pcOffset | pcReset | pcChange | pcUpdate);
	}
}

void COgxTextEditorCtrl::Initialize(LONG x, LONG y, LONG lang, LONG fl)
{
	GROUP_KEYWORD grp;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	xc = x;
	yc = y;
	flags = fl;
	language = lang;

}

void COgxTextEditorCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si;

	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_HORZ, &si);

	int pos = si.nPos;

	switch(nSBCode)
	{
		case SB_LINEDOWN:
			if(pos < si.nMax)
				pos++;
			break;
		case SB_LINEUP:
			if(pos > si.nMin)
				pos--;
			break;
		case SB_PAGEDOWN:
			pos += si.nPage;
			if(pos > si.nMax)
				pos = si.nMax;
			break;
		case SB_PAGEUP:
			pos -= si.nPage;
			if(pos < si.nMin)
				pos = si.nMin;
			break;
		case SB_THUMBTRACK:
			pos = nPos;
			if(pos < si.nMin)
				pos = si.nMin;
			if(pos > si.nMax)
				pos = si.nMax;
			break;
	}

	xoffs = pos;
	SetScrollPos(SB_HORZ, pos, TRUE);
	UpdateCaret(pcUpdate);
}

void COgxTextEditorCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si;

	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;

	GetScrollInfo(SB_VERT, &si);

	int pos = si.nPos;

	switch(nSBCode)
	{
		case SB_LINEDOWN:
			if(pos < si.nMax)
				pos++;
			break;
		case SB_LINEUP:
			if(pos > si.nMin)
				pos--;
			break;
		case SB_PAGEDOWN:
			pos += si.nPage;
			if(pos > si.nMax)
				pos = si.nMax;
			break;
		case SB_PAGEUP:
			pos -= si.nPage;
			if(pos < si.nMin)
				pos = si.nMin;
			break;
		case SB_THUMBTRACK:
			pos = nPos;
			if(pos < si.nMin)
				pos = si.nMin;
			if(pos > si.nMax)
				pos = si.nMax;
			break;
	}
	yoffs = asmCnvUnrealToReal(this, pos);
	SetScrollPos(SB_VERT, pos, TRUE);
	UpdateCaret(pcPosition | pcUpdate | pcChange);
}

void COgxTextEditorCtrl::UpdateScroll(int type, int& offs, int count, int page)
{
	SCROLLINFO si;
	bool visible;

	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;

	GetScrollInfo(type, &si);

	int delta = count - page;
	visible = (delta > 0);
	offs = (visible ? (offs > delta ? delta : offs) : 0);
	si.nPage = (visible ? page : 0);
	si.nMax = (visible ? count - 1 : 0);
	si.nPos = (visible ? offs : 0);

	SetScrollInfo(type, &si, TRUE);
	ShowScrollBar(type, visible);
}

void COgxTextEditorCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	COleControl::OnChar(nChar, nRepCnt, nFlags);
	if(nChar > 31 && !(flags & OGXEF_READONLY))
	{
		CString txt((char)nChar);

		RemoveSelection(true);

		switch(nChar)
		{
			case '{':
				txt = "{}";
				break;
			case '(':
				txt = "()";
				break;
			case '[':
				txt = "[]";
				break;
			case '\'':
				txt = "\'\'";
				break;
			case '\"':
				txt = "\"\"";
				break;
		}
		SetUndo(urInsert, xc + 1, yc, txt);
		text[yc].line.Insert(xc, txt) ; xc++;
		is_modify = true;
		TextChange(yc, yc, OGX_ACT_INSERT);
		UpdateCaret(pcPosition | pcReset | pcOffset | pcUpdate);
	}
}

int COgxTextEditorCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	text.Add(LINE_TEXT());
	return 0;
}

BOOL COgxTextEditorCtrl::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

void COgxTextEditorCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	COleControl::OnKeyDown(nChar, nRepCnt, nFlags);
	bool shift = ((::GetKeyState(VK_SHIFT) & 0x80) == 0x80);
	bool ctrl = ((::GetKeyState(VK_CONTROL) & 0x80) == 0x80);

	if(ctrl)
	{
		switch(nChar)
		{
			case 'A':
				SelectAll();
				break;
			case 'C':
				TextCopy();
				break;
			case 'Z':
				TextUndo();
				break;
			case 'Y':
				TextRedo();
				break;
			case 'X':
			case VK_DELETE:
				TextCut();
				break;
			case 'V':
				TextPaste();
				break;
		}
	}
	switch(nChar)
	{
		case VK_INSERT:
			if(shift)
				TextPaste();
			break;
		case VK_RETURN:
		case VK_BACK:
		case VK_DELETE:
			if(RemoveSelection(true) && (nChar != VK_RETURN))
				return;
			break;
		case VK_NEXT:
		case VK_PRIOR:
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_HOME:
		case VK_END:
			if(!shift)
				RemoveSelection(false);
			break;
	}
	switch(nChar)
	{
		case VK_NEXT:
		case VK_PRIOR:
			KeyPageY(nChar == VK_NEXT, shift);
			break;
		case VK_RETURN:
			KeyReturn(true);
			break;
		case VK_LEFT:
		case VK_RIGHT:
			KeyPosX(nChar == VK_LEFT, shift);
			break;
		case VK_UP:
		case VK_DOWN:
			KeyPosY(nChar == VK_UP, shift);
			break;
		case VK_BACK:
		case VK_DELETE:
			KeyRemove(nChar == VK_BACK, true);
			break;
		case VK_TAB:
			KeyTab(shift);
			break;
		case VK_HOME:
		case VK_END:
			KeyPageX((nChar == VK_HOME), shift, ctrl);
			break;
	}
}

void COgxTextEditorCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	COleControl::OnLButtonDblClk(nFlags, point);
	if(!(flags & OGXEF_DISABLE_SELECT_DBLCLK))
	{
		// выделить слово
		CRect rc(GetCoordWordFromPoint(point));
		// установить выделение
		SetSelection(CPoint(rc.left, rc.top), CPoint(rc.right, rc.bottom), true);
	}
	// отправить уведомление родителю
	TextDoubleClick(yc);
}

void COgxTextEditorCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	COleControl::OnLButtonDown(nFlags, point);
	if(!(::GetKeyState(VK_SHIFT) & 0x80))
		startSel = point;
	is_selection = true;
	SetFocus();
	SetSelection(GetCoordCaretFromPoint(startSel), GetCoordCaretFromPoint(point), true);
	SetCapture();
}

void COgxTextEditorCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	COleControl::OnLButtonUp(nFlags, point);
	if(is_selection)
	{
		SetSelection(GetCoordCaretFromPoint(startSel), GetCoordCaretFromPoint(point), false);
		ReleaseCapture();
		is_selection = false;
	}
}

void COgxTextEditorCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	COleControl::OnMouseMove(nFlags, point);
	if(is_selection)
	{
		SetSelection(GetCoordCaretFromPoint(startSel), GetCoordCaretFromPoint(point), false);
	}
}

BOOL COgxTextEditorCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	OnVScroll(SB_THUMBTRACK, asmCnvRealToUnreal(this, yoffs) + (zDelta < WHEEL_DELTA ? 4 : -4), NULL);
	return COleControl::OnMouseWheel(nFlags, zDelta, pt);
}

void COgxTextEditorCtrl::OnSize(UINT nType, int cx, int cy)
{
	COleControl::OnSize(nType, cx, cy);
	int xp(cx / grp_fonts->wnf);
	int yp(cy / grp_fonts->hnf);
	if(xpage != xp || ypage != yp)
	{
		xpage = xp;
		ypage = yp;
		UpdateCaret(pcUpdate | pcChange);
	}
}

void COgxTextEditorCtrl::OnKillFocus(CWnd* pNewWnd)
{
	HideCaret();
	DestroyCaret();
	Invalidate();
}

void COgxTextEditorCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CreateSolidCaret(2, grp_fonts->hnf);
	ShowCaret();
	Invalidate();
}

CRect COgxTextEditorCtrl::GetCoordWordFromPoint(const CPoint& pt)
{
	int x1, x2;
	// необходимо определить вертикальное положение каретки в зависимости от невидимых строк
	int xx((pt.x / grp_fonts->wnf) + xoffs);
	int yy(asmCnvScreenToReal(this, pt.y / grp_fonts->hnf));
	xx = asmIndexFromPosition(text[yy].line, xx);
	asmFindWord(text[yy].line, xx, &x1, &x2);
	return CRect(x1, yy, x2, yy);
}

CPoint COgxTextEditorCtrl::GetCoordCaretFromPoint(const CPoint& pt)
{
	int xx = (pt.x / grp_fonts->wnf) + xoffs;
	int yy = asmCnvScreenToReal(this, pt.y / grp_fonts->hnf);
	return CPoint(asmIndexFromPosition(text[yy].line, xx), yy);
}

bool COgxTextEditorCtrl::RemoveSelection(bool clear, bool undo)
{
	if(!(flags & OGXEF_READONLY))
	{
		if(i1_sel != i2_sel)
		{
			if(clear)
			{
				CString txt(copy());
				long status;

				is_modify = true;

				int xx1Sel = x1_sel , xx2Sel = x2_sel;
				int yy1Sel = y1_sel , yy2Sel = y2_sel;

				if(y1_sel != y2_sel)
				{
					if(yy1Sel > yy2Sel)
					{
						int ii = yy1Sel;
						yy1Sel = yy2Sel;
						yy2Sel = ii;
					}
					CString ln(text[yy1Sel].line.Left(xx1Sel) + text[yy2Sel].line.Mid(xx2Sel));
					status = text[yy1Sel].status;
					text.RemoveAt(yy1Sel, (yy2Sel - yy1Sel) + 1);
					text.InsertAt(yy1Sel, LINE_TEXT(ln, status));
				}
				else
				{
					if(xx1Sel > xx2Sel)
					{
						int ii = xx1Sel;
						xx1Sel = xx2Sel;
						xx2Sel = ii;
					}
					CString ln(text[yy1Sel].line.Left(xx1Sel) + text[yy1Sel].line.Mid(xx2Sel));
					text[yy1Sel].line = ln;
				}
				if(undo)
				{
					SetUndo(urDeleteSel, xx1Sel, yy1Sel, txt);
				}
				yc = yy1Sel;
				xc = xx1Sel;
				TextChange(y1_sel, y2_sel, OGX_ACT_REMOVE_SEL);
				UpdateCaret(pcPosition | pcOffset | pcUpdate | pcReset | pcChange);
				return true;
			}
		}
	}

	return false;
}

void COgxTextEditorCtrl::SetSelection(const CPoint& pt1, const CPoint& pt2, bool begin)
{
	xc = pt2.x;
	yc = pt2.y;

	if(begin)
	{
		y1_sel = pt1.y;
		x1_sel = pt1.x;
		i1_sel = asmGetOffsetLine(this, y1_sel) + x1_sel;
	}
	x2_sel = pt2.x;
	y2_sel = pt2.y;
	i2_sel = asmGetOffsetLine(this, y2_sel) + x2_sel;
	UpdateCaret(pcPosition | pcUpdate | pcChange);
}

void COgxTextEditorCtrl::KeyPosX(bool left, bool shift)
{
	if(left)
	{
		if(xc)
		{
			xc--;
		}
		else if(yc)
		{
			yc = asmGotoLine(this, yc, gtPrev);
			xc = text[yc].line.GetLength();
		}
	}
	else
	{
		if(xc < text[yc].line.GetLength())
		{
			xc++;
		}
		else if(yc < text.GetSize())
		{
			xc = 0;
			yc = asmGotoLine(this, yc, gtNext);
		}
	}
	UpdateCaret(pcPosition | pcOffset | pcUpdate | pcShift * shift | pcReset);
}

void COgxTextEditorCtrl::KeyPosY(bool up, bool shift)
{
	if(up)
	{
		if(yc)
		{
			yc = asmGotoLine(this, yc, gtPrev);
			xc = min(asmIndexFromPosition(text[yc].line, xcl), text[yc].line.GetLength());
		}
		else
		{
			xc = 0;
		}
	}
	else if(yc < text.GetSize())
	{
		yc = asmGotoLine(this, yc, gtNext);
		xc = min(asmIndexFromPosition(text[yc].line, xcl), text[yc].line.GetLength());
	}
	else
	{
		xc = text[yc].line.GetLength();
	}
	UpdateCaret(pcOffset | pcUpdate | pcShift * shift | pcReset | pcChange);
}

void COgxTextEditorCtrl::KeyPageX(bool home, bool shift, bool ctrl)
{
	if(ctrl)
	{
		if(home)
		{
			yc = 0;
			xc = 0;
		}
		else
		{
			yc = text.GetSize() - 1;
			xc = text[yc].line.GetLength();
		}
	}
	else
	{
		if(home)
		{
			CString ln(text[yc].line);
			int len = ln.GetLength();
			ln.TrimLeft();
			len -= ln.GetLength();
			xc = (xc == len ? 0 : len);
		}
		else
		{
			xc = text[yc].line.GetLength();
		}
	}
	UpdateCaret(pcPosition | pcOffset | pcUpdate | pcShift * shift | pcReset | pcChange * ctrl);
}

void COgxTextEditorCtrl::KeyPageY(bool next, bool shift)
{
	yc = asmGotoLine(this, yc, next ? gtNextPage : gtPrevPage);
	if(next)
	{
		if(yc < text.GetSize())
		{
			xc = min(asmIndexFromPosition(text[yc].line, xcl), text[yc].line.GetLength());
		}
		else
		{
			yc = text.GetSize() - 1;
			xc = text[yc].line.GetLength();
		}
	}
	else
	{
		if(yc < 0)
		{
			xc = 0;
			yc = 0;
		}
		else
		{
			xc = min(asmIndexFromPosition(text[yc].line, xcl), text[yc].line.GetLength());
		}
	}
	UpdateCaret(pcOffset | pcUpdate | pcShift * shift | pcReset | pcChange);
}

void COgxTextEditorCtrl::KeyRemove(bool back, bool undo)
{
	if(!(flags & OGXEF_READONLY))
	{
		int x = xc, y = yc;

		CString txt, ln, ln1;

		if(back)
		{
			if(xc)
			{
				xc--;
				txt = text[yc].line.GetAt(xc);
				text[yc].line.Delete(xc);
			}
			else if(yc)
			{
				ln = text[yc].line;
				text.RemoveAt(yc);
				yc--;
				ln1 = text[yc].line;
				text[yc].line = (ln1 + ln);
				xc = ln1.GetLength();
			}
		}
		else
		{
			if(xc < text[yc].line.GetLength())
			{
				txt = text[yc].line.GetAt(xc);
				text[yc].line.Delete(xc);
			}
			else if(yc < text.GetSize())
			{
				ln = text[yc].line;
				text.RemoveAt(yc);
				ln1 = text[yc].line;
				text[yc].line = (ln + ln1);
			}
		}
		if(undo)
		{
			SetUndo(back ? urRemove : urDelete, x, y, txt);
		}
		TextChange(y, y, OGX_ACT_REMOVE);
		is_modify = true;
		UpdateCaret(pcPosition | pcOffset | pcUpdate | pcReset | pcChange);
	}
}

void COgxTextEditorCtrl::KeyReturn(bool undo)
{
	if(!(flags & OGXEF_READONLY))
	{
		long status;
		CString ln(text[yc].line);

		text[yc].line = ln.Left(xc);
		status = text[yc].status;
		text[yc].status &= ~(SSL_NODE | SSL_NODEE);
		text.InsertAt(yc + 1, LINE_TEXT(ln.Mid(xc), status));
		if(undo)
		{
			SetUndo(urReturn, 0, yc + 1, "");
		}
		TextChange(yc, yc + 1, OGX_ACT_INSERT);
		yc++ ; xc = 0;
		is_modify = true;
		UpdateCaret(pcPosition | pcOffset | pcUpdate | pcReset);
	}
}

void COgxTextEditorCtrl::KeyTab(bool shift)
{
	if(!(flags & OGXEF_READONLY))
	{
		SetUndo(urInsert, xc + 1, yc, "\t");
		is_modify = true;
		text[yc].line.Insert(xc, '\t');
		xc++;
		TextChange(yc, yc, OGX_ACT_INSERT);
		UpdateCaret(pcPosition | pcOffset | pcUpdate | pcShift * shift | pcReset);
	}
}

void COgxTextEditorCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
}

void COgxTextEditorCtrl::OnResetState()
{
	COleControl::OnResetState();
}

void COgxTextEditorCtrl::SetUndo(int type, int x, int y, const CString& txt)
{
	UNDO_REDO* ur;
	// удалить все повторы отмен
	for(int i = 0 ; i < redo.GetSize() ; i++)
		delete redo[i];
	redo.RemoveAll();

	ur = new UNDO_REDO;
	ur->type = type;
	ur->x = xc ; ur->x1 = x;
	ur->y = yc ; ur->y1 = y;

	ur->i1_sel = i1_sel;
	ur->i2_sel = i2_sel;

	ur->x1_sel = x1_sel;
	ur->x2_sel = x2_sel;
	ur->y1_sel = y1_sel;
	ur->y2_sel = y2_sel;

	ur->text = txt;

	if(undo.GetSize() > 64)
	{
		delete undo[0];
		undo.RemoveAt(0);
	}
	undo.Add(ur);
}

void COgxTextEditorCtrl::UpdateCaret(long flags)
{
	int x(asmPositionFromIndex(text[yc].line, xc));
	int y_offs(asmCnvRealToUnreal(this, yoffs));

	if(flags & pcOffset)
	{
		// просто определяем каретка вне экрана, и если, да, то сдвинуть экран
		int xxo = xoffs + xpage;
		int yyo = asmCnvRealToUnreal(this, asmGotoLine(this, yoffs, gtNextPage));
		int yyc = asmCnvRealToUnreal(this, yc);

		if(yyc < y_offs)
		{
			y_offs = yyc;
		}
		else if(yyc >= yyo)
		{
			y_offs = asmGotoLine(this, yc, gtPrevPage);
			y_offs = asmGotoLine(this, y_offs, gtNext);
			y_offs = asmCnvRealToUnreal(this, y_offs);
		}
		if(x < xoffs)
		{
			xoffs = x - 2;
			if(xoffs < 0)
				xoffs = 0;
		}
		else if(x >= xxo)
		{
			xoffs = (x - xpage) + 1;
		}
	}
	if(flags & pcShift)
	{
		x2_sel = xc;
		y2_sel = yc;
		i2_sel = asmGetOffsetLine(this, yc) + xc;
	}
	else if(flags & pcReset)
	{
		x1_sel = x2_sel = xc;
		y1_sel = y1_sel = yc;
		i1_sel = i2_sel = asmGetOffsetLine(this, yc) + xc;
	}

	UpdateScroll(SB_VERT, y_offs, asmCnvRealToUnreal(this, text.GetSize()), ypage);
	UpdateScroll(SB_HORZ, xoffs, 256, xpage);
	yoffs = asmCnvUnrealToReal(this, y_offs);
	if(flags & pcChange)
	{
		TextChange(yoffs, yoffs, OGX_ACT_VSCROLL);
	}

	if(flags & pcPosition)
		xcl = x;
	if(flags & pcUpdate)
	{
		Invalidate();
	}
	SetCaretPos(CPoint(asmGetScreenX(this, yc, x) - xoffs * grp_fonts->wnf, asmCnvRealToScreen(this, yc)));
}

CString COgxTextEditorCtrl::copy() const
{
	CString ln;
	// сформировать строку из выделенного
	int xx1Sel = x1_sel, xx2Sel = x2_sel;
	int yy1Sel = y1_sel, yy2Sel = y2_sel;

	if(y1_sel != y2_sel)
	{
		if(yy1Sel > yy2Sel)
		{
			int ii = yy1Sel;
			yy1Sel = yy2Sel;
			yy2Sel = ii;
		}
		ln = (text[yy1Sel].line.Mid(xx1Sel) + "\r\n");
		int count = (yy2Sel - yy1Sel) - 1;
		for(int i = 0 ; i < count ; i++)
			ln += text[yy1Sel + 1 + i].line + "\r\n";
		ln += text[yy2Sel].line.Left(xx2Sel);
	}
	else
	{
		if(xx1Sel > xx2Sel)
		{
			int ii = xx1Sel;
			xx1Sel = xx2Sel;
			xx2Sel = ii;
		}
		ln = (text[yy1Sel].line.Mid(xx1Sel, xx2Sel - xx1Sel));
	}
	return ln;
}

void COgxTextEditorCtrl::paste(const CString& buffer)
{
	int pos = 0, x = xc, y = yc, p;
	bool finish(false);

	while(!finish)
	{
		p = buffer.Find("\r\n", pos);
		if(finish = (p == -1)) p = buffer.GetLength();
		CString s(buffer.Mid(pos, p - pos));
		if(finish)
		{
			text[yc].line.Insert(xc, s);
			xc += s.GetLength();
		}
		else
		{
			long status(text[yc].status);
			if(status & SSL_NODE) status &= ~(SSL_NODE | SSL_NODEE);
			CString ln(text[yc].line.Mid(xc));
			text[yc].line = text[yc].line.Left(xc) + s;
			yc++;
			text.InsertAt(yc, LINE_TEXT(ln, status));
			xc = 0;
		}
		pos = p + 2;
	}
	TextChange(y, yc, OGX_ACT_INSERT_SEL);
}

void COgxTextEditorCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if(pdc)
	{
		CRect rect_client;
		const char* lex;
		int hgdi = -1;
		long color = -1, bk_color = -1;
		int xx, iLine, yy = 0, lenLex, cx, cy, niLine, i;
		GetClientRect(rect_client);
		xpage = rect_client.Width() / grp_fonts->wnf;
		ypage = rect_client.Height() / grp_fonts->hnf;
		cx = (xpage + 1) * grp_fonts->wnf;
		cy = (ypage + 1) * grp_fonts->hnf;
		iLine = yoffs;
		pdc->SelectObject(pen_cur);
		for(i = 0 ; i <= ypage ; i++)
		{
			if(iLine >= text.GetSize())
				break;
			bk_color = -1;
			// преобразуем текущую линию в последовательность лексем
			// заменяем табуляторы и перенос строки пробелами
			xx = -(xoffs * grp_fonts->wnf);
			if((lex = asmParsing(this, iLine, TRUE)))
			{
				// проходим по преобразованной строке и отрисовываем ее
				while(true)
				{
					if(!(lenLex = *(WORD*)(lex)))
						break;
					if(color != *(long*)(lex + 4))
					{
						color = *(long*)(lex + 4);
						pdc->SetTextColor(color);
					}
					if(bk_color != *(long*)(lex + 8))
					{
						bk_color = *(long*)(lex + 8);
						pdc->SetBkColor(bk_color);
					}
					if(hgdi != *(BYTE*)(lex + 3))
					{
						hgdi = *(BYTE*)(lex + 3);
						pdc->SelectObject(grp_fonts->hFont[hgdi]);
					}
					pdc->TextOut(xx, yy, (lex + 12), lenLex);
					xx += lenLex * *(BYTE*)(lex + 2);
					lex += (lenLex + 12);
				}
			}
			// достигли края экрана ?
			if(xx < cx)
				pdc->FillSolidRect(xx, yy, cx - xx, grp_fonts->hnf, std_group->c_bk_text);
			// если эта строка является текущей и у нас установлен флаг выделения текущей то:
			if(flags & OGXEF_SELECT_CURRENT_LINE && yc == iLine && GetFocus() == this)
			{
				pdc->MoveTo(0, yy);
				pdc->LineTo(cx - 1, yy);
				pdc->LineTo(cx - 1, yy + grp_fonts->hnf - 1);
				pdc->LineTo(0, yy + grp_fonts->hnf - 1);
				pdc->LineTo(0, yy);
			}
			yy += grp_fonts->hnf;
			niLine = asmGotoLine(this, iLine, gtNext);
			if(niLine == iLine)
				break;
			iLine = niLine;
		}
		// достигли низа экрана ?
		if(yy < cy)
			pdc->FillSolidRect(0, yy, cx, cy - yy, std_group->c_bk_text);
	}
}

void COgxTextEditorCtrl::ApplyUndoRedo(bool undo, UNDO_REDO* ur)
{
	CString txt(ur->text);

	int xx=ur->x;
	int yy=ur->y;

	i1_sel = ur->i1_sel;
	i2_sel = ur->i2_sel;
	x1_sel = ur->x1_sel;
	x2_sel = ur->x2_sel;
	y1_sel = ur->y1_sel;
	y2_sel = ur->y2_sel;

	switch(ur->type)
	{
		case urDelete:
			if(ur->text != "")
			{
				if(undo)
				{
					text[yy].line.Insert(xx, ur->text);
				}
				else
				{
					xc = xx ; yc = yy;
					KeyReturn(false);
				}
			}
			else
			{
				if(undo)
				{
					text[yy].line.Delete(xx, ur->text.GetLength());
				}
				else
				{
					xc = xx ; yc = yy;
					KeyRemove(false, false);
				}
			}
			break;
		case urRemove:
			if(ur->text != "")
			{
				if(undo)
				{
					text[ur->y1].line.Insert(ur->x1, ur->text);
				}
				else
				{
					text[ur->y1].line.Delete(ur->x1, ur->text.GetLength());
				}
			}
			else
			{
				if(undo)
				{
					xc = ur->x1 ; yc = ur->y1;
					KeyReturn(false);
				}
				else
				{
					xc = xx ; yc = yy;
					KeyRemove(true, false);
				}
			}
			break;
		case urReturn:
			if(undo)
			{
				xc = ur->x1 ; yc = ur->y1;
				KeyRemove(true, false);
			}
			else
			{
				xc = xx ; yc = yy;
				KeyReturn(false);
			}
			break;
		case urDeleteSel:
			if(undo)
			{
				xc = ur->x1 ; yc = ur->y1;
				paste(ur->text);
			}
			else
			{
				RemoveSelection(true, false);
			}
			break;
		case urInsert:
			if(undo)
			{
				text[yy].line.Delete(xx, ur->text.GetLength());
			}
			else
			{
				text[yy].line.Insert(xx, ur->text);
			}
			break;
	}

	if(undo)
	{
		xc = xx;
		yc = yy;
	}
	else
	{
		xc = ur->x1;
		yc = ur->y1;
	}

	UpdateCaret(pcPosition | pcOffset | pcUpdate | pcChange);
}

LONG COgxTextEditorCtrl::GetLineWords(LONG y, OLE_HANDLE array)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const char* lex(asmGetWords(this, y));
	CArray<LEXEM_TYPE>* arr = (CArray<LEXEM_TYPE>*)array;
	arr->RemoveAll();
	while(true)
	{
		WORD len = *(WORD*)(lex);
		WORD tp = *(WORD*)(lex + 2);
		if(len == 0)
			break;
		arr->Add(LEXEM_TYPE(CString(lex + 4, len), tp));
		lex += (len + 4);
	}
	return arr->GetSize();
}

void COgxTextEditorCtrl::SetStatusNode(LONG line, LONG status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(text[line].status & SSL_NODE)
	{
		text[line].status &= ~(SSL_NODE | SSL_NODEE);
		text[line].status |= status;
		line++;
		long y = line;
		while(y < text.GetSize())
		{
			if(!(text[y].status & SSL_ELEM))
				break;
			text[y].status = (status & SSL_NODEE ? SSL_ELEM | SSL_VIS : SSL_ELEM);
			y++;
		}
		// если каретка попала в область изменений и статус свернут
		if((yc >= line && yc < y) && (status & SSL_NODE))
		{
			yc = asmGotoLine(this, yc, gtPrev);
			int x = text[yc].line.GetLength();
			if(xc > x)
				xc = x;
		}
		UpdateCaret(pcUpdate | pcChange);
	}
}

BOOL COgxTextEditorCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, ::LoadCursor(NULL, IDC_IBEAM), reinterpret_cast<HBRUSH>(COLOR_WINDOW));
	return COleControl::PreCreateWindow(cs);
}

void COgxTextEditorCtrl::SetFontParams(OLE_HANDLE grp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	grp_fonts = (GROUP_FONTS*)grp;
	if(IsWindow(m_hWnd))
		Invalidate();
}

void COgxTextEditorCtrl::AddString(LPCTSTR line, LONG status)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	text.Add(LINE_TEXT(line, status));
	if(IsWindow(m_hWnd))
		Invalidate();
}

void COgxTextEditorCtrl::GetCaretPosition(LONG* x, LONG* y)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(x) *x = xc;
	if(y) *y = yc;
}

void COgxTextEditorCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CPoint pt(GetCoordCaretFromPoint(point));
	// если нажали внутри выделения, то не сбрасывать его
	if(IsTextCopy())
	{
		int index(asmGetOffsetLine(this, pt.y) + pt.x);
		if(index >= i1_sel && index < i2_sel)
			return;
	}
	startSel = point;
	SetSelection(pt, pt, true);
	SetFocus();
	COleControl::OnRButtonDown(nFlags, point);
}

void COgxTextEditorCtrl::FormatText(LONG is_update)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	for(int i = 0 ; i < text.GetSize() ; i++)
		text[i].line = asmFormatLine(text[i].line);
	if(is_update)
		UpdateAll(0);
	is_modify = true;
}

void COgxTextEditorCtrl::SetCaretPosition(LONG x, LONG y, LONG is_update)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(y < 0) y = 0;
	else if(y >= text.GetSize()) y = (text.GetSize() - 1);
	if(x < 0) x = 0;
	if(x >= text[y].line.GetLength()) x = (text[y].line.GetLength() - 1);
	xc = x;
	yc = y;
	if(is_update)
	{
		startSel.x = 0;
		startSel.y = y;
		UpdateCaret(pcPosition | pcOffset | pcReset | pcChange | pcUpdate);
	}
}
