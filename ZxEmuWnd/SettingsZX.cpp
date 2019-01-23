
#include "stdafx.h"
#include "SettingsZX.h"

ssh_cws nameROMs[] = {L"48K", L"128K"};

ssh_cws options[] = {
	L"soundEnable",
	L"monitorEnable",
	L"keyboardEnable",
	L"soundFraquency",
	L"delayCPU",
	L"delayGPU",
	L"path",
	L"model",
	nullptr
};

SettingsZX::SettingsZX() {
	ssh_ws buf[MAX_PATH];
	::GetModuleFileName(::GetModuleHandle(NULL), buf, MAX_PATH);
	mainDir = buf;
	mainDir = mainDir.left(mainDir.find_rev(L'\\') + 1);
	curPath = mainDir;

	modelZX = 0;
	delayCPU = DELAY_CPU;
	delayGPU = DELAY_GPU;
	isSound = true;
	isMonitor = false;
	isKeyboard = false;
	sndFreq = 44100;
}

bool readLine(FILE* hh, StringZX& name, StringZX& value) {
	StringZX tmp(L'\0', 256);
	if(!fgetws(tmp.buffer(), 255, hh)) return false;
	auto l = wcslen(tmp.buffer()) - 1;
	if(tmp[l] == L'\n') tmp.set(l, L'\0');
	name = tmp.left(tmp.find(L'='));
	value = tmp.substr(tmp.find(L'=') + 1);
	return true;
}

void SettingsZX::load(const StringZX& path) {

	StringZX name, value;
	FILE* hh = nullptr;

	_wfopen_s(&hh, path, L"rt,ccs=UTF-16LE");
	if(hh) {
		while(true) {
			if(!readLine(hh, name, value)) break;
			auto dd = name.buffer();
			auto dd1 = value.buffer();
			int num = 0;
			ssh_cws opt;
			while((opt = options[num]) != nullptr) {
				if(name == opt) break;
				num++;
			}
			ssh_d val = *(ssh_d*)asm_ssh_wton(value.buffer(), (ssh_u)Radix::_dec);
			switch(num) {
				case 0: isSound = value == L"true"; break;
				case 1: isMonitor = value == L"true"; break;
				case 2: isKeyboard = value == L"true"; break;
				case 3: sndFreq = val; break;
				case 4: delayCPU = val; break;
				case 5: delayGPU = val; break;
				case 6: curPath = value; break;
				case 7: modelZX = val; break;
			}
		}
		fclose(hh);
	}
}

void SettingsZX::save(const StringZX& path) {
	FILE* hh = nullptr;

	_wfopen_s(&hh, path, L"wt,ccs=UTF-16LE");
	if(hh) {
		int num = 0;
		ssh_cws opt;
		while((opt = options[num]) != nullptr) {
			StringZX line(opt);
			line += L'=';
			ssh_u val = -1;
			ssh_b bval = 255;
			ssh_cws sval = nullptr;
			switch(num) {
				case 0: bval = isSound; break;
				case 1: bval = isMonitor; break;
				case 2: bval = isKeyboard; break;
				case 3: val = sndFreq; break;
				case 4: val = delayCPU; break;
				case 5: val = delayGPU; break;
				case 6: sval = curPath; break;
				case 7: val = modelZX; break;
			}
			if(val != -1) line += asm_ssh_ntow(&val, (ssh_u)Radix::_dec);
			else if(sval) line += sval;
			else if(bval != 255) line += (bval ? L"true" : L"false");
			line += L'\n';
			num++;
			fputws(line, hh);
		}
		fclose(hh);
	}
}
