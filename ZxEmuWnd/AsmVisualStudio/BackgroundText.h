
#pragma once

class CBackgroundText : public CWnd
{
	DECLARE_DYNAMIC(CBackgroundText)
public:
	CBackgroundText() {}
	virtual ~CBackgroundText() {}
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	CRect rcClient;
	CFont font1,font2;
	DECLARE_MESSAGE_MAP()
};
