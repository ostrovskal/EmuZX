
#include "stdafx.h"
#include "zxListBox.h"

BEGIN_MSG_MAP(zxListBox, zxWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_SETFONT()
	ON_WM_KEYDOWN()
END_MSG_MAP()

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
		case LB_GETCOUNT: return (wParam == 1 ? countVisibleItems : countItems);
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

void zxListBox::onSize(UINT type, int nWidth, int nHeight) {
	SetRect(&rectWnd, 0, 0, nWidth, nHeight);
	countVisibleItems = ((rectWnd.bottom - rectWnd.top) / heightItem);
}

void zxListBox::onPaint() {
	if(IsWindowVisible(hWnd)) {
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
		for(int top = 0; top <= countVisibleItems; top++) {
			int idx = top + topIndex;
			if(idx < countItems) {
				bool isSel = (idx == curSel);
				// стереть
				FillRect(hdc, rc, (isSel ? hbrSel : hbrUnsel));
				// заполнить стк
				dis.itemState = (isSel ? ODS_SELECTED : 0);
				dis.itemID = idx;
				SendMessage(h, WM_DRAWITEM, wndID, (LPARAM)&dis);
				rc->top += heightItem;
				rc->bottom += heightItem;
			}
		}
		SelectObject(hdc, old);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
	}
}

void zxListBox::onSetFont(HFONT hF, BOOL redraw) {
	if(hF) {
		DeleteObject(hFont);
		isFont = false;
		hFont = hF;
	}
	if(redraw) InvalidateRect(hWnd, nullptr, true);
}

void zxListBox::onVScroll(UINT code, UINT p, HWND hWndScroll) {
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	if(GetScrollInfo(hWnd, SB_VERT, &si)) {
		int pos = si.nPos;

		switch(code) {
			case SB_LINEDOWN: pos++; break;
			case SB_LINEUP: pos--; break;
			case SB_PAGEDOWN: pos += si.nPage; break;
			case SB_PAGEUP: pos -= si.nPage; break;
			case SB_THUMBTRACK: pos = p; break;
		}
		if(pos >= (si.nMax - countVisibleItems)) {
			pos = si.nMax - countVisibleItems;
		} else if(pos < si.nMin) {
			pos = si.nMin;
		}
		topIndex = pos;
		si.nPos = pos;
		auto ret = SetScrollPos(hWnd, SB_VERT, pos, true);
		sendNotify(LBN_VSCROLL);
	}
}

void zxListBox::sendNotify(int code) {
	SendMessage(parent->getHWND(), WM_COMMAND, MAKELONG(wndID, code), (LPARAM)hWnd);
	InvalidateRect(hWnd, nullptr, false);
}

BOOL zxListBox::onMouseWheel(UINT vkKey, short delta, POINT pt) {
	onVScroll(SB_THUMBTRACK, topIndex + (delta < WHEEL_DELTA ? 4 : -4), nullptr);
	return TRUE;
}

void zxListBox::onLButtonDown(UINT vkKey, POINT pt) {
	SetFocus(hWnd);
	curSel = topIndex + (pt.y / heightItem);
	sendNotify(LBN_SELCHANGE);
}

void zxListBox::onLButtonDblClk(UINT vkKey, POINT pt) {
	SetFocus(hWnd);
	curSel = topIndex + (pt.y / heightItem);
	sendNotify(LBN_DBLCLK);
}

void zxListBox::onKeyDown(UINT nVirtKey, UINT nRepeat, UINT nFlags) {
	int delta = 0;
	switch(nVirtKey) {
		case VK_UP: delta = -1; break;
		case VK_DOWN: delta = 1; break;
		case VK_NEXT: delta = countVisibleItems - 1; break;
		case VK_PRIOR: delta = -(countVisibleItems - 1); break;
	}
	if(delta != 0) {
		topIndex += delta;
		sendNotify(LBN_VSCROLL);
	}
}

void zxListBox::changeScroll(int& offs, int count, int page) {
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;

	if(GetScrollInfo(hWnd, SB_VERT, &si)) {
		page--;
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