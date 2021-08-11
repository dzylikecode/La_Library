
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

#define ROCKET_STATE_ON_PAD    0
#define ROCKET_STATE_IN_FLIGHT 1

SURFACE background;

SOUND sound;

class ROCKET :public SPRITE
{
public:
	int anim_state;
	bool bAccelerate;
	REAL vel;
	REAL acc;
}rocket;

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo26\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("GANTRY.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("ROCKET8.BMP"));

	static aniDICT rocketDict;
	rocketDict.resize(3);
	for (int i = 0; i < 3; i++)
	{
		rocketDict[i].createFromSurface(24, 80, temp, i * (24 + 1) + 1, 0 * (80 + 1) + 1);
	}
	rocket.content.setDict(&rocketDict);
	rocket.content.autoOrder();
	rocket.x = 312; rocket.y = 420;
	rocket.setAniSpeed(2);

	// set animation state to on pad
	rocket.anim_state = ROCKET_STATE_ON_PAD;

	// use varsI[0] to hold acceleration mode, set to on
	rocket.bAccelerate = true; // -1 is off

	// use varsF[0,1] to hold the velocity and acceleration
	rocket.vel = 0; // initial velocity
	rocket.acc = 0.2; // initial acceleration 0.2 pixel/frame^2

	sound.create(path + TEXT("ROCKET.WAV"));

	ShowCursor(FALSE);
	keyboard.create();
	fpsSet.set(60);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	keyboard.read();

	static int debounce_a = 0; // used to debounce acceleration toggle key

	// test if user is toggling acceleration control
	if (keyboard[DIK_A] && !debounce_a)
	{
		// toggle acceleration
		rocket.bAccelerate = !rocket.bAccelerate;

		// debounce key, so it doesn't toggle 60 times!
		debounce_a = 1;
	}

	if (!keyboard[DIK_A])
		debounce_a = 0;

	// test if user is changing acceleration
	if (keyboard[DIK_DOWN])
	{
		// decrease acceleration factor   
		if ((rocket.acc -= 0.1) < 0)
			rocket.acc = 0.1;
	}
	else if (keyboard[DIK_UP])
	{
		// increase acceleration factor   
		rocket.acc += 0.1;
	} 

 // test if player is firing rocket
	if (keyboard[DIK_SPACE] && rocket.anim_state == ROCKET_STATE_ON_PAD)
	{
		// fire the rocket
		rocket.anim_state = ROCKET_STATE_IN_FLIGHT;

		// start sound
		sound.play();

	}

	// test state of rocket
	if (rocket.anim_state == ROCKET_STATE_ON_PAD)
	{
		// make sure there is no animation and frame is 0
		rocket.setAniWord(0);

	}
	else // rocket is in flight
	{
		// animate the rocket
		rocket.animate();

		// move the rocket

		// update the position with the current velocity
		rocket.y -= (int)(rocket.vel + 0.5);

		// update velocity with acceleration (if acceleration is on)
		if (rocket.bAccelerate)
			rocket.vel += rocket.acc;

		// test if the rocket has moved off the screen
		if (rocket.y < -4 * rocket.getHeight())
		{
			// reset everything
			rocket.anim_state = ROCKET_STATE_ON_PAD;
			rocket.setAniWord(0);
			rocket.vel = 0;

			// reset position
			rocket.x = 312; rocket.y = 420;

			// turn sound off
			sound.stop();
		}
	}

	rocket.drawOn();

	gPrintf(10, 10, RGB(0, 255, 255), TEXT("ACCELERATION DEMO - Space to Launch, Arrows to Change Acceleration."));

	// draw information

	// first acceleration mode
	if (rocket.bAccelerate)
		gPrintf(10, 25, RGB(0, 255, 0), TEXT("Acceleration is ON"));
	else
		gPrintf(10, 25, RGB(0, 255, 0), TEXT("Acceleration is OFF"));

	gPrintf(10, 40, RGB(0, 255, 0), TEXT("Velocity = %f pixels/frame"), rocket.vel);
	gPrintf(10, 55, RGB(0, 255, 0), TEXT("Acceleration = %f pixels/frame^2"), rocket.acc);


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


