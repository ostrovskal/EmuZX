
#pragma once

#include "PanelOutputView.h"

class CPanelTaskView : public CDockablePane
{
public:
	enum ColumnTypes
	{
		ctDesc = 0,
		ctDoc,
		ctNum
	};
	enum ModeTypes
	{
		modeError,
		modeInfo,
		modeWarning
	};
	class CToolBarTask : public CMFCToolBar
	{
		virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
		{
			CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
		}
		virtual BOOL AllowShowOnList() const {return FALSE;}
	};
	// конструктор
	CPanelTaskView();
	// деструктор
	virtual ~CPanelTaskView() {}
	// сброс
	void Reset();
	// применение фильтра панели вывода
	int UpdateList(int type, bool is_update);
	// события
	afx_msg void OnCopy();
	afx_msg void OnTaskErrors();
	afx_msg void OnTaskInformations();
	afx_msg void OnTaskWarnings();
	afx_msg void OnClearContent();
	afx_msg void OnGotoLocation();
	afx_msg void OnGotoNextLocation();
	afx_msg void OnGotoPrevLocation();
	afx_msg void OnUpdateTaskPanel(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTaskNextLocation(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTaskPrevLocation(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTaskErrors(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTaskWarnings(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTaskInfo(CCmdUI *pCmdUI);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLbnSelchangeTaskList();
	afx_msg void OnLbnDblclkTaskList();
	afx_msg void OnListDblClick(NMHDR* pNMHDR, LRESULT* pResult);
protected:
	int nMode, cur_loc;
	CFont m_font;
	CRect rectClient;
	CMFCListCtrl m_wndList;
	CToolBarTask m_ToolBar;
	CPanelOutputView* out;
	int GetListItemSel();
	void AdjustHorzScroll();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	void AdjustLayout();
	DECLARE_MESSAGE_MAP()
};

