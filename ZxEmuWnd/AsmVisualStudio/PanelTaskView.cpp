
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "PanelTaskView.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_LIST_VIEW		1

BEGIN_MESSAGE_MAP(CPanelTaskView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_NOTIFY(NM_DBLCLK, ID_LIST_VIEW, &CPanelTaskView::OnListDblClick)
	ON_COMMAND(ID_TASK_ERRORS, &CPanelTaskView::OnTaskErrors)
	ON_COMMAND(ID_COPY, &CPanelTaskView::OnCopy)
	ON_COMMAND(ID_TASK_INFORMATIONS, &CPanelTaskView::OnTaskInformations)
	ON_COMMAND(ID_TASK_WARNINGS, &CPanelTaskView::OnTaskWarnings)
	ON_COMMAND(ID_CLEAR_CONTENT, &CPanelTaskView::OnClearContent)
	ON_COMMAND(ID_GOTO_LOCATION, &CPanelTaskView::OnGotoLocation)
	ON_COMMAND(ID_GOTO_NEXT_LOCATION, &CPanelTaskView::OnGotoNextLocation)
	ON_COMMAND(ID_GOTO_PREV_LOCATION, &CPanelTaskView::OnGotoPrevLocation)
	ON_UPDATE_COMMAND_UI(ID_TASK_ERRORS, &CPanelTaskView::OnUpdateTaskErrors)
	ON_UPDATE_COMMAND_UI(ID_TASK_INFORMATIONS, &CPanelTaskView::OnUpdateTaskInfo)
	ON_UPDATE_COMMAND_UI(ID_TASK_WARNINGS, &CPanelTaskView::OnUpdateTaskWarnings)
	ON_UPDATE_COMMAND_UI(ID_GOTO_LOCATION, &CPanelTaskView::OnUpdateTaskPanel)
	ON_UPDATE_COMMAND_UI(ID_CLEAR_CONTENT, &CPanelTaskView::OnUpdateTaskPanel)
	ON_UPDATE_COMMAND_UI(ID_GOTO_NEXT_LOCATION, &CPanelTaskView::OnUpdateTaskNextLocation)
	ON_UPDATE_COMMAND_UI(ID_GOTO_PREV_LOCATION, &CPanelTaskView::OnUpdateTaskPrevLocation)
	ON_UPDATE_COMMAND_UI(ID_COPY, &CPanelTaskView::OnUpdateTaskPanel)
	ON_LBN_SELCHANGE(1, &CPanelTaskView::OnLbnSelchangeTaskList)
	ON_LBN_DBLCLK(1, &CPanelTaskView::OnLbnDblclkTaskList)
END_MESSAGE_MAP()

void CPanelTaskView::OnListDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMITEMACTIVATE* lpnm = (NMITEMACTIVATE*)pNMHDR;
	cur_loc = lpnm->iItem;
	OnGotoLocation();
	*pResult = 0;
}

CPanelTaskView::CPanelTaskView() : nMode(-1), cur_loc(-1) {}

int CPanelTaskView::GetListItemSel()
{
	int index(0);
	POSITION pos(m_wndList.GetFirstSelectedItemPosition());
	while(pos)
	{
		index = m_wndList.GetNextSelectedItem(pos);
	}

	return index;
}
void CPanelTaskView::OnLbnDblclkTaskList()
{
	cur_loc = GetListItemSel();
	OnGotoLocation();
}
void CPanelTaskView::OnLbnSelchangeTaskList()
{
	cur_loc = GetListItemSel();
}

void CPanelTaskView::OnUpdateTaskNextLocation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(cur_loc < m_wndList.GetItemCount() && cur_loc != -1);
}

void CPanelTaskView::OnUpdateTaskPrevLocation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(cur_loc > 0);
}

void CPanelTaskView::OnUpdateTaskErrors(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(nMode == modeError);
}

void CPanelTaskView::OnUpdateTaskWarnings(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(nMode == modeWarning);
}

void CPanelTaskView::OnUpdateTaskInfo(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(nMode == modeInfo);
}

int CPanelTaskView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	out = theApp.frm->GetPanelOutput();
	m_font.CreateStockObject(DEFAULT_GUI_FONT);
	if(!m_wndList.Create(LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | WS_CHILD | WS_VISIBLE, CRect(0, 0, 1, 1), this, 1))
		return -1;
	m_wndList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_wndList.InsertColumn(ctDesc, "Описание", LVCFMT_LEFT, 600);
	m_wndList.InsertColumn(ctDoc, "Документ", LVCFMT_LEFT, 500);
	m_wndList.InsertColumn(ctNum, "Строка", LVCFMT_LEFT, 100);
	m_wndList.SetFont(&m_font);
	m_ToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_TOOLBAR_TASK);
	m_ToolBar.LoadToolBar(IDR_TOOLBAR_TASK, 0, 0, TRUE);
	m_ToolBar.SetOwner(this);
	m_ToolBar.SetRouteCommandsViaFrame(FALSE);
	AdjustLayout();
	return 0;
}

void CPanelTaskView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPanelTaskView::AdjustHorzScroll()
{
	m_wndList.SetColumnWidth(ctDesc, LVSCW_AUTOSIZE_USEHEADER);
	m_wndList.SetColumnWidth(ctDoc, LVSCW_AUTOSIZE_USEHEADER);
	m_wndList.SetColumnWidth(ctNum, LVSCW_AUTOSIZE_USEHEADER);
}

void CPanelTaskView::AdjustLayout()
{
	if(GetSafeHwnd())
	{
		GetClientRect(rectClient);
		int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		m_ToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndList.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelTaskView::OnPaint()
{
	CPaintDC dc(this);
	CRect rectList;
	m_wndList.GetWindowRect(rectList);
	ScreenToClient(rectList);
	rectList.InflateRect(1, 1);
	dc.Draw3dRect(rectList, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPanelTaskView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndList.SetFocus();
}

void CPanelTaskView::OnCopy()
{
	// поместить в буфер обмена
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;

		if(EmptyClipboard())
		{
			CString str, text;
			int i, count(m_wndList.GetItemCount());
			for(i = 0 ; i < count ; i++)
				text += (m_wndList.GetItemText(i, ctDesc) + " " + m_wndList.GetItemText(i, ctDoc) + " " + m_wndList.GetItemText(i, ctNum) + "\r\n");
			WORD locale(0x419);
			// сначала поместить 0x419 - CF_LOCALE
			clipbuffer = GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, 2);
			memcpy(GlobalLock(clipbuffer), &locale, 2);
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_LOCALE, clipbuffer);
			// поместить в буфер обмена
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.GetLength() + 1);
			strcpy((char*)GlobalLock(clipbuffer), (const char*)text);
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
	}
}

void CPanelTaskView::OnTaskErrors()
{
	UpdateList(modeError, true);
}

void CPanelTaskView::OnTaskInformations()
{
	UpdateList(modeInfo, true);
}

void CPanelTaskView::OnTaskWarnings()
{
	UpdateList(modeWarning, true);
}

void CPanelTaskView::OnClearContent()
{
	CPanelOutputView* out(theApp.frm->GetPanelOutput());
	out->Reset();
	m_wndList.DeleteAllItems();
	cur_loc = -1;
	theApp.frm->GetStatusBar()->SetInformation(NULL);
}

void CPanelTaskView::OnGotoLocation()
{
	m_wndList.SetItemState(cur_loc, LVIS_SELECTED, LVIS_SELECTED);
	if(theApp.frm->GotoDocument(m_wndList.GetItemText(cur_loc, ctDoc), atol(m_wndList.GetItemText(cur_loc, ctNum))))
		theApp.frm->GetStatusBar()->SetInformation(m_wndList.GetItemText(cur_loc, ctDesc));
}

void CPanelTaskView::OnGotoNextLocation()
{
	cur_loc++;
	OnGotoLocation();
}

void CPanelTaskView::OnGotoPrevLocation()
{
	cur_loc--;
	OnGotoLocation();
}

void CPanelTaskView::OnUpdateTaskPanel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_wndList.GetItemCount() > 0);
}

void CPanelTaskView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_POPUP_TASK, point.x, point.y, this, TRUE);
}

void CPanelTaskView::Reset()
{
	OnClearContent();
}

int CPanelTaskView::UpdateList(int type, bool is_update)
{
	int count(out->ogx.GetCountLines()), i, result(0);
	nMode = type;
	m_wndList.DeleteAllItems();
	for(i = 0 ; i < count ; i++)
	{
		CString ln(out->ogx.GetString(i));
		switch(nMode)
		{
			case modeInfo:
				m_wndList.InsertItem(0, "111");
				m_wndList.SetItemText(0, ctDesc, "Sergey");
				m_wndList.SetItemText(0, ctDoc, "c:\\1.doc");
				m_wndList.SetItemText(0, ctNum, "1");
				break;
			case modeError:
				// TASM
				//E:\AsmVisualStudio\Projects\qqq\qqq\qqqMain.asm line 0001: unrecognized directive.            (.686)
				//E:\AsmVisualStudio\Projects\qqq\qqq\qqqMain.asm line 0012: Label not found: (start)
				//line 0015: No END directive before EOF.       
				// :*error*:
//				E:\AsmVisualStudio\Projects\Nine\Nine\NineMain.asm(11) : error A2070: invalid instruction operands
//				LINK : fatal error LNK1104: cannot open file "NineMain.obj"
//				NineMain.obj : error LNK2001: unresolved external symbol _GetInclude
//				E:\AsmVisualStudio\Projects\Nine\Nine\Debug\Nine.exe : fatal error LNK1120: 1 unresolved externals
				m_wndList.InsertItem(0, "111");
				m_wndList.SetItemText(0, ctDesc, "Sergey");
				m_wndList.SetItemText(0, ctDoc, "c:\\1.doc");
				m_wndList.SetItemText(0, ctNum, "1");
				break;
			case modeWarning:
//				LINK : warning LNK4096: /BASE value "10000" is invalid for Windows 95; image may not run
				m_wndList.InsertItem(0, "111");
				m_wndList.SetItemText(0, ctDesc, "Sergey");
				m_wndList.SetItemText(0, ctDoc, "c:\\1.doc");
				m_wndList.SetItemText(0, ctNum, "1");
				break;
		}
	}
	m_wndList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	cur_loc = 0;
	AdjustHorzScroll();
	return result;
}
