
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
*  简    介:
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

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

FRAMECounter fpsSet;
GAMEBox gameBox;

using namespace AUDIO;
using namespace GDI;
using namespace GRAPHIC;
using namespace INPUT_;

KEYBOARD keyboard;

#define WINDOW_WIDTH SCREEN_WIDTH
#define WINDOW_HEIGHT SCREEN_HEIGHT


// object defines
#define NUM_OBJECTS     16
#define OBJECT_SPACING  250


// initialize camera position and direction
VECTOR4D  cam_pos = { 0,0,0 };
VECTOR4D  cam_target = { 0,0,0 };
VECTOR4D cam_dir = { 0,0,0 };

// all your initialization code goes here...
VECTOR4D vscale = { 1.0,1.0,1.0 },
vpos = { 0,0,0 },
vrot = { 0,0,0 };

CAM4DV1    cam;           // the single camera
OBJECT4DV1 obj;           // used to hold our cube mesh                   
RENDERLIST4DV1 rend_list; // the render list


void StartUp(void)
{
	TSTRING path = TEXT("F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo40\\");

	// initialize the camera with 90 FOV, normalized coordinates
	cam.set(
		CAM_MODEL_UVN,
		cam_pos,  // initial camera position
		cam_dir,  // initial camera angles
		&cam_target,   // no initial target
		50.0,      // near and far clipping planes
		8000.0,
		90.0,      // field of view in degrees
		WINDOW_WIDTH,   // size of final screen viewport
		WINDOW_HEIGHT);

	// load the cube
	LoadPLG(obj, path + TEXT("tank1.plg"), vpos, vscale, vrot);

	// set the position of the cube in the world
	obj.world_pos.x = 0;
	obj.world_pos.y = 0;
	obj.world_pos.z = 0;


	keyboard.create();

	fpsSet.set(30);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	keyboard.read();

	static MATRIX4X4 mrot;   // general rotation matrix

	static float x_ang = 0,  // object rotation angles
		y_ang = 5,
		z_ang = 0;

	// these are used to create a circling camera
	static float view_angle = 0;
	static float camera_distance = 1750;
	static VECTOR4D pos = { 0,0,0 };

	char work_string[256]; // temp string

	// game logic here...

	// reset the render list
	Reset(rend_list);

	// reset angles
	x_ang = 0;
	y_ang = 1;
	z_ang = 0;

	// generate rotation matrix around y axis
	BuildXYZRotation(x_ang, y_ang, z_ang, mrot);

	// rotate the local coords of the object
	Transform(obj, mrot, TRANSFORM_LOCAL_ONLY);

	// compute new camera position, rides on a circular path 
	// with a sinudoid modulation
	cam.pos.x = camera_distance * CosFast(view_angle);
	cam.pos.y = camera_distance * SinFast(view_angle);
	cam.pos.z = 2 * camera_distance * SinFast(view_angle);

	// advance angle to circle around
	if ((view_angle += 1) >= 360)
		view_angle = 0;

	// generate camera matrix
	BuildUVN(cam, UVN_MODE_SIMPLE);
	char buffer[1024];
	// now cull the current object
	strcpy(buffer, "Objects Culled: ");

	for (int x = -NUM_OBJECTS / 2; x < NUM_OBJECTS / 2; x++)
		for (int z = -NUM_OBJECTS / 2; z < NUM_OBJECTS / 2; z++)
		{
			// reset the object (this only matters for backface and object removal)
			Reset(obj);

			// set position of object
			obj.world_pos.x = x * OBJECT_SPACING + OBJECT_SPACING / 2;
			obj.world_pos.y = 0;
			obj.world_pos.z = z * OBJECT_SPACING + OBJECT_SPACING / 2;

			// attempt to cull object   
			if (!Cull(obj, cam, CULL_OBJECT_XYZ_PLANES))
			{
				// if we get here then the object is visible at this world position
				// so we can insert it into the rendering list
				// perform local/model to world transform
				ModelToWorld(obj);

				// insert the object into render list
				Insert(rend_list, obj);
			}
			else
			{
				sprintf(work_string, "[%d, %d] ", x, z);
				strcat(buffer, work_string);
			}

		}

	gPrintf(0, WINDOW_HEIGHT - 20, RGB(0, 255, 0), AToT(buffer));

	// Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

	// remove backfaces
	RemoveBackfaces(rend_list, cam);

	// apply world to camera transform
	WorldToCamera(rend_list, cam);

	// apply camera to perspective transformation
	CameraToPerspective(rend_list, cam);

	// apply screen transform
	PerspectiveToScreen(rend_list, cam);

	// draw instructions
	gPrintf(0, 0, RGB(0, 255, 0), TEXT("Press ESC to exit."));

	BeginDrawOn();
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

	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("键盘控制移动"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


