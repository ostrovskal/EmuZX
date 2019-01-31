
#include "stdafx.h"
#include "zxListBox.h"

//BEGIN_MSG_MAP(zxListBox, zxWnd)
//END_MSG_MAP()

void zxListBox::postCreate() {

	hFont = CreateFont(-12, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Courier New");

	hbrSel = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
	hbrUnsel = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}

INT_PTR zxListBox::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case LB_SETITEMHEIGHT: 
			heightItem = (int)lParam;
			countVisibleItems = ((rectWnd.bottom - rectWnd.top) / heightItem) + 1; 
			changeScroll(topIndex, countItems, countVisibleItems);
			break;
		case LB_GETITEMHEIGHT: return heightItem;
		case LB_SETCOUNT: 
			countItems = (int)wParam; 
			changeScroll(topIndex, countItems, countVisibleItems); 
			break;
		case LB_GETCOUNT: return countItems;
		case LB_SETTOPINDEX: {
			int oldTop = topIndex;
			topIndex = (int)wParam;
			if(topIndex >= countItems) topIndex = countItems - countVisibleItems;
			if(topIndex < 0) topIndex = 0;
			if(oldTop != topIndex) changeScroll(topIndex, countItems, countVisibleItems);
			break;
		}
		case LB_GETTOPINDEX: return topIndex;
		case LB_SETCURSEL: {
			int oldSel = curSel;
			if(countItems == 0) {
				curSel = -1;
			} else {
				curSel = (int)wParam;
				if(curSel < 0) curSel = 0;
				else if(curSel >= countItems) curSel = countItems - 1;
				if(curSel < topIndex || curSel >= (topIndex + countVisibleItems)) {
					topIndex = curSel;
					changeScroll(topIndex, countItems, countVisibleItems);
				}
			}
			if(oldSel != curSel) sendNotify(LBN_SELCHANGE);
			break;
		}
		case LB_GETCURSEL: return curSel;
	}
	return zxWnd::wndProc(hWnd, message, wParam, lParam);
}

bool zxListBox::onSize(WPARAM type, int nWidth, int nHeight) {
	SetRect(&rectWnd, 0, 0, nWidth, nHeight);
	countVisibleItems = ((rectWnd.bottom - rectWnd.top) / heightItem) + 1;
	return true;
}

bool zxListBox::onPaint() {
	
	if(isWindowVisible()) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		DRAWITEMSTRUCT dis;
		HWND h = parent->getHWND();
		RECT* rc = &dis.rcItem;

		rc->left = rectWnd.left;
		rc->right = rectWnd.right;
		rc->top = rectWnd.top;
		rc->bottom = rc->top + heightItem;

		dis.CtlID = wndID;
		dis.hwndItem = hWnd;
		dis.itemData = 0;
		dis.CtlType = 0;
		dis.itemAction = 0;

		dis.hDC = hdc;
		auto old = SelectObject(hdc, hFont);
		for(int top = 0; top < countVisibleItems; top++) {
			int idx = top + topIndex;
			if(idx < countItems) {
				bool isSel = (idx == curSel);
				// стереть
				FillRect(hdc, rc, (isSel ? hbrSel : hbrUnsel));
				// заполнить стк
				dis.itemState = (isSel ? ODS_SELECTED : 0);
				dis.itemID = top;
				SendMessage(h, WM_DRAWITEM, wndID, (LPARAM)&dis);
				rc->top += heightItem;
				rc->bottom += heightItem;
			}
		}
		SelectObject(hdc, old);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		return true;
	}
	return false;
}

void zxListBox::onFont(HFONT hF, bool redraw) {
	if(hF) {
		DeleteObject(hFont);
		isFont = false;
		hFont = hF;
	}
	if(redraw) InvalidateRect(hWnd, nullptr, true);
}

bool zxListBox::onScroll(UINT code, UINT p, HWND hWnd, int type) {
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	if(GetScrollInfo(hWnd, SB_VERT, &si)) {
		int pos = si.nPos;

		switch(code) {
			case SB_LINEDOWN: if(pos < si.nMax) pos++; break;
			case SB_LINEUP: if(pos > si.nMin) pos--; break;
			case SB_PAGEDOWN: pos += si.nPage; if(pos > si.nMax) pos = si.nMax; break;
			case SB_PAGEUP: pos -= si.nPage; if(pos < si.nMin) pos = si.nMin; break;
			case SB_THUMBTRACK: pos = p; if(pos < si.nMin) pos = si.nMin; if(pos > si.nMax) pos = si.nMax; break;
		}
		topIndex = pos;
		SetScrollPos(hWnd, SB_VERT, pos, true);
		sendNotify(LBN_VSCROLL);
		return true;
	}
	return false;
}

void zxListBox::sendNotify(int code) {
	SendMessage(parent->getHWND(), WM_COMMAND, MAKELONG(wndID, code), (LPARAM)hWnd);
	InvalidateRect(hWnd, nullptr, true);
}

bool zxListBox::onMouseWheel(int vkKey, int x, int y, int delta) {
	return onScroll(SB_THUMBTRACK, topIndex + (delta < WHEEL_DELTA ? 4 : -4), hWnd, true);
}

bool zxListBox::onMouseButtonDown(int vkKey, int x, int y, bool left) {
	SetFocus(hWnd);
	curSel = topIndex + (y / heightItem);
	sendNotify(LBN_SELCHANGE);
	return true;
}

bool zxListBox::onMouseButtonDblClk(int vkKey, int x, int y, bool left) {
	SetFocus(hWnd);
	curSel = topIndex + (y / heightItem);
	sendNotify(LBN_DBLCLK);
	return true;
}

void zxListBox::changeScroll(int& offs, int count, int page) {
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;

	if(GetScrollInfo(hWnd, SB_VERT, &si)) {
		int delta = count - page;
		bool visible = delta > 0;
		offs = (visible ? (offs > delta ? delta : offs) : 0);

		si.nPage = (visible ? page : 0);
		si.nMax = (visible ? count - 1 : 0);
		si.nPos = (visible ? offs : 0);

		SetScrollInfo(hWnd, SB_VERT, &si, true);
		InvalidateRect(hWnd, nullptr, true);
	}
}