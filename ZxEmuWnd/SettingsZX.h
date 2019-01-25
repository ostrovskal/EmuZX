#pragma once

#define DELAY_CPU		10
#define DELAY_GPU		20

class SettingsZX {
public:
	SettingsZX();
	virtual ~SettingsZX() { }
	void load(const StringZX& path);
	void save(const StringZX& path);

	StringZX mainDir;
	StringZX curPath;

	ssh_d delayCPU;
	ssh_d delayGPU;
	ssh_d sndFreq;

	bool isSound;
	bool isMonitor;
	bool isKeyboard;
	bool isTurbo;

	ssh_b modelZX;

	StringZX mru[10];
};