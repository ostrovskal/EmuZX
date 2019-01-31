
#pragma once

#include "OgxTextEditor.h"

class CPanelOutputView : public CDockablePane
{
public:
	CPanelOutputView() {}
	virtual ~CPanelOutputView() {}
	void AdjustLayout();
	// обновление базовых опций
	void UpdateOptions();
	// добавить новую строку
	void AddString(const char* text);
	// установка заголовка
	void SetTitle(const char* title);
	// сброс
	void Reset();
	// события
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnClearContent();
	afx_msg void OnCopy();
	afx_msg void OnUpdateIsText(CCmdUI *pCmdUI);
	// редактор OGX
	COgxTextEditor ogx;
protected:
	void TextDoubleClick(long y_line);
	DECLARE_EVENTSINK_MAP()
	DECLARE_MESSAGE_MAP()
};
