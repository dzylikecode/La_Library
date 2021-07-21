
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

SOUND sinSound;


void StartUp(void)
{
	sinSound.create(64000);

	BYTE temp[64000];
	// note the math, 127 is the scale or amplitude -> 因为是 8 位嘛 然后又有正负 幅值的绝对值就是 7 位啦
	// 6.28 is to convert to radians
	// (i % 110) read below
	// we are playing at 11025 HZ or 11025 cycles/sec therefore, in 1 sec
	// 播放是 11025 HZ，也可成为 采样或者 播放频率，但是声音周期要是 100 HZ 
	// we want 100 cycles of our synthesized sound, thus 11025/100 is approx.
	// 于是 11025 / 100 = 110 个点是一个周期内的插值点
	// 110, thus we want the waveform to repeat each 110 clicks of index, so
	// normalize to 110

	for (DWORD i = 0; i < 64000; i++)
	{
		//rand() 是嘈杂的声音
		//rand() 换成 i , 则是哔哔哔单调的声音
		temp[i] = 127 * sin(6.28 * ((float)(rand() % 110)) / (float)110);
	}
	BeginComposeOn(&sinSound);
	sinSound.copyContent(temp, 64000);
	EndComposeOn(&sinSound);
	sinSound.play(true);
}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("键盘控制移动"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


