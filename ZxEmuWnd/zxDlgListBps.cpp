
#include "stdafx.h"
#include "zxDlgListBps.h"
#include "zxDebugger.h"
#include "zxDlgAddBp.h"

static LVCOLUMN columns[] = {
	{LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_MINWIDTH | LVCF_IDEALWIDTH | LVCF_DEFAULTWIDTH, LVCFMT_LEFT, 20, L"№", 0, 0, 0, 0, 10, 10, 10},
	{LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_MINWIDTH | LVCF_IDEALWIDTH | LVCF_DEFAULTWIDTH, LVCFMT_LEFT, 120, L"Диапазон", 0, 0, 0, 0, 50, 50, 50},
	{LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_MINWIDTH | LVCF_IDEALWIDTH | LVCF_DEFAULTWIDTH, LVCFMT_LEFT, 70, L"Условие", 0, 0, 0, 0, 20, 20, 20},
	{LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_MINWIDTH | LVCF_IDEALWIDTH | LVCF_DEFAULTWIDTH, LVCFMT_LEFT, 60, L"Значение", 0, 0, 0, 0, 40, 40, 40},
	{LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_MINWIDTH | LVCF_IDEALWIDTH | LVCF_DEFAULTWIDTH, LVCFMT_CENTER, 70, L"Тип доступа", 0, 0, 0, 0, 40, 40, 40},
};

static TBBUTTON tbb[] = {
	{13, IDM_NEW, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Добавить\tCtr+N"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{14, IDM_CHANGE, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Редактировать\tShift+C"},
	{12, IDM_REMOVE, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Удалить\tDel"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{15, IDM_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Сохранить\tDel"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{17, IDM_UP, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Выше\tUp"},
	{16, IDM_DOWN, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Ниже\tDown"},
};

bool zxDlgListBps::onCommand(int wmId, int param, LPARAM lParam) {
	switch(wmId) {
		case IDM_UP:
		case IDM_DOWN: {
			ZX_BREAK_POINT tmp;
			auto bps = theApp.debug->bps;
			memcpy(&tmp, &bps[itemSelected], sizeof(ZX_BREAK_POINT));
			int nItemSelected = (itemSelected + (wmId == IDM_UP ? -1 : 1));
			memcpy(&bps[itemSelected], &bps[nItemSelected], sizeof(ZX_BREAK_POINT));
			memcpy(&bps[nItemSelected], &tmp, sizeof(ZX_BREAK_POINT));
			setItems();
			itemSelected = nItemSelected;
			ListView_SetItemState(hWndList, itemSelected, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			updateItems();
			break;
		}
		case IDM_CHANGE:
		case IDM_NEW: {
			zxDlgAddBp add;
			ZX_BREAK_POINT* bpItem = nullptr;
			if(wmId == IDM_CHANGE) {
				// fill data
				bpItem = &theApp.debug->bps[itemSelected];
				add.edit(bpItem);
			}
			if(add.create(IDD_DIALOG_ADD_BP, this, true) == IDOK) {
				// проверка на пересечение
				auto _bp = &add.result;
				auto bps = theApp.debug->bps;
				if(wmId == IDM_CHANGE) {
					memcpy(bpItem, _bp, sizeof(ZX_BREAK_POINT));
				} else {
					for(int i = 0; i < COUNT_BP; i++) {
						auto bp = &bps[i];
						if(bp->address1 == -1) {
							// установка
							memcpy(bp, _bp, sizeof(ZX_BREAK_POINT));
							break;
						}
						if((_bp->flags & FBP_ACCESS) != (bp->flags & FBP_ACCESS)) continue;
						bool is1 = (_bp->address1 <= bp->address1) && (_bp->address2 >= bp->address1);
						bool is2 = (_bp->address1 <= bp->address2) && (_bp->address2 >= bp->address2);
						if(is1 || is2) {
							bool dec = theApp.getOpt(OPT_DECIMAL)->dval;
							StringZX s1(fromNum(_bp->address1, radix[dec + 10]));
							StringZX s2(fromNum(_bp->address2, radix[dec + 10]));
							StringZX d1(fromNum(bp->address1, radix[dec + 10]));
							StringZX d2(fromNum(bp->address2, radix[dec + 10]));
							MsgBox(StringZX::fmt(L"Перекрытие точки останова с <%i : %s - %s, %s - %s>!", i + 1, s1, s2, d1, d2), L"Ошибка", MB_ICONERROR);
							break;
						}
					}
				}
				// обновление
				setItems();
			}
			break;
		}
		case IDM_REMOVE: {
			theApp.debug->removeBP(itemSelected);
			ListView_DeleteItem(hWndList, itemSelected);
			itemSelected = -1;
			updateItems();
			break;
		}
		case IDM_SAVE:
			theApp.debug->saveBPS();
			break;
		case IDOK: break;
		default: return false;
	}
	return true;
}

void zxDlgListBps::onInitDialog(HWND hWnd, LPARAM lParam) {
	hWndList = GetDlgItem(hWnd, IDC_LIST_BP);
	ListView_SetExtendedListViewStyleEx(hWndList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

bool zxDlgListBps::onNotify(LPNMHDR nm) {
	if(nm->idFrom == IDC_LIST_BP) {
		switch(nm->code) {
			case NM_CLICK:
				itemSelected = (int)SendMessage(hWndList, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
				updateItems();
				break;
		}
	}
	return false;
}

void zxDlgListBps::updateItems() {
	auto bps = theApp.debug->bps;
	bool is = (itemSelected != -1);
	SendMessage(hWndToolbar, TB_SETSTATE, IDM_CHANGE, (is << 2));
	SendMessage(hWndToolbar, TB_SETSTATE, IDM_REMOVE, (is << 2));

	auto count = (int)SendMessage(hWndList, LVM_GETITEMCOUNT, 0, 0) - 1;
	bool is2 = is & (itemSelected < count);

	SendMessage(hWndToolbar, TB_SETSTATE, IDM_DOWN, (is2 << 2));
	is2 = is & (itemSelected > 0);
	SendMessage(hWndToolbar, TB_SETSTATE, IDM_UP, (is2 << 2));
}

void zxDlgListBps::postCreate() {
	makeToolbar(IDB_TOOLBAR_DEBUGGER, tbb, 18, 9, 16, 16);
	for(int i = 0; i < 5; i++) SendMessage(hWndList, LVM_INSERTCOLUMN, i, (LPARAM)&columns[i]);
	setItems();
}

void zxDlgListBps::setItems() {
	LVITEM lvi;

	auto dec = theApp.getOpt(OPT_DECIMAL)->dval;

	ListView_DeleteAllItems(hWndList);

	memset(&lvi, 0, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 256;

	for(int i = 0; i < COUNT_BP; i++) {
		auto bp = &theApp.debug->bps[i];
		if(bp->address1 == -1) continue;
		lvi.iItem = i;

		lvi.iSubItem = 0;
		lvi.pszText = (ssh_ws*)fromNum(i + 1, radix[19]);
		SendMessage(hWndList, LVM_INSERTITEM, i, (LPARAM)&lvi);

		lvi.iSubItem = 1;
	
		auto txt = StringZX::fmt(L"%s - %s", StringZX(fromNum(bp->address1, radix[dec + 22])), StringZX(fromNum(bp->address2, radix[dec + 22])));
		lvi.pszText = txt.buffer();
		SendMessage(hWndList, LVM_SETITEM, 1, (LPARAM)&lvi);

		lvi.iSubItem = 2;
		lvi.pszText = (bp->value == -1) ? L"" : cond_bp[(bp->flags & FBP_COND) >> 3];
		SendMessage(hWndList, LVM_SETITEM, 3, (LPARAM)&lvi);

		lvi.iSubItem = 3;
		lvi.pszText = (bp->value == -1) ? L"" : fromNum(bp->value, radix[dec + 16]);
		SendMessage(hWndList, LVM_SETITEM, 2, (LPARAM)&lvi);

		lvi.iSubItem = 4;
		lvi.pszText = (bp->flags & FBP_MEM) ? L"MEM" : L"PC";
		SendMessage(hWndList, LVM_SETITEM, 3, (LPARAM)&lvi);
	}
}