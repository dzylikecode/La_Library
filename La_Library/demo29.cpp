
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

#define FRICTION_FACTOR     (float)(0.1)  // friction of table
#define PUCK_START_X        30            // starting location of puck
#define PUCK_START_Y        220
#define PUCK_STATE_RESTING  0             // puck is sitting
#define PUCK_STATE_MOVING   1             // puck has been hit

// extents of table
#define TABLE_MAX_X     620
#define TABLE_MIN_X     14
#define TABLE_MAX_Y     370
#define TABLE_MIN_Y     80


SURFACE background;

SOUND sound;


class PUCK :public SPRITE
{
public:
	int anim_state;
	REAL tempVx, tempVy;
	REAL tempX, tempY;
}puck;

float friction = FRICTION_FACTOR; // frictional coefficient

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo29\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("HOCKEY8.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("PUCK8.BMP"));

	static aniDICT puckDict;
	puckDict.resize(1);
	for (int i = 0; i < 1; i++)
	{
		puckDict[i].createFromSurface(32, 32, temp, (i) * (32 + 1) + 1, (0) * (32 + 1) + 1);
	}
	puck.content.setDict(&puckDict);
	puck.content.autoOrder();
	puck.x = PUCK_START_X; puck.y = PUCK_START_Y;

	// set state of puck
	puck.anim_state = PUCK_STATE_RESTING;

	puck.tempVx = 0; puck.tempVy = 0;
	puck.tempX = puck.x; puck.tempY = puck.y;

	sound.create(path + TEXT("PUCK.WAV"));

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

	// is player changing friction
	if (keyboard[DIK_RIGHT])
		friction += 0.005;
	else if (keyboard[DIK_LEFT])
		if ((friction -= 0.005) < 0)
			friction = 0.0;

	// check if player is hiting puck
	if (keyboard[DIK_SPACE] && puck.anim_state == PUCK_STATE_RESTING)
	{
		// first set state to motion
		puck.anim_state = PUCK_STATE_MOVING;

		// reset puck position
		puck.tempX = PUCK_START_X;
		puck.tempY = PUCK_START_Y;

		// select random initial trajectory
		puck.tempVx = 12 + rand() % 8;
		puck.tempVy = -8 + rand() % 17;

	}

	// move puck
	puck.tempX += puck.tempVx;
	puck.tempY += puck.tempVy;

	///////////////////////////////////////////////////////////

	// apply friction in direction opposite current trajectory
	float fx = -puck.tempVx;
	float fy = -puck.tempVy;
	float length_f = sqrt(fx * fx + fy * fy); // normally we would avoid square root at all costs!

	// compute the frictional resistance

	if (fabs(length_f) >= 0.50)
	{
		fx = friction * fx / length_f;
		fy = friction * fy / length_f;
	}
	else
	{
		// force 0.0
		fx = fy = 0.0;

		// kill velocity
		puck.tempVx = 0.0;
		puck.tempVy = 0.0;

		// puck is stuck, let player fire again
		puck.anim_state = PUCK_STATE_RESTING;
	}

	// now apply friction to forward velocity
	puck.tempVx += fx;
	puck.tempVy += fy;

	// test if puck is off screen
	if (puck.tempX + puck.getWidth() >= TABLE_MAX_X || puck.tempX <= TABLE_MIN_X)
	{
		// invert velocity
		puck.tempVx = -puck.tempVx;

		// move puck
		puck.tempX += puck.tempVx;
		puck.tempY += puck.tempVy;

		sound.play();
	}

	if (puck.tempY + puck.getHeight() >= TABLE_MAX_Y || puck.tempY <= TABLE_MIN_Y)
	{
		// invert velocity
		puck.tempVy = -puck.tempVy;

		// move puck
		puck.tempX += puck.tempVx;
		puck.tempY += puck.tempVy;

		sound.play();

	}

	// copy floating point position to bob x,y
	puck.x = puck.tempX + 0.5;
	puck.y = puck.tempY + 0.5;

	// draw skid marks to table if puck is in motion
	if (fabs(puck.tempVx) > 0 || fabs(puck.tempVy) > 0)
	{
		BeginDrawOn(&background);
		GRAPHIC::SetPixel(puck.x + puck.getWidth() / 2 - 2 + rand() % 4, puck.y + puck.getHeight() / 2 - 2 + rand() % 4, curPalette[16 + rand() % 8], &background);
		EndDrawOn(&background);
	}

	// draw the puck
	puck.drawOn();

	gPrintf(10, 10, RGB(0, 255, 0), TEXT("SIMPLE FRICTION DEMO - Hit Space to Shoot Puck, Arrows to Change Friction."));

	// draw information


	gPrintf(10, 410, RGB(0, 255, 0), TEXT("Friction: X=%f, Y=%f"), fx, fy);
	gPrintf(10, 430, RGB(0, 255, 0), TEXT("Velocity: X=%f, Y=%f"), puck.tempVx, puck.tempVy);
	gPrintf(10, 450, RGB(0, 255, 0), TEXT("Frictional Coefficient: %f"), friction);

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


