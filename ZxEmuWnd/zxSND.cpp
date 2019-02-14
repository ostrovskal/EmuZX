
#include "stdafx.h"
#include "zxSND.h"
#include "zxBlipBuffer.h"
#include "zxCPU.h"

#define MAX_AUDIO_BUFFER		8192 * 5

static DWORD nextpos;
static int sixteenbit;

static ZX_SPEAKER_TYPE speaker_type[] = { {200, -37.0}, {1000, -67.0}, {0, 0.0} };

void zxSND::execute(ssh_u new_tm) {
	return;
	long count;
	ayOverlay();
	blipLeft->endFrame(10);// machine_current->timings.tstates_per_frame);
	if(ayStereo != SOUND_STEREO_AY_NONE) {
		blipRight->endFrame(10);// machine_current->timings.tstates_per_frame);
		count = blipLeft->readSamples(samples, soundFramesiz, 1);
		blipRight->readSamples(samples + 1, count, 1);
		count <<= 1;
	} else {
		count = blipLeft->readSamples(samples, soundFramesiz, BLIP_BUFFER_DEF_STEREO);
	}
	frame(samples, count);
	//movie_add_sound(samples, count);
	ayChangeCount = 0;
}

void zxSND::frame(short* data, int len) {
	DWORD i1, i2;
	int lsb = 1;

	UCHAR *ucbuffer1, *ucbuffer2;
	DWORD length1, length2;
	DWORD playcursor;
	long cursordiff;

	if(sixteenbit) len *= 2;

	while(len) {
		while(true) {
			IDirectSoundBuffer_GetCurrentPosition(sndBuf, &playcursor, NULL);
			cursordiff = (long)nextpos - (long)playcursor;
			if(playcursor > nextpos) cursordiff += MAX_AUDIO_BUFFER;
			if(cursordiff < len * 6) break;
			Sleep(10);
		}
		if(SUCCEEDED(IDirectSoundBuffer_Lock(sndBuf, nextpos, (DWORD)len, (void**)&ucbuffer1, &length1, (void**)&ucbuffer2, &length2, 0))) {
			for(i1 = 0; i1 < length1 && len > 0; i1++) {
				if(sixteenbit) {
					if(lsb) {
						ucbuffer1[i1] = *data & 0xff;
					} else {
						ucbuffer1[i1] = *data >> 8;
						data++;
					}
					lsb = !lsb;
				} else {
					ucbuffer1[i1] = ((*data++) >> 8) ^ 0x80;
				}
				nextpos++;
				len--;
			}
			if(ucbuffer2) {
				for(i2 = 0; i2 < length2 && len > 0; i2++) {
					if(sixteenbit) {
						if(lsb) {
							ucbuffer2[i2] = *data & 0xff;
						} else {
							ucbuffer2[i2] = *data >> 8;
							data++;
						}
						lsb = !lsb;
					} else {
						ucbuffer2[i2] = ((*data++) >> 8) ^ 0x80;
					}
					nextpos++;
					len--;
				}
			} else {
				i2 = 0;
			}
			if(nextpos >= MAX_AUDIO_BUFFER) nextpos -= MAX_AUDIO_BUFFER;
			IDirectSoundBuffer_Unlock(sndBuf, ucbuffer1, i1, ucbuffer2, i2);
		}
	}
}

bool zxSND::init(int freq, int stereo, bool _8bit) {

	WAVEFORMATEX pcmwf;
	DSBUFFERDESC dsbd;

	CoInitialize(NULL);

	try {
		if(FAILED(CoCreateInstance(CLSID_DirectSound, NULL, CLSCTX_INPROC_SERVER, IID_IDirectSound, (void**)&dSnd))) throw(0);
		if(FAILED(IDirectSound_Initialize(dSnd, NULL))) throw(0);
		if(FAILED(IDirectSound_SetCooperativeLevel(dSnd, GetDesktopWindow(), DSSCL_NORMAL))) throw(0);

		memset(&pcmwf, 0, sizeof(WAVEFORMATEX));
		pcmwf.cbSize = 0;
		if(_8bit) {
			pcmwf.wBitsPerSample = 8;
			sixteenbit = 0;
		} else {
			pcmwf.wBitsPerSample = 16;
			sixteenbit = 1;
		}
		pcmwf.nChannels = stereo ? 2 : 1;
		pcmwf.nBlockAlign = pcmwf.nChannels * (sixteenbit ? 2 : 1);
		pcmwf.nSamplesPerSec = freq;
		pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
		pcmwf.wFormatTag = WAVE_FORMAT_PCM;

		memset(&dsbd, 0, sizeof(DSBUFFERDESC));
		dsbd.dwBufferBytes = MAX_AUDIO_BUFFER;
		dsbd.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
		dsbd.dwSize = sizeof(DSBUFFERDESC);
		dsbd.lpwfxFormat = &pcmwf;

		if(FAILED(IDirectSound_CreateSoundBuffer(dSnd, &dsbd, &sndBuf, NULL))) throw(0);
		if(FAILED(IDirectSoundBuffer_Play(sndBuf, 0, 0, DSBPLAY_LOOPING))) throw(0);

		nextpos = 0;

		return true;
	} catch(...) {
		uninit();
	}
	return false;
}

void zxSND::uninit() {
	if(sndBuf) {
		IDirectSoundBuffer_Stop(sndBuf);
		IDirectSoundBuffer_Release(sndBuf);
	}
	IDirectSound_Release(dSnd);
	CoUninitialize();
}

double zxSND::getVolume(int volume) {
	if(volume < 0) volume = 0;
	else if(volume > 100) volume = 100;
	return volume / 100.0;
}

zxSND::zxSND() : dSnd(nullptr), sndBuf(nullptr) {
	blipLeft = nullptr;
	blipRight = nullptr;
	beepLeft = nullptr;
	beepRight = nullptr;
	specLeft = nullptr;
	specRight = nullptr;
	covoxLeft = nullptr;
	covoxRight = nullptr;
	ayLeftA = nullptr;
	ayRightA = nullptr;
	ayLeftB = nullptr;
	ayRightB = nullptr;
	ayLeftC = nullptr;
	ayRightC = nullptr;
	samples = nullptr;
	enabledEver = 0;
	ayStereo = SOUND_STEREO_AY_NONE;

	init();
}

zxSND::~zxSND() {
	finish();
	uninit();
}

void zxSND::finish() {
	SAFE_DELETE(blipLeft);
	SAFE_DELETE(blipRight);
	SAFE_DELETE(beepLeft);
	SAFE_DELETE(beepRight);
	SAFE_DELETE(specLeft);
	SAFE_DELETE(specRight);
	SAFE_DELETE(covoxLeft);
	SAFE_DELETE(covoxRight);
	SAFE_DELETE(ayLeftA);
	SAFE_DELETE(ayRightA);
	SAFE_DELETE(ayLeftB);
	SAFE_DELETE(ayRightB);
	SAFE_DELETE(ayLeftC);
	SAFE_DELETE(ayRightC);

	SAFE_DELETE(samples);
}

bool zxSND::initBlip(zxBlipBuffer** buf, zxBlipSynth** syn) {
	*buf = new zxBlipBuffer();
	*syn = new zxBlipSynth();

	(*buf)->setClockRate(getEffectiveProcessorSpeed());
	if((*buf)->setSampleRate(22050, 1000)) {
		finish();
		return false;
	}
	(*syn)->setVolume(getVolume(50));
	(*syn)->setOutput(*buf);
	(*buf)->setBassFreq(speaker_type[0].bass);
	(*syn)->setTrebleEq(speaker_type[0].treble);
	return true;
}

void zxSND::ayInit() {
	static const int levels[16] = { 0x0000, 0x0385, 0x053D, 0x0770, 0x0AD7, 0x0FD5, 0x15B0, 0x230C, 0x2B4C, 0x43C1, 0x5A4B, 0x732F, 0x9204, 0xAFF1, 0xD921, 0xFFFF };

	for(int f = 0; f < 16; f++) ayToneLevels[f] = (levels[f] * AMPL_AY_TONE + 0x8000) / 0xffff;
	ayNoiseTick = ayNoisePeriod = 0;
	ayEnvInternalTick = ayEnvTick = ayEnvPeriod = 0;
	ayToneCycles = ayEnvCycles = 0;
	for(int f = 0; f < 3; f++) ayToneTick[f] = ayToneHigh[f] = 0, ayTonePeriod[f] = 1;
	ayChangeCount = 0;
}

bool zxSND::isInSoundEnabledRange() {
	return true;
	//return settings_current.emulation_speed >= MIN_SPEED_PERCENTAGE && settings_current.emulation_speed <= MAX_SPEED_PERCENTAGE;
}

void zxSND::ayDoTone(int level, unsigned int tone_count, int *var, int chan) {
	*var = 0;
	ayToneTick[chan] += tone_count;
	if(ayToneTick[chan] >= ayTonePeriod[chan]) {
		ayToneTick[chan] -= ayTonePeriod[chan];
		ayToneHigh[chan] = !ayToneHigh[chan];
	}
	if(level) {
		*var = (ayToneHigh[chan] ? level : 0);
	}
}

void zxSND::ayOverlay() {
	static int rng = 1;
	static int noise_toggle = 0;
	static int env_first = 1, env_rev = 0, env_counter = 15;
	int tone_level[3];
	int mixer, envshape;
	int g, level;
	DWORD f;
	ZX_AY_CHANGE *change_ptr = ayChange;
	int changes_left = ayChangeCount;
	int reg, r;
	int chan1, chan2, chan3;
	int last_chan1 = 0, last_chan2 = 0, last_chan3 = 0;
	unsigned int tone_count, noise_count;
	for(f = 0; f < 10/*machine_current->timings.tstates_per_frame*/; f += AY_CLOCK_DIVISOR * AY_CLOCK_RATIO) {
		while(changes_left && f >= change_ptr->tstates) {
			ayRegisters[reg = change_ptr->reg] = change_ptr->val;
			change_ptr++;
			changes_left--;
			switch(reg) {
				case 0: case 1: case 2: case 3: case 4: case 5:
					r = reg >> 1;
					ayTonePeriod[r] = (ayRegisters[reg & ~1] | (ayRegisters[reg | 1] & 15) << 8);
					if(!ayTonePeriod[r]) ayTonePeriod[r]++;
					if(ayToneTick[r] >= ayTonePeriod[r] * 2)
						ayToneTick[r] %= ayTonePeriod[r] * 2;
					break;
				case 6:
					ayNoiseTick = 0;
					ayNoisePeriod = (ayRegisters[reg] & 31);
					break;
				case 11: case 12:
					ayEnvPeriod = ayRegisters[11] | (ayRegisters[12] << 8);
					break;
				case 13:
					ayEnvInternalTick = ayEnvTick = ayEnvCycles = 0;
					env_first = 1;
					env_rev = 0;
					env_counter = (ayRegisters[13] & AY_ENV_ATTACK) ? 0 : 15;
					break;
			}
		}
		for(g = 0; g < 3; g++) tone_level[g] = ayToneLevels[ayRegisters[8 + g] & 15];
		envshape = ayRegisters[13];
		level = ayToneLevels[env_counter];
		for(g = 0; g < 3; g++) {
			if(ayRegisters[8 + g] & 16) tone_level[g] = level;
		}
		ayEnvCycles += AY_CLOCK_DIVISOR;
		noise_count = 0;
		while(ayEnvCycles >= 16) {
			ayEnvCycles -= 16;
			noise_count++;
			ayEnvTick++;
			while(ayEnvTick >= ayEnvPeriod) {
				ayEnvTick -= ayEnvPeriod;
				if(env_first ||
					((envshape & AY_ENV_CONT) && !(envshape & AY_ENV_HOLD))) {
					if(env_rev)
						env_counter -= (envshape & AY_ENV_ATTACK) ? 1 : -1;
					else
						env_counter += (envshape & AY_ENV_ATTACK) ? 1 : -1;
					if(env_counter < 0)
						env_counter = 0;
					if(env_counter > 15)
						env_counter = 15;
				}
				ayEnvInternalTick++;
				while(ayEnvInternalTick >= 16) {
					ayEnvInternalTick -= 16;
					if(!(envshape & AY_ENV_CONT))
						env_counter = 0;
					else {
						if(envshape & AY_ENV_HOLD) {
							if(env_first && (envshape & AY_ENV_ALT))
								env_counter = (env_counter ? 0 : 15);
						} else {
							/* non-hold */
							if(envshape & AY_ENV_ALT)
								env_rev = !env_rev;
							else
								env_counter = (envshape & AY_ENV_ATTACK) ? 0 : 15;
						}
					}
					env_first = 0;
				}
				if(!ayEnvPeriod) break;
			}
		}
		chan1 = tone_level[0];
		chan2 = tone_level[1];
		chan3 = tone_level[2];
		mixer = ayRegisters[7];
		ayToneCycles += AY_CLOCK_DIVISOR;
		tone_count = ayToneCycles >> 3;
		ayToneCycles &= 7;
		if((mixer & 1) == 0) {
			level = chan1;
			ayDoTone(level, tone_count, &chan1, 0);
		}
		if((mixer & 0x08) == 0 && noise_toggle) chan1 = 0;
		if((mixer & 2) == 0) {
			level = chan2;
			ayDoTone(level, tone_count, &chan2, 1);
		}
		if((mixer & 0x10) == 0 && noise_toggle)
			chan2 = 0;
		if((mixer & 4) == 0) {
			level = chan3;
			ayDoTone(level, tone_count, &chan3, 2);
		}
		if((mixer & 0x20) == 0 && noise_toggle)
			chan3 = 0;
		if(last_chan1 != chan1) {
			ayLeftA->update(f, chan1);
			if(ayRightA) ayRightA->update(f, chan1);
			last_chan1 = chan1;
		}
		if(last_chan2 != chan2) {
			ayLeftB->update(f, chan2);
			if(ayRightB) ayRightB->update(f, chan2);
			last_chan2 = chan2;
		}
		if(last_chan3 != chan3) {
			ayLeftC->update(f, chan3);
			if(ayRightC) ayRightC->update(f, chan3);
			last_chan3 = chan3;
		}
		ayNoiseTick += noise_count;
		while(ayNoiseTick >= ayNoisePeriod) {
			ayNoiseTick -= ayNoisePeriod;
			if((rng & 1) ^ ((rng & 2) ? 1 : 0)) noise_toggle = !noise_toggle;
			if(rng & 1) rng ^= 0x24000;
			rng >>= 1;
			if(!ayNoisePeriod) break;
		}
	}
}

DWORD zxSND::getEffectiveProcessorSpeed() {
	return 50;// machine_current->timings.processor_speed / 100 * settings_current.emulation_speed;
}

void zxSND::init() {
	double treble;

	if(init(22050, false, true)) return;
	if(!initBlip(&blipLeft, &beepLeft)) return;
	if(ayStereo != SOUND_STEREO_AY_NONE && !initBlip(&blipRight, &beepRight)) return;
	
	treble = speaker_type[0].treble;
	ayLeftA = new zxBlipSynth();
	ayLeftA->setVolume(getVolume(50));
	ayLeftA->setTrebleEq(treble);

	ayLeftB = new zxBlipSynth();
	ayLeftB->setVolume(getVolume(50));
	ayLeftB->setTrebleEq(treble);

	ayLeftC = new zxBlipSynth();
	ayLeftC->setVolume(getVolume(50));
	ayLeftC->setTrebleEq(treble);

	specLeft = new zxBlipSynth();
	specLeft->setOutput(blipLeft);
	specLeft->setTrebleEq(treble);

	covoxLeft = new zxBlipSynth();
	covoxLeft->setVolume(getVolume(50));
	covoxLeft->setOutput(blipLeft);
	covoxLeft->setTrebleEq(treble);

	ayRightA = nullptr;
	ayRightB = nullptr;
	ayRightC = nullptr;

	zxBlipSynth **ayLeft = nullptr;
	zxBlipSynth **ayLeftMid = nullptr;
	zxBlipSynth **ayRightMid = nullptr;
	zxBlipSynth **ayRight = nullptr;

	if(ayStereo != SOUND_STEREO_AY_NONE) {
		if(ayStereo == SOUND_STEREO_AY_ACB) {
			ayLeft = &ayLeftA;
			ayLeftMid = &ayLeftC;
			ayRightMid = &ayRightC;
			ayRight = &ayLeftB;
		} else/* if(ayStereo == SOUND_STEREO_AY_ABC)*/ {
			ayLeft = &ayLeftA;
			ayLeftMid = &ayLeftB;
			ayRightMid = &ayRightB;
			ayRight = &ayLeftC;
		}
		(*ayLeft)->setOutput(blipLeft);
		(*ayLeftMid)->setOutput(blipLeft);
		(*ayRight)->setOutput(blipRight);

		*ayRightMid = new zxBlipSynth();
		(*ayRightMid)->setVolume(50);
		(*ayRightMid)->setOutput(blipRight);
		(*ayRightMid)->setTrebleEq(treble);

		specRight = new zxBlipSynth();
		specRight->setVolume(50);
		specRight->setOutput(blipRight);
		specRight->setTrebleEq(treble);

		covoxRight = new zxBlipSynth();
		covoxRight->setVolume(50);
		covoxRight->setOutput(blipRight);
		covoxRight->setTrebleEq(treble);

	} else {
		ayLeftA->setOutput(blipLeft);
		ayLeftB->setOutput(blipLeft);
		ayLeftC->setOutput(blipLeft);
	}
	enabled = enabledEver = 1;
	soundChannels = (ayStereo != SOUND_STEREO_AY_NONE ? 2 : 1);
	float hz = (float)10;// getEffectiveProcessorSpeed() / machine_current->timings.tstates_per_frame;
	soundFramesiz = (int)((float)22050 / hz);
	soundFramesiz++;
	samples = new short[soundFramesiz * soundChannels];
	//movie_init_sound(22050, ayStereo);
}

void zxSND::ayWrite(int reg, int val, DWORD now) {
	if(ayChangeCount < AY_CHANGE_MAX) {
		ayChange[ayChangeCount].tstates = now;
		ayChange[ayChangeCount].reg = (reg & 15);
		ayChange[ayChangeCount].val = val;
		ayChangeCount++;
	}
}

void zxSND::ayReset() {
	int f;
	ayInit();
	ayChangeCount = 0;
	for(f = 0; f < 16; f++) ayWrite(f, 0, 0);
	for(f = 0; f < 3; f++) ayToneHigh[f] = 0;
	ayToneCycles = ayEnvCycles = 0;
}

void zxSND::specdrumWrite(ssh_w port, ssh_b val) {
	/*
	specLeft->update(tstates, (val - 128) * 128);
	if(specRight) specRight->update(tstates, (val - 128) * 128);
	machine_current->specdrum.specdrum_dac = val - 128;
	*/
}

void zxSND::covoxWrite(ssh_w port, ssh_b val) {
	/*
	covoxLeft->update(tstates, val * 128);
	if(covoxRight) covoxRight->update(tstates, val * 128);
	machine_current->covox.covox_dac = val;
	*/
}

void zxSND::beeper(ssh_d at_tstates, int on) {
	static int beeper_ampl[] = {0, AMPL_TAPE, AMPL_BEEPER, AMPL_BEEPER + AMPL_TAPE};
	int val;

	/*
	if(tape_is_playing()) {
		if(!settings_current.sound_load || machine_current->timex) on = on & 0x02;
	} else {
	if(on == 1) on = 0;
	}
	*/
	if(on == 1) on = 0;
	val = beeper_ampl[on];
	beepLeft->update(at_tstates, val);
	if(ayStereo != SOUND_STEREO_AY_NONE) beepRight->update(at_tstates, val);
}
