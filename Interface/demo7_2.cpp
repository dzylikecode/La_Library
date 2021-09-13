
// DEMOII7_2.CPP
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
#define WINDOW_WIDTH      400 // size of window
#define WINDOW_HEIGHT     400

#define WINDOW_BPP        16    // bitdepth of window (8,16,24 etc.)
								// note: if windowed and not
								// fullscreen then bitdepth must
								// be same as system bitdepth
								// also if 8-bit the a pallete
								// is created and attached

#define WINDOWED_APP      1     // 0 not windowed, 1 windowed

// PROTOTYPES /////////////////////////////////////////////


// GLOBALS ////////////////////////////////////////////////

HWND main_window_handle = NULL; // save the window handle
HINSTANCE main_instance = NULL; // save the instance
char buffer[256];                         // used to print text

// initialize camera position and direction
POINT4D  cam_pos = { 0,0,0,1 };
VECTOR4D cam_dir = { 0,0,0,1 };

// all your initialization code goes here...
VECTOR4D vscale = { 5.0,5.0,5.0,1 },  // scale of object
vpos = { 0,0,0,1 },        // position of object
vrot = { 0,0,0,1 };        // initial orientation of object

CAM4DV1    cam;     // the single camera
OBJECT4DV1 obj;     // used to hold our cube mesh                   



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
		CAM_MODEL_EULER, // the euler model
		&cam_pos,  // initial camera position
		&cam_dir,  // initial camera angles
		NULL,      // no target
		50.0,      // near and far clipping planes
		500.0,
		90.0,      // field of view in degrees
		WINDOW_WIDTH,   // size of final screen viewport
		WINDOW_HEIGHT);

	// load the cube
	char path[] = "F:\\Git_WorkSpace\\La_Library\\Interface\\Resource\\demo7\\cube1.plg";
	Load_OBJECT4DV1_PLG(&obj, path, &vscale, &vpos, &vrot);

	// set the position of the cube in the world
	obj.world_pos.x = 0;
	obj.world_pos.y = 0;
	obj.world_pos.z = 100;



} // end Game_Init


//////////////////////////////////////////////////////////

void Game_Main(void)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!

	static MATRIX4X4 mrot; // general rotation matrix

	static float x_ang = 0, // these track the rotation
		y_ang = 2, // of the object
		z_ang = 0;

	int index; // looping var

	// start the timing clock
	Start_Clock();

	// clear the drawing surface 
	DDraw_Fill_Surface(lpddsback, 0);

	// read keyboard and other devices here
	DInput_Read_Keyboard();

	// game logic here...

	// reset the object (this only matters for backface and object removal)
	Reset_OBJECT4DV1(&obj);

	// reset angles
	x_ang = z_ang = 0;

	// is user trying to rotate
	if (KEY_DOWN(VK_DOWN))
		x_ang = 1;
	else
		if (KEY_DOWN(VK_UP))
			x_ang = -1;

	// generate rotation matrix around y axis
	Build_XYZ_Rotation_MATRIX4X4(x_ang, y_ang, z_ang, &mrot);

	// rotate the local coords of single polygon in renderlist
	Transform_OBJECT4DV1(&obj, &mrot, TRANSFORM_LOCAL_ONLY, 1);

	// perform local/model to world transform
	Model_To_World_OBJECT4DV1(&obj);

	// generate camera matrix
	Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

	// apply world to camera transform
	World_To_Camera_OBJECT4DV1(&obj, &cam);

	// apply camera to perspective transformation
	Camera_To_Perspective_OBJECT4DV1(&obj, &cam);

	// apply screen transform
	Perspective_To_Screen_OBJECT4DV1(&obj, &cam);

	// draw instructions
	Draw_Text_GDI("Press ESC to exit.", 0, 0, RGB(0, 255, 0), lpddsback);
	Draw_Text_GDI("Use UP ARROW and DOWN ARROW to rotate.", 0, 20, RGB(0, 255, 0), lpddsback);

	// lock the back buffer
	DDraw_Lock_Back_Surface();

	// render the object
	Draw_OBJECT4DV1_Wire16(&obj, back_buffer, back_lpitch);

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

//////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);

	gameBox.create(WINDOW_WIDTH, WINDOW_HEIGHT, TEXT("¼üÅÌ¿ØÖÆÒÆ¶¯"));
	gameBox.setGameStart(Game_Init);
	gameBox.setGameBody(Game_Main);
	gameBox.startCommuication();
	return argc;
}
//////////////////////////////////////////////////////////