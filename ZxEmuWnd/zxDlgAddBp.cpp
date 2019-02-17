
#include "stdafx.h"
#include "zxDlgAddBp.h"

ssh_cws cond_bp[] = {L"==", L"<>", L"<", L">", L"<=", L">="};

BEGIN_MSG_MAP(zxDlgAddBp, zxDialog)
	ON_CONTROL(EN_CHANGE, IDC_EDIT_ADDRESS1, onNotifyAddr1)
	ON_CONTROL(EN_CHANGE, IDC_EDIT_ADDRESS2, onNotifyAddr2)
	ON_CONTROL(CBN_SELCHANGE, IDC_COMBO_ACCESS, onNotifyAccess)
END_MSG_MAP()

int zxDlgAddBp::onInitDialog(HWND hWnd) {
	HWND h;
	h = GetDlgItem(hWnd, IDC_COMBO_COND);
	for(auto& c : cond_bp) SendMessage(h, CB_ADDSTRING, 0, (LPARAM)c);

	h = GetDlgItem(hWnd, IDC_COMBO_ACCESS);
	SendMessage(h, CB_ADDSTRING, 0, (LPARAM)L"Выполнение");
	SendMessage(h, CB_ADDSTRING, 0, (LPARAM)L"Запись в память");

	if(_bp) {
		auto dec = theApp->getOpt(OPT_DECIMAL)->dval;
		SendMessage(h, CB_SETCURSEL, ((_bp->flags & FBP_ACCESS) >> 1), 0);
		if(_bp->flags & FBP_ADDR) SetWindowText(GetDlgItem(hWnd, IDC_EDIT_ADDRESS1), fromNum(_bp->address1, radix[dec + 22]));
		if(_bp->flags & FBP_ADDR) SetWindowText(GetDlgItem(hWnd, IDC_EDIT_ADDRESS2), fromNum(_bp->address2, radix[dec + 22]));
		if(_bp->flags & FBP_VAL) {
			SetWindowText(GetDlgItem(hWnd, IDC_EDIT_VALUE), fromNum(_bp->value, radix[dec + 16]));
			SetWindowText(GetDlgItem(hWnd, IDC_EDIT_MASK), fromNum(_bp->mask, radix[dec + 16]));
			SendMessage(GetDlgItem(hWnd, IDC_COMBO_COND), CB_SETCURSEL, ((_bp->flags & FBP_COND) >> 2), 0);
		}
	} else {
		isAddr2 = true;
		SendMessage(h, CB_SETCURSEL, 0, 0);
	}
	updateAccess();
	return 1;
}

int zxDlgAddBp::getValue(HWND hWnd, int flag) {
	result.flags &= ~flag;
	if(GetWindowText(hWnd, tmpBuf, 260) == 0) return 0;
	result.flags |= flag;
	return *(int*)asm_ssh_wton(tmpBuf, (ssh_u)Radix::_dec);
}

void zxDlgAddBp::onOK() {
	result.address1 = getValue(GetDlgItem(hWnd, IDC_EDIT_ADDRESS1), FBP_ADDR);
	result.address2 = getValue(GetDlgItem(hWnd, IDC_EDIT_ADDRESS2), FBP_ADDR);
	auto f = (int)SendMessage(GetDlgItem(hWnd, IDC_COMBO_ACCESS), CB_GETCURSEL, 0, 0);
	if(f == 1) {
		result.value = getValue(GetDlgItem(hWnd, IDC_EDIT_VALUE), FBP_VAL);
		result.mask = getValue(GetDlgItem(hWnd, IDC_EDIT_MASK), FBP_MASK);
	}
	if(result.address1 > 65535 || result.address2 > 65535 ||
		(f == 1 && result.flags & FBP_VAL && result.value > 255) ||
	   (f == 1 && result.flags & FBP_MASK && result.mask > 255)) {
		MessageBox(hWnd, L"Слишком большие величины. Недопустимо заданы адреса/значение/маска !", L"Ошибка", MB_ICONERROR);
		return;
	}
	if(result.address1 > result.address2) {
		MessageBox(hWnd, L"Начальный адрес диапазона должен быть меньше последнего!", L"Ошибка", MB_ICONERROR);
		return;
	}
	result.flags |= ((f == -1) ? 0 : 1 << f);
	if(f == 1) {
		f = (int)SendMessage(GetDlgItem(hWnd, IDC_COMBO_COND), CB_GETCURSEL, 0, 0);
		result.flags |= (f == -1 ? 0 : (f << 2));
	} else {
		result.flags &= ~(FBP_VAL | FBP_MASK);
	}
	zxDialog::onOK();
}

void zxDlgAddBp::onNotifyAddr1() {
	auto l = GetWindowText(GetDlgItem(hWnd, IDC_EDIT_ADDRESS1), tmpBuf, 260);
	if(isAddr2) {
		owner = true;
		SetWindowText(GetDlgItem(hWnd, IDC_EDIT_ADDRESS2), tmpBuf);
		owner = false;
	}
	EnableWindow(GetDlgItem(hWnd, IDOK), l > 0);
}

void zxDlgAddBp::onNotifyAddr2() {
	isAddr2 = owner;
}

void zxDlgAddBp::onNotifyAccess() {
	updateAccess();
}

void zxDlgAddBp::updateAccess() {
	auto f = (SendMessage(GetDlgItem(hWnd, IDC_COMBO_ACCESS), CB_GETCURSEL, 0, 0) == 1);
	EnableWindow(GetDlgItem(hWnd, IDC_COMBO_COND), f); 
	EnableWindow(GetDlgItem(hWnd, IDC_EDIT_VALUE), f); 
	EnableWindow(GetDlgItem(hWnd, IDC_EDIT_MASK), f);
}