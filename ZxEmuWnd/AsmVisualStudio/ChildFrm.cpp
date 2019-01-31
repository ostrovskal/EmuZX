
#include "stdafx.h"
#include "AsmVisualStudio.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
END_MESSAGE_MAP()

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CMDIChildWndEx::PreCreateWindow(cs))
		return FALSE;
	return TRUE;
}

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CDocumentCommon* doc((CDocumentCommon*)GetActiveDocument());
	theApp.frm->GetSolution()->SetCurrentElement(doc ? doc->elem : NULL);
	CMDIChildWndEx::OnUpdateFrameTitle(bAddToTitle);
}
