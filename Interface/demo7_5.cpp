// DEMOII7_5.CPP
// READ THIS!
// To compile make sure to include DDRAW.LIB, DSOUND.LIB,
// DINPUT.LIB, DINPUT8.LIB, WINMM.LIB in the project link list, and of course 
// the C++ source modules T3DLIB1-5.CPP and the headers T3DLIB1-5.H
// be in the working directory of the compiler

// INCLUDES ///////////////////////////////////////////////

#define INITGUID       // make sure al the COM interfaces are available
					   // instead of this you can include the .LIB file
					   // DXGUID.LIB

#define WIN32_LEAN_AND_MEAN  

#include "La_Interface.h"
#include "3D1.h"
#include "3D5.h"
#include "La_Master.h"
#include "La_Modify.h"

// DEFINES ////////////////////////////////////////////////

// defines for windows interface
#define WINDOW_CLASS_NAME "WIN3DCLASS"  // class name
#define WINDOW_TITLE      "T3D Graphics Console Ver 2.0"
#define WINDOW_WIDTH      800  // size of window
#define WINDOW_HEIGHT     600

#define WINDOW_BPP        16    // bitdepth of window (8,16,24 etc.)
								// note: if windowed and not
								// fullscreen then bitdepth must
								// be same as system bitdepth
								// also if 8-bit the a pallete
								// is created and attached

#define WINDOWED_APP      0     // 0 not windowed, 1 windowed

// object defines
#define NUM_OBJECTS     16
#define OBJECT_SPACING  250

// PROTOTYPES /////////////////////////////////////////////



// GLOBALS ////////////////////////////////////////////////

HWND main_window_handle = NULL; // save the window handle
HINSTANCE main_instance = NULL; // save the instance
char buffer[2048];                        // used to print text

// initialize camera position and direction
POINT4D  cam_pos = { 0,0,0,1 };
POINT4D  cam_target = { 0,0,0,1 };
VECTOR4D cam_dir = { 0,0,0,1 };

// all your initialization code goes here...
VECTOR4D vscale = { 1.0,1.0,1.0,1 },
vpos = { 0,0,0,1 },
vrot = { 0,0,0,1 };

CAM4DV1        cam;       // the single camera
OBJECT4DV1     obj;       // used to hold our cube mesh                   
RENDERLIST4DV1 rend_list; // the render list



// T3D II GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

void Game_Init(void)
{
	// this function is where you do all the initialization 
	// for your game

	// start up DirectDraw (replace the parms as you desire)
	DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);

	// initialize directinput
	DInput_Init();

	// acquire the keyboard 
	DInput_Init_Keyboard();

	// add calls to acquire other directinput devices here...

	// initialize directsound and directmusic
	DSound_Init();
	DMusic_Init();

	// hide the mouse
	if (!WINDOWED_APP)
		ShowCursor(FALSE);



	Open_Error_File("ERROR.TXT");

	// initialize math engine
	Build_Sin_Cos_Tables();

	// initialize the camera with 90 FOV, normalized coordinates
	Init_CAM4DV1(&cam,      // the camera object
		CAM_MODEL_UVN, // the euler model
		&cam_pos,  // initial camera position
		&cam_dir,  // initial camera angles
		&cam_target,      // no target
		50.0,      // near and far clipping planes
		8000.0,
		90.0,      // field of view in degrees
		WINDOW_WIDTH,   // size of final screen viewport
		WINDOW_HEIGHT);

	// load the object
	char path[] = "F:\\Git_WorkSpace\\La_Library\\Interface\\Resource\\demo7\\tank1.plg";
	Load_OBJECT4DV1_PLG(&obj, path, &vscale, &vpos, &vrot);

	// set the default position of the object in the world
	obj.world_pos.x = 0;
	obj.world_pos.y = 0;
	obj.world_pos.z = 0;



} // end Game_Init

//////////////////////////////////////////////////////

void Game_Main(void)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!

	static MATRIX4X4 mrot;   // general rotation matrix

	static float x_ang = 0,  // object rotation angles
		y_ang = 5,
		z_ang = 0;

	// these are used to create a circling camera
	static float view_angle = 0;
	static float camera_distance = 1750;
	static VECTOR4D pos = { 0,0,0,0 };

	char work_string[256]; // temp string

	int index; // looping var

	// start the timing clock
	Start_Clock();

	// clear the drawing surface 
	DDraw_Fill_Surface(lpddsback, 0);

	// read keyboard and other devices here
	DInput_Read_Keyboard();

	// game logic here...

	// reset the render list
	Reset_RENDERLIST4DV1(&rend_list);

	// reset angles
	x_ang = 0;
	y_ang = 1;
	z_ang = 0;

	// generate rotation matrix around y axis
	Build_XYZ_Rotation_MATRIX4X4(x_ang, y_ang, z_ang, &mrot);

	// rotate the local coords of the object
	Transform_OBJECT4DV1(&obj, &mrot, TRANSFORM_LOCAL_ONLY, 1);

	// compute new camera position, rides on a circular path 
	// with a sinudoid modulation
	cam.pos.x = camera_distance * Fast_Cos(view_angle);
	cam.pos.y = camera_distance * Fast_Sin(view_angle);
	cam.pos.z = 2 * camera_distance * Fast_Sin(view_angle);

	// advance angle to circle around
	if ((view_angle += 1) >= 360)
		view_angle = 0;

	// generate camera matrix
	Build_CAM4DV1_Matrix_UVN(&cam, UVN_MODE_SIMPLE);

	// now cull the current object
	strcpy(buffer, "Objects Culled: ");

	for (int x = -NUM_OBJECTS / 2; x < NUM_OBJECTS / 2; x++)
		for (int z = -NUM_OBJECTS / 2; z < NUM_OBJECTS / 2; z++)
		{
			// reset the object (this only matters for backface and object removal)
			Reset_OBJECT4DV1(&obj);

			// set position of object
			obj.world_pos.x = x * OBJECT_SPACING + OBJECT_SPACING / 2;
			obj.world_pos.y = 0;
			obj.world_pos.z = z * OBJECT_SPACING + OBJECT_SPACING / 2;

			// attempt to cull object   
			if (!Cull_OBJECT4DV1(&obj, &cam, CULL_OBJECT_XYZ_PLANES))
			{
				// if we get here then the object is visible at this world position
				// so we can insert it into the rendering list
				// perform local/model to world transform
				Model_To_World_OBJECT4DV1(&obj);

				// insert the object into render list
				Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &obj);
			} // end if
			else
			{
				sprintf(work_string, "[%d, %d] ", x, z);
				strcat(buffer, work_string);
			}

		} // end for

	Draw_Text_GDI(buffer, 0, WINDOW_HEIGHT - 20, RGB(0, 255, 0), lpddsback);

	// Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

	// remove backfaces
	Remove_Backfaces_RENDERLIST4DV1(&rend_list, &cam);

	// apply world to camera transform
	World_To_Camera_RENDERLIST4DV1(&rend_list, &cam);

	// apply camera to perspective transformation
	Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);

	// apply screen transform
	Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);

	// draw instructions
	Draw_Text_GDI("Press ESC to exit.", 0, 0, RGB(0, 255, 0), lpddsback);

	// lock the back buffer
	DDraw_Lock_Back_Surface();

	// render the object
	Draw_RENDERLIST4DV1_Wire16(&rend_list, back_buffer, back_lpitch);

	// unlock the back buffer
	DDraw_Unlock_Back_Surface();

	// flip the surfaces
	DDraw_Flip();

	// sync to 30ish fps
	Wait_Clock(30);

	// check of user is trying to exit
	if (KEY_DOWN(VK_ESCAPE))
	{
		gameBox.exitFromGameBody();

	} // end if



} // end Game_Main


int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);

	gameBox.create(WINDOW_WIDTH, WINDOW_HEIGHT, TEXT("���̿����ƶ�"));
	gameBox.setGameStart(Game_Init);
	gameBox.setGameBody(Game_Main);
	gameBox.startCommuication();
	return argc;
}
//////////////////////////////////////////////////////////