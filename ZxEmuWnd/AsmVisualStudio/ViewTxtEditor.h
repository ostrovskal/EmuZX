
#pragma once

#include "OgxTextEditor.h"

class CViewTxtEditor : public CView
{
public:
	CDocumentTxtEditor* GetDocument() const;
	virtual void OnDraw(CDC* pDC);
	virtual ~CViewTxtEditor() {}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	// конструктор
	CViewTxtEditor() : ogx(NULL), doc(NULL) {}
	// редактор OGX
	COgxTextEditor* ogx;
	CDocumentTxtEditor* doc;
	// область клиента
	CRect rectClient;
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CViewTxtEditor)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


#ifndef _DEBUG
inline CDocumentTxtEditor* CViewTxtEditor::GetDocument() const {return reinterpret_cast<CDocumentTxtEditor*>(m_pDocument);}
#endif

