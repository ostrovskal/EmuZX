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
	// конструктор
	zxGamepad();
	// проверка на вновь нажатую кнопку
	bool is_once_pressed(bool second, Buttons but) const { return ((wNowButtons[second] & (1 << but)) != 0); }
	// вернуть статус кнопки
	bool is_pressed(bool second, Buttons but) const { return ((wButtons[second] & (1 << but)) != 0); }
	// вернуть статус оси
	long is_axis(bool second, Axis ax) const { return axis[second][ax]; }
	// вернуть статус ротации
	long is_rotate(bool second, Axis ax) const { return rot[second][ax]; }
	// признак коннекта
	bool is_connected(bool second) const { return connected[second]; }
	// признак дизконнекта
	bool is_removed(bool second) const { return removed[second]; }
	// признак нового подключения
	bool is_inserted(bool second) const { return inserted[second]; }
	// обновление состояния
	void update();
	// инициализация
	void init();
	// переключение в предустановленный режим (KEMPSTON, SINCLAIR и т.д.)
	void changeMode(bool second, int mode);
	// вернуть оригинальную раскладку кнопок
	ssh_b* getOrigMap(bool second) { return mapOrig[second]; }
	// режим маппинга
	int getMode(bool second) const { return modes[second]; }
	// ремапинг кнопок
	void remap(bool second);
	// предопределенные раскладки
	static ssh_b* getPredefinedMode(int mode, int& count);
protected:
	// обновление статуса кнопки
	void updateKey(bool pressed, int k);
	// статус 16-ти только что нажатых кнопок
	ssh_w wNowButtons[2];
	// статус 16-ти нажатых кнопок
	ssh_w wButtons[2];
	// статус 3-х осей
	long axis[2][3];
	// статус 3-х осей ротации
	long rot[2][3];
	// признаки
	bool connected[2];
	bool inserted[2];
	bool removed[2];
	// оригинальная привязка кнопок к zx портам
	ssh_b mapOrig[2][countButtons];
	// текущая привязка кнопок к zx портам
	ssh_b mapCurrent[2][countButtons];
	// режим маппинга
	int modes[2];
};
