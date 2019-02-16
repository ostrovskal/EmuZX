#pragma once

#include <mmsystem.h>
#include "dsound.h"

#pragma comment(lib,"dsound.lib")

#define MIN_SPEED_PERCENTAGE	2
#define MAX_SPEED_PERCENTAGE	500
#define AMPL_BEEPER				( 50 * 256)
#define AMPL_TAPE				( 2 * 256 )
#define AMPL_AY_TONE			( 24 * 256 )
#define AY_CHANGE_MAX			8000

#define AY_ENV_CONT				8
#define AY_ENV_ATTACK			4
#define AY_ENV_ALT				2
#define AY_ENV_HOLD				1
#define AY_CLOCK_DIVISOR		16
#define AY_CLOCK_RATIO			2

struct ZX_AY_CHANGE {
	DWORD tstates;
	unsigned char reg, val;
};

struct ZX_SPEAKER_TYPE {
	int bass;
	double treble;
};

class zxBlipBuffer;
class zxBlipSynth;

class zxSND {
public:
	struct SOUNDBUFFER {
		LPDIRECTSOUNDBUFFER		lp;
		void*					address;
		long					nNewSize;
		long					nSize;
	};

	zxSND();
	
	virtual ~zxSND();
	
	void execute(ssh_u tm);
protected:
	void frame(short* data, int len);
	void uninit();
	bool dxinit();
	double getVolume(int type);
	void finish();
	bool initBlip(zxBlipBuffer** buf, zxBlipSynth** synth);
	void ayInit();
	bool isInSoundEnabledRange();
	void ayDoTone(int level, unsigned int tone_count, int *var, int chan);
	void ayOverlay();
	DWORD getEffectiveProcessorSpeed();
	bool init();
	void ayWrite(int reg, int val, DWORD now);
	void ayReset();
	void specdrumWrite(ssh_w port, ssh_b val);
	void covoxWrite(ssh_w port, ssh_b val);
	void beeper(ssh_d at_tstates, int on);

	LPDIRECTSOUND dSnd;
	LPDIRECTSOUNDBUFFER	sndBuf;

	int soundFramesiz;
	int soundChannels;
	unsigned int ayToneLevels[16];
	unsigned int ayToneTick[3], ayToneHigh[3], ayNoiseTick;
	unsigned int ayToneCycles, ayEnvCycles;
	unsigned int ayEnvInternalTick, ayEnvTick;
	unsigned int ayTonePeriod[3], ayNoisePeriod, ayEnvPeriod;
	ssh_b ayRegisters[16];
	ZX_AY_CHANGE ayChange[AY_CHANGE_MAX];
	int ayChangeCount;

	zxBlipBuffer* blipLeft, *blipRight;
	short* samples;

	zxBlipSynth* beepLeft, *beepRight;
	zxBlipSynth* ayLeftA, *ayLeftB, *ayLeftC;
	zxBlipSynth* ayRightA, *ayRightB, *ayRightC;
	zxBlipSynth* specLeft, *specRight;
	zxBlipSynth* covoxLeft, *covoxRight;
	int enabled;
	int enabledEver;
	int ayStereo;
};
