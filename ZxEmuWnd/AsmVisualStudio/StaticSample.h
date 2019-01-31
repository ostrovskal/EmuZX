
#pragma once

class CStaticSample : public CStatic
{
	DECLARE_DYNAMIC(CStaticSample)
public:
	// �����������
	CStaticSample();
	// ����������
	virtual ~CStaticSample() {}
	afx_msg void OnPaint();
	// ��������� ���������� ����������� ������ �������
	void SetDrawParams(COLORREF* color_bk, COLORREF* color, const char* font_family, int* font_size, bool* is_bold, bool* is_italic, bool* is_underline);
protected:
	// ������� ������
	int cur_size;
	// ������� �������� ������
	bool cur_is_bold, cur_is_italic, cur_is_underline;
	// ������� �����
	CFont cur_font;
	// ������� ����
	COLORREF cur_color;
	// ������� ������� ����
	COLORREF cur_color_bk;
	// ������� ��� ������
	CString cur_name;
	DECLARE_MESSAGE_MAP()
};


