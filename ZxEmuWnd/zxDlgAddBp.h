
#pragma once

#include "zxWnd.h"

class zxDlgAddBp : public zxDialog {
public:
	zxDlgAddBp() : zxDialog() {}
	virtual ~zxDlgAddBp() {}
	void edit(int new_bp);
protected:
};
