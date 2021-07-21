
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
	// note the math, 127 is the scale or amplitude -> ��Ϊ�� 8 λ�� Ȼ���������� ��ֵ�ľ���ֵ���� 7 λ��
	// 6.28 is to convert to radians
	// (i % 110) read below
	// we are playing at 11025 HZ or 11025 cycles/sec therefore, in 1 sec
	// ������ 11025 HZ��Ҳ�ɳ�Ϊ �������� ����Ƶ�ʣ�������������Ҫ�� 100 HZ 
	// we want 100 cycles of our synthesized sound, thus 11025/100 is approx.
	// ���� 11025 / 100 = 110 ������һ�������ڵĲ�ֵ��
	// 110, thus we want the waveform to repeat each 110 clicks of index, so
	// normalize to 110

	for (DWORD i = 0; i < 64000; i++)
	{
		//rand() �����ӵ�����
		//rand() ���� i , ���������ٵ���������
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
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("���̿����ƶ�"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


