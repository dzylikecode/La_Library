
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

#define ARM_ANG         2   // arm rotation angle increment

SURFACE background;

POLYGON2D arm1[2], arm2[2];       // the arms of the linkage
								  // since the transform function modify
								  // the local coordites, I need two versions, 
								  // so I can make a copy of the original object and always
								  // transform from there, this is just because normally
								  // you have a set of local coords and the transformed temp
								  // coords, but I saved space and made them the same, so
								  // this is a fix to make it work
								  // hence index 0 is original and 1 will be transformed


void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo33\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("ARMGRID.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

	// define points of arm1
	VERTEX2D arm1_vertices[4] = { -10,-10, 100,-10, 100, 10, -10, 10, };

	// initialize arm1
	arm1[0].xC = 0; // position it
	arm1[0].yC = 0;
	arm1[0].vx = 0;
	arm1[0].vy = 0;
	arm1[0].color = curPalette[250]; // green
	arm1[0].copyVertex(arm1_vertices, 4);

	// define points of arm2
	VERTEX2D arm2_vertices[4] = { -10,-10, 100,-10, 100, 10, -10,10, };

	// initialize arm2
	arm2[0].xC = 0; // position it
	arm2[0].yC = 0;
	arm2[0].vx = 0;
	arm2[0].vy = 0;
	arm2[0].color = curPalette[250]; // green
	arm2[0].copyVertex(arm2_vertices, 4);

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

	static int curr_angle1 = -90, curr_angle2 = 45; // current link angles

	// test for arm1 rotation
	if (keyboard[DIK_A])
	{
		curr_angle1 -= ARM_ANG;
	}
	else if (keyboard[DIK_S])
	{
		curr_angle1 += ARM_ANG;
	}


	// test for arm2 rotation
	if (keyboard[DIK_D])
	{
		curr_angle2 -= ARM_ANG;
	}
	else if (keyboard[DIK_F])
	{
		curr_angle2 += ARM_ANG;
	}


	// test for overflow
	if (curr_angle1 >= 360)
		curr_angle1 -= 360;
	else if (curr_angle1 < 0)
		curr_angle1 += 360;

	if (curr_angle2 >= 360)
		curr_angle2 -= 360;
	else if (curr_angle2 < 0)
		curr_angle2 += 360;


	// first arm1

	// first copy arms to transform polygon
	arm1[1] = arm1[0];


	// rotate
	arm1[1].rotate(curr_angle1);

	// now translate
	arm1[1].translateEveryPoint(324, 400);
	// now arm2

	// first copy arms to transform polygon
	arm2[1] = arm2[0];

	// rotate
	arm2[1].rotate(curr_angle2);
	// and rotate around link 1
	arm2[1].rotate(curr_angle1);

	// now translate to link 1
	arm2[1].translateEveryPoint((arm1[1][1].x + arm1[1][2].x) / 2, (arm1[1][1].y + arm1[1][2].y) / 2);

	// lock back buffer and copy background into it
	BeginDrawOn();

	// draw arms
	arm1[1].draw();
	arm2[1].draw();

	// unlock back surface
	EndDrawOn();

	gPrintf(10, 10, RGB(0, 255, 0), TEXT("Forward Kinematic DEMO, Press <ESC> to Exit."));

	gPrintf(10, 25, RGB(0, 255, 0), TEXT("<A>, <S> to adjust Arm 1, <D>, <F> to adjust Arm 2"));
	gPrintf(10, 60, RGB(0, 255, 0), TEXT("Arm 1 Angle=%d, Arm 2 Angle=%d"), 360 - curr_angle1, 360 - curr_angle2);

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


