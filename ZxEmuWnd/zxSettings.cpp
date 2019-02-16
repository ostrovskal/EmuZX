
#include "stdafx.h"
#include "zxSettings.h"
#include "zxDebugger.h"

extern zxDebugger* debug;

ssh_cws nameROMs[] = {L"48K", L"128K", L"SCORPION", L"PENTAGON"};
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
	{OPTT_STRING, L"bps0"},
	{OPTT_STRING, L"bps1"},
	{OPTT_STRING, L"bps2"},
	{OPTT_STRING, L"bps3"},
	{OPTT_STRING, L"bps4"},
	{OPTT_STRING, L"bps5"},
	{OPTT_STRING, L"bps6"},
	{OPTT_STRING, L"bps7"},
	{OPTT_STRING, L"bps8"},
	{OPTT_STRING, L"bps9"},
	{OPTT_STRING, L"path"},

	{OPTT_BOOL, L"soundEnableAY", 1},
	{OPTT_BOOL, L"soundEnableBeeper", 1},
	{OPTT_BOOL, L"soundEnableCovox", 1},
	{OPTT_BOOL, L"soundEnableSpecdrum", 1},
	{OPTT_BOOL, L"soundEnable8Bit", 0},
	{OPTT_BOOL, L"soundEnableMic", 1},
	{OPTT_DWORD, L"soundEnableAyStereo", AY_STEREO_ABC},

	{OPTT_BOOL, L"debuggerEnable"},
	{OPTT_BOOL, L"keyboardEnable"},
	{OPTT_BOOL, L"turboEnable"},
	{OPTT_BOOL, L"decimalEnable", 0},
	{OPTT_BOOL, L"executeEnable", 1},
	{OPTT_BOOL, L"debuggerShowPADDR", 1},
	{OPTT_BOOL, L"debuggerShowPPADDR"},
	{OPTT_BOOL, L"debuggerShowData"},
	{OPTT_DWORD, L"delayCPU", 10},
	{OPTT_DWORD, L"delayGPU", 20},
	{OPTT_DWORD, L"periodBlink", 1},
	{OPTT_DWORD, L"periodSound", 1},
	{OPTT_DWORD, L"periodBorder", 16},
	{OPTT_BOOL, L"writeROM", 0},
	{OPTT_BOOL, L"autoSave", 1},

	{OPTT_DWORD, L"soundFrequency", SND_FREQ_44100},
	{OPTT_DWORD, L"soundBeeperVolume", 50},
	{OPTT_DWORD, L"soundAyVolume", 50},
	{OPTT_DWORD, L"soundCovoxVolume", 50},
	{OPTT_DWORD, L"soundSpecdrumVolume", 50},
	
	{OPTT_DWORD, L"memoryModel", MODEL_48K},
	{OPTT_DWORD, L"postProcess", PP_BILINEAR},
	{OPTT_DWORD, L"aspectRatio", AR_2X},

	{OPTT_STRING, L"keyboardWndPos"},
	{OPTT_STRING, L"debuggerdWndPos"},
	{OPTT_STRING, L"mainWndPos"},

	{OPTT_DWORD, L"colorBlack", 0xff000000},
	{OPTT_DWORD, L"colorBlue", 0xff2030c0},
	{OPTT_DWORD, L"colorRed", 0xffc04010},
	{OPTT_DWORD, L"colorMagenta", 0xffc040c0},
	{OPTT_DWORD, L"colorGreen", 0xff40b010},
	{OPTT_DWORD, L"colorCyan", 0xff50c0b0},
	{OPTT_DWORD, L"colorYellow", 0xffe0c010},
	{OPTT_DWORD, L"colorWhite", 0xffc0c0c0},
	{OPTT_DWORD, L"colorLightBlack", 0xff000000},
	{OPTT_DWORD, L"colorLightBlue", 0xff3040ff},
	{OPTT_DWORD, L"colorLightRed", 0xffff4030},
	{OPTT_DWORD, L"colorLightMagenta", 0xffff70f0},
	{OPTT_DWORD, L"colorLightGreen", 0xff50e010},
	{OPTT_DWORD, L"colorLightCyan", 0xff50e0ff},
	{OPTT_DWORD, L"colorLightYellow", 0xffffe850},
	{OPTT_DWORD, L"colorLightWhite", 0xffffffff},

	{OPTT_STRING, L"joy1Status"},
	{OPTT_STRING, L"joy2Status"},
	{OPTT_STRING, L"joy3Status"},
	{OPTT_STRING, L"joy4Status"},
	{OPTT_STRING, L"joy1Mapping", JOY_KEMPSTON},
	{OPTT_STRING, L"joy2Mapping", JOY_INTERFACE_I},
	{OPTT_STRING, L"joy3Mapping", JOY_INTERFACE_II},
	{OPTT_STRING, L"joy4Mapping", JOY_CURSOR},

	{OPTT_BOOL, L"soundEnable"}
};

zxSettings::zxSettings() {
	::GetModuleFileName(hInst,	mainDir.reserved(MAX_PATH), MAX_PATH);
	mainDir.releaseReserved();
	mainDir = mainDir.left(mainDir.find_rev(L'\\') + 1);
	options = opt;
	get(OPT_CUR_PATH)->sval = mainDir;
}

bool zxSettings::readLine(FILE* hh, zxString& name, zxString& value) {
	zxString tmp(L'\0', 256);
	if(!fgetws(tmp.buffer(), 255, hh)) return false;
	auto l = wcslen(tmp.buffer()) - 1;
	if(tmp[l] == L'\n') tmp.set(l, L'\0');
	name = tmp.left(tmp.find(L'='));
	value = tmp.substr(tmp.find(L'=') + 1);
	return true;
}

void zxSettings::load(const zxString& path) {
	zxString name, value;

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

void zxSettings::save(const zxString& path) {
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
