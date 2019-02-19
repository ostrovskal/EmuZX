
#include "stdafx.h"
#include "zxSND.h"

static int sndTicks[3] = {SND_TICKS_PER_FRAME_1_CONST, SND_TICKS_PER_FRAME_2_CONST, SND_TICKS_PER_FRAME_4_CONST};
static int sndFreq[3] = {44100, 22050, 11025};

static ssh_w volTable[zxSND::countRegAY] = {
	0x0000 / 2, 0x0344 / 2, 0x04BC / 2, 0x06ED / 2,
	0x0A3B / 2, 0x0F23 / 2, 0x1515 / 2, 0x2277 / 2,
	0x2898 / 2, 0x4142 / 2, 0x5B2B / 2, 0x726C / 2,
	0x9069 / 2, 0xB555 / 2, 0xD79B / 2, 0xFFFF / 2
};

static ssh_b eshape[] = {
	0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255,
	0, 1, 255, 255, 0, 1, 255, 255, 0, 1, 255, 255, 0, 1, 255, 255,
	1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1,
	1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1
};

void zxSND::play(char* buffer, int len) {

	char* memory = new char[len];
	memcpy(memory, buffer, len);

	WAVEHDR* curBuf(&bufs[nCurrent]);
	WAVEHDR* oldBuf((nCurrent < (NUM_BUFFERS - 1)) ? &bufs[nCurrent + 1] : &bufs[0]);
	ssh_memzero(curBuf, sizeof(WAVEHDR));
	curBuf->lpData = memory;
	curBuf->dwBufferLength = len;
	curBuf->dwFlags = 0;
	curBuf->dwLoops = 0;
	waveOutPrepareHeader(hsnd, curBuf, sizeof(WAVEHDR));
	waveOutWrite(hsnd, curBuf, sizeof(WAVEHDR));
	while(waveOutUnprepareHeader(hsnd, oldBuf, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) Sleep(15);
	SAFE_DELETE(oldBuf);
	nCurrent++;
	if(nCurrent >= NUM_BUFFERS) nCurrent = 0;
}

bool zxSND::init(int nID) {
	WAVEOUTCAPS woc;
	WAVEFORMATEX wfx;

	int freq = sndFreq[theApp->getOpt(OPT_SND_FREQUENCY)->dval];
	int bits = theApp->getOpt(OPT_SND_8BIT)->dval ? 8 : 16;

	stereoAY = theApp->getOpt(OPT_SND_AY_STEREO)->dval;
	updateStep = (ssh_i)(((double)SND_STEP * (double)freq * (double)8) / (double)(freq * bits * 2) * 2.0); // 1773400

	countDevices = waveOutGetNumDevs();

	for(int i = 0; i <= countDevices; i++) {
		SAFE_DELETE(sndDevices[i].name);
		sndDevices[i].name = new ssh_ws[MAXPNAMELEN];
		if(i == 0) {
			wcscpy_s(sndDevices[i].name, MAXPNAMELEN, L"По умолчанию");
		} else {
			waveOutGetDevCaps(i - 1, &woc, sizeof(woc));
			wcscpy_s(sndDevices[i].name, MAXPNAMELEN, woc.szPname);
		}
	}

	ssh_memzero(&wfx, sizeof(wfx));

	wfx.cbSize = sizeof(wfx);
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.nSamplesPerSec = freq;
	wfx.wBitsPerSample = bits;
	wfx.nBlockAlign = 2 * bits / 8;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * freq;

	return (waveOutOpen(&hsnd, nID, &wfx, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR);
}

void zxSND::setTicksPerFrame(int delayCpu) {
	// delayCPU = 10 => 100%
	double delitel = TIMER_CONST * 100.0 / ((double)delayCpu / 10.0) * 100.0;
	double koef = (double)delitel / (double)TIMER_CONST;

	for(int i = 0; i < 2; i++) {
		SND_TICKS_PER_FRAME[i] = (int)((double)sndTicks[i] * koef);
		if(SND_TICKS_PER_FRAME[i] > SND_TICKS_PER_FRAME_MAX) SND_TICKS_PER_FRAME[i] = SND_TICKS_PER_FRAME_MAX;
	}
}

void zxSND::writeNVRegAY(int reg, int val) {
	if(PSGcounter < SND_ARRAY_LEN) {
		PSGARRAY[PSGcounter].tcounter = theApp->bus.tCounter;
		PSGARRAY[PSGcounter].reg = reg;
		PSGARRAY[PSGcounter].val = val;
		PSGcounter++;
	}
}

void zxSND::writeRegAY(int reg, int val) {
	if(reg < 16) {
		curRegsAY[reg] = val;
		writeNVRegAY(reg, val);
	}
}

void zxSND::applyRegisterAY(int reg, int val) {
	if(reg <= AY_ESHAPE) origRegsAY[reg] = val;
	switch(reg) {
		case AY_AFINE:
		case AY_ACOARSE:
			origRegsAY[AY_ACOARSE] &= 0x0f;
			channelA.genPeriod = (origRegsAY[AY_AFINE] + 256 * origRegsAY[AY_ACOARSE]) * updateStep / SND_STEP;
			if(!channelA.genPeriod) channelA.genPeriod = updateStep / SND_STEP;
			break;
		case AY_BFINE:
		case AY_BCOARSE:
			origRegsAY[AY_BCOARSE] &= 0x0f;
			channelB.genPeriod = (origRegsAY[AY_BFINE] + 256 * origRegsAY[AY_BCOARSE]) * updateStep / SND_STEP;
			if(!channelB.genPeriod) channelB.genPeriod = updateStep / SND_STEP;
			break;
		case AY_CFINE:
		case AY_CCOARSE:
			origRegsAY[AY_CCOARSE] &= 0x0f;
			channelC.genPeriod = (origRegsAY[AY_CFINE] + 256 * origRegsAY[AY_CCOARSE]) * updateStep / SND_STEP;
			if(!channelC.genPeriod) channelC.genPeriod = updateStep / SND_STEP;
			break;
		case AY_AVOL:
			origRegsAY[AY_AVOL] &= 0x1f;
			break;
		case AY_BVOL:
			origRegsAY[AY_BVOL] &= 0x1f;
			break;
		case AY_CVOL:
			origRegsAY[AY_CVOL] &= 0x1f;
			break;
		case AY_ENABLE:
			channelA.isChannel = (origRegsAY[AY_ENABLE] & 1);
			channelB.isChannel = (origRegsAY[AY_ENABLE] & 2);
			channelC.isChannel = (origRegsAY[AY_ENABLE] & 4);
			channelA.isNoise =	 (origRegsAY[AY_ENABLE] & 8);
			channelB.isNoise =	 (origRegsAY[AY_ENABLE] & 16);
			channelC.isNoise =	 (origRegsAY[AY_ENABLE] & 32);
			break;
		case AY_NOISEPER:
			origRegsAY[AY_NOISEPER] &= 0x1f;
			periodN = origRegsAY[AY_NOISEPER] * updateStep / SND_STEP;
			if(periodN == 0) periodN = updateStep / SND_STEP;
			break;
		case AY_EFINE:
		case AY_ECOARSE:
			periodE = ((origRegsAY[AY_EFINE] + 256 * origRegsAY[AY_ECOARSE])) * updateStep / SND_STEP;
			if(periodE == 0) periodE = updateStep / SND_STEP / 2;
			break;
		case AY_ESHAPE: {
			origRegsAY[AY_ESHAPE] &= 0x0f;
			auto pshape = &eshape[origRegsAY[AY_ESHAPE] * 4];
			cont = pshape[0]; att = pshape[1];
			if(origRegsAY[AY_ESHAPE] > 7) { alt = pshape[2]; hold = pshape[3]; }
			envelopeCounter = 0;
			envVolume = (att ? 0 : 15);
			up = att;
			holded = 0;
		}
		break;
		case SND_BEEPER:
			beepVal = (val ? volTable[beeperVolume] : 0);
			break;
	}
}

void zxSND::envelopeStep() {
	if(holded == 1) return;

	if((alt ? periodE : (periodE * 2)) > envelopeCounter)
		envelopeCounter++;
	else {
		envelopeCounter = 0;
		if(up) {
			if(envVolume < 15) envVolume++;
			else {
				if(cont == 0) { holded = 1; envVolume = 0; }
				else if(hold) { if(alt) envVolume = 0; holded = 1; }
				else { if(alt) up = 0; else envVolume = 0; }
			}
		} else {
			if(envVolume > 0) envVolume--;
			else {
				if(cont == 0) { 
					holded = 1;
				} else if(hold) {
					if(alt) envVolume = 15;
					holded = 1;
				} else {
					if(alt) up = 1;
					else envVolume = 15;
				}
			}
		}
	}
}

void zxSND::execute() {
	if(!hsnd) init(0);

	if(PSGcounter > 0) {
		if(theApp->bus.tCounter == 0) theApp->bus.tCounter++;
		for(int i = 0; i < PSGcounter; i++) PSGARRAY[i].pos = (SND_TICKS_PER_FRAME[0] - 1) * PSGARRAY[i].tcounter / theApp->bus.tCounter;
	}
	// Генерим звук
	int j = 0;
	for(int i = 0; i < SND_TICKS_PER_FRAME[0]; i++) {
		if(PSGcounter > 0) {
			while(i == PSGARRAY[j].pos) {
				applyRegisterAY(PSGARRAY[j].reg, PSGARRAY[j].val);
				PSGARRAY[j].pos = SND_TICKS_PER_FRAME[0];
				j++;
				if(j >= SND_ARRAY_LEN) break;
			}
		}
		envelopeStep();
		channelA.gen(i, this);
		channelB.gen(i, this);
		channelC.gen(i, this);
		beeperBuffer[i] = beepVal;
	}
	// Микшируем и записываем
	auto sndBuf = (isSndDump ? soundBufferDump : soundBuffer);
	auto freq = theApp->getOpt(OPT_SND_FREQUENCY)->dval;
	auto bufA = channelA.channelData;
	auto bufB = channelB.channelData;
	auto bufC = channelC.channelData;
//	unsigned char stereo_mode = 0; /* 2=Mono 1=ABC 0=ACB*/
	/*
	if (sound_fil!=-1)
	{
	if (stereo_mode==2)
	{
	Mix(sound_bufferD, 44100, 0,4,soundA,soundB,soundC,beeper_buffer);
	Mix(sound_bufferD, 44100, 1,4,soundA,soundB,soundC,beeper_buffer);
	}

	if (stereo_mode==1)
	{
	Mix(sound_bufferD, 44100, 0,3,soundA,soundB,beeper_buffer);
	Mix(sound_bufferD, 44100, 1,3,soundC,soundB,beeper_buffer);
	}

	if (stereo_mode==0)
	{
	Mix(sound_bufferD, 44100, 0,3,soundA,soundC,beeper_buffer);
	Mix(sound_bufferD, 44100, 1,3,soundB,soundC,beeper_buffer);
	}

	TODO ("io_write(sound_fil, sound_bufferD, SOUND_TICKS_PER_FRAME*4);");
	}

	if (stereo_mode==2)
	{
	Mix(sound_buffer, 44100, 0,4,soundA,soundB,soundC,beeper_buffer);
	Mix(sound_buffer, 44100, 1,4,soundA,soundB,soundC,beeper_buffer);
	}

	if (stereo_mode==1)
	{
	Mix(sound_buffer, 44100, 0,3,soundA,soundB,beeper_buffer);
	Mix(sound_buffer, 44100, 1,3,soundC,soundB,beeper_buffer);
	}

	if (stereo_mode==0)
	{
	Mix(sound_buffer, 44100, 0,3,soundA,soundC,beeper_buffer);
	Mix(sound_buffer, 44100, 1,3,soundB,soundC,beeper_buffer);
	}

	*/

	switch(stereoAY) {
		case AY_STEREO_NONE:
			mix(sndBuf, freq, 0, 3, bufA, bufC, beeperBuffer);
			mix(sndBuf, freq, 1, 3, bufB, bufC, beeperBuffer);
			break;
		case AY_STEREO_ABC:
			mix(sndBuf, freq, 0, 4, bufA, bufB, bufC, beeperBuffer);
			mix(sndBuf, freq, 1, 4, bufA, bufB, bufC, beeperBuffer);
			break;
		case AY_STEREO_ACB:
			mix(sndBuf, freq, 0, 3, bufA, bufB, beeperBuffer);
			mix(sndBuf, freq, 1, 3, bufC, bufB, beeperBuffer);
			break;
	}
	PSGcounter = 0;
	theApp->bus.tCounter = 0;
}

void zxSND::mix(ssh_w* buf, int num, int offs, int count, ...) {
	va_list ap;

	va_start(ap, count);
	for(int i = 0; i < count; i++) mixing_ch[i] = va_arg(ap, ssh_w*);
	for(int i = 0; i < SND_TICKS_PER_FRAME[num]; i++) {
		buf[i * 2 + offs] = 0;
		for(int j = 0; j < count; j++) buf[i * 2 + offs] += mixing_ch[j][i << num] / count;
	}
}

void zxSND::AY_CHANNEL::gen(int i, zxSND* snd) {
	if(genCounter >= genPeriod) genCounter = 0;
	bool tone = genCounter < (genPeriod / 2);
	genCounter++;
	if(noiseCounter >= snd->periodN) { noiseCounter = 0; rnd = rand(); }
	bool noise = rnd & 1;
	noiseCounter++;
	channelData[i] = (((tone | isChannel) & (noise | isNoise)) | !genPeriod);
	channelData[i] = (channelData[i]) ? (snd->origRegsAY[AY_AVOL] & 0x10) ? volTable[snd->envVolume] : volTable[snd->origRegsAY[AY_AVOL]] : 0;
}

void zxSND::stop() {
	PSGcounter = 0;
	origRegsAY[AY_AVOL] = origRegsAY[AY_BVOL] = origRegsAY[AY_CVOL] = 0;
}