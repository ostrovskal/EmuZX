
#pragma once

class CStaticSample : public CStatic
{
	DECLARE_DYNAMIC(CStaticSample)
public:
	// конструктор
	CStaticSample();
	// деструктор
	virtual ~CStaticSample() {}
	afx_msg void OnPaint();
	// установка параметров отображения текста примера
	void SetDrawParams(COLORREF* color_bk, COLORREF* color, const char* font_family, int* font_size, bool* is_bold, bool* is_italic, bool* is_underline);
protected:
	// текущий размер
	int cur_size;
	// текущие свойства шрифта
	bool cur_is_bold, cur_is_italic, cur_is_underline;
	// текущий шрифт
	CFont cur_font;
	// текущий цвет
	COLORREF cur_color;
	// текущий фоновый цвет
	COLORREF cur_color_bk;
	// текущее имя шрифта
	CString cur_name;
	DECLARE_MESSAGE_MAP()
};


