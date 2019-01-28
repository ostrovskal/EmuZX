// ViewBinEditor.cpp : implementation file
//

#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ViewBinEditor.h"


// CViewBinEditor

IMPLEMENT_DYNCREATE(CViewBinEditor, CView)

CViewBinEditor::CViewBinEditor()
{

}

CViewBinEditor::~CViewBinEditor()
{
}

BEGIN_MESSAGE_MAP(CViewBinEditor, CView)
END_MESSAGE_MAP()


// CViewBinEditor drawing

void CViewBinEditor::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CViewBinEditor diagnostics

#ifdef _DEBUG
void CViewBinEditor::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewBinEditor::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewBinEditor message handlers
