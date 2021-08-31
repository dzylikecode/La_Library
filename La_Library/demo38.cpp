
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
#include "La_Master.h"

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

#define WINDOW_WIDTH SCREEN_WIDTH
#define WINDOW_HEIGHT SCREEN_HEIGHT

// initialize camera position and direction
VECTOR4D cam_pos = { 0,0,-100 };
VECTOR4D cam_dir = { 0,0,0 };

// all your initialization code goes here...
VECTOR4D vscale = { .5,.5,.5 },
vpos = { 0,0,0 },
vrot = { 0,0,0 };

CAM4DV1        cam;                     // the single camera
RENDERLIST4DV1 rend_list;               // the single renderlist
POLYF4DV1      poly1;                   // our lonely polygon
VECTOR4D       poly1_pos = { 0,0,100 }; // world position of polygon


void StartUp(void)
{
	// initialize a single polygon
	poly1.state = POLY4DV1_STATE_ACTIVE;
	poly1.attr = 0;
	poly1.color = RGB_DX(0, 255, 0);

	poly1.vlist[0].x = 0;
	poly1.vlist[0].y = 50;
	poly1.vlist[0].z = 0;
	poly1.vlist[0].w = 1;

	poly1.vlist[1].x = 50;
	poly1.vlist[1].y = -50;
	poly1.vlist[1].z = 0;
	poly1.vlist[1].w = 1;

	poly1.vlist[2].x = -50;
	poly1.vlist[2].y = -50;
	poly1.vlist[2].z = 0;
	poly1.vlist[2].w = 1;

	poly1.next = poly1.prev = NULL;

	// initialize the camera with 90 FOV, normalized coordinates
	cam.set(
		CAM_MODEL_EULER, // euler camera model
		cam_pos,  // initial camera position
		cam_dir,  // initial camera angles
		NULL,      // no initial target
		50.0,      // near and far clipping planes
		500.0,
		90.0,      // field of view in degrees
		WINDOW_WIDTH,   // size of final screen viewport
		WINDOW_HEIGHT);


	keyboard.create();

	fpsSet.set(60);
}




void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	keyboard.read();

	static MATRIX4X4 mrot; // general rotation matrix
	static float ang_y = 0;      // rotation angle

	// initialize the render list
	Reset(rend_list);

	// insert polygon into the render list
	Insert(rend_list, poly1);

	// generate rotation matrix around y axis
	BuildXYZRotation(0, ang_y, 0, mrot);

	// rotate polygon slowly
	if (++ang_y >= 360.0) ang_y = 0;

	// rotate the local coords of single polygon in renderlist
	Transform(rend_list, mrot, TRANSFORM_LOCAL_ONLY);

	// perform local/model to world transform
	ModelToWorld(rend_list, poly1_pos);

	// generate camera matrix
	BuildEuler(cam, CAM_ROT_SEQ_ZYX);

	// apply world to camera transform
	WorldToCamera(rend_list, cam);

	// apply camera to perspective transformation
	CameraToPerspective(rend_list, cam);

	// apply screen transform
	PerspectiveToScreen(rend_list, cam);

	// draw instructions
	gPrintf(0, 0, RGB(0, 255, 0), TEXT("Press ESC to exit."));

	BeginDrawOn();

	// render the polygon list
	DrawWire(rend_list);

	EndDrawOn();

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


