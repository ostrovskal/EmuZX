
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "BackgroundText.h"

IMPLEMENT_DYNAMIC(CBackgroundText, CWnd)

BEGIN_MESSAGE_MAP(CBackgroundText, CWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()

void CBackgroundText::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	GetClientRect(&rcClient);
	Invalidate();
}

void CBackgroundText::OnPaint()
{
	CPaintDC dc(this);

	int cx((int)(rcClient.right + rcClient.left) / 2);
	int cy((int)(rcClient.top + rcClient.bottom) / 2);

	dc.SelectObject(&font1);
	dc.SetTextAlign(TA_TOP | TA_CENTER);
	dc.SetBkColor(RGB(128, 128, 128));
	dc.SetTextColor(RGB(255, 255, 255));
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0, 0, 0));
	dc.TextOut(cx + 1, cy - 65, "Visual Assembler");
	dc.SetTextColor(RGB(255, 255, 255));
	dc.TextOut(cx, cy - 70, "Visual Assembler");
	dc.SetTextAlign(TA_TOP | TA_LEFT);
	dc.SelectObject(&font2);
	dc.TextOut(cx + 85, cy + 10, "Версия 1.0");
	dc.TextOut(cx + 85, cy + 35, "Владикавказ 2010, РСО - А");
}

int CBackgroundText::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	font1.CreateFont(-40, 0, 0, 0, FW_MEDIUM, TRUE, TRUE, NULL, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 0x04 | FF_ROMAN, "Times New Roman Cyr");
	font2.CreateFont(-15, 0, 0, 0, FW_MEDIUM, FALSE, TRUE, NULL, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 0x04 | FF_ROMAN, "Times New Roman Cyr");

	return 0;
}
