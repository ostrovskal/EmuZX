#pragma once

#define TAPE_MAX_BLOCKS		100
#define TIMER_CONST			19

#define SND_ARRAY_LEN				1000
#define SND_MAX_OUTPUT				0x7fff
#define SND_STEP					800
#define SND_BEEPER					255

#define SND_TICKS_PER_FRAME_1_CONST	882
#define SND_TICKS_PER_FRAME_2_CONST	441
#define SND_TICKS_PER_FRAME_4_CONST	220
#define SND_TICKS_PER_FRAME_MAX		10000

class zxSound {
public:
	enum Buffers { NUM_BUFFERS = 10 };
	enum RegisterAY {
		AY_AFINE, AY_ACOARSE, AY_BFINE, AY_BCOARSE, AY_CFINE, AY_CCOARSE, AY_NOISEPER, AY_ENABLE, AY_AVOL,
		AY_BVOL, AY_CVOL, AY_EFINE, AY_ECOARSE, AY_ESHAPE, AY_PORTA, AY_BEEPER, countRegAY
	};
	struct AY_CHANNEL {
		ssh_w channelData[SND_TICKS_PER_FRAME_MAX * 2];
		ssh_i genPeriod, genCounter, noiseCounter;
		ssh_u rnd;
		bool isChannel;
		bool isNoise;
		void gen(int i, zxSound* snd);
	};
	struct AY8910_STEPS {
		ssh_d tcounter;
		ssh_b reg, val;
		ssh_i pos;
	};
	struct SND_DEVICE {
		SND_DEVICE() : name(nullptr) {}
		ssh_ws* name;
	};

	zxSound() : hsnd(0), nCurrent(0), countDevices(0), PSGcounter(0), tcounter(0), updateStep(0),
				timeout(TIMER_CONST), beepVal(0), isSndDump(false), beeperVolume(12) { }
	void play(char* buffer, int len);
	bool init(int nID);
protected:
	void update();
	void applyRegisterAY(int reg, int val);
	void setTicksPerFrame();
	void writeNVRegAY(int reg, int val);
	void writeRegAY(int reg, int val);
	void envelopeStep();
	void mix(ssh_w* buf, int freq, int offs, int count, ...);

	HWAVEOUT hsnd;
	WAVEHDR bufs[NUM_BUFFERS];
	SND_DEVICE sndDevices[20];
	AY8910_STEPS PSGARRAY[SND_ARRAY_LEN];
	int PSGcounter;

	int countDevices;
	char nCurrent;
	int SND_TICKS_PER_FRAME[3];

	int registerLatch;
	int timeout;

	ssh_i tcounter, updateStep;

	ssh_b curRegsAY[countRegAY];
	ssh_b origRegsAY[countRegAY];

	ssh_i beeperVolume;
	ssh_b cont, att, alt, hold, up, holded;

	AY_CHANNEL channelA, channelB, channelC;
	
	int stereoAY;
	bool isSndDump;

	ssh_w soundBuffer[SND_TICKS_PER_FRAME_MAX * 2 * 2];
	ssh_w soundBufferDump[SND_TICKS_PER_FRAME_MAX * 2 * 2];
	ssh_w beeperBuffer[SND_TICKS_PER_FRAME_MAX * 2];

	ssh_w beepVal;
	ssh_i periodN, periodE;
	ssh_b envVolume;
	ssh_i envelopeCounter;
	ssh_w* mixing_ch[10];
};