
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "PanelOutputView.h"

BEGIN_MESSAGE_MAP(CPanelOutputView, CDockablePane)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLEAR_CONTENT, &CPanelOutputView::OnClearContent)
	ON_COMMAND(ID_COPY, &CPanelOutputView::OnCopy)
	ON_UPDATE_COMMAND_UI(ID_CLEAR_CONTENT, &CPanelOutputView::OnUpdateIsText)
	ON_UPDATE_COMMAND_UI(ID_COPY, &CPanelOutputView::OnUpdateIsText)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CPanelOutputView, CDockablePane)
	ON_EVENT(CPanelOutputView, 4, 2, TextDoubleClick, VTS_I4)
END_EVENTSINK_MAP()

void CPanelOutputView::TextDoubleClick(long y_line)
{
	//	взять строку из позиции
	CString str(ogx.GetString(y_line));
	//	найти номер строки и файл (файл(номер):текст
	int pos(str.Find('('));
	if(pos != -1)
	{
		// открыть документ
		CString path(str.Left(pos));
		pos++;
		int pos1(str.Find(')', pos));
		if(pos1 != -1)
		{
			if(theApp.frm->GotoDocument(path, atol(str.Mid(pos, pos1 - pos))))
				theApp.frm->GetStatusBar()->SetInformation(str);
		}
	}
}

void CPanelOutputView::OnPaint()
{
	CPaintDC dc(this);
	CRect rectWnd;
	ogx.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	rectWnd.InflateRect(1, 1);
	dc.Draw3dRect(rectWnd, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPanelOutputView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

int CPanelOutputView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	CConfiguration* cfg(theApp.frm->GetConfiguration());
	// окно редактора
	if(!ogx.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 1, 1), this, 4))
		return -1;
	ogx.Initialize(0, 0, LANG_TXT, OGXEF_READONLY | OGXEF_DISABLE_SELECT_DBLCLK | OGXEF_SELECT_CURRENT_LINE);
	ogx.SetStdGroup((long)cfg->GetStdGroup(TE_OUTPUT));
	ogx.SetFontParams((long)cfg->GetGroupFonts(TE_OUTPUT));
	AdjustLayout();

	return 0;
}

void CPanelOutputView::AdjustLayout()
{
	if(GetSafeHwnd())
	{
		CRect rectClient;
		GetClientRect(rectClient);
		ogx.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelOutputView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	ogx.SetFocus();
}

void CPanelOutputView::UpdateOptions()
{
	ogx.Invalidate();
}

void CPanelOutputView::SetTitle(const char* txt)
{
	ogx.SetText("", SSL_VIS);
	ogx.AddString(txt, SSL_VIS);
	ogx.AddString("", SSL_VIS);
	theApp.frm->ShowPane(this, TRUE, TRUE, TRUE);
}
void CPanelOutputView::AddString(const char* txt)
{
	ogx.AddString(txt, SSL_VIS);
	ogx.SetCaretPosition(0, ogx.GetCountLines(), FALSE);
}

void CPanelOutputView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_POPUP_OUTPUT, point.x, point.y, this, TRUE);
}

void CPanelOutputView::OnClearContent()
{
	ogx.SetText("", SSL_VIS);
	theApp.frm->GetStatusBar()->SetInformation(NULL);
}

void CPanelOutputView::OnCopy()
{
	// поместить в буфер обмена
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;

		if(EmptyClipboard())
		{
			WORD locale(0x419);
			CString text(ogx.GetText());
			// сначала поместить 0x419 - CF_LOCALE
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, 2);
			memcpy(GlobalLock(clipbuffer), &locale, 2);
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_LOCALE, clipbuffer);
			// поместить в буфер обмена
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.GetLength() + 1);
			memcpy(GlobalLock(clipbuffer), (const char*)text, text.GetLength());
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
	}
}

void CPanelOutputView::OnUpdateIsText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ogx.GetCountLines() > 1);
}

void CPanelOutputView::Reset()
{
	OnClearContent();
}
