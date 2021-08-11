
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

#define FRICTION_FACTOR  (float)(0.05)  // friction of the virtual space

// these are the gravity constants, they are selected to simply work
#define VIRTUAL_GRAVITY_CONSTANT     (float)0.01
#define SHIP_MASS                    (float)2
#define BLACK_HOLE_MASS              (float)50000
#define MAX_VEL                      (float)30

SURFACE background;

SOUND sound;

class SHIP :public SPRITE
{
public:
	int angle;
	REAL tempVx, tempVy;
	REAL tempX, tempY;
}ship;

class BLACK_HOLE :public SPRITE
{

}blackHole;

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo27\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("GRAVSKY8.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("BLAZE8.BMP"));

	static aniDICT shipDict;
	shipDict.resize(32);
	for (int i = 0; i < 32; i++)
	{
		shipDict[i].createFromSurface(22, 18, temp, (i % 16) * (22 + 1) + 1, (i / 16) * (18 + 1) + 1);
	}
	ship.content.setDict(&shipDict);
	ship.content.autoOrder();
	ship.x = 320; ship.y = 240;
	// well use varsI[0] to hold the direction 0..15, 0-360 degrees, clockwise
	ship.angle = 0; // along +x axis to start

	ship.tempVx = 0; ship.tempVy = 0;
	ship.tempX = ship.x; ship.tempY = ship.y;

	temp.createFromBitmap(path + TEXT("PHOTON8.BMP"));
	static aniDICT blackHoleDict;
	blackHoleDict.resize(7);
	for (int i = 0; i < 7; i++)
	{
		blackHoleDict[i].createFromSurface(44, 44, temp, (i) * (44 + 1) + 1, (0) * (44 + 1) + 1);
	}
	blackHole.content.setDict(&blackHoleDict);
	blackHole.content.autoOrder();
	blackHole.x = 32 + rand() % (SCREEN_WIDTH - 64); blackHole.y = 32 + rand() % (SCREEN_HEIGHT - 64);
	blackHole.setAniSpeed(3);

	sound.create(path + TEXT("BHOLE.WAV"));
	sound.play(true);

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

	// check the player controls

	// is the player turning right or left?
	if (keyboard[DIK_RIGHT])
	{
		// there are 16 possible positions for the ship to point in
		if (++ship.angle >= 16)
			ship.angle = 0;
	}
	else if (keyboard[DIK_LEFT])
	{
		// there are 16 possible positions for the ship to point in
		if (--ship.angle < 0)
			ship.angle = 15;
	}

	// now test for forward thrust
	if (keyboard[DIK_UP])
	{
		// thrust ship in current direction

		float rad_angle = (float)ship.angle * (float)3.14159 / (float)8;
		float xv = cos(rad_angle);
		float yv = sin(rad_angle);

		//原有速度和新动力矢量叠加，进而产生惯性的效果
		ship.tempVx += xv;
		ship.tempVy += yv;

		// animate the ship------有尾部喷射火焰
		ship.setAniWord(ship.angle + 16 * (rand() % 2));

	}
	else // show non thrust version
		ship.setAniWord(ship.angle);

	// move ship
	ship.tempX += ship.tempVx;
	ship.tempY += ship.tempVy;

	// always apply friction in direction opposite current trajectory
	//摩擦力
	float fx = -ship.tempVx;
	float fy = -ship.tempVy;
	float length_f = sqrt(fx * fx + fy * fy); // normally we would avoid square root at all costs!

	// compute the frictional resistance
	//摩擦力的计算方法
	if (fabs(length_f) > 0.1)
	{
		fx = FRICTION_FACTOR * fx / length_f;
		fy = FRICTION_FACTOR * fy / length_f;
	}
	else
		fx = fy = 0;

	// now apply friction to forward velocity
	// 摩擦效果，这其实是加速度
	ship.tempVx += fx;
	ship.tempVy += fy;

	////////////////////////////////////////////////////////////////////

	// gravity calculation section

	// step 1: compute vector from black hole to ship, note that the centers
	// of each object are used
	float grav_x = (blackHole.x + blackHole.getWidth() / 2) - (ship.x + ship.getWidth() / 2);
	float grav_y = (blackHole.y + blackHole.getHeight() / 2) - (ship.y + ship.getHeight() / 2);
	float radius_squared = grav_x * grav_x + grav_y * grav_y; // equal to radius squared
	float length_grav = sqrt(radius_squared);

	// step 2: normalize the length of the vector to 1.0
	grav_x = grav_x / length_grav;
	grav_y = grav_y / length_grav;

	// step 3: compute the gravity force
	float grav_force = (VIRTUAL_GRAVITY_CONSTANT) * (SHIP_MASS * BLACK_HOLE_MASS) / radius_squared;

	// step 4: apply g-force in the direction of grav_x, grav_y with the magnitude of grav_force
	//力的分解
	ship.tempVx += grav_x * grav_force;
	ship.tempVy += grav_y * grav_force;

	////////////////////////////////////////////////////////////////////

	// test if ship is off screen
	if (ship.tempX > SCREEN_WIDTH)
		ship.tempX = -ship.getWidth();
	else if (ship.tempX < -ship.getWidth())
		ship.tempX = SCREEN_WIDTH;

	if (ship.tempY > SCREEN_HEIGHT)
		ship.tempY = -ship.getHeight();
	else if (ship.tempY < -ship.getHeight())
		ship.tempY = SCREEN_HEIGHT;

	// test if velocity is insane
	if ((ship.tempVx * ship.tempVx + ship.tempVy * ship.tempVy) > MAX_VEL)
	{
		// scale velocity down
		ship.tempVx *= .95;
		ship.tempVy *= .95;

	}
	blackHole.animate();
	blackHole.drawOn();

	ship.x = ship.tempX; ship.y = ship.tempY;

	ship.drawOn();

	gPrintf(10, 10, RGB(0, 255, 255), TEXT("GRAVITY MASS DEMO - Use Arrows to Control Ship."));

	// draw information


	gPrintf(10, 420, RGB(0, 255, 0), TEXT("Friction: X=%f, Y=%f"), fx, fy);
	gPrintf(10, 440, RGB(0, 255, 0), TEXT("Velocity: X=%f, Y=%f"), ship.tempVx, ship.tempVy);
	gPrintf(10, 460, RGB(0, 255, 0), TEXT("Gravity: X=%f, Y=%f"), ship.tempX, ship.tempY);

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

	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("键盘控制移动"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


