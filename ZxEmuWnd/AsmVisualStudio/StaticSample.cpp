
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "StaticSample.h"

IMPLEMENT_DYNAMIC(CStaticSample, CStatic)

CStaticSample::CStaticSample()
{
	cur_color = RGB(255, 255, 255);
	cur_color_bk = RGB(0, 0, 0);
	cur_name = "MS Sans Serif";
	cur_size = 14;
	cur_is_bold = false;
	cur_is_italic = false;
	cur_is_underline = false;
	cur_font.CreateFont(-14, 0, 0, 0, FW_NORMAL, cur_is_italic, cur_is_underline, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, cur_name);
}

BEGIN_MESSAGE_MAP(CStaticSample, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CStaticSample::OnPaint()
{
	CRect rc;
	CPaintDC dc(this);

	GetClientRect(rc);
	dc.FillSolidRect(rc, cur_color_bk);
	dc.SetTextColor(cur_color);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(cur_font);
	dc.DrawText("AaBbCcXxYyZz¿‡¡·¬‚›˝ﬁ˛ﬂˇ", rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void CStaticSample::SetDrawParams(COLORREF* color_bk, COLORREF* color, const char* font_family, int* font_size, bool* is_bold, bool* is_italic, bool* is_underline)
{
	if(color_bk) cur_color_bk = *color_bk;
	if(color) cur_color = *color;
	if(font_family) cur_name = font_family;
	if(font_size) cur_size = *font_size;
	if(is_bold) cur_is_bold = *is_bold;
	if(is_italic) cur_is_italic = *is_italic;
	if(is_underline) cur_is_underline = *is_underline;

	cur_font.DeleteObject();
	cur_font.CreateFont(-cur_size, 0, 0, 0, cur_is_bold ? FW_BOLD : FW_NORMAL, cur_is_italic, cur_is_underline, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, cur_name);
	if(IsWindow(m_hWnd))
		Invalidate();
}
