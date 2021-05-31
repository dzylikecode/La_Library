#include "La_GameBox.h"
#include <windowsx.h>
#include "La_WinGDI.h"
#include "La_Graphic.h"

namespace
{
	void La_GameBody(void)
	{
		return;
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CREATE:

			ERROR_MSG(INFO, TEXT("Create Windows Successfully"));
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}


GAMEBox::GAMEBox() :bCreated(false), bExit(false), bWindowed(true), bDX(true)
{
	wndclassEx.cbSize = sizeof(WNDCLASSEX);
	wndclassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclassEx.lpfnWndProc = WindowProc;
	wndclassEx.cbClsExtra = 0;
	wndclassEx.cbWndExtra = 0;
	wndclassEx.hInstance = GetModuleHandle(nullptr);
	wndclassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassEx.lpszMenuName = nullptr;
	wndclassEx.lpszClassName = TEXT("La_GameBox");
	wndclassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	exInitialize = La_GameBody;
	gameBody = La_GameBody;
	exShutDown = La_GameBody;
}

bool GAMEBox::create(int width, int height, LPCTSTR title, bool bTopLeft)
{
	if (bCreated) //ֻ������һ��
	{
		ERROR_MSG(WARN, TEXT("Game Box has been Created!"));
		return true;
	}

	if (!RegisterClassEx(&wndclassEx))
	{
		ERROR_MSG(ERR, TEXT("Register Window Class Failed !!!"));
		//End_Debug(); ��Ӧ���ڴ˽���������Ӧ��������������
		return false;
	}
	int iHeight = height;
	int iWidth = width;
	DWORD wndStyle = WS_OVERLAPPEDWINDOW;
	int wndX = FULL_SCREEN_X / 2 - iWidth / 2;
	int wndY = FULL_SCREEN_Y / 2 - iHeight / 2;



	if (!iHeight || !iWidth || (iWidth == FULL_SCREEN_X && iHeight == FULL_SCREEN_Y))
	{
		wndStyle = WS_POPUP | WS_VISIBLE;
		wndX = 0;
		wndY = 0;
		iWidth = FULL_SCREEN_X;
		iHeight = FULL_SCREEN_Y;
	}

	if (bDX)
	{
		if (bWindowed)
		{
			//DX ģʽ�£���ֹ�ı� window ��С
			wndStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_VISIBLE;

		}
		else
		{
			wndStyle = WS_POPUP | WS_VISIBLE;
			wndX = 0;
			wndY = 0;
		}
	}// win GDI �ҿɲ���



	hwnd = CreateWindow(wndclassEx.lpszClassName, title,
		wndStyle,
		wndX, wndY,
		iWidth, iHeight,
		NULL, NULL, hInst, nullptr);

	if (!hwnd)
	{
		ERROR_MSG(ERR, TEXT("Create Window Failed"));
		hInst = NULL;
		return false;
	}

	if (bWindowed)
	{
		//���õ��ǿͻ�����С�����Ǵ��ڴ�С
		//�����б�Ҫ�ĵ���
		RECT window_rect = { wndX, wndY, wndX + width - 1, wndY + height - 1 };


		//������Ҫ�Ĵ��ھ��εĴ�С��
		//������Ĵ��ھ��������Դ��͸�CreateWindowEx������
		//���ڴ���һ���ͻ��������С�Ĵ���
		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(hwnd),
			GetMenu(hwnd) != NULL,
			GetWindowExStyle(hwnd));

		if (bTopLeft)
		{
			wndX = 0;
			wndY = 0;
		}
		// now resize the window
		MoveWindow(hwnd,
			wndX,
			wndY,
			window_rect.right - window_rect.left, // width
			window_rect.bottom - window_rect.top, // height
			FALSE);

		if (bTopLeft)
		{
			POINT point = { 0, 0 };
			//��ÿͻ������ڵ�λ��
			ClientToScreen(hwnd, &point);

			wndX = -point.x;
			wndY = -point.y;

			MoveWindow(hwnd,
				wndX,
				wndY,
				window_rect.right - window_rect.left, // width
				window_rect.bottom - window_rect.top, // height
				FALSE);
		}
		InvalidateRect(hwnd, nullptr, true);
	}
	// show the window, so there's no garbage on first render
	ShowWindow(hwnd, SW_SHOW);
	if (bWindowed)
	{
		GDI::InitializeGDI(hwnd);
	}
	/*Initialize_Clock();
	Initialize_WinGDI(LaDzy_global_hwnd);
	Initialize_Mathematic();
	Initialize_Input(LaDzy_global_hInstance, LaDzy_global_hwnd);
	Initialize_Audio(LaDzy_global_hwnd);
	*/
	if (bDX)
	{
		GRAPHIC::InitializeGraphics(hwnd, iWidth, iHeight, bWindowed);
	}

	bCreated = true;
	return true;
}

bool GAMEBox::startCommuication()
{
	if (!bCreated)
	{
		ERROR_MSG(ERR, TEXT("Create Game Box Failed, Can not Start Commuication!"));
		return false;
	}
	ERROR_MSG(INFO, TEXT("Start Commuicating With System"));
	exInitialize();
	MSG msg;
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);

			DispatchMessage(&msg);
		}
		if (bExit)
		{
			SendMessage(hwnd, WM_CLOSE, 0, 0);
		}
		else
		{
			gameBody();
		}
	}

	if (bCreated)
	{
		bCreated = false;		//��������
	}
	exShutDown();
	ERROR_MSG(WARN, TEXT("End Commuicating With System"));
	return true;
}
