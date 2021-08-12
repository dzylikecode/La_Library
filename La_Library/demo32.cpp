
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

#define NUM_BALLS       10   // number of pool balls
#define BALL_RADIUS     12   // radius of ball


// extents of table
#define TABLE_MIN_X     100
#define TABLE_MAX_X     500
#define TABLE_MIN_Y     50
#define TABLE_MAX_Y     450


// variable lookup indices
#define INDEX_X               0 
#define INDEX_Y               1  
#define INDEX_XV              2 
#define INDEX_YV              3  
#define INDEX_MASS            4


#define DOT_PRODUCT(ux,uy,vx,vy) ((ux)*(vx) + (uy)*(vy))


SURFACE background;

SOUND sound[8];
float cof_E = 1.0;  // coefficient of restitution, < 1 makes them loose energy
						// during the collision modeling friction, heat, deformation
						// etc. > 1 is impossible, but makes them gain energy!


class BALL :public SPRITE
{
public:
	REAL tempX, tempY;
	REAL tempVx, tempVy;
	REAL mass;
}balls[NUM_BALLS];

POLYGON2D shape;                  // the shape to bounce off

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo31\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("GREENGRID.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("POOLBALLS8.BMP"));

	static aniDICT ballDict;
	ballDict.resize(6);
	for (int i = 0; i < 6; i++)
	{
		int width = 24; int height = 24;
		ballDict[i].createFromSurface(width, height, temp, (i) * (width + 1) + 1, (0) * (height + 1) + 1);
	}
	balls[0].content.setDict(&ballDict);
	balls[0].content.autoOrder();
	balls[0].x = 0; balls[0].y = 0;

	// create all the clones
	for (int index = 1; index < NUM_BALLS; index++)
		balls[index].clone(balls[0]);

	// now set the initial conditions of all the balls
	for (int index = 0; index < NUM_BALLS; index++)
	{
		// set position in center of object
		balls[index].tempX = RAND_RANGE(TABLE_MIN_X + 20, TABLE_MAX_X - 20);
		balls[index].tempY = RAND_RANGE(TABLE_MIN_Y + 20, TABLE_MAX_Y - 20);

		// set initial velocity
		balls[index].tempVx = RAND_RANGE(-100, 100) / 15;
		balls[index].tempVy = RAND_RANGE(-100, 100) / 15;

		balls[index].mass = 1;

		// set ball
		balls[index].setAniWord(rand() % 6);

	}

	// set clipping region
	min_clip_x = TABLE_MIN_X;
	min_clip_y = TABLE_MIN_Y;
	max_clip_x = TABLE_MAX_X;
	max_clip_y = TABLE_MAX_Y;


	sound[0].create(path + TEXT("PBALL.WAV"));

	// clone sounds
	for (int index = 1; index < 8; index++)
		sound[index].cloneFrom(sound[0]);

	ShowCursor(FALSE);
	keyboard.create();
	fpsSet.set(30);
}

void Ball_Sound(void)
{
	// this functions hunts for an open handle to play a collision sound

	// start a hit sound
	for (int sound_index = 0; sound_index < 8; sound_index++)
	{
		// test if this sound is playing
		if (!sound[sound_index].isPlaying())
		{
			sound[sound_index].play();
			break;
		}
	}
}

void Collision_Response(void)
{
	// this function does all the "real" physics to determine if there has
	// been a collision between any ball and any other ball, if there is a collision
	// the function uses the mass of each ball along with the initial velocities to 
	// compute the resulting velocities

	// from the book we know that in general 
	// va2 = (e+1)*mb*vb1+va1(ma - e*mb)/(ma+mb)
	// vb2 = (e+1)*ma*va1+vb1(ma - e*mb)/(ma+mb)

	// and the objects will have direction vectors co-linear to the normal
	// of the point of collision, but since we are using spheres here as the objects
	// we know that the normal to the point of collision is just the vector from the 
	// center's of each object, thus the resulting velocity vector of each ball will
	// be along this normal vector direction

	// step 1: test each object against each other object and test for a collision
	// there are better ways to do this other than a double nested loop, but since
	// there are a small number of objects this is fine, also we want to somewhat model
	// if two or more balls hit simulataneously

	for (int ball_a = 0; ball_a < NUM_BALLS; ball_a++)
	{
		for (int ball_b = ball_a + 1; ball_b < NUM_BALLS; ball_b++)
		{
			if (ball_a == ball_b)
				continue;

			// compute the normal vector from a->b
			float nabx = (balls[ball_b].tempX - balls[ball_a].tempX);
			float naby = (balls[ball_b].tempY - balls[ball_a].tempY);
			float length = sqrt(nabx * nabx + naby * naby);

			// is there a collision?
			if (length <= 2.0 * (BALL_RADIUS * .75))
			{
				// the balls have made contact, compute response

				// compute the response coordinate system axes
				// normalize normal vector
				nabx /= length;
				naby /= length;

				// compute the tangential vector perpendicular to normal, simply rotate vector 90
				float tabx = -naby;
				float taby = nabx;

				// draw collision

				SwitchSurface(true);
				BeginDrawOn();
				POINT ponit = gameBox.getOriginXY();
				// blue is normal
				GRAPHIC::DrawLine(ponit.x + balls[ball_a].tempX + 0.5,
					ponit.y + balls[ball_a].tempY + 0.5,
					ponit.x + balls[ball_a].tempX + 20 * nabx + 0.5,
					ponit.y + balls[ball_a].tempY + 20 * naby + 0.5,
					curPalette[252]);

				// yellow is tangential
				GRAPHIC::DrawLine(ponit.x + balls[ball_a].tempX + 0.5,
					ponit.y + balls[ball_a].tempY + 0.5,
					ponit.x + balls[ball_a].tempX + 20 * tabx + 0.5,
					ponit.y + balls[ball_a].tempY + 20 * taby + 0.5,
					curPalette[251]);

				EndDrawOn();
				SwitchSurface(false);

				// tangential is also normalized since it's just a rotated normal vector

				// step 2: compute all the initial velocities
				// notation ball: (a,b) initial: i, final: f, n: normal direction, t: tangential direction

				float vait = DOT_PRODUCT(balls[ball_a].tempVx,
					balls[ball_a].tempVy,
					tabx, taby);

				float vain = DOT_PRODUCT(balls[ball_a].tempVx,
					balls[ball_a].tempVy,
					nabx, naby);

				float vbit = DOT_PRODUCT(balls[ball_b].tempVx,
					balls[ball_b].tempVy,
					tabx, taby);

				float vbin = DOT_PRODUCT(balls[ball_b].tempVx,
					balls[ball_b].tempVy,
					nabx, naby);


				// now we have all the initial velocities in terms of the n and t axes
				// step 3: compute final velocities after collision, from book we have
				// note: all this code can be optimized, but I want you to see what's happening :)

				float ma = balls[ball_a].mass;
				float mb = balls[ball_b].mass;

				float vafn = (mb * vbin * (cof_E + 1) + vain * (ma - cof_E * mb)) / (ma + mb);
				float vbfn = (ma * vain * (cof_E + 1) - vbin * (ma - cof_E * mb)) / (ma + mb);

				// now luckily the tangential components are the same before and after, so
				float vaft = vait;
				float vbft = vbit;

				// and that's that baby!
				// the velocity vectors are:
				// object a (vafn, vaft)
				// object b (vbfn, vbft)    

				// the only problem is that we are in the wrong coordinate system! we need to 
				// translate back to the original x,y coordinate system, basically we need to 
				// compute the sum of the x components relative to the n,t axes and the sum of
				// the y components relative to the n,t axis, since n,t may both have x,y
				// components in the original x,y coordinate system

				float xfa = vafn * nabx + vaft * tabx;
				float yfa = vafn * naby + vaft * taby;

				float xfb = vbfn * nabx + vbft * tabx;
				float yfb = vbfn * naby + vbft * taby;

				// store results
				balls[ball_a].tempVx = xfa;
				balls[ball_a].tempVy = yfa;

				balls[ball_b].tempVx = xfb;
				balls[ball_b].tempVy = yfb;

				// update position
				balls[ball_a].tempX += balls[ball_a].tempVx;
				balls[ball_a].tempY += balls[ball_a].tempVy;

				balls[ball_b].tempX += balls[ball_b].tempVx;
				balls[ball_b].tempY += balls[ball_b].tempVy;

			}

		}

	}

}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	BeginDrawOn();
	// draw table
	HLine(TABLE_MIN_X, TABLE_MAX_X, TABLE_MIN_Y, curPalette[250]);
	HLine(TABLE_MIN_X, TABLE_MAX_X, TABLE_MAX_Y, curPalette[250]);
	VLine(TABLE_MIN_Y, TABLE_MAX_Y, TABLE_MIN_X, curPalette[250]);
	VLine(TABLE_MIN_Y, TABLE_MAX_Y, TABLE_MAX_X, curPalette[250]);

	EndDrawOn();

	keyboard.read();

	// check for change of e
	if (keyboard[DIK_RIGHT])
		cof_E += .01;
	else if (keyboard[DIK_LEFT])
		cof_E -= .01;

	float total_ke_x = 0, total_ke_y = 0;

	// move all the balls and compute system momentum
	for (int index = 0; index < NUM_BALLS; index++)
	{
		// move the ball
		balls[index].tempX += balls[index].tempVx;
		balls[index].tempY += balls[index].tempVy;

		// add x,y contributions to kinetic energy
		total_ke_x += (balls[index].tempVx * balls[index].tempVx * balls[index].mass);
		total_ke_y += (balls[index].tempVy * balls[index].tempVy * balls[index].mass);

	}

	// test for boundary collision with virtual table edge, no need for collision
	// response here, I know what's going to happen :)
	for (int index = 0; index < NUM_BALLS; index++)
	{
		if ((balls[index].tempX >= TABLE_MAX_X - BALL_RADIUS) ||
			(balls[index].tempX <= TABLE_MIN_X + BALL_RADIUS))
		{
			// invert velocity
			balls[index].tempVx = -balls[index].tempVx;

			balls[index].tempX += balls[index].tempVx;
			balls[index].tempY += balls[index].tempVy;

			// start a hit sound
			Ball_Sound();

		}

		if ((balls[index].tempY >= TABLE_MAX_Y - BALL_RADIUS) ||
			(balls[index].tempY <= TABLE_MIN_Y + BALL_RADIUS))
		{
			// invert velocity
			balls[index].tempVy = -balls[index].tempVy;

			balls[index].tempX += balls[index].tempVx;
			balls[index].tempY += balls[index].tempVy;

			// play sound
			Ball_Sound();

		}

	}

	// draw the balls
	for (int index = 0; index < NUM_BALLS; index++)
	{
		balls[index].x = balls[index].tempX + 0.5 - BALL_RADIUS;
		balls[index].y = balls[index].tempY + 0.5 - BALL_RADIUS;
		balls[index].drawOn();
	}

	// draw the velocity vectors
	BeginDrawOn();
	for (int index = 0; index < NUM_BALLS; index++)
	{
		GRAPHIC::DrawLine(balls[index].tempX + 0.5,
			balls[index].tempY + 0.5,
			balls[index].tempX + 2 * balls[index].tempVx + 0.5,
			balls[index].tempY + 2 * balls[index].tempVy + 0.5,
			curPalette[246]);
	}
	EndDrawOn();

	gPrintf(10, 10, RGB(0, 255, 0), TEXT("ELASTIC Object-Object Collision Response DEMO, Press <ESC> to Exit."));

	gPrintf(10, 30, RGB(0, 255, 0), TEXT("Coefficient of Restitution e=%f, use <RIGHT>, <LEFT> arrow to change."), cof_E);
	gPrintf(10, 465, RGB(0, 255, 0), TEXT("Total System Kinetic Energy Sum(1/2MiVi^2)=%f "), 0.5 * sqrt(total_ke_x * total_ke_x + total_ke_y * total_ke_y));

	fpsSet.adjust();
	gPrintf(0, 0, RED_GDI, TEXT("%d"), fpsSet.get());
	Flush();

	// run collision response algorithm here
	Collision_Response();
}

void GameClose()
{

}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);

	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("¼üÅÌ¿ØÖÆÒÆ¶¯"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


