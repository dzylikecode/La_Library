

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

// 3D engine constants for overlay star field
#define NEAR_Z            10    // the near clipping plane
#define FAR_Z             2000  // the far clipping plane    
#define VIEW_DISTANCE      400  // viewing distance from viewpoint 
								// this gives a field of view of 90 degrees
								// when projected on a window of 800 wide

// create some constants for ease of access
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define POINT_LIGHT2_INDEX    3 // point light index
#define SPOT_LIGHT2_INDEX     4 // spot light index

// alien defines
#define NUM_ALIENS            16 // total number of ALIEN fighters 

// states for aliens
#define ALIEN_STATE_DEAD      0  // alien is dead
#define ALIEN_STATE_ALIVE     1  // alien is alive
#define ALIEN_STATE_DYING     2  // alien is in process of dying

// star defines
#define NUM_STARS           256  // number of stars in sim

// game state defines
#define GAME_STATE_INIT       0  // game initializing for first time
#define GAME_STATE_RESTART    1  // game restarting after game over
#define GAME_STATE_RUN        2  // game running normally
#define GAME_STATE_DEMO       3  // game in demo mode
#define GAME_STATE_OVER       4  // game over
#define GAME_STATE_EXIT       5  // game state exit

#define MAX_MISSES_GAME_OVER  25 // miss this many and it's game over man

// interface text positions
#define TPOS_SCORE_X          346  // players score
#define TPOS_SCORE_Y          4

#define TPOS_HITS_X           250  // total kills
#define TPOS_HITS_Y           36

#define TPOS_ESCAPED_X        224  // aliens that have escaped
#define TPOS_ESCAPED_Y        58

#define TPOS_GLEVEL_X         430 // diff level of game
#define TPOS_GLEVEL_Y         58

#define TPOS_SPEED_X          404 // diff level of game
#define TPOS_SPEED_Y          36

#define TPOS_GINFO_X          400  // general information
#define TPOS_GINFO_Y          240

#define TPOS_ENERGY_X         158 // weapon energy level
#define TPOS_ENERGY_Y         6

// player defines
#define CROSS_START_X         0
#define CROSS_START_Y         0
#define CROSS_WIDTH           56
#define CROSS_HEIGHT          56

// font display stuff
#define FONT_HPITCH           12  // space between chars
#define FONT_VPITCH           14  // space between lines

// difficulty stuff
#define DIFF_RATE             (float)(.001f)  // rate to advance  difficulty per frame
#define DIFF_PMAX             75


// explosion stuff
#define NUM_EXPLOSIONS        16

// this a 3D star
typedef struct STAR3D_TYP
{
	USHORT color;     // color of point 16-bit
	float x, y, z;      // coordinates of point in 3D
} STAR3D, * STAR3D_PTR;

// ALIEN fighter object
typedef struct ALIEN_TYP
{
	int      state;  // state of this ALIEN fighter
	int      count;  // generic counter
	int      aux;    // generic auxialiary var
	VECTOR3D  pos;    // position of ALIEN fighter
	VECTOR3D vel;    // velocity of ALIEN fighter
	VECTOR3D rot;    // rotational velocities
	VECTOR3D ang;    // current orientation
} ALIEN, * ALIEN_PTR;



// initialize camera position and direction
VECTOR4D  cam_pos = { 0,0	,0 };
VECTOR4D  cam_target = { 0,0,0 };
VECTOR4D cam_dir = { 0,0,0 };

// all your initialization code goes here...
VECTOR4D vscale = { 1.0,1.0,1.0 },
vpos = { 0,0,0 },
vrot = { 0,0,0 };

CAM4DV1        cam;       // the single camera
RENDERLIST4DV2 rend_list; // the render list


// color constants for line drawing
COLOR rgb_green,
rgb_white,
rgb_red,
rgb_blue;

// colors for lights
RGBAV1 white, light_gray, gray, black, red, green, blue;


// music and sound stuff
int
laser_id = -1, // sound of laser pulse
explosion_id = -1, // sound of explosion
flyby_id = -1, // sound of ALIEN fighter flying by
game_over_id = -1, // game over
ambient_systems_id = -1; // ambient sounds

MUSIC mainMusic;
SOUND sound[6];

// star globals
STAR3D stars[NUM_STARS]; // the starfield

// player and game globals
int player_z_vel = 4; // virtual speed of viewpoint/ship

float cross_x = 0, // cross hairs
cross_y = 0;

int cross_x_screen = WINDOW_WIDTH / 2,   // used for cross hair
cross_y_screen = WINDOW_HEIGHT / 2,
target_x_screen = WINDOW_WIDTH / 2,   // used for targeter
target_y_screen = WINDOW_HEIGHT / 2;

int escaped = 0;   // tracks number of missed ships
int hits = 0;   // tracks number of hits
int score = 0;   // take a guess :)
int weapon_energy = 100; // weapon energy
int weapon_active = 0;   // tracks if weapons are energized

int game_state = GAME_STATE_INIT; // state of game
int game_state_count1 = 0;               // general counters
int game_state_count2 = 0;
int restart_state = 0;               // state when game is restarting
int restart_state_count1 = 0;               // general counter

// diffiuculty
float  diff_level = 1;                   // difficulty level used in velocity and probability calcs

// alien globals
ALIEN      aliens[NUM_ALIENS];   // the ALIEN fighters
OBJECT4DV2 obj_alien;            // the master object

// explosions
OBJECT4DV2 explobj[NUM_EXPLOSIONS]; // the explosions

// game imagery assets
SPRITE cockpit;               // the cockpit image
SPRITE starscape;             // the background starscape
aniDICT tech_font;             // the bitmap font for the engine
SPRITE crosshair;             // the player's cross hair

// these are the positions of the energy binding on the main lower control panel
VECTOR2D energy_bindings[6] = { {342, 527}, {459, 527},
							   {343, 534}, {458, 534},
							   {343, 540}, {458, 540} };
// these hold the positions of the weapon burst which use lighting too
// the starting points are known, but the end points are computed on the fly
// based on the cross hair
VECTOR2D weapon_bursts[4] = { {78, 500}, {0,0},     // left energy weapon
							 {720, 500}, {0,0} };  // right energy weapon

int px = 0, py = 0, pz = 500; // debug stuff for object tracking


////////////////////////////////////////////////////////////////////
int Start_Mesh_Explosion(OBJECT4DV2* obj,  // object to destroy
	MATRIX4X4& mrot, // initial orientation of object
	int detail,          // the detail level,1 highest detail
	float rate,          // velocity of explosion shrapnel
	int lifetime);

void Render_Energy_Bindings(VECTOR2D* bindings, // array containing binding positions in the form
												  // start point, end point, start point, end point...
	int num_bindings,     // number of energy bindings to render 1-3
	int num_segments,     // number of segments to randomize bindings into
	int amplitude,        // amplitude of energy binding
	int color)
{
	// this functions renders the energy bindings across the main exterior energy
	// transmission emitters :) Basically, the function takes two points in 2d then
	// it anchors a line at the two ends and randomly walks from end point to the
	// other by breaking the line into segments and then randomly modulating the y position
	// and amount amplitude +-, maximum number of segments 16
	VECTOR2D segments[17]; // to hold constructed segments after construction

	// render each binding
	for (int index = 0; index < num_bindings; index++)
	{
		// store starting and ending positions

		// starting position
		segments[0] = bindings[index * 2];

		// ending position
		segments[num_segments] = bindings[index * 2 + 1];

		// compute vertical gradient, so if y positions of endpoints are
		// greatly different bindings will be modulated using the straight line
		// as a basis
		float dyds = (segments[num_segments].y - segments[0].y) / (float)num_segments;
		float dxds = (segments[num_segments].x - segments[0].x) / (float)num_segments;

		// now build up segments
		for (int sindex = 1; sindex < num_segments; sindex++)
		{
			segments[sindex].x = segments[sindex - 1].x + dxds;
			segments[sindex].y = segments[0].y + sindex * dyds + RAND_RANGE(-amplitude, amplitude);
		} // end for segment            

	// draw binding
		for (int sindex = 0; sindex < num_segments; sindex++)
			GRAPHIC::DrawLine(segments[sindex].x, segments[sindex].y,
				segments[sindex + 1].x, segments[sindex + 1].y,
				color);

	}
}

////////////////////////////////////////////////////////////////////

void Render_Weapon_Bursts(VECTOR2D* burstpoints, // array containing energy burst positions in the form
												   // start point, end point, start point, end point...
	int num_bursts,
	int num_segments,     // number of segments to randomize bindings into
	int amplitude,        // amplitude of energy binding
	int color)
{
	// this functions renders the weapon energy bursts from the weapon emitter or wherever
	// function derived from Render_Energy_Binding, but generalized 
	// Basically, the function takes two points in 2d then
	// it anchors a line at the two ends and randomly walks from end point to the
	// other by breaking the line into segments and then randomly modulating the x,y position
	// and amount amplitude +-, maximum number of segments 16

	VECTOR2D segments[17]; // to hold constructed segments after construction

	// render each energy burst
	for (int index = 0; index < num_bursts; index++)
	{
		// store starting and ending positions

		// starting position
		segments[0] = burstpoints[index * 2];

		// ending position
		segments[num_segments] = burstpoints[index * 2 + 1];

		// compute horizontal/vertical gradients, so we can modulate the lines 
		// on the proper trajectory
		float dyds = (segments[num_segments].y - segments[0].y) / (float)num_segments;
		float dxds = (segments[num_segments].x - segments[0].x) / (float)num_segments;

		// now build up segments
		for (int sindex = 1; sindex < num_segments; sindex++)
		{
			segments[sindex].x = segments[0].x + sindex * dxds + RAND_RANGE(-amplitude, amplitude);
			segments[sindex].y = segments[0].y + sindex * dyds + RAND_RANGE(-amplitude, amplitude);
		} // end for segment            

	// draw binding
		for (int sindex = 0; sindex < num_segments; sindex++)
			GRAPHIC::DrawLine(segments[sindex].x, segments[sindex].y,
				segments[sindex + 1].x, segments[sindex + 1].y,
				color);
	}
}

///////////////////////////////////////////////////////////////

int Load_Bitmap_Font(const TCHAR* fontfile, aniDICT& font)
{
	// this is a semi generic font loader...
	// expects the file name of a font in a template that is 
	// 4 rows of 16 cells, each character is 16x14, and cell 0 is the space character
	// characters from 32 to 95, " " to "-", suffice for 90% of text work

	// load the font bitmap template


	SURFACE temp;
	temp.createFromBitmap(fontfile);
	// create the bob that will hold the font, use a bob for speed, we can use the 
	// hardware blitter
	font.resize(64);

	// load all the frames
	for (int index = 0; index < 64; index++)
	{
		int width = 16; int height = 14;
		font[index].createFromSurface(width, height, temp, (index % 16) * (width + 1) + 1, (index / 16) * (height + 1) + 1);
	}

	return(1);
}

///////////////////////////////////////////////////////////////

int Draw_Bitmap_Font_String(aniDICT& font,  // pointer to bob containing font
	int x, int y,  // screen position to render
	char* string,  // string to render
	int hpitch, int vpitch, // horizontal and vertical pitch
	SURFACE& dest) // destination surface
{
	// this function draws a string based on a 64 character font sent in as a bob
	// the string will be drawn at the given x,y position with intercharacter spacing
	// if hpitch and a interline spacing of vpitch

	// are things semi valid?

	// loop and render
	for (int index = 0; index < strlen(string); index++)
	{
		// set the position and character
		int curr_frame = string[index] - 32;
		// test for overflow set to space
		if (curr_frame > 63 || curr_frame < 0) curr_frame = 0;

		// render character (i hate making a function call!)
		font[curr_frame].drawOn(dest, x, y);

		// move position
		x += hpitch;

	}
	return(1);
}

/////////////////////////////////////////////////////////

int Start_Alien(void)
{
	// this function hunts in the alien list, finds an available alien and 
	// starts it up

	for (int index = 0; index < NUM_ALIENS; index++)
	{
		// is this alien available?
		if (aliens[index].state == ALIEN_STATE_DEAD)
		{
			// clear any residual data
			memset((void*)&aliens[index], 0, sizeof(ALIEN));

			// start alien up
			aliens[index].state = ALIEN_STATE_ALIVE;

			// select random position in bounding volume
			aliens[index].pos.x = RAND_RANGE(-1000, 1000);
			aliens[index].pos.y = RAND_RANGE(-1000, 1000);
			aliens[index].pos.z = 20000;

			// select velocity based on difficulty level
			aliens[index].vel.x = RAND_RANGE(-10, 10);
			aliens[index].vel.y = RAND_RANGE(-10, 10);
			aliens[index].vel.z = -(10 * diff_level + rand() % 200);

			// set rotation rate for z axis only
			aliens[index].rot.z = RAND_RANGE(-5, 5);

			// return the index
			return(index);
		} // end if   

	} // end for index

// failure
	return(-1);

} // end Start_Alien


/////////////////////////////////////////////////////////

void Init_Aliens(void)
{
	// initializes all the ALIEN fighters to a known state
	for (int index = 0; index < NUM_ALIENS; index++)
	{
		// zero ALIEN fighter out
		memset((void*)&aliens[index], 0, sizeof(ALIEN));

		// set any other specific info now...
		aliens[index].state = ALIEN_STATE_DEAD;

	} // end for

} // end Init_Aliens

/////////////////////////////////////////////////////////

void Draw_Aliens(void)
{
	// this function draws all the active ALIEN fighters

	MATRIX4X4 mrot; // used to transform objects

	for (int index = 0; index < NUM_ALIENS; index++)
	{
		// which state is alien in
		switch (aliens[index].state)
		{
			// is the alien dead? if so move on
		case ALIEN_STATE_DEAD: break;

			// is the alien alive?
		case ALIEN_STATE_ALIVE:
		case ALIEN_STATE_DYING:
		{
			// reset the object (this only matters for backface and object removal)
			Reset(obj_alien);

			// generate rotation matrix around y axis
			BuildXYZRotation(aliens[index].ang.x, aliens[index].ang.y, aliens[index].ang.z, mrot);

			// rotate the local coords of the object
			Transform(obj_alien, mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

			// set position of constant shaded cube
			obj_alien.world_pos.x = aliens[index].pos.x;
			obj_alien.world_pos.y = aliens[index].pos.y;
			obj_alien.world_pos.z = aliens[index].pos.z;

			// attempt to cull object
			if (!Cull(obj_alien, cam, CULL_OBJECT_XYZ_PLANES))
			{
				// perform world transform
				ModelToWorld(obj_alien, TRANSFORM_TRANS_ONLY);

				// insert the object into render list
				Insert(rend_list, obj_alien, 0);
			} // end if 


		 // ok, now test for collision with energy bursts, strategy is as follows
		 // we will project the bounding box of the object into screen space to coincide with
		 // the energy burst on the screen, if there is an overlap then the target is hit
		 // simply need to do a few transforms, this kinda sucks that we are going to do this work
		 // 2x, but the problem is that the graphics pipeline knows nothing about collision etc.,
		 // so we can't "get to" the information me want since we are ripping the objects apart
		 // and passing them down the pipeline for a later world to camera transform, thus you
		 // may want to think about this problem when making your own game, how tight to couple
		 // collision and the engine, HOWEVER, the only reason we are having a problem at all is that
		 // we want to use the screen coords of the energy burst this is fine, but a more effective method
		 // would be to compute the 3D world coords of where the energy burst is firing and then project
		 // that parallelpiped in 3D space to see where the player is trying to fire, this is a classic
		 // problem with screen picking, hence, in the engine, when an object is rendered sometimes its 
		 // a good idea to track somekind of collision boundary in screen coords that can be used later
		 // for "object picking" and collision, anyway, let's do it the easy way, but the long way....

		 // first is the player firing weapon?
			if (weapon_active)
			{
				// we need 4 transforms total, first we need all our points in world coords,
				// then we need camera coords, then perspective coords, then screen coords

				// we need to transform 2 points: the center and a point lying on the surface of the  
				// bounding sphere, as long as the 

				VECTOR4D pbox[4], // bounding box coordinates, center points, surrounding object
								 // denoted by X's, we need to project these to screen coords
								 // ....X.... 
								 // . |   | .
								 // X |-O-| X
								 // . |   | . 
								 // ....X....
								 // we will use the average radius as the distance to each X from the center

					presult; // used to hold temp results

			// world to camera transform

			// transform center point only
				Mul(obj_alien.world_pos, cam.mcam, presult);

				// result holds center of object in camera coords now
				// now we are in camera coords, aligned to z-axis, compute radial point axis aligned
				// bounding box points

				// x+r, y, z
				pbox[0].x = presult.x + obj_alien.avg_radius[obj_alien.curr_frame];
				pbox[0].y = presult.y;
				pbox[0].z = presult.z;
				pbox[0].w = 1;

				// x-r, y, z
				pbox[1].x = presult.x - obj_alien.avg_radius[obj_alien.curr_frame];
				pbox[1].y = presult.y;
				pbox[1].z = presult.z;
				pbox[1].w = 1;

				// x, y+r, z
				pbox[2].x = presult.x;
				pbox[2].y = presult.y + obj_alien.avg_radius[obj_alien.curr_frame];
				pbox[2].z = presult.z;
				pbox[2].w = 1;

				// x, y-r, z
				pbox[3].x = presult.x;
				pbox[3].y = presult.y - obj_alien.avg_radius[obj_alien.curr_frame];
				pbox[3].z = presult.z;
				pbox[3].w = 1;


				// now we are ready to project the points to screen space
				float alpha = (0.5 * cam.viewport_width - 0.5);
				float beta = (0.5 * cam.viewport_height - 0.5);

				// loop and process each point
				for (int bindex = 0; bindex < 4; bindex++)
				{
					float z = pbox[bindex].z;

					// perspective transform first
					pbox[bindex].x = cam.view_dist * pbox[bindex].x / z;
					pbox[bindex].y = cam.view_dist * pbox[bindex].y * cam.aspect_ratio / z;
					// z = z, so no change

					// screen projection
					pbox[bindex].x = alpha * pbox[bindex].x + alpha;
					pbox[bindex].y = -beta * pbox[bindex].y + beta;

				} // end for bindex

#ifdef DEBUG_ON
			 // now we have the 4 points is screen coords and we can test them!!! ya!!!!
				Draw_Clip_Line16(pbox[0].x, pbox[2].y, pbox[1].x, pbox[2].y, rgb_red, back_buffer, back_lpitch);
				Draw_Clip_Line16(pbox[0].x, pbox[3].y, pbox[1].x, pbox[3].y, rgb_red, back_buffer, back_lpitch);

				Draw_Clip_Line16(pbox[0].x, pbox[2].y, pbox[0].x, pbox[3].y, rgb_red, back_buffer, back_lpitch);
				Draw_Clip_Line16(pbox[1].x, pbox[2].y, pbox[1].x, pbox[3].y, rgb_red, back_buffer, back_lpitch);
#endif

				// test for collision
				if ((cross_x_screen > pbox[1].x) && (cross_x_screen < pbox[0].x) &&
					(cross_y_screen > pbox[2].y) && (cross_y_screen < pbox[3].y))
				{
					Start_Mesh_Explosion(&obj_alien, // object to blow
						mrot,      // initial orientation
						3,          // the detail level,1 highest detail
						.5,         // velocity of explosion shrapnel
						100);       // total lifetime of explosion         

   // remove from simulation
					aliens[index].state = ALIEN_STATE_DEAD;

					// make some sound
					sound[explosion_id].play();

					// increment hits
					hits++;

					// take into consideration the z, the speed, the level, blah blah
					score += (int)((diff_level * 10 + obj_alien.world_pos.z / 10));

				} // end if

			} // end if weapon active

		} break;

		default: break;

		} // end switch

	} // end for index

// debug code

#ifdef DEBUG_ON
// reset the object (this only matters for backface and object removal)
	Reset_OBJECT4DV2(&obj_alien);
	static int ang_y = 0;
	// generate rotation matrix around y axis
	Build_XYZ_Rotation_MATRIX4X4(0, ang_y++, 0, &mrot);

	// rotate the local coords of the object
	Transform_OBJECT4DV2(&obj_alien, &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

	// set position of constant shaded cube
	obj_alien.world_pos.x = px;
	obj_alien.world_pos.y = py;
	obj_alien.world_pos.z = pz;

	// perform world transform
	Model_To_World_OBJECT4DV2(&obj_alien, TRANSFORM_TRANS_ONLY);

	// insert the object into render list
	Insert_OBJECT4DV2_RENDERLIST4DV2(&rend_list, &obj_alien, 0);
#endif

} // end Draw_Aliens

//////////////////////////////////////////////////////////

void Draw_Mesh_Explosions(void)
{
	// this function draws the mesh explosions
	MATRIX4X4 mrot; // used to transform objects

	// draw the explosions, note we do NOT cull them
	for (int eindex = 0; eindex < NUM_EXPLOSIONS; eindex++)
	{
		// is the mesh explosions active
		if ((explobj[eindex].state & OBJECT4DV2_STATE_ACTIVE))
		{
			// reset the object
			Reset(explobj[eindex]);

			// generate rotation matrix 
			BuildXYZRotation(0, 0, 0, mrot);

			// rotate the local coords of the object
			Transform(explobj[eindex], mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

			// perform world transform
			ModelToWorld(explobj[eindex], TRANSFORM_TRANS_ONLY);

			// insert the object into render list
			Insert(rend_list, explobj[eindex], 0);

			// now animate the mesh
			VECTOR4D* trajectory = (VECTOR4D*)explobj[eindex].ivar1;

			for (int pindex = 0; pindex < explobj[eindex].num_polys; pindex++)
			{
				// vertex 0
				Add(explobj[eindex].vlist_local[pindex * 3 + 0].v,
					trajectory[pindex * 2 + 0],
					explobj[eindex].vlist_local[pindex * 3 + 0].v);

				// vertex 1
				Add(explobj[eindex].vlist_local[pindex * 3 + 1].v,
					trajectory[pindex * 2 + 0],
					explobj[eindex].vlist_local[pindex * 3 + 1].v);

				// vertex 2
				Add(explobj[eindex].vlist_local[pindex * 3 + 2].v,
					trajectory[pindex * 2 + 0],
					explobj[eindex].vlist_local[pindex * 3 + 2].v);
			}

			// update counter, test for terminate
			if (--explobj[eindex].ivar2 < 0)
			{
				// reset this object
				explobj[eindex].state = OBJECT4DV2_STATE_NULL;

				// release memory of object, but only data that isn't linked to master object
				// local vertex list
				if (explobj[eindex].head_vlist_local)
					free(explobj[eindex].head_vlist_local);

				// transformed vertex list
				if (explobj[eindex].head_vlist_trans)
					free(explobj[eindex].head_vlist_trans);

				// polygon list
				if (explobj[eindex].plist)
					free(explobj[eindex].plist);

				// trajectory list
				if ((VECTOR4D*)(explobj[eindex].ivar1))
					free((VECTOR4D*)explobj[eindex].ivar1);

				// now clear out object completely
				memset((void*)&explobj[eindex], 0, sizeof(OBJECT4DV2));

			} // end if 

		} // end if  

	} // end for eindex

} // end Draw_Mesh_Explosions

//////////////////////////////////////////////////////////

int Start_Mesh_Explosion(OBJECT4DV2* obj,  // object to destroy
	MATRIX4X4& mrot, // initial orientation of object
	int detail,          // the detail level,1 highest detail
	float rate,          // velocity of explosion shrapnel
	int lifetime)        // total lifetime of explosion
{
	// this function "blows up" an object, it takes a pointer to the object to
	// be destroyed, the detail level of the polyogonal explosion, 1 is high detail, 
	// numbers greater than 1 are lower detail, the detail selects the polygon extraction
	// stepping, hence a detail of 5 would mean every 5th polygon in the original mesh
	// should be part of the explosion, on average no more than 10-50% of the polygons in 
	// the original mesh should be used for the explosion; some would be vaporized and
	// in a more practical sense, when an object is far, there's no need to have detail
	// the next parameter is the rate which is used as a general scale for the explosion
	// velocity, and finally lifetime which is the number of cycles to display the explosion

	// the function works by taking the original object then copying the core information
	// except for the polygon and vertex lists, the function operates by selecting polygon 
	// by polygon to be destroyed and then makes a copy of the polygon AND all of it's vertices,
	// thus the vertex coherence is completely lost, this is a necessity since each polygon must
	// be animated separately by the engine, thus they can't share vertices, additionally at the
	// end if the vertex list will be the velocity and rotation information for each polygon
	// this is hidden to the rest of the engine, now for the explosion, the center of the object
	// is used as the point of origin then a ray is drawn thru each polygon, then each polygon
	// is thrown at some velocity with a small rotation rate
	// finally the system can only have so many explosions at one time

	// step: 1 find an available explosion
	for (int eindex = 0; eindex < NUM_EXPLOSIONS; eindex++)
	{
		// is this explosion available?
		if (explobj[eindex].state == OBJECT4DV2_STATE_NULL)
		{
			// copy the object, including the pointers which we will unlink shortly...
			explobj[eindex] = *obj;

			explobj[eindex].state = OBJECT4DV2_STATE_ACTIVE | OBJECT4DV2_STATE_VISIBLE;

			// recompute a few things
			explobj[eindex].num_polys = obj->num_polys / detail;
			explobj[eindex].num_vertices = 3 * obj->num_polys;
			explobj[eindex].total_vertices = 3 * obj->num_polys; // one frame only

			// unlink/re-allocate all the pointers except the texture coordinates, we can use those, they don't
			// depend on vertex coherence
			// allocate memory for vertex lists
			if (!(explobj[eindex].vlist_local = (VERTEX4DTV1*)malloc(sizeof(VERTEX4DTV1) * explobj[eindex].num_vertices)))
				return(0);

			// clear data
			memset((void*)explobj[eindex].vlist_local, 0, sizeof(VERTEX4DTV1) * explobj[eindex].num_vertices);

			if (!(explobj[eindex].vlist_trans = (VERTEX4DTV1*)malloc(sizeof(VERTEX4DTV1) * explobj[eindex].num_vertices)))
				return(0);

			// clear data
			memset((void*)explobj[eindex].vlist_trans, 0, sizeof(VERTEX4DTV1) * explobj[eindex].num_vertices);

			// allocate memory for polygon list
			if (!(explobj[eindex].plist = (POLY4DV2*)malloc(sizeof(POLY4DV2) * explobj[eindex].num_polys)))
				return(0);

			// clear data
			memset((void*)explobj[eindex].plist, 0, sizeof(POLY4DV2) * explobj[eindex].num_polys);

			// now, we need somewhere to store the vector trajectories of the polygons and their rotational rates
			// so let's allocate an array of VECTOR4D elements to hold this information and then store it
			// in ivar1, therby using ivar as a pointer, this is perfectly fine :)
			// each record will consist of a velocity and a rotational rate in x,y,z, so V0,R0, V1,R1,...Vn-1, Rn-1
			// allocate memory for polygon list
			if (!(explobj[eindex].ivar1 = (int)malloc(sizeof(VECTOR4D) * 2 * explobj[eindex].num_polys)))
				return(0);

			// clear data
			memset((void*)explobj[eindex].ivar1, 0, sizeof(VECTOR4D) * 2 * explobj[eindex].num_polys);

			// alias working pointer
			VECTOR4D* trajectory = (VECTOR4D*)explobj[eindex].ivar1;

			// these are needed to track the "head" of the vertex list for multi-frame objects
			explobj[eindex].head_vlist_local = explobj[eindex].vlist_local;
			explobj[eindex].head_vlist_trans = explobj[eindex].vlist_trans;

			// set the lifetime in ivar2
			explobj[eindex].ivar2 = lifetime;

			// now comes the tricky part, loop and copy each polygon, but as we copy the polygons, we have to copy
			// the vertices, and insert them, and fix up the vertice indices etc.
			for (int pindex = 0; pindex < explobj[eindex].num_polys; pindex++)
			{
				// alright, we want to copy the (pindex*detail)th poly from the master to the (pindex)th
				// polygon in explosion
				explobj[eindex].plist[pindex] = obj->plist[pindex * detail];

				// we need to modify, the vertex list pointer and the vertex indices themselves
				explobj[eindex].plist[pindex].vlist = explobj[eindex].vlist_local;

				// now comes the hard part, we need to first copy the vertices from the original mesh
				// into the new vertex storage, but at the same time keep the same vertex ordering

				// vertex 0
				explobj[eindex].plist[pindex].vert[0] = pindex * 3 + 0;
				explobj[eindex].vlist_local[pindex * 3 + 0] = obj->vlist_local[obj->plist[pindex * detail].vert[0]];

				// vertex 1
				explobj[eindex].plist[pindex].vert[1] = pindex * 3 + 1;
				explobj[eindex].vlist_local[pindex * 3 + 1] = obj->vlist_local[obj->plist[pindex * detail].vert[1]];

				// vertex 2
				explobj[eindex].plist[pindex].vert[2] = pindex * 3 + 2;
				explobj[eindex].vlist_local[pindex * 3 + 2] = obj->vlist_local[obj->plist[pindex * detail].vert[2]];

				// reset shading flag to constant for some of the shrapnel since they are giving off light 
				if ((rand() % 5) == 1)
				{
					SET_BIT(explobj[eindex].plist[pindex].attr, POLY4DV2_ATTR_SHADE_MODE_PURE);
					RESET_BIT(explobj[eindex].plist[pindex].attr, POLY4DV2_ATTR_SHADE_MODE_GOURAUD);
					RESET_BIT(explobj[eindex].plist[pindex].attr, POLY4DV2_ATTR_SHADE_MODE_FLAT);

					// set color
					explobj[eindex].plist[pindex].color = RGB_DX(128 + rand() % 128, 0, 0);

				} // end if

			 // add some random noise to trajectory
				static VECTOR4D mv;

				// first compute trajectory as vector from center to vertex piercing polygon
				trajectory[pindex * 2 + 0] = obj->vlist_local[obj->plist[pindex * detail].vert[0]].v;
				Scale(rate, trajectory[pindex * 2 + 0], trajectory[pindex * 2 + 0]);

				mv.x = RAND_RANGE(-10, 10);
				mv.y = RAND_RANGE(-10, 10);
				mv.z = RAND_RANGE(-10, 10);
				mv.w = 1;

				Add(mv, trajectory[pindex * 2 + 0], trajectory[pindex * 2 + 0]);

				// now rotation rate, this is difficult to do since we don't have the center of the polygon
				// maybe later...
				// trajectory[pindex*2+1] = 

			} // end for pindex


		// now transform final mesh into position destructively
			Transform(explobj[eindex], mrot, TRANSFORM_LOCAL_ONLY, 1);

			return(1);

		} // end if found a free explosion

	} // end for eindex

// return failure
	return(0);

} // end Start_Mesh_Explosion

///////////////////////////////////////////////////////////

void Process_Aliens(void)
{
	// this function moves the aliens and performs AI

	for (int index = 0; index < NUM_ALIENS; index++)
	{
		// which state is alien in
		switch (aliens[index].state)
		{
			// is the alien dead? if so move on
		case ALIEN_STATE_DEAD: break;

			// is the alien alive?
		case ALIEN_STATE_ALIVE:
		case ALIEN_STATE_DYING:
		{
			// move the alien
			aliens[index].pos.x += aliens[index].vel.y;
			aliens[index].pos.y += aliens[index].vel.y;
			aliens[index].pos.z += aliens[index].vel.z;

			// rotate the alien
			if ((aliens[index].ang.x += aliens[index].rot.x) > 360)
				aliens[index].ang.x = 0;

			if ((aliens[index].ang.y += aliens[index].rot.y) > 360)
				aliens[index].ang.y = 0;

			if ((aliens[index].ang.z += aliens[index].rot.z) > 360)
				aliens[index].ang.z = 0;

			// test if alien has past players near clipping plane
			// if so remove from simulation
			if (aliens[index].pos.z < cam.near_clip_z)
			{
				// remove from simulation
				aliens[index].state = ALIEN_STATE_DEAD;

				// another one gets away!
				escaped++;
			} // end if 

		} break;


		default: break;

		} // end switch

	} // end for index

} // end Process_Aliens

//////////////////////////////////////////////////////////

void Init_Starfield(void)
{
	// create the starfield
	for (int index = 0; index < NUM_STARS; index++)
	{
		// randomly position stars in an elongated cylinder stretching from
		// the viewpoint 0,0,-d to the yon clipping plane 0,0,far_z
		stars[index].x = -WINDOW_WIDTH / 2 + rand() % WINDOW_WIDTH;
		stars[index].y = -WINDOW_HEIGHT / 2 + rand() % WINDOW_HEIGHT;
		stars[index].z = NEAR_Z + rand() % (FAR_Z - NEAR_Z);

		// set color of stars
		stars[index].color = rgb_white;
	} // end for index

} // end Init_Starfield

//////////////////////////////////////////////////////////

void Move_Starfield(void)
{
	// move the stars

	int index; // looping var

	// the stars are technically stationary,but we are going
	// to move them to simulate motion of the viewpoint
	for (index = 0; index < NUM_STARS; index++)
	{
		// move the next star
		stars[index].z -= (player_z_vel + 3 * diff_level);

		// test for past near clipping plane
		if (stars[index].z <= NEAR_Z)
			stars[index].z = FAR_Z;

	} // end for index

} // end Move_Starfield

/////////////////////////////////////////////////////////

void Draw_Starfield(SURFACE& surface)
{
	// draw the stars in 3D using perspective transform

	int index; // looping var
	int lpitch = surface.getLpitch();
	COLOR* video_buffer = surface.getMemory();

	// draw each star
	for (index = 0; index < NUM_STARS; index++)
	{
		// draw the next star
		// step 1: perspective transform
		float x_per = VIEW_DISTANCE * stars[index].x / stars[index].z;
		float y_per = VIEW_DISTANCE * stars[index].y / stars[index].z;

		// step 2: compute screen coords
		int x_screen = WINDOW_WIDTH / 2 + x_per;
		int y_screen = WINDOW_HEIGHT / 2 - y_per;

		// clip to screen coords
		if (x_screen >= WINDOW_WIDTH || x_screen < 0 ||
			y_screen >= WINDOW_HEIGHT || y_screen < 0)
		{
			// continue to next star
			continue;
		} // end if
		else
		{
			// else render to buffer

			int i = ((float)(10000 * NEAR_Z) / stars[index].z);
			if (i > 255) i = 255;

			(video_buffer)[x_screen + y_screen * lpitch] = RGB_DX(i, i, i);
		} // end else

	} // end for index

}  // Draw_Starfield

TSTRING path = TEXT("F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo53\\");
MATV1 materials[MAX_MATERIALS];
int  numMaterials;
LIGHTV1 lights[4];

void StartUp(void)
{
	// initialize the camera with 90 FOV, normalized coordinates
	cam.set(
		CAM_MODEL_EULER, // the euler model
		cam_pos,  // initial camera position
		cam_dir,  // initial camera angles
		&cam_target,      // no target
		150.0,      // near and far clipping planes
		12000.0,
		120.0,      // field of view in degrees
		WINDOW_WIDTH,   // size of final screen viewport
		WINDOW_HEIGHT);

	SetTexturePath(TToA(path));
	explosion_id = 0;
	laser_id = 1;

	game_over_id = 2;
	ambient_systems_id = 3;

	sound[explosion_id].create(path + TEXT("exp1.wav"));
	sound[laser_id].create(path + TEXT("shocker.wav"));
	sound[game_over_id].create(path + TEXT("gameover.wav"));
	sound[ambient_systems_id].create(path + TEXT("stationthrob2.wav"));

	mainMusic.create(TEXT("midifile2.mid"), path);


	rgb_green = RGB_DX(0, 255, 0);
	rgb_white = RGB_DX(255, 255, 255);
	rgb_blue = RGB_DX(0, 0, 255);
	rgb_red = RGB_DX(255, 0, 0);



	static aniDICT cockpitDict;
	cockpitDict.resize(2);
	cockpitDict[0].createFromBitmap(path + TEXT("cockpit03.BMP"));
	cockpitDict[1].createFromBitmap(path + TEXT("cockpit03b.BMP"));
	cockpit.content.setDict(&cockpitDict);
	cockpit.content.autoOrder();

	static aniDICT starscapeDict;
	starscapeDict.resize(3);
	starscapeDict[0].createFromBitmap(path + TEXT("nebblue01.bmp"));
	starscapeDict[1].createFromBitmap(path + TEXT("nebred01.bmp"));
	starscapeDict[2].createFromBitmap(path + TEXT("nebgreen03.bmp"));
	starscape.content.setDict(&starscapeDict);
	starscape.content.autoOrder();

	static aniDICT crosshairDict;
	crosshairDict.resize(1);
	crosshairDict[0].createFromBitmap(path + TEXT("crosshair01.bmp"));
	crosshair.content.setDict(&crosshairDict);
	crosshair.content.autoOrder();


	// load flat shaded cube
	vscale.set(18.00, 18.00, 18.00);
	LoadCOB(obj_alien, TToA(path) + "tie04.cob",
		vscale, vpos, vrot, VERTEX_FLAGS_SWAP_YZ | VERTEX_FLAGS_SWAP_XZ |
		VERTEX_FLAGS_INVERT_WINDING_ORDER |
		VERTEX_FLAGS_TRANSFORM_LOCAL |
		VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD, materials, numMaterials);

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

	VECTOR4D slight_pos = { 0,0,200,0 };
	VECTOR4D slight_dir = { 0,0,0,0 };

	// spot light
	lights[POINT_LIGHT2_INDEX].set(
		LIGHTV1_STATE_OFF,         // turn the light on
		LIGHTV1_ATTR_POINT,  // spot light type 2
		black, green, black,      // color for diffuse term only
		slight_pos, slight_dir, // need pos only
		0, .0005, 0,                 // linear attenuation only
		0, 0, 1);			          // spotlight powerfactor only

	keyboard.create();

	fpsSet.set(30);
}
MOUSE mouse;
void GameBody(void)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!

	static MATRIX4X4 mrot;   // general rotation matrix

	// these are used to create a circling camera
	static float view_angle = 0;
	static float camera_distance = 6000;
	static VECTOR4D pos = { 0,0,0,0 };
	static float tank_speed;
	static float turning = 0;
	// state variables for different rendering modes and help
	static int wireframe_mode = 0;
	static int backface_mode = 1;
	static int lighting_mode = 1;
	static int zsort_mode = 1;

	char work_string[256]; // temp string

	// what state is game in?
	switch (game_state)
	{
	case GAME_STATE_INIT:
	{
		// load the font
		Load_Bitmap_Font(path + TEXT("tech_char_set_01.bmp"), tech_font);
		mouse.create();
		// transition to un state
		game_state = GAME_STATE_RESTART;
	} break;

	case GAME_STATE_RESTART:
	{
		// start music
		mainMusic.play();
		sound[ambient_systems_id].play(true);

		// initialize the stars
		Init_Starfield();

		// initialize all the aliens
		Init_Aliens();

		// reset all vars
		player_z_vel = 4; // virtual speed of viewpoint/ship

		cross_x = CROSS_START_X; // cross hairs
		cross_y = CROSS_START_Y;

		cross_x_screen = CROSS_START_X;   // used for cross hair
		cross_y_screen = CROSS_START_Y;
		target_x_screen = CROSS_START_X;   // used for targeter
		target_y_screen = CROSS_START_Y;

		escaped = 0;   // tracks number of missed ships
		hits = 0;   // tracks number of hits
		score = 0;   // take a guess :)
		weapon_energy = 100; // weapon energy
		weapon_active = 0;   // weapons are off

		game_state_count1 = 0;    // general counters
		game_state_count2 = 0;
		restart_state = 0;    // state when game is restarting
		restart_state_count1 = 0;    // general counter

		// difficulty
		diff_level = 1;

		// transition to run state
		game_state = GAME_STATE_RUN;

	} break;

	case GAME_STATE_RUN:
	case GAME_STATE_OVER: // keep running sim, but kill diff_level, and player input
	{
		// clear the drawing surface 
		//DDraw_Fill_Surface(lpddsback, 0);

		// blt to destination surface
		graphicOut.fillColor();
		starscape.drawOn();

		// reset the render list
		Reset(rend_list);

		// read keyboard and other devices here
		keyboard.read();
		mouse.read();
		// game logic begins here...


#ifdef DEBUG_ON      
		if (keyboard_state[DIK_UP])
		{
			py += 10;

		} // end if

		if (keyboard_state[DIK_DOWN])
		{
			py -= 10;
		} // end if

		if (keyboard_state[DIK_RIGHT])
		{
			px += 10;
		} // end if

		if (keyboard_state[DIK_LEFT])
		{
			px -= 10;
		} // end if

		if (keyboard_state[DIK_1])
		{
			pz += 20;

		} // end if

		if (keyboard_state[DIK_2])
		{
			pz -= 20;

		} // end if
#endif

	  // modes and lights

	  // wireframe mode
		if (keyboard[DIK_W])
		{
			// toggle wireframe mode
			if (++wireframe_mode > 1)
				wireframe_mode = 0;

			Sleep(100); // wait, so keyboard doesn't bounce
		} // end if

	  // change nebula
		if (keyboard[DIK_N])
		{
			if (++starscape.getCurWord() >= starscape.content.getDict()->getSize())
				starscape.setAniWord(0);

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

	 // z-sorting
		if (keyboard[DIK_Z])
		{
			// toggle z sorting
			zsort_mode = -zsort_mode;
			Sleep(100); // wait, so keyboard doesn't bounce
		} // end if

	 // track cross hair

		cross_x += mouse.getX();
		cross_y -= mouse.getY();

		// check bounds (x,y) are in 2D space coords, not screen coords
		if (cross_x >= (WINDOW_WIDTH / 2 - CROSS_WIDTH / 2))
			cross_x = (WINDOW_WIDTH / 2 - CROSS_WIDTH / 2) - 1;
		else if (cross_x <= -(WINDOW_WIDTH / 2 - CROSS_WIDTH / 2))
			cross_x = -(WINDOW_WIDTH / 2 - CROSS_WIDTH / 2) + 1;

		if (cross_y >= (WINDOW_HEIGHT / 2 - CROSS_HEIGHT / 2))
			cross_y = (WINDOW_HEIGHT / 2 - CROSS_HEIGHT / 2) - 1;
		else if (cross_y <= -(WINDOW_HEIGHT / 2 - CROSS_HEIGHT / 2))
			cross_y = -(WINDOW_HEIGHT / 2 - CROSS_HEIGHT / 2) + 1;

		// player is done moving create camera matrix //////////////////////////
		BuildEuler(cam, CAM_ROT_SEQ_ZYX);

		// perform all non-player game AI and motion here /////////////////////

		// move starfield
		Move_Starfield();

		// move aliens
		Process_Aliens();

		// update difficulty of game
		if ((diff_level += DIFF_RATE) > DIFF_PMAX)
			diff_level = DIFF_PMAX;

		// start a random alien as a function of game difficulty
		if ((rand() % (DIFF_PMAX - (int)diff_level + 2)) == 1)
			Start_Alien();

		// perform animation and transforms on lights //////////////////////////

		// lock the back buffer
		BeginDrawOn();

		// draw all 3D entities
		Draw_Aliens();

		// draw mesh explosions
		Draw_Mesh_Explosions();

		// entire into final 3D pipeline /////////////////////////////////////// 

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

		// draw the starfield now
		Draw_Starfield(graphicOut);

		// render the list
		if (wireframe_mode == 0)
		{
			DrawSolid(rend_list);
		}
		else if (wireframe_mode == 1)
		{
			DrawWire(rend_list);
		} // end if

	 // draw energy bindings
		Render_Energy_Bindings(energy_bindings, 1, 8, 16, RGB_DX(0, 255, 0));

		// fire the weapon
		if (game_state == GAME_STATE_RUN && mouse.getLButton() && weapon_energy > 20)
		{
			// set endpoints of energy bolts
			weapon_bursts[1].x = cross_x_screen + RAND_RANGE(-4, 4);
			weapon_bursts[1].y = cross_y_screen + RAND_RANGE(-4, 4);

			weapon_bursts[3].x = cross_x_screen + RAND_RANGE(-4, 4);
			weapon_bursts[3].y = cross_y_screen + RAND_RANGE(-4, 4);

			// draw the weapons firing
			Render_Weapon_Bursts(weapon_bursts, 2, 8, 16, RGB_DX(0, 255, 0));

			// decrease weapon energy
			if ((weapon_energy -= 5) < 0)
				weapon_energy = 0;

			// make sound
			sound[laser_id].play();

			// energize weapons
			weapon_active = 1;

			// turn the lights on!
			lights[POINT_LIGHT2_INDEX].state = LIGHTV1_STATE_ON;
			cockpit.setAniWord(1);

		}
		else
		{
			weapon_active = 0;

			// turn the lights off!
			lights[POINT_LIGHT2_INDEX].state = LIGHTV1_STATE_OFF;
			cockpit.setAniWord(0);
		} // end else

		if ((weapon_energy += 1) > 100)
			weapon_energy = 100;

		// draw any overlays ///////////////////////////////////////////////////

		// unlock the back buffer
		EndDrawOn();

		// draw cockpit
		cockpit.drawOn();


		// draw information
		sprintf(work_string, "SCORE:%d", score);
		Draw_Bitmap_Font_String(tech_font, TPOS_SCORE_X, TPOS_SCORE_Y, work_string, FONT_HPITCH, FONT_VPITCH, graphicOut);

		sprintf(work_string, "HITS:%d", hits);
		Draw_Bitmap_Font_String(tech_font, TPOS_HITS_X, TPOS_HITS_Y, work_string, FONT_HPITCH, FONT_VPITCH, graphicOut);

		sprintf(work_string, "ESCAPED:%d", escaped);
		Draw_Bitmap_Font_String(tech_font, TPOS_ESCAPED_X, TPOS_ESCAPED_Y, work_string, FONT_HPITCH, FONT_VPITCH, graphicOut);

		sprintf(work_string, "LEVEL:%2.4f", diff_level);
		Draw_Bitmap_Font_String(tech_font, TPOS_GLEVEL_X, TPOS_GLEVEL_Y, work_string, FONT_HPITCH, FONT_VPITCH, graphicOut);

		sprintf(work_string, "VEL:%3.2fK/S", player_z_vel + diff_level);
		Draw_Bitmap_Font_String(tech_font, TPOS_SPEED_X, TPOS_SPEED_Y, work_string, FONT_HPITCH, FONT_VPITCH, graphicOut);

		sprintf(work_string, "NRG:%d", weapon_energy);
		Draw_Bitmap_Font_String(tech_font, TPOS_ENERGY_X, TPOS_ENERGY_Y, work_string, FONT_HPITCH, FONT_VPITCH, graphicOut);

		// process game over stuff
		if (game_state == GAME_STATE_OVER)
		{
			// do rendering
			sprintf(work_string, "GAME OVER");
			Draw_Bitmap_Font_String(tech_font, TPOS_GINFO_X - (FONT_HPITCH / 2) * strlen(work_string), TPOS_GINFO_Y, work_string, FONT_HPITCH, FONT_VPITCH, graphicOut);

			sprintf(work_string, "PRESS ENTER TO PLAY AGAIN");
			Draw_Bitmap_Font_String(tech_font, TPOS_GINFO_X - (FONT_HPITCH / 2) * strlen(work_string), TPOS_GINFO_Y + 2 * FONT_VPITCH, work_string, FONT_HPITCH, FONT_VPITCH, graphicOut);

			// logic...
			if (keyboard[DIK_RETURN])
			{
				game_state = GAME_STATE_RESTART;
			} // end if

		} // end if

#ifdef DEBUG_ON
	   // display diagnostics
		sprintf(work_string, "LE[%s]:AMB=%d,INFL=%d,PNTL=%d,SPTL=%d,ZS[%s],BFRM[%s], WF=%s",
			((lighting_mode == 1) ? "ON" : "OFF"),
			lights[AMBIENT_LIGHT_INDEX].state,
			lights[INFINITE_LIGHT_INDEX].state,
			lights[POINT_LIGHT_INDEX].state,
			lights[SPOT_LIGHT2_INDEX].state,
			((zsort_mode == 1) ? "ON" : "OFF"),
			((backface_mode == 1) ? "ON" : "OFF"),
			((wireframe_mode == 1) ? "ON" : "OFF"));

		Draw_Bitmap_Font_String(&tech_font, 4, WINDOW_HEIGHT - 16, work_string, FONT_HPITCH, FONT_VPITCH, lpddsback);
#endif

		// draw startup information for first few seconds of restart
		if (restart_state == 0)
		{
			sprintf(work_string, "GET READY!");
			Draw_Bitmap_Font_String(tech_font, TPOS_GINFO_X - (FONT_HPITCH / 2) * strlen(work_string), TPOS_GINFO_Y, work_string, FONT_HPITCH, FONT_VPITCH, graphicOut);

			// update counter
			if (++restart_state_count1 > 100)
				restart_state = 1;
		} // end if

#ifdef DEBUG_ON
		sprintf(work_string, "p=[%d, %d, %d]", px, py, pz);
		Draw_Bitmap_Font_String(&tech_font, TPOS_GINFO_X - (FONT_HPITCH / 2) * strlen(work_string), TPOS_GINFO_Y + 24, work_string, FONT_HPITCH, FONT_VPITCH, lpddsback);
#endif

		if (game_state == GAME_STATE_RUN)
		{
			// draw cross on top of everything, it's holographic :)
			cross_x_screen = WINDOW_WIDTH / 2 + cross_x;
			cross_y_screen = WINDOW_HEIGHT / 2 - cross_y;

			crosshair.x = cross_x_screen - CROSS_WIDTH / 2 + 1;
			crosshair.y = cross_y_screen - CROSS_HEIGHT / 2;

			crosshair.drawOn();
		}
		fpsSet.adjust();
		gPrintf(0, 0, RED_GDI, TEXT("%d"), fpsSet.get());
		Flush();

		// check if player has lost?
		if (escaped == MAX_MISSES_GAME_OVER)
		{
			game_state = GAME_STATE_OVER;
			sound[game_over_id].play();
			escaped++;
		} // end if



		if (KEY_DOWN(VK_ESCAPE))
			gameBox.exitFromGameBody();

	} break;

	case GAME_STATE_DEMO:
	{


	} break;

	case GAME_STATE_EXIT:
	{


	} break;


	default: break;

	}




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


