#pragma once

#include <mmsystem.h>
#include "dsound.h"

#pragma comment(lib,"dsound.lib")

class zxSND {
public:
	struct SOUNDBUFFER {
		LPDIRECTSOUNDBUFFER		lp;
		void*					address;
		long					nNewSize;
		long					nSize;
	};

	zxSND() : dSnd(nullptr), sndBuf0(nullptr) { sndStk.lp = nullptr; init(44100); }
	virtual ~zxSND() { uninit();  }
	void execute(ssh_u tm);
protected:
	LPDIRECTSOUND dSnd;
	LPDIRECTSOUNDBUFFER	sndBuf0;
	SOUNDBUFFER sndStk;
	WAVEFORMATEX fmtWave;

	bool init(int freq);
	void uninit();
};
