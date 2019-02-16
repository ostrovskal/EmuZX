#pragma once

class zxGamepad final {
	friend BOOL CALLBACK EnumJoys(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	friend BOOL CALLBACK EnumObjs(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
public:
	enum { COUNT_CONTROLLERS = 4 };
	enum Buttons {
		povUp		= 0,// 31_8
		povRight,		// 31_1
		povDown,		// 31_4
		povLeft,		// 31_2
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
	// переключение в предустановленный режим (KEMPSTON, SINCLAIR и т.д.)
	void changeMode(ssh_d idx, int mode);
	// вернуть оригинальную раскладку кнопок
	ssh_b* getOrigMap(ssh_d idx) { return mapOrig[idx]; }
	// ремапинг кнопок
	void remap(ssh_d idx);
protected:
	BOOL create(const DIDEVICEINSTANCE* pdidInstance);
	BOOL enumObjs(const DIDEVICEOBJECTINSTANCE* pdidoi);
	// обновление статуса кнопки
	void updateKey(bool pressed, int k);
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
	// оригинальная привязка кнопок к zx портам
	ssh_b mapOrig[COUNT_CONTROLLERS][countButtons];
	// текущая привязка кнопок к zx портам
	ssh_b mapCurrent[COUNT_CONTROLLERS][countButtons];
	// режим маппинга
	int modes[COUNT_CONTROLLERS];
	// количество типов объектов
	Types types[COUNT_CONTROLLERS][countTypes];
	// джойстики
	LPDIRECTINPUTDEVICE8 joys[COUNT_CONTROLLERS];
};
