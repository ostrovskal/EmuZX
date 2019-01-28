
#pragma once

#include "AsmGridCtrl.h"

class CPanelPropertiesView : public CDockablePane
{
public:
	enum InfoTypes
	{
		IT_DEFAULT,
		IT_FILE,
		IT_MODULE,
		IT_PROCEDURE,
		IT_STRUCTURE,
		IT_EQU,
		IT_CODE_EDITOR,
		IT_TEXT_EDITOR,
		IT_BIN_EDITOR,
		IT_RESOURCE
	};
	// �����������
	CPanelPropertiesView() : nType(IT_DEFAULT) {}
	// ����������
	virtual ~CPanelPropertiesView() {}
	//
	void AdjustLayout();
	// ���������� ��� ������ ����������� ����������
	void UpdateAll(int type, void* stk, void* sub_stk, bool is_show);
	// ��������� �����
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}
protected:
	// ���
	int nType;
	// �������
	CAsmGridCtrl m_wndPropList;
	// �������
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	// ����� �������
	DECLARE_MESSAGE_MAP()
};
