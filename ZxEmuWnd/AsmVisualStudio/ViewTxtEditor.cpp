
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "DocumentTxtEditor.h"
#include "ViewTxtEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CViewTxtEditor, CView)
BEGIN_MESSAGE_MAP(CViewTxtEditor, CView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CViewTxtEditor::OnDraw(CDC* dc)
{
}

#ifdef _DEBUG
void CViewTxtEditor::AssertValid() const
{
	CView::AssertValid();
}

void CViewTxtEditor::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDocumentTxtEditor* CViewTxtEditor::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocumentTxtEditor)));
	return (CDocumentTxtEditor*)m_pDocument;
}
#endif //_DEBUG

int CViewTxtEditor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CConfiguration* cfg(theApp.frm->GetConfiguration());
	doc = GetDocument();
	ogx = doc->GetOgx();
	ogx->Create(NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 1, 1), this, ID_OGX_TXT_EDITOR);
	ogx->Initialize(0, 0, LANG_TXT, 0);
	ogx->SetStdGroup((long)cfg->GetStdGroup(TE_TEXT));
	ogx->SetFontParams((long)cfg->GetGroupFonts(TE_TEXT));

	return 0;
}

BOOL CViewTxtEditor::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

void CViewTxtEditor::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	rectClient.SetRect(0, 0, cx, cy);
	if(ogx) ogx->MoveWindow(rectClient);
}

BOOL CViewTxtEditor::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
}
