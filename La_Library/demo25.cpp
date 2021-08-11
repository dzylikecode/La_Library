
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


#define NUM_CHOPPERS    24   // number of choppers in simulation


SURFACE background;


class CHOP :public SPRITE
{
public:
	int sWidth, sHeight;
}chopper[NUM_CHOPPERS];

SOUND sound;

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo25\\");

	background.createFromBitmap(path + TEXT("SKYSCROLL8.BMP"));

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("CHOP8.BMP"));

	static aniDICT chopDict;
	chopDict.resize(3);
	for (int i = 0; i < 3; i++)
	{
		chopDict[i].createFromSurface(200, 64, temp, 0 * (200 + 1) + 1, i * (64 + 1) + 1);
	}
	chopper[0].content.setDict(&chopDict);
	chopper[0].content.autoOrder();
	chopper[0].vx = 2 + rand() % 6; chopper[0].vy = 0;
	chopper[0].x = rand() % SCREEN_WIDTH; chopper[0].y = rand() % SCREEN_HEIGHT;
	chopper[0].setAniSpeed(1);

	// now make the remaining 15 clones
	for (int index = 1; index < NUM_CHOPPERS; index++)
	{
		// clone the bob
		chopper[index].clone(chopper[0]);

		// set new position and velocity
		chopper[index].vx = 2 + rand() % 6; chopper[index].vy = 0;
		chopper[index].x = rand() % SCREEN_WIDTH; chopper[index].y = rand() % SCREEN_HEIGHT;

		// now here the tricky part, alter the size of the bob, but make sure
		// to keep the aspect ratio the same, so it doesn't look scrunched
		float width = 200 - 10 * index;
		float height = (float)width * ((float)64 / (float)200);

		// store new width and height in varsIable cache positions 0,1
		chopper[index].sWidth = width;
		chopper[index].sHeight = height;

	}

	sound.create(path + TEXT("CHOP.WAV"));
	sound.play(true);

	ShowCursor(FALSE);

	fpsSet.set(60);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);
	background.scrollX(-1);

	// move and animate the fleet
	for (int index = 0; index < NUM_CHOPPERS; index++)
	{
		// move the chopper
		chopper[index].move();

		// test for off the screen wrap around
		if (chopper[index].x > SCREEN_WIDTH)
			chopper[index].x = -chopper[index].sWidth;

		// animate the bat
		chopper[index].animate();

	}
	// draw the choppers
	for (int index = NUM_CHOPPERS - 1; index >= 0; index--)
		chopper[index].drawOn(chopper[index].sWidth, chopper[index].sHeight);

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


