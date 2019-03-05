
#include "stdafx.h"
#include "zxSettings.h"

static ZX_OPTION opt[] = {
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

	{OPTT_STRING, L"keyboardWndPos"},
	{OPTT_STRING, L"debuggerdWndPos"},
	{OPTT_STRING, L"mainWndPos"},

	{OPTT_HEX, L"colorBlack", 0xff000000},
	{OPTT_HEX, L"colorBlue", 0xff2030c0},
	{OPTT_HEX, L"colorRed", 0xffc04010},
	{OPTT_HEX, L"colorMagenta", 0xffc040c0},
	{OPTT_HEX, L"colorGreen", 0xff40b010},
	{OPTT_HEX, L"colorCyan", 0xff50c0b0},
	{OPTT_HEX, L"colorYellow", 0xffe0c010},
	{OPTT_HEX, L"colorWhite", 0xffc0c0c0},
	{OPTT_HEX, L"colorLightBlack", 0xff000000},
	{OPTT_HEX, L"colorLightBlue", 0xff3040ff},
	{OPTT_HEX, L"colorLightRed", 0xffff4030},
	{OPTT_HEX, L"colorLightMagenta", 0xffff70f0},
	{OPTT_HEX, L"colorLightGreen", 0xff50e010},
	{OPTT_HEX, L"colorLightCyan", 0xff50e0ff},
	{OPTT_HEX, L"colorLightYellow", 0xffffe850},
	{OPTT_HEX, L"colorLightWhite", 0xffffffff},

	{OPTT_DWORD, L"sizeBorder", 4},

	{OPTT_DWORD, L"periodCpu", 5},
	{OPTT_DWORD, L"periodBlink", 3},
	{OPTT_BOOL, L"autoSave", 1},

	{OPTT_BOOL, L"soundEnableAY", 1},
	{OPTT_BOOL, L"soundEnableBeeper", 1},
	{OPTT_BOOL, L"soundEnableCovox", 1},
	{OPTT_BOOL, L"soundEnableSpecdrum", 1},
	{OPTT_BOOL, L"sound8Bit", 0},
	{OPTT_BOOL, L"soundEnableMic", 1},
	{OPTT_DWORD, L"soundAyStereo", AY_STEREO_ABC},
	{OPTT_DWORD, L"soundFrequency", SND_FREQ_44100},
	{OPTT_DWORD, L"soundBeeperVolume", 50},
	{OPTT_DWORD, L"soundAyVolume", 50},
	{OPTT_DWORD, L"soundDeviceID", 0},

	{OPTT_BOOL, L"debuggerEnable", 0},
	{OPTT_BOOL, L"keyboardEnable", 0},
	{OPTT_BOOL, L"turboEnable", 0},
	{OPTT_BOOL, L"decimalEnable", 0},
	{OPTT_BOOL, L"executeEnable", 1},
	{OPTT_BOOL, L"soundEnable", 1},
	{OPTT_BOOL, L"joyEnable", 0},

	{OPTT_DWORD, L"model", MODEL_48K},
	{OPTT_DWORD, L"filter", FILTER_NONE},
	{OPTT_DWORD, L"aspectRatio", AR_2X},

	{OPTT_DWORD, L"joy1Mapping", JOY_KEMPSTON},
	{OPTT_DWORD, L"joy2Mapping", JOY_SINCLAIR_I},

	{OPTT_STRING, L"joy1Status"},
	{OPTT_STRING, L"joy2Status"}
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
	FILE* hf;

	// значения по умолчанию в фактические значения(если, например, отсутствует файл настроек)
	for(auto& o : opt) { o.dval = o.ddef; o.sval = o.sdef; }
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
					case OPTT_HEX:
						o.dval = *(ssh_d*)asm_ssh_wton(value.buffer(), o.type == OPTT_HEX);
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
	FILE* hf;

	if(!_wfopen_s(&hf, path, L"wt,ccs=UTF-16LE")) {
		for(auto& o : opt) {
			ssh_cws value;
			ssh_u u;
			switch(o.type) {
				case OPTT_DWORD:
				case OPTT_HEX:
					u = o.dval;
					value = asm_ssh_ntow(&u, o.type == OPTT_HEX);
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
