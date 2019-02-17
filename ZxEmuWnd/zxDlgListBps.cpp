
#include "stdafx.h"
#include "zxDlgListBps.h"
#include "zxDebugger.h"
#include "zxDlgAddBp.h"

static TBBUTTON tbb[] = {
	{13, IDM_NEW, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Добавить\\Ctr+N"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{14, IDM_CHANGE, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Редактировать\\Ctrl+E"},
	{12, IDM_REMOVE, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Удалить\\Del"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{15, IDM_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Сохранить\\Ctrl+S"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{17, IDM_UP, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Выше\\Up"},
	{16, IDM_DOWN, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Ниже\\Down"},
};

BEGIN_MSG_MAP(zxDlgListBps, zxDialog)
	ON_COMMAND_RANGE(IDM_UP, IDM_DOWN, onUpDown)
	ON_COMMAND_RANGE(IDM_NEW, IDM_CHANGE, onNewEdit)
	ON_COMMAND(IDM_REMOVE, onRemove)
	ON_COMMAND(IDM_SAVE, onSave)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BP, onClickList)
END_MSG_MAP()

void zxDlgListBps::onUpDown() {
	ZX_BREAK_POINT tmp;
	auto bps = theApp->debug->bps;
	memcpy(&tmp, &bps[itemSelected], sizeof(ZX_BREAK_POINT));
	int nItemSelected = (itemSelected + (wmId == IDM_UP ? -1 : 1));
	memcpy(&bps[itemSelected], &bps[nItemSelected], sizeof(ZX_BREAK_POINT));
	memcpy(&bps[nItemSelected], &tmp, sizeof(ZX_BREAK_POINT));
	setItems();
	itemSelected = nItemSelected;
	ListView_SetItemState(hWndList, itemSelected, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	updateItems();
}

void zxDlgListBps::onNewEdit() {
	zxDlgAddBp add;
	ZX_BREAK_POINT* bpItem = nullptr;
	auto bps = debug->bps;
	if(wmId == IDM_CHANGE) {
		// fill data
		bpItem = &bps[itemSelected];
		add.edit(bpItem);
	}
	if(add.create(IDD_DIALOG_ADD_BP, hWnd, true) == IDOK) {
		auto _bp = &add.result;
		// проверка на пересечение
		for(int i = 0; i < COUNT_BP; i++) {
			auto bp = &bps[i];
			if(!(bp->flags & FBP_ADDR)) {
				// установка
				if(bpItem) {
					memcpy(bpItem, _bp, sizeof(ZX_BREAK_POINT));
				} else {
					memcpy(bp, _bp, sizeof(ZX_BREAK_POINT));
				}
				break;
			}
			if((_bp->flags & FBP_ACCESS) != (bp->flags & FBP_ACCESS) || bp == bpItem) continue;
			bool is1 = (_bp->address1 <= bp->address1) && (_bp->address2 >= bp->address1);
			bool is2 = (_bp->address1 <= bp->address2) && (_bp->address2 >= bp->address2);
			if(is1 || is2) {
				bool dec = theApp->getOpt(OPT_DECIMAL)->dval;
				zxString s1(fromNum(_bp->address1, radix[dec + 22]));
				zxString s2(fromNum(_bp->address2, radix[dec + 22]));
				zxString d1(fromNum(bp->address1, radix[dec + 22]));
				zxString d2(fromNum(bp->address2, radix[dec + 22]));
				MessageBox(hWnd, zxString::fmt(L"Перекрытие точек останова <%s - %s>/<%s - %s>!", s1, s2, d1, d2), L"Ошибка", MB_ICONERROR);
				break;
			}
		}
		// обновление
		setItems();
	}
}

void zxDlgListBps::onOK() {
	debug->saveBPS();
	zxDialog::onOK();
}

void zxDlgListBps::onRemove() {
	debug->removeBP(itemSelected);
	ListView_DeleteItem(hWndList, itemSelected);
	itemSelected = -1;
	updateItems();
}

void zxDlgListBps::onSave() {
	debug->saveBPS();
}

void zxDlgListBps::onClickList(LPNMHDR nm, LRESULT* pResult) {
	itemSelected = (int)SendMessage(hWndList, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
	updateItems();
}

int zxDlgListBps::onInitDialog(HWND hWnd) {
	debug = theApp->debug;
	hWndList = GetDlgItem(hWnd, IDC_LIST_BP);
	ListView_SetExtendedListViewStyleEx(hWndList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	if(!hWndToolbar) makeToolbar(IDB_TOOLBAR_DEBUGGER, tbb, 18, 9, 16, 16, IDT_TOOLBAR_LIST_BPS);

	static int columnWidth[] = {20, 120, 70, 60, 60, 70};
	static ssh_ws* columnNames[] = {L"#", L"Диапазон", L"Условие", L"Значение", L"Маска", L"Тип доступа"};

	LVCOLUMN lvc;
	memset(&lvc, 0, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 6; i++) {
		lvc.cx = columnWidth[i];
		lvc.pszText = columnNames[i];
		SendMessage(hWndList, LVM_INSERTCOLUMN, i, (LPARAM)&lvc);
	}
	setItems();
	return 1;
}

void zxDlgListBps::updateItems() {
	if(hWndToolbar) {
		auto bps = theApp->debug->bps;
		bool is = (itemSelected != -1);
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_CHANGE, (is << 2));
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_REMOVE, (is << 2));

		auto count = (int)SendMessage(hWndList, LVM_GETITEMCOUNT, 0, 0) - 1;
		bool is2 = is & (itemSelected < count);

		SendMessage(hWndToolbar, TB_SETSTATE, IDM_DOWN, (is2 << 2));
		is2 = is & (itemSelected > 0);
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_UP, (is2 << 2));
	}
}

void zxDlgListBps::setItems() {
	LVITEM lvi;

	auto dec = theApp->getOpt(OPT_DECIMAL)->dval;

	ListView_DeleteAllItems(hWndList);
	itemSelected = -1;

	memset(&lvi, 0, sizeof(LVITEM));
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 256;

	for(int i = 0; i < COUNT_BP; i++) {
		auto bp = &debug->bps[i];
		if(!(bp->flags & FBP_ADDR)) continue;
		lvi.iItem = i;

		lvi.iSubItem = 0;
		lvi.pszText = (ssh_ws*)fromNum(i + 1, radix[19]);
		SendMessage(hWndList, LVM_INSERTITEM, i, (LPARAM)&lvi);

		lvi.iSubItem = 1;
	
		auto txt = zxString::fmt(L"%s - %s", zxString(fromNum(bp->address1, radix[dec + 22])), zxString(fromNum(bp->address2, radix[dec + 22])));
		lvi.pszText = txt.buffer();
		SendMessage(hWndList, LVM_SETITEM, 1, (LPARAM)&lvi);

		lvi.iSubItem = 2;
		lvi.pszText = (bp->flags & FBP_VAL) ? cond_bp[(bp->flags & FBP_COND) >> 2] : L"";
		SendMessage(hWndList, LVM_SETITEM, 2, (LPARAM)&lvi);

		lvi.iSubItem = 3;
		lvi.pszText = (bp->flags & FBP_VAL) ? fromNum(bp->value, radix[dec + 16]) : L"";
		SendMessage(hWndList, LVM_SETITEM, 3, (LPARAM)&lvi);

		lvi.iSubItem = 4;
		lvi.pszText = (bp->flags & (FBP_VAL | FBP_MASK)) == (FBP_VAL | FBP_MASK) ? fromNum(bp->mask, radix[dec + 16]) : L"";
		SendMessage(hWndList, LVM_SETITEM, 4, (LPARAM)&lvi);

		lvi.iSubItem = 5;
		lvi.pszText = (bp->flags & FBP_MEM) ? L"MEM" : L"PC";
		SendMessage(hWndList, LVM_SETITEM, 5, (LPARAM)&lvi);
	}
	updateItems();
}