
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
	// �����������
	CPanelFindView() : cur_loc(-1) {}
	// ����������
	virtual ~CPanelFindView() {}
	void AdjustLayout();
	// ���������� ������� �����
	void UpdateOptions();
	// ���������� ��������� ��� ������ ������
	void SetTitle(const char* text);
	// ������� ������ - ���������� ��������
	void SetFinish();
	// �������� ����� ������ ��� ������
	void AddString(const CString& document, const CString& text_line, long num_line);
	// �����
	void Reset();
	// �������
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


