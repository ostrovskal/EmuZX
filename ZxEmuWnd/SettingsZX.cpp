
#include "stdafx.h"
#include "SettingsZX.h"
#include "zxDebugger.h"

extern zxDebugger* debug;

ssh_cws nameROMs[] = {L"48K", L"128K"};
ssh_cws namePPs[] = {L"None", L"Mixed", L"Bilinear"};

ZX_OPTION opt[] = {
	{OPTT_STRING, L"mru0"},
	{OPTT_STRING, L"mru1"},
	{OPTT_STRING, L"mru2"},
	{OPTT_STRING, L"mru3"},
	{OPTT_STRING, L"mru4"},
	{OPTT_STRING, L"mru5"},
	{OPTT_STRING, L"mru6"},
	{OPTT_STRING, L"mru7"},
	{OPTT_STRING, L"mru8"},
	{OPTT_STRING, L"mru9"},
	{OPTT_STRING, L"bps0", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"bps1", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"bps2", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"bps3", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"bps4", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"bps5", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"bps6", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"bps7", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"bps8", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"bps9", 0, L"-1,-1,-1,0"},
	{OPTT_STRING, L"path", 0, L"-1,-1,-1,0"},

	{OPTT_BOOL, L"soundEnable", 0},
	{OPTT_BOOL, L"debuggerEnable", 0},
	{OPTT_BOOL, L"keyboardEnable", 0},
	{OPTT_BOOL, L"turboEnable", 0},
	{OPTT_BOOL, L"decimalEnable", 1},
	{OPTT_BOOL, L"executeEnable", 1},
	{OPTT_BOOL, L"debuggerShowPADDR", 1},
	{OPTT_BOOL, L"debuggerShowPPADDR", 0},
	{OPTT_BOOL, L"debuggerShowData", 0},
	{OPTT_DWORD, L"delayCPU", 10},
	{OPTT_DWORD, L"delayGPU", 20},
	{OPTT_DWORD, L"soundFrequency", 44100},
	{OPTT_DWORD, L"memoryModel", MODEL_48K},
	{OPTT_DWORD, L"postProcess", PP_BILINEAR},
	{OPTT_DWORD, L"aspectRatio", AR_AS_IS},

	{OPTT_STRING, L"keyboardWndPos", 0},
	{OPTT_STRING, L"debuggerdWndPos", 0},
	{OPTT_STRING, L"mainWndPos", 0}
};

SettingsZX::SettingsZX() {
	::GetModuleFileName(hInst,	mainDir.reserved(MAX_PATH), MAX_PATH);
	mainDir.releaseReserved();
	mainDir = mainDir.left(mainDir.find_rev(L'\\') + 1);
	options = opt;
	get(OPT_CUR_PATH)->sval = mainDir;
}

bool SettingsZX::readLine(FILE* hh, StringZX& name, StringZX& value) {
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

	_wfopen_s(&hf, path, L"rt,ccs=UTF-16LE");
	if(hf) {
		while(true) {
			if(!readLine(hf, name, value)) break;
			for(auto& o : opt) {
				if(name != o.name) continue;
				switch(o.type) {
					case OPTT_STRING:
						o.sval = value;
						break;
					case OPTT_DWORD:
						o.dval = *(ssh_d*)asm_ssh_wton(value.buffer(), (ssh_u)Radix::_dec);
						break;
					case OPTT_BOOL:
						o.dval = value == L"true";
						break;
				}
				break;
			}
		}
		fclose(hf);
	}
}

void SettingsZX::save(const StringZX& path) {
	_wfopen_s(&hf, path, L"wt,ccs=UTF-16LE");
	if(hf) {
		for(auto& o : opt) {
			ssh_cws value;
			ssh_u u;
			switch(o.type) {
				case OPTT_DWORD:
					u = o.dval;
					value = asm_ssh_ntow(&u, (ssh_u)Radix::_dec);
					break;
				case OPTT_BOOL:
					value = o.dval ? L"true" : L"false";
					break;
				default:
					value = o.sval;
					break;
			}
			auto ret = swprintf_s(tmpBuf, MAX_PATH, L"%s=%s\n", o.name, value);
			fputws(tmpBuf, hf);
		}
		fclose(hf);
	}
}
