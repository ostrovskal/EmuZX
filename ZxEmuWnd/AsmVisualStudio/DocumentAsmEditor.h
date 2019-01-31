
#pragma once

#include "DocumentCommon.h"

class CViewAsmEditor;
class CDocumentAsmEditor : public CDocumentCommon
{
public:
	// деструктор
	virtual ~CDocumentAsmEditor() {}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// установка списка процедур
	virtual void FillListProcedures(CElement* elem);
	// события
	afx_msg void OnBookmarkActivated();
	afx_msg void OnBookmarkDeactivated();
	afx_msg void OnBookmarkActivatedAll();
	afx_msg void OnBookmarkDeactivatedAll();
	afx_msg void OnBookmarkSet();
	afx_msg void OnBookmarkDelete();
	afx_msg void OnBookmarkDeleteAll();
	afx_msg void OnBookmarkNext();
	afx_msg void OnBookmarkPrev();
	afx_msg void OnFormatText();
	afx_msg void OnUpdateBookmarkActivated(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBookmarkDeactivated(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBookmarkDelete(CCmdUI* pCmdUI);
protected:
	// конструктор
	CDocumentAsmEditor() {}
	// изменение текста редактора
	void TextChange(long y1, long y2, long action);
	DECLARE_DYNCREATE(CDocumentAsmEditor)
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
};

