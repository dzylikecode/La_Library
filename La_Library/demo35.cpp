
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

// variable indices
#define SHIP_X_POS  0
#define SHIP_Y_POS  1
#define SHIP_X_VEL  2

#define SHIP_INITIAL_VELOCITY   0.050    // 50 pixels/sec

SURFACE background;

class SHIP :public SPRITE
{
public:
	REAL tempX, tempY;
	REAL tempVx;
};

SHIP ship;

SOUND engineSound;
CLOCK clockTime;

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo35\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("LAVA.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("LAVASHIP.BMP"));

	static aniDICT shipDict;
	shipDict.resize(2);
	for (int i = 0; i < 2; i++)
	{
		int width = 80; int height = 30;
		shipDict[i].createFromSurface(width, height, temp, (i) * (width + 1) + 1, (0) * (height + 1) + 1);
	}
	ship.content.setDict(&shipDict);
	ship.content.autoOrder();
	ship.x = 0; ship.y = 0;
	ship.tempX = 0; ship.tempY = SCREEN_HEIGHT / 2;
	ship.tempVx = SHIP_INITIAL_VELOCITY;


	engineSound.create(path + TEXT("PULSAR.WAV"));
	engineSound.play(true);

	ShowCursor(FALSE);
	keyboard.create();
	fpsSet.set(60);
}


void GameBody(void)
{

	clockTime.start();
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);


	keyboard.read();

	static int delay = 30; // initial delay per frame

	// update delay
	if (keyboard[DIK_RIGHT])
		delay += 5;
	else if (delay > 5 && keyboard[DIK_LEFT])
		delay -= 5;


	// draw the ship
	ship.x = ship.tempX + 0.5;
	ship.y = ship.tempY + 0.5;
	ship.drawOn(0);

	// draw shadow
	ship.x -= 64;
	ship.y += 128;
	ship.drawOn(1);

	gPrintf(10, 10, RGB(0, 255, 0), TEXT("Time Based Kinematic Motion DEMO, Press <ESC> to Exit."));

	gPrintf(10, 25, RGB(0, 255, 0), TEXT("Frame rate = %f, use <RIGHT>, <LEFT> arrows to change load"), 1000 * 1 / (float)delay);
	gPrintf(10, 40, RGB(0, 255, 0), TEXT("<G>, <B> adjusts particle gravity, <W>, <E> adjusts particle wind, <CTRL> to fire."));
	gPrintf(10, 60, RGB(0, 255, 0), TEXT("Ship Velocity is %f pixels/sec"), 1000 * ship.tempVx);

	TIMEMaster::sleep(delay);
	gPrintf(0, 0, RED_GDI, TEXT("%d"), 1000 / delay);
	Flush();


	REAL dt = clockTime.record();
	ship.tempX += ship.tempVx * dt;

	gPrintf(10, 75, RGB(0, 255, 0), TEXT("Time is %f ms"), dt);

	if (ship.tempX > SCREEN_WIDTH + ship.getWidth())
		ship.tempX = -ship.getWidth();
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


