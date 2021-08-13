
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

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

FRAMECounter fpsSet;
GAMEBox gameBox;

using namespace AUDIO;
using namespace GDI;
using namespace GRAPHIC;
using namespace INPUT_;

KEYBOARD keyboard;

// defines for polygon cannon
#define CANNON_X0       39  // position of tip of cannon
#define CANNON_Y0       372
#define NUM_PROJECTILES 16  // number of projectiles

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

#define MAX_PARTICLES                     256

// color ranges
#define COLOR_RED_START                   32
#define COLOR_RED_END                     47

#define COLOR_GREEN_START                 96
#define COLOR_GREEN_END                   111

#define COLOR_BLUE_START                  144
#define COLOR_BLUE_END                    159

#define COLOR_WHITE_START                 16
#define COLOR_WHITE_END                   31

SURFACE background;

class PROJECTILE
{
public:
	int state;      // state 0 off, 1 on
	float x, y;      // position
	float vx, vy;   // velocity
	int detonate;   // tracks when the projectile detonates
	PROJECTILE() :
		state(0),
		x(0), y(0),
		vx(0), vy(0),
		detonate(0) {};
};

// a single particle
class PARTICLE
{
public:
	int state;
	int type;            // type of particle effect
	float x, y;           // world position of particle
	float vx, vy;         // velocity of particle
	int curr_color;      // the current rendering color of particle
	int start_color;     // the start color or range effect
	int end_color;       // the ending color of range effect
	int counter;         // general state transition timer
	int max_count;       // max value for counter
	PARTICLE() :
		state(PARTICLE_STATE_DEAD),
		type(PARTICLE_TYPE_FADE),
		x(0),
		y(0),
		vx(0),
		vy(0),
		start_color(0),
		end_color(0),
		curr_color(0),
		counter(0),
		max_count(0) {};
public:
	void create(int type, int color, int count)
	{
		// set general state info
		state = PARTICLE_STATE_ALIVE;
		type = type;
		counter = 0;
		max_count = count;

		// set color ranges, always the same
		switch (color)
		{
		case PARTICLE_COLOR_RED:
		{
			start_color = COLOR_RED_START;
			end_color = COLOR_RED_END;
		} break;

		case PARTICLE_COLOR_GREEN:
		{
			start_color = COLOR_GREEN_START;
			end_color = COLOR_GREEN_END;
		} break;

		case PARTICLE_COLOR_BLUE:
		{
			start_color = COLOR_BLUE_START;
			end_color = COLOR_BLUE_END;
		} break;

		case PARTICLE_COLOR_WHITE:
		{
			start_color = COLOR_WHITE_START;
			end_color = COLOR_WHITE_END;
		} break;

		break;

		}

		// what type of particle is being requested
		if (type == PARTICLE_TYPE_FLICKER)
		{
			// set current color
			curr_color = RAND_RANGE(start_color, end_color);

		}
		else
		{
			// particle is fade type
			// set current color
			curr_color = start_color;
		}
	}
};

float gravity_force = 0.2;  // gravity
float wind_force = -0.01; // wind resistance

float particle_wind = 0;    // assume it operates in the X direction
float particle_gravity = .02; // assume it operates in the Y direction

PARTICLE particles[MAX_PARTICLES]; // the particles for the particle engine


SOUND cannonSound[8]; // sound ids for cannon
SOUND explosionSound[8]; // explosion ids

POLYGON2D cannon; // the ship

PROJECTILE missiles[NUM_PROJECTILES]; // array of missiles

void Start_Particle(int type, int color, int count, int x, int y, int xv, int yv)
{
	// this function starts a single particle

	int pindex = -1; // index of particle

	// first find open particle
	for (int index = 0; index < MAX_PARTICLES; index++)
		if (particles[index].state == PARTICLE_STATE_DEAD)
		{
			pindex = index;
			break;
		}
	// did we find one
	if (pindex == -1)
		return;

	// set general state info
	particles[pindex].x = x;
	particles[pindex].y = y;
	particles[pindex].vx = xv;
	particles[pindex].vy = yv;

	particles[pindex].create(type, color, count);

}

void Start_Particle_Explosion(int type, int color, int count,
	int x, int y, int xv, int yv, int num_particles)
{
	// this function starts a particle explosion at the given position and velocity

	while (--num_particles >= 0)
	{
		// compute random trajectory angle
		int ang = rand() % 360;

		// compute random trajectory velocity
		float vel = 2 + rand() % 4;

		Start_Particle(type, color, count,
			x + RAND_RANGE(-4, 4), y + RAND_RANGE(-4, 4),
			xv + cos_look[ang] * vel, yv + sin_look[ang] * vel);
	}
}

void Start_Particle_Ring(int type, int color, int count,
	int x, int y, int xv, int yv, int num_particles)
{
	// this function starts a particle explosion at the given position and velocity
	// note the use of look up tables for sin,cos

	// compute random velocity on outside of loop
	float vel = 2 + rand() % 4;

	while (--num_particles >= 0)
	{
		// compute random trajectory angle
		int ang = rand() % 360;

		// start the particle
		Start_Particle(type, color, count,
			x, y,
			xv + cos_look[ang] * vel,
			yv + sin_look[ang] * vel);

	}

}

void Draw_Particles(void)
{
	// this function draws all the particles

	// lock back surface
	BeginDrawOn();

	for (int index = 0; index < MAX_PARTICLES; index++)
	{
		// test if particle is alive
		if (particles[index].state == PARTICLE_STATE_ALIVE)
		{
			// render the particle, perform world to screen transform
			int x = particles[index].x;
			int y = particles[index].y;

			// test for clip
			if (x >= SCREEN_WIDTH || x < 0 || y >= SCREEN_HEIGHT || y < 0)
				continue;

			// draw the pixel
			GRAPHIC::SetPixel(x, y, curPalette[particles[index].curr_color]);
		}
	}
	EndDrawOn();

}

void Process_Particles(void)
{
	// this function moves and animates all particles

	for (int index = 0; index < MAX_PARTICLES; index++)
	{
		// test if this particle is alive
		if (particles[index].state == PARTICLE_STATE_ALIVE)
		{
			// translate particle
			particles[index].x += particles[index].vx;
			particles[index].y += particles[index].vy;

			// update velocity based on gravity and wind
			particles[index].vx += particle_wind;
			particles[index].vy += particle_gravity;

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


void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo34\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("PARTICLEGRID.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

	cannonSound[0].create(path + TEXT("CANNON.WAV"));
	for (int i = 1; i < 8; i++)
		cannonSound[i].cloneFrom(cannonSound[0]);

	explosionSound[0].create(path + TEXT("EXP1.WAV"));
	for (int i = 1; i < 8; i++)
		explosionSound[i].cloneFrom(explosionSound[0]);


	// define points of cannon
	VERTEX2D cannon_vertices[4] = { 0,-2, 30,0, 30,2, 0,2, };

	cannon.xC = CANNON_X0; // position it
	cannon.yC = CANNON_Y0;
	cannon.vx = 0;
	cannon.vy = 0;
	cannon.color = curPalette[95]; // green
	cannon.copyVertex(cannon_vertices, 4);

	ShowCursor(FALSE);
	keyboard.create();
	fpsSet.set(15);//要 15 的时候时间才很合理
}


void Cannon_Sound(void)
{
	// this functions hunts for an open handle to play a cannon sound

	// start a hit sound
	for (int sound_index = 0; sound_index < 8; sound_index++)
	{
		// test if this sound is playing
		if (!cannonSound[sound_index].isPlaying())
		{
			cannonSound[sound_index].play();
			break;
		}
	}

}

void Explosion_Sound(void)
{
	// this functions hunts for an open handle to play a explosion sound

	// start a hit sound
	for (int sound_index = 0; sound_index < 8; sound_index++)
	{
		// test if this sound is playing
		if (!explosionSound[sound_index].isPlaying())
		{
			explosionSound[sound_index].play();
			break;
		}
	}
}

void Move_Projectiles(void)
{
	// this function moves all the projectiles and does the physics model
	for (int index = 0; index < NUM_PROJECTILES; index++)
	{
		if (missiles[index].state == 1)
		{
			// translate
			missiles[index].x += missiles[index].vx;
			missiles[index].y += missiles[index].vy;

			// apply forces
			missiles[index].vx += wind_force;
			missiles[index].vy += gravity_force;

			// update detonatation counter
			if (--missiles[index].detonate <= 0)
			{
				// select a normal or ring

				if (RAND_RANGE(0, 3) == 0)
				{
					// start a particle explosion
					Start_Particle_Ring(PARTICLE_TYPE_FADE, PARTICLE_COLOR_RED + rand() % 4, RAND_RANGE(2, 5),
						missiles[index].x, missiles[index].y,
						0, 0, RAND_RANGE(75, 100));
				}
				else
				{
					Start_Particle_Explosion(PARTICLE_TYPE_FADE, PARTICLE_COLOR_RED + rand() % 4, RAND_RANGE(2, 5),
						missiles[index].x, missiles[index].y,
						0, 0, RAND_RANGE(20, 50));
				}
				// make some noise
				Explosion_Sound();
				// kill the missile
				missiles[index].state = 0;

			}

			// test for off screen
			else if (missiles[index].x >= SCREEN_WIDTH ||
				missiles[index].y >= SCREEN_HEIGHT ||
				missiles[index].y < 0)
			{
				// kill the missile
				missiles[index].state = 0;
			}
		}
	}
}

void Draw_Projectiles(void)
{
	// this function draws all the projectiles 
	for (int index = 0; index < NUM_PROJECTILES; index++)
	{
		// is this one alive?
		if (missiles[index].state == 1)
		{
			GRAPHIC::DrawRectangle(missiles[index].x - 1, missiles[index].y - 1,
				missiles[index].x + 1, missiles[index].y + 1,
				curPalette[95]);
		}
	}
}

void Fire_Projectile(int angle, float vel)
{
	// this function starts a projectile with the given angle and velocity 
	// at the tip of the cannon
	for (int index = 0; index < NUM_PROJECTILES; index++)
	{
		// find an open projectile
		if (missiles[index].state == 0)
		{
			// set this missile in motion at the head of cannon with the proper angle
			missiles[index].x = cannon[1].x + cannon.xC;
			missiles[index].y = cannon[1].y + cannon.yC;

			// compute velocity vector based on angle
			missiles[index].vx = vel * cos_look[angle];
			missiles[index].vy = -vel * sin_look[angle];

			// set detonation time
			missiles[index].detonate = RAND_RANGE(30, 40);

			// mark as active
			missiles[index].state = 1;

			// make sound
			Cannon_Sound();

			// bail
			break;
		}
	}
}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);


	keyboard.read();

	static int   curr_angle = 0; // current angle of elevation from horizon
	static float curr_vel = 10; // current velocity of projectile

	BeginDrawOn();
	cannon.draw();
	EndDrawOn();

	// test for rotate
	if ((curr_angle < 90) && keyboard[DIK_UP]) // rotate left
	{
		cannon.rotate(-5);
		curr_angle += 5;
	}
	else if ((curr_angle > 0) && keyboard[DIK_DOWN]) // rotate right
	{
		cannon.rotate(5);
		curr_angle -= 5;
	}

	// test for projectile velocity
	if (keyboard[DIK_RIGHT])
	{
		if (curr_vel < 30) curr_vel += 0.1;
	}
	else if (keyboard[DIK_LEFT])
	{
		if (curr_vel > 0) curr_vel -= 0.1;
	}

	// test for wind force
	if (keyboard[DIK_W])
	{
		if (particle_wind < 2) particle_wind += 0.01;
	}
	else if (keyboard[DIK_E])
	{
		if (particle_wind > -2) particle_wind -= 0.01;
	}

	// test for gravity force
	if (keyboard[DIK_G])
	{
		if (particle_gravity < 5) particle_gravity += 0.01;
	}
	else if (keyboard[DIK_B])
	{
		if (particle_gravity > -5) particle_gravity -= 0.01;
	}

	// test for fire!
	if (keyboard[DIK_LCONTROL])
	{
		Fire_Projectile(curr_angle, curr_vel);
	}

	// move all the projectiles
	Move_Projectiles();

	// move particles
	Process_Particles();

	// draw the projectiles
	Draw_Projectiles();

	// draw the particles
	Draw_Particles();

	gPrintf(10, 10, RGB(0, 255, 0), TEXT("Particle System DEMO, Press <ESC> to Exit."));

	gPrintf(10, 25, RGB(0, 255, 0), TEXT("<RIGHT>, <LEFT> to adjust velocity, <UP>, <DOWN> to adjust angle"));
	gPrintf(10, 40, RGB(0, 255, 0), TEXT("<G>, <B> adjusts particle gravity, <W>, <E> adjusts particle wind, <CTRL> to fire."));
	gPrintf(10, 60, RGB(0, 255, 0), TEXT("Cannon: Ang=%d, Vel=%f"), curr_angle, curr_vel);
	gPrintf(10, 75, RGB(0, 255, 0), TEXT("Particle: Wind force=%f, Gravity Force=%f"), particle_wind, particle_gravity);


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


