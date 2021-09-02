
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
			ERROR_MSG(ERR, TEXT("�޷���ʼ�� INPUT error code�� %d"), hre);
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
		//��ü��̽ӿ�
		if (lpdi8->CreateDevice(GUID_SysKeyboard, &lpdikey, nullptr) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ������ʧ��"));
			return false;
		}

		//����Э���ȼ�
		//����ռ����̨����->����û�������С����ʱ��Ҳ���ܵ���Ϣ
		//FOREGROUND->��Ҫ��������ǰ̨ʱ���ܷ��ʣ���̨��ʱ���Զ�ȡ��
		if (lpdikey->SetCooperativeLevel(globalHwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		{
			//���ʧ�ܣ�һ���ǳ����ڶ�ռ���̣����¼�������ʧ��
			ERROR_MSG(ERR, TEXT("��ʼ������ʧ��"));
			return false;
		}

		//�������ݸ�ʽ
		//���Ը���Ҫ��������Լ����ݰ������ݽṹ
		//���ڼ��̲�ȡԤ����Ľṹ�Ϳ�����
		if (lpdikey->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ������ʧ��"));
			return false;
		}

		//����豸
		if (lpdikey->Acquire() != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ������ʧ��"));
			return false;
		}
		return true;
	}

	bool MOUSE::create()
	{
		if (lpdi8->CreateDevice(GUID_SysMouse, &lpdimouse, nullptr) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ�����ʧ��"));
			return false;
		}

		//����Э���ȼ�
		//����ռ����̨����->����û�������С����ʱ��Ҳ���ܵ���Ϣ
		//FOREGROUND->��Ҫ��������ǰ̨ʱ���ܷ��ʣ���̨��ʱ���Զ�ȡ��
		if (lpdimouse->SetCooperativeLevel(globalHwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ�����ʧ��"));
			return false;
		}

		//��������ȡԤ����Ľṹ�Ϳ�����
		if (lpdimouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ�����ʧ��"));
			return false;
		}

		//����豸
		if (lpdimouse->Acquire() != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ�����ʧ��"));
			return false;
		}
		return true;
	}

	/// <summary>
	/// ö����Ϸ�ˣ�����ֻö�ٵ�һ��
	/// </summary>
	/// <returns>
	/// ����ֵ��΢����� BOOL
	/// </returns>
	BOOL CALLBACK JOYSTICK::EnumJoysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID guid_ptr)
	{
		//������� guid
		//Ҳ���Դ��� guid �б�
		//��һ����̬�� static int  �����ڼ���
		//�浽 guid �б���
		//�����Է���
		*(GUID*)guid_ptr = lpddi->guidInstance; //lpddi ���洢������Ϣ
		curCallBack->joyname = lpddi->tszProductName;
		//ֹͣ
		return DIENUM_STOP; //���Լ���������ֻҪ��һ���͹���
	}

	bool JOYSTICK::create(int min_x/* =-256 */, int max_x/* =256 */, int min_y/* =-256 */, int max_y/* =256 */, int dead_zone /* = 10 */)
	{
	// ���� 1  �豸����
	// ���� 2  �ص�����������ɨ��õ�������
	// ���� 3  �õ� GUID
	// ���� 4  ɨ�����ֻ���Ѿ����Ӻõ�
		curCallBack = this;
		lpdi8->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticks, &joystickGUID, DIEDFL_ATTACHEDONLY);

		//create the version 1.0 interface
		LPDIRECTINPUTDEVICE lpdijoy_temp = nullptr;

		if (lpdi8->CreateDevice(joystickGUID, &lpdijoy, nullptr) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ����Ϸ��ʧ��"));
			return false;
		}

		if (lpdijoy->SetCooperativeLevel(globalHwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ����Ϸ��ʧ��"));
			return false;
		}

		if (lpdijoy->SetDataFormat(&c_dfDIJoystick) != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ����Ϸ��ʧ��"));
			return false;
		}

		//����Ϸ�����÷�Χ
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

		//�������� -> ������������λ�õ�ʱ�򲻷����κ�����
		//���õķ������ñ��� ���� x �� ��Χ �� -1024 - 1024
		//Ҫ�� -102 102 ����Ϊ������������ ����Ϊ 10%
		//��ô 10000 * 10% �õ��ľ��� 1000

		dead_zone *= 100;//����ת��

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

		//�������䶼����������
		dead_band.dwData = dead_zone;

		lpdijoy->SetProperty(DIPROP_DEADZONE, &dead_band.diph);

		if (lpdijoy->Acquire() != DI_OK)
		{
			ERROR_MSG(ERR, TEXT("��ʼ����Ϸ��ʧ��"));
			return false;
		}
		bfound = true;

		return false;
	}


}
