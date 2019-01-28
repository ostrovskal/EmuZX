
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "DocumentAsmEditor.h"
#include "ViewAsmEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CViewAsmEditor, CView)
BEGIN_MESSAGE_MAP(CViewAsmEditor, CView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COLLAPSE_ALL, &CViewAsmEditor::OnCollapseAll)
	ON_COMMAND(ID_EXPAND_ALL, &CViewAsmEditor::OnExpandAll)
	ON_COMMAND(ID_COMBOBOX_GOTO_PROCEDURE, &CViewAsmEditor::OnComboboxGotoProcedure)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void CViewAsmEditor::TextChange(long y1, long y2, long action)
{
	if(action == OGX_ACT_VSCROLL)
	{
		y1 = 0;
		count_nums = ogx->GetPageLines(num_lines, num_statuses);
		// добавить ректы узлов для управления
		nodes.RemoveAll();
		for(int i = 0 ; i < count_nums ; i++)
		{
			if(num_statuses[i] & SSL_NODE)
				nodes.Add(NODE_ELEMENT(num_lines[i], CRect(nWidthNumLines + 1, y1 + 1, nWidthNumLines + CE_WIDTH_TREE - 1, y1 + *hline - 1), num_statuses[i] && SSL_NODEE));
			y1 += *hline;
		}
		Invalidate();
	}
}

void CViewAsmEditor::OnDraw(CDC* dc)
{
	CString str;
	CPen pen;
	int y = cyTlb, x = nWidthNumLines, hline4, cy = rectClient.Height(), num;
	pen.CreatePen(PS_SOLID, 1, *colorLines);
	dc->SetBkMode(TRANSPARENT);
	dc->SetTextColor(*colorLines);
	dc->SelectObject(fontNumLines);
	dc->SelectObject(pen);
	for(int i = 0 ; i < count_nums ; i++)
	{
		hline4 = *hline / 4;
		// нарисовать номера строк
		num = num_lines[i] + 1;
		str.Format("%03i", num);
		CRect rc(0, y, nWidthNumLines + CE_WIDTH_TREE, y + *hline);
		CRect rc1(0, y, nWidthNumLines, y + *hline);
		// проверить на закладку
		if(num_statuses[i] & SSL_BMA)
		{
			bool is((num_statuses[i] & (SSL_BM | SSL_BMA)) != 0);
			dc->SetTextColor(*colorBookmark);
			if(is) dc->SelectObject(fontBookmarks);
			dc->FillSolidRect(rc, *colorBkBookmark);
			dc->DrawText(str, rc1, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			dc->SetTextColor(*colorLines);
			if(is) dc->SelectObject(fontNumLines);
		}
		else
		{
			dc->FillSolidRect(rc, *colorBk);
			dc->DrawText(str, rc1, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
		// рисуем дерево
		switch(num_statuses[i] & ~(SSL_BMA | SSL_BM | SSL_VIS))
		{
			case SSL_NODE:
				// квадрат
				dc->MoveTo(x + CE_WIDTH_TREE - CE_WIDTH_TREE / 4, y + hline4 - 1);
				dc->LineTo(x + CE_WIDTH_TREE / 4 - 1, y + hline4 - 1);
				dc->LineTo(x + CE_WIDTH_TREE / 4 - 1, y + *hline - hline4);
				dc->LineTo(x + CE_WIDTH_TREE - CE_WIDTH_TREE / 4, y + *hline - hline4);
				dc->LineTo(x + CE_WIDTH_TREE - CE_WIDTH_TREE / 4, y + hline4 - 1);
				// крестик
				dc->MoveTo(x + CE_WIDTH_TREE / 4 + 1, y + *hline / 2);
				dc->LineTo(x + CE_WIDTH_TREE - CE_WIDTH_TREE / 4 - 1, y + *hline / 2);
				dc->MoveTo(x + CE_WIDTH_TREE / 2, y + hline4 + 1);
				dc->LineTo(x + CE_WIDTH_TREE / 2, y + *hline - hline4 - 1);
				break;
			case SSL_NODEE:
				// квадрат
				dc->MoveTo(x + CE_WIDTH_TREE - CE_WIDTH_TREE / 4, y + hline4 - 1);
				dc->LineTo(x + CE_WIDTH_TREE / 4 - 1, y + hline4 - 1);
				dc->LineTo(x + CE_WIDTH_TREE / 4 - 1, y + *hline - hline4);
				dc->LineTo(x + CE_WIDTH_TREE / 2, y + *hline - hline4);
				dc->LineTo(x + CE_WIDTH_TREE / 2, y + *hline);
				dc->LineTo(x + CE_WIDTH_TREE / 2, y + *hline - hline4);
				dc->LineTo(x + CE_WIDTH_TREE - CE_WIDTH_TREE / 4, y + *hline - hline4);
				dc->LineTo(x + CE_WIDTH_TREE - CE_WIDTH_TREE / 4, y + hline4 - 1);
				// минус
				dc->MoveTo(x + CE_WIDTH_TREE / 4 + 1, y + *hline / 2);
				dc->LineTo(x + CE_WIDTH_TREE - CE_WIDTH_TREE / 4 - 1, y + *hline / 2);
				break;
			case SSL_ELEM:
				dc->MoveTo(x + CE_WIDTH_TREE / 2, y);
				if(!(num_statuses[i + 1] & SSL_ELEM))
				{
					dc->LineTo(x + CE_WIDTH_TREE / 2, y + *hline / 2);
					dc->LineTo(x + CE_WIDTH_TREE - CE_WIDTH_TREE / 4, y + *hline / 2);
				}
				else
				{
					dc->LineTo(x + CE_WIDTH_TREE / 2, y + *hline);
				}
				break;
		}
		y += *hline;
	}
	while(y < cy)
	{
		num++;
		// нарисовать номера строк
		str.Format("%03i", num);
		CRect rc(0, y, nWidthNumLines + CE_WIDTH_TREE, y + *hline);
		CRect rc1(0, y, nWidthNumLines, y + *hline);
		dc->FillSolidRect(rc, *colorBk);
		dc->DrawText(str, rc1, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		y += *hline;
	}
	dc->FillSolidRect(0, h_client + cyTlb, nWidthNumLines + CE_WIDTH_TREE, rectClient.Height() - h_client, RGB(255, 255, 255));
	pen.DeleteObject();
}

#ifdef _DEBUG
void CViewAsmEditor::AssertValid() const
{
	CView::AssertValid();
}

void CViewAsmEditor::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDocumentAsmEditor* CViewAsmEditor::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocumentAsmEditor)));
	return (CDocumentAsmEditor*)m_pDocument;
}
#endif //_DEBUG

void CViewAsmEditor::AdjustLayout()
{
	if(GetSafeHwnd())
	{
		cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		m_ToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
		if(ogx)
		{
			CRect rc;
			ogx->SetWindowPos(NULL, nWidthNumLines + CE_WIDTH_TREE, rectClient.top + cyTlb, rectClient.Width() - (nWidthNumLines + CE_WIDTH_TREE), rectClient.Height() - cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
			ogx->GetClientRect(rc);
			h_client = rc.Height();
		}
	}
}

IMPLEMENT_SERIAL(CViewEditorComboBoxButton, CMFCToolBarComboBoxButton, 1)

int CViewAsmEditor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	GROUP_FONTS* gf;
	STD_GROUP* std;

	if(CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_ToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_TOOLBAR_EDITOR);
	m_ToolBar.LoadToolBar(IDR_TOOLBAR_EDITOR, 0, 0, TRUE);
	m_ToolBar.SetOwner(this);
	m_ToolBar.SetRouteCommandsViaFrame(FALSE);
	m_ToolBar.ReplaceButton(ID_COMBOBOX_GOTO_PROCEDURE, CViewEditorComboBoxButton());
	m_ComboProc =  DYNAMIC_DOWNCAST(CViewEditorComboBoxButton, m_ToolBar.GetButton(0));
	CConfiguration* cfg(theApp.frm->GetConfiguration());
	doc = GetDocument();
	ogx = doc->GetOgx();
	ogx->Create(NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 1, 1), this, ID_OGX_CODE_EDITOR);
	ogx->Initialize(0, 0, LANG_ASM, OGXEF_SELECT_CURRENT_LINE);
	for(int i = 0 ; i < 8 ; i++)
		ogx->SetGroup(i, (long)cfg->GetCodeEditorGroup(i));
	std = cfg->GetStdGroup(TE_EDITOR);
	colorBookmark = (COLORREF*)&std->c_bookmark;
	colorBkBookmark = (COLORREF*)&std->c_bk_bookmark;
	colorBk = (COLORREF*)&std->c_bk_text;
	colorLines = (COLORREF*)&std->c_text;
	ogx->SetStdGroup((long)std);
	gf = cfg->GetGroupFonts(TE_EDITOR);
	hline = &gf->hnf;
	ogx->SetFontParams((long)gf);
	fontNumLines.CreateFont(-10, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, "Courier New");
	fontBookmarks.CreateFont(-12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, "Courier New");
	return 0;
}

BOOL CViewAsmEditor::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

void CViewAsmEditor::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	rectClient.SetRect(0, 0, cx, cy);
	AdjustLayout();
}

void CViewAsmEditor::OnLButtonDown(UINT nFlags, CPoint pt)
{
	for(int i = 0 ; i < nodes.GetSize() ; i++)
	{
		if(nodes[i].rect.PtInRect(pt))
		{
			ogx->SetStatusNode(nodes[i].line, (nodes[i].collapse ? SSL_NODE : SSL_NODEE));
			break;
		}
	}
	CView::OnLButtonDown(nFlags, pt);
}

BOOL CViewAsmEditor::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN;
	cs.style &= ~WS_VISIBLE;
	return CView::PreCreateWindow(cs);
}

void CViewAsmEditor::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CView::OnUpdate(pSender, lHint, pHint);
	if(ogx) ogx->UpdateAll(0);
}

void CViewAsmEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_POPUP_CODE_EDITOR, point.x, point.y, this, TRUE);
}

void CViewAsmEditor::OnCollapseAll()
{
}

void CViewAsmEditor::OnExpandAll()
{
}

void CViewAsmEditor::OnComboboxGotoProcedure()
{
	CString name(m_ComboProc->GetItem(m_ComboProc->GetCurSel()));
	if(doc->elem)
	{
		CElement::BLOCK_ELEMENT* blk(doc->elem->GetBlock(name, CElement::blkProc));
		if(blk)
		{
			ogx->GotoLine(blk->line_begin, TRUE);
		}
	}
}

void CViewAsmEditor::OnSetFocus(CWnd* pOldWnd)
{
	if(ogx) ogx->SetFocus();
}

void CViewAsmEditor::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if(bActivate && !theApp.frm->IsStartSolution())
		ShowWindow(SW_NORMAL);
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
