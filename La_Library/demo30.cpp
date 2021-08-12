
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


void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo30\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("PTABLE8.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("POOLBALLS8.BMP"));

	static aniDICT ballDict;
	ballDict.resize(6);
	for (int i = 0; i < 6; i++)
	{
		int width = 24; int height = 24;
		ballDict[i].createFromSurface(width, height, temp, (i) * (width + 1) + 1, (0) * (height + 1) + 1);
	}
	balls[0].content.setDict(&ballDict);
	balls[0].content.autoOrder();
	balls[0].x = 0; balls[0].y = 0;

	// create all the clones
	for (int index = 1; index < NUM_BALLS; index++)
		balls[index].clone(balls[0]);

	// now set the initial conditions of all the balls
	for (int index = 0; index < NUM_BALLS; index++)
	{
		// set position randomly
		balls[index].x = 320 - 50 + rand() % 100;
		balls[index].y = 240 - 100 + rand() % 200;

		// set initial velocity
		balls[index].vx = -6 + rand() % 13;
		balls[index].vy = -6 + rand() % 13;

		// set ball
		balls[index].setAniWord(rand() % 6);

	}

	sound[0].create(path + TEXT("PBALL.WAV"));

	// clone sounds
	for (int index = 1; index < 8; index++)
		sound[index].cloneFrom(sound[0]);

	ShowCursor(FALSE);
	keyboard.create();
	fpsSet.set(30);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	keyboard.read();

	// move all the balls
	for (int index = 0; index < NUM_BALLS; index++)
	{
		// move the ball
		balls[index].move();

		// test for collision with table edges
		if ((balls[index].x + balls[index].getWidth() >= TABLE_MAX_X) ||
			(balls[index].x <= TABLE_MIN_X))
		{
			// invert velocity
			balls[index].vx = -balls[index].vx;

			// start a hit sound
			// if ((rand()%3)==0)
			for (int sound_index = 0; sound_index < 8; sound_index++)
			{
				// test if this sound is playing
				if (!sound[sound_index].isPlaying())
				{
					sound[sound_index].play();
					break;
				}
			}

		}

		if ((balls[index].y + balls[index].getHeight() >= TABLE_MAX_Y) ||
			(balls[index].y <= TABLE_MIN_Y))
		{
			// invert velocity
			balls[index].vy = -balls[index].vy;

			// start a hit sound
			// if ((rand()%3)==0)
			for (int sound_index = 0; sound_index < 8; sound_index++)
			{
				// test if this sound is playing
				if (!sound[sound_index].isPlaying())
				{
					sound[sound_index].play();
					break;
				}
			}

		}

	}
	// draw the balls
	for (int index = 0; index < NUM_BALLS; index++)
		balls[index].drawOn();

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


