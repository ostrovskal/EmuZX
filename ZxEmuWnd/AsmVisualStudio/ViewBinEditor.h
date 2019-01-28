
#pragma once

class CViewBinEditor : public CView
{
	DECLARE_DYNCREATE(CViewBinEditor)
public:
	virtual void OnDraw(CDC* pDC);
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	CViewBinEditor();
	virtual ~CViewBinEditor();
	DECLARE_MESSAGE_MAP()
};


