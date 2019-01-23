
#include "stdafx.h"
#include "ssh_io.h"

namespace ssh
{
	void Gamepad::update()
	{
		for(ssh_d idx = 0; idx < MAX_CONTROLLERS; idx++)
		{
			XINPUT_STATE state;
			GAMEPAD* pad(&_pad[idx]);

			bool bWasConnected(pad->is_connected);
			HRESULT hr(ssh_xin_gstate ? ssh_xin_gstate(idx, &state) : ERROR_INVALID_FUNCTION);
			pad->is_connected = (hr == ERROR_SUCCESS);
			pad->is_removed = (bWasConnected && !pad->is_connected);
			pad->is_inserted = (!bWasConnected && pad->is_connected);
			if(!pad->is_connected) continue;
			if(pad->is_inserted)
			{
				ssh_memzero(pad, sizeof(GAMEPAD));
				pad->is_connected = true;
				pad->is_inserted = true;
				if(ssh_xin_caps) ssh_xin_caps(idx, XINPUT_DEVTYPE_GAMEPAD, &pad->caps);
			}
			//memcpy(pad, &state.Gamepad, sizeof(XINPUT_GAMEPAD));
			// проверка на deadzone
			short lx(state.Gamepad.sThumbLX);
			short ly(state.Gamepad.sThumbLY);
			if((lx < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && lx > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) && (ly < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && ly > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) lx = ly = 0;
			pad->sThumbLX = lx; pad->fThumbLX = lx / 32767.0f;
			pad->sThumbLY = ly; pad->fThumbLY = ly / 32767.0f;
			short rx(state.Gamepad.sThumbRX);
			short ry(state.Gamepad.sThumbRY);
			if((pad->sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pad->sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) && (pad->sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pad->sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) rx = ry = 0;
			pad->sThumbRX = rx; pad->fThumbRX = rx / 32767.0f;
			pad->sThumbRY = ry; pad->fThumbRY = ry / 32767.0f;
			// остальные
			pad->wButtons = state.Gamepad.wButtons;
			pad->bLeftTrigger = state.Gamepad.bLeftTrigger;
			pad->bRightTrigger = state.Gamepad.bRightTrigger;
			// какие кнопки были нажаты
			pad->wPressedButtons = (pad->wLastButtons ^ pad->wButtons) & pad->wButtons;
			pad->wLastButtons = pad->wButtons;
			// признак левый триггер был нажат или отпущен
			bool bPressed = (pad->bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			pad->is_pressedLeftTrigger = (bPressed) ? !pad->is_lastLeftTrigger : false;
			pad->is_lastLeftTrigger = bPressed;
			// признак правый триггер был нажат или отпущен
			bPressed = (pad->bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			pad->is_pressedRightTrigger = (bPressed) ? !pad->is_lastRightTrigger : false;
			pad->is_lastRightTrigger = bPressed;
		}
	}

	void Gamepad::vibration(ssh_d idx, Side side, ssh_w speed) const
	{
		if(_pad[idx].is_connected && ssh_xin_sstate)
		{
			XINPUT_VIBRATION vibration;
			vibration.wLeftMotorSpeed = (side & left ? speed : 0);
			vibration.wRightMotorSpeed = (side & right ? speed : 0);;
			ssh_xin_sstate(idx, &vibration);
		}
	}

	Range<float> Gamepad::get_stick(ssh_d idx, Side side) const
	{
		if(_pad[idx].is_connected)
		{
			if((side == left)) return Range<float>(-_pad[idx].fThumbLX, -_pad[idx].fThumbLY);
			if((side == right)) return Range<float>(-_pad[idx].fThumbRX, _pad[idx].fThumbRY);
		}
		return Range<float>();
	}

	bool Gamepad::is_trigger(ssh_d idx, Side side) const
	{
		if(_pad[idx].is_connected)
		{
			if((side == left)) return _pad[idx].is_pressedLeftTrigger;
			if((side == right)) return _pad[idx].is_pressedRightTrigger;
		}
		return false;
	}
}

