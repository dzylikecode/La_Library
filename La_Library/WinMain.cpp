
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
#include ".\\Resource\\demo15\\resource.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

GAMEBox gameBox;

using namespace AUDIO;
using namespace GDI;

MUSIC midiMusic[10];
SOUND sound[4];

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo15\\");
	TCHAR buffer[30];
	for (int i = 0; i < 10; i++)
	{
		_stprintf(buffer, TEXT("MIDIFILE%d.MID"), i);
		midiMusic[i].create(buffer, path);
	}

	sound[0].create(path + TEXT("DEACTIVATE1.WAV"));
	sound[1].create(path + TEXT("EXP0.WAV"));
	sound[2].create(path + TEXT("INCORRECT1.WAV"));
	sound[3].create(path + TEXT("UPLINK1.WAV"));
}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	for (int i = 0; i < 10; i++)
	{
		if (KEY_DOWN('0' + i))
		{
			midiMusic[i].play();
			SetWinBKMode(false);
			SetWinTextBkColor(RGB(0, 0, 0));
			SetWinTextColor(RGB(0, 255, 0));

			WinPrintf(8, 200, TEXT("Playing MIDI segment [%d]   "), i);
		}
	}
}

void GameClose()
{
	
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT		ps;		
	HDC				hdc;
	TCHAR buffer[80];      
	int id = 0;

	switch (msg)
	{
	case WM_COMMAND:

		switch (LOWORD(wparam))
		{
		case MENU_FILE_ID_EXIT:
			PostQuitMessage(0);
			break;


		case MENU_HELP_ABOUT:
			MessageBox(hwnd, TEXT("Menu MIDI Demo"), TEXT("About MIDI Menu"), MB_OK | MB_ICONEXCLAMATION);
			break;


		case MENU_PLAY_ID_0:
		case MENU_PLAY_ID_1:
		case MENU_PLAY_ID_2:
		case MENU_PLAY_ID_3:
		case MENU_PLAY_ID_4:
		case MENU_PLAY_ID_5:
		case MENU_PLAY_ID_6:
		case MENU_PLAY_ID_7:
		case MENU_PLAY_ID_8:
		case MENU_PLAY_ID_9:
			// play the midi file
			id = LOWORD(wparam) - MENU_PLAY_ID_0;

			midiMusic[id].play();
			SetWinBKMode(false);
			SetWinTextBkColor(RGB(0, 0, 0));
			SetWinTextColor(RGB(0, 255, 0));
			WinPrintf(8, 200, TEXT("Playing MIDI segment [%d]   "), id);

			break;

		case MENU_PLAY_ID_10:
		case MENU_PLAY_ID_11:
		case MENU_PLAY_ID_12:
		case MENU_PLAY_ID_13:

			// play the digital file
			id = LOWORD(wparam) - MENU_PLAY_ID_10;

			sound[id].play(false);

			SetWinBKMode(false);
			SetWinTextBkColor(RGB(0, 0, 0));
			SetWinTextColor(RGB(0, 255, 0));

			WinPrintf(8, 200, TEXT("Playing DIGITAL WAV [%d]   "), id);

			break;
		}

		break;

	case WM_PAINT:

		hdc = BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);

		return(0);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return(0);
		break;
	}
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.setWndProc(WndProc);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("¼üÅÌ¿ØÖÆÒÆ¶¯"));
	gameBox.attachMenu(IDR_MENU1);
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


