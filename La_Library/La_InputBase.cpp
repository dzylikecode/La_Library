
#include "La_InputBase.h"
#include "La_WindowsBase.h"
#include "La_Log.h"
#pragma comment(lib, "dinput8.lib")
namespace
{
	LPDIRECTINPUT8  lpdi8 = nullptr;
	HWND globalHwnd;
	
	INPUT_::JOYSTICK* curCallBack = nullptr;
}


namespace INPUT_
{
	bool InitializeInput(HWND hwnd, HINSTANCE hInstance)
	{
		HRESULT hre = 0;
		if (FAILED(hre = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpdi8, nullptr)))
		{
			ERROR_MSG(ERR, TEXT("无法初始化 INPUT error code： %d"), hre);
			return false;
		}

		globalHwnd = hwnd;

		return true;
	}
	
	void CloseInput()
	{
		if (lpdi8)
		{
			lpdi8->Release();
			lpdi8 = nullptr;
		}
	}

	


	bool KEYBOARD::create()
	{
		//获得键盘接口
		if (lpdi8->CreateDevice(GUID_SysKeyboard, &lpdikey, nullptr) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化键盘失败"));
			return false;
		}

		//设置协作等级
		//不独占，后台工作->当它没被激活，最小化的时候也会受到信息
		//FOREGROUND->是要求运行在前台时才能访问，后台的时候自动取消
		if (lpdikey->SetCooperativeLevel(globalHwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		{
			//如果失败，一般是程序在独占键盘，导致键盘申请失败
			ERROR_MSG(ERR, TEXT("初始化键盘失败"));
			return false;
		}

		//设置数据格式
		//可以根据要求来设计自己数据包的数据结构
		//现在键盘采取预定义的结构就可以了
		if (lpdikey->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化键盘失败"));
			return false;
		}

		//获得设备
		if (lpdikey->Acquire() != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化键盘失败"));
			return false;
		}
		return true;
	}

	bool MOUSE::create()
	{
		if (lpdi8->CreateDevice(GUID_SysMouse, &lpdimouse, nullptr) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化鼠标失败"));
			return false;
		}

		//设置协作等级
		//不独占，后台工作->当它没被激活，最小化的时候也会受到信息
		//FOREGROUND->是要求运行在前台时才能访问，后台的时候自动取消
		if (lpdimouse->SetCooperativeLevel(globalHwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化鼠标失败"));
			return false;
		}

		//现在鼠标采取预定义的结构就可以了
		if (lpdimouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化鼠标失败"));
			return false;
		}

		//获得设备
		if (lpdimouse->Acquire() != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化鼠标失败"));
			return false;
		}
		return true;
	}

	/// <summary>
	/// 枚举游戏杆，但是只枚举第一个
	/// </summary>
	/// <returns>
	/// 返回值是微软定义的 BOOL
	/// </returns>
	BOOL CALLBACK JOYSTICK::EnumJoysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID guid_ptr)
	{
		//传入的是 guid
		//也可以传入 guid 列表，
		//用一个静态数 static int  表明第几个
		//存到 guid 列表当中
		//还可以分类
		*(GUID*)guid_ptr = lpddi->guidInstance; //lpddi 里面储存了信息
		curCallBack->joyname = lpddi->tszProductName;
		//停止
		return DIENUM_STOP; //可以继续，但是只要第一个就够了
	}

	bool JOYSTICK::create(int min_x/* =-256 */, int max_x/* =256 */, int min_y/* =-256 */, int max_y/* =256 */, int dead_zone /* = 10 */)
	{
	// 参数 1  设备类型
	// 参数 2  回调函数来接受扫描得到的数据
	// 参数 3  得到 GUID
	// 参数 4  扫描的是只有已经连接好的
		curCallBack = this;
		lpdi8->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticks, &joystickGUID, DIEDFL_ATTACHEDONLY);

		//create the version 1.0 interface
		LPDIRECTINPUTDEVICE lpdijoy_temp = nullptr;

		if (lpdi8->CreateDevice(joystickGUID, &lpdijoy, nullptr) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化游戏杆失败"));
			return false;
		}

		if (lpdijoy->SetCooperativeLevel(globalHwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化游戏杆失败"));
			return false;
		}

		if (lpdijoy->SetDataFormat(&c_dfDIJoystick) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化游戏杆失败"));
			return false;
		}

		//给游戏杆设置范围
		DIPROPRANGE joy_axis_range;

		// x axis
		joy_axis_range.lMin = min_x;
		joy_axis_range.lMax = max_x;

		joy_axis_range.diph.dwSize = sizeof(DIPROPRANGE);
		joy_axis_range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		joy_axis_range.diph.dwObj = DIJOFS_X;
		joy_axis_range.diph.dwHow = DIPH_BYOFFSET;

		lpdijoy->SetProperty(DIPROP_RANGE, &joy_axis_range.diph);

		// y axis
		joy_axis_range.lMin = min_y;
		joy_axis_range.lMax = max_y;

		joy_axis_range.diph.dwSize = sizeof(DIPROPRANGE);
		joy_axis_range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		joy_axis_range.diph.dwObj = DIJOFS_Y;
		joy_axis_range.diph.dwHow = DIPH_BYOFFSET;

		lpdijoy->SetProperty(DIPROP_RANGE, &joy_axis_range.diph);

		//设置死区 -> 用来靠近中心位置的时候不发送任何数据
		//设置的方法是用比例 例如 x 的 范围 是 -1024 - 1024
		//要想 -102 102 区间为死区，则计算得 比例为 10%
		//那么 10000 * 10% 得到的就是 1000

		dead_zone *= 100;//比例转化

		DIPROPDWORD dead_band;

		dead_band.diph.dwSize = sizeof(DIPROPDWORD);
		dead_band.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dead_band.diph.dwObj = DIJOFS_X;
		dead_band.diph.dwHow = DIPH_BYOFFSET;

		dead_band.dwData = dead_zone;

		lpdijoy->SetProperty(DIPROP_DEADZONE, &dead_band.diph);

		dead_band.diph.dwSize = sizeof(DIPROPDWORD);
		dead_band.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dead_band.diph.dwObj = DIJOFS_Y;
		dead_band.diph.dwHow = DIPH_BYOFFSET;

		//正负区间都会设置死区
		dead_band.dwData = dead_zone;

		lpdijoy->SetProperty(DIPROP_DEADZONE, &dead_band.diph);

		if (lpdijoy->Acquire() != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("初始化游戏杆失败"));
			return false;
		}
		bfound = true;

		return false;
	}


}
