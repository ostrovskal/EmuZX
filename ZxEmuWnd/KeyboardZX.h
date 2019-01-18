#pragma once

class KeyboardZX {
public:
	KeyboardZX() : spec(-1), prevKey(-1) {}
	virtual ~KeyboardZX() {}

	void execute(int key, bool pressed);
protected:
	void setKey(int num, bool pressed);
	void remapping(byte* map, int key, bool pressed);
	byte spec, prevKey;
};
