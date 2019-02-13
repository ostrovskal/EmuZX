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

#define OPT_SOUND		21
#define OPT_DEBUGGER	22
#define OPT_KEYBOARD	23
#define OPT_TURBO		24
#define OPT_DECIMAL		25
#define OPT_EXECUTE		26
#define OPT_SHOW_PADDR	27
#define OPT_SHOW_PPADDR	28
#define OPT_SHOW_DATA	29
#define OPT_DELAY_CPU	30
#define OPT_DELAY_GPU	31
#define OPT_SND_FREQ	32
#define OPT_MEM_MODEL	33
#define OPT_PP			34
#define OPT_ASPECT_RATIO 35

#define OPT_WND_KEY_POS		36
#define OPT_WND_DEBUG_POS	37
#define OPT_WND_MAIN_POS	38

#define OPTT_STRING		0
#define OPTT_DWORD		1
#define OPTT_BOOL		2

#define AR_1X			0
#define AR_2X			1
#define AR_3X			2
#define AR_4X			3
#define AR_AS_IS		4

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
	ZX_OPTION* get(int idx) { return &options[idx]; }

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