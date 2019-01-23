
/*
*	�����:		������� �. �.
*	�������:	�����������, 19 ���� 2015, 1:26
*	�����������:10 ������ 2016
*	��������:	������ ��� �������� �����/������ � ��������
*/

#pragma once

#include "ssh_map.h"

#define IS_KEY(vk)			keyboard->is_keyPressed(vk)
#define GET_KEY				keyboard->getKey()
#define GET_SYM				keyboard->getSym()

#define IS_MCAPT(ptr)		mouse->is_capture(ptr)
#define IS_KCAPT(ptr)		keyboard->is_capture(ptr)


namespace ssh
{
	class SSH Mouse final
	{
	public:
		enum Flags
		{
			msDclickR = 0,
			msDclickL = 1,
			msDclickM = 2,
			msVisible = 3
		};
		// ���������, ����������� ��������� ����
		struct MOUSE
		{
			MOUSE(const Pts<int>& hot, const Bar<int>& bar) : hot(hot), bar(bar) {}
			// ������� �����
			Pts<int> hot;
			// �������
			Bar<int> bar;
		};
		// ������ � �������
		static Mouse* instance() { static Mouse mouse; return &mouse; }
		// ���������� �������
		void set_pos(const Pts<int>& pt) { ::SetCursorPos(pt.x, pt.y); position = pt; }
		// ��������
		void add(const String& name, const Pts<int>& hot, const Bar<int>& bar) { mouses[name] = new MOUSE(hot, bar); }
		// �������
		void remove(const String& name) { if(current == mouses[name]) current = nullptr; mouses.remove(name); }
		// ������� �������
		void set(const String& name) { current = mouses[name]; }
		// ������� �������
		MOUSE* get_current() const { return current; }
		// ������� ������� ���������
		bool is_visible() const { return status.test_bit(msVisible); }
		// ��������� �������� ���������
		void setVisible(bool use) { status.set_bit(msVisible, use); }
		// ��������� ������� ������
		void setWheelStatus(long w) { wheel = w; }
		// ��������� ������� �������� �����
		void setDoubleClickStatus(bool dclickM, bool dclickR, bool dclickL) { status.set_bit(msDclickR, dclickR); status.set_bit(msDclickL, dclickL); status.set_bit(msDclickM, dclickM); }
		// ��������� ������� ����������
		void update(const Pts<int>& pt, long k) { old_position = position; position = pt; key = k; if(!(k & (MK_LBUTTON | MK_MBUTTON))) hcapture = 0; }
		// ������� ������� ������� ������ Ctrl
		bool is_keyControl() const { return ((key & MK_CONTROL) != 0); }
		// ������� ������� ������� ������ Shift
		bool is_keyShift() const { return ((key & MK_SHIFT) != 0); }
		// ������� ������� ������� ����� ������
		bool is_keyLeft(ssh_u h) const { return ((hcapture == 0 ? 1 : h == hcapture) && ((key & MK_LBUTTON) != 0)); }
		// ������� ������� ������� ������ ������
		bool is_keyRight() const { return ((key & MK_RBUTTON) != 0); }
		// ������� ������� ������� ������� ������
		bool is_keyMiddle(ssh_u h) const { return ((hcapture == 0 ? 1 : h == hcapture) && ((key & MK_MBUTTON) != 0)); }
		// ������� ������� ������� �������� �����
		bool is_doubleClickRight() const { return status.test_bit(msDclickR); }
		// ������� ������� ������ �������� �����
		bool is_doubleClickLeft() const { return status.test_bit(msDclickL); }
		// ������� ������� �������� �������� �����
		bool is_doubleClickMiddle() const { return status.test_bit(msDclickM); }
		// ���������� ������
		void set_capture(ssh_u h) { hcapture = h; }
		// ��������� �� ������
		bool is_capture(ssh_u h) const { return (h == hcapture); }
		// ������� ��������� ������
		long get_wheelState() { long tmp(wheel); wheel = 0; return tmp; }
		// ������� ��������� ������
		long get_keyState() const { return key; }
		// ������� �������� �������
		const Pts<int>& get_pos() const { return position; }
		// ������� ������� ����� ����� � ������ ��������
		auto get_sub_pos() const { return (position - old_position); }
		// ������� ������ �������
		auto get_old_pos() const { return old_position; }
		// ������� �������
		auto get_hot_pos() const { return Pts<int>(current ? position + current->hot : position); }
#ifdef _DEBUG
		// ����
		static void unit_test();
#endif
	protected:
		// �������
		Pts<int> position;
		// ������ �������
		Pts<int> old_position;
		// �������� ������
		long wheel = 0;
		// ������ ������
		long key = 0;
		// ������
		Bits status;
		// ������� ����
		MOUSE* current = nullptr;
		// ��� ����
		Map<MOUSE*, String> mouses;
		// ����� �������
		ssh_u hcapture = 0;
	};

	class SSH Keyboard final
	{
	public:
		// ��������� ������� ������
		void set_key(ssh_b code, ssh_l repeat, bool use)
		{
			keys[code] = use;
			cur_key = (use ? code : 0);
			cur_char = 0;
			rep = (use ? repeat : 0);
		}
		// ������ � �������
		static Keyboard* instance() { static Keyboard keyb; return &keyb; }
		// ��������� ������� ������
		void set_sym(ssh_b code_char, ssh_u repeat) { cur_char = code_char; }
		// ������� ������� ���������� ������
		ssh_b get_sym() const { return cur_char; }
		// ������� ������� ������
		ssh_b get_key() { rep--; return (rep >= 0 ? cur_key : 0); }
		// ������� ������� �������
		bool is_keyPressed(ssh_b code) const { return keys[code]; }
		// ���������� ������
		void set_capture(ssh_u h) { hcapture = h; }
		// ��������� �� ������
		bool is_capture(ssh_u h) const { return (h == hcapture || hcapture == 0); }
#ifdef _DEBUG
		// ����
		static void unit_test();
#endif
	protected:
		// �����������
		Keyboard() { ssh_memzero(keys, sizeof(keys)); }
		// ������ �������� ���� ������
		bool keys[256];
		// ������� ������
		ssh_b cur_char = 0;
		// ������� ������
		ssh_b cur_key = 0;
		// ������ �������
		ssh_l rep = 0;
		// ����� �������
		ssh_u hcapture = 0;
	};

	class SSH Gamepad final
	{
	public:
		enum { MAX_CONTROLLERS	= 4 };
		enum Side : int
		{
			left	= 1,
			right	= 2
		};
		enum Buttons
		{
			cross_up	= XINPUT_GAMEPAD_DPAD_UP,
			cross_down	= XINPUT_GAMEPAD_DPAD_DOWN,
			cross_left	= XINPUT_GAMEPAD_DPAD_LEFT,
			cross_right = XINPUT_GAMEPAD_DPAD_RIGHT,
			left_turn	= XINPUT_GAMEPAD_LEFT_SHOULDER,
			right_turn	= XINPUT_GAMEPAD_RIGHT_SHOULDER,
			start		= XINPUT_GAMEPAD_START,
			escape		= XINPUT_GAMEPAD_BACK,
			a			= XINPUT_GAMEPAD_A,
			b			= XINPUT_GAMEPAD_B,
			x			= XINPUT_GAMEPAD_X,
			y			= XINPUT_GAMEPAD_Y,
			left_stick	= XINPUT_GAMEPAD_LEFT_THUMB,
			right_stick = XINPUT_GAMEPAD_RIGHT_THUMB
		};
		struct GAMEPAD
		{
			GAMEPAD() { ssh_memzero(this, sizeof(GAMEPAD)); }
			// ������
			ssh_w wButtons;
			ssh_b bLeftTrigger;
			ssh_b bRightTrigger;
			short sThumbLX;
			short sThumbLY;
			short sThumbRX;
			short sThumbRY;
			// ����������� ����������
			XINPUT_CAPABILITIES caps;
			// ��������
			bool is_connected;
			bool is_inserted;
			bool is_removed;
			// �������� ����� � ��������� [-1,+1]
			float fThumbRX;
			float fThumbRY;
			float fThumbLX;
			float fThumbLY;
			// ������� ������
			ssh_w wPressedButtons;
			bool is_pressedLeftTrigger;
			bool is_pressedRightTrigger;
			// ���������� ��������� ������
			ssh_w wLastButtons;
			bool is_lastLeftTrigger;
			bool is_lastRightTrigger;
		};
		// ������ � �������
		static Gamepad* instance() { static Gamepad pad; return &pad; }
		// �������� �� ����� ������� ������
		bool is_once_pressed(ssh_d idx, Buttons but) const { return (_pad[idx].is_connected ? ((_pad[idx].wPressedButtons & but) != 0) : false); }
		// ������� ������� ������� ������
		bool is_pressed(ssh_d idx, Buttons but) const { return (_pad[idx].is_connected ? ((_pad[idx].wButtons & but) != 0) : false); }
		// �������� �� ������� ������
		bool is_released(ssh_d idx, Buttons but) const { return (_pad[idx].is_connected ? ((_pad[idx].wLastButtons & but) != 0) : false); }
		// �������� �� ������� �������
		bool is_trigger(ssh_d idx, Side side) const;
		// ������� ��������
		bool is_connect(ssh_d idx) const { return _pad[idx].is_connected; }
		// ������� �����������
		bool is_removed(ssh_d idx) const { return _pad[idx].is_removed; }
		// ������� ������ �����������
		bool is_inserted(ssh_d idx) const { return _pad[idx].is_inserted; }
		// ������� �������� ������
		Range<float> get_stick(ssh_d idx, Side side) const;
		// ���������� ��������� � �������� �� �������
		void update();
		// ��������� �������� ��������
		void vibration(ssh_d idx, Side side, ssh_w speed) const;
#ifdef _DEBUG
		// ����
		static void unit_test();
#endif
	protected:
		// �����������
		Gamepad::Gamepad() { if(ssh_xin_enable) ssh_xin_enable(true); }
		// ����������
		virtual Gamepad::~Gamepad() { if(ssh_xin_enable) ssh_xin_enable(false); }
		// ������ ������������
		GAMEPAD _pad[MAX_CONTROLLERS];
	};
}
