
#pragma once

const UINT WM_GRID_CHANGE_PROPERTY = RegisterWindowMessage(_T("GRID_CHANGE_PROPERTY"));

class CAsmGridCtrl : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CAsmGridCtrl)
public:
	// �����������
	CAsmGridCtrl() {}
	// ����������
	virtual ~CAsmGridCtrl() {}
	// ��������� ��������
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
};


