#pragma once

#define DELAY_CPU		10
#define DELAY_GPU		20

#define MODEL_48K		0
#define MODEL_128K		1

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

#define OPT_WND_KEY_POS		35
#define OPT_WND_DEBUG_POS	36
#define OPT_WND_MAIN_POS	37

#define OPTT_STRING		0
#define OPTT_DWORD		1
#define OPTT_BOOL		2

struct ZX_OPTION {
	ssh_b type;
	ssh_cws name;
	DWORD dval;
	StringZX sval;
};

class SettingsZX {
public:
	SettingsZX();
	virtual ~SettingsZX() { options = nullptr; }
	
	// ��������� ���������
	void load(const StringZX& path);

	// ��������� ���������
	void save(const StringZX& path);

	// �������� ����� �� ��
	ZX_OPTION* get(int idx) { return &options[idx]; }

	// ����� ����������
	StringZX mainDir;

	// ��� ��������� ����������� ���������(����)
	StringZX nameLoadProg;
protected:
	// ��������� ������ �� ��������
	bool readLine(FILE* hh, StringZX& name, StringZX& value);
		
	// ������ �����
	ZX_OPTION* options;
};