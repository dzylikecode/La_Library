
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include "La_Windows.h"
#include "La_WinGDI.h"
#include "La_GameBox.h"
#include "La_Graphic.h"
#include "La_GraphicEx.h"
#include "La_Input.h"
#include "La_Audio.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

GAMEBox gameBox;

using namespace AUDIO;

SOUND fightSound;


void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo13\\");
	fightSound.create(path + TEXT("FLIGHT.WAV"));
	fightSound.play(true);
}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT	ps;
	HDC			hdc;

	static HWND volume_hwnd, freq_hwnd, pan_hwnd;

	switch (message)
	{
	case WM_CREATE:
		volume_hwnd = CreateWindow(TEXT("SCROLLBAR"), TEXT(""),
			WS_CHILD | WS_VISIBLE,
			80, 40,
			160, 16,
			hwnd, NULL, gameBox.getHInstance(), NULL);   //显然嘛，hwnd 就是 父窗口嘛

		freq_hwnd = CreateWindow(TEXT("SCROLLBAR"), TEXT(""),
			WS_CHILD | WS_VISIBLE,
			80, 100,
			160, 16,
			hwnd, NULL, gameBox.getHInstance(), NULL);

		pan_hwnd = CreateWindow(TEXT("SCROLLBAR"), TEXT(""), WS_CHILD | WS_VISIBLE,
			80, 160,
			160, 16,
			hwnd, NULL, gameBox.getHInstance(), NULL);

		SetScrollRange(volume_hwnd, SB_CTL, 0, 100, TRUE);
		SetScrollPos(volume_hwnd, SB_CTL, 100, TRUE);

		SetScrollRange(freq_hwnd, SB_CTL, 0, 50000, TRUE);
		SetScrollPos(freq_hwnd, SB_CTL, 11000, TRUE);

		SetScrollRange(pan_hwnd, SB_CTL, 0, 20000, TRUE);
		SetScrollPos(pan_hwnd, SB_CTL, 10000, TRUE);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		// first the static text
		// set the color
		SetTextColor(hdc, RGB(0, 255, 0));
		SetBkColor(hdc, RGB(0, 0, 0));
		SetBkMode(hdc, OPAQUE);

		TextOut(hdc, 160 - 5 * 7, 40 - 20, TEXT("VOLUME        "), strlen("VOLUME        "));
		TextOut(hdc, 160 - 5 * 13, 100 - 20, TEXT("PLAYBACK RATE        "), strlen("PLAYBACK RATE        "));
		TextOut(hdc, 160 - 5 * 14, 160 - 20, TEXT("STEREO PANNING        "), strlen("STEREO PANNING        "));

		// end painting
		EndPaint(hwnd, &ps);
		return 0;

	case WM_HSCROLL:
	case WM_VSCROLL:

		switch (LOWORD(wParam))
		{
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			int npos = (int)HIWORD(wParam);			// scroll box position 
			HWND hwndscrollbar = (HWND)lParam;        // handle of scroll bar

			hdc = GetDC(hwnd);
			SetTextColor(hdc, RGB(0, 255, 0));
			SetBkColor(hdc, RGB(0, 0, 0));
			SetBkMode(hdc, OPAQUE);
			TCHAR buffer[MAX_BUFFER] = { 0 };

			//总之这个调节起来相当不丝滑，需要改进公式
			if (hwndscrollbar == volume_hwnd)
			{
				SetScrollPos(volume_hwnd, SB_CTL, npos, TRUE);

				_stprintf(buffer, TEXT("VOLUME=%d       "), npos);
				TextOut(hdc, 160 - 5 * 7, 40 - 20, buffer, lstrlen(buffer));
#define VOLUME_SPAN		100
				//-10000 - 0 是衰减系数，0就是没有衰减 -10000相当于 -100dB
#define VOLUME_GAP		((DSBVOLUME_MAX - DSBVOLUME_MIN)/VOLUME_SPAN)
	//正向化
#define VOLUME_POSITIVE(n)	((DWORD)VOLUME_GAP*(n - VOLUME_SPAN))
				fightSound.setVolume(VOLUME_POSITIVE(npos));

			}
			else if (hwndscrollbar == freq_hwnd)
			{
				SetScrollPos(freq_hwnd, SB_CTL, npos, TRUE);

				_stprintf(buffer, TEXT("PLAYBACK RATE=%d      "), npos);
				TextOut(hdc, 160 - 5 * 13, 100 - 20, buffer, lstrlen(buffer));

				fightSound.setFrequency(npos);
			}
			else if (hwndscrollbar == pan_hwnd)
			{
				SetScrollPos(pan_hwnd, SB_CTL, npos, TRUE);

				_stprintf(buffer, TEXT("STEREO PANNING=%d      "), (npos - 10000));
				TextOut(hdc, 160 - 5 * 14, 160 - 20, buffer, lstrlen(buffer));

				fightSound.setPan((npos - 10000));
			}

			ReleaseDC(hwnd, hdc);
			break;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		fightSound.stop();
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.setWndProc(WndProc);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("键盘控制移动"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


