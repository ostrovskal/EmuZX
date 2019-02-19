
#include "stdafx.h"

#include "zxBus.h"
#include "zxDebugger.h"
#include "zxKeyboard.h"
#include "zxGamepad.h"
#include "zxDlgSettings.h"
#include "Z80.h"

static ssh_w old_tstate = 0;
static int idsModel[] = {IDM_48K, IDM_128K, IDM_PENTAGON_128K, IDM_SCORPION_256K, 0};
static int idsPP[] = {IDM_PP_NONE, IDM_PP_MIXED, IDM_PP_BILINEAR, 0};
static int idsAR[] = {IDM_1X, IDM_2X, IDM_3X, IDM_4X, IDM_AS_IS, 0};

ssh_cws nameROMs[] = {L"48K", L"128K", L"PENTAGON 128K", L"SCORPION 256K"};
ssh_cws namePPs[] = {L"None", L"TV", L"Bilinear"};

static CRITICAL_SECTION cs;

HMENU hMenu = nullptr;
HINSTANCE hInst = nullptr;

static void CALLBACK timerProc(HWND hWNd, UINT nID, UINT_PTR elapsed, DWORD) {
	theApp->bus.execute();
}

void setOrGetWndPos(HWND h, int id_opt, bool get, bool activate) {
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

ssh_w modifyTSTATE(int adding, int remove) {
	EnterCriticalSection(&cs);
	(*_TSTATE) &= ~remove;
	(*_TSTATE) |= adding;
	LeaveCriticalSection(&cs);
	return (*_TSTATE);
}

void zxFrame::pauseCPU(bool isPause) {
	if(hCpuThread) {
		if(isPause) {
			old_tstate = (*_TSTATE);
			modifyTSTATE(0, ZX_EXEC);
			Wow64SuspendThread(hCpuThread);
		} else {
			modifyTSTATE(old_tstate, 0xffff);
			ResumeThread(hCpuThread);
		}
	}
}

bool dlgSaveOrOpenFile(bool bOpen, ssh_cws title, ssh_cws filter, ssh_cws defExt, zxString& folder) {
	bool result;
	static ssh_ws flt[MAX_PATH];

	int flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));

	if(bOpen) flags |= OFN_FILEMUSTEXIST;

	auto files = new ssh_ws[MAX_PATH];
	memset(files, 0, MAX_PATH);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = theApp->getHWND();

	ofn.lpstrDefExt = defExt;
	ofn.lpstrFilter = filter;
	ofn.lpstrInitialDir = folder;
	ofn.lpstrTitle = title;
	ofn.lpstrFile = files;
	ofn.lpstrFileTitle = nullptr;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = flags;

	result = bOpen ? ::GetOpenFileName(&ofn) : ::GetSaveFileName(&ofn);
	if(result) folder = ofn.lpstrFile;
	SAFE_DELETE(files);
	return result;
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
	ON_COMMAND_RANGE(IDM_48K, IDM_SCORPION_256K, onUpdate)
	ON_COMMAND_RANGE(IDM_1X, IDM_AS_IS, onUpdate)
	ON_COMMAND_RANGE(IDM_PP_NONE, IDM_PP_BILINEAR, onUpdate)
	ON_COMMAND_RANGE(1000, 1009, onProcessMRU)
END_MSG_MAP()

void zxFrame::updateCaption() {
	zxString::fmt(L"sshZX (%s - %s) %s [%s]", (((*_TSTATE) & ZX_EXEC) ? L"execute" : L"pause"), (getOpt(OPT_TURBO)->dval ? L"turbo" : L"normal"),
				  nameROMs[getOpt(OPT_MEM_MODEL)->dval], opts.nameLoadProg.str());
	SetWindowText(theApp->getHWND(), (LPCWSTR)tmpBuf);
}

bool zxFrame::checkedSubMenu(HMENU hMenu, int id_opt, int val, int* ids) {
	int id;
	auto mids = ids;
	auto opt = getOpt(id_opt);

	if(val == -1) val = opt->dval;

	while((id = *mids++)) CheckMenuItem(hMenu, id, 0);
	CheckMenuItem(hMenu, ids[val], MF_CHECKED);

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

void zxFrame::onUpdate() {
	static int change[] = { IDM_PAUSE, ST_EXECUTE | ST_EXECUTE_GO, 
							IDM_JOYSTICK, ST_JOYSTICK,
							IDM_TURBO, ST_TURBO, IDM_SOUND, ST_SOUND, IDM_DEBUGGER, ST_DEBUGGER, IDM_KEYBOARD, ST_KEYBOARD,
							IDM_FILTER, ST_FILTER, IDM_PP_NONE, ST_FILTER, IDM_PP_BILINEAR, ST_FILTER, IDM_PP_MIXED, ST_FILTER, 
							IDM_1X, ST_ASPECT, IDM_2X, ST_ASPECT, IDM_3X, ST_ASPECT, IDM_4X, ST_ASPECT, IDM_AS_IS, ST_ASPECT,
							IDM_MODEL, ST_MODEL, IDM_48K, ST_MODEL, IDM_128K, ST_MODEL, IDM_PENTAGON_128K, ST_MODEL, IDM_SCORPION_256K, ST_MODEL};
	for(int i = 0; i < 40; i += 2) {
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

	changeState(OPT_TURBO, IDM_TURBO, state & ST_TURBO);
	changeState(OPT_JOYSTICK_ALL, IDM_JOYSTICK, state & ST_JOYSTICK);
	changeState(OPT_SOUND_ALL, IDM_SOUND, state & ST_SOUND);
	debug->show(changeState(OPT_DEBUGGER, IDM_DEBUGGER, state & ST_DEBUGGER));
	keyboard->show(changeState(OPT_KEYBOARD, IDM_KEYBOARD, state & ST_KEYBOARD));

	bus.updateData();

	int val = ((state & ST_FILTER) ? wmId - IDM_PP_NONE : -1);
	checkedSubMenu(hMenu, OPT_PP, val, idsPP);

	val = ((state & ST_MODEL) ? wmId - IDM_48K : -1);
	checkedSubMenu(hMenu, OPT_MEM_MODEL, val, idsModel);
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
			SIZE sz;
			makeWndSize(&sz);
			GetClientRect(hWnd, &r);
			int nCx = sz.cx + (cx - r.right);
			int nCy = sz.cy + (cy - r.bottom);
			x += ((cx - nCx) / 2);
			y += ((cy - nCy) / 2);
			if(x < 0) x = 0;
			if(y < 0) y = 0;
			MoveWindow(hWnd, x, y, nCx, nCy, true);
		}
	}
	updateCaption();
}

void zxFrame::onKeyDown(UINT nVirtKey, UINT nRepeat, UINT nFlags) {
	processKeys();
}

void zxFrame::onKeyUp(UINT nVirtKey, UINT nRepeat, UINT nFlags) {
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
	/*
	RECT *selr, wr, cr, statr, or;
	int width, height, w_ofs, h_ofs, w_max, h_max;

	selr = (RECT *)lParam;
	GetWindowRect( fuse_hWnd, &wr );
	GetClientRect( fuse_hWnd, &cr );
	GetClientRect( fuse_hStatusWindow, &statr );

	w_ofs = ( wr.right - wr.left ) - ( cr.right - cr.left );
	h_ofs = ( wr.bottom - wr.top ) - ( cr.bottom - cr.top );
	if( settings_current.statusbar ) h_ofs += ( statr.bottom - statr.top );

	// max scaler size in desktop workarea
	SystemParametersInfo(SPI_GETWORKAREA, 0, &or , 0);
	w_max = (or .right - or .left) - w_ofs + DISPLAY_ASPECT_WIDTH / 2;
	w_max /= DISPLAY_ASPECT_WIDTH;
	h_max = (or .bottom - or .top) - h_ofs + DISPLAY_SCREEN_HEIGHT / 2;
	h_max /= DISPLAY_SCREEN_HEIGHT;

	if(w_max < h_max) {
		h_max = w_max;
	} else {
		w_max = h_max;
	}

	// current scaler
	width = selr->right - selr->left + DISPLAY_ASPECT_WIDTH / 2;
	height = selr->bottom - selr->top + DISPLAY_SCREEN_HEIGHT / 2;

	width -= w_ofs; height -= h_ofs;
	width /= DISPLAY_ASPECT_WIDTH; height /= DISPLAY_SCREEN_HEIGHT;

	if(wParam == WMSZ_LEFT || wParam == WMSZ_RIGHT) {
		height = width;
	} else if(wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM) {
		width = height;
	}

	if(width < 1 || height < 1) {
		width = 1; height = 1;
	}

	if(width > w_max || height > h_max) {
		width = w_max; height = h_max;
	}

	if(width > MAX_SCALE || height > MAX_SCALE) {
		width = MAX_SCALE; height = MAX_SCALE;
	}

	if(width < height) {
		height = width;
	} else {
		width = height;
	}

	width *= DISPLAY_ASPECT_WIDTH; height *= DISPLAY_SCREEN_HEIGHT;
	width += w_ofs; height += h_ofs;

	// Set window size
	if(wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT) {
		selr->top = selr->bottom - height;
	} else {
		selr->bottom = selr->top + height;
	}
	if(wParam == WMSZ_LEFT || wParam == WMSZ_TOPLEFT || wParam == WMSZ_BOTTOMLEFT) {
		selr->left = selr->right - width;
	} else {
		selr->right = selr->left + width;
	}
	*/
}

void zxFrame::onDestroy() {
	PostQuitMessage(0);
}

void zxFrame::onClose() {
	setOrGetWndPos(getHWND(), OPT_WND_MAIN_POS, true, false);
	setOrGetWndPos(debug->getHWND(), OPT_WND_DEBUG_POS, true, false);
	setOrGetWndPos(keyboard->getHWND(), OPT_WND_KEY_POS, true, false);

	opts.save(opts.mainDir + L"settings.zx");
	saveState(opts.mainDir + L"auto_state.zx");

	modifyTSTATE(ZX_TERMINATE, 0);

	DestroyWindow(hWnd);
}

void zxFrame::onOpen() {
	zxString folder(getOpt(OPT_CUR_PATH)->sval);
	if(wmId == IDM_OPEN)
		wmId = dlgSaveOrOpenFile(true, L"Открыть", L"Снепшот\0*.z80\0Состояние\0*.zx\0", L"z80", folder);
	if(wmId) {
		zxString ext(folder.substr(folder.find_rev(L'.') + 1).lower());
		bool result = false;
		if(ext == L"z80") {
			result = loadZ80(folder);
		} else if(ext == L"zx") {
			result = loadState(folder);
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
	if(dlgSaveOrOpenFile(false, L"Сохранить", L"Снепшот\0*.z80\0Состояние\0*.zx\0Скриншот\0*.tga\0", L"z80", folder)) {
		zxString ext(folder.substr(folder.find_rev(L'.') + 1).lower());
		bool result = false;
		if(ext == L"z80") {
			result = saveZ80(folder);
		} else if(ext == L"zx") {
			result = saveState(folder);
		} else if(ext == L"tga") {
			result = bus.saveScreen(folder);
		}
		if(!result) MessageBox(hWnd, L"Не удалось выполнить операцию!", L"Ошибка", MB_ICONERROR);
		else getOpt(OPT_CUR_PATH)->sval = folder.left(folder.find_rev(L'\\') + 1);
	}
}

void zxFrame::onRestore() {
	if(!loadState(opts.mainDir + L"auto_state.zx")) {
		MessageBox(hWnd, L"Не удалось загрузить состояние машины!", L"Ошибка", MB_ICONERROR);
		modifyTSTATE(ZX_RESET, 0);
	}
	checkedSubMenu(hMenu, OPT_MEM_MODEL, -1, idsModel);
}

void zxFrame::onProcessMRU() {
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.cch = MAX_PATH;
	mii.dwTypeData = tmpBuf;
	if(GetMenuItemInfo(hMenuMRU, wmId, FALSE, &mii)) {
		getOpt(OPT_CUR_PATH)->sval = mii.dwTypeData;
		onOpen();
	}
}

BOOL zxFrame::onEraseBkgnd(HDC hdc) {
	return FALSE;
}

int zxFrame::onCalcSize(bool isParams, LPARAM lParam) {
	return 0;
	/*
	int mode = getOpt(OPT_ASPECT_RATIO)->dval;
	int cx = 288, cy = 224;
	switch(mode) {
	case AR_2X: cx *= 2; cy *= 2; break;
	case AR_3X: cx *= 3; cy *= 3; break;
	case AR_4X: cx *= 4; cy *= 4; break;
	case AR_AS_IS: return 0;
	}
	if(isParams) {
	auto lp = (LPNCCALCSIZE_PARAMS)lParam;
	LPRECT r = &lp->rgrc[1];
	RECT r1;
	SetRect(&lp->rgrc[2], r->left, r->top, r->right, r->bottom);

	SetRect(&r1, r->left, r->top, r->left + cx + 16, r->top + cy + 50);
	SetRect(&lp->rgrc[0], r1.left + 8, r1.top + 50, r1.right - 8, r1.bottom - 8);
	memcpy(&lp->rgrc[1], &r1, sizeof(RECT));
	return WVR_VALIDRECTS;
	}
	auto lp = (LPRECT)lParam;
	lp->right = lp->left + cx + 8;
	lp->bottom = lp->top + cy + 8;
	return 0;
	*/
}

void zxFrame::onSize(UINT type, int nWidth, int nHeight) {
	if(hWndToolbar) {
		SendMessage(hWndToolbar, WM_SIZE, type, MAKELONG(nWidth, nHeight));
		GetWindowRect(hWndToolbar, &wndRect);
		wndRect.top = wndRect.bottom - wndRect.top;
		wndRect.left = 0;
		wndRect.right = nWidth;
		wndRect.bottom = nHeight;
	}
}

void zxFrame::onNotify(LPNMHDR nm, LRESULT* pResult) {
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
	gamepad = nullptr;
	hMenuPP = nullptr;
	hMenuModel = nullptr;
	hCpuThread = nullptr;
	hTimer = 0;
	debug = nullptr;
}

zxFrame::~zxFrame() {
	pauseCPU(true);
	KillTimer(hWnd, hTimer);

	SAFE_DELETE(debug);
	SAFE_DELETE(keyboard);
	SAFE_DELETE(gamepad);
	CloseHandle(hCpuThread);
	DeleteCriticalSection(&cs);
}

void zxFrame::makeWndSize(SIZE* sz) {
	RECT rc;
	int mul = (getOpt(OPT_ASPECT_RATIO)->dval) + 1;
	if(mul > 4) mul = 2;
	sz->cx = 320 * mul;
	sz->cy = (256 * mul) + 46;
	if(hWnd) {
		GetWindowRect(hWnd, &rc);
		rc.right += sz->cx;
		rc.bottom += sz->cy;
	} else {
		rc.left = CW_USEDEFAULT;
		rc.top = CW_USEDEFAULT;
		rc.right = sz->cx;
		rc.bottom = sz->cy;
	}
	//std::chrono::microseconds(50);
	AdjustWindowRect(&rc, GetWindowLong(hWnd, GWL_STYLE), TRUE);
}

int zxFrame::run() {

	opts.load(opts.mainDir + L"settings.zx");
	
	SIZE sz;
	makeWndSize(&sz);

	if(!create(L"EmuWnd", L"EmuWnd", WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, sz.cx, sz.cy, nullptr, 0, IDC_ZXEMUWND))
		return -1;

	if(!bus.changeModel(getOpt(OPT_MEM_MODEL)->dval, 255))
		return -1;

	makeToolbar(IDB_TOOLBAR_COMMON, tbb, 14, 20, 32, 32, IDT_TOOLBAR_MAIN);

	ShowWindow(hWnd, true);
	UpdateWindow(hWnd);

	hMenu = GetMenu(hWnd);
	hMenuMRU = GetSubMenu(GetSubMenu(hMenu, 0), 7);
	hMenuModel = GetSubMenu(GetSubMenu(hMenu, 2), 6);
	hMenuPP = GetSubMenu(GetSubMenu(hMenu, 1), 3);

	debug = new zxDebugger;
	debug->create(IDD_DIALOG_DEBUGGER, hWnd, false);

	keyboard = new zxKeyboard;
	keyboard->create(IDD_DIALOG_KEYBOARD, hWnd, false);

	gamepad = new zxGamepad;
	gamepad->init();

	loadState(opts.mainDir + L"auto_state.zx");

	setOrGetWndPos(debug->getHWND(), OPT_WND_DEBUG_POS, false, false);
	setOrGetWndPos(keyboard->getHWND(), OPT_WND_KEY_POS, false, false);
	setOrGetWndPos(hWnd, OPT_WND_MAIN_POS, false, true);

	updateData(getOpt(OPT_EXECUTE)->dval * STS_EXECUTE | ST_EXECUTE | ST_EXECUTE_GO);

	for(int i = 9; i >= 0; i--) modifyMRU(getOpt(OPT_MRU9)->sval);

//	DWORD cpuID;
//	hCpuThread = CreateThread(nullptr, 0, ProcBus, nullptr, 0, &cpuID);
	
	hTimer = SetTimer(hWnd, 10000, 20, timerProc);

	SetActiveWindow(hWnd);

	while(true) {
//		while(!::PeekMessage(&thState.msg, NULL, NULL, NULL, PM_NOREMOVE)) {
//			theApp->bus.execute();
//		}
		if(!msgPump()) break;
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
		for(int i = 0; i < count; i++) {
			mii.fMask = MIIM_ID;
			mii.wID = 1000 + i;
			SetMenuItemInfo(hMenuMRU, i, TRUE, &mii);
		}
	}
}

bool zxFrame::saveState(ssh_cws path) {
	bool result = false;

	pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(_hf != -1) {
			// сохраняем состояние шины
			if(!bus.saveState(_hf)) throw(0);
			// имя загруженной проги
			zxString* progs(&opts.nameLoadProg);
			auto l = (int)(progs->length() + 1) * 2;
			if(_write(_hf, progs->buffer(), l) != l) throw(0);
			result = true;
		}
	} catch(...) { 
		result = false; 
	}

	SAFE_CLOSE1(_hf);

	pauseCPU(false);

	return result;
}

bool zxFrame::loadState(ssh_cws path) {
	bool result = false;
	int oldModel = *_MODEL;

	pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_RDONLY | _O_BINARY, _SH_DENYRD, _S_IREAD);
		if(_hf != -1) {
			auto l = _filelength(_hf);
			ssh_b tmpRegs[COUNT_REGS];
			if(_read(_hf, tmpRegs, COUNT_REGS) != COUNT_REGS) throw(0);
			// меняем иодель
			if(!bus.changeModel(tmpRegs[MODEL], oldModel)) throw(0);
			// грузим состояние
			if(!bus.loadState(_hf)) throw(0);
			// грузим имя сохраненной проги
			l -= _tell(_hf);
			if(_read(_hf, tmpBuf, l) != l) throw(0);
			opts.nameLoadProg = tmpBuf;
			result = true;
		}
	} catch(...) {
		result = false;
	}

	SAFE_CLOSE1(_hf);

	pauseCPU(false);

	return result;
}
