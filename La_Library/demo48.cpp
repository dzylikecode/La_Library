

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
#include "resource.h"

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
#define NUM_TOWERS        64 // 96
#define NUM_TANKS         32 // 24
#define TANK_SPEED        15

// create some constants for ease of access
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT_INDEX      3 // spot light index


// initialize camera position and direction
VECTOR4D  cam_pos = { 0,0,-250 };
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
LIGHTV1 lights[4];
ASTRING path = "F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo48\\";

MATV1 materials[MAX_MATERIALS];
int numMaterials = 0;

// state variables for different rendering modes and help
int wireframe_mode = -1;
int backface_mode = 1;
int lighting_mode = 1;
int help_mode = 1;
int zsort_mode = 1;

// controlled during load dialog
int swapyz = 0,
iwinding = 0,
dolocal = 1,
doworld = 1;

TCHAR ascfilename[256]; // holds file name when loader loads

BOOL CALLBACK DialogProc(HWND hwnddlg, UINT umsg, WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	TCHAR buffer[256];
	// what is the message 
	switch (msg)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
			// handle the FILE menu
		case ID_FILE_EXIT:
		{
			// terminate window
			PostQuitMessage(0);
		} break;

		case ID_FILE_LOADASCFILE:
		{
			// call line input dialog
			if (DialogBox((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc) == IDOK)
			{

				// load in default object
				vscale.set(5.00, 5.00, 5.00);

				LoadCOB(obj_player, TToA(AToT(path) + ascfilename),
					vscale, vpos, vrot,
					(swapyz * VERTEX_FLAGS_SWAP_YZ) |
					(iwinding * VERTEX_FLAGS_INVERT_WINDING_ORDER) |
					(dolocal * VERTEX_FLAGS_TRANSFORM_LOCAL) |
					(doworld * VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD),
					materials, numMaterials);
			}
		} break;

		case ID_LIGHTS_TOGGLEAMBIENTLIGHT:
		{
			// toggle ambient light
			if (lights[AMBIENT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
				lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
			else
				lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		} break;

		case ID_LIGHTS_TOGGLEPOINTLIGHT:
		{
			// toggle point light
			if (lights[POINT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
				lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
			else
				lights[POINT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		} break;

		case ID_LIGHTS_TOGGLESPOTLIGHT:
		{
			// toggle spot light
			if (lights[SPOT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
				lights[SPOT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
			else
				lights[SPOT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		} break;

		case ID_LIGHTS_TOGGLEINFINITELIGHT:
		{
			// toggle spot light
			if (lights[INFINITE_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
				lights[INFINITE_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
			else
				lights[INFINITE_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		} break;

		case ID_ZSORTING_ENABLE:
		{
			zsort_mode = 1;
		} break;

		case ID_ZSORTING_DISABLE:
		{
			zsort_mode = -1;
		} break;

		case ID_BACKFACEREMOVAL_ENABLE:
		{
			backface_mode = 1;
		} break;

		case ID_BACKFACEREMOVAL_DISABLE:
		{
			backface_mode = -1;
		} break;


		case ID_RENDERINGMODE_SOLID:
		{
			// toggle wireframe mode
			wireframe_mode = -1;
		} break;

		case ID_RENDERINGMODE_WIREFRAME:
		{
			// toggle wireframe mode
			wireframe_mode = 1;
		} break;


		// handle the HELP menu
		case ID_HELP_ABOUT:
		{
			_stprintf(buffer, TEXT("Polys: %d, Vertices: %d"), obj_player.num_polys, obj_player.num_vertices);
			//  pop up a message box
			MessageBox(hwnd, buffer,
				TEXT(".COB File Loader Demo"),
				MB_OK | MB_ICONEXCLAMATION);
		} break;

		default: break;

		} // end switch wparam

	} break; // end WM_COMMAND


	case WM_DESTROY:
	{
		// kill the application			
		PostQuitMessage(0);
		return(0);
	} break;

	default:break;

	}
	// process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}



BOOL CALLBACK DialogProc(HWND hwnddlg, UINT umsg, WPARAM wparam, LPARAM lparam)  // second message parameter
{
	// dialog handler for the line input

	// get the handle to the text edit box
	HWND htextedit = GetDlgItem(hwnddlg, IDC_EDIT1);
	HWND hswapyz = GetDlgItem(hwnddlg, IDC_SWAPYZ);
	HWND hiwinding = GetDlgItem(hwnddlg, IDC_IWINDING);
	HWND hlocal = GetDlgItem(hwnddlg, IDC_LOCAL);
	HWND hworld = GetDlgItem(hwnddlg, IDC_WORLD);


	switch (umsg)
	{
	case WM_INITDIALOG:
	{
		SetFocus(htextedit);
	} break;

	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
		case IDOK:
		{
			int linelength = SendMessage(htextedit, EM_LINELENGTH, 0, 0);
			ascfilename[0] = (unsigned char)255;
			SendMessage(htextedit, EM_GETLINE, 0, (LPARAM)(LPCSTR)ascfilename);
			ascfilename[linelength] = 0;

			// set swap and winding order globals
			swapyz = (int)SendMessage(hswapyz, BM_GETCHECK, 0, 0);
			iwinding = (int)SendMessage(hiwinding, BM_GETCHECK, 0, 0);
			dolocal = (int)SendMessage(hlocal, BM_GETCHECK, 0, 0);
			doworld = (int)SendMessage(hworld, BM_GETCHECK, 0, 0);

			EndDialog(hwnddlg, IDOK);
			return TRUE;
		} break;

		case IDCANCEL:
		{
			EndDialog(hwnddlg, IDCANCEL);
			return TRUE;
		} break;

		default: break;
		}

	} break;
	}
	return 0;
}



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

	LoadCOB(obj_player, path + "sphere01.cob",
		VECTOR4D(5.00, 5.00, 5.00), vpos, vrot,
		VERTEX_FLAGS_SWAP_YZ |
		VERTEX_FLAGS_TRANSFORM_LOCAL | VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD,
		materials, numMaterials);

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
	lights[SPOT_LIGHT_INDEX].set(
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
	char work_string[256]; // temp string

	// game logic here...
	// draw the sky
	// reset the render list
	Reset(rend_list);

	// allow user to move camera

	// forward/backward
	if (keyboard[DIK_UP])
	{
		// move forward
		cam.pos.x += 5 * SinFast(cam.dir.y);
		cam.pos.z += 5 * CosFast(cam.dir.y);
	}

	if (keyboard[DIK_DOWN])
	{
		// move backward
		cam.pos.x -= 5 * SinFast(cam.dir.y);
		cam.pos.z -= 5 * CosFast(cam.dir.y);
	}

	// rotate object
	if (keyboard[DIK_RIGHT])
	{
		obj_player.dir.y += 2;
	}

	if (keyboard[DIK_LEFT])
	{
		obj_player.dir.y -= 2;
	}

	if (obj_player.dir.y > 360)
		obj_player.dir.y -= 360;

	if (obj_player.dir.y < 0)
		obj_player.dir.y += 360;

	// ambient rotation
	obj_player.dir.y += 2;


	// scale object
	if (keyboard[DIK_PGUP])
	{
		Scale(obj_player, VECTOR4D(1.1, 1.1, 1.1));
	}

	if (keyboard[DIK_PGDN])
	{
		Scale(obj_player, VECTOR4D(.9, .9, .9));
	}

	// modes and lights

	// wireframe mode
	if (keyboard[DIK_W])
	{
		// toggle wireframe mode
		wireframe_mode = -wireframe_mode;
		Sleep(100); // wait, so keyboard doesn't bounce
	}

	// backface removal
	if (keyboard[DIK_B])
	{
		// toggle backface removal
		backface_mode = -backface_mode;
		Sleep(100); // wait, so keyboard doesn't bounce
	}

	// lighting
	if (keyboard[DIK_L])
	{
		// toggle lighting engine completely
		lighting_mode = -lighting_mode;
		Sleep(100); // wait, so keyboard doesn't bounce
	}

	// toggle ambient light
	if (keyboard[DIK_A])
	{
		// toggle ambient light
		if (lights[AMBIENT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[AMBIENT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

		Sleep(100); // wait, so keyboard doesn't bounce
	}

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
		if (lights[SPOT_LIGHT_INDEX].state == LIGHTV1_STATE_ON)
			lights[SPOT_LIGHT_INDEX].state = LIGHTV1_STATE_OFF;
		else
			lights[SPOT_LIGHT_INDEX].state = LIGHTV1_STATE_ON;

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
	lights[POINT_LIGHT_INDEX].pos.x = 300 * CosFast(plight_ang);
	lights[POINT_LIGHT_INDEX].pos.y = 200;
	lights[POINT_LIGHT_INDEX].pos.z = 300 * SinFast(plight_ang);

	if ((plight_ang += 3) > 360)
		plight_ang = 0;

	// move spot light source in ellipse around game world
	lights[SPOT_LIGHT_INDEX].pos.x = 200 * CosFast(slight_ang);
	lights[SPOT_LIGHT_INDEX].pos.y = 200;
	lights[SPOT_LIGHT_INDEX].pos.z = 200 * SinFast(slight_ang);

	if ((slight_ang -= 5) < 0)
		slight_ang = 360;


	/////////////////////////////////////////

	// generate camera matrix
	BuildEuler(cam, CAM_ROT_SEQ_ZYX);

	// insert the player into the world
	// reset the object (this only matters for backface and object removal)
	Reset(obj_player);

	// generate rotation matrix around y axis
	BuildXYZRotation(0, obj_player.dir.y, 0, mrot);

	// rotate the local coords of the object
	Transform(obj_player, mrot, TRANSFORM_LOCAL_TO_TRANS);

	// perform world transform
	ModelToWorld(obj_player, TRANSFORM_TRANS_ONLY);


	// perform lighting
	if (lighting_mode == 1)
		LightWorld(obj_player, cam, lights, 4);

	// insert the object into render list
	Insert2(rend_list, obj_player, 0, lighting_mode);

	// insert the object into render list
	//Insert_OBJECT4DV1_RENDERLIST4DV12(&rend_list, &obj_player,0, 0);

	//////////////////////////////////////

	// remove backfaces
	if (backface_mode == 1)
		RemoveBackfaces(rend_list, cam);

	// light scene all at once 
	//if (lighting_mode==1)
	//   Light_RENDERLIST4DV1_World16(&rend_list, &cam, lights, 4);

	// apply world to camera transform
	WorldToCamera(rend_list, cam);

	// sort the polygon list (hurry up!)
	if (zsort_mode == 1)
		Sort(rend_list, SORT_POLYLIST_AVGZ);

	// apply camera to perspective transformation
	CameraToPerspective(rend_list, cam);

	// apply screen transform
	PerspectiveToScreen(rend_list, cam);

	gPrintf(0, WINDOW_HEIGHT - 20, RGB(0, 255, 0),
		TEXT("pos:[%f, %f, %f] heading:[%f] elev:[%f]"),
		cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.y, cam.dir.x);

	gPrintf(0, WINDOW_HEIGHT - 34, RGB(0, 255, 0),
		TEXT("Lighting [%s]: Ambient=%d, Infinite=%d, Point=%d, Spot=%d | Zsort [%s], BckFceRM [%s]"),
		((lighting_mode == 1) ? TEXT("ON") : TEXT("OFF")),
		lights[AMBIENT_LIGHT_INDEX].state,
		lights[INFINITE_LIGHT_INDEX].state,
		lights[POINT_LIGHT_INDEX].state,
		lights[SPOT_LIGHT_INDEX].state,
		((zsort_mode == 1) ? TEXT("ON") : TEXT("OFF")),
		((backface_mode == 1) ? TEXT("ON") : TEXT("OFF")));

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

	}

	BeginDrawOn();

	// render the object
	if (wireframe_mode == 1)
		DrawWire(rend_list);
	else
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
	gameBox.setWndProc(WindowProc);
	gameBox.setClassStyle(CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("¼üÅÌ¿ØÖÆÒÆ¶¯"));
	gameBox.attachMenu(IDR_ASCMENU);
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


