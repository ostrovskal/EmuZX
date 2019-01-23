#pragma once

class KeyboardZX {
public:
	KeyboardZX() : pnum(-1), pkey(255), pkey1(255), pkey2(255) {}
	virtual ~KeyboardZX() {}

	void execute(ssh_b key, bool pressed);
protected:
	void writePort(ssh_b num, bool pressed);
	bool remapping(ssh_b* map, ssh_b key, bool pressed);
	//ssh_b spec, prevKey;
	ssh_b pkey, pkey1, pkey2;
	int pnum;
};
