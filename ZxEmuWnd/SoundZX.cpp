
#include "stdafx.h"
#include "SoundZX.h"
#include "CpuZX.h"

extern HWND hWnd;
DWORD tm1 = 0;
int index = 0;
int cur_index = 0;
ssh_b buffer[65536];

void SoundZX::execute(DWORD tm) {
	if((CpuZX::STATE & CpuZX::SOUND) == 0) return;
	CpuZX::STATE &= ~CpuZX::SOUND;
	if((CpuZX::portFE & 16) == 16) {
		ssh_w bt = (ssh_w)(tm - tm1) >> 4;
		buffer[index] = bt;
		index++;
		index &= 32767;
		tm1 = tm;
		if(sndStk.lp) {
			ssh_d status;
			sndStk.lp->GetStatus(&status);
			if((status & DSBSTATUS_PLAYING) != DSBSTATUS_PLAYING) {
				bt = buffer[cur_index];
				cur_index++;
				cur_index &= 32767;
				byte* adr = (byte*)sndStk.address;
				byte* adrE = adr + sndStk.nNewSize;
				for(int i = 0; i < bt; i++) *adr++ = 140;
				for(int i = 0; i < bt; i++) *adr++ = 125;
				memset(adr, 128, adrE - adr);
				if(SUCCEEDED(sndStk.lp->SetCurrentPosition(0)))
					sndStk.lp->Play(0, 0, 0);
			}
		}
	}
}

bool SoundZX::init(int freq) {

	DSBUFFERDESC dsbd;
	memset(buffer, 0, sizeof(buffer));

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
		dsbd.dwBufferBytes = 512;
		dsbd.dwReserved = 0;
		dsbd.lpwfxFormat = &fmtWave;

		if(FAILED(dSnd->CreateSoundBuffer(&dsbd, &sndStk.lp, nullptr))) throw(0);
		if(FAILED(sndStk.lp->Lock(0, 512, &sndStk.address, (DWORD*)&sndStk.nNewSize, nullptr, nullptr, DSBLOCK_ENTIREBUFFER))) throw(0);
		memset(sndStk.address, 128, sndStk.nNewSize);
		if(FAILED(sndStk.lp->Unlock(sndStk.address, 512, nullptr, 0))) throw(0);
	} catch(...) {
		uninit();
		return false;
	}
	return true;
}


void SoundZX::uninit() {

}
