
#include "stdafx.h"

#include "zxBus.h"
#include "zxDebugger.h"
#include "zxKeyboard.h"
#include "zxGamepad.h"
#include "zxDlgSettings.h"

CRITICAL_SECTION cs;

static int idsModel[] = {IDM_48K, IDM_128K, IDM_PENTAGON_128K, IDM_SCORPION_256K, 0};
static int idsPP[] = { IDM_FILTER_NONE, IDM_FILTER_SMOOTH, IDM_FILTER_BILINEAR, IDM_FILTER_BICUBIC, 
						IDM_FILTER_ROUND_NONE, IDM_FILTER_ROUND_SMOOTH, IDM_FILTER_ROUND_BILINEAR, IDM_FILTER_ROUND_BICUBIC, 0};
static int idsAR[] = {IDM_1X, IDM_2X, IDM_3X, IDM_4X, IDM_AS_IS, 0};

ssh_cws nameROMs[] = {L"48K", L"128K", L"PENTAGON 128K", L"SCORPION 256K"};

HMENU hMenu = nullptr;
HINSTANCE hInst = nullptr;

static void CALLBACK timerProc(HWND, UINT, UINT_PTR, DWORD) {
	theApp->bus.execute();
}

BEGIN_MSG_MAP(zxFrame, zxWnd)
	ON_NOTIFY(TBN_DROPDOWN, IDT_TOOLBAR_MAIN, onNotify)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(IDM_SETTINGS, onSettings)
	ON_COMMAND(IDM_RESET, onReset)
	ON_COMMAND(IDM_RESTORE, onRestore)
	ON_COMMAND(IDM_EXIT, onClose)
	ON_COMMAND(IDM_OPEN, onOpen)
	ON_COMMAND(IDM_SAVE, onSave)
	ON_COMMAND(IDM_PAUSE, onUpdate)
	ON_COMMAND(IDM_SOUND, onUpdate)
	ON_COMMAND(IDM_TURBO, onUpdate)
	ON_COMMAND(IDM_DEBUGGER, onUpdate)
	ON_COMMAND(IDM_KEYBOARD, onUpdate)
	ON_COMMAND(IDM_JOYSTICK, onUpdate)
	ON_COMMAND(IDM_TRAP_TAPE, onUpdate)
	ON_COMMAND(IDM_FULLSCREEN, onFullScreen)
	ON_COMMAND_RANGE(IDM_48K, IDM_SCORPION_256K, onUpdate)
	ON_COMMAND_RANGE(IDM_1X, IDM_AS_IS, onUpdate)
	ON_COMMAND_RANGE(IDM_FILTER_NONE, IDM_FILTER_ROUND_BICUBIC, onUpdate)
	ON_COMMAND_RANGE(1000, 1009, onProcessMRU)
END_MSG_MAP()

void zxFrame::updateCaption() {
	SetWindowText(theApp->getHWND(), zxString::fmt(L"sshZX (%s - %s) %s [%s]", (((*_TSTATE) & ZX_EXEC) ? L"execute" : L"pause"), (getOpt(OPT_TURBO)->dval ? L"turbo" : L"normal"),
				  nameROMs[getOpt(OPT_MODEL)->dval], opts.nameLoadProg.str()));
}

bool zxFrame::checkedSubMenu(HMENU hSubMenu, int id_opt, ssh_d val, int* ids) {
	int id;
	auto mids = ids;
	auto opt = getOpt(id_opt);

	if(val == -1) val = opt->dval;

	while((id = *mids++)) CheckMenuItem(hSubMenu, id, 0);
	CheckMenuItem(hSubMenu, ids[val], MF_CHECKED);

	auto ret = opt->dval != val;
	if(ret) opt->dval = val;
	return ret;
}

bool zxFrame::changeState(int id_opt, int id, bool change) {
	auto opt = getOpt(id_opt);
	if(change) opt->dval = !opt->dval;
	bool v = (bool)opt->dval;
	CheckMenuItem(hMenu, id, v << 3);
	if(hWndToolbar) SendMessage(hWndToolbar, TB_SETSTATE, id, TBSTATE_ENABLED | (int)v);
	return v;
}

void zxFrame::onRestore() {
	bool isExec = (*_TSTATE & ZX_EXEC);
	if (!bus.loadState(opts.mainDir + L"auto_state.zx")) {
		MessageBox(hWnd, L"Не удалось загрузить состояние машины!", L"Ошибка", MB_ICONERROR);
		modifyTSTATE(ZX_RESET, 0);
	}
	updateData(ST_EXECUTE | isExec * STS_EXECUTE);
}

void zxFrame::onFullScreen() {
	static long wndStyles = 0;
	auto sts = GetWindowLong(hWnd, GWL_STYLE);;
	if(!wndStyles) wndStyles = sts;
	if(sts != wndStyles) {
		// fullscreen
		SetWindowLong(hWnd, GWL_STYLE, wndStyles);
		SetWindowLong(hWnd, GWL_EXSTYLE, 0);
		ShowWindow(hWndToolbar, SW_NORMAL);
		ShowWindow(hWnd, SW_NORMAL);
	} else {
		SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPED);
		SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);
		//DestroyMenu(hMenu);
		ShowWindow(hWndToolbar, SW_HIDE);
		ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	}
	UpdateWindow(hWnd);
}

void zxFrame::onUpdate() {
	static int change[] = { IDM_PAUSE, ST_EXECUTE | ST_EXECUTE_GO, 
							IDM_JOYSTICK, ST_JOYSTICK, IDM_TRAP_TAPE, ST_TAPE,
							IDM_TURBO, ST_TURBO, IDM_SOUND, ST_SOUND, IDM_DEBUGGER, ST_DEBUGGER, IDM_KEYBOARD, ST_KEYBOARD,
							IDM_FILTER_NONE, ST_FILTER, IDM_FILTER_SMOOTH, ST_FILTER, IDM_FILTER_BILINEAR, ST_FILTER, IDM_FILTER_BICUBIC, ST_FILTER,
							IDM_FILTER_ROUND_NONE, ST_FILTER, IDM_FILTER_ROUND_SMOOTH, ST_FILTER, IDM_FILTER_ROUND_BILINEAR, ST_FILTER, IDM_FILTER_ROUND_BICUBIC, ST_FILTER,
							IDM_1X, ST_ASPECT, IDM_2X, ST_ASPECT, IDM_3X, ST_ASPECT, IDM_4X, ST_ASPECT, IDM_AS_IS, ST_ASPECT,
							IDM_48K, ST_MODEL, IDM_128K, ST_MODEL, IDM_PENTAGON_128K, ST_MODEL, IDM_SCORPION_256K, ST_MODEL};
	for(int i = 0; i < 48; i += 2) {
		if(change[i] == wmId) {
			int state = change[i + 1];
			if(wmId == IDM_PAUSE) state |= (!(*_TSTATE & ZX_EXEC) * STS_EXECUTE);
			updateData(state);
			break;
		}
	}
}

void zxFrame::updateData(ssh_d state) {
	bool exec = getOpt(OPT_EXECUTE)->dval;
	if(state & ST_EXECUTE) {
		exec = (state & STS_EXECUTE);
		modifyTSTATE(exec * ZX_EXEC, ZX_EXEC);
	}
	getOpt(OPT_EXECUTE)->dval = exec;
	changeState(OPT_EXECUTE, IDM_PAUSE, false);
	if(state & ST_EXECUTE_GO) debug->setProgrammPause(!exec, false);

	changeState(OPT_TRAP_TAPE, IDM_TRAP_TAPE, state & ST_TAPE);
	changeState(OPT_TURBO, IDM_TURBO, state & ST_TURBO);
	changeState(OPT_JOYSTICK, IDM_JOYSTICK, state & ST_JOYSTICK);
	changeState(OPT_SOUND, IDM_SOUND, state & ST_SOUND);
	debug->show(changeState(OPT_DEBUGGER, IDM_DEBUGGER, state & ST_DEBUGGER));
	keyboard->show(changeState(OPT_KEYBOARD, IDM_KEYBOARD, state & ST_KEYBOARD));

	bus.updateData();

	auto val = ((state & ST_FILTER) ? wmId - IDM_FILTER_NONE : -1);
	checkedSubMenu(hMenu, OPT_FILTER, val, idsPP);

	val = ((state & ST_MODEL) ? wmId - IDM_48K : -1);
	checkedSubMenu(hMenu, OPT_MODEL, val, idsModel);
	if(state & ST_MODEL) bus.changeModel(val, *_MODEL);

	val = ((state & ST_ASPECT) ? wmId - IDM_1X : -1);
	checkedSubMenu(hMenu, OPT_ASPECT_RATIO, val, idsAR);
	if(state & ST_ASPECT) {
		RECT r;
		if(GetWindowRect(hWnd, &r)) {
			int x = r.left;
			int y = r.top;
			int cx = r.right - x;
			int cy = r.bottom - y;
			auto sz = makeWndSize();
			auto ncx = LOWORD(sz), ncy = HIWORD(sz);
			x += ((cx - ncx) / 2);
			y += ((cy - ncy) / 2);
			if(x < 0) x = 0;
			if(y < 0) y = 0;
			MoveWindow(hWnd, x, y, ncx, ncy, true);
		}
	}
	updateCaption();
}

void zxFrame::onKeyDown(UINT, UINT, UINT) {
	processKeys();
}

void zxFrame::onKeyUp(UINT, UINT, UINT) {
	processKeys();
}

void zxFrame::processKeys() {
	if(GetKeyboardState(theApp->keyboard->vkKeys))
		theApp->keyboard->processKeys();
}

void zxFrame::onSettings() {
	zxDlgSettings dlg;
	dlg.create(IDD_DIALOG_SETTINGS, hWnd, true);
}

void zxFrame::onReset() {
	modifyTSTATE(ZX_RESET, 0);
}

void zxFrame::onSizing(UINT corner, LPRECT rect) {
	if(getOpt(OPT_ASPECT_RATIO)->dval != AR_AS_IS) {
		static int corners[] = { 0, 1, 2, 4, 5, 6, 8, 9, 10 };
		auto sz = makeWndSize();
		int cx = LOWORD(sz), cy = HIWORD(sz);
		auto l = &rect->left, t = &rect->top, r = &rect->right, b = &rect->bottom;
		auto c = corners[corner];
		if(c & 1) *l = *r - cx;
		if(c & 2) *r = *l + cx;
		if(c & 4) *t = *b - cy;
		if(c & 8) *b = *t + cy;
	}
}

void zxFrame::onDestroy() {
	PostQuitMessage(0);
}

void zxFrame::onClose() {
	setOrGetWndPos(getHWND(), OPT_WND_MAIN_POS, true);
	setOrGetWndPos(debug->getHWND(), OPT_WND_DEBUG_POS, true);
	setOrGetWndPos(keyboard->getHWND(), OPT_WND_KEY_POS, true);

	opts.save(opts.mainDir + L"settings.zx");
	bus.saveState(opts.mainDir + L"auto_state.zx");

	DestroyWindow(hWnd);
}

void zxFrame::onOpen() {
	zxString folder(getOpt(OPT_CUR_PATH)->sval);
	if(wmId == IDM_OPEN) wmId = dlgSaveOrOpenFile(true, L"Открыть", L"Снепшот\0*.z80\0Диск\0*.trd\0TZX\0*.tzx\0Лента\0*.tap\0Звук\0*.wav\0Состояние\0*.zx\0", L"z80", folder);
	if(wmId) {
		zxString ext(folder.substr(folder.find_rev(L'.') + 1).lower());
		bool result = false;
		if(ext == L"z80") {
			result = bus.loadZ80(folder);
		} else if(ext == L"trd") {
			result = bus.selectTRD(folder, 0);
		} else if(ext == L"wav") {
			result = bus.loadWAV(folder);
		} else if(ext == L"tzx") {
			result = bus.loadTZX(folder);
		} else if(ext == L"tap") {
			result = bus.loadTAP(folder);
		} else if(ext == L"zx") {
			result = bus.loadState(folder);
		}
		if(!result) MessageBox(hWnd, L"Не удалось выполнить операцию!", L"Ошибка", MB_OK);
		else {
			auto pos = folder.find_rev(L'\\') + 1;
			getOpt(OPT_CUR_PATH)->sval = folder.left(pos);
			opts.nameLoadProg = folder.substr(pos, folder.find_rev(L'.') - pos);
			modifyMRU(folder);
			updateCaption();
		}
	}
}

void zxFrame::onSave() {
	zxString folder(getOpt(OPT_CUR_PATH)->sval);
	if(dlgSaveOrOpenFile(false, L"Сохранить", L"Снепшот\0*.z80\0Диск\0*.trd\0Звук\0*.wav\0Лента\0*.tap\0TZX\0*.tzx\0Состояние\0*.zx\0Скриншот\0*.tga\0", L"z80", folder)) {
		zxString ext(folder.substr(folder.find_rev(L'.') + 1).lower());
		bool result = false;
		if(ext == L"z80") {
			result = bus.saveZ80(folder);
		} else if(ext == L"zx") {
			result = bus.saveState(folder);
		} else if(ext == L"trd") {
			result = bus.saveTRD(folder);
		} else if(ext == L"wav") {
			result = bus.saveWAV(folder);
		} else if(ext == L"tzx") {
			result = bus.saveTZX(folder);
		} else if(ext == L"tap") {
			result = bus.saveTAP(folder);
		} else if(ext == L"tga") {
			result = bus.saveScreen(folder);
		}
		if(!result) MessageBox(hWnd, L"Не удалось выполнить операцию!", L"Ошибка", MB_ICONERROR);
		else getOpt(OPT_CUR_PATH)->sval = folder.left(folder.find_rev(L'\\') + 1);
	}
}

void zxFrame::onProcessMRU() {
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.cch = MAX_PATH;
	mii.dwTypeData = (ssh_ws*)TMP_BUF;
	if(GetMenuItemInfo(hMenuMRU, wmId, FALSE, &mii)) {
		getOpt(OPT_CUR_PATH)->sval = mii.dwTypeData;
		onOpen();
	}
}

BOOL zxFrame::onEraseBkgnd(HDC) {
	return FALSE;
}

void zxFrame::onSize(UINT type, int nWidth, int nHeight) {
	int top = 0;
	if(hWndToolbar) {
		if(IsWindowVisible(hWndToolbar)) {
			SendMessage(hWndToolbar, WM_SIZE, type, MAKELONG(nWidth, nHeight));
			GetWindowRect(hWndToolbar, &wndRect);
			top = (wndRect.bottom - wndRect.top) - 1;
		}
	}
	wndRect.top = top;
	wndRect.left = 0;
	wndRect.right = nWidth;
	wndRect.bottom = nHeight;
}

void zxFrame::onNotify(LPNMHDR nm, LRESULT*) {
	TPMPARAMS tpm;
	RECT rc;

	LPNMTOOLBAR lpnmTB = (LPNMTOOLBAR)nm;
	auto id = lpnmTB->iItem;
	SendMessage(lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)id, (LPARAM)&rc);
	MapWindowPoints(lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);

	HMENU hPopupMenu = nullptr;
	switch(id) {
		case IDM_MODEL: hPopupMenu = hMenuModel; break;
		case IDM_OPEN: hPopupMenu = hMenuMRU; break;
		case IDM_FILTER: hPopupMenu = hMenuPP; break;
	}
	updateData(0);

	tpm.cbSize = sizeof(TPMPARAMS);
	tpm.rcExclude = rc;

	TrackPopupMenuEx(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, hWnd, &tpm);
}

static TBBUTTON tbb[] = {
	{0, IDM_OPEN, TBSTATE_ENABLED, BTNS_DROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Открыть\\Alt+O"},
	{1, IDM_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Сохранить\\Alt+S"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{2, IDM_RESET, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Сброс\\Alt+R"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{3, IDM_RESTORE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Восстановить состояние\\Alt+V"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{4, IDM_PAUSE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Остановить\\Возобновить эмуляцию\\Alt+P"},
	{5, IDM_SOUND, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Вкл\\Выкл звук"},
	{12, IDM_JOYSTICK, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Вкл\\Выкл джойстик"},
	{13, IDM_KEYBOARD, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Показать\\Скрыть клавиатуру\\Alt+K"},
	{7, IDM_DEBUGGER, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Показать\\Скрыть дизассемблер\\Alt+D"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{8, IDM_TURBO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Ускорить\\Замедлить\\Alt+T"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{11, IDM_FILTER, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Фильтр"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{9, IDM_MODEL, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Модель памяти"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{10, IDM_SETTINGS, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Настройки\\Alt+N"},
};

zxFrame::zxFrame() : zxWnd() {
	InitializeCriticalSection(&cs);
	hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_ZXEMUWND));
	hMenuMRU = nullptr;
	keyboard = nullptr;
	gamepad = nullptr;
	hMenuPP = nullptr;
	hMenuModel = nullptr;
	hTimer = 0;
	debug = nullptr;
}

zxFrame::~zxFrame() {
	if (hTimer) {
		KillTimer(hWnd, hTimer);
		hTimer = 0;
	}

	SAFE_DELETE(debug);
	SAFE_DELETE(keyboard);
	SAFE_DELETE(gamepad);
	DeleteCriticalSection(&cs);
}

long zxFrame::makeWndSize() {
	RECT rc;
	auto mul = (getOpt(OPT_ASPECT_RATIO)->dval) + 1;
	auto border = (getOpt(OPT_SIZE_BORDER)->dval) * 16;
	if(mul > 4) mul = 2;
	int cx = (256 + border) * mul;
	int cy = ((192 + border) * mul) + 46UL;
	rc.left = 0;
	rc.top = 0;
	rc.right = cx;
	rc.bottom = cy;
	AdjustWindowRect(&rc, GetWindowLong(hWnd, GWL_STYLE), TRUE);
	return MAKELONG(rc.right, rc.bottom);
}

int zxFrame::run() {

	opts.load(opts.mainDir + L"settings.zx");
	
	auto sz = makeWndSize();

	if(!create(L"EmuWnd", L"EmuWnd", WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, LOWORD(sz), HIWORD(sz), nullptr, 0, IDC_ZXEMUWND))
		return -1;

	if(!bus.changeModel(getOpt(OPT_MODEL)->dval, 255))
		return -1;

	makeToolbar(IDB_TOOLBAR_COMMON, tbb, 14, 20, 32, 32, IDT_TOOLBAR_MAIN);

	ShowWindow(hWnd, true);
	UpdateWindow(hWnd);

	hMenu = GetMenu(hWnd);
	hMenuMRU = GetSubMenu(GetSubMenu(hMenu, 0), 7);
	hMenuModel = GetSubMenu(GetSubMenu(hMenu, 2), 7);
	hMenuPP = GetSubMenu(GetSubMenu(hMenu, 1), 5);

	HEX = getOpt(OPT_DECIMAL)->dval;

	debug = new zxDebugger;
	debug->create(IDD_DIALOG_DEBUGGER, hWnd, false);

	keyboard = new zxKeyboard;
	keyboard->create(IDD_DIALOG_KEYBOARD, hWnd, false);

	gamepad = new zxGamepad;
	gamepad->init();

	bus.loadState(opts.mainDir + L"auto_state.zx");

	setOrGetWndPos(debug->getHWND(), OPT_WND_DEBUG_POS, false);
	setOrGetWndPos(keyboard->getHWND(), OPT_WND_KEY_POS, false);
	setOrGetWndPos(hWnd, OPT_WND_MAIN_POS, false);

	updateData(getOpt(OPT_EXECUTE)->dval * STS_EXECUTE | ST_EXECUTE | ST_EXECUTE_GO);

	for(int i = 9; i >= 0; i--) modifyMRU(getOpt(OPT_MRU9)->sval);

	SetActiveWindow(hWnd);

	hTimer = SetTimer(hWnd, 10000, 20, timerProc);
	if(hTimer) {
		while (msgPump()) {}
	}
	return (int)thState.msg.wParam;
}

void zxFrame::modifyMRU(zxString path) {
	int i = 9, n = 9;
	bool insert = path.is_empty();
	if(!insert) {
		for(i = 0; i < 9; i++) if(getOpt(i)->sval == path) break;
		n = i;
	}
	for(; i > 0; i--) getOpt(i)->sval = getOpt(i - 1)->sval;
	getOpt(OPT_MRU0)->sval = path;
	if(!insert) {
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.cch = (UINT)path.length();
		mii.dwTypeData = path.buffer();

		RemoveMenu(hMenuMRU, IDM_MRU_EMPTY, MF_BYCOMMAND);
		RemoveMenu(hMenuMRU, n, MF_BYPOSITION);
		InsertMenuItem(hMenuMRU, 0, TRUE, &mii);
		auto count = GetMenuItemCount(hMenuMRU);
		for(i = 0; i < count; i++) {
			mii.fMask = MIIM_ID;
			mii.wID = 1000 + i;
			SetMenuItemInfo(hMenuMRU, i, TRUE, &mii);
		}
	}
}

void zxFrame::setOrGetWndPos(HWND h, int id_opt, bool get) {
	RECT r;
	auto o = theApp->getOpt(id_opt);

	if(get) {
		if(GetWindowRect(h, &r))
			theApp->getOpt(id_opt)->sval = zxString::fmt(L"%d,%d,%d,%d", r.left, r.top, r.right, r.bottom);
	} else {
		int count = 0;
		auto pos = o->sval.split(L",", count);
		if(count == 4) {
			r.left = _wtoi(pos[0]);
			r.top = _wtoi(pos[1]);
			r.right = _wtoi(pos[2]);
			r.bottom = _wtoi(pos[3]);
			MoveWindow(h, r.left, r.top, r.right - r.left, r.bottom - r.top, true);
		}
	}
}

