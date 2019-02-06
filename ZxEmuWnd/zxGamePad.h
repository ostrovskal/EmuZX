#pragma once

class zxGamepad final {
	friend BOOL CALLBACK EnumJoys(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	friend BOOL CALLBACK EnumObjs(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
public:
	struct Remapping {
		ssh_b port;
		ssh_b bit;
	};
	enum { COUNT_CONTROLLERS = 4 };
	enum Buttons {
		crossUp		= 0,// 31_8
		crossRight,		// 31_1
		crossDown,		// 31_4
		crossLeft,		// 31_2
		A, B, X, Y,		// 31_16 � �.�
		but1, but2, but3, but4, but5, but6, but7, but8
	};
	enum Mode {
		ANY = 0, KEMPSTON, SINCLAIR
	};
	enum Axis {
		axisX = 0, axisY, axisZ
	};
	enum Types {
		xAxis = 0, yAxis, zAxis, RxAxis, RyAxis, RzAxis, Slider, POV, Button, countTypes
	};
	// �����������
	zxGamepad() { reset(); }
	// ����������
	virtual ~zxGamepad() { cleanup(); }
	// �������� �� ����� ������� ������
	bool is_once_pressed(ssh_d idx, Buttons but) const { return ((wNowButtons[idx] & (1 << but)) != 0); }
	// ������� ������ ������
	bool is_pressed(ssh_d idx, Buttons but) const { return ((wButtons[idx] & (1 << but)) != 0); }
	// ������� ������ ���
	long is_axis(ssh_d idx, Axis ax) const { return axis[idx][ax]; }
	// ������� ������ �������
	long is_rotate(ssh_d idx, Axis ax) const { return rot[idx][ax]; }
	// ������� ��������
	bool is_connected(ssh_d idx) const { return connected[idx]; }
	// ������� �����������
	bool is_removed(ssh_d idx) const { return removed[idx]; }
	// ������� ������ �����������
	bool is_inserted(ssh_d idx) const { return inserted[idx]; }
	// ���������� ���������
	void update();
	// �����
	void reset();
	// �������������
	bool init(HWND hWnd);
	// ������������ � ����������������� ����� (KEMPSTON, SINCLAIR)
	void changeMode(ssh_d idx, Mode mode, Remapping* remapData, int count);
	// �������� ������
	void remap(ssh_d idx, Buttons but);
protected:
	BOOL create(const DIDEVICEINSTANCE* pdidInstance);
	BOOL enumObjs(const DIDEVICEOBJECTINSTANCE* pdidoi);
	// ������������ ��������
	void cleanup();
	// ������ 16-�� ������ ��� ������� ������
	ssh_w wNowButtons[COUNT_CONTROLLERS];
	// ������ 16-�� ������� ������
	ssh_w wButtons[COUNT_CONTROLLERS];
	// ������ 3-� ����
	long axis[COUNT_CONTROLLERS][3];
	// ������ 3-� ���� �������
	long rot[COUNT_CONTROLLERS][3];
	// ��������
	bool connected[COUNT_CONTROLLERS];
	bool inserted[COUNT_CONTROLLERS];
	bool removed[COUNT_CONTROLLERS];
	// ������������� ����������
	int granPOV[COUNT_CONTROLLERS];
	// �������� ������ � zx ������
	Remapping map[COUNT_CONTROLLERS][16];
	// ����� ��������
	Mode modes[COUNT_CONTROLLERS];
	// ���������� ����� ��������
	Types types[COUNT_CONTROLLERS][countTypes];
	// ���������
	LPDIRECTINPUTDEVICE8 joys[COUNT_CONTROLLERS];
};
