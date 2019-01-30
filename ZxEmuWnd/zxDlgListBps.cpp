
#include "stdafx.h"
#include "zxDlgListBps.h"
#include "zxDebugger.h"
#include "zxDlgAddBp.h"

static LVCOLUMN columns[] = {
	{LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_MINWIDTH | LVCF_IDEALWIDTH | LVCF_DEFAULTWIDTH, LVCFMT_LEFT, 30, L"№", 0, 0, 0, 0, 20, 20, 20},
	{LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_MINWIDTH | LVCF_IDEALWIDTH | LVCF_DEFAULTWIDTH, LVCFMT_LEFT, 120, L"Диапазон", 0, 0, 0, 0, 50, 50, 50},
	{LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_MINWIDTH | LVCF_IDEALWIDTH | LVCF_DEFAULTWIDTH, LVCFMT_LEFT, 80, L"Значение", 0, 0, 0, 0, 50, 50, 50},
	{LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_MINWIDTH | LVCF_IDEALWIDTH | LVCF_DEFAULTWIDTH, LVCFMT_CENTER, 80, L"Тип доступа", 0, 0, 0, 0, 50, 50, 50},
};

static TBBUTTON tbb[] = {
	{13, IDM_NEW, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Добавить\tCtr+N"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{14, IDM_CHANGE, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Редактировать\tShift+C"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{12, IDM_REMOVE, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Удалить\tDel"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{15, IDM_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Сохранить\tDel"},
};

bool zxDlgListBps::onCommand(int wmId, int param, LPARAM lParam) {
	switch(wmId) {
		case IDM_CHANGE:
		case IDM_NEW: {
			zxDlgAddBp add;
			if(wmId == IDM_CHANGE) {
				//SendMessage(hWndList, LVM_GETSELECTEDCOUNT)
				// fill data
				//add.edit()
			}
			add.create(IDD_DIALOG_ADD_BP, this, true);
			break;
		}
		case IDM_REMOVE: {
			ZX_BREAK_POINT* bps = theApp.debug->bps;
			memcpy(&bps[1], &bps[2], &bps[9] - &bps[1]);
			memset(&bps[9], -1, sizeof(ZX_BREAK_POINT));
			break;
		}
		case IDM_SAVE:
			theApp.debug->saveBPS();
			break;
		default: return false;
	}
	return true;
}

void zxDlgListBps::onInitDialog(HWND hWnd, LPARAM lParam) {
	hWndList = GetDlgItem(hWnd, IDC_LIST_BP);
	SetWindowLong(hWndList, GWL_EXSTYLE, GetWindowLong(hWndList, GWL_EXSTYLE) | LVS_EX_FULLROWSELECT);
}

bool zxDlgListBps::onNotify(LPNMHDR nm) {
	NMLVDISPINFO* plvdi;

	switch(((LPNMHDR)nm)->code) {
		case LVN_GETDISPINFO:
			plvdi = (NMLVDISPINFO*)nm;
			auto bp = &theApp.debug->bps[plvdi->item.iItem];
			auto dec = theApp.getOpt(OPT_DECIMAL)->dval;
			switch(plvdi->item.iSubItem) {
				case 0: plvdi->item.pszText = (ssh_ws*)fromNum(plvdi->item.iItem, radix[dec + 18]); break;
				case 1: 
					StringZX::fmt(L"%s - %s", StringZX(fromNum(bp->address1, radix[dec + 22])), StringZX(fromNum(bp->address2, radix[dec + 22])));
					plvdi->item.pszText = (bp->address1 == -1 ? L"Слот свободен" : tmpBuf);
					break;
				case 2: plvdi->item.pszText = (bp->value == -1 ? L"Любое" : fromNum(bp->value, radix[dec])); break;
				case 3: plvdi->item.pszText = (bp->address1 == -1 ? L"Не задан" : ((bp->flags & FBP_MEM) ? L"MEM" : L"PC"));
			}
			return true;
	}
	return false;
}

void zxDlgListBps::postCreate() {

	makeToolbar(IDB_TOOLBAR_DEBUGGER, tbb, 15, 7, 16, 16);
	
	for(int i = 0; i < 4; i++) SendMessage(hWndList, LVM_INSERTCOLUMN, i, (LPARAM)&columns[i]);
	LVITEM lvI;
	lvI.pszText = LPSTR_TEXTCALLBACK;
	lvI.mask = LVIF_TEXT | LVIF_STATE;
	lvI.stateMask = 0; lvI.iSubItem = 0; lvI.state = 0;

	for(int index = 0; index < 10; index++) {
		lvI.iItem = index;
		if(ListView_InsertItem(hWndList, &lvI) == -1) break;
	}
}