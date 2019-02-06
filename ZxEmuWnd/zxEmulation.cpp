
#include "stdafx.h"

#include "CpuZX.h"
#include "GpuZX.h"
#include "SoundZX.h"
#include "zxDebugger.h"
#include "zxKeyboard.h"
#include "zxGamepad.h"
#include "zxDlgSettings.h"
#include "Z80.h"

static ssh_w old_tstate = 0;
static int idsModel[] = {IDM_48K, IDM_128K, 0};
static int idsPP[] = {IDM_PP_NONE, IDM_PP_MIXED, IDM_PP_BILINEAR, 0};
static int idsAR[] = {IDM_1X, IDM_2X, IDM_3X, IDM_4X, IDM_AS_IS, 0};

static CRITICAL_SECTION cs;

HMENU hMenu = nullptr;
HINSTANCE hInst = nullptr;

static ssh_d WINAPI ProcCPU(void* params) {
	return theApp.procCPU();
}

void setOrGetWndPos(HWND h, int id_opt, bool get, bool activate) {
	RECT r;
	auto o = theApp.getOpt(id_opt);

	if(get) {
		if(GetWindowRect(h, &r))
			theApp.getOpt(id_opt)->sval = StringZX::fmt(L"%d,%d,%d,%d", r.left, r.top, r.right, r.bottom);
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
	_TSTATE &= ~remove;
	_TSTATE |= adding;
	LeaveCriticalSection(&cs);
	return _TSTATE;
}

void zxEmulation::pauseCPU(bool isPause) {
	if(isPause) {
		old_tstate = _TSTATE;
		modifyTSTATE(0, ZX_EXEC);
		Wow64SuspendThread(hCpuThread);
	} else {
		modifyTSTATE(old_tstate, 0xffff);
		ResumeThread(hCpuThread);
	}
}

bool dlgSaveOrOpenFile(bool bOpen, ssh_cws title, ssh_cws filter, ssh_cws defExt, StringZX& folder) {
	bool result;
	static ssh_ws flt[MAX_PATH];

	int flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));

	if(bOpen) flags |= OFN_FILEMUSTEXIST;

	auto files = new ssh_ws[MAX_PATH];
	memset(files, 0, MAX_PATH);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = theApp;

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

bool zxEmulation::changeState(int id_opt, int id, bool change) {
	auto opt = getOpt(id_opt);
	if(change) opt->dval = !opt->dval;
	bool v = (bool)opt->dval;
	CheckMenuItem(hMenu, id, v << 3);
	if(hWndToolbar) SendMessage(hWndToolbar, TB_SETSTATE, id, TBSTATE_ENABLED | (int)v);
	return v;
}

void zxEmulation::changeTurbo(bool change) {
	delayCPU = getOpt(OPT_DELAY_CPU)->dval;
	delayGPU = getOpt(OPT_DELAY_GPU)->dval;
	if(changeState(OPT_TURBO, IDM_TURBO, change)) {
		delayCPU /= 2;
		delayGPU /= 2;
	}
}

void zxEmulation::changeSound(bool change) {
	changeState(OPT_SOUND, IDM_SOUND, change);
}

void zxEmulation::changeWndDebugger(bool change) {
	debug->show(changeState(OPT_DEBUGGER, IDM_DEBUGGER, change));
}

void zxEmulation::changeWndKeyboard(bool change) {
	keyboard->show(changeState(OPT_KEYBOARD, IDM_KEYBOARD, change));
}

void zxEmulation::changeExecute(bool change) {
	if(change) modifyTSTATE(_TSTATE ^ ZX_EXEC, ZX_EXEC);
	CheckMenuItem(hMenu, IDM_PAUSE, _TSTATE & ZX_EXEC);
	if(hWndToolbar) SendMessage(hWndToolbar, TB_SETSTATE, IDM_PAUSE, TBSTATE_ENABLED | ((_TSTATE & ZX_EXEC) >> 3));
	if(change) debug->setProgrammPause((_TSTATE & ZX_EXEC) != ZX_EXEC, false);
}

BEGIN_MSG_MAP(zxEmulation, zxWnd)
	ON_NOTIFY(TBN_DROPDOWN, IDT_TOOLBAR_MAIN, onNotify)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(IDM_SETTINGS, onSettings)
	ON_COMMAND(IDM_RESET, onReset)
	ON_COMMAND(IDM_RESTORE, onRestore)
	ON_COMMAND(IDM_EXIT, onClose)
	ON_COMMAND(IDM_OPEN, onOpen)
	ON_COMMAND(IDM_SAVE, onSave)
	ON_COMMAND(IDM_PAUSE, onPause)
	ON_COMMAND(IDM_SOUND, onSound)
	ON_COMMAND(IDM_TURBO, onTurbo)
	ON_COMMAND(IDM_DEBUGGER, onDebugger)
	ON_COMMAND(IDM_KEYBOARD, onKeyboard)
	ON_COMMAND_RANGE(IDM_48K, IDM_128K, onFilter)
	ON_COMMAND_RANGE(IDM_1X, IDM_AS_IS, onAspectRatio)
	ON_COMMAND_RANGE(IDM_PP_NONE, IDM_PP_BILINEAR, onFilter)
	ON_COMMAND_RANGE(1000, 1009, onProcessMRU)
END_MSG_MAP()

void zxEmulation::onSettings() {
	zxDlgSettings dlg;
	dlg.create(IDD_DIALOG_SETTINGS, 0, this, true);
}

void zxEmulation::onReset() {
	modifyTSTATE(ZX_RESET, 0);
}

void zxEmulation::onClose() {
	setOrGetWndPos(getHWND(), OPT_WND_MAIN_POS, true, false);
	setOrGetWndPos(debug->getHWND(), OPT_WND_DEBUG_POS, true, false);
	setOrGetWndPos(keyboard->getHWND(), OPT_WND_KEY_POS, true, false);

	opts.save(opts.mainDir + L"settings.zx");
	zilog->saveStateZX(opts.mainDir + L"auto_state.zx");

	modifyTSTATE(ZX_TERMINATE, 0);
	TerminateThread(hCpuThread, 0);

	DestroyWindow(hWnd);
	PostQuitMessage(0);
}

void zxEmulation::onPause() {
	changeExecute(true);
}

void zxEmulation::onTurbo() {
	changeTurbo(true);
}

void zxEmulation::onSound() {
	changeSound(true);
}

void zxEmulation::onDebugger() {
	changeWndDebugger(true);
}

void zxEmulation::onKeyboard() {
	changeWndKeyboard(true);
}

void zxEmulation::onFilter() {
	checkedSubMenu(hMenu, OPT_PP, wmId - IDM_PP_NONE, idsPP);
}

void zxEmulation::onModel() {
	checkedSubMenu(hMenu, OPT_MEM_MODEL, wmId - IDM_48K, idsModel);
}

void zxEmulation::onAspectRatio() {
	checkedSubMenu(hMenu, OPT_ASPECT_RATIO, wmId - IDM_1X, idsAR);
}

void zxEmulation::onOpen() {
	StringZX folder(getOpt(OPT_CUR_PATH)->sval);
	if(wmId == IDM_OPEN)
		wmId = dlgSaveOrOpenFile(true, L"Открыть", L"Снепшот\0*.z80\0Состояние\0*.zx\0", L"z80", folder);
	if(wmId) {
		StringZX ext(folder.substr(folder.find_rev(L'.') + 1).lower());
		bool result = false;
		if(ext == L"z80") {
			result = loadZ80(folder);
		} else if(ext == L"zx") {
			result = zilog->loadStateZX(folder);
		}
		if(!result) MessageBox(hWnd, L"Не удалось выполнить операцию!", L"Ошибка", MB_OK);
		else {
			auto pos = folder.find_rev(L'\\') + 1;
			getOpt(OPT_CUR_PATH)->sval = folder.left(pos);
			opts.nameLoadProg = folder.substr(pos, folder.find_rev(L'.') - pos);
			modifyMRU(folder);
		}
	}
}

void zxEmulation::onSave() {
	StringZX folder(getOpt(OPT_CUR_PATH)->sval);
	if(dlgSaveOrOpenFile(false, L"Сохранить", L"Снепшот\0*.z80\0Состояние\0*.zx\0Скриншот\0*.tga\0", L"z80", folder)) {
		StringZX ext(folder.substr(folder.find_rev(L'.') + 1).lower());
		bool result = false;
		if(ext == L"z80") {
			result = saveZ80(folder);
		} else if(ext == L"zx") {
			result = zilog->saveStateZX(folder);
		} else if(ext == L"tga") {
			result = gpu->saveScreen(folder);
		}
		if(!result) MessageBox(hWnd, L"Не удалось выполнить операцию!", L"Ошибка", MB_ICONERROR);
		else getOpt(OPT_CUR_PATH)->sval = folder.left(folder.find_rev(L'\\') + 1);
	}
}

void zxEmulation::onRestore() {
	if(!zilog->loadStateZX(opts.mainDir + L"auto_state.zx")) {
		MessageBox(hWnd, L"Не удалось загрузить состояние машины!", L"Ошибка", MB_ICONERROR);
		modifyTSTATE(ZX_RESET, 0);
	}
	checkedSubMenu(hMenu, OPT_MEM_MODEL, -1, idsModel);
}

void zxEmulation::onProcessMRU() {
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.cch = MAX_PATH;
	mii.dwTypeData = tmpBuf;
	if(GetMenuItemInfo(hMenuMRU, wmId, FALSE, &mii)) {
		getOpt(OPT_CUR_PATH)->sval = mii.dwTypeData;
		onOpen();
	}
}

BOOL zxEmulation::onEraseBkgnd(HDC hdc) {
	return FALSE;
}

int zxEmulation::onCalcSize(bool isParams, LPARAM lParam) {
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

void zxEmulation::onSize(UINT type, int nWidth, int nHeight) {
	if(hWndToolbar) {
		SendMessage(hWndToolbar, WM_SIZE, type, MAKELONG(nWidth, nHeight));
		::GetWindowRect(hWndToolbar, &wndRect);
		wndRect.top = wndRect.bottom - wndRect.top;
		wndRect.left = 0;
		wndRect.right = nWidth;
		wndRect.bottom = nHeight;
	}
}

void zxEmulation::processKeys() {
	if(GetKeyboardState(keyboard->vkKeys))
		keyboard->processKeys();
}

void zxEmulation::onKeyDown(UINT nVirtKey, UINT nRepeat, UINT nFlags) { processKeys(); }
void zxEmulation::onKeyUp(UINT nVirtKey, UINT nRepeat, UINT nFlags) { processKeys(); }

void zxEmulation::onNotify(LPNMHDR nm, LRESULT* pResult) {
	TPMPARAMS tpm;
	RECT rc;

	LPNMTOOLBAR lpnmTB = (LPNMTOOLBAR)nm;
	auto id = lpnmTB->iItem;
	SendMessage(lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)id, (LPARAM)&rc);
	MapWindowPoints(lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);

	HMENU hPopupMenu = nullptr;
	int* ids = nullptr;
	int id_opt = -1;
	switch(id) {
		case IDM_MODEL: ids = idsModel; id_opt = OPT_MEM_MODEL; hPopupMenu = hMenuModel; break;
		case IDM_OPEN: hPopupMenu = hMenuMRU; break;
		case IDM_FILTER: ids = idsPP; id_opt = OPT_PP; hPopupMenu = hMenuPP; break;
	}
	if(id_opt != -1) checkedSubMenu(hPopupMenu, id_opt, -1, ids);

	tpm.cbSize = sizeof(TPMPARAMS);
	tpm.rcExclude = rc;

	TrackPopupMenuEx(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, hWnd, &tpm);
}

static TBBUTTON tbb[] = {
	{0, IDM_OPEN, TBSTATE_ENABLED, BTNS_DROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Открыть"},
	{1, IDM_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Сохранить"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{2, IDM_RESET, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Сброс"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{3, IDM_RESTORE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Восстановить состояние"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{4, IDM_PAUSE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Остановить\\Возобновить эмуляцию"},
	{5, IDM_SOUND, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Вкл\\Выкл звук"},
	{6, IDM_KEYBOARD, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Показать\\Скрыть клавиатуру"},
	{7, IDM_DEBUGGER, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Показать\\Скрыть дизассемблер"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{8, IDM_TURBO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Ускорить\\Замедлить"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{11, IDM_FILTER, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Фильтр"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{9, IDM_MODEL, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Модель памяти"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{10, IDM_SETTINGS, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Настройки"},
};

zxEmulation::zxEmulation() : zxWnd() {
	InitializeCriticalSection(&cs);
	hMenuMRU = nullptr;
	gamepad = nullptr;
	hMenuPP = nullptr;
	hMenuModel = nullptr;
	hCpuThread = nullptr;
	zilog = nullptr;
	gpu = nullptr;
	snd = nullptr;
	debug = nullptr;
}

zxEmulation::~zxEmulation() {
	pauseCPU(true);
	SAFE_DELETE(debug);
	SAFE_DELETE(snd);
	SAFE_DELETE(gpu);
	SAFE_DELETE(zilog);
	SAFE_DELETE(gamepad);
	DeleteCriticalSection(&cs);
}

ssh_d zxEmulation::procCPU() {
	LARGE_INTEGER sample;
	QueryPerformanceFrequency(&sample);

	DWORD ms = sample.LowPart / 1000;

	QueryPerformanceCounter(&sample);

	ssh_u sndTm = 0;
	ssh_u startCpu = sample.LowPart;
	ssh_u startBrd = startCpu;
	ssh_u startGpu = startCpu;

	while(!(_TSTATE & ZX_TERMINATE)) {
		QueryPerformanceCounter(&sample);
		ssh_u current = sample.LowPart;
		if((current - startGpu) > (ms * delayGPU)) {
			keyboard->processJoystick();
			gpu->showScreen();
			_TSTATE |= ZX_TRAP;
			startGpu = current;
		}
		if((_TSTATE & ZX_EXEC)) {
			if((current - startCpu) > delayCPU) {
				zilog->execute(false); startCpu = current;
				if(getOpt(OPT_SOUND)->dval) snd->execute(sndTm++);
			}
			if((current - startBrd) > (delayCPU * 16)) {
				gpu->execute();
				startBrd = current;
			}
		}
	}

	return 0;
}

int zxEmulation::run() {
	if(!create(L"EmuWnd", L"EmuWnd", WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 554, nullptr, 0, IDC_ZXEMUWND))
		return -1;

	makeToolbar(IDB_TOOLBAR_COMMON, tbb, 12, 19, 32, 32, IDT_TOOLBAR_MAIN);

	ShowWindow(hWnd, true);
	UpdateWindow(hWnd);

	opts.load(opts.mainDir + L"settings.zx");

	hMenu = GetMenu(hWnd);
	hMenuMRU = GetSubMenu(GetSubMenu(hMenu, 0), 7);
	hMenuModel = GetSubMenu(GetSubMenu(hMenu, 2), 4);
	hMenuPP = GetSubMenu(GetSubMenu(hMenu, 1), 3);

	debug = new zxDebugger;
	debug->create(IDD_DIALOG_DEBUGGER, IDA_ACCEL_DEBUGGER, this, false);

	keyboard = new zxKeyboard;
	keyboard->create(IDD_DIALOG_KEYBOARD, 0, this, false);

	zilog = new CpuZX;

	setOrGetWndPos(debug->getHWND(), OPT_WND_DEBUG_POS, false, false);
	setOrGetWndPos(keyboard->getHWND(), OPT_WND_KEY_POS, false, false);
	setOrGetWndPos(hWnd, OPT_WND_MAIN_POS, false, true);

	changeSound(false);
	changeWndDebugger(false);
	changeWndKeyboard(false);
	changeTurbo(false);
	changeExecute(false);

	checkedSubMenu(hMenu, OPT_MEM_MODEL, -1, idsModel);
	checkedSubMenu(hMenu, OPT_PP, -1, idsPP);
	checkedSubMenu(hMenu, OPT_ASPECT_RATIO, -1, idsAR);

	for(int i = 9; i >= 0; i--) modifyMRU(getOpt(OPT_MRU9)->sval);

	zilog->loadStateZX(opts.mainDir + L"auto_state.zx");

	gamepad = new zxGamepad;
	gamepad->init(hWnd);
	gamepad->changeMode(0, zxGamepad::KEMPSTON, nullptr, 0);

	gpu = new GpuZX;
	snd = new SoundZX;

	DWORD cpuID;
	hCpuThread = CreateThread(nullptr, 0, ProcCPU, nullptr, 0, &cpuID);

	SetActiveWindow(hWnd);
		
	HACCEL hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_ZXEMUWND));
	MSG msg;

	while(SshMsgPump(&msg, nullptr)) { }
	return (int)msg.wParam;
}

void zxEmulation::modifyMRU(StringZX path) {
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

bool zxEmulation::checkedSubMenu(HMENU hMenu, int id_opt, int val, int* ids) {
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
