#pragma once

class zxFile {
public:
	zxFile() : hf(-1) { }
	zxFile(int handle) : hf(handle) { if(getPos() == -1L) hf = -1; }
	zxFile(ssh_cws path, bool read, bool append = false) : hf(-1) { open(path, read, append); }
	virtual ~zxFile() { close(); }
	bool open(ssh_cws path, bool read, bool append = false) {
		int access = (read ? _SH_DENYRD : _SH_DENYWR);
		int mode = (read ? _S_IREAD : _S_IWRITE);
		int flags = (read ? _O_RDONLY : _O_WRONLY);
		if(!read) {
			if(!append) flags |= _O_CREAT | _O_TRUNC; else flags |= _O_APPEND;
		}
		return (_wsopen_s(&hf, path, flags | _O_BINARY, access, mode) == 0);
	}
	void close() { if(hf != -1) { _close(hf); hf = -1; } }
	bool read(void* ptr, long size) { return (hf != -1 ? (_read(hf, ptr, size) == size) : false); }
	bool write(void* ptr, long size) { return (hf != -1 ? (_write(hf, ptr, size) == size) : false); }
	bool setSize(long size) { return (hf != -1 ? (_chsize(hf, size) == 0) : false); }
	bool setPos(long pos, int mode) { return (hf != -1 ? _lseek(hf, pos, mode) == pos : false); }
	auto length() { return (hf != -1 ? _filelength(hf) : 0); }
	long getPos() { return (hf != -1 ? _tell(hf) : -1L); }
	bool remove(ssh_cws path) { return _wunlink(path) == 0; }
protected:
	int hf;
};
