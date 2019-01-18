#pragma once

#include "GpuZX.h"

class DecoderZX;
class GpuZX;
class BorderZX;
class KeyboardZX;
class SoundZX;
class CpuZX {
public:
	enum {
		INT		= 1,
		BORDER	= 2,
		SOUND	= 4
	};
	CpuZX();
	virtual ~CpuZX();

	void signalRESET();
	void signalINT();
	void signalNMI();

	// выполнение
	void execute();

	// обновление графики
	void updateGpu();

	// перерисовка бордера
	void redrawBorder();
	
	// проигрывание звука
	void playSound();

	// обновление клавиш
	void updateKey(int key, bool pressed);
	
	// сохранение состояния
	bool saveStateZX(const wchar_t* path);

	// загрузка состояния
	bool loadStateZX(const wchar_t* path);

	// дешифратор
	DecoderZX* decoder;

	// графическая карта
	GpuZX* gpu;

	// бордер
	BorderZX* brd;

	// клавиатура
	KeyboardZX* key;

	// бипер
	SoundZX* snd;

	// содержмое памяти
	static byte memory[65536];

	static int frames;

	static int keyd;
	static int keyu;
	
	static bool isExec;

	// триггеры
	static byte IFF1;
	static byte IFF2;

	// статус
	static byte STATE;

	// цвет бордера
	static byte border;

	// статус звука
	static byte sound;

	// признак наличия прерывания INT
	static byte trap;

	// прерывания
	static byte RI;

	// регенерация
	static byte RR;

	// режим прерываний
	static byte IM;

	// счетчик комманд
	static word RPC;

	// регистры
	static byte cpu[22];

	// порты
	static byte port[65536];

	static byte* A;
	static byte* B;

	static word* BC;
	static word* DE;
	static word* HL;
	static word* SP;
};
