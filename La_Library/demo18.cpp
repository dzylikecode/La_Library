
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
#include "La_Physics.h"
#include "La_Math.h"
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
using namespace LADZY;

KEYBOARD keyboard;

// DEFINES ////////////////////////////////////////////////

// mathematical defines
CONST float FRICTION = 0.1;


// size of universe
#define UNIVERSE_MIN_X   (-8000)
#define UNIVERSE_MAX_X   (8000)
#define UNIVERSE_MIN_Y   (-8000)
#define UNIVERSE_MAX_Y   (8000)

// weapons defines
#define MAX_PLASMA         32   // max number of plasma pulses
#define PLASMA_SPEED       16   // pixels/sec of plasma pulse
#define PLASMA_SPEED_SLOW  12   // slow version for gunships

#define PLASMA_STATE_OFF   0   // this plasma is dead or off
#define PLASMA_STATE_ON    1   // this one is alive and in flight
#define PLASMA_RANGE_1     30  // type 1 plasma disruptor

#define PLASMA_ANIM_PLAYER     0  // a plasma torpedeo from player
#define PLASMA_ANIM_ENEMY      1  // a plasma torpedeo from any enemy

// asteroid field defines
#define MAX_ROCKS             300
#define ROCK_STATE_OFF        0   // this rock is dead or off
#define ROCK_STATE_ON         1   // this one is alive and in flight

#define ROCK_LARGE            0   // sizes of rock
#define ROCK_MEDIUM           1
#define ROCK_SMALL            2

// explosion defines 
#define MAX_BURSTS            8
#define BURST_STATE_OFF       0   // this burst is dead or off
#define BURST_STATE_ON        1   // this one is alive

// defines for player states
#define PLAYER_STATE_DEAD           0
#define PLAYER_STATE_DYING          1
#define PLAYER_STATE_INVINCIBLE     2
#define PLAYER_STATE_ALIVE          3
#define PLAYER_REGEN_COUNT          100 // click before regenerating

#define WRAITH_INDEX_DIR            3 // index of direction var

#define MAX_PLAYER_SPEED            16

// sound id's
#define MAX_FIRE_SOUNDS       8
#define MAX_EXPL_SOUNDS       8


// defines for stations
#define MAX_STATIONS           20

// life state for stations
#define STATION_STATE_DEAD       0
#define STATION_STATE_ALIVE      1
#define STATION_STATE_DAMAGED    2  
#define STATION_STATE_DYING      3

#define STATION_SHIELDS_ANIM_ON  0  // animations for shields
#define STATION_SHIELDS_ANIM_OFF 1 

#define STATION_RANGE_RING       300

#define INDEX_STATION_DAMAGE     2  // tracks the current damage of station
#define MAX_STATION_DAMAGE      100

#define STATION_MIN_UPLINK_DISTANCE  150


// defines for mines
#define MAX_MINES                16

// life state for stations
#define MINE_STATE_DEAD       0
#define MINE_STATE_ALIVE      1
#define MINE_STATE_DAMAGED    2  
#define MINE_STATE_DYING      3

#define MINE_STATE_AI_ACTIVATED   1
#define MINE_STATE_AI_SLEEP       0

#define INDEX_MINE_AI_STATE       1  // state of ai system
#define INDEX_MINE_DAMAGE         2  // tracks the current damage of MINE
#define INDEX_MINE_CONTACT_COUNT  3  // tracks how long mine has been in contact with player

#define MAX_MINE_DAMAGE            50 
#define MAX_MINE_CONTACT_COUNT     20

#define MAX_MINE_VELOCITY        16  
#define MIN_MINE_TRACKING_DIST   1000
#define MIN_MINE_ACTIVATION_DIST 250 

// defines for the star field

#define MAX_STARS                256   // number of stars in universe

#define STAR_PLANE_0             0    // far plane
#define STAR_PLANE_1             1    // near plane
#define STAR_PLANE_2             2

#define STAR_COLOR_0             8    // color of farthest star plane
#define STAR_COLOR_1             7
#define STAR_COLOR_2             15   // color of nearest star plane


// defines for particle system
#define PARTICLE_STATE_DEAD               0
#define PARTICLE_STATE_ALIVE              1

// types of particles
#define PARTICLE_TYPE_FLICKER             0
#define PARTICLE_TYPE_FADE                1 

// color of particle
#define PARTICLE_COLOR_RED                0
#define PARTICLE_COLOR_GREEN              1
#define PARTICLE_COLOR_BLUE               2
#define PARTICLE_COLOR_WHITE              3

#define MAX_PARTICLES                     128

// color ranges
#define COLOR_RED_START                   32
#define COLOR_RED_END                     47

#define COLOR_GREEN_START                 96
#define COLOR_GREEN_END                   111

#define COLOR_BLUE_START                  144
#define COLOR_BLUE_END                    159

#define COLOR_WHITE_START                 16
#define COLOR_WHITE_END                   31

// indices used to access data arrays in BOBs 
#define INDEX_WORLD_X                     8
#define INDEX_WORLD_Y                     9

// defines for the states of the main loop
#define GAME_STATE_INIT                  0
#define GAME_STATE_MENU                  1
#define GAME_STATE_RESTART               2
#define GAME_STATE_RUNNING               3
#define GAME_STATE_UPLINK                4
#define GAME_STATE_EXIT                  5
#define GAME_STATE_WAITING_FOR_EXIT      6
#define GAME_STATE_PAUSED                7

// the menu defines
#define MENU_STATE_MAIN                  0   // main menu state 
#define MENU_STATE_INST                  1   // instructions state
#define MENU_SEL_NEW_GAME                0
#define MENU_SEL_INSTRUCTIONS            1
#define MENU_SEL_EXIT                    2
#define MAX_INSTRUCTION_PAGES            6  

// defines for font
#define FONT_NUM_CHARS         96  // entire character set
#define FONT_WIDTH             12
#define FONT_HEIGHT            12
#define FONT_WIDTH_NEXT_NUM    8
#define FONT_WIDTH_NEXT_LOWER  7
#define FONT_WIDTH_NEXT_UPPER  8

// number of starting objects
#define NUM_ACTIVE_MINES             16
#define NUM_ACTIVE_STATIONS          8



void Start_Burst(int x, int y, int width, int height, int xv, int yv);
void Start_Rock(int x, int y, int size, int xv, int yv);

// TYPES //////////////////////////////////////////////////

// used to contain a single star
typedef struct STAR_TYP
{
	int x, y;                   // position of star
	UCHAR color;               // color of star
	int plane;                 // plane that star is in

} STAR, * STAR_PTR;


// a single particle
typedef struct PARTICLE_TYP
{
	int state;           // state of the particle
	int type;            // type of particle effect
	int x, y;             // world position of particle
	int xv, yv;           // velocity of particle
	int curr_color;      // the current rendering color of particle
	int start_color;     // the start color or range effect
	int end_color;       // the ending color of range effect
	int counter;         // general state transition timer
	int max_count;       // max value for counter

} PARTICLE, * PARTICLE_PTR;

class specialSPRITE :public SPRITE
{
public:
	int varsI[30];
	int state;
	int anim_state;
	int counter_1;
};

SURFACE andre;

specialSPRITE wraith;                   // the player 
aniDICT plasmaDict;
specialSPRITE plasma[MAX_PLASMA];       // plasma pulses
specialSPRITE rocks[MAX_ROCKS];         // the asteroids
specialSPRITE rock_s, rock_m, rock_l;   // the master templates for surface info
specialSPRITE bursts[MAX_BURSTS];       // the explosion bursts
aniDICT stationsDict;
specialSPRITE stations[MAX_STATIONS];   // the starbase stations
aniDICT minesDict;
specialSPRITE mines[MAX_MINES];         // the predator mines
specialSPRITE hud;                      // the art for the scanner hud
aniDICT stationsmallDict;
specialSPRITE stationsmall;             // small station bob

int rock_sizes[3] = { 96,56,32 }; // X,Y dims for scaler

STAR stars[MAX_STARS];          // the star field

PARTICLE particles[MAX_PARTICLES]; // the particles for the particle engine

// player state variables
int player_state = PLAYER_STATE_ALIVE;
int player_score = 0;   // the score
int player_ships = 3;   // ships left
int player_regen_counter = 0; // used to track when to regen
int player_damage = 0;   // damage of player
int player_counter = 0;   // used for state transition tracking
int player_regen_count = 0;   // used to regenerate player
int player_shield_count = 0;  // used to display shields
int ready_counter = 0,        // used to draw a little "get ready"
ready_state = 0;

float mine_tracking_rate = 2; // rate that mines track player

int win_counter = 0,          // tracks if player won
player_won = 0;

int station_id = -1,          // uplink station id
num_stations_destroyed = 0;

int intro_done = 0;           // flags if intro has played already

int game_paused = 0,
pause_debounce = 0,
huds_on = 1,
scanner_on = 1,
huds_debounce = 0,
scanner_debounce = 0;

// color palettes, so we don't have to reload all the time
laPALETTE         game_palette; // holds the main game palette
laPALETTE         menu_palette; // gee what do you think
laPALETTE        andre_palette; // for me

// positional and state info for player
float player_x = 0,
player_y = 0,
player_dx = 0,
player_dy = 0,
player_xv = 0,
player_yv = 0,
vel = 0;
//math
REAL cos_look16[16];
REAL sin_look16[16];

class MATHSHEET
{
public:
	MATHSHEET()
	{
		int ang = 0; // looping var to track angle

		for (ang = 0; ang < 16; ang++)
		{
			float fang = PI * (ang * 22.5) / 180;

			cos_look16[ang] = -cos(fang + PI / 2);
			sin_look16[ang] = -sin(fang + PI / 2);
		}
	}
}mathSheet;

// sound id's
SOUND sound[80];
int intro_music_id = -1,
main_music_id = -1,
ready_id = -1,
engines_id = -1,
scream_id = -1,
game_over_id = -1,
mine_powerup_id = -1,
deactivate_id = -1,
main_menu_id = -1,
beep0_id = -1,
beep1_id = -1,
station_blow_id = -1,
station_throb_id = -1;

int expl_ids[MAX_EXPL_SOUNDS] = { -1,-1,-1,-1,-1,-1,-1,-1 };
int fire_ids[MAX_FIRE_SOUNDS] = { -1,-1,-1,-1,-1,-1,-1,-1 };
int game_state = GAME_STATE_INIT;   // initial game state


TSTRING path = TEXT("./Resource/demo18/");
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

void Pad_Name(LPCTCH filename, LPCTCH extension, LPTCH padstring, int num)
{
	// build up blank padstring
	_stprintf(padstring, TEXT("%s%04d.%s"), filename, num, extension);
}

bool Load_Andre(void)
{
	GRAPHIC::BITMAP bitmap;
	if (bitmap.load(path + TEXT("OUTART/ENEMYAI.DAT")))
	{
		bitmap.getPalette(andre_palette);
		SURFACE temp;
		temp.createFromBitmap(bitmap);
		andre.createFromSurface(216, 166, temp, 320 - 216 / 2, 240 - 166 / 2);

		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////

void Init_Reset_Particles(void)
{
	// this function serves as both an init and reset for the particles

	// loop thru and reset all the particles to dead
	for (int index = 0; index < MAX_PARTICLES; index++)
	{
		particles[index].state = PARTICLE_STATE_DEAD;
		particles[index].type = PARTICLE_TYPE_FADE;
		particles[index].x = 0;
		particles[index].y = 0;
		particles[index].xv = 0;
		particles[index].yv = 0;
		particles[index].start_color = 0;
		particles[index].end_color = 0;
		particles[index].curr_color = 0;
		particles[index].counter = 0;
		particles[index].max_count = 0;
	}

}

/////////////////////////////////////////////////////////////////////////

void Start_Particle(int type, int color, int count, int x, int y, int xv, int yv)
{
	// this function starts a single particle

	int pindex = -1; // index of particle

	// first find open particle
	for (int index = 0; index < MAX_PARTICLES; index++)
		if (particles[index].state == PARTICLE_STATE_DEAD)
		{
			// set index
			pindex = index;
			break;
		}    

 // did we find one
	if (pindex == -1)
		return;

	// set general state info
	particles[pindex].state = PARTICLE_STATE_ALIVE;
	particles[pindex].type = type;
	particles[pindex].x = x;
	particles[pindex].y = y;
	particles[pindex].xv = xv;
	particles[pindex].yv = yv;
	particles[pindex].counter = 0;
	particles[pindex].max_count = count;

	// set color ranges, always the same
	switch (color)
	{
	case PARTICLE_COLOR_RED:
	{
		particles[pindex].start_color = COLOR_RED_START;
		particles[pindex].end_color = COLOR_RED_END;
	} break;

	case PARTICLE_COLOR_GREEN:
	{
		particles[pindex].start_color = COLOR_GREEN_START;
		particles[pindex].end_color = COLOR_GREEN_END;
	} break;

	case PARTICLE_COLOR_BLUE:
	{
		particles[pindex].start_color = COLOR_BLUE_START;
		particles[pindex].end_color = COLOR_BLUE_END;
	} break;

	case PARTICLE_COLOR_WHITE:
	{
		particles[pindex].start_color = COLOR_WHITE_START;
		particles[pindex].end_color = COLOR_WHITE_END;
	} break;

	break;

	} 

// what type of particle is being requested
	if (type == PARTICLE_TYPE_FLICKER)
	{
		// set current color
		particles[pindex].curr_color = RAND_RANGE(particles[pindex].start_color, particles[pindex].end_color);

	} 
	else
	{
		// particle is fade type
		// set current color
		particles[pindex].curr_color = particles[pindex].start_color;
	}

}

////////////////////////////////////////////////////////////////////////////////

void Start_Particle_Explosion(int type, int color, int count,
	int x, int y, int xv, int yv, int num_particles)
{
	// this function starts a particle explosion at the given position and velocity

	while (--num_particles >= 0)
	{
		// compute random trajectory angle
		int ang = rand() % 16;

		// compute random trajectory velocity
		float vel = 2 + rand() % 4;

		Start_Particle(type, color, count,
			x + RAND_RANGE(-4, 4), y + RAND_RANGE(-4, 4),
			xv + cos_look16[ang] * vel, yv + sin_look16[ang] * 16);

	}

}

////////////////////////////////////////////////////////////////////////////////

void Draw_Particles(void)
{
	// this function draws all the particles

	BeginDrawOn();

	for (int index = 0; index < MAX_PARTICLES; index++)
	{
		// test if particle is alive
		if (particles[index].state == PARTICLE_STATE_ALIVE)
		{
			// render the particle, perform world to screen transform
			int x = particles[index].x - player_x + (SCREEN_WIDTH / 2);
			int y = particles[index].y - player_y + (SCREEN_HEIGHT / 2);

			// test for clip
			if (x >= SCREEN_WIDTH || x < 0 || y >= SCREEN_HEIGHT || y < 0)
				continue;

			// draw the pixel
			GRAPHIC::SetPixel(x, y, curPalette[particles[index].curr_color]);

		}

	}

	EndDrawOn();

}

////////////////////////////////////////////////////////////////////

void Move_Particles(void)
{
	// this function moves and animates all particles

	for (int index = 0; index < MAX_PARTICLES; index++)
	{
		// test if this particle is alive
		if (particles[index].state == PARTICLE_STATE_ALIVE)
		{
			// translate particle
			particles[index].x += particles[index].xv;
			particles[index].y += particles[index].yv;

			// now based on type of particle perform proper animation
			if (particles[index].type == PARTICLE_TYPE_FLICKER)
			{
				// simply choose a color in the color range and assign it to the current color
				particles[index].curr_color = RAND_RANGE(particles[index].start_color, particles[index].end_color);

				// now update counter
				if (++particles[index].counter >= particles[index].max_count)
				{
					// kill the particle
					particles[index].state = PARTICLE_STATE_DEAD;

				}

			}
			else
			{
				// must be a fade, be careful!
				// test if it's time to update color
				if (++particles[index].counter >= particles[index].max_count)
				{
					// reset counter
					particles[index].counter = 0;

					// update color
					if (++particles[index].curr_color > particles[index].end_color)
					{
						// transition is complete, terminate particle
						particles[index].state = PARTICLE_STATE_DEAD;

					}

				}

			}

		}

	}

}

////////////////////////////////////////////////////////////////////////////////////

void Init_Stars(void)
{
	// this function initializes all the stars in the star field

	int index; // looping variable

	for (index = 0; index < MAX_STARS; index++)
	{

		// select plane that star will be in

		switch (rand() % 3)
		{
		case STAR_PLANE_0:
		{
			stars[index].color = STAR_COLOR_0;
			stars[index].plane = STAR_PLANE_0;
		} break;

		case STAR_PLANE_1:
		{
			stars[index].color = STAR_COLOR_1;
			stars[index].plane = STAR_PLANE_1;
		} break;

		case STAR_PLANE_2:
		{
			stars[index].color = STAR_COLOR_2;
			stars[index].plane = STAR_PLANE_2;
		} break;

		default:break;

		}

		// set fields that aren't plane specific
		stars[index].x = rand() % SCREEN_WIDTH;   // change this latter to reflect clipping
		stars[index].y = rand() % SCREEN_HEIGHT;  // region

	}

}

/////////////////////////////////////////////////////////////////////////////

void Move_Stars(void)
{
	// this function moves the star field, note that the star field is always
	// in screen coordinates, otherwise, we would need thousands of stars to
	// fill up the universe instead of 50!

	int index,   // looping variable
		star_x,  // used as fast aliases to star position
		star_y,
		plane_0_dx,
		plane_0_dy,
		plane_1_dx,
		plane_1_dy,
		plane_2_dx,
		plane_2_dy;

	// pre-compute plane translations
	plane_0_dx = -int(player_xv) >> 2;
	plane_0_dy = -int(player_yv) >> 2;

	plane_1_dx = -int(player_xv) >> 1;
	plane_1_dy = -int(player_yv) >> 1;

	plane_2_dx = -int(player_xv);
	plane_2_dy = -int(player_yv);

	// move all the stars based on the motion of the player

	for (index = 0; index < MAX_STARS; index++)
	{
		// locally cache star position to speed up calculations
		star_x = stars[index].x;
		star_y = stars[index].y;

		// test which star field star is in so it is translated with correct
		// perspective

		switch (stars[index].plane)
		{
		case STAR_PLANE_0:
		{
			// move the star based on differntial motion of player
			// far plane is divided by 4

			star_x += plane_0_dx;
			star_y += plane_0_dy;

		} break;

		case STAR_PLANE_1:
		{
			// move the star based on differntial motion of player
			// middle plane is divided by 2

			star_x += plane_1_dx;
			star_y += plane_1_dy;

		} break;

		case STAR_PLANE_2:
		{
			// move the star based on differntial motion of player
			// near plane is divided by 1

			star_x += plane_2_dx;
			star_y += plane_2_dy;

		} break;

		} // end switch plane

  // test if a star has flown off an edge

		if (star_x >= SCREEN_WIDTH)
			star_x = star_x - SCREEN_WIDTH;
		else if (star_x < 0)
				star_x = SCREEN_WIDTH + star_x;

		if (star_y >= SCREEN_HEIGHT)
			star_y = star_y - SCREEN_HEIGHT;
		else if (star_y < 0)
				star_y = SCREEN_HEIGHT + star_y;

		// reset stars position in structure
		stars[index].x = star_x;
		stars[index].y = star_y;

	}

}

/////////////////////////////////////////////////////////////////////////////

void Draw_Stars(void)
{
	// this function draws all the stars

	// lock back surface
	BeginDrawOn();

	// draw all the stars
	for (int index = 0; index < MAX_STARS; index++)
	{
		// draw stars 
		GRAPHIC::SetPixel(stars[index].x, stars[index].y, curPalette[stars[index].color]);

	}


	EndDrawOn();

}

///////////////////////////////////////////////////

void Draw_Scanner(void)
{
	// this function draws all the stars

	int index, sx, sy; // looping and position

	BeginDrawOn();

	// draw all the rocks
	for (index = 0; index < MAX_ROCKS; index++)
	{
		// draw rock blips
		if (rocks[index].state == ROCK_STATE_ON)
		{
			sx = ((rocks[index].varsI[INDEX_WORLD_X] - UNIVERSE_MIN_X) >> 7) + (SCREEN_WIDTH / 2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
			sy = ((rocks[index].varsI[INDEX_WORLD_Y] - UNIVERSE_MIN_Y) >> 7) + 32;

			GRAPHIC::SetPixel(sx, sy, curPalette[8]);
		}

	}

	// draw all the mines
	for (index = 0; index < MAX_MINES; index++)
	{
		// draw gunship blips
		if (mines[index].state == MINE_STATE_ALIVE)
		{
			sx = ((mines[index].varsI[INDEX_WORLD_X] - UNIVERSE_MIN_X) >> 7) + (SCREEN_WIDTH / 2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
			sy = ((mines[index].varsI[INDEX_WORLD_Y] - UNIVERSE_MIN_Y) >> 7) + 32;

			GRAPHIC::SetPixel(sx, sy, curPalette[12]);
			GRAPHIC::SetPixel(sx, sy + 1, curPalette[12]);

		}

	}

	EndDrawOn();

	// draw all the stations
	for (index = 0; index < MAX_STATIONS; index++)
	{
		// draw station blips
		if (stations[index].state == STATION_STATE_ALIVE)
		{
			sx = ((stations[index].varsI[INDEX_WORLD_X] - UNIVERSE_MIN_X) >> 7) + (SCREEN_WIDTH / 2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
			sy = ((stations[index].varsI[INDEX_WORLD_Y] - UNIVERSE_MIN_Y) >> 7) + 32;

			// test for state
			if (stations[index].anim_state == STATION_SHIELDS_ANIM_ON)
			{
				stationsmall.x = sx - 3;
				stationsmall.y = sy - 3;
				stationsmall.drawOn(0);


			}
			else
			{

				stationsmall.x = sx - 3;
				stationsmall.y = sy - 3;
				stationsmall.drawOn(1);


			}

		}

	}


	// unlock the secondary surface
	BeginDrawOn();

	// draw player as white blip
	sx = ((int(player_x) - UNIVERSE_MIN_X) >> 7) + (SCREEN_WIDTH / 2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
	sy = ((int(player_y) - UNIVERSE_MIN_Y) >> 7) + 32;

	int col = rand() % 256;

	GRAPHIC::SetPixel(sx, sy, curPalette[col]);
	GRAPHIC::SetPixel(sx + 1, sy, curPalette[col]);
	GRAPHIC::SetPixel(sx, sy + 1, curPalette[col]);
	GRAPHIC::SetPixel(sx + 1, sy + 1, curPalette[col]);

	// unlock the secondary surface
	EndDrawOn();

	// now draw the art around the edges

	hud.x = 320 - 64;
	hud.y = 32 - 4;
	hud.drawOn(0);

	hud.x = 320 + 64 - 16;
	hud.y = 32 - 4;
	hud.drawOn(1);

	hud.x = 320 - 64;
	hud.y = 32 + 128 - 20;
	hud.drawOn(2);


	hud.x = 320 + 64 - 16;
	hud.y = 32 + 128 - 20;
	hud.drawOn(3);


}

void Start_Mine(int override, int x, int y, int ai_state);
///////////////////////////////////////////////////////////

void Init_Stations(void)
{
	// this function loads and initializes the stations to a known state


	// create the first bob
	stations[0].x = 0;
	stations[0].y = 0;

	TCHAR buffer[80];
	// load animation frames 
	stationsDict.resize(17);
	SURFACE temp;
	for (int frame = 0; frame <= 16; frame++)
	{
		// load the rocks imagery 

		Pad_Name(path + TEXT("OUTART/STATION"), TEXT("BMP"), buffer, frame);
		temp.createFromBitmap(buffer);
		stationsDict[frame].createFromSurface(192, 144, temp, 0, 0);

	}
	stations[0].content.setDict(&stationsDict);
	// set state to off
	stations[0].state = STATION_STATE_ALIVE;

	// set anim state
	stations[0].anim_state = STATION_SHIELDS_ANIM_ON;

	// set damage to 0
	stations[0].varsI[INDEX_STATION_DAMAGE] = 0;

	// set animation rate
	stations[0].setAniSpeed(15);

	// load in the shield on/off animations
	stations[0].content.resize(2);
	stations[0].content[STATION_SHIELDS_ANIM_ON] = ANIM(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	stations[0].content[STATION_SHIELDS_ANIM_OFF] = ANIM(16);

	// set animation to on
	stations[0].setAniString(STATION_SHIELDS_ANIM_ON);

	// make copies
	for (int ship = 1; ship < MAX_STATIONS; ship++)
	{
		stations[ship].clone(stations[0]);
	}

	stationsmall.x = 0;
	stationsmall.y = 0;


	temp.createFromBitmap(path + TEXT("OUTART/STATIONSMALL8.BMP"));

	stationsmallDict.resize(2);
	stationsmallDict[0].createFromSurface(8, 8, temp, 0 * (8 + 1) + 1, 0 * (8 + 1) + 1);
	stationsmallDict[1].createFromSurface(8, 8, temp, 1 * (8 + 1) + 1, 0 * (8 + 1) + 1);
	stationsmall.content.setDict(&stationsmallDict);
	stationsmall.content.autoOrder();
}

///////////////////////////////////////////////////////////

void Reset_Stations(void)
{
	// this function resets all the stations in preparation for another run

	for (int index = 0; index < MAX_STATIONS; index++)
	{
		// set state to off
		stations[index].state = STATION_STATE_DEAD;

		// set anim state
		stations[index].anim_state = STATION_SHIELDS_ANIM_ON;

		// set damage to 0
		stations[index].varsI[INDEX_STATION_DAMAGE] = 0;

	}

}

///////////////////////////////////////////////////////////

void Start_Station(int override = 0, int x = 0, int y = 0)
{
	// this functions starts a station, note that if override = 1
	// then the function uses the sent data otherwise it's random

	// first find an available stations
	for (int index = 0; index < MAX_STATIONS; index++)
	{
		// is this one dead
		if (stations[index].state == STATION_STATE_DEAD)
		{
			// position the station

			int xpos = RAND_RANGE((UNIVERSE_MIN_X + 256), (UNIVERSE_MAX_X - 256));
			int ypos = RAND_RANGE((UNIVERSE_MIN_Y + 256), (UNIVERSE_MAX_Y - 256));

			// set position
			stations[index].varsI[INDEX_WORLD_X] = xpos;
			stations[index].varsI[INDEX_WORLD_Y] = ypos;

			// start a mine up in the vicinity
			int ang = rand() % 16;
			float mine_x = xpos + STATION_RANGE_RING * cos_look16[ang];
			float mine_y = ypos + STATION_RANGE_RING * sin_look16[ang];

			// start a deactivated mine
			Start_Mine(1, mine_x, mine_y, MINE_STATE_AI_SLEEP);

			// set velocity
			stations[index].vx = 0;
			stations[index].vy = 0;

			// set remaining state variables
			stations[index].state = STATION_STATE_ALIVE;

			// set animation to on
			stations[index].setAniString(STATION_SHIELDS_ANIM_ON);

			// set anim state
			stations[index].anim_state = STATION_SHIELDS_ANIM_ON;

			// set damage to 0
			stations[index].varsI[INDEX_STATION_DAMAGE] = 0;

			// done so exit
			return;

		}

	}

}

////////////////////////////////////////////////////////////

void Move_Stations(void)
{
	// this function moves/animates all the stations

	for (int index = 0; index < MAX_STATIONS; index++)
	{
		// test if station is alive
		if (stations[index].state == STATION_STATE_ALIVE)
		{
			// move the stations
			stations[index].varsI[INDEX_WORLD_X] += stations[index].vx;
			stations[index].varsI[INDEX_WORLD_Y] += stations[index].vy;

			// test for boundaries
			if (stations[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X)
			{
				stations[index].varsI[INDEX_WORLD_X] = UNIVERSE_MIN_X;
			}
			else if (stations[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X)
			{
				stations[index].varsI[INDEX_WORLD_X] = UNIVERSE_MAX_X;
			}

			if (stations[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y)
			{
				stations[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MIN_Y;
			}
			else if (stations[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y)
			{
				stations[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MAX_Y;
			}

			// test for damage level
			if (stations[index].varsI[INDEX_STATION_DAMAGE] > (MAX_STATION_DAMAGE / 4) &&
				(rand() % (20 - (stations[index].varsI[INDEX_STATION_DAMAGE] >> 3))) == 1)
			{
				int width = 20 + rand() % 60;

				// start a burst
				Start_Burst(stations[index].varsI[INDEX_WORLD_X] - (stations[index].getWidth() * .5) + RAND_RANGE(0, stations[index].getWidth()),
					stations[index].varsI[INDEX_WORLD_Y] - (stations[index].getHeight() * .5) + RAND_RANGE(0, stations[index].getHeight()),
					width, (width >> 2) + (width >> 1),
					int(stations[index].vx) * .5, int(stations[index].vy) * .5);

				// add some particles

			}

		}

	}

}

///////////////////////////////////////////////////////////

void Draw_Stations(void)
{
	// this function draws all the stations

	for (int index = 0; index < MAX_STATIONS; index++)
	{
		// test if station is alive
		if (stations[index].state == STATION_STATE_ALIVE)
		{
			// transform to screen coords
			stations[index].x = stations[index].varsI[INDEX_WORLD_X] - (stations[index].getWidth() >> 1) - player_x + (SCREEN_WIDTH / 2);
			stations[index].y = stations[index].varsI[INDEX_WORLD_Y] - (stations[index].getHeight() >> 1) - player_y + (SCREEN_HEIGHT / 2);

			// draw the station
			stations[index].drawOn();

			// animate the station
			stations[index].animate();

		}

	}

}
////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

void Init_Mines(void)
{
	// this function loads and initializes the mines to a known state


	mines[0].x = 0;
	mines[0].y = 0;

	// load animation frames 
	SURFACE temp;
	minesDict.resize(16);
	for (int frame = 0; frame < 16; frame++)
	{
		// load the mine
		TCHAR buffer[80];
		Pad_Name(path + TEXT("OUTART/PREDMINE"), TEXT("BMP"), buffer, frame);
		temp.createFromBitmap(buffer);

		// load the actual .BMP
		minesDict[frame].createFromSurface(48, 36, temp, 0, 0);

	}
	mines[0].content.setDict(&minesDict);
	mines[0].content.autoOrder();
	// set state to off
	mines[0].state = MINE_STATE_DEAD;

	// set ai state to sleep mode
	mines[0].varsI[INDEX_MINE_AI_STATE] = MINE_STATE_AI_SLEEP;

	// set damage to 0
	mines[0].varsI[INDEX_MINE_DAMAGE] = 0;

	// set contact count to 0
	mines[0].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

	// set animation rate
	mines[0].setAniSpeed(3);

	// make copies
	for (int mine = 1; mine < MAX_MINES; mine++)
	{
		mines[mine].clone(mines[0]);
	}

}

////////////////////////////////////////////////////////////

void Reset_Mines(void)
{
	// this functions resets all the mines

	// make copies
	for (int mine = 0; mine < MAX_MINES; mine++)
	{
		// set state to off
		mines[mine].state = MINE_STATE_DEAD;

		// set ai state to sleep mode
		mines[mine].varsI[INDEX_MINE_AI_STATE] = MINE_STATE_AI_SLEEP;

		// set damage to 0
		mines[mine].varsI[INDEX_MINE_DAMAGE] = 0;

		// set contact count to 0
		mines[mine].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

	}

}

///////////////////////////////////////////////////////////

void Start_Mine(int override = 0, int x = 0, int y = 0, int ai_state = MINE_STATE_AI_ACTIVATED)
{
	// this functions starts a mine, note that if override = 1
	// then the function uses the sent data otherwise it's random

	// first find an available stations
	for (int index = 0; index < MAX_MINES; index++)
	{
		// is this one dead
		if (mines[index].state == MINE_STATE_DEAD)
		{

			if (!override)
			{
				// position the mine
				int xpos = RAND_RANGE((UNIVERSE_MIN_X + 256), (UNIVERSE_MAX_X - 256));
				int ypos = RAND_RANGE((UNIVERSE_MIN_Y + 256), (UNIVERSE_MAX_Y - 256));

				// set position
				mines[index].varsI[INDEX_WORLD_X] = xpos;
				mines[index].varsI[INDEX_WORLD_Y] = ypos;

				// set velocity
				mines[index].vx = RAND_RANGE(-8, 8);
				mines[index].vy = RAND_RANGE(-8, 8);

				// set remaining state variables
				mines[index].state = MINE_STATE_ALIVE;

				// set damage to 0
				mines[index].varsI[INDEX_MINE_DAMAGE] = 0;

				// set ai state
				mines[index].varsI[INDEX_MINE_AI_STATE] = ai_state;

				// set contact count
				mines[index].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

				// done so exit
				return;

			}
			else
			{
				// set position
				mines[index].varsI[INDEX_WORLD_X] = x;
				mines[index].varsI[INDEX_WORLD_Y] = y;

				// set velocity
				mines[index].vx = 0;
				mines[index].vy = 0;

				// set remaining state variables
				mines[index].state = MINE_STATE_ALIVE;

				// set damage to 0
				mines[index].varsI[INDEX_MINE_DAMAGE] = 0;

				// set ai state
				mines[index].varsI[INDEX_MINE_AI_STATE] = ai_state;

				// set contact count
				mines[index].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

				// done so exit
				return;

			}


		}

	}

}

////////////////////////////////////////////////////////////

void Move_Mines(void)
{
	// this function moves/animates all the mines

	for (int index = 0; index < MAX_MINES; index++)
	{
		// test if station is alive
		if (mines[index].state == MINE_STATE_ALIVE)
		{

			// test for activation
			if (mines[index].varsI[INDEX_MINE_AI_STATE] == MINE_STATE_AI_ACTIVATED)
			{

				// move the mines
				mines[index].varsI[INDEX_WORLD_X] += mines[index].vx;
				mines[index].varsI[INDEX_WORLD_Y] += mines[index].vy;


				// add damage trails
				if (mines[index].varsI[INDEX_MINE_DAMAGE] > (MAX_MINE_DAMAGE >> 1) &&
					(rand() % 3) == 1)
				{
					Start_Particle(PARTICLE_TYPE_FLICKER, PARTICLE_COLOR_WHITE, 30 + rand() % 25,
						mines[index].varsI[INDEX_WORLD_X] + RAND_RANGE(-4, 4),
						mines[index].varsI[INDEX_WORLD_Y] + RAND_RANGE(-4, 4),
						(mines[index].vx * .125), (mines[index].vy * .125));

					Start_Particle(PARTICLE_TYPE_FADE, PARTICLE_COLOR_RED, 5,
						mines[index].varsI[INDEX_WORLD_X] + RAND_RANGE(-4, 4),
						mines[index].varsI[INDEX_WORLD_Y] + RAND_RANGE(-4, 4),
						(mines[index].vx * .125), (mines[index].vy * .125));

				}

				// tracking algorithm

				// compute vector toward player
				float vx = player_x - mines[index].varsI[INDEX_WORLD_X];
				float vy = player_y - mines[index].varsI[INDEX_WORLD_Y];

				// normalize vector (sorta :)
				float length = DistanceFast(vx, vy);


				// only track if reasonable close
				if (length < MIN_MINE_TRACKING_DIST)
				{
					vx = mine_tracking_rate * vx / length;
					vy = mine_tracking_rate * vy / length;

					// add velocity vector to current velocity
					mines[index].vx += vx;
					mines[index].vy += vy;

					// add a little noise
					if ((rand() % 10) == 1)
					{
						vx = RAND_RANGE(-1, 1);
						vy = RAND_RANGE(-1, 1);
						mines[index].vx += vx;
						mines[index].vy += vy;
					}

					// test velocity vector of mines
					length = DistanceFast(mines[index].vx, mines[index].vy);

					// test for velocity overflow and slow
					if (length > MAX_MINE_VELOCITY)
					{
						// slow down
						mines[index].vx *= 0.75;
						mines[index].vy *= 0.75;

					}

				}
				else
				{
					// add a random velocity component
					if ((rand() % 30) == 1)
					{
						vx = RAND_RANGE(-2, 2);
						vy = RAND_RANGE(-2, 2);

						// add velocity vector to current velocity
						mines[index].vx += vx;
						mines[index].vy += vy;

						// test velocity vector of mines
						length = DistanceFast(mines[index].vx, mines[index].vy);

						// test for velocity overflow and slow
						if (length > MAX_MINE_VELOCITY)
						{
							// slow down
							mines[index].vx *= 0.75;
							mines[index].vy *= 0.75;

						}

					}

				}

			}
			else
			{
				// compute dist from player
				float vx = player_x - mines[index].varsI[INDEX_WORLD_X];
				float vy = player_y - mines[index].varsI[INDEX_WORLD_Y];

				float length = DistanceFast(vx, vy);

				if (length < MIN_MINE_ACTIVATION_DIST)
				{
					mines[index].varsI[INDEX_MINE_AI_STATE] = MINE_STATE_AI_ACTIVATED;

					// sound off
					sound[mine_powerup_id].play();
				}

			}

			// test for boundaries
			if (mines[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X)
			{
				mines[index].varsI[INDEX_WORLD_X] = UNIVERSE_MIN_X;
			}
			else if (mines[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X)
			{
				mines[index].varsI[INDEX_WORLD_X] = UNIVERSE_MAX_X;
			}

			if (mines[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y)
			{
				mines[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MIN_Y;
			}
			else if (mines[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y)
			{
				mines[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MAX_Y;
			}

			// check for collision with player
			if (player_state == PLAYER_STATE_ALIVE && player_state == PLAYER_STATE_ALIVE &&
				Collision_Test(player_x - (wraith.getWidth() * .5),
					player_y - (wraith.getHeight() * .5),
					wraith.getWidth(), wraith.getHeight(),
					mines[index].varsI[INDEX_WORLD_X] - (mines[index].getWidth() * .5),
					mines[index].varsI[INDEX_WORLD_Y] - (mines[index].getHeight() * .5),
					mines[index].getWidth(), mines[index].getHeight()))
			{

				// test for contact count
				if (++mines[index].varsI[INDEX_MINE_CONTACT_COUNT] > MAX_MINE_CONTACT_COUNT)
				{

					// add to players score
					player_score += 250;

					// damage player
					player_damage += 30;

					// engage shields
					player_shield_count = 3;

					int width = 30 + rand() % 40;

					// start a burst
					Start_Burst(mines[index].varsI[INDEX_WORLD_X],
						mines[index].varsI[INDEX_WORLD_Y],
						width, (width * .5) + (width * .25),
						int(mines[index].vx) * .5, int(mines[index].vy) * .5);


					// kill the mine
					mines[index].state = MINE_STATE_DEAD;

					// start a new mine
					Start_Mine();

					// process next mine
					continue;

				}

			}
			else // no collision or other problem, so reset
			{
				mines[index].varsI[INDEX_MINE_CONTACT_COUNT] = 0;
			}

		}

	}

}

///////////////////////////////////////////////////////////

void Draw_Mines(void)
{
	// this function draws all the mines

	for (int index = 0; index < MAX_MINES; index++)
	{
		// test if mine is alive
		if (mines[index].state == MINE_STATE_ALIVE)
		{
			// transform to screen coords
			mines[index].x = mines[index].varsI[INDEX_WORLD_X] - (mines[index].getWidth() >> 1) - player_x + (SCREEN_WIDTH / 2);
			mines[index].y = mines[index].varsI[INDEX_WORLD_Y] - (mines[index].getHeight() >> 1) - player_y + (SCREEN_HEIGHT / 2);

			// draw the station
			mines[index].drawOn();

			// animate the mine
			mines[index].animate();

			// draw info text above mine

			gPrintf(mines[index].x - 32, mines[index].y - 16, RGB(0, 255, 0), TEXT("VELOCITY[%f, %f]"), mines[index].vx, mines[index].vy);

		}

	}

}




//////////////////////////////////////////////////////


void Init_Plasma(void)
{
	// this function initializes and loads all the plasma 
	// weapon pulses


	// load the plasma imagery 

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("OUTART/ENERGY8.BMP"));

	plasma[0].x = 0;
	plasma[0].y = 0;
	// load animation frames
	plasmaDict.resize(16);
	for (int frame = 0; frame < 16; frame++)
		plasmaDict[frame].createFromSurface(8, 8, temp, (frame % 8) * (8 + 1) + 1, (frame / 8) * (8 + 1) + 1);

	plasma[0].content.setDict(&plasmaDict);
	// set animation rate
	plasma[0].setAniSpeed(1);

	// load animations
	plasma[0].content.resize(2);
	plasma[0].content[PLASMA_ANIM_PLAYER] = ANIM(0, 1, 2, 3, 4, 5, 6, 7);
	plasma[0].content[PLASMA_ANIM_ENEMY] = ANIM(8, 9, 10, 11, 12, 13, 14, 15);

	// set state to off
	plasma[0].state = PLASMA_STATE_OFF;

	for (int pulse = 1; pulse < MAX_PLASMA; pulse++)
	{
		plasma[pulse].clone(plasma[0]);
	}


}

/////////////////////////////////////////////////////////

void Reset_Plasma(void)
{
	// this function resets all the plasma pulses
	for (int pulse = 0; pulse < MAX_PLASMA; pulse++)
	{
		plasma[pulse].state = PLASMA_STATE_OFF;
	}

}

///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

void Move_Plasma(void)
{
	// this function moves all the plasma pulses and checks for
	// collision with the rocks

	for (int index = 0; index < MAX_PLASMA; index++)
	{
		// test if plasma pulse is in flight
		if (plasma[index].state == PLASMA_STATE_ON)
		{
			// move the pulse 
			plasma[index].varsI[INDEX_WORLD_X] += plasma[index].vx;
			plasma[index].varsI[INDEX_WORLD_Y] += plasma[index].vy;

			// test for boundaries
			if ((++plasma[index].counter_1 > PLASMA_RANGE_1) ||
				(plasma[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X) ||
				(plasma[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X) ||
				(plasma[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y) ||
				(plasma[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y))
			{
				// kill the pulse
				plasma[index].state = PLASMA_STATE_OFF;
				plasma[index].counter_1 = 0;

				// move to next pulse
				continue;
			}


			// test for mines
			for (int mine = 0; mine < MAX_MINES; mine++)
			{
				if (mines[mine].state == MINE_STATE_ALIVE &&
					plasma[index].anim_state == PLASMA_ANIM_PLAYER)
				{
					// test for collision 
					if (Collision_Test(plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].getWidth() * .5),
						plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].getHeight() * .5),
						plasma[index].getWidth(), plasma[index].getHeight(),
						mines[mine].varsI[INDEX_WORLD_X] - (mines[mine].getWidth() * .5),
						mines[mine].varsI[INDEX_WORLD_Y] - (mines[mine].getHeight() * .5),
						mines[mine].getWidth(), mines[mine].getHeight()))
					{
						// kill pulse
						plasma[index].state = PLASMA_STATE_OFF;
						plasma[index].counter_1 = 0;

						// do damage to mine
						mines[mine].varsI[INDEX_MINE_DAMAGE] += (1 + rand() % 3);

						// increase the damage on the mine and test for death
						if (mines[mine].varsI[INDEX_MINE_DAMAGE] > MAX_MINE_DAMAGE)
						{
							// kill the ship    
							mines[mine].state = MINE_STATE_DEAD;

							// start a new mine
							Start_Mine();

							// add to players score
							player_score += 250;

						}

						int width = 30 + rand() % 40;

						// start a burst
						Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
							plasma[index].varsI[INDEX_WORLD_Y],
							width, (width * .5) + (width * .25),
							int(mines[mine].vx) * .5, int(mines[mine].vy) * .5);
						break;
					}

				}

			}


			///////////////////////////////////////////////////////

			// test for stations
			for (int station = 0; station < MAX_STATIONS; station++)
			{
				if (stations[station].state == STATION_STATE_ALIVE &&
					plasma[index].anim_state == PLASMA_ANIM_PLAYER)
				{
					// test for collision 
					if ( //stations[station].anim_state == STATION_SHIELDS_ANIM_OFF &&
						Collision_Test(plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].getWidth() * .5),
							plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].getHeight() * .5),
							plasma[index].getWidth(), plasma[index].getHeight(),
							stations[station].varsI[INDEX_WORLD_X] - (stations[station].getWidth() * .5),
							stations[station].varsI[INDEX_WORLD_Y] - (stations[station].getHeight() * .5),
							stations[station].getWidth(), stations[station].getHeight()))
					{
						// kill pulse
						plasma[index].state = PLASMA_STATE_OFF;
						plasma[index].counter_1 = 0;

						// do damage to station
						stations[station].varsI[INDEX_STATION_DAMAGE] += (1 + rand() % 3);

						// increase the damage on the station and test for death
						if (stations[station].varsI[INDEX_STATION_DAMAGE] > MAX_STATION_DAMAGE)
						{
							// kill the station
							stations[station].state = STATION_STATE_DEAD;

							// add to players score
							player_score += 10000;

							// has played won
							if (++num_stations_destroyed >= NUM_ACTIVE_STATIONS)
							{
								win_counter = 0;
								player_won = 1;
							}

							// make big sound
							sound[station_blow_id].play();

						}

						// start a burst
						Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
							plasma[index].varsI[INDEX_WORLD_Y],
							40 + rand() % 20, 30 + rand() % 16,
							int(stations[station].vx) * .5, int(stations[station].vy) * .5);
						break;
					}

				}

			}


			///////////////////////////////////////////////////////


				// test for collision with player
			if (plasma[index].anim_state == PLASMA_ANIM_ENEMY && player_state == PLAYER_STATE_ALIVE &&
				Collision_Test(plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].getWidth() * .5),
					plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].getHeight() * .5),
					plasma[index].getWidth(), plasma[index].getHeight(),
					player_x - (wraith.getWidth() * .5),
					player_y - (wraith.getHeight() * .5),
					wraith.getWidth(), wraith.getHeight()))
			{

				Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
					plasma[index].varsI[INDEX_WORLD_Y],
					40 + rand() % 20, 30 + rand() % 16,
					int(player_xv) * .5, int(player_yv) * .5);

				// update players damage
				player_damage += 2;


				// engage shields
				player_shield_count = 3;

				// kill the pulse
				plasma[index].state = PLASMA_STATE_OFF;
				plasma[index].counter_1 = 0;

				// plasma is dead
				continue;
			}

			////////////////////////////////////////////////////////


				   // test for collision with rocks
			for (int rock = 0; rock < MAX_ROCKS; rock++)
			{
				if (rocks[rock].state == ROCK_STATE_ON)
				{
					// test for collision 
					if (Collision_Test(plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].getWidth() * .5),
						plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].getHeight() * .5),
						plasma[index].getWidth(), plasma[index].getHeight(),
						rocks[rock].varsI[INDEX_WORLD_X] - (rocks[rock].getWidth() * .5),
						rocks[rock].varsI[INDEX_WORLD_Y] - (rocks[rock].getHeight() * .5),
						rocks[rock].getWidth(), rocks[rock].getHeight()))
					{
						// kill pulse
						plasma[index].state = PLASMA_STATE_OFF;
						plasma[index].counter_1 = 0;

						switch (rocks[rock].varsI[0])
						{
						case ROCK_LARGE:
						{
							// start explosion
							Start_Burst(plasma[index].varsI[INDEX_WORLD_X], plasma[index].varsI[INDEX_WORLD_Y],
								68 + rand() % 12, 54 + rand() % 10,
								rocks[rock].vx * .5, rocks[rock].vy * .5);

						} break;

						case ROCK_MEDIUM:
						{
							// start explosion
							Start_Burst(plasma[index].varsI[INDEX_WORLD_X], plasma[index].varsI[INDEX_WORLD_Y],
								52 + rand() % 10, 44 + rand() % 8,
								rocks[rock].vx * .5, rocks[rock].vy * .5);

						} break;

						case ROCK_SMALL:
						{

							// start explosion
							Start_Burst(plasma[index].varsI[INDEX_WORLD_X], plasma[index].varsI[INDEX_WORLD_Y],
								34 - 4 + rand() % 8, 30 - 3 + rand() % 6,
								rocks[rock].vx * .5, rocks[rock].vy * .5);

						} break;

						}

						// update score
						player_score += rocks[rock].varsI[2];

						// test strength of rock, cause damage
						rocks[rock].varsI[2] -= 50;

						// split test
						if (rocks[rock].varsI[2] > 0 && rocks[rock].varsI[2] < 50)
						{
							// test the size of rock
							switch (rocks[rock].varsI[0])
							{
							case ROCK_LARGE:
							{
								// split into two medium
								Start_Rock(rocks[rock].varsI[INDEX_WORLD_X] + rand() % 16, rocks[rock].varsI[INDEX_WORLD_Y] + rand() % 16,
									ROCK_MEDIUM,
									rocks[rock].vx - 2 + rand() % 4, rocks[rock].vy - 2 + rand() % 4);

								Start_Rock(rocks[rock].varsI[INDEX_WORLD_X] + rand() % 16, rocks[rock].varsI[INDEX_WORLD_Y] + rand() % 16,
									ROCK_MEDIUM,
									rocks[rock].vx - 2 + rand() % 4, rocks[rock].vy - 2 + rand() % 4);

								// throw in a small?
								if ((rand() % 3) == 1)
									Start_Rock(rocks[rock].varsI[INDEX_WORLD_X] + rand() % 16, rocks[rock].varsI[INDEX_WORLD_Y] + rand() % 16,
										ROCK_SMALL,
										rocks[rock].vx - 2 + rand() % 4, rocks[rock].vy - 2 + rand() % 4);

								// kill the original
								rocks[rock].state = ROCK_STATE_OFF;

							} break;

							case ROCK_MEDIUM:
							{
								// split into 1 - 3 small
								int num_rocks = 1 + rand() % 3;

								for (; num_rocks >= 1; num_rocks--)
								{
									Start_Rock(rocks[rock].varsI[INDEX_WORLD_X] + rand() % 8, rocks[rock].varsI[INDEX_WORLD_Y] + rand() % 8,
										ROCK_SMALL,
										rocks[rock].vx - 2 + rand() % 4, rocks[rock].vy - 2 + rand() % 4);

								}
								// kill the original
								rocks[rock].state = ROCK_STATE_OFF;

							} break;

							case ROCK_SMALL:
							{
								// just kill it
								rocks[rock].state = ROCK_STATE_OFF;

							} break;

							default:break;


							}

						}
						else if (rocks[rock].varsI[2] <= 0)
						{
							// kill rock
							rocks[rock].state = ROCK_STATE_OFF;
						} // end else

					// break out of loop
						break;

					}

				}

			}

		}

	}

}

///////////////////////////////////////////////////////////

void Draw_Plasma(void)
{
	// this function draws all the plasma pulses

	for (int index = 0; index < MAX_PLASMA; index++)
	{
		// test if plasma pulse is in flight
		if (plasma[index].state == PLASMA_STATE_ON)
		{
			// transform to screen coords
			plasma[index].x = plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].getWidth() >> 1) - player_x + (SCREEN_WIDTH / 2);
			plasma[index].y = plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].getHeight() >> 1) - player_y + (SCREEN_HEIGHT / 2);

			// draw the pulse
			plasma[index].drawOn();

			// animate the pulse
			plasma[index].animate();

		}

	}

}

///////////////////////////////////////////////////////////

void Fire_Plasma(int x, int y, int xv, int yv, int source = PLASMA_ANIM_PLAYER)
{
	// this function fires a plasma pulse at the given starting
	// position and velocity, of course, one must be free for 
	// this to work

	// scan for a pulse that is available
	for (int index = 0; index < MAX_PLASMA; index++)
	{
		// is this one available
		  // test if plasma pulse is in flight
		if (plasma[index].state == PLASMA_STATE_OFF)
		{
			// start this one up, note the use of world coords
			plasma[index].varsI[INDEX_WORLD_X] = x - (plasma[0].getWidth() * .5);
			plasma[index].varsI[INDEX_WORLD_Y] = y - (plasma[0].getHeight() * .5);

			plasma[index].vx = xv;
			plasma[index].vy = yv;
			plasma[index].restart();
			plasma[index].state = PLASMA_STATE_ON;
			plasma[index].counter_1 = 0; // used to track distance

			// set animation and typing info for collision engine
			plasma[index].anim_state = source;
			plasma[index].setAniString(source);

			// start sound up
			for (int sound_index = 0; sound_index < MAX_FIRE_SOUNDS; sound_index++)
			{
				// test if this sound is playing

				if (sound[fire_ids[sound_index]].getStatus() == 0)
				{
					sound[fire_ids[sound_index]].play();
					break;
				}

			}

			// later
			return;

		}

	}

}




///////////////////////////////////////////////////////////

void Init_Rocks(void)
{
	// this function initializes and loads all the rocks 

	// create the large rock

	// load animation frames
	static aniDICT rock_lDict;
	rock_lDict.resize(16);
	SURFACE temp;
	for (int frame = 0; frame < 16; frame++)
	{
		// load the rocks imagery 
		TCHAR buffer[80];
		Pad_Name(path + TEXT("OUTART/ROCKL"), TEXT("BMP"), buffer, frame);
		temp.createFromBitmap(buffer);
		rock_lDict[frame].createFromSurface(96, 96, temp, 0, 0);

	}
	rock_l.content.setDict(&rock_lDict);
	rock_l.content.autoOrder();
	// set animation rate
	rock_l.setAniSpeed(1 + rand() % 5);
	rock_l.vx = -4 + rand() % 8;
	rock_l.vy = 4 + rand() % 4;

	rock_l.x = rand() % SCREEN_WIDTH;
	rock_l.y = rand() % SCREEN_HEIGHT;

	// set size of rock
	rock_l.varsI[0] = ROCK_LARGE;
	rock_l.varsI[1] = rock_sizes[rock_l.varsI[0]];

	// set state to off
	rock_l.state = ROCK_STATE_OFF;

	// create the medium rock
	static aniDICT rock_mDict;
	rock_mDict.resize(16);
	// load animation frames
	for (int frame = 0; frame < 16; frame++)
	{
		// load the rocks imagery 
		TCHAR buffer[80];
		Pad_Name(path + TEXT("OUTART/ROCKM"), TEXT("BMP"), buffer, frame);
		temp.createFromBitmap(buffer);
		// load the actual .BMP
		rock_mDict[frame].createFromSurface(56, 56, temp, 0, 0);

	}
	rock_m.content.setDict(&rock_mDict);
	rock_m.content.autoOrder();

	// set animation rate
	rock_m.setAniSpeed(1 + rand() % 5);

	rock_m.vx = -4 + rand() % 8;
	rock_m.vy = 4 + rand() % 4;
	rock_m.x = rand() % SCREEN_WIDTH;
	rock_m.y = rand() % SCREEN_HEIGHT;

	// set size of rock
	rock_m.varsI[0] = ROCK_MEDIUM;
	rock_m.varsI[1] = rock_sizes[rock_m.varsI[0]];

	// set state to off
	rock_m.state = ROCK_STATE_OFF;

	// create the small rock
	static aniDICT rock_sDict;
	rock_sDict.resize(16);
	// load animation frames
	for (int frame = 0; frame < 16; frame++)
	{
		// load the rocks imagery 
		TCHAR buffer[80];
		Pad_Name(path + TEXT("OUTART/ROCKS"), TEXT("BMP"), buffer, frame);
		temp.createFromBitmap(buffer);


		rock_sDict[frame].createFromSurface(32, 32, temp, 0, 0);


	}
	rock_s.content.setDict(&rock_sDict);
	rock_s.content.autoOrder();

	// set animation rate
	rock_s.setAniSpeed(1 + rand() % 5);
	rock_s.vx = -4 + rand() % 8;
	rock_s.vy = -4 + rand() % 8;
	rock_s.x = rand() % SCREEN_WIDTH;
	rock_s.y = rand() % SCREEN_HEIGHT;


	// set size of rock
	rock_s.varsI[0] = ROCK_SMALL;
	rock_s.varsI[1] = rock_sizes[rock_s.varsI[0]];

	// set state to off
	rock_s.state = ROCK_STATE_OFF;

	// make semi copies
	for (int rock = 0; rock < MAX_ROCKS; rock++)
	{
		rocks[rock].clone(rock_l);
		// set animation rate
		rocks[rock].setAniSpeed(1 + rand() % 5);

		rocks[rock].vx = -4 + rand() % 8;
		rocks[rock].vy = -4 + rand() % 8;

		rocks[rock].x = 0;
		rocks[rock].y = 0;


		// set position
		rocks[rock].varsI[INDEX_WORLD_X] = RAND_RANGE(UNIVERSE_MIN_X, UNIVERSE_MAX_X);
		rocks[rock].varsI[INDEX_WORLD_Y] = RAND_RANGE(UNIVERSE_MIN_Y, UNIVERSE_MAX_Y);

		// set size of rock
		rocks[rock].varsI[0] = rand() % 3;
		rocks[rock].varsI[1] = rock_sizes[rocks[rock].varsI[0]];

		// based on size of rock switch in correct bitmaps and sizes
		switch (rocks[rock].varsI[1])
		{
		case ROCK_LARGE:
		{
			// copy dd bitmap surfaces
			rocks[rock].content.setDict(&rock_lDict);

		} break;

		case ROCK_MEDIUM:
		{
			// copy dd bitmap surfaces
			rocks[rock].content.setDict(&rock_mDict);

		} break;

		case ROCK_SMALL:
		{
			// copy dd bitmap surfaces
			rocks[rock].content.setDict(&rock_sDict);

		} break;

		default: break;

		}

	}

}

//////////////////////////////////////////////////////////////////////////////

void Reset_Rocks(void)
{
	// this function resets all the rocks

	for (int rock = 0; rock < MAX_ROCKS; rock++)
	{
		// reset state
		rocks[rock].state = ROCK_STATE_OFF;

	}

}



//////////////////////////////////////////////////////////////////////////////

void Seed_Rocks(void)
{
	// this function seeds the universe with rocks

	   // scan for a rock to initialize
	for (int index = 0; index < (MAX_ROCKS * .75); index++)
	{
		// is this rock available?
		if (rocks[index].state == ROCK_STATE_OFF)
		{
			// set animation rate
			rocks[index].setAniSpeed(1 + rand() % 5);

			// set velocity
			rocks[index].vx = -6 + rand() % 12;
			rocks[index].vy = -6 + rand() % 12;

			// set position
			rocks[index].x = 0;
			rocks[index].y = 0;
			rocks[index].varsI[INDEX_WORLD_X] = RAND_RANGE(UNIVERSE_MIN_X, UNIVERSE_MAX_X);
			rocks[index].varsI[INDEX_WORLD_Y] = RAND_RANGE(UNIVERSE_MIN_Y, UNIVERSE_MAX_Y);

			// set size of rock
			rocks[index].varsI[0] = rand() % 3;
			rocks[index].varsI[1] = rock_sizes[rocks[index].varsI[0]];

			// set strength of rock
			switch (rocks[index].varsI[0])
			{
			case ROCK_LARGE:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 100 + rand() % 100;

				// copy dd bitmap surfaces
				rocks[index].content.setDict(rock_l.content.getDict());

			} break;

			case ROCK_MEDIUM:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 40 + rand() % 30;

				rocks[index].content.setDict(rock_m.content.getDict());

			} break;

			case ROCK_SMALL:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 10;

				rocks[index].content.setDict(rock_s.content.getDict());
			} break;

			default:break;

			}

			// turn rock on
			rocks[index].state = ROCK_STATE_ON;

		}

	}

}


//////////////////////////////////////////////////////////

void Move_Rocks(void)
{
	// this function moves all the rocks pulses and checks for
	// collision with the rocks

	for (int index = 0; index < MAX_ROCKS; index++)
	{
		// test if rocks pulse is in flight
		if (rocks[index].state == ROCK_STATE_ON)
		{
			// move the rock
			rocks[index].varsI[INDEX_WORLD_X] += rocks[index].vx;
			rocks[index].varsI[INDEX_WORLD_Y] += rocks[index].vy;

			// test if rock is out of universe, but persist object
			if (rocks[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X)
				rocks[index].varsI[INDEX_WORLD_X] = UNIVERSE_MIN_X;
			else if (rocks[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X)
				rocks[index].varsI[INDEX_WORLD_X] = UNIVERSE_MAX_X;

			if (rocks[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_X)
				rocks[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MIN_X;
			else if (rocks[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y)
				rocks[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MAX_Y;


			// test for collision with rocks
			if (player_state == PLAYER_STATE_ALIVE && Collision_Test(player_x - (wraith.getWidth() * .5), player_y - (wraith.getHeight() * .5),
				wraith.getWidth(), wraith.getHeight(),
				rocks[index].varsI[INDEX_WORLD_X] - (rocks[index].getWidth() * .5),
				rocks[index].varsI[INDEX_WORLD_Y] - (rocks[index].getHeight() * .5),
				rocks[index].getWidth(), rocks[index].getHeight()))
			{
				// what size rock did we hit?

				switch (rocks[index].varsI[0])
				{
				case ROCK_LARGE:
				{
					// start explosion
					Start_Burst(rocks[index].varsI[INDEX_WORLD_X], rocks[index].varsI[INDEX_WORLD_Y],
						68 + rand() % 12, 54 + rand() % 10,
						rocks[index].vx * .5, rocks[index].vy * .5);

					// update players damage
					player_damage += 35;

					// update velocity vector
					player_xv += (rocks[index].vx);
					player_yv += (rocks[index].vy);

					// rotate ship a bit
					wraith.varsI[WRAITH_INDEX_DIR] += (RAND_RANGE(-4, 4));
					if (wraith.varsI[WRAITH_INDEX_DIR] > 15)
						wraith.varsI[WRAITH_INDEX_DIR] -= 15;
					else if (wraith.varsI[WRAITH_INDEX_DIR] < 0)
						wraith.varsI[WRAITH_INDEX_DIR] += 15;


				} break;

				case ROCK_MEDIUM:
				{
					// start explosion
					Start_Burst(rocks[index].varsI[INDEX_WORLD_X], rocks[index].varsI[INDEX_WORLD_Y],
						52 + rand() % 10, 44 + rand() % 8,
						rocks[index].vx * .5, rocks[index].vy * .5);

					// update players damage
					player_damage += 15;

					// update velocity vector
					player_xv += (rocks[index].vx * .5);
					player_yv += (rocks[index].vy * .5);

					// rotate ship a bit
					wraith.varsI[WRAITH_INDEX_DIR] += (RAND_RANGE(-4, 4));
					if (wraith.varsI[WRAITH_INDEX_DIR] > 15)
						wraith.varsI[WRAITH_INDEX_DIR] -= 15;
					else if (wraith.varsI[WRAITH_INDEX_DIR] < 0)
						wraith.varsI[WRAITH_INDEX_DIR] += 15;

				} break;

				case ROCK_SMALL:
				{
					// start explosion
					Start_Burst(rocks[index].varsI[INDEX_WORLD_X], rocks[index].varsI[INDEX_WORLD_Y],
						34 - 4 + rand() % 8, 30 - 3 + rand() % 6,
						rocks[index].vx * .5, rocks[index].vy * .5);

					// update players damage
					player_damage += 5;

					// update velocity vector
					player_xv += (rocks[index].vx * .25);
					player_yv += (rocks[index].vy * .25);

					// rotate ship a bit
					wraith.varsI[WRAITH_INDEX_DIR] += (RAND_RANGE(-4, 4));
					if (wraith.varsI[WRAITH_INDEX_DIR] > 15)
						wraith.varsI[WRAITH_INDEX_DIR] -= 15;
					else if (wraith.varsI[WRAITH_INDEX_DIR] < 0)
						wraith.varsI[WRAITH_INDEX_DIR] += 15;


				} break;

				}

				// update score
				player_score += rocks[index].varsI[2];

				// engage shields
				player_shield_count = 3;

				// kill the original
				rocks[index].state = ROCK_STATE_OFF;

			}

		}

	}

	// now test if it's time to add a new rock to the list
	if ((rand() % 100) == 50)
	{
		// scan for a rock to initialize
		for (int index = 0; index < MAX_ROCKS; index++)
		{
			// is this rock available?
			if (rocks[index].state == ROCK_STATE_OFF)
			{
				// set animation rate
				rocks[index].setAniSpeed(1 + rand() % 5);

				// set velocity
				rocks[index].vx = -6 + rand() % 12;
				rocks[index].vy = -6 + rand() % 12;

				// set position
				rocks[index].x = 0;
				rocks[index].y = 0;
				rocks[index].varsI[INDEX_WORLD_X] = RAND_RANGE(UNIVERSE_MIN_X, UNIVERSE_MAX_X);
				rocks[index].varsI[INDEX_WORLD_Y] = RAND_RANGE(UNIVERSE_MIN_Y, UNIVERSE_MAX_Y);

				// set size of rock
				rocks[index].varsI[0] = rand() % 3;
				rocks[index].varsI[1] = rock_sizes[rocks[index].varsI[0]];

				// set strength of rock
				switch (rocks[index].varsI[0])
				{
				case ROCK_LARGE:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 100 + rand() % 100;

					// copy dd bitmap surfaces
					rocks[index].content.setDict(rock_l.content.getDict());

				} break;

				case ROCK_MEDIUM:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 40 + rand() % 30;

					// copy dd bitmap surfaces
					rocks[index].content.setDict(rock_m.content.getDict());

				} break;

				case ROCK_SMALL:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 10;

					// copy dd bitmap surfaces
					rocks[index].content.setDict(rock_s.content.getDict());
				} break;

				default:break;

				}


				// turn rock on
				rocks[index].state = ROCK_STATE_ON;

				// later
				return;

			}

		}

	}

}

///////////////////////////////////////////////////////////

void Start_Rock(int x, int y, int size, int xv, int yv)
{
	// this functions starts a rock up with the sent parms, considering
	// there is one left

	// scan for a rock to initialize
	for (int index = 0; index < MAX_ROCKS; index++)
	{
		// is this rock available?
		if (rocks[index].state == ROCK_STATE_OFF)
		{
			// set animation rate
			rocks[index].setAniSpeed(1 + rand() % 5);

			// set velocity
			rocks[index].vx = xv;
			rocks[index].vy = yv;



			// set position
			rocks[index].x = 0;
			rocks[index].y = 0;
			rocks[index].varsI[INDEX_WORLD_X] = x;
			rocks[index].varsI[INDEX_WORLD_Y] = y;

			// set size of rock
			rocks[index].varsI[0] = size;
			rocks[index].varsI[1] = rock_sizes[rocks[index].varsI[0]];

			// set strength of rock
			switch (rocks[index].varsI[0])
			{
			case ROCK_LARGE:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 100 + rand() % 100;

				rocks[index].content.setDict(rock_l.content.getDict());

			} break;

			case ROCK_MEDIUM:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 40 + rand() % 30;

				rocks[index].content.setDict(rock_m.content.getDict());
			} break;

			case ROCK_SMALL:
			{
				// set hardness of rock
				rocks[index].varsI[2] = 10;

				rocks[index].content.setDict(rock_s.content.getDict());
			} break;

			default:break;

			}

			// turn rock on
			rocks[index].state = ROCK_STATE_ON;

			// later
			return;

		}

	}

}

///////////////////////////////////////////////////////////

void Draw_Rocks(void)
{
	// this function draws all the rocks 

	for (int index = 0; index < MAX_ROCKS; index++)
	{
		// test if rocks pulse is in flight
		if (rocks[index].state == ROCK_STATE_ON)
		{
			// transform to screen coords
			rocks[index].x = rocks[index].varsI[INDEX_WORLD_X] - (rocks[index].getWidth() * .5) - player_x + (SCREEN_WIDTH / 2);
			rocks[index].y = rocks[index].varsI[INDEX_WORLD_Y] - (rocks[index].getHeight() * .5) - player_y + (SCREEN_HEIGHT / 2);

			// draw the rock
			rocks[index].drawOn();
			rocks[index].animate();


		}

	}

}

///////////////////////////////////////////////////////////

void Init_Bursts(void)
{
	// this function initializes and loads all the bursts 

	// load the bursts imagery 
	SURFACE temp;
	temp.createFromBitmap(path + TEXT("OUTART/EXPL8.BMP"));

	bursts[0].x = 0;
	bursts[0].y = 0;
	static aniDICT burstsDict;
	burstsDict.resize(14);
	// load animation frames
	for (int frame = 0; frame < 14; frame++)
		burstsDict[frame].createFromSurface(42, 36, temp, (frame % 6) * (42 + 1) + 1, (frame / 6) * (36 + 1) + 1);

	bursts[0].content.setDict(&burstsDict);
	bursts[0].content.autoOrder();
	// set animation rate
	bursts[0].setAniSpeed(1);

	// set size of burst
	bursts[0].varsI[0] = bursts[0].getWidth();
	bursts[0].varsI[1] = bursts[0].getHeight();

	// set state to off
	bursts[0].state = BURST_STATE_OFF;

	for (int burst = 1; burst < MAX_BURSTS; burst++)
	{
		bursts[burst].clone(bursts[0]);
	}



}

///////////////////////////////////////////////////////////

void Reset_Bursts(void)
{
	// this function resets all the bursts

	for (int burst = 0; burst < MAX_BURSTS; burst++)
	{
		// set state to off
		bursts[burst].state = BURST_STATE_OFF;
	}


}


///////////////////////////////////////////////////////////

void Move_Bursts(void)
{
	// this function moves all the bursts 

	for (int index = 0; index < MAX_BURSTS; index++)
	{
		// test if bursts pulse is in moving
		if (bursts[index].state == BURST_STATE_ON)
		{
			// move the burst
			// Move_BOB(&bursts[index]);
			bursts[index].varsI[INDEX_WORLD_X] += bursts[index].vx;
			bursts[index].varsI[INDEX_WORLD_Y] += bursts[index].vy;

			// test if burst is off screen or done with animation
			if (bursts[index].getDictIndex() >= bursts[index].content.getDict()->getSize() - 1)
			{
				// kill burst and put back on available list
				bursts[index].state = BURST_STATE_OFF;
			}

		}

	}

}

///////////////////////////////////////////////////////////

void Draw_Bursts(void)
{
	// this function draws all the bursts 
	for (int index = 0; index < MAX_BURSTS; index++)
	{
		// test if bursts pulse is in flight
		if (bursts[index].state == BURST_STATE_ON)
		{
			// transform bursts to screen space
			bursts[index].x = bursts[index].varsI[INDEX_WORLD_X] - (bursts[index].getWidth() * .5) - player_x + (SCREEN_WIDTH / 2);
			bursts[index].y = bursts[index].varsI[INDEX_WORLD_Y] - (bursts[index].getHeight() * .5) - player_y + (SCREEN_HEIGHT / 2);

			// is scaling needed
			if (bursts[index].varsI[0] != bursts[index].getWidth() ||
				bursts[index].varsI[1] != bursts[index].getHeight())
			{
				// draw the burst scaled
				bursts[index].drawOn(bursts[index].varsI[0], bursts[index].varsI[1]);
			}
			else // draw normal
				bursts[index].drawOn();

			// animate the explosion
			bursts[index].animate();

		}

	}

}

///////////////////////////////////////////////////////////

void Start_Burst(int x, int y, int width, int height, int xv, int yv)
{
	// this function starts a burst up

	// now test if it's time to add a new burst to the list

	// scan for a burst to initialize
	for (int index = 0; index < MAX_BURSTS; index++)
	{
		// is this burst available?
		if (bursts[index].state == BURST_STATE_OFF)
		{
			// set animation rate
			bursts[index].setAniSpeed(1);
			bursts[index].restart();

			// set velocity
			bursts[index].vx = xv; bursts[index].vy = yv;
			// set position
			bursts[index].x = bursts[index].y = 0;
			bursts[index].varsI[INDEX_WORLD_X] = x;
			bursts[index].varsI[INDEX_WORLD_Y] = y;

			// set size of burst
			bursts[index].varsI[0] = width;
			bursts[index].varsI[1] = height;

			// turn burst on
			bursts[index].state = BURST_STATE_ON;

			// shoot some particles out
			Start_Particle_Explosion(PARTICLE_TYPE_FLICKER, RAND_RANGE(PARTICLE_COLOR_RED, PARTICLE_COLOR_GREEN), 20 + rand() % 40,
				bursts[index].varsI[INDEX_WORLD_X], bursts[index].varsI[INDEX_WORLD_Y],
				bursts[index].vx, bursts[index].vy,
				8 + rand() % 8);


			// start sound up
			for (int sound_index = 0; sound_index < MAX_EXPL_SOUNDS; sound_index++)
			{
				// test if this sound is playing
				if (sound[expl_ids[sound_index]].getStatus() == 0)
				{
					sound[expl_ids[sound_index]].play();
					break;
				}

			}

			// later
			return;

		}

	}

}

///////////////////////////////////////////////////////////

void Draw_Info(void)
{
	// this function draws all the information at the top of the screen

	static int red_glow = 0; // used for damage display
	static int warning_count = 0;
	TCHAR buffer[MAX_BUFFER];


	// build up final string
	_stprintf(buffer, TEXT("SCORE %08d"), player_score);
	gPrintf(10, 10, RGB(0, 255, 0), buffer);
	// draw damage----为什么要这么多呢，因为有三层套娃用了 printf  2^3 = 8
	_stprintf(buffer, TEXT("DAMAGE %d%%%%%%%%"), player_damage);

	if (player_damage < 90)//4 个 %
		gPrintf(350 - 8 * (lstrlen(buffer) - 4 + 1), 10, RGB(0, 255, 0), buffer);
	else
		gPrintf(350 - 8 * (lstrlen(buffer) - 4 + 1), 10, RGB(red_glow, 0, 0), buffer);

	// update red glow
	if ((red_glow += 15) > 255)
		red_glow = 0;

	// draw ships
	gPrintf(520, 10, RGB(0, 255, 0), TEXT("SHIPS %d"), player_ships);

	gPrintf(10, 460, RGB(0, 255, 0), TEXT("VEL %.2f Kps"), vel * (100 / MAX_PLAYER_SPEED));

	gPrintf(480, 460, RGB(0, 255, 0), TEXT("POS [%.2f,%.2f]"), player_x, player_y);

	gPrintf(320 - 120, 460, RGB(0, 255, 0), TEXT("TRACKING RATE = %.2f"), mine_tracking_rate);


}




/////////////////////////////////////////////////////////////////////

void Do_Intro(void)
{
	graphicOut.fillColor();
	// the worlds simples intro
	SwitchSurface(true);
	// clear out buffers
	graphicOut.fillColor();

	// draw in logo screen
	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("OUTART/TRACKINTRO.BMP"));
	laPALETTE temp;
	bitmap.getPalette(temp);
	curPalette.set(temp);
	// set the palette to background image palette

	SURFACE introduct;
	introduct.createFromBitmap(bitmap);
	POINT point = gameBox.getOriginXY();
	introduct.drawOn(point.x, point.y, false);


	Sleep(5000);

	BeginDrawOn();
	// transition to black
	ScreenTransitions(SCREEN_DARKNESS, 50);
	EndDrawOn();
	// clear out buffers
	graphicOut.fillColor();
	SwitchSurface(false);
	graphicOut.fillColor();
}

///////////////////////////////////////////////////////////

void Load_Sound_Music(void)
{
	// this function loads all the sounds and music

	// load in intro music
	int soundIndex = 0;

	intro_music_id = soundIndex;
	sound[intro_music_id].create(path + TEXT("OUTSOUND/INTRO.WAV"));
	soundIndex++;

	main_music_id = soundIndex;
	sound[main_music_id].create(path + TEXT("OUTSOUND/STARSNG.WAV"));
	soundIndex++;

	// load get ready
	ready_id = soundIndex;
	sound[ready_id].create(path + TEXT("OUTSOUND/ENTERING1.WAV"));
	soundIndex++;

	// load engines
	engines_id = soundIndex;
	sound[engines_id].create(path + TEXT("OUTSOUND/ENGINES.WAV"));
	soundIndex++;

	// load scream
	scream_id = soundIndex;
	sound[scream_id].create(path + TEXT("OUTSOUND/BREAKUP.WAV"));
	soundIndex++;

	// load game over
	game_over_id = soundIndex;
	sound[game_over_id].create(path + TEXT("OUTSOUND/GAMEOVER.WAV"));
	soundIndex++;

	// load mine powerup
	mine_powerup_id = soundIndex;
	sound[mine_powerup_id].create(path + TEXT("OUTSOUND/MINEPOWER1.WAV"));
	soundIndex++;

	deactivate_id = soundIndex;
	sound[deactivate_id].create(path + TEXT("OUTSOUND/DEACTIVATE1.WAV"));
	soundIndex++;

	station_throb_id = soundIndex;
	sound[station_throb_id].create(path + TEXT("OUTSOUND/STATIONTHROB2.WAV"));
	soundIndex++;

	beep0_id = soundIndex;
	sound[beep0_id].create(path + TEXT("OUTSOUND/BEEP3.WAV"));
	soundIndex++;


	beep1_id = soundIndex;
	sound[beep1_id].create(path + TEXT("OUTSOUND/BEEP1.WAV"));
	soundIndex++;

	// load the explosions sounds

	station_blow_id = soundIndex;
	sound[station_blow_id].create(path + TEXT("OUTSOUND/STATIONBLOW.WAV"));
	soundIndex++;

	// these are the two different source masters
	expl_ids[0] = soundIndex;
	sound[expl_ids[0]].create(path + TEXT("OUTSOUND/EXPL1.WAV"));
	soundIndex++;


	expl_ids[1] = soundIndex;
	sound[expl_ids[1]].create(path + TEXT("OUTSOUND/EXPL2.WAV"));
	soundIndex++;

	// now make copies
	for (int index = 2; index < MAX_EXPL_SOUNDS; index++)
	{
		// replicate sound
		expl_ids[index] = soundIndex;
		sound[expl_ids[index]].cloneFrom(sound[expl_ids[rand() % 2]]);
		soundIndex++;

	}

	// load the plasma weapons sounds
	fire_ids[0] = soundIndex;
	sound[fire_ids[0]].create(path + TEXT("OUTSOUND/PULSE.WAV"));
	soundIndex++;

	// now make copies
	for (int index = 1; index < MAX_FIRE_SOUNDS; index++)
	{
		// replicate sound
		fire_ids[index] = soundIndex;
		sound[fire_ids[index]].cloneFrom(sound[fire_ids[0]]);
		soundIndex++;
	}

}

///////////////////////////////////////////////////////////////////

void Load_HUD(void)
{
	// this function loads the animation for the hud


	// load the wraith ship
	SURFACE temp;
	temp.createFromBitmap(path + TEXT("OUTART/HUDART8.BMP"));

	hud.x = hud.y = 0;
	static aniDICT hudDict;
	hudDict.resize(4);
	// load hud frames
	for (int index = 0; index < 4; index++)
		hudDict[index].createFromSurface(24, 26, temp, index * (24 + 1) + 1, 0 * (26 + 1) + 1);
	hud.content.setDict(&hudDict);
	hud.content.autoOrder();
}


///////////////////////////////////////////////////////////////////////////////

void Load_Player(void)
{
	// this function loads the animation for the player


	// load the wraith ship
	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("OUTART/WRAITHB8.BMP"));

	bitmap.getPalette(game_palette);
	curPalette.set(game_palette);
	// set the palette to background image palette


	// save this palette


	// now create the wraith


	static aniDICT wraithDict;
	wraithDict.resize(35);
	SURFACE temp;
	temp.createFromBitmap(bitmap);
	// load wraith frames
	for (int index = 0; index < 35; index++)
		wraithDict[index].createFromSurface(64, 64, temp, (index % 8) * (64 + 1) + 1, (index / 8) * (64 + 1) + 1);

	wraith.content.setDict(&wraithDict);
	wraith.content.autoOrder();

	// set position
	wraith.x = (SCREEN_WIDTH / 2) - wraith.getWidth() / 2;
	wraith.y = (SCREEN_HEIGHT / 2) - wraith.getHeight() / 2;

	// set starting direction
	wraith.varsI[WRAITH_INDEX_DIR] = 0;


}



///////////////////////////////////////////////////////////

// WINX GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

void Game_Reset(void)
{
	// this function resets all the game related systems and vars for another run

	int index; // looping var

	// player state variables
	player_state = PLAYER_STATE_ALIVE;
	player_score = 0;   // the score
	player_ships = 3;   // ships left
	player_regen_counter = 0; // used to track when to regen
	player_damage = 0;   // damage of player
	player_counter = 0;   // used for state transition tracking
	player_regen_count = 0;   // used to regenerate player
	player_shield_count = 0;  // used to display shields
	win_counter = 0;
	player_won = 0;
	num_stations_destroyed = 0; // number of stations destroyed thus far

	// menu_up = 0;             // flags if the main menu is up
	// intro_done = 0;          // flags if intro has played already

	// positional and state info for player
	player_x = 0;
	player_y = 0;
	player_dx = 0;
	player_dy = 0;
	player_xv = 0;
	player_yv = 0;
	vel = 0;
	wraith.varsI[WRAITH_INDEX_DIR] = 0;
	game_state = GAME_STATE_RESTART;   // initial game state
	station_id = -1;

	ready_counter = 0,        // used to draw a little "get ready"
		ready_state = 0;

	// call all the reset funcs
	Init_Reset_Particles();

	Reset_Mines();
	Reset_Rocks();
	Reset_Plasma();
	Reset_Bursts();
	Reset_Stations();

	// start the rocks
	Seed_Rocks();

	// start the mines
	for (index = 0; index < NUM_ACTIVE_MINES; index++)
		Start_Mine();

	// start the stations
	for (index = 0; index < NUM_ACTIVE_STATIONS; index++)
		Start_Station();

}

////////////////////////////////////////////////////////////

void StartUp(void)
{
	// this function is where you do all the initialization 
	// for your game

	//mono.clear();
	//mono.print("\ndebugger on-line\n");

	// seed random number generate
	srand(rand());



	// load all the sound and music
	Load_Sound_Music();

	// initialize the plasma pulses
	Init_Plasma();

	// initialize all the stars
	Init_Stars();

	// init rocks
	Init_Rocks();
	Seed_Rocks();

	// init all the explosions
	Init_Bursts();

	// initialize all the Mines
	Init_Mines();

	// initialize all the stations, must be after mines!
	Init_Stations();

	// intialize particle system
	Init_Reset_Particles();

	Load_Andre();


	// hide the mouse
	ShowCursor(FALSE);

	// start the intro music
	sound[intro_music_id].play();

	// do the introduction
	Do_Intro();

	// load the player
	Load_Player();

	// load the hud art
	Load_HUD();

	keyboard.create();

}


///////////////////////////////////////////////////////////

void GameBody(void)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!

	int  index;                  // looping var
	int  dx, dy;                  // general deltas used in collision detection
	float xv = 0, yv = 0;            // used to compute velocity 
	static int engines_on = 0;   // tracks state of engines each frame 
	static int andre_up = 0;


	// what state is the main loop in?
	if (game_state == GAME_STATE_INIT)
	{
		// reset everything

		// transition into menu state
		// game_state = GAME_STATE_MENU;

		// transition into run state
		game_state = GAME_STATE_RESTART;
	} // end game_state is game init
	else if (game_state == GAME_STATE_MENU)
	{

		game_state = GAME_STATE_RESTART;

	} // end if in menu state
	else if (game_state == GAME_STATE_RESTART)
	{
		// start the main music
		sound[main_music_id].play(true);
		// restart the game
		Game_Reset();

		// set to run state
		game_state = GAME_STATE_RUNNING;


	}
	else if (game_state == GAME_STATE_RUNNING)
	{

		// start the timing clock
		fpsSet.start();

		// clear the drawing surface
		graphicOut.fillColor();

		// read all input devices
		keyboard.read();

		// only process player if alive
		if (player_state == PLAYER_STATE_ALIVE)
		{

			// test for tracking rate change
			if (keyboard[DIK_EQUALS])
			{
				if ((mine_tracking_rate += 0.1) > 4.0)
					mine_tracking_rate = 4.0;
			} // end if
			else
				if (keyboard[DIK_MINUS])
				{
					if ((mine_tracking_rate -= 0.1) < 0)
						mine_tracking_rate = 0;
				}


			// test if player is moving
			if (keyboard[DIK_RIGHT])
			{
				// rotate player to right
				if (++wraith.varsI[WRAITH_INDEX_DIR] > 15)
					wraith.varsI[WRAITH_INDEX_DIR] = 0;

			} // end if
			else if (keyboard[DIK_LEFT])
			{
				// rotate player to left
				if (--wraith.varsI[WRAITH_INDEX_DIR] < 0)
					wraith.varsI[WRAITH_INDEX_DIR] = 15;

			}

			// vertical/speed motion
			if (keyboard[DIK_UP])
			{
				// move player forward
				xv = cos_look16[wraith.varsI[WRAITH_INDEX_DIR]];
				yv = sin_look16[wraith.varsI[WRAITH_INDEX_DIR]];

				// test to turn on engines
				if (!engines_on)
					sound[engines_id].play(true);


				// set engines to on
				engines_on = 1;

				Start_Particle(PARTICLE_TYPE_FADE, PARTICLE_COLOR_GREEN, 3,
					player_x + RAND_RANGE(-2, 2), player_y + RAND_RANGE(-2, 2), (-int(player_xv) * .125), (-int(player_yv) * .125));

			}
			else if (engines_on)
			{
				// reset the engine on flag and turn off sound
				engines_on = 0;

				// turn off the sound
				sound[engines_id].stop();
			}



			// toggle huds
			if (keyboard[DIK_H] && !huds_debounce)
			{
				huds_debounce = 1;
				huds_on = (huds_on) ? 0 : 1;

				sound[beep1_id].play();

			}

			if (!keyboard[DIK_H])
				huds_debounce = 0;

			// toggle scanner
			if (keyboard[DIK_S] && !scanner_debounce)
			{
				scanner_debounce = 1;
				scanner_on = (scanner_on) ? 0 : 1;

				sound[beep1_id].play();

			}

			if (!keyboard[DIK_S])
				scanner_debounce = 0;



			// add velocity change to player's velocity
			player_xv += xv;
			player_yv += yv;

			// test for maximum velocity
			vel = DistanceFast(player_xv, player_yv);

			if (vel >= MAX_PLAYER_SPEED)
			{
				// re-compute velocity vector by normalizing then re-scaling
				player_xv = (MAX_PLAYER_SPEED - 1) * player_xv / vel;
				player_yv = (MAX_PLAYER_SPEED - 1) * player_yv / vel;
			}

			// add in frictional coefficient

			// move player, note that these are in world coords
			player_x += player_xv;
			player_y += player_yv;

			// wrap player to universe
			if (player_x > UNIVERSE_MAX_X)
			{
				// reset pos
				player_x = UNIVERSE_MIN_X;

				// set warp flag
			}
			else if (player_x < UNIVERSE_MIN_X)
			{
				// reset pos
				player_x = UNIVERSE_MAX_X;

				// set warp flag
			}

			// y coords
			if (player_y > UNIVERSE_MAX_Y)
			{
				// reset pos
				player_y = UNIVERSE_MIN_Y;

				// set warp flag
			}
			else if (player_y < UNIVERSE_MIN_Y)
			{
				// reset pos
				player_y = UNIVERSE_MAX_Y;

				// set warp flag
			}


			// test if player is firing
			if (keyboard[DIK_LCONTROL] || keyboard[DIK_SPACE])
			{
				// compute plasma velocity vector
				float plasma_xv = cos_look16[wraith.varsI[WRAITH_INDEX_DIR]] * PLASMA_SPEED;
				float plasma_yv = sin_look16[wraith.varsI[WRAITH_INDEX_DIR]] * PLASMA_SPEED;

				// fire the plasma taking into consideration ships's velocity
				Fire_Plasma(player_x, player_y,
					player_xv + plasma_xv,
					player_yv + plasma_yv, PLASMA_ANIM_PLAYER);

			}

			// do bounds check


			// regenerate player
			if (++player_regen_counter >= PLAYER_REGEN_COUNT)
			{
				// regenerate the player a bit
				if (player_damage > 0)
					player_damage--;

				// reset counter
				player_regen_counter = 0;

			}


			// test for dying state transition
			if (player_damage >= 100 && player_state == PLAYER_STATE_ALIVE)
			{
				// set damage to 100
				player_damage = 100;

				// kill player
				player_state = PLAYER_STATE_DYING;
				player_ships--;

				// set counter to 0
				player_counter = 0;

				// turn engines off
				engines_on = 0;

				// start scream
				sound[scream_id].play();

			}


		}
		else if (player_state == PLAYER_STATE_DYING)
		{
			// player is dying

			// start random bursts
			int bwidth = 16 + rand() % 64;

			if ((rand() % 4) == 1)
				Start_Burst(player_x + rand() % 40, player_y + rand() % 8,
					bwidth, (bwidth >> 2) + (bwidth >> 1),
					-4 + rand() % 8, 2 + rand() % 4);

			// get jiggie with it
			wraith.varsI[WRAITH_INDEX_DIR] += (RAND_RANGE(-1, 1));

			if (wraith.varsI[WRAITH_INDEX_DIR] > 15)
				wraith.varsI[WRAITH_INDEX_DIR] = 0;
			else if (wraith.varsI[WRAITH_INDEX_DIR] < 0)
					wraith.varsI[WRAITH_INDEX_DIR] = 15;

			// update state counter
			if (++player_counter > 150)
			{
				// set state to invincible  
				player_state = PLAYER_STATE_INVINCIBLE;

				// reset counter
				player_counter = 0;

				// reset damage
				player_damage = 0;
			}


		}
		else if (player_state == PLAYER_STATE_INVINCIBLE)
		{
			// player is invincible

			// flicker shields and play energizing sound

			// update state counter
			if (++player_counter > 70)
			{
				// set state to invincible  
				player_state = PLAYER_STATE_ALIVE;

				// reset counter
				player_counter = 0;

				// reset player position
				player_x = 0;
				player_y = 0;
				player_dx = 0;
				player_dy = 0;
				player_xv = 0;
				player_yv = 0;
				wraith.varsI[WRAITH_INDEX_DIR] = 0;

				// decrease player ships
				if (player_ships == 0)
				{
					// change state to dead, reset all vars
					player_state = PLAYER_STATE_DEAD;
					player_counter = 0;
					ready_state = 1;
					ready_counter = 0;

					// turn engines
					sound[engines_id].stop();

					// play game over sound
					sound[game_over_id].play();

				}

			}

			// start ready again
			if ((player_counter == 20) && (player_ships > 0))
			{
				// reset to ready
				ready_counter = 0,
					ready_state = 0;
			}
		}

		// end player sub states

		// move the stations
		Move_Stations();

		// move the mines
		Move_Mines();

		// move the asteroids
		Move_Rocks();

		// move the stars
		Move_Stars();

		// move the particles
		Move_Particles();

		// move the explosions
		Move_Bursts();

		// draw the stars
		Draw_Stars();

		// draw the stations
		Draw_Stations();

		// draw the tocks
		Draw_Rocks();

		// draw the mines
		Draw_Mines();

		// draw the particles
		Draw_Particles();

		// draw the plasma
		Draw_Plasma();

		// move the plasma
		Move_Plasma();

		// draw the player if alive
		if (player_state == PLAYER_STATE_ALIVE || player_state == PLAYER_STATE_DYING)
		{
			// set the current frame
			if (engines_on)
				wraith.setAniWord(wraith.varsI[WRAITH_INDEX_DIR] + 16 * (rand() % 2));
			else
				wraith.setAniWord(wraith.varsI[WRAITH_INDEX_DIR]);

			// draw the bob
			wraith.drawOn();

			// draw the shields
			if (player_shield_count > 0)
			{
				player_shield_count--;

				// select shield frame
				wraith.setAniWord(34 - player_shield_count);

				// draw the bob
				wraith.drawOn();

			}

		}

		// draw explosions last
		Draw_Bursts();

		// draw the score and ships left
		if (huds_on)
			Draw_Info();

		if (scanner_on)
			Draw_Scanner();

		// last state here so it will overlay
		if (player_state == PLAYER_STATE_DEAD)
		{
			// player is dead
			ready_state = 1;
			ready_counter = 0;

			// player is done!
			gPrintf((SCREEN_WIDTH / 2) - 8 * (lstrlen(TEXT("G A M E   O V E R")) >> 1), SCREEN_HEIGHT / 2, RGB(0, 255, 0), TEXT("G A M E   O V E R"));

		}

		// draw get ready?
		if (ready_state == 0)
		{
			// test if counter is 10 for voice
			if (ready_counter == 10)
				sound[ready_id].play();

			// draw text
			gPrintf(320 - 8 * lstrlen(TEXT("E N T E R I N G   S E C T O R!")) / 2, 200, RGB(0, rand() % 255, 0), TEXT("E N T E R I N G   S E C T O R - ALPHA 11"));

			// increment counter
			if (++ready_counter > 60)
			{
				// set state to ready
				ready_state = 1;
				ready_counter = 0;

			}

		}

		// test for me!
		if (keyboard[DIK_LALT] && keyboard[DIK_RALT] && keyboard[DIK_A])
		{

			andre.drawOn(0, 0);

			andre_up = 1;
		} // end if
		else
		{
			andre_up = 0;
			curPalette.set(game_palette);
		} // end else

		if (andre_up == 1)
		{
			curPalette.set(andre_palette);
			andre_up = 2;
		}


		// flip the surfaces
		fpsSet.adjust();
		Flush();

		// check of user is trying to exit
		if (++player_counter > 10)
			if (keyboard[DIK_ESCAPE])
			{
				// send game back to menu state
				game_state = GAME_STATE_EXIT;

				// stop all sounds
				sound[engines_id].stop();

			}


	}
	else if (game_state == GAME_STATE_PAUSED)
	{
		// pause game
		if (keyboard[DIK_P] && !pause_debounce)
		{
			pause_debounce = 1;
			game_paused = (game_paused) ? 0 : 1;

			sound[beep1_id].play();

		}

		if (!keyboard[DIK_P])
			game_paused = 0;

		if (game_paused)
		{
			// draw text
			gPrintf(320 - 8 * lstrlen(TEXT("G A M E  P A U S E D  -- P R E S S  <P>")) / 2, 200, RGB(255, 0, 0), TEXT("G A M E  P A U S E D  -- P R E S S  <P>"));
		}
		else
			game_state = GAME_STATE_RUNNING;


	}
	else if (game_state == GAME_STATE_EXIT)
	{
		// this is the exit state, called just once
		gameBox.exitFromGameBody();
		game_state = GAME_STATE_WAITING_FOR_EXIT;

	}
	else if (game_state == GAME_STATE_WAITING_FOR_EXIT)
	{
		// wait here in safe state
	}

}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);

	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("键盘控制移动"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}