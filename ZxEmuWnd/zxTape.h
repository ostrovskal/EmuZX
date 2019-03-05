#pragma once

#define DIRECT_REC_SIZE		882
#define micMaxLen			9000
#define micMax				10 * 1024

#define TBLK_STD_SPEED		0x10
#define TBLK_TURBO_SPEED	0x11
#define TBLK_PURE_TONE		0x12
#define TBLK_PULSE_SEQ		0x13
#define TBLK_PURE_DATA		0x14
#define TBLK_DIRECT_REC		0x15
#define TBLK_C64_STD_ROM	0x16
#define TBLK_C64_TURBO_ROM	0x17
#define TBLK_CSW_REC		0x18
#define TBLK_GENERAL_DATA	0x19
#define TBLK_PAUSE_OR_STOP	0x20
#define TBLK_GROUP_START	0x21
#define TBLK_GROUP_END		0x22
#define TBLK_JUMP			0x23
#define TBLK_LOOP_START		0x24
#define TBLK_LOOP_END		0x25
#define TBLK_CALL_SEQ		0x26
#define TBLK_RETURN_SEQ		0x27
#define TBLK_SELECT			0x28
#define TBLK_STOP_TAPE_48K	0x2A
#define TBLK_SET_SIGNAL_LEV	0x2B
#define TBLK_TEXT_DESC		0x30
#define TBLK_MESSAGE		0x31
#define TBLK_ARCHIVE_INFO	0x32
#define TBLK_HARDWATE_TYPE	0x33
#define TBLK_EMULATION_INFO	0x34
#define TBLK_CUSTOM_INFO	0x35
#define TBLK_SCREEN			0x40
#define TBLK_VERSION		0x5A	

class zxTape {
	friend class zxBus;
public:
	struct BLOCK {
		BLOCK(ssh_b _id) : id(_id), data(nullptr), info(nullptr) { }
		~BLOCK() { delete[] data; delete[] info; }
		// тип блока
		ssh_b id;
		// длина данных, длина структуры блока
		ssh_i len, ilen;
		// данные
		ssh_b* data;
		// тип блока данных
		ssh_b* info;
	};
	#pragma pack(push, 1)
	struct STD_BLOCK {
		ssh_w pause;
		ssh_w len;
	};
	struct TURBO_BLOCK {
		ssh_w pilotL;
		ssh_w syncFL;
		ssh_w syncSL;
		ssh_w zeroL;
		ssh_w oneL;
		ssh_w pilot_toneL_inPulses;
		ssh_b used_bits;
		ssh_w pause;
		ssh_b l1, l2, l3;
	};
	struct PURE_TONE {
		ssh_w len, n;
	};
	struct PURE_DATA {
		ssh_w z, o;
		ssh_b used;
		ssh_w pause;
		ssh_b l1, l2, l3;
	};
	struct DIRECT_REC {
		ssh_w TS;
		ssh_w pause;
		ssh_b used;
		ssh_b l1, l2, l3;
	};
	struct WAVE {
		ssh_w type;
		ssh_w channels;
		ssh_i SamplesPerSec;
		ssh_i AvgBytesPerSec;
		ssh_w align;
		ssh_w bits;
	};
	#pragma pack(pop)

	zxTape();
	~zxTape() { reset(); }
	void update(int ticks);
	void reset() { blocks.free(); resetBlock(); }
	void pause() { isPause = true; }
	void stop() { next(); pos = len = 0; isPause = false; }
	int hookPC(ssh_w pc);
	bool isExecute();
	bool loadTAP(ssh_cws path);
	bool loadTZX(ssh_cws path);
	bool loadWAV(ssh_cws path);
	bool saveTAP(ssh_cws path);
	bool saveTZX(ssh_cws path);
	bool saveWAV(ssh_cws path);
protected:
	void trapLoad();
	void trapSave();
	void addBlock(ssh_b id, ssh_i len, ssh_b* data, int ilen, void* info);
	void b2bset(ssh_b* dst, ssh_i pos, ssh_b bit);
	// управление лентой в режиме TZX
	void control(int force);
	void addMicBlock();
	void resetBlock() { pos = len = 0; isPause = false; curr = blocks.root(); }
	bool isSkip();
	bool fill(ssh_b* tape, ssh_i* len, BLOCK* next);
	bool isStd();
	bool next();
	ssh_b b2bget(ssh_b* src, ssh_i pos);
	bool isPause, isTraps, isDirectMic, isReadOnly, isRecording;
	short rewCounter;
	// позиция в блоке
	ssh_i pos;
	// длина блока
	ssh_i len;
	ssh_b in, out, koef;
	// блоки
	List<BLOCK> blocks;
	// текущий блок ленты
	List<BLOCK>::Node* curr;
	// позиция магнитофона, предыдущая позиция магнитофона
	ssh_i micPos, micPrevPos;
	// буфер магнитофона
	ssh_b micBuf[micMaxLen];
	// буфер магнитофона
	ssh_b tapeMIC[micMax];
	// буфер записи магнитофона
	ssh_b recBuffer[DIRECT_REC_SIZE];
	// позиция в буфере записи магнитофона
	ssh_i recPosBuffer;
	// буфер ленты
	ssh_b tape[micMax * 2];
};