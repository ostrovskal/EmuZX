#pragma once

class DecoderZX;
class CpuZX {
public:
	CpuZX();
	virtual ~CpuZX();

	// выполнение
	void execute(bool run_debugger);

	// загрузка/сохранение состояния
	bool loadStateZX(const StringZX& path);
	bool saveStateZX(const StringZX& path);

	// дешифратор
	DecoderZX* decoder;
protected:
	void signalRESET();
	void signalINT();
	void signalNMI();

	// ПЗУ
	ssh_b* ROM;
	ssh_d szROM;
};
