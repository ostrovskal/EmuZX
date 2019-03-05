#pragma once

enum Options {
	OPT_MRU0,
	OPT_MRU1,
	OPT_MRU2,
	OPT_MRU3,
	OPT_MRU4,
	OPT_MRU5,
	OPT_MRU6,
	OPT_MRU7,
	OPT_MRU8,
	OPT_MRU9,

	OPT_BPS0,
	OPT_BPS1,
	OPT_BPS2,
	OPT_BPS3,
	OPT_BPS4,
	OPT_BPS5,
	OPT_BPS6,
	OPT_BPS7,
	OPT_BPS8,
	OPT_BPS9,

	OPT_CUR_PATH,

	OPT_WND_KEY_POS,
	OPT_WND_DEBUG_POS,
	OPT_WND_MAIN_POS,

	OPT_COLOR_BLACK,
	OPT_COLOR_BLUE,
	OPT_COLOR_RED,
	OPT_COLOR_MAGENTA,
	OPT_COLOR_GREEN,
	OPT_COLOR_CYAN,
	OPT_COLOR_YELLOW,
	OPT_COLOR_WHITE,
	OPT_COLOR_L_BLACK,
	OPT_COLOR_L_BLUE,
	OPT_COLOR_L_RED,
	OPT_COLOR_L_MAGENTA,
	OPT_COLOR_L_GREEN,
	OPT_COLOR_L_CYAN,
	OPT_COLOR_L_YELLOW,
	OPT_COLOR_L_WHITE,

	OPT_SIZE_BORDER,

	OPT_PERIOD_CPU,
	OPT_PERIOD_BLINK,
	OPT_AUTO_SAVE,

	OPT_SND_AY,
	OPT_SND_BEEPER,
	OPT_SND_COVOX,
	OPT_SND_SPECDRUM,
	OPT_SND_8BIT,
	OPT_SND_MIC,
	OPT_SND_AY_STEREO,
	OPT_SND_FREQUENCY,
	OPT_SND_BEEPER_VOL,
	OPT_SND_AY_VOL,
	OPT_SND_DEVICE_ID,

	OPT_DEBUGGER,
	OPT_KEYBOARD,
	OPT_TURBO,
	OPT_DECIMAL,
	OPT_EXECUTE,
	OPT_SOUND,
	OPT_JOYSTICK,
	OPT_TRAP_TAPE,

	OPT_MODEL,
	OPT_FILTER,
	OPT_ASPECT_RATIO,

	OPT_JOY1_MAPPING,
	OPT_JOY2_MAPPING,

	OPT_JOY1_STATUS,
	OPT_JOY2_STATUS
};

#define OPTT_STRING			0
#define OPTT_DWORD			1
#define OPTT_HEX			2
#define OPTT_BOOL			3

#define AY_STEREO_NONE		0
#define AY_STEREO_ABC		1
#define AY_STEREO_ACB		2

#define SND_FREQ_44100		0
#define SND_FREQ_22050		1
#define SND_FREQ_11025		2

#define AR_1X				0
#define AR_2X				1
#define AR_3X				2
#define AR_4X				3
#define AR_AS_IS			4

#define JOY_KEMPSTON		0
#define JOY_SINCLAIR_I		1
#define JOY_SINCLAIR_II		2
#define JOY_CURSOR			3
#define JOY_KEYBOARD		4
#define JOY_CUSTOM			5

#define DELAY_CPU			10
#define DELAY_GPU			20

#define MODEL_48K			0
#define MODEL_128K			1
#define MODEL_PENTAGON_128K	2
#define MODEL_SCORPION_256K	3

#define FILTER_NONE			0
#define FILTER_MIXED		1
#define FILTER_BILINEAR		2

struct ZX_OPTION {
	ssh_b type;
	ssh_cws name;
	DWORD ddef, dval;
	zxString sdef, sval;
};

class zxSettings {
public:
	zxSettings();
	
	// загрузить установки
	void load(const zxString& path);

	// сохранить установки
	void save(const zxString& path);

	// получить опцию по ИД
	ZX_OPTION* get(int idx) const { return &options[idx]; }

	// папка приложения
	zxString mainDir;

	// имя последней загруженной программы(игры)
	zxString nameLoadProg;
protected:
	// прочитать строку из настроек
	bool readLine(FILE* hh, zxString& name, zxString& value);
		
	// массив опций
	ZX_OPTION* options;
};