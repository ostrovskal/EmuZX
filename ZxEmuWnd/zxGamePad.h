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
		A, B, X, Y,		// 31_16 и т.д
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
	// конструктор
	zxGamepad() { reset(); }
	// деструктор
	virtual ~zxGamepad() { cleanup(); }
	// проверка на вновь нажатую кнопку
	bool is_once_pressed(ssh_d idx, Buttons but) const { return ((wNowButtons[idx] & (1 << but)) != 0); }
	// вернуть статус кнопки
	bool is_pressed(ssh_d idx, Buttons but) const { return ((wButtons[idx] & (1 << but)) != 0); }
	// вернуть статус оси
	long is_axis(ssh_d idx, Axis ax) const { return axis[idx][ax]; }
	// вернуть статус ротации
	long is_rotate(ssh_d idx, Axis ax) const { return rot[idx][ax]; }
	// признак коннекта
	bool is_connected(ssh_d idx) const { return connected[idx]; }
	// признак дизконнекта
	bool is_removed(ssh_d idx) const { return removed[idx]; }
	// признак нового подключения
	bool is_inserted(ssh_d idx) const { return inserted[idx]; }
	// обновление состояния
	void update();
	// сброс
	void reset();
	// инициализация
	bool init(HWND hWnd);
	// переключение в предустановленный режим (KEMPSTON, SINCLAIR)
	void changeMode(ssh_d idx, Mode mode, Remapping* remapData, int count);
	// ремапинг кнопок
	void remap(ssh_d idx, Buttons but);
protected:
	BOOL create(const DIDEVICEINSTANCE* pdidInstance);
	BOOL enumObjs(const DIDEVICEOBJECTINSTANCE* pdidoi);
	// освобождение ресурсов
	void cleanup();
	// статус 16-ти только что нажатых кнопок
	ssh_w wNowButtons[COUNT_CONTROLLERS];
	// статус 16-ти нажатых кнопок
	ssh_w wButtons[COUNT_CONTROLLERS];
	// статус 3-х осей
	long axis[COUNT_CONTROLLERS][3];
	// статус 3-х осей ротации
	long rot[COUNT_CONTROLLERS][3];
	// признаки
	bool connected[COUNT_CONTROLLERS];
	bool inserted[COUNT_CONTROLLERS];
	bool removed[COUNT_CONTROLLERS];
	// гранулярность крестовины
	int granPOV[COUNT_CONTROLLERS];
	// привязка кнопок в zx портам
	Remapping map[COUNT_CONTROLLERS][16];
	// режим маппинга
	Mode modes[COUNT_CONTROLLERS];
	// количество типов объектов
	Types types[COUNT_CONTROLLERS][countTypes];
	// джойстики
	LPDIRECTINPUTDEVICE8 joys[COUNT_CONTROLLERS];
};
