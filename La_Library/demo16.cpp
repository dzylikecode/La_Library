
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

FRAMECounter fpsSet;
GAMEBox gameBox;

using namespace AUDIO;
using namespace GDI;
using namespace GRAPHIC;

const int flyNum = 128;
const int MIN_X_FLY = 190;
const int MAX_X_FLY = 450;
const int MIN_Y_FLY = 350;
const int MAX_Y_FLY = 470;


SURFACE background;

class FLY :public SPRITE
{
public:
	int timeChangeMotion;
};
LINKEDList<FLY> flyGroup;
aniDICT flyDict;
SOUND flySound;

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo16\\");
	background.createFromBitmap(path + TEXT("FLYBACK.BMP"));

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("FLYS8.BMP"));
	flyDict.resize(4);
	for (int i = 0; i < 4; i++)
	{
		flyDict[i].createFromSurface(8, 8, temp, i * (8 + 1) + 1, 0 * (8 + 1) + 1);
	}

	FLY tempFly;

	int surNum = 0;
	while (surNum < flyNum)
	{
		surNum++;
		tempFly.content.setDict(&flyDict);
		tempFly.x = 320 - 32 + rand() % 64;
		tempFly.y = 450 - rand() % 32;
		tempFly.vx = -4 + rand() % 9;
		tempFly.vy = -4 + rand() % 9;
		tempFly.setAniSpeed(1);
		tempFly.timeChangeMotion = 2 + rand() % 8;
		tempFly.content.resize(1);
		tempFly.content[0] = ANIM(0, 1, 2, 3);
		flyGroup.insertBefore(tempFly);
	}
	flySound.create(path + TEXT("FLYS.WAV"));
	flySound.play(true);


	fpsSet.set(60);
}

void Fly_AI(void)
{
	flyGroup.reset();
	while (!flyGroup.endOfList())
	{
		FLY& curFly = flyGroup.data();
		if (--curFly.timeChangeMotion <= 0)
		{
			curFly.vx = -4 + rand() % 9;
			curFly.vy = -4 + rand() % 9;
			curFly.timeChangeMotion = 2 + rand() % 8;
		}

		curFly.move();
		curFly.animate();
		if (curFly.x > MAX_X_FLY)
		{
			curFly.vx = -curFly.vx;
			curFly.x = MAX_X_FLY;
		}

		if (curFly.x < MIN_X_FLY)
		{
			curFly.vx = -curFly.vx;
			curFly.x = MIN_X_FLY;
		}

		if (curFly.y > MAX_Y_FLY)
		{
			curFly.vy = -curFly.vy;
			curFly.y = MAX_Y_FLY;
		}

		if (curFly.y < MIN_Y_FLY)
		{
			curFly.vy = -curFly.vy;
			curFly.y = MIN_Y_FLY;
		}
		flyGroup.next();
	}
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	Fly_AI();
	flyGroup.reset();
	while (!flyGroup.endOfList())
	{
		flyGroup.data().drawOn();
		flyGroup.next();
	}

	fpsSet.adjust();
	gPrintf(0, 0, RED_GDI, TEXT("%d"), fpsSet.get());
	Flush();
}

void GameClose()
{

}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);

	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("¼üÅÌ¿ØÖÆÒÆ¶¯"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


