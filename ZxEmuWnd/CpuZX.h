#pragma once

class DecoderZX;
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

	// загрузка/сохранение состояния
	bool loadStateZX(const StringZX& path);
	bool saveStateZX(const StringZX& path);

	bool loadZ80(const StringZX& path);
	bool saveZ80(const StringZX& path);

	// дешифратор
	DecoderZX* decoder;

	// содержмое памяти
	static ssh_b memory[65536];

	// ПЗУ
	ssh_b* ROM;
	long szROM;

	static bool isExec;

	// триггеры
	static ssh_b IFF1;
	static ssh_b IFF2;

	// статус
	static ssh_b STATE;

	// содержимое порта FE
	static ssh_b portFE;

	// признак наличия прерывания INT
	static ssh_b trap;

	// прерывания
	static ssh_b RI;

	// регенерация
	static ssh_b RR;

	// режим прерываний
	static ssh_b IM;

	// счетчик комманд
	static ssh_w PC;

	// регистры
	static ssh_b cpu[22];

	// порты
	static ssh_b port[65536];

	static ssh_b* A;
	static ssh_b* B;

	static ssh_w* BC;
	static ssh_w* DE;
	static ssh_w* HL;
	static ssh_w* SP;
};
