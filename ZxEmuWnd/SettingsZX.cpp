
#include "stdafx.h"
#include "SettingsZX.h"

ssh_cws nameROMs[] = {L"48K", L"128K"};
ssh_cws namePPs[] = {L"None", L"Mixed", L"Bilinear"};

ssh_cws options[] = {
	L"soundEnable",
	L"monitorEnable",
	L"keyboardEnable",
	L"turboEnable",
	L"postProcess",
	L"soundFraquency",
	L"delayCPU",
	L"delayGPU",
	L"path",
	L"model",
	L"mru0",
	L"mru1",
	L"mru2",
	L"mru3",
	L"mru4",
	L"mru5",
	L"mru6",
	L"mru7",
	L"mru8",
	L"mru9",
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
	isTurbo = false;
	postProcess = 0;
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
			bool bval = value == L"true";
			switch(num) {
				case 0: isSound = bval; break;
				case 1: isMonitor = bval; break;
				case 2: isKeyboard = bval; break;
				case 3: isTurbo = bval; break;
				case 4: postProcess = (ssh_b)val; break;
				case 5: sndFreq = val; break;
				case 6: delayCPU = val; break;
				case 7: delayGPU = val; break;
				case 8: curPath = value; break;
				case 9: modelZX = (ssh_b)val; break;
				default: mru[num - 10] = value; break;
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
				case 3: bval = isTurbo; break;
				case 4: val = postProcess; break;
				case 5: val = sndFreq; break;
				case 6: val = delayCPU; break;
				case 7: val = delayGPU; break;
				case 8: sval = curPath; break;
				case 9: val = modelZX; break;
				default: sval = mru[num - 10]; break;
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
