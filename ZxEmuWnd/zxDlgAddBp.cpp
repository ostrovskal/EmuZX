
#include "stdafx.h"
#include "zxDlgAddBp.h"

void zxDlgAddBp::onInitDialog(HWND hWnd, LPARAM lParam) {
	hWndCond = GetDlgItem(hWnd, IDC_COMBO_COND);
	hWndAccess = GetDlgItem(hWnd, IDC_COMBO_ACCESS);

	hWndAddr1 = GetDlgItem(hWnd, IDC_EDIT_ADDRESS1);
	hWndAddr2 = GetDlgItem(hWnd, IDC_EDIT_ADDRESS2);
	hWndValue = GetDlgItem(hWnd, IDC_EDIT_VALUE);

	hWndOK = GetDlgItem(hWnd, IDOK);

	auto ret = SendMessage(hWndCond, CB_ADDSTRING, 0, (LPARAM)L"==");
	ret = SendMessage(hWndCond, CB_ADDSTRING, 0, (LPARAM)L"!=");

	ret = SendMessage(hWndAccess, CB_ADDSTRING, 0, (LPARAM)L"Выполнение");
	ret = SendMessage(hWndAccess, CB_ADDSTRING, 0, (LPARAM)L"Запись в память");

	if(_bp) {
		auto dec = theApp.getOpt(OPT_DECIMAL)->dval;
		SetWindowText(hWndAddr1, fromNum(_bp->address1, radix[dec + 10]));
		SetWindowText(hWndAddr2, fromNum(_bp->address2, radix[dec + 10]));
		if(_bp->value != -1) SetWindowText(hWndValue, fromNum(_bp->value, radix[dec]));
		
		if(_bp->value != -1) SendMessage(hWndCond, CB_SETCURSEL, ((_bp->flags & FBP_EQ) != FBP_EQ), 0);
		SendMessage(hWndAccess, CB_SETCURSEL, ((_bp->flags & FBP_MEM) == FBP_MEM), 0);
		isAddr2 = true;
	} else {
		//SendMessage(hWndCond, CB_SETCURSEL, 0, 0);
		SendMessage(hWndAccess, CB_SETCURSEL, 0, 0);
	}
}

int zxDlgAddBp::getValue(HWND hWnd, int def) {
	auto ret = (GetWindowText(hWnd, tmpBuf, 260) == 0) ?
		def : *(int*)asm_ssh_wton(tmpBuf, ((theApp.getOpt(OPT_DECIMAL)->dval == 0) ? (ssh_u)Radix::_hex : (ssh_u)Radix::_dec));
	return ret;
}

bool zxDlgAddBp::onCommand(int wmId, int param, LPARAM lParam) {
	switch(wmId) {
		case IDOK:
			result.address1 = getValue(hWndAddr1, -1);
			result.address2 = getValue(hWndAddr2, result.address1);
			result.value = getValue(hWndValue, -1);
			if(result.address1 > 65535 || result.address2 > 65535 || result.value > 255) {
				MessageBox(hWnd, L"Слишком большие величины. Недопустимо заданы адреса/значение!", L"Ошибка", MB_ICONERROR);
				return false;
			}
			if(result.address1 > result.address2) {
				MessageBox(hWnd, L"Начальный адрес диапазона должен быть меньше последнего!", L"Ошибка", MB_ICONERROR);
				return false;
			}
			result.flags = ((SendMessage(hWndCond, CB_GETCURSEL, 0, 0) == 0) ? FBP_EQ : 0);
			result.flags |= ((SendMessage(hWndAccess, CB_GETCURSEL, 0, 0) == 0) ? FBP_PC : FBP_MEM);
			break;
		case IDC_EDIT_ADDRESS1: {
			auto l = GetWindowText(hWndAddr1, tmpBuf, 260);
			if(isAddr2) SetWindowText(hWndAddr2, tmpBuf);
			EnableWindow(hWndOK, l > 0);
			break;
		}
		default: return false;
	}
	return true;
}
