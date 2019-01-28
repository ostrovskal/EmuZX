
#pragma once

#include "OgxTextEditor.h"

class CPanelFindView : public CDockablePane
{
public:
	class CToolBarFind : public CMFCToolBar
	{
		virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
		{
			CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
		}
		virtual BOOL AllowShowOnList() const {return FALSE;}
	};
	// конструктор
	CPanelFindView() : cur_loc(-1) {}
	// деструктор
	virtual ~CPanelFindView() {}
	void AdjustLayout();
	// обновление базовых опций
	void UpdateOptions();
	// установить заголовок при старте поиска
	void SetTitle(const char* text);
	// вывести строку - завершения операции
	void SetFinish();
	// добавить новую строку для поиска
	void AddString(const CString& document, const CString& text_line, long num_line);
	// сброс
	void Reset();
	// события
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnClearContent();
	afx_msg void OnGotoLocation();
	afx_msg void OnGotoNextLocation();
	afx_msg void OnGotoPrevLocation();
	afx_msg void OnCopy();
	afx_msg void OnUpdateIsText(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNextLocation(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrevLocation(CCmdUI* pCmdUI);
protected:
	int cur_loc;
	CToolBarFind m_ToolBar;
	COgxTextEditor ogx;
	void TextDoubleClick(long y_line);
	DECLARE_EVENTSINK_MAP()
	DECLARE_MESSAGE_MAP()
};


