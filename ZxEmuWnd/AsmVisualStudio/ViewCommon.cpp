// ViewCommon.cpp : implementation file
//

#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "ViewCommon.h"


// CViewCommon

IMPLEMENT_DYNCREATE(CViewCommon, CView)

CViewCommon::CViewCommon()
{

}

CViewCommon::~CViewCommon()
{
}

BEGIN_MESSAGE_MAP(CViewCommon, CView)
END_MESSAGE_MAP()


// CViewCommon drawing

void CViewCommon::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CViewCommon diagnostics

#ifdef _DEBUG
void CViewCommon::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewCommon::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewCommon message handlers
