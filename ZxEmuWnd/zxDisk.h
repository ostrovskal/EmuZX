#pragma once

#include <sys\timeb.h>

#define DISK_TRD	0
#define DISK_FDI	1

class zxDisk {
	friend class zxBus;
public:
	struct DISK {
		// им€
		ssh_ws name[255];
		// тип (0 - trd/1 - fdi)
		ssh_b tp;
		// текущий трек
		ssh_b trk;
		// текуща€ сторона
		ssh_b ro;
		// дескриптор
		int fd;
	};

	struct REC_SET {
		unsigned b0 : 1;
		unsigned b1 : 1;
		unsigned b2 : 1;
		unsigned b3 : 1;
		unsigned b4 : 1;
		unsigned b5 : 1;
		unsigned b6 : 1;
		unsigned b7 : 1;
	};

	union REC {
		ssh_b byte;
		REC_SET bit;
	};

	zxDisk();
	ssh_b readPort(ssh_b A0A7);
	void writePort(ssh_b A0A7, ssh_b val);
	bool selectTRD(ssh_cws path, int slot);
	int curNumDisk() const { return out & 3; }
	DISK& curDisk() { return disks[out & 3]; }
protected:
	bool isNone() { return _wcsnicmp(curDisk().name, L"none", 4) == 0; }

	ssh_u curTime() {
		ftime(&tmb);
		return(tmb.time * 1000 + tmb.millitm);
	}
	ssh_b readDataTRD();
	void writeData(ssh_b data);
	void seekDelay(ssh_b dst_track);
	void setFlagsSeeks();
	void writePortTRD(ssh_b val);

	DISK disks[4];
	REC rec;

	struct timeb tmb;
	ssh_u done;
	ssh_b spin;
	ssh_b dir;
	ssh_b track;
	ssh_b sector;
	ssh_b data;
	ssh_b in, out;
	ssh_b buffer[256 * 16];
	ssh_b* read;
	ssh_i rlen, rpos;
	ssh_b six[6];
	ssh_i wlen;
	ssh_i wpos;
};
