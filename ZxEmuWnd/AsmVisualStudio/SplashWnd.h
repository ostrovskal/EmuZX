
#pragma once

class CSplashWnd : public CWnd
{
	DECLARE_DYNAMIC(CSplashWnd)
public:
	CSplashWnd() {}
	virtual ~CSplashWnd() {}
	BOOL Create(CWnd* pParentWnd = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	CBitmap m_bitmap;
	void CloseSplashWindow();
	DECLARE_MESSAGE_MAP()
};
