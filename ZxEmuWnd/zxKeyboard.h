#pragma once

struct ZX_KEY {
	int butID;
	ssh_cws name_k;
	ssh_cws name_l;
	ssh_cws name_c;
	ssh_cws name_e;
	ssh_cws name_sh_e;
	ssh_cws name_sh_k;
	ssh_cws name_g;
	ssh_b port;
	ssh_b bit;
	HWND hWndKey;
};

class zxKeyboard {
public:
	zxKeyboard() {}
	virtual ~zxKeyboard() {}
protected:
};