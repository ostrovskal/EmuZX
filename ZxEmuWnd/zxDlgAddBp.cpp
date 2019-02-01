
#include "stdafx.h"
#include "zxDlgAddBp.h"

ssh_cws cond_bp[] = {L"==", L"<>", L"<", L">", L"<=", L">="};

void zxDlgAddBp::onInitDialog(HWND hWnd, LPARAM lParam) {
	hWndCond = GetDlgItem(hWnd, IDC_COMBO_COND);
	hWndAccess = GetDlgItem(hWnd, IDC_COMBO_ACCESS);

	hWndAddr1 = GetDlgItem(hWnd, IDC_EDIT_ADDRESS1);
	hWndAddr2 = GetDlgItem(hWnd, IDC_EDIT_ADDRESS2);
	hWndValue = GetDlgItem(hWnd, IDC_EDIT_VALUE);

	hWndOK = GetDlgItem(hWnd, IDOK);

	for(auto& c : cond_bp) SendMessage(hWndCond, CB_ADDSTRING, 0, (LPARAM)c);

	SendMessage(hWndAccess, CB_ADDSTRING, 0, (LPARAM)L"Выполнение");
	SendMessage(hWndAccess, CB_ADDSTRING, 0, (LPARAM)L"Запись в память");

	if(_bp) {
		auto dec = theApp.getOpt(OPT_DECIMAL)->dval;
		SetWindowText(hWndAddr1, fromNum(_bp->address1, radix[dec + 10]));
		SetWindowText(hWndAddr2, fromNum(_bp->address2, radix[dec + 10]));
		if(_bp->value != -1) SetWindowText(hWndValue, fromNum(_bp->value, radix[dec + 16]));
		if(_bp->value != -1) SendMessage(hWndCond, CB_SETCURSEL, ((_bp->flags & FBP_COND) >> 2), 0);
		SendMessage(hWndAccess, CB_SETCURSEL, ((_bp->flags & FBP_ACCESS) >> 1), 0);
	} else {
		isAddr2 = true;
		SendMessage(hWndAccess, CB_SETCURSEL, 0, 0);
	}
	updateAccess();
}

int zxDlgAddBp::getValue(HWND hWnd, int def) {
	auto ret = (GetWindowText(hWnd, tmpBuf, 260) == 0) ?
		def : *(int*)asm_ssh_wton(tmpBuf, ((theApp.getOpt(OPT_DECIMAL)->dval == 0) ? (ssh_u)Radix::_hex : (ssh_u)Radix::_dec));
	return ret;
}

bool zxDlgAddBp::onCommand(int wmId, int param, LPARAM lParam) {
	switch(wmId) {
		case IDOK: {
			result.address1 = getValue(hWndAddr1, -1);
			result.address2 = getValue(hWndAddr2, result.address1);
			result.value = getValue(hWndValue, -1);
			auto f = (int)SendMessage(hWndAccess, CB_GETCURSEL, 0, 0);
			if(result.address1 > 65535 || result.address2 > 65535 || (f == 1 && result.value > 255)) {
				MessageBox(hWnd, L"Слишком большие величины. Недопустимо заданы адреса/значение!", L"Ошибка", MB_ICONERROR);
				return false;
			}
			if(result.address1 > result.address2) {
				MessageBox(hWnd, L"Начальный адрес диапазона должен быть меньше последнего!", L"Ошибка", MB_ICONERROR);
				return false;
			}
			result.flags = ((f == -1) ? 0 : 1 << f);
			if(f == 1) {
				f = (int)SendMessage(hWndCond, CB_GETCURSEL, 0, 0);
				result.flags |= (f == -1 ? 0 : (f << 2));
			} else {
				result.value = -1;
			}
			break;
		}
		case IDC_EDIT_ADDRESS1:
			if(param == EN_CHANGE) {
				auto l = GetWindowText(hWndAddr1, tmpBuf, 260);
				if(isAddr2) {
					owner = true;
					SetWindowText(hWndAddr2, tmpBuf);
					owner = true;
				}
				EnableWindow(hWndOK, l > 0);
			}
			break;
		case IDC_EDIT_ADDRESS2:
			if(param == EN_CHANGE) isAddr2 = owner;
			break;
		case IDC_COMBO_ACCESS:
			if(param == CBN_SELCHANGE) updateAccess();
			break;
		default: return false;
	}
	return true;
}

void zxDlgAddBp::updateAccess() {
	auto f = (SendMessage(hWndAccess, CB_GETCURSEL, 0, 0) == 1);
	EnableWindow(hWndCond, f); EnableWindow(hWndValue, f);
}