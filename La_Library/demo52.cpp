

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
#include "La_3D.h"
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

#define POINT_SIZE        100
#define NUM_POINTS_X      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS        (NUM_POINTS_X*NUM_POINTS_Z)

// defines for objects 
#define NUM_TOWERS        96 
#define NUM_TANKS         32 
#define TANK_SPEED        15

// create some constants for ease of access
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT1_INDEX     4 // point light index
#define SPOT_LIGHT2_INDEX     3 // spot light index

// initialize camera position and direction
VECTOR4D  cam_pos = { 0,40	,0 };
VECTOR4D  cam_target = { 0,0,0 };
VECTOR4D cam_dir = { 0,0,0 };

// all your initialization code goes here...
VECTOR4D vscale = { 1.0,1.0,1.0 },
vpos = { 0,0,0 },
vrot = { 0,0,0 };

CAM4DV1        cam;       // the single camera

OBJECT4DV2     obj_tower,    // used to hold the master tower
obj_tank,     // used to hold the master tank
obj_marker,   // the ground marker
obj_player;   // the player object     

VECTOR4D  towers[NUM_TOWERS],
tanks[NUM_TANKS];

IMAGE textures[12]; // holds our texture library 


RENDERLIST4DV2 rend_list; // the render list

LIGHTV1 lights[5];
ASTRING path = "F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo52\\";

MATV1 materials[MAX_MATERIALS];
int numMaterials = 0;
RGBAV1 white, gray, black, red, green, blue;

TCHAR ascfilename[256]; // holds file name when loader loads

void StartUp(void)
{
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

	SetTexturePath(path);

	// load the master tank object
	vscale.set(0.75, 0.75, 0.75);
	LoadPLG(obj_tank, path + "tank3.plg", vscale, vpos, vrot);

	// load player object for 3rd person view
	vscale.set(15.75, 15.75, 15.75);
	LoadCOB(obj_player, path + "tie04.cob",
		vscale, vpos, vrot, VERTEX_FLAGS_INVERT_TEXTURE_V |
		VERTEX_FLAGS_SWAP_YZ | VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD,
		materials, numMaterials);

	// load the master tower object
	vscale.set(1.0, 2.0, 1.0);
	LoadPLG(obj_tower, path + "towerg1.plg", vscale, vpos, vrot);

	// load the master ground marker
	vscale.set(3.0, 3.0, 3.0);
	LoadPLG(obj_marker, path + "marker2.plg", vscale, vpos, vrot);

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

	// create some working colors
	RGBAV1 white, gray, black, red, green, blue;

	white.rgba = RGBA(255, 255, 255, 0);
	gray.rgba = RGBA(100, 100, 100, 0);
	black.rgba = RGBA(0, 0, 0, 0);
	red.rgba = RGBA(255, 0, 0, 0);
	green.rgba = RGBA(0, 255, 0, 0);
	blue.rgba = RGBA(0, 0, 255, 0);

	// ambient light
	lights[AMBIENT_LIGHT_INDEX].set(
		LIGHTV1_STATE_ON,      // turn the light on
		LIGHTV1_ATTR_AMBIENT,  // ambient light type
		gray, black, black,    // color for ambient term only
		VECTOR4D(0, 0, 0), VECTOR4D(0, 0, 0),   // no need for pos or dir
		0, 0, 0,                 // no need for attenuation
		0, 0, 0);                // spotlight info NA


	VECTOR4D dlight_dir = { -1,0,-1,0 };

	// directional light
	lights[INFINITE_LIGHT_INDEX].set(
		LIGHTV1_STATE_ON,      // turn the light on
		LIGHTV1_ATTR_INFINITE, // infinite light type
		black, gray, black,    // color for diffuse term only
		VECTOR4D(0, 0, 0), dlight_dir,     // need direction only
		0, 0, 0,                 // no need for attenuation
		0, 0, 0);                // spotlight info NA


	VECTOR4D plight_pos = { 0,200,0,0 };

	// point light
	lights[POINT_LIGHT_INDEX].set(
		LIGHTV1_STATE_ON,      // turn the light on
		LIGHTV1_ATTR_POINT,    // pointlight type
		black, green, black,   // color for diffuse term only
		plight_pos, VECTOR4D(0, 0, 0),     // need pos only
		0, .001, 0,              // linear attenuation only
		0, 0, 1);                // spotlight info NA

	VECTOR4D slight_pos = { 0,200,0,0 };
	VECTOR4D slight_dir = { -1,0,-1,0 };

	// spot light
	lights[SPOT_LIGHT2_INDEX].set(
		LIGHTV1_STATE_ON,         // turn the light on
		LIGHTV1_ATTR_SPOTLIGHT2,  // spot light type 2
		black, red, black,      // color for diffuse term only
		slight_pos, slight_dir, // need pos only
		0, .001, 0,                 // linear attenuation only
		0, 0, 1);			          // spotlight powerfactor only

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
	static VECTOR4D pos = { 0,0,0,0 };
	static float tank_speed;
	static float turning = 0;
	// state variables for different rendering modes and help
	static int wireframe_mode = 1;
	static int backface_mode = 1;
	static int lighting_mode = 1;
	static int help_mode = 1;
	static int zsort_mode = 1;

	char work_string[256]; // temp string

	DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2, RGB_DX(0, 35, 50));

	DrawRectangle(0, WINDOW_HEIGHT / 2 - 1, WINDOW_WIDTH, WINDOW_HEIGHT, RGB_DX(20, 12, 0));

	// game logic here...
	// draw the sky
	// reset the render list
	Reset(rend_list);

	// allow user to move camera

	// turbo
	if (keyboard[DIK_SPACE])
		tank_speed = 5 * TANK_SPEED;
	else
		tank_speed = TANK_SPEED;

	// forward/backward
	if (keyboard[DIK_UP])
	{
		// move forward
		cam.pos.x += tank_speed * SinFast(cam.dir.y);
		cam.pos.z += tank_speed * CosFast(cam.dir.y);
	} // end if

	if (keyboard[DIK_DOWN])
	{
		// move backward
		cam.pos.x -= tank_speed * SinFast(cam.dir.y);
		cam.pos.z -= tank_speed * CosFast(cam.dir.y);
	} // end if

 // rotate
	if (keyboard[DIK_RIGHT])
	{
		cam.dir.y += 3;

		// add a little turn to object
		if ((turning += 2) > 25)
			turning = 25;
	} // end if

	if (keyboard[DIK_LEFT])
	{
		cam.dir.y -= 3;

		// add a little turn to object
		if ((turning -= 2) < -25)
			turning = -25;

	}
	else // center heading again
	{
		if (turning > 0)
			turning -= 1;
		else
			if (turning < 0)
				turning += 1;

	}

	// modes and lights

	// wireframe mode
	if (keyboard[DIK_W])
	{
		// toggle wireframe mode
		if (++wireframe_mode > 1)
			wireframe_mode = 0;
		Sleep(100); // wait, so keyboard doesn't bounce
	} // end if

 // backface removal
	if (keyboard[DIK_B])
	{
		// toggle backface removal
		backface_mode = -backface_mode;
		Sleep(100); // wait, so keyboard doesn't bounce
	} // end if

 // lighting
	if (keyboard[DIK_L])
	{
		// toggle lighting engine completely
		lighting_mode = -lighting_mode;
		Sleep(100); // wait, so keyboard doesn't bounce
	} // end if

 // toggle ambient light
	if (keyboard[DIK_A])
	{
		// toggle ambient light
		if (lights[AMBIENT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		Sleep(100); // wait, so keyboard doesn't bounce
	} // end if

 // toggle infinite light
	if (keyboard[DIK_I])
	{
		// toggle ambient light
		if (lights[INFINITE_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[INFINITE_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[INFINITE_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		Sleep(100); // wait, so keyboard doesn't bounce
	} // end if

 // toggle point light
	if (keyboard[DIK_P])
	{
		// toggle point light
		if (lights[POINT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		Sleep(100); // wait, so keyboard doesn't bounce
	} // end if


 // toggle spot light
	if (keyboard[DIK_S])
	{
		// toggle spot light
		if (lights[SPOT_LIGHT2_INDEX].state == LIGHTV1_STATE_ON)
			lights[SPOT_LIGHT2_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[SPOT_LIGHT2_INDEX].state = LIGHTV1_STATE_ON;

		Sleep(100); // wait, so keyboard doesn't bounce
	} // end if


 // help menu
	if (keyboard[DIK_H])
	{
		// toggle help menu 
		help_mode = -help_mode;
		Sleep(100); // wait, so keyboard doesn't bounce
	} // end if

 // z-sorting
	if (keyboard[DIK_Z])
	{
		// toggle z sorting
		zsort_mode = -zsort_mode;
		Sleep(100); // wait, so keyboard doesn't bounce
	} // end if

	static float plight_ang = 0, slight_ang = 0; // angles for light motion

	// move point light source in ellipse around game world
	lights[POINT_LIGHT_INDEX].pos.x = 4000 * CosFast(plight_ang);
	lights[POINT_LIGHT_INDEX].pos.y = 200;
	lights[POINT_LIGHT_INDEX].pos.z = 4000 * SinFast(plight_ang);

	if ((plight_ang += 3) > 360)
		plight_ang = 0;

	// move spot light source in ellipse around game world
	lights[SPOT_LIGHT2_INDEX].pos.x = 2000 * CosFast(slight_ang);
	lights[SPOT_LIGHT2_INDEX].pos.y = 200;
	lights[SPOT_LIGHT2_INDEX].pos.z = 2000 * SinFast(slight_ang);

	if ((slight_ang -= 5) < 0)
		slight_ang = 360;

	// generate camera matrix
	BuildEuler(cam, CAM_ROT_SEQ_ZYX);

	// insert the player into the world
	// reset the object (this only matters for backface and object removal)
	Reset(obj_player);

	// set position of tank

	obj_player.world_pos.x = cam.pos.x + 300 * SinFast(cam.dir.y);
	obj_player.world_pos.y = cam.pos.y - 70;
	obj_player.world_pos.z = cam.pos.z + 300 * CosFast(cam.dir.y);


	// generate rotation matrix around y axis
	static int turn = 0;
	BuildXYZRotation(1, cam.dir.y + turning, 2, mrot);

	// rotate the local coords of the object
	Transform(obj_player, mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

	// perform world transform
	ModelToWorld(obj_player, TRANSFORM_TRANS_ONLY);

	//Light_OBJECT4DV2_World16(&obj_player, &cam, lights, 4);

	// insert the object into render list
	Insert(rend_list, obj_player, 0);



	//////////////////////////////////////////////////////////

	// insert the tanks in the world
	for (int index = 0; index < NUM_TANKS; index++)
	{
		// reset the object (this only matters for backface and object removal)
		Reset(obj_tank);

		// generate rotation matrix around y axis
		BuildXYZRotation(0, tanks[index].w, 0, mrot);

		// rotate the local coords of the object
		Transform(obj_tank, mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

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

			//Light_OBJECT4DV2_World16(&obj_tank, &cam, lights, 4);

			// insert the object into render list
			Insert(rend_list, obj_tank, 0);
		} // end if

	} // end for

////////////////////////////////////////////////////////


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

			//Light_OBJECT4DV2_World16(&obj_tower, &cam, lights, 4);

			// insert the object into render list
			Insert(rend_list, obj_tower, 0);

		} // end if

	} // end for

///////////////////////////////////////////////////////////////


	static int mcount = 0, mdir = 2;

	mcount += mdir;
	if (mcount > 200 || mcount < -200) { mdir = -mdir; mcount += mdir; }

	// insert the ground markers into the world
	for (int index_x = 0; index_x < NUM_POINTS_X; index_x++)
		for (int index_z = 0; index_z < NUM_POINTS_Z; index_z++)
		{
			// reset the object (this only matters for backface and object removal)
			Reset(obj_marker);

			// set position of tower
			obj_marker.world_pos.x = RAND_RANGE(-10, 10) - UNIVERSE_RADIUS + index_x * POINT_SIZE;
			obj_marker.world_pos.y = obj_marker.max_radius[0] + 50 * SinFast(index_x * 10 + SinFast(index_z) + mcount);
			obj_marker.world_pos.z = RAND_RANGE(-10, 10) - UNIVERSE_RADIUS + index_z * POINT_SIZE;

			// attempt to cull object   
			if (!Cull(obj_marker, cam, CULL_OBJECT_XYZ_PLANES))
			{
				// if we get here then the object is visible at this world position
				// so we can insert it into the rendering list
				// perform local/model to world transform
				ModelToWorld(obj_marker);

				//Light_OBJECT4DV2_World16(&obj_marker, &cam, lights, 4);

				// insert the object into render list
				Insert(rend_list, obj_marker, 0);

			} // end if

		} // end for



// remove backfaces
	if (backface_mode == 1)
		RemoveBackfaces(rend_list, cam);

	// light scene all at once 
	if (lighting_mode == 1)
		LightWorld(rend_list, cam, lights, 4);

	// apply world to camera transform
	WorldToCamera(rend_list, cam);

	// sort the polygon list (hurry up!)
	if (zsort_mode == 1)
		Sort(rend_list, SORT_POLYLIST_AVGZ);

	// apply camera to perspective transformation
	CameraToPerspective(rend_list, cam);

	// apply screen transform
	PerspectiveToScreen(rend_list, cam);

	sprintf(work_string, "pos:[%f, %f, %f] heading:[%f] elev:[%f]",
		cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.y, cam.dir.x);

	gPrintf(0, WINDOW_HEIGHT - 20, RGB(0, 255, 0), AToT(work_string));

	sprintf(work_string, "Lighting [%s]: Ambient=%d, Infinite=%d, Point=%d, Spot=%d | Zsort [%s], BckFceRM [%s]",
		((lighting_mode == 1) ? "ON" : "OFF"),
		lights[AMBIENT_LIGHT_INDEX].state,
		lights[INFINITE_LIGHT_INDEX].state,
		lights[POINT_LIGHT_INDEX].state,
		lights[SPOT_LIGHT2_INDEX].state,
		((zsort_mode == 1) ? "ON" : "OFF"),
		((backface_mode == 1) ? "ON" : "OFF"));

	gPrintf(0, WINDOW_HEIGHT - 34, RGB(0, 255, 0), AToT(work_string));

	// draw instructions
	gPrintf(0, 0, RGB(0, 255, 0), TEXT("Press ESC to exit. Press <H> for Help."));

	// should we display help
	int text_y = 16;
	if (help_mode == 1)
	{
		// draw help menu
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<A>..............Toggle ambient light source."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<I>..............Toggle infinite light source."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<P>..............Toggle point light source."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<S>..............Toggle spot light source."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<W>..............Toggle wire frame/solid mode."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<B>..............Toggle backface removal."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<Z>..............Toggle sort."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<RIGHT ARROW>....Rotate object right."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<LEFT ARROW>.....Rotate object left."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<UP ARROW>.......Move camera forward."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<DOWN ARROW>.....Move camera backward."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<PG UP>..........Scale object down"));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<PG DWN>.........Scale object down"));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<SPACE BAR>......Turbo."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<H>..............Toggle Help."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<ESC>............Exit demo."));

	} // end help

// lock the back buffer
	BeginDrawOn();

	// render the object

	if (wireframe_mode == 0)
		DrawWire(rend_list);
	else if (wireframe_mode == 1)
		DrawSolid(rend_list);


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
	//gameBox.setWndProc(WindowProc);
	//gameBox.setClassStyle(CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("¼üÅÌ¿ØÖÆÒÆ¶¯"));
	//gameBox.attachMenu(IDR_ASCMENU);
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


