#pragma once

#define DELAY_CPU		10
#define DELAY_GPU		20

#define MODEL_48K		0
#define MODEL_128K		1
#define MODEL_SCORPION	2
#define MODEL_PENTAGON	3

#define PP_NONE			0
#define PP_MIXED		1
#define PP_BILINEAR		2

#define OPT_MRU0		0
#define OPT_MRU1		1
#define OPT_MRU2		2
#define OPT_MRU3		3
#define OPT_MRU4		4
#define OPT_MRU5		5
#define OPT_MRU6		6
#define OPT_MRU7		7
#define OPT_MRU8		8
#define OPT_MRU9		9

#define OPT_BPS0		10
#define OPT_BPS1		11
#define OPT_BPS2		12
#define OPT_BPS3		13
#define OPT_BPS4		14
#define OPT_BPS5		15
#define OPT_BPS6		16
#define OPT_BPS7		17
#define OPT_BPS8		18
#define OPT_BPS9		19

#define OPT_CUR_PATH	20

#define OPT_SND_AY			21
#define OPT_SND_BEEPER		22
#define OPT_SND_COVOX		23
#define OPT_SND_SPECDRUM	24
#define OPT_SND_8BIT		25
#define OPT_SND_MIC			26
#define OPT_SND_AY_STEREO	27

#define OPT_DEBUGGER		28
#define OPT_KEYBOARD		29
#define OPT_TURBO			30
#define OPT_DECIMAL			31
#define OPT_EXECUTE			32
#define OPT_SHOW_PADDR		33
#define OPT_SHOW_PPADDR		34
#define OPT_SHOW_DATA		35
#define OPT_DELAY_CPU		36
#define OPT_DELAY_GPU		37
#define OPT_PERIOD_BLINK	38
#define OPT_PERIOD_SND		39
#define OPT_PERIOD_BORDER	40
#define OPT_WRITE_ROM		41
#define OPT_AUTO_SAVE		42

#define OPT_SND_FREQUENCY	43
#define OPT_SND_BEEPER_VOL	44
#define OPT_SND_AY_VOL		45
#define OPT_SND_COVOX_VOL	46
#define OPT_SND_SPECDRUM_VOL 47

#define OPT_MEM_MODEL		48
#define OPT_PP				49
#define OPT_ASPECT_RATIO	50

#define OPT_WND_KEY_POS		51
#define OPT_WND_DEBUG_POS	52
#define OPT_WND_MAIN_POS	53

#define OPT_COLOR_BLACK		54
#define OPT_COLOR_BLUE		55
#define OPT_COLOR_RED		56
#define OPT_COLOR_MAGENTA	57
#define OPT_COLOR_GREEN		58
#define OPT_COLOR_CYAN		59
#define OPT_COLOR_YELLOW	60
#define OPT_COLOR_WHITE		61
#define OPT_COLOR_L_BLACK	62
#define OPT_COLOR_L_BLUE	63
#define OPT_COLOR_L_RED		64
#define OPT_COLOR_L_MAGENTA	65
#define OPT_COLOR_L_GREEN	66
#define OPT_COLOR_L_CYAN	67
#define OPT_COLOR_L_YELLOW	68
#define OPT_COLOR_L_WHITE	69

#define OPT_JOY1_MAPPING	70
#define OPT_JOY2_MAPPING	71
#define OPT_JOY3_MAPPING	72
#define OPT_JOY4_MAPPING	73

#define OPT_JOY1_STATUS		74
#define OPT_JOY2_STATUS		75
#define OPT_JOY3_STATUS		76
#define OPT_JOY4_STATUS		77

#define OPT_SOUND_ALL		78

#define OPTT_STRING		0
#define OPTT_DWORD		1
#define OPTT_BOOL		2

#define AY_STEREO_NONE	0
#define AY_STEREO_ABC	1
#define AY_STEREO_ACB	2

#define SND_FREQ_22050	0
#define SND_FREQ_44100	1
#define SND_FREQ_48000	2

#define AR_1X			0
#define AR_2X			1
#define AR_3X			2
#define AR_4X			3
#define AR_AS_IS		4

#define JOY_KEMPSTON		0
#define JOY_INTERFACE_I		1
#define JOY_INTERFACE_II	2
#define JOY_SINCLAIR		3
#define JOY_CURSOR			4
#define JOY_KEYBOARD		5	// O,P,Q,A,SPACE,M,N,B,V
#define JOY_CUSTOM			6

struct ZX_OPTION {
	ssh_b type;
	ssh_cws name;
	DWORD dval;
	zxString sval;
};

class zxSettings {
public:
	zxSettings();
	virtual ~zxSettings() { options = nullptr; }
	
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