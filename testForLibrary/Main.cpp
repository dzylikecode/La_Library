

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


// create some constants for ease of access
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT1_INDEX     4 // point light index
#define SPOT_LIGHT2_INDEX     3 // spot light index


// initialize camera position and direction
VECTOR4D  cam_pos = { 0,0,0 };
VECTOR4D  cam_target = { 0,0,0 };
VECTOR4D cam_dir = { 0,0,0 };

// all your initialization code goes here...
VECTOR4D vscale = { 1.0,1.0,1.0 },
vpos = { 0,0,0 },
vrot = { 0,0,0 };

CAM4DV1        cam;       // the single camera

OBJECT4DV2     obj_flat_cube;

RENDERLIST4DV2 rend_list; // the render list

LIGHTV1 lights[5];
ASTRING path = "F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo51\\";

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
	// load constant shaded water
	vscale.set(20.00, 20.00, 20.00);
	LoadCOB(obj_flat_cube, path + "cube_flat_textured_01.cob",
		vscale, vpos, vrot, VERTEX_FLAGS_SWAP_YZ |
		VERTEX_FLAGS_TRANSFORM_LOCAL |
		VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD,materials, numMaterials);

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

	// game logic here...
	// draw the sky
	// reset the render list
	// reset the render list
	Reset(rend_list);

	// modes and lights

	// wireframe mode
	if (keyboard[DIK_W])
	{
		// toggle wireframe mode
		if (++wireframe_mode > 1)
			wireframe_mode = 0;
		Sleep(100); // wait, so keyboard doesn't bounce
	} 

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
	} 
 // toggle point light
	if (keyboard[DIK_P])
	{
		// toggle point light
		if (lights[POINT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		Sleep(100); // wait, so keyboard doesn't bounce
	} 


 // toggle spot light
	if (keyboard[DIK_S])
	{
		// toggle spot light
		if (lights[SPOT_LIGHT2_INDEX].state == LIGHTV1_STATE_ON)
			lights[SPOT_LIGHT2_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[SPOT_LIGHT2_INDEX].state = LIGHTV1_STATE_ON;

		Sleep(100); // wait, so keyboard doesn't bounce
	} 


 // help menu
	if (keyboard[DIK_H])
	{
		// toggle help menu 
		help_mode = -help_mode;
		Sleep(100); // wait, so keyboard doesn't bounce
	} 

 // z-sorting
	if (keyboard[DIK_Z])
	{
		// toggle z sorting
		zsort_mode = -zsort_mode;
		Sleep(100); // wait, so keyboard doesn't bounce
	} 

	static float plight_ang = 0, slight_ang = 0; // angles for light motion

	// move point light source in ellipse around game world
	lights[POINT_LIGHT_INDEX].pos.x = 1000 * CosFast(plight_ang);
	lights[POINT_LIGHT_INDEX].pos.y = 100;
	lights[POINT_LIGHT_INDEX].pos.z = 1000 * SinFast(plight_ang);

	if ((plight_ang += 3) > 360)
		plight_ang = 0;

	// move spot light source in ellipse around game world
	lights[SPOT_LIGHT2_INDEX].pos.x = 1000 * CosFast(slight_ang);
	lights[SPOT_LIGHT2_INDEX].pos.y = 200;
	lights[SPOT_LIGHT2_INDEX].pos.z = 1000 * SinFast(slight_ang);

	if ((slight_ang -= 5) < 0)
		slight_ang = 360;

	// generate camera matrix
	BuildEuler(cam, CAM_ROT_SEQ_ZYX);

	// use these to rotate objects
	static float x_ang = 0, y_ang = 0, z_ang = 0;

	//////////////////////////////////////////////////////////////////////////
	// constant shaded water

	// reset the object (this only matters for backface and object removal)
	Reset(obj_flat_cube);

	// set position of constant shaded cube
	obj_flat_cube.world_pos.x = 0;
	obj_flat_cube.world_pos.y = 0;
	obj_flat_cube.world_pos.z = 150;

	// generate rotation matrix around y axis
	BuildXYZRotation(x_ang, y_ang, z_ang, mrot);

	// rotate the local coords of the object
	Transform(obj_flat_cube, mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

	// perform world transform
	ModelToWorld(obj_flat_cube, TRANSFORM_TRANS_ONLY);

	// insert the object into render list
	Insert(rend_list, obj_flat_cube, 0);

	// update rotation angles
	if ((x_ang += 1) > 360) x_ang = 0;
	if ((y_ang += 2) > 360) y_ang = 0;
	if ((z_ang += 3) > 360) z_ang = 0;

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

	sprintf(work_string, "Lighting [%s]: Ambient=%d, Infinite=%d, Point=%d, Spot=%d | Zsort [%s], BckFceRM [%s]",
		((lighting_mode == 1) ? "ON" : "OFF"),
		lights[AMBIENT_LIGHT_INDEX].state,
		lights[INFINITE_LIGHT_INDEX].state,
		lights[POINT_LIGHT_INDEX].state,
		lights[SPOT_LIGHT2_INDEX].state,
		((zsort_mode == 1) ? "ON" : "OFF"),
		((backface_mode == 1) ? "ON" : "OFF"));

	gPrintf(0, WINDOW_HEIGHT - 34, RGB(0, 255, 0),AToT(work_string));

	// draw instructions
	gPrintf( 0, 0, RGB(0, 255, 0), TEXT("Press ESC to exit. Press <H> for Help."));

	// should we display help
	int text_y = 16;
	if (help_mode == 1)
	{
		// draw help menu
		gPrintf( 0, text_y += 12, RGB(255, 255, 255), TEXT("<A>..............Toggle ambient light source."));
		gPrintf( 0, text_y += 12, RGB(255, 255, 255), TEXT("<I>..............Toggle infinite light source."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<P>..............Toggle point light source."));
		gPrintf( 0, text_y += 12, RGB(255, 255, 255), TEXT("<S>..............Toggle spot light source."));
		gPrintf( 0, text_y += 12, RGB(255, 255, 255), TEXT("<W>..............Toggle wire frame/solid mode."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<B>..............Toggle backface removal."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<H>..............Toggle Help."));
		gPrintf(0, text_y += 12, RGB(255, 255, 255), TEXT("<ESC>............Exit demo."));

	} 

// lock the back buffer
	BeginDrawOn();

	// render the object

	if (wireframe_mode == 0)
		DrawWire(rend_list);
	else if (wireframe_mode == 1)
		DrawSolid(rend_list);



	// unlock the back buffer
	EndDrawOn();
	
	//static int poly = 0;
	//poly %= rend_list.num_polys;
	//if (!(rend_list.poly_ptrs[poly]->state & POLY4DV2_STATE_ACTIVE) ||
	//	(rend_list.poly_ptrs[poly]->state & POLY4DV2_STATE_CLIPPED) ||
	//	(rend_list.poly_ptrs[poly]->state & POLY4DV2_STATE_BACKFACE))
	//{

	//}
	//else
	//{
	//	// need to test for textured first, since a textured poly can either
	//	// be emissive, or flat shaded, hence we need to call different
	//	// rasterizers    
	//	if (rend_list.poly_ptrs[poly]->attr & POLY4DV2_ATTR_SHADE_MODE_TEXTURE)
	//	{
	//		// assign the texture
	//		SURFACE temp;
	//		COLOR* buffer = rend_list.poly_ptrs[poly]->texture.pbuffer;
	//		int width = rend_list.poly_ptrs[poly]->texture.width;
	//		int height = rend_list.poly_ptrs[poly]->texture.height;
	//		temp.createFromBuffer(buffer, width, height);
	//		temp.drawOn(0, 0);
	//	}
	//}
	//poly++;
	

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


