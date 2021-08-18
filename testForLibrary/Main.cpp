
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
using namespace INPUT_;

KEYBOARD keyboard;

#define NUM_BALLS       10   // number of pool balls

// extents of table
#define TABLE_MAX_X     468
#define TABLE_MIN_X     176
#define TABLE_MAX_Y     448
#define TABLE_MIN_Y     44


SURFACE background;

SOUND sound[8];


class BALL :public SPRITE
{
public:
}balls[NUM_BALLS];







IMAGE_GIF gif;

void StartUp(void)
{

	gif.load(TEXT("F:\\Git_WorkSpace\\La_Library\\Debug\\1-1.gif"));


	ShowCursor(FALSE);
	keyboard.create();
	fpsSet.set(30);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	keyboard.read();


	static int i = 0;

	gif.frame[i++].drawOn(0, 0, true);

	i %= gif.frame.getSize();

	gPrintf(10, 10, RGB(0, 255, 0), TEXT("ELASTIC COLLISION DEMO"));

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


