
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


// defines for the game universe
#define UNIVERSE_RADIUS   4000

#define POINT_SIZE        200
#define NUM_POINTS_X      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS        (NUM_POINTS_X*NUM_POINTS_Z)

// defines for objects
#define NUM_TOWERS        96
#define NUM_TANKS         24
#define TANK_SPEED        15


// initialize camera position and direction
VECTOR4D  cam_pos = { 0,40,0 };
VECTOR4D  cam_target = { 0,0,0 };
VECTOR4D cam_dir = { 0,0,0 };

// all your initialization code goes here...
VECTOR4D vscale = { 1.0,1.0,1.0 },
vpos = { 0,0,0 },
vrot = { 0,0,0 };

CAM4DV1        cam;       // the single camera

OBJECT4DV1     obj_tower,    // used to hold the master tower
obj_tank,     // used to hold the master tank
obj_marker,   // the ground marker
obj_player;   // the player object             

VECTOR4D     towers[NUM_TOWERS],
tanks[NUM_TANKS];

RENDERLIST4DV1 rend_list; // the render list

void StartUp(void)
{
	TSTRING path = TEXT("F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo41\\");

	// initialize the camera with 90 FOV, normalized coordinates
	cam.set(
		CAM_MODEL_EULER, // the euler model
		cam_pos,  // initial camera position
		cam_dir,  // initial camera angles
		&cam_target,      // no target
		200.0,      // near and far clipping planes
		12000.0,
		120.0,      // field of view in degrees
		WINDOW_WIDTH,   // size of final screen viewport
		WINDOW_HEIGHT);

	LoadPLG(obj_tank, path + TEXT("tank2.plg"), vpos, VECTOR4D(0.75, 0.75, 0.75), vrot);
	LoadPLG(obj_player, path + TEXT("tank2.plg"), vpos, VECTOR4D(0.75, 0.75, 0.75), vrot);


	LoadPLG(obj_tower, path + TEXT("tower1.plg"), vpos, VECTOR4D(1.0, 2.0, 1.0), vrot);
	LoadPLG(obj_marker, path + TEXT("marker1.plg"), vpos, VECTOR4D(3.0, 3.0, 3.0), vrot);

	// position the tanks
	for (int index = 0; index < NUM_TANKS; index++)
	{
		// randomly position the tanks
		tanks[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].y = 0; // obj_tank.max_radius;
		tanks[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].w = RAND_RANGE(0, 360);
	}


	// position the towers
	for (int index = 0; index < NUM_TOWERS; index++)
	{
		// randomly position the tower
		towers[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		towers[index].y = 0; // obj_tower.max_radius;
		towers[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
	}


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

	// these are used to create a circling camera
	static float view_angle = 0;
	static float camera_distance = 6000;
	static VECTOR4D pos = { 0,0,0 };
	static float tank_speed;
	static float turning = 0;

	char work_string[256]; // temp string

	// game logic here...
	// draw the sky
	DrawRectangle(0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT / 2, RGB_DX(0, 140, 192));

	// draw the ground
	DrawRectangle(0, WINDOW_HEIGHT / 2, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, RGB_DX(103, 62, 3));

	// reset the render list
	Reset(rend_list);

	// allow user to move camera

	// turbo
	if (keyboard[DIK_SPACE]) tank_speed = 5 * TANK_SPEED;
	else tank_speed = TANK_SPEED;

	// forward/backward
	if (keyboard[DIK_UP])
	{
		// move forward
		cam.pos.x += tank_speed * SinFast(cam.dir.y);
		cam.pos.z += tank_speed * CosFast(cam.dir.y);
	}

	if (keyboard[DIK_DOWN])
	{
		// move backward
		cam.pos.x -= tank_speed * SinFast(cam.dir.y);
		cam.pos.z -= tank_speed * CosFast(cam.dir.y);
	}

	// rotate
	if (keyboard[DIK_RIGHT])
	{
		cam.dir.y += 3;

		// add a little turn to object
		if ((turning += 2) > 15)
			turning = 15;

	}

	if (keyboard[DIK_LEFT])
	{
		cam.dir.y -= 3;

		// add a little turn to object
		if ((turning -= 2) < -15)
			turning = -15;

	}
	else // center heading again
	{
		if (turning > 0)
			turning -= 1;
		else if (turning < 0)
			turning += 1;

	}

	// generate camera matrix
	BuildEuler(cam, CAM_ROT_SEQ_ZYX);

	// insert the tanks in the world
	for (int index = 0; index < NUM_TANKS; index++)
	{
		// reset the object (this only matters for backface and object removal)
		Reset(obj_tank);

		// generate rotation matrix around y axis
		BuildXYZRotation(0, tanks[index].w, 0, mrot);

		// rotate the local coords of the object
		Transform(obj_tank, mrot, TRANSFORM_LOCAL_TO_TRANS);

		// set position of tank
		obj_tank.world_pos.x = tanks[index].x;
		obj_tank.world_pos.y = tanks[index].y;
		obj_tank.world_pos.z = tanks[index].z;

		// attempt to cull object   
		if (!Cull(obj_tank, cam, CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			ModelToWorld(obj_tank, TRANSFORM_TRANS_ONLY);

			// insert the object into render list
			Insert(rend_list, obj_tank);
		}
	}

	// insert the player into the world
	// reset the object (this only matters for backface and object removal)
	Reset(obj_player);

	// set position of tank
	obj_player.world_pos.x = cam.pos.x + 300 * SinFast(cam.dir.y);
	obj_player.world_pos.y = cam.pos.y - 70;
	obj_player.world_pos.z = cam.pos.z + 300 * CosFast(cam.dir.y);

	// generate rotation matrix around y axis
	BuildXYZRotation(0, cam.dir.y + turning, 0, mrot);

	// rotate the local coords of the object
	Transform(obj_player, mrot, TRANSFORM_LOCAL_TO_TRANS);

	// perform world transform
	ModelToWorld(obj_player, TRANSFORM_TRANS_ONLY);

	// insert the object into render list
	Insert(rend_list, obj_player);


	// insert the towers in the world
	for (int index = 0; index < NUM_TOWERS; index++)
	{
		// reset the object (this only matters for backface and object removal)
		Reset(obj_tower);

		// set position of tower
		obj_tower.world_pos.x = towers[index].x;
		obj_tower.world_pos.y = towers[index].y;
		obj_tower.world_pos.z = towers[index].z;

		// attempt to cull object   
		if (!Cull(obj_tower, cam, CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			ModelToWorld(obj_tower);

			// insert the object into render list
			Insert(rend_list, obj_tower);
		}

	}

	// insert the ground markers into the world
	for (int index_x = 0; index_x < NUM_POINTS_X; index_x++)
		for (int index_z = 0; index_z < NUM_POINTS_Z; index_z++)
		{
			// reset the object (this only matters for backface and object removal)
			Reset(obj_marker);

			// set position of tower
			obj_marker.world_pos.x = RAND_RANGE(-10, 10) - UNIVERSE_RADIUS + index_x * POINT_SIZE;
			obj_marker.world_pos.y = obj_marker.max_radius;
			obj_marker.world_pos.z = RAND_RANGE(-10, 10) - UNIVERSE_RADIUS + index_z * POINT_SIZE;

			// attempt to cull object   
			if (!Cull(obj_marker, cam, CULL_OBJECT_XYZ_PLANES))
			{
				// if we get here then the object is visible at this world position
				// so we can insert it into the rendering list
				// perform local/model to world transform
				ModelToWorld(obj_marker);

				// insert the object into render list
				Insert(rend_list, obj_marker);
			}
		}
	// remove backfaces
	RemoveBackfaces(rend_list, cam);

	// apply world to camera transform
	WorldToCamera(rend_list, cam);

	// apply camera to perspective transformation
	CameraToPerspective(rend_list, cam);

	// apply screen transform
	PerspectiveToScreen(rend_list, cam);

	sprintf(work_string, "pos:[%f, %f, %f] heading:[%f] elev:[%f]",
		cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.y, cam.dir.x);

	gPrintf(0, WINDOW_HEIGHT - 20, RGB(0, 255, 0), AToT(work_string));

	// draw instructions
	gPrintf(0, 0, RGB(0, 255, 0), TEXT("Press ESC to exit. Press Arrow Keys to Move. Space for TURBO."));

	BeginDrawOn();

	// render the object
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


