#pragma once

class zxGamepad final {
public:
	enum Buttons {
		povUp = 0, povRight, povDown, povLeft,
		but1, but2, but3, but4, but5, but6, but7, but8, but9, but10, but11, but12,
		xAxisP, xAxisM, yAxisP, yAxisM, zAxisP, zAxisM,
		xRotP, xRotM, yRotP, yRotM, zRotP, zRotM,
		countButtons
	};
	enum Axis {
		axisX = 0, axisY, axisZ
	};
	enum Types {
		xAxis = 0, yAxis, zAxis, RxAxis, RyAxis, RzAxis, Slider, POV, Button, countTypes
	};
	// �����������
	zxGamepad();
	// �������� �� ����� ������� ������
	bool is_once_pressed(bool second, Buttons but) const { return ((wNowButtons[second] & (1 << but)) != 0); }
	// ������� ������ ������
	bool is_pressed(bool second, Buttons but) const { return ((wButtons[second] & (1 << but)) != 0); }
	// ������� ������ ���
	long is_axis(bool second, Axis ax) const { return axis[second][ax]; }
	// ������� ������ �������
	long is_rotate(bool second, Axis ax) const { return rot[second][ax]; }
	// ������� ��������
	bool is_connected(bool second) const { return connected[second]; }
	// ������� �����������
	bool is_removed(bool second) const { return removed[second]; }
	// ������� ������ �����������
	bool is_inserted(bool second) const { return inserted[second]; }
	// ���������� ���������
	void update();
	// �������������
	void init();
	// ������������ � ����������������� ����� (KEMPSTON, SINCLAIR � �.�.)
	void changeMode(bool second, int mode);
	// ������� ������������ ��������� ������
	ssh_b* getOrigMap(bool second) { return mapOrig[second]; }
	// ����� ��������
	int getMode(bool second) const { return modes[second]; }
	// �������� ������
	void remap(bool second);
	// ���������������� ���������
	static ssh_b* getPredefinedMode(int mode, int& count);
protected:
	// ���������� ������� ������
	void updateKey(bool pressed, int k);
	// ������ 16-�� ������ ��� ������� ������
	ssh_w wNowButtons[2];
	// ������ 16-�� ������� ������
	ssh_w wButtons[2];
	// ������ 3-� ����
	long axis[2][3];
	// ������ 3-� ���� �������
	long rot[2][3];
	// ��������
	bool connected[2];
	bool inserted[2];
	bool removed[2];
	// ������������ �������� ������ � zx ������
	ssh_b mapOrig[2][countButtons];
	// ������� �������� ������ � zx ������
	ssh_b mapCurrent[2][countButtons];
	// ����� ��������
	int modes[2];
};
