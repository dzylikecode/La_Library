// DEMOII7_1.CPP
// READ THIS!
// To compile make sure to include DDRAW.LIB, DSOUND.LIB,
// DINPUT.LIB, DINPUT8.LIB, WINMM.LIB in the project link list, and of course 
// the C++ source modules T3DLIB1-5.CPP and the headers T3DLIB1-5.H
// be in the working directory of the compiler

// INCLUDES ///////////////////////////////////////////////

#define INITGUID       // make sure al the COM interfaces are available
					   // instead of this you can include the .LIB file
					   // DXGUID.LIB

#include "La_Interface.h"
#include "3D1.h"
#include "3D5.h"
#include "La_Master.h"
#include "La_Modify.h"
// DEFINES ////////////////////////////////////////////////

// defines for windows interface
#define WINDOW_CLASS_NAME "WIN3DCLASS"  // class name
#define WINDOW_TITLE      "T3D Graphics Console Ver 2.0"
#define WINDOW_WIDTH      400   // size of window
#define WINDOW_HEIGHT     400

#define WINDOW_BPP        16    // bitdepth of window (8,16,24 etc.)
								// note: if windowed and not
								// fullscreen then bitdepth must
								// be same as system bitdepth
								// also if 8-bit the a pallete
								// is created and attached

#define WINDOWED_APP      1     // 0 not windowed, 1 windowed




// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void* parms = NULL);
int Game_Shutdown(void* parms = NULL);
int Game_Main(void* parms = NULL);



// initialize camera position and direction
POINT4D  cam_pos = { 0,0,-100,1 };
VECTOR4D cam_dir = { 0,0,0,1 };

// all your initialization code goes here...
VECTOR4D vscale = { .5,.5,.5,1 },
vpos = { 0,0,0,1 },
vrot = { 0,0,0,1 };

CAM4DV1        cam;                     // the single camera
RENDERLIST4DV1 rend_list;               // the single renderlist
POLYF4DV1      poly1;                   // our lonely polygon
POINT4D        poly1_pos = { 0,0,100,1 }; // world position of polygon



// T3D II GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

void Game_Init(void)
{


	// hide the mouse
	if (!WINDOWED_APP)
		ShowCursor(FALSE);

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
	Init_CAM4DV1(&cam,      // the camera object
		CAM_MODEL_EULER, // euler camera model
		&cam_pos,  // initial camera position
		&cam_dir,  // initial camera angles
		NULL,      // no initial target
		50.0,      // near and far clipping planes
		500.0,
		90.0,      // field of view in degrees
		WINDOW_WIDTH,   // size of final screen viewport
		WINDOW_HEIGHT);



} // end Game_Init


//////////////////////////////////////////////////////////

void Game_Main()
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!

	static MATRIX4X4 mrot; // general rotation matrix
	static float ang_y = 0;      // rotation angle

	int index; // looping var

	// start the timing clock
	Start_Clock();

	// clear the drawing surface 
	DDraw_Fill_Surface(lpddsback, 0);


	// game logic here...

	// initialize the renderlist
	Reset_RENDERLIST4DV1(&rend_list);

	// insert polygon into the renderlist
	Insert_POLYF4DV1_RENDERLIST4DV1(&rend_list, &poly1);

	// generate rotation matrix around y axis
	Build_XYZ_Rotation_MATRIX4X4(0, ang_y, 0, &mrot);

	// rotate polygon slowly
	if (++ang_y >= 360.0) ang_y = 0;

	// rotate the local coords of single polygon in renderlist
	Transform_RENDERLIST4DV1(&rend_list, &mrot, TRANSFORM_LOCAL_ONLY);

	// perform local/model to world transform
	Model_To_World_RENDERLIST4DV1(&rend_list, &poly1_pos);

	// generate camera matrix
	Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

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

	// render the polygon list
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

	gameBox.create(WINDOW_WIDTH, WINDOW_HEIGHT, TEXT("¼üÅÌ¿ØÖÆÒÆ¶¯"));
	gameBox.setGameStart(Game_Init);
	gameBox.setGameBody(Game_Main);
	gameBox.startCommuication();
	return argc;
}
//////////////////////////////////////////////////////////