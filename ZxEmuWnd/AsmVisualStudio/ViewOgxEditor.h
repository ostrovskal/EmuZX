
#pragma once

class CViewOgxEditor : public CView
{
	DECLARE_DYNCREATE(CViewOgxEditor)
public:
	virtual void OnDraw(CDC* pDC);
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	CViewOgxEditor();
	virtual ~CViewOgxEditor();
	DECLARE_MESSAGE_MAP()
};


