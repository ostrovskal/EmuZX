
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "DocumentAsmEditor.h"
#include "ViewAsmEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDocumentAsmEditor, CDocumentCommon)
BEGIN_MESSAGE_MAP(CDocumentAsmEditor, CDocumentCommon)
	ON_COMMAND(ID_BOOKMARK_ACTIVATED, &CDocumentAsmEditor::OnBookmarkActivated)
	ON_COMMAND(ID_BOOKMARK_DEACTIVATED, &CDocumentAsmEditor::OnBookmarkDeactivated)
	ON_COMMAND(ID_BOOKMARK_ACTIVATED_ALL, &CDocumentAsmEditor::OnBookmarkActivatedAll)
	ON_COMMAND(ID_BOOKMARK_DEACTIVATED_ALL, &CDocumentAsmEditor::OnBookmarkDeactivatedAll)
	ON_COMMAND(ID_BOOKMARK_SET, &CDocumentAsmEditor::OnBookmarkSet)
	ON_COMMAND(ID_BOOKMARK_DELETE, &CDocumentAsmEditor::OnBookmarkDelete)
	ON_COMMAND(ID_BOOKMARK_DELETE_ALL, &CDocumentAsmEditor::OnBookmarkDeleteAll)
	ON_COMMAND(ID_BOOKMARK_NEXT, &CDocumentAsmEditor::OnBookmarkNext)
	ON_COMMAND(ID_BOOKMARK_PREV, &CDocumentAsmEditor::OnBookmarkPrev)
	ON_UPDATE_COMMAND_UI(ID_BOOKMARK_ACTIVATED, &CDocumentAsmEditor::OnUpdateBookmarkActivated)
	ON_UPDATE_COMMAND_UI(ID_BOOKMARK_DEACTIVATED, &CDocumentAsmEditor::OnUpdateBookmarkDeactivated)
	ON_UPDATE_COMMAND_UI(ID_BOOKMARK_DELETE, &CDocumentAsmEditor::OnUpdateBookmarkDelete)
	ON_COMMAND(ID_FORMAT_TEXT, &CDocumentAsmEditor::OnFormatText)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDocumentAsmEditor, CDocumentCommon)
	ON_EVENT(CDocumentAsmEditor, ID_OGX_CODE_EDITOR, 1, TextChange, VTS_I4 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

void CDocumentAsmEditor::TextChange(long y1, long y2, long action)
{
	((CViewAsmEditor*)GetView())->TextChange(y1, y2, action);
}

#ifdef _DEBUG
void CDocumentAsmEditor::AssertValid() const
{
	CDocumentCommon::AssertValid();
}

void CDocumentAsmEditor::Dump(CDumpContext& dc) const
{
	CDocumentCommon::Dump(dc);
}
#endif //_DEBUG

void CDocumentAsmEditor::OnBookmarkActivated()
{
	long x, y;
	ogx.GetCaretPosition(&x, &y);
	ogx.SetStatusLine(y, ogx.GetStatusLine(y) | SSL_BMA, TRUE);
	if(elem) elem->SetBookmark(y, SSL_BMA);
}

void CDocumentAsmEditor::OnBookmarkDeactivated()
{
	long x, y;
	ogx.GetCaretPosition(&x, &y);
	ogx.SetStatusLine(y, (ogx.GetStatusLine(y) & ~SSL_BMA), TRUE);
	if(elem) elem->SetBookmark(y, SSL_BM);
}

void CDocumentAsmEditor::OnBookmarkActivatedAll()
{
	long count(ogx.GetCountLines()), status;
	for(int i = 0 ; i < count ; i++)
	{
		status = ogx.GetStatusLine(i);
		if(!(status & SSL_BM))
			continue;
		ogx.SetStatusLine(i, status | SSL_BMA, FALSE);
		if(elem) elem->SetBookmark(i, SSL_BMA);
	}
	ogx.UpdateAll(0);
}

void CDocumentAsmEditor::OnBookmarkDeactivatedAll()
{
	long count(ogx.GetCountLines()), status;
	for(int i = 0 ; i < count ; i++)
	{
		status = ogx.GetStatusLine(i);
		if(!(status & SSL_BM))
			continue;
		ogx.SetStatusLine(i, (status & ~SSL_BMA), FALSE);
		if(elem) elem->SetBookmark(i, SSL_BM);
	}
	ogx.UpdateAll(0);
}

void CDocumentAsmEditor::OnBookmarkSet()
{
	long x, y;
	ogx.GetCaretPosition(&x, &y);
	ogx.SetStatusLine(y, ogx.GetStatusLine(y) | SSL_BMA, TRUE);
	if(elem) elem->SetBookmark(y, SSL_BMA);
}

void CDocumentAsmEditor::OnBookmarkDelete()
{
	long x, y;
	ogx.GetCaretPosition(&x, &y);
	ogx.SetStatusLine(y, ogx.GetStatusLine(y) & ~(SSL_BMA | SSL_BM), TRUE);
	if(elem) elem->SetBookmark(y, SSL_BMD);
}

void CDocumentAsmEditor::OnBookmarkDeleteAll()
{
	long count(ogx.GetCountLines()), status;
	for(int i = 0 ; i < count ; i++)
	{
		status = ogx.GetStatusLine(i);
		if(status & SSL_BM)
		{
			ogx.SetStatusLine(i, (status & ~(SSL_BMA | SSL_BM)), FALSE);
			if(elem) elem->SetBookmark(i, SSL_BMD);
		}
	}
	ogx.UpdateAll(0);
}

void CDocumentAsmEditor::OnBookmarkNext()
{
	long count(ogx.GetCountLines()), x, y;
	ogx.GetCaretPosition(&x, &y);
	y++;
	for( ; y < count ; y++)
	{
		if((ogx.GetStatusLine(y) & (SSL_BMA | SSL_VIS)) == (SSL_BMA | SSL_VIS))
		{
			ogx.GotoLine(y + 1, FALSE);
			ogx.UpdateAll(0);
			break;
		}
	}
}

void CDocumentAsmEditor::OnBookmarkPrev()
{
	long x, y;
	ogx.GetCaretPosition(&x, &y);
	y--;
	for( ; y >= 0 ; y--)
	{
		if((ogx.GetStatusLine(y) & (SSL_BMA | SSL_VIS)) == (SSL_BMA | SSL_VIS))
		{
			ogx.GotoLine(y + 1, FALSE);
			ogx.UpdateAll(0);
			break;
		}
	}
}

void CDocumentAsmEditor::OnUpdateBookmarkActivated(CCmdUI* pCmdUI)
{
	long x, y;
	ogx.GetCaretPosition(&x, &y);
	pCmdUI->Enable((ogx.GetStatusLine(y) & SSL_BM));
}

void CDocumentAsmEditor::OnUpdateBookmarkDeactivated(CCmdUI* pCmdUI)
{
	long x, y;
	ogx.GetCaretPosition(&x, &y);
	pCmdUI->Enable((ogx.GetStatusLine(y) & SSL_BMA));
}

void CDocumentAsmEditor::OnUpdateBookmarkDelete(CCmdUI* pCmdUI)
{
	long x, y;
	ogx.GetCaretPosition(&x, &y);
	pCmdUI->Enable(ogx.GetStatusLine(y) & SSL_BMA);
}

void CDocumentAsmEditor::OnFormatText()
{
	ogx.FormatText(TRUE);
}

void CDocumentAsmEditor::FillListProcedures(CElement* elem)
{
	CElement::BLOCK_ELEMENT* blk;
	CViewAsmEditor* view((CViewAsmEditor*)GetView());
	size_t index(0);
	if(view && elem)
	{
		view->m_ComboProc->RemoveAllItems();
		while((blk = elem->EnumBlocks(index)))
		{
			if(blk->type = CElement::blkProc)
				view->m_ComboProc->AddItem(blk->name);
		}
	}
}