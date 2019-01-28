
#pragma once

const UINT WM_GRID_CHANGE_PROPERTY = RegisterWindowMessage(_T("GRID_CHANGE_PROPERTY"));

class CAsmGridCtrl : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CAsmGridCtrl)
public:
	// конструктор
	CAsmGridCtrl() {}
	// деструктор
	virtual ~CAsmGridCtrl() {}
	// изменение свойства
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
};


