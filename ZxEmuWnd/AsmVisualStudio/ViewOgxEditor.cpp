
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ViewOgxEditor.h"

IMPLEMENT_DYNCREATE(CViewOgxEditor, CView)

CViewOgxEditor::CViewOgxEditor()
{

}

CViewOgxEditor::~CViewOgxEditor()
{
}

BEGIN_MESSAGE_MAP(CViewOgxEditor, CView)
END_MESSAGE_MAP()


// CViewOgxEditor drawing

void CViewOgxEditor::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CViewOgxEditor diagnostics

#ifdef _DEBUG
void CViewOgxEditor::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewOgxEditor::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewOgxEditor message handlers
