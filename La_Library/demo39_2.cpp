
/*
***************************************************************************************
*  程    序:
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/08/31 9:14:54
*  最后修改:
*
*  简    介: 有背面消除
*
***************************************************************************************
*/

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
VECTOR4D cam_pos = { 0,0,0 };
VECTOR4D cam_dir = { 0,0,0 };

// all your initialization code goes here...
VECTOR4D vscale = { 5.0,5.0,5.0 },
vpos = { 0,0,0 },
vrot = { 0,0,0 };

CAM4DV1        cam;                     // the single camera
OBJECT4DV1 obj;     // used to hold our cube mesh   


void StartUp(void)
{
	TSTRING path = TEXT("F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo39\\");

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

	// load the cube
	LoadPLG(obj, path + TEXT("cube2.plg"), vpos, vscale, vrot);

	// set the position of the cube in the world
	obj.world_pos.x = 0;
	obj.world_pos.y = 0;
	obj.world_pos.z = 100;


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

	static float x_ang = 0, // these track the rotation
		y_ang = 2, // of the object
		z_ang = 0;

	// initialize the render list
	Reset(obj);

	// reset angles
	x_ang = z_ang = 0;

	// is user trying to rotate
	if (KEY_DOWN(VK_DOWN))
		x_ang = 1;
	else if (KEY_DOWN(VK_UP))
		x_ang = -1;

	// generate rotation matrix around y axis
	BuildXYZRotation(x_ang, y_ang, z_ang, mrot);

	// rotate the local coords of single polygon in renderlist
	Transform(obj, mrot, TRANSFORM_LOCAL_ONLY);

	// perform local/model to world transform
	ModelToWorld(obj);

	// generate camera matrix
	BuildEuler(cam, CAM_ROT_SEQ_ZYX);

	// remove backfaces
	RemoveBackfaces(obj, cam);

	// apply world to camera transform
	WorldToCamera(obj, cam);

	// apply camera to perspective transformation
	CameraToPerspective(obj, cam);

	// apply screen transform
	PerspectiveToScreen(obj, cam);

	// draw instructions
	gPrintf(0, 0, RGB(0, 255, 0), TEXT("Press ESC to exit."));
	gPrintf(0, 20, RGB(0, 255, 0), TEXT("Use UP ARROW and DOWN ARROW to rotate."));

	// lock the back buffer
	BeginDrawOn();

	// render the object
	DrawWire(obj);

	// unlock the back buffer
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

	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("键盘控制移动"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


