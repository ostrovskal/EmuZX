
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "AsmGridCtrl.h"

IMPLEMENT_DYNAMIC(CAsmGridCtrl, CMFCPropertyGridCtrl)

void CAsmGridCtrl::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
	GetParent()->SendMessage(WM_GRID_CHANGE_PROPERTY, (WPARAM)pProp->GetData(), (LPARAM)pProp);
}
