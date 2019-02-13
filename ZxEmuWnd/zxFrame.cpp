
#include "stdafx.h"

#include "zxBus.h"
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

static ssh_d WINAPI ProcBus(void* params) {
	return theApp->bus.execute();
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

bool zxFrame::changeState(int id_opt, int id, bool change) {
	auto opt = getOpt(id_opt);
	if(change) opt->dval = !opt->dval;
	bool v = (bool)opt->dval;
	CheckMenuItem(hMenu, id, v << 3);
	if(hWndToolbar) SendMessage(hWndToolbar, TB_SETSTATE, id, TBSTATE_ENABLED | (int)v);
	return v;
}

void zxFrame::changeTurbo(bool change) {
	bus.changeTurbo(changeState(OPT_TURBO, IDM_TURBO, change));
}

void zxFrame::changeSound(bool change) {
	changeState(OPT_SOUND, IDM_SOUND, change);
	bus.changeSound();
}

void zxFrame::changeWndDebugger(bool change) {
	debug->show(changeState(OPT_DEBUGGER, IDM_DEBUGGER, change));
}

void zxFrame::changeWndKeyboard(bool change) {
	keyboard->show(changeState(OPT_KEYBOARD, IDM_KEYBOARD, change));
}

void zxFrame::changeExecute(bool exec, bool change) {
	modifyTSTATE(exec * ZX_EXEC, ZX_EXEC);
	getOpt(OPT_EXECUTE)->dval = exec;
	CheckMenuItem(hMenu, IDM_PAUSE, exec << 3);
	if(hWndToolbar) SendMessage(hWndToolbar, TB_SETSTATE, IDM_PAUSE, TBSTATE_ENABLED | (int)exec);
	if(change) debug->setProgrammPause(!exec, false);
}

BEGIN_MSG_MAP(zxFrame, zxWnd)
	ON_NOTIFY(TBN_DROPDOWN, IDT_TOOLBAR_MAIN, onNotify)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
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
	ON_COMMAND_RANGE(IDM_48K, IDM_128K, onModel)
	ON_COMMAND_RANGE(IDM_1X, IDM_AS_IS, onAspectRatio)
	ON_COMMAND_RANGE(IDM_PP_NONE, IDM_PP_BILINEAR, onFilter)
	ON_COMMAND_RANGE(1000, 1009, onProcessMRU)
END_MSG_MAP()

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
	dlg.create(IDD_DIALOG_SETTINGS, this, true);
}

void zxFrame::onReset() {
	modifyTSTATE(ZX_RESET, 0);
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

void zxFrame::onPause() {
	changeExecute(!((*_TSTATE) & ZX_EXEC), true);
}

void zxFrame::onTurbo() {
	changeTurbo(true);
}

void zxFrame::onSound() {
	changeSound(true);
}

void zxFrame::onDebugger() {
	changeWndDebugger(true);
}

void zxFrame::onKeyboard() {
	changeWndKeyboard(true);
}

void zxFrame::onFilter() {
	checkedSubMenu(hMenu, OPT_PP, wmId - IDM_PP_NONE, idsPP);
}

void zxFrame::onModel() {
	checkedSubMenu(hMenu, OPT_MEM_MODEL, wmId - IDM_48K, idsModel);
	bus.changeModel(wmId - IDM_48K, *_MODEL);
}

void zxFrame::onAspectRatio() {
	RECT r;
	if(GetWindowRect(hWnd, &r)) {
		checkedSubMenu(hMenu, OPT_ASPECT_RATIO, wmId - IDM_1X, idsAR);
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

void zxFrame::onOpen() {
	zxString folder(getOpt(OPT_CUR_PATH)->sval);
	if(wmId == IDM_OPEN)
		wmId = dlgSaveOrOpenFile(true, L"�������", L"�������\0*.z80\0���������\0*.zx\0", L"z80", folder);
	if(wmId) {
		zxString ext(folder.substr(folder.find_rev(L'.') + 1).lower());
		bool result = false;
		if(ext == L"z80") {
			result = loadZ80(folder);
		} else if(ext == L"zx") {
			result = loadState(folder);
		}
		if(!result) MessageBox(hWnd, L"�� ������� ��������� ��������!", L"������", MB_OK);
		else {
			auto pos = folder.find_rev(L'\\') + 1;
			getOpt(OPT_CUR_PATH)->sval = folder.left(pos);
			opts.nameLoadProg = folder.substr(pos, folder.find_rev(L'.') - pos);
			modifyMRU(folder);
		}
	}
}

void zxFrame::onSave() {
	zxString folder(getOpt(OPT_CUR_PATH)->sval);
	if(dlgSaveOrOpenFile(false, L"���������", L"�������\0*.z80\0���������\0*.zx\0��������\0*.tga\0", L"z80", folder)) {
		zxString ext(folder.substr(folder.find_rev(L'.') + 1).lower());
		bool result = false;
		if(ext == L"z80") {
			result = saveZ80(folder);
		} else if(ext == L"zx") {
			result = saveState(folder);
		} else if(ext == L"tga") {
			result = bus.saveScreen(folder);
		}
		if(!result) MessageBox(hWnd, L"�� ������� ��������� ��������!", L"������", MB_ICONERROR);
		else getOpt(OPT_CUR_PATH)->sval = folder.left(folder.find_rev(L'\\') + 1);
	}
}

void zxFrame::onRestore() {
	if(!loadState(opts.mainDir + L"auto_state.zx")) {
		MessageBox(hWnd, L"�� ������� ��������� ��������� ������!", L"������", MB_ICONERROR);
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
	{0, IDM_OPEN, TBSTATE_ENABLED, BTNS_DROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"�������\\Alt+O"},
	{1, IDM_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"���������\\Alt+S"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{2, IDM_RESET, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"�����\\Alt+R"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{3, IDM_RESTORE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"������������ ���������\\Alt+V"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{4, IDM_PAUSE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"����������\\����������� ��������\\Alt+P"},
	{5, IDM_SOUND, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"���\\���� ����"},
	{6, IDM_KEYBOARD, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"��������\\������ ����������\\Alt+K"},
	{7, IDM_DEBUGGER, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"��������\\������ ������������\\Alt+D"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{8, IDM_TURBO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"��������\\���������\\Alt+T"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{11, IDM_FILTER, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"������"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{9, IDM_MODEL, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"������ ������"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{10, IDM_SETTINGS, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"���������\\Alt+N"},
};

zxFrame::zxFrame() : zxWnd() {
	InitializeCriticalSection(&cs);
	hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_ZXEMUWND));
	hMenuMRU = nullptr;
	gamepad = nullptr;
	hMenuPP = nullptr;
	hMenuModel = nullptr;
	hCpuThread = nullptr;
	debug = nullptr;
}

zxFrame::~zxFrame() {
	pauseCPU(true);
	SAFE_DELETE(debug);
	SAFE_DELETE(keyboard);
	SAFE_DELETE(gamepad);
	DeleteCriticalSection(&cs);
}

void zxFrame::makeWndSize(SIZE* sz) {
	int mul = (getOpt(OPT_ASPECT_RATIO)->dval) + 1;
	if(mul > 5) mul = 2;
	sz->cx = 320 * mul;
	sz->cy = (256 * mul) + 46;
}

int zxFrame::run() {

	opts.load(opts.mainDir + L"settings.zx");
	
	SIZE sz;
	makeWndSize(&sz);

	if(!create(L"EmuWnd", L"EmuWnd", WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, sz.cx, sz.cy, nullptr, 0, IDC_ZXEMUWND))
		return -1;

	if(!bus.changeModel(getOpt(OPT_MEM_MODEL)->dval, 255))
		return -1;

	makeToolbar(IDB_TOOLBAR_COMMON, tbb, 12, 19, 32, 32, IDT_TOOLBAR_MAIN);

	ShowWindow(hWnd, true);
	UpdateWindow(hWnd);

	hMenu = GetMenu(hWnd);
	hMenuMRU = GetSubMenu(GetSubMenu(hMenu, 0), 7);
	hMenuModel = GetSubMenu(GetSubMenu(hMenu, 2), 4);
	hMenuPP = GetSubMenu(GetSubMenu(hMenu, 1), 3);

	debug = new zxDebugger;
	debug->create(IDD_DIALOG_DEBUGGER, this, false);

	keyboard = new zxKeyboard;
	keyboard->create(IDD_DIALOG_KEYBOARD, this, false);

	gamepad = new zxGamepad;
	gamepad->init(hWnd);
	gamepad->changeMode(0, zxGamepad::KEMPSTON, nullptr, 0);

	loadState(opts.mainDir + L"auto_state.zx");

	setOrGetWndPos(debug->getHWND(), OPT_WND_DEBUG_POS, false, false);
	setOrGetWndPos(keyboard->getHWND(), OPT_WND_KEY_POS, false, false);
	setOrGetWndPos(hWnd, OPT_WND_MAIN_POS, false, true);

	changeSound(false);
	changeTurbo(false);
	changeExecute(getOpt(OPT_EXECUTE)->dval, true);
	changeWndDebugger(false);
	changeWndKeyboard(false);

	checkedSubMenu(hMenu, OPT_MEM_MODEL, -1, idsModel);
	checkedSubMenu(hMenu, OPT_PP, -1, idsPP);
	checkedSubMenu(hMenu, OPT_ASPECT_RATIO, -1, idsAR);

	for(int i = 9; i >= 0; i--) modifyMRU(getOpt(OPT_MRU9)->sval);

	DWORD cpuID;
	hCpuThread = CreateThread(nullptr, 0, ProcBus, nullptr, 0, &cpuID);

	SetActiveWindow(hWnd);
		
	MSG msg;

	while(SshMsgPump(&msg, false)) { }
	return (int)msg.wParam;
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

bool zxFrame::saveState(ssh_cws path) {
	bool result = false;

	pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(_hf != -1) {
			// ��������� ��������� ����
			if(!bus.saveState(_hf)) throw(0);
			// ��� ����������� �����
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
			ssh_b tmpRegs[50];
			if(_read(_hf, tmpRegs, 50) != 50) throw(0);
			// ������ ������
			if(!bus.changeModel(tmpRegs[MODEL - 65536], oldModel)) throw(0);
			// ������ ���������
			if(!bus.loadState(_hf)) throw(0);
			// ������ ��� ����������� �����
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
