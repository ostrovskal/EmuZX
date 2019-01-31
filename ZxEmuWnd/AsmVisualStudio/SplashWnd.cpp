
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "SplashWnd.h"

IMPLEMENT_DYNAMIC(CSplashWnd, CWnd)

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CSplashWnd::Create(CWnd* pParentWnd)
{
	if(!m_bitmap.LoadBitmap(IDB_BITMAP_SPLASH))
		return FALSE;

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	return CreateEx(WS_EX_TOPMOST, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	CenterWindow();
	SetTimer(1, 1000, NULL);
	return 0;
}

void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);
	CDC dcImage;

	if(dcImage.CreateCompatibleDC(&dc))
	{
		BITMAP bm;
		m_bitmap.GetBitmap(&bm);
		CBitmap* pOldBitmap(dcImage.SelectObject(&m_bitmap));
		dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
		dcImage.SelectObject(pOldBitmap);
	}
}

void CSplashWnd::CloseSplashWindow()
{
	DestroyWindow();
	AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::OnTimer(UINT_PTR nIDEvent)
{
	CloseSplashWindow();
	CWnd::OnTimer(nIDEvent);
}

BOOL CSplashWnd::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN ||
		pMsg->message == WM_SYSKEYDOWN ||
		pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_RBUTTONDOWN ||
		pMsg->message == WM_MBUTTONDOWN ||
		pMsg->message == WM_NCLBUTTONDOWN ||
		pMsg->message == WM_NCRBUTTONDOWN ||
		pMsg->message == WM_NCMBUTTONDOWN)
	{
		CloseSplashWindow();
		return TRUE;
	}
	return CWnd::PreTranslateMessage(pMsg);
}
