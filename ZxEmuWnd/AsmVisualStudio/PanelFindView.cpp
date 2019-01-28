
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "PanelFindView.h"

BEGIN_MESSAGE_MAP(CPanelFindView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COPY, &CPanelFindView::OnCopy)
	ON_COMMAND(ID_CLEAR_CONTENT, &CPanelFindView::OnClearContent)
	ON_COMMAND(ID_GOTO_LOCATION, &CPanelFindView::OnGotoLocation)
	ON_COMMAND(ID_GOTO_NEXT_LOCATION, &CPanelFindView::OnGotoNextLocation)
	ON_COMMAND(ID_GOTO_PREV_LOCATION, &CPanelFindView::OnGotoPrevLocation)
	ON_UPDATE_COMMAND_UI(ID_GOTO_PREV_LOCATION, &CPanelFindView::OnUpdatePrevLocation)
	ON_UPDATE_COMMAND_UI(ID_CLEAR_CONTENT, &CPanelFindView::OnUpdateIsText)
	ON_UPDATE_COMMAND_UI(ID_GOTO_LOCATION, &CPanelFindView::OnUpdateIsText)
	ON_UPDATE_COMMAND_UI(ID_GOTO_NEXT_LOCATION, &CPanelFindView::OnUpdateNextLocation)
	ON_UPDATE_COMMAND_UI(ID_COPY, &CPanelFindView::OnUpdateIsText)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CPanelFindView, CDockablePane)
	ON_EVENT(CPanelFindView, 5, 2, TextDoubleClick, VTS_I4)
END_EVENTSINK_MAP()

void CPanelFindView::TextDoubleClick(long y_line)
{
	cur_loc = y_line;
	SendMessage(WM_COMMAND, MAKELONG(ID_GOTO_LOCATION, 0), 0);
}

void CPanelFindView::OnUpdateNextLocation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(cur_loc < (ogx.GetCountLines() - 1) && cur_loc != -1);
}

void CPanelFindView::OnUpdatePrevLocation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(cur_loc > 2);
}

int CPanelFindView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	CConfiguration* cfg(theApp.frm->GetConfiguration());
	// окно редактора
	if(!ogx.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 1, 1), this, 5))
		return -1;
	ogx.Initialize(0, 0, LANG_TXT, OGXEF_READONLY | OGXEF_DISABLE_SELECT_DBLCLK | OGXEF_SELECT_CURRENT_LINE);
	ogx.SetStdGroup((long)cfg->GetStdGroup(TE_FIND));
	ogx.SetFontParams((long)cfg->GetGroupFonts(TE_FIND));
	// панель инструментов
	m_ToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_TOOLBAR_FIND);
	m_ToolBar.LoadToolBar(IDR_TOOLBAR_FIND, 0, 0, TRUE);
	m_ToolBar.SetOwner(this);
	m_ToolBar.SetRouteCommandsViaFrame(FALSE);
	AdjustLayout();
	return 0;
}

void CPanelFindView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPanelFindView::OnPaint()
{
	CPaintDC dc(this);
	CRect rectWnd;
	ogx.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	rectWnd.InflateRect(1, 1);
	dc.Draw3dRect(rectWnd, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPanelFindView::AdjustLayout()
{
	if(GetSafeHwnd())
	{
		CRect rectClient;
		GetClientRect(rectClient);
		int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		m_ToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
		ogx.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelFindView::OnCopy()
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

void CPanelFindView::OnClearContent()
{
	ogx.SetText("", SSL_VIS);
	cur_loc = -1;
	theApp.frm->GetStatusBar()->SetInformation(NULL);
}

void CPanelFindView::OnGotoLocation()
{
	//	взять строку из позиции
	CString str(ogx.GetString(cur_loc));
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

void CPanelFindView::OnGotoNextLocation()
{
	cur_loc++;
	OnGotoLocation();
}

void CPanelFindView::OnGotoPrevLocation()
{
	cur_loc--;
	OnGotoLocation();
}

void CPanelFindView::OnUpdateIsText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ogx.GetCountLines() > 1);
}

void CPanelFindView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	ogx.SetFocus();
}

void CPanelFindView::UpdateOptions()
{
	ogx.Invalidate();
}

void CPanelFindView::SetTitle(const char* text)
{
	ogx.SetText("", SSL_VIS);
	ogx.AddString(text, SSL_VIS);
	ogx.AddString("", SSL_VIS);
	theApp.frm->ShowPane(this, TRUE, TRUE, TRUE);
}

void CPanelFindView::AddString(const CString& document, const CString& text_line, long num_line)
{
	String text;
	ogx.AddString(text.format("%s(%d):\t%s", document, num_line, text_line), SSL_VIS);
	ogx.SetCaretPosition(0, ogx.GetCountLines(), FALSE);
}

void CPanelFindView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_POPUP_FIND, point.x, point.y, this, TRUE);
}

void CPanelFindView::Reset()
{
	OnClearContent();
}

void CPanelFindView::SetFinish()
{
	String str;
	int count(ogx.GetCountLines() - 3);
	ogx.AddString("", SSL_VIS);
	ogx.AddString(str.format("Завершение операции. Итого - %i", count), SSL_VIS);
	ogx.AddString("", SSL_VIS);
}
