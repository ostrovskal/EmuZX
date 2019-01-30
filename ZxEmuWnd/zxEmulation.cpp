#include "stdafx.h"
#include "ZxTimer.h"
#include "CpuZX.h"
#include "GpuZX.h"
#include "BorderZX.h"
#include "SoundZX.h"
#include "KeyBoardZX.h"
#include "zxDebugger.h"
#include "zxKeyboard.h"
#include "Z80.h"

static ssh_w old_tstate = 0;
static int idsModel[] = {IDM_48K, IDM_128K, 0};
static int idsPP[] = {IDM_PP_NONE, IDM_PP_MIXED, IDM_PP_BILINEAR, 0};
static CRITICAL_SECTION cs;

HMENU hMenu = nullptr;
HINSTANCE hInst = nullptr;

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
	{11, IDM_POST_PROCESS, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Фильтр"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{9, IDM_MODEL, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Модель памяти"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{10, IDM_SETTINGS, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Настройки"},
};

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
			r.left		= _wtoi(pos[0]);
			r.top		= _wtoi(pos[1]);
			r.right		= _wtoi(pos[2]);
			r.bottom	= _wtoi(pos[3]);
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

void pauseCPU(bool isPause, int adding) {
	if(isPause) {
		old_tstate = _TSTATE;
		modifyTSTATE(0, ZX_EXEC);
		Sleep(100);
	} else {
		modifyTSTATE(old_tstate | adding, 0xffff);
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

static ssh_d WINAPI ProcCPU(void* params) {
	return theApp.procCPU();
}

zxEmulation::zxEmulation() : zxWnd() {
	InitializeCriticalSection(&cs);
	hWndToolbar = nullptr;
	hMenuMRU = nullptr;
	hMenuPP = nullptr;
	hMenuModel = nullptr;
	hCpuThread = nullptr;
	zilog = nullptr;
	gpu = nullptr;
	brd = nullptr;
	snd = nullptr;
	debug = nullptr;
}

zxEmulation::~zxEmulation() {
	pauseCPU(true, 0);
	CloseHandle(hCpuThread);
	SAFE_DELETE(debug);
	SAFE_DELETE(snd);
	SAFE_DELETE(brd);
	SAFE_DELETE(gpu);
	SAFE_DELETE(zilog);
	DeleteCriticalSection(&cs);
}

ssh_d zxEmulation::procCPU() {
	ZxTimer tm;

	tm.start();

	DWORD sndTm = 0;
	DWORD startCpu = tm.samplePerformanceCounter();
	DWORD startBrd = startCpu;
	DWORD startGpu = startCpu / tm.millisecondsFrequency;

	while(!(_TSTATE & ZX_TERMINATE)) {
		DWORD current = tm.samplePerformanceCounter();
		DWORD millis = current / tm.millisecondsFrequency;
		if((_TSTATE & ZX_EXEC)) {
			if((current - startCpu) > delayCPU) {
				zilog->execute(false); startCpu = current;
				if(getOpt(OPT_SOUND)->dval) snd->execute(sndTm++);
			}
		}
		if((current - startBrd) > (delayCPU * 16)) { brd->execute(); startBrd = current; }
		if((millis - startGpu) > delayGPU) { gpu->execute(); startGpu = millis; }
	}
	tm.stop();
	modifyTSTATE(0, ZX_TERMINATE);

	return 0;
}

bool zxEmulation::onCommand(int wmId, int param, LPARAM lParam) {
	StringZX folder(getOpt(OPT_CUR_PATH)->sval);
	// Разобрать выбор в меню:
	switch(wmId) {
		case 1000: case 1001: case 1002: case 1003: case 1004:
		case 1005: case 1006: case 1007: case 1008: case 1009:
			mii.fMask = MIIM_TYPE;
			mii.fType = MFT_STRING;
			mii.cch = MAX_PATH;
			mii.dwTypeData = tmpBuf;
			if(!GetMenuItemInfo(hMenuMRU, wmId, FALSE, &mii)) break;
			folder = mii.dwTypeData;
			wmId = 1;
		case IDM_OPEN:
			if(wmId == IDM_OPEN) {
				wmId = dlgSaveOrOpenFile(true, L"Открыть", L"Снепшот\0*.z80\0Состояние\0*.zx\0", L"z80", folder);
			}
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
					getOpt(OPT_CUR_PATH)->sval = folder.left(folder.find_rev(L'\\') + 1);
					modifyMRU(folder);
				}
				break;
			}
		case IDM_SAVE:
			if(dlgSaveOrOpenFile(false, L"Сохранение", L"Снепшот\0*.z80\0Состояние\0*.zx\0Экран\0*.tga\0", L"z80", folder)) {
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
			break;
		case IDM_RESTORE:
			if(!zilog->loadStateZX(opts.mainDir + L"auto_state.zx")) {
				MessageBox(hWnd, L"Не удалось загрузить состояние машины!", L"Ошибка", MB_ICONERROR);
				modifyTSTATE(ZX_RESET, 0);
			}
			checkedModelOrPP(hMenu, OPT_MEM_MODEL, -1, idsModel);
			break;
		case IDM_TURBO: changeTurbo(true); break;
		case IDM_SOUND: changeSound(true); break;
		case IDM_PAUSE: changeExecute(true); break;
		case IDM_DEBUGGER: changeWndDebugger(true); break;
		case IDM_KEYBOARD: changeWndKeyboard(true); break;
		case IDM_PP_NONE:
		case IDM_PP_MIXED:
		case IDM_PP_BILINEAR: checkedModelOrPP(hMenu, OPT_PP, wmId - IDM_PP_NONE, idsPP); break;
		case IDM_48K:
		case IDM_128K: if(!checkedModelOrPP(hMenu, OPT_MEM_MODEL, wmId - IDM_48K, idsModel)) break;
		case IDM_RESET: modifyTSTATE(ZX_RESET, 0); break;
		case IDM_EXIT: DestroyWindow(hWnd); break;
		default: return false;
	}
	return true;
}

bool zxEmulation::onClose() {
	setOrGetWndPos(getHWND(), OPT_WND_MAIN_POS, true, false);
	setOrGetWndPos(debug->getHWND(), OPT_WND_DEBUG_POS, true, false);
	setOrGetWndPos(keyboard->getHWND(), OPT_WND_KEY_POS, true, false);
	return true;
}

bool zxEmulation::onSize(WPARAM type, int nWidth, int nHeight) {
	SendMessage(hWndToolbar, WM_SIZE, type, MAKELONG(nWidth, nHeight));
	::GetWindowRect(hWndToolbar, &wndRect);
	wndRect.top = wndRect.bottom - wndRect.top;
	wndRect.left = 0;
	wndRect.right = nWidth;
	wndRect.bottom = nHeight;
	return true;
}

bool zxEmulation::onKey(int nVirtKey, LPARAM keyData, bool pressed) {
	if(keyb) keyb->execute((ssh_b)nVirtKey, pressed);
	return keyb == nullptr;
}

bool zxEmulation::onNotify(LPNMHDR nm) {
	switch(nm->code) {
		case TBN_DROPDOWN: {
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
				case IDM_POST_PROCESS: ids = idsPP; id_opt = OPT_PP; hPopupMenu = hMenuPP; break;
			}
			if(id_opt != -1) checkedModelOrPP(hPopupMenu, id_opt, -1, ids);

			tpm.cbSize = sizeof(TPMPARAMS);
			tpm.rcExclude = rc;

			TrackPopupMenuEx(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, hWnd, &tpm);
			return true;
		}
	}
	return false;
}

void zxEmulation::postCreate() {

}

int zxEmulation::run() {
	RECT rc;
	::SetRect(&rc, CW_USEDEFAULT, CW_USEDEFAULT, 640, 554);
	if(!create(L"EmuWnd", L"EmuWnd", WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, rc, nullptr, 0, IDC_ZXEMUWND))
		return -1;

	makeToolbar(IDB_TOOLBAR_COMMON, tbb, 12, 19, 32, 32);

	ShowWindow(hWnd, true);
	UpdateWindow(hWnd);

	opts.load(opts.mainDir + L"settings.zx");

	hMenu = GetMenu(hWnd);
	hMenuMRU = GetSubMenu(GetSubMenu(hMenu, 0), 7);
	hMenuModel = GetSubMenu(GetSubMenu(hMenu, 1), 9);
	hMenuPP = GetSubMenu(GetSubMenu(hMenu, 1), 7);

	debug = new zxDebugger;
	debug->create(IDD_DIALOG_DEBUGGER, this, false);

	keyboard = new zxKeyboard;
	keyboard->create(IDD_DIALOG_KEYBOARD, this, false);

	zilog = new CpuZX;

	setOrGetWndPos(debug->getHWND(), OPT_WND_DEBUG_POS, false, false);
	setOrGetWndPos(keyboard->getHWND(), OPT_WND_KEY_POS, false, false);
	setOrGetWndPos(hWnd, OPT_WND_MAIN_POS, false, true);

	changeSound(false);
	changeWndDebugger(false);
	changeWndKeyboard(false);
	changeTurbo(false);
	changeExecute(false);

	checkedModelOrPP(hMenu, OPT_MEM_MODEL, -1, idsModel);
	checkedModelOrPP(hMenu, OPT_PP, -1, idsPP);
	
	for(int i = 9; i >= 0; i--) modifyMRU(getOpt(OPT_MRU9)->sval);

	if(!zilog->loadStateZX(opts.mainDir + L"auto_state.zx")) {
		MessageBox(hWnd, L"Не удалось восстановить состояние машины!", L"Ошибка", MB_ICONERROR);
		modifyTSTATE(ZX_RESET, 0);
	}

	gpu = new GpuZX;
	brd = new BorderZX;
	snd = new SoundZX;
	keyb = new KeyboardZX;

	debug->updateRegisters(_PC, true, true);

	DWORD cpuID;
	hCpuThread = CreateThread(nullptr, 0, ProcCPU, nullptr, 0, &cpuID);

	HACCEL hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_ZXEMUWND));

	MSG msg;

	while(true) {
		while(!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) {
			// функция для работы пока нет сообщений
//			SendMsg(WM_IDLE, 0, 0);
		}
		if(!::GetMessage(&msg, NULL, NULL, NULL)) {
			break;
		}
		if(!TranslateAccelerator(nullptr, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	opts.save(opts.mainDir + L"settings.zx");
	zilog->saveStateZX(opts.mainDir + L"auto_state.zx");
	
	modifyTSTATE(ZX_TERMINATE, 0);
	while(!(_TSTATE & ZX_TERMINATE)) {}

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

bool zxEmulation::checkedModelOrPP(HMENU hMenu, int id_opt, int val, int* ids) {
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

bool zxEmulation::changeState(int id_opt, int id, bool change) {
	auto opt = getOpt(id_opt);
	if(change) opt->dval = !opt->dval;
	bool v = (bool)opt->dval;
	CheckMenuItem(hMenu, id, v << 3);
	SendMessage(hWndToolbar, TB_SETSTATE, id, TBSTATE_ENABLED | (int)v);
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
	SendMessage(hWndToolbar, TB_SETSTATE, IDM_PAUSE, TBSTATE_ENABLED | (_TSTATE >> 3));
}
