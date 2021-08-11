
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

// defines for polygon cannon
#define CANNON_X0  39       // position of tip of cannon
#define CANNON_Y0  372
#define NUM_PROJECTILES 16  // number of projectiles

SURFACE background;

typedef struct PROJ_TYP
{
	int state;      // state 0 off, 1 on
	float x, y;      // position
	float xv, yv;   // velocity

} PROJECTILE, * PROJECTILE_PTR;

SOUND cannon_ids[8]; // sound ids

POLYGON2D cannon; // the ship

PROJECTILE missiles[NUM_PROJECTILES]; // array of missiles

float gravity_force = 0.2;  // gravity
float wind_force = -0.01; // wind resistance

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo28\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("TANKGRID.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);



	cannon_ids[0].create(path + TEXT("CANNON.WAV"));
	for (int i = 1; i < 8; i++)
		cannon_ids[i].cloneFrom(cannon_ids[0]);

	// define points of cannon
	VERTEX2D cannon_vertices[4] = { 0, -2, 30, 0, 30, 2, 0, 2, };
	cannon.xC = CANNON_X0;
	cannon.yC = CANNON_Y0;
	cannon.vx = 0;
	cannon.vy = 0;
	cannon.color = curPalette[250];
	cannon.copyVertex(cannon_vertices, 4);

	memset(missiles, 0, sizeof(PROJECTILE) * NUM_PROJECTILES);


	ShowCursor(FALSE);
	keyboard.create();
	fpsSet.set(30);
}

void Cannon_Sound(void)
{
	// this functions hunts for an open handle to play a cannon sound

	// start a hit sound
	for (int sound_index = 0; sound_index < 8; sound_index++)
	{
		// test if this sound is playing
		if (!cannon_ids[sound_index].isPlaying())
		{
			cannon_ids[sound_index].play();
			break;
		}

	}

}

void Move_Projectiles(void)
{
	// this function moves all the projectiles and does the physics model
	for (int index = 0; index < NUM_PROJECTILES; index++)
	{
		if (missiles[index].state == 1)
		{
			// translate
			missiles[index].x += missiles[index].xv;
			missiles[index].y += missiles[index].yv;

			// apply forces
			missiles[index].xv += wind_force;
			missiles[index].yv += gravity_force;

			// test for off screen
			if (missiles[index].x >= SCREEN_WIDTH ||
				missiles[index].y >= SCREEN_HEIGHT ||
				missiles[index].y < 0)
			{
				// kill the missile
				missiles[index].state = 0;
			}
		}
	}
}

void Draw_Projectiles(void)
{
	// this function draws all the projectiles 
	for (int index = 0; index < NUM_PROJECTILES; index++)
	{
		// is this one alive?
		if (missiles[index].state == 1)
		{
			DrawRectangle(missiles[index].x - 1, missiles[index].y - 1,
				missiles[index].x + 1, missiles[index].y + 1,
				curPalette[249]);
		}
	}
}

void Fire_Projectile(int angle, float vel)
{
	// this function starts a projectile with the given angle and velocity 
	// at the tip of the cannon
	for (int index = 0; index < NUM_PROJECTILES; index++)
	{
		// find an open projectile
		if (missiles[index].state == 0)
		{
			// set this missile in motion at the head of cannon with the proper angle
			missiles[index].x = cannon[1].x + cannon.xC;
			missiles[index].y = cannon[1].y + cannon.yC;

			// compute velocity vector based on angle
			missiles[index].xv = vel * cos_look[angle];
			missiles[index].yv = -vel * sin_look[angle];

			// mark as active
			missiles[index].state = 1;

			// make sound
			Cannon_Sound();

			// bail
			break;

		}
	}
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);
	BeginDrawOn();
	cannon.draw();
	EndDrawOn();
	keyboard.read();


	static int   curr_angle = 0; // current angle of elevation from horizon
	static float curr_vel = 10; // current velocity of projectile

	// test for rotate
	if ((curr_angle < 90) && keyboard[DIK_UP]) // rotate left
	{
		cannon.rotate(-5);
		curr_angle += 5;
	}
	else if ((curr_angle > 0) && keyboard[DIK_DOWN]) // rotate right
	{
		cannon.rotate(5);
		curr_angle -= 5;
	}

	// test for projectile velocity
	if (keyboard[DIK_RIGHT])
	{
		if (curr_vel < 30) curr_vel += 0.1;
	}
	else if (keyboard[DIK_LEFT])
	{
		if (curr_vel > 0) curr_vel -= 0.1;
	}

	// test for wind force
	if (keyboard[DIK_W])
	{
		if (wind_force < 2) wind_force += 0.01;
	}
	else if (keyboard[DIK_E])
	{
		if (wind_force > -2) wind_force -= 0.01;
	}

	// test for gravity force
	if (keyboard[DIK_G])
	{
		if (gravity_force < 15) gravity_force += 0.1;
	}
	else if (keyboard[DIK_B])
	{
		if (gravity_force > -15) gravity_force -= 0.1;
	}

	// test for fire!
	if (keyboard[DIK_LCONTROL])
	{
		Fire_Projectile(curr_angle, curr_vel);

	}

	// move all the projectiles
	Move_Projectiles();

	// draw the projectiles
	Draw_Projectiles();

	gPrintf(10, 10, RGB(0, 255, 0), TEXT("Trajectory DEMO, Press <ESC> to Exit."));

	// draw information
	gPrintf(10, 25, RGB(0, 255, 0), TEXT("<RIGHT>, <LEFT> to adjust velocity, <UP>, <DOWN> to adjust angle"));
	gPrintf(10, 40, RGB(0, 255, 0), TEXT("<G>, <B> to adjust gravity, <W>, <E> to adjust wind, <CTRL> to fire."));

	gPrintf(10, 60, RGB(0, 255, 0), TEXT("Ang=%d, Vel=%f"), curr_angle, curr_vel);
	gPrintf(10, 75, RGB(0, 255, 0), TEXT("Wind force=%f, Gravity Force=%f"), wind_force, gravity_force);

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


