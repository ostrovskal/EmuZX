
#include "stdafx.h"
#include "SoundZX.h"
#include "CpuZX.h"

extern HWND hWnd;

void SoundZX::execute() {
	if((CpuZX::STATE & CpuZX::SOUND) == 0) return;
	CpuZX::STATE &= ~CpuZX::SOUND;
	if(sndStk.lp) memset(sndStk.address, (CpuZX::sound & 16) ? (CpuZX::sound & 15) << 1 : 128, sndStk.nNewSize);
}

bool SoundZX::init(int freq) {

	DSBUFFERDESC dsbd;

	try {
		if(FAILED(DirectSoundCreate(NULL, &dSnd, NULL))) throw(0);
		if(FAILED(dSnd->SetCooperativeLevel(hWnd, DSSCL_NORMAL))) throw(0);

		fmtWave.cbSize = sizeof(WAVEFORMATEX);
		fmtWave.wFormatTag = 1;
		fmtWave.nChannels = 1;
		fmtWave.nSamplesPerSec = freq;
		fmtWave.nAvgBytesPerSec = freq;
		fmtWave.nBlockAlign = 1;
		fmtWave.wBitsPerSample = 8;

		memset(&dsbd, 0, sizeof(DSBUFFERDESC));
		dsbd.dwSize = sizeof(DSBUFFERDESC);
		dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;

		if(FAILED(dSnd->CreateSoundBuffer(&dsbd, &sndBuf0, NULL))) throw(0);
		dsbd.dwFlags = DSBCAPS_GLOBALFOCUS;
		dsbd.dwBufferBytes = 32;
		dsbd.dwReserved = 0;
		dsbd.lpwfxFormat = &fmtWave;

		if(FAILED(dSnd->CreateSoundBuffer(&dsbd, &sndStk.lp, nullptr))) throw(0);
		if(FAILED(sndStk.lp->Lock(0, 32, &sndStk.address, &sndStk.nNewSize, nullptr, nullptr, 2))) throw(0);
		memset(sndStk.address, 128, sndStk.nNewSize);
		if(FAILED(sndStk.lp->Unlock(sndStk.address, 32, nullptr, 0))) throw(0);
		if(FAILED(sndStk.lp->SetCurrentPosition(0))) throw(0);
		sndStk.lp->Play(0, 0, DSBPLAY_LOOPING);
	} catch(...) {
		uninit();
		return false;
	}
	return true;
}


void SoundZX::uninit() {

}
