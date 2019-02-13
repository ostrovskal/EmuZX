
#include "stdafx.h"
#include "zxSND.h"
#include "zxCPU.h"

int play_index = 0;
int cur_index = 0;

int cur_signal = -1;
ssh_u old_tm = 0;

ssh_w buffer[16384];

void zxSND::execute(ssh_u new_tm) {
	ssh_u tm = (new_tm - old_tm);
//	if(((*_TSTATE) & ZX_SOUND) == 0) {
//		if(tm > 32768) {
//			cur_signal = -1;
//			old_tm = new_tm;
//		}
//		return;
//	}
//	modifyTSTATE(0, ZX_SOUND);
	return;

	ssh_b signal = ((*_PORT_FE) & 16) != 0;
	if(cur_signal == -1) {
		cur_signal = signal;
		old_tm = new_tm;
		return;
	}
	if(cur_signal != signal) {
		cur_signal = signal;
		buffer[cur_index++] = (ssh_w)tm;
		cur_index &= 16383;
		old_tm = new_tm;
	}
	if(sndStk.lp) {
		int index = cur_index - play_index;
		if(index > 0) {
			ssh_d status;
			sndStk.lp->GetStatus(&status);
			if((status & DSBSTATUS_PLAYING) != DSBSTATUS_PLAYING) {
				ssh_w bt = buffer[play_index++];
				play_index &= 16383;
				if(bt > 255) bt = 256;
				else if(bt == 0) bt = 3;
				byte* adr = (byte*)sndStk.address;
				byte* adrE = adr + sndStk.nNewSize;
				int count = bt * 2 / sndStk.nNewSize;
				for(int ii = 0; ii < count; ii++) {
					if(adr >= adrE) break;
					for(int i = 0; i < bt; i++) {
						if(adr >= adrE) break;
						*adr++ = 150;
					}
					for(int i = 0; i < bt; i++) {
						if(adr >= adrE) break;
						*adr++ = 125;
					}
				}
				memset(adr, 128, adrE - adr);
				if(SUCCEEDED(sndStk.lp->SetCurrentPosition(0)))
					sndStk.lp->Play(0, 0, 0);
			}
		}
	}
}

bool zxSND::init(int freq) {
	return false;
	DSBUFFERDESC dsbd;
	memset(buffer, 0, sizeof(buffer));

	try {
		if(FAILED(DirectSoundCreate(NULL, &dSnd, NULL))) throw(0);
		if(FAILED(dSnd->SetCooperativeLevel(theApp->getHWND(), DSSCL_NORMAL))) throw(0);

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


void zxSND::uninit() {

}
