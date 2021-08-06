#pragma once
#ifndef INITGUID
#define INITGUID
#endif
#include <dinput.h>
#include <objbase.h>
#include "La_Windows.h"
namespace INPUT_
{
	bool InitializeInput(HWND hwnd, HINSTANCE hInstance);

	typedef UCHAR DIKEYSTATE[256];  //键盘的信息就是256个

	
	class KEYBOARD
	{
	private:
		LPDIRECTINPUTDEVICE8  lpdikey; //发现是抽象类指针
	public:
		DIKEYSTATE keyboard_state;
	public:
		KEYBOARD() :keyboard_state{ 0 }, lpdikey(nullptr){};
		bool create();
		void read()
		{
			if (lpdikey)
			{
				//获取信息
				lpdikey->GetDeviceState(sizeof(DIKEYSTATE), (LPVOID)keyboard_state);
			}
			else//keyboard isn't plugged in, zero out state  //因为可能保存了上次残留的信息
			{
				empty();
			}
		}
		bool operator[](int dik_code) { return keyboard_state[dik_code] & 0x80; }
		void empty()
		{
			memset(keyboard_state, 0, sizeof(DIKEYSTATE));
		}
		void clear()
		{
			if (lpdikey)
			{
				//归还设备
				lpdikey->Unacquire();
				//释放对象
				lpdikey->Release();
				lpdikey = nullptr;
			}
		}
		~KEYBOARD()
		{
			clear();
		}
	};

	class MOUSE
	{
	public:
		enum BUTTON
		{
			LEFT = 0,
			RIGHT = 1,
			MIDDLE = 2,
		};
	private:
		LPDIRECTINPUTDEVICE8  lpdimouse = nullptr;
	public:
		DIMOUSESTATE mouse_state;
	public:
		MOUSE() :lpdimouse(nullptr), mouse_state{ 0 }{};
		bool create();
		LONG getX(void) { return mouse_state.lX; }
		LONG getY(void) { return mouse_state.lY; }
		LONG getZ(void) { return mouse_state.lZ; }
		BYTE* getButton(void) { return mouse_state.rgbButtons; }
		bool getLButton(void) { return mouse_state.rgbButtons[LEFT] & 0x80; }
		bool getRButton(void) { return mouse_state.rgbButtons[RIGHT] & 0x80; }
		bool getMButton(void) { return mouse_state.rgbButtons[MIDDLE] & 0x80; }
		void read()
		{
			if (lpdimouse)
			{
				//获取信息
				lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state);
			}
			else//mouse isn't plugged in, zero out state
			{
				empty();
			}
		}
		void clear()
		{
			if (lpdimouse)
			{
				lpdimouse->Unacquire();
				lpdimouse->Release();
				lpdimouse = nullptr;
			}
		}
		void empty()
		{
			memset(&mouse_state, 0, sizeof(DIMOUSESTATE));
		}
		~MOUSE()
		{
			clear();
		}
	};

	class JOYSTICK
	{
	private:
		static BOOL CALLBACK EnumJoysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID guid_ptr);
	private:
		LPDIRECTINPUTDEVICE8 lpdijoy;
		GUID joystickGUID;
		bool bfound;
		TSTRING joyname;
	public:
		//数据格式
		DIJOYSTATE joy_state;
	public:
		JOYSTICK() :lpdijoy(nullptr), joystickGUID{ 0 }, bfound(false), joy_state{ 0 }{};
		bool create(int min_x = -256, int max_x = 256, int min_y = -256, int max_y = 256, int dead_zone = 10);
		const TCHAR* getName()const { return joyname; }
		void read()
		{
			if (!bfound)
			{
				return;
			}

			if (lpdijoy)
			{
				//this is needed for joysticks only
				//轮询操作
				lpdijoy->Poll();
				lpdijoy->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&joy_state);
			}
			else
			{
				empty();
			}
		}
		void clear()
		{
			if (lpdijoy)
			{
				lpdijoy->Unacquire();
				lpdijoy->Release();
				lpdijoy = nullptr;
			}
		}
		void empty()
		{
			memset(&joy_state, 0, sizeof(DIJOYSTATE));
		}
		~JOYSTICK()
		{
			clear();
		}
	};

	void CloseInput();
}