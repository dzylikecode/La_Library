
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

// center position of shape
#define SHAPE_CENTER_X         320
#define SHAPE_CENTER_Y         240

// variable lookup indices
#define INDEX_X               0 
#define INDEX_Y               1  
#define INDEX_XV              2 
#define INDEX_YV              3  

#define DOT_PRODUCT(ux,uy,vx,vy) ((ux)*(vx) + (uy)*(vy))


SURFACE background;

SOUND sound[8];


class BALL :public SPRITE
{
public:
	REAL tempX, tempY;
	REAL tempVx, tempVy;
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
		balls[index].tempX = RAND_RANGE(SHAPE_CENTER_X - 50, SHAPE_CENTER_X + 50);
		balls[index].tempY = RAND_RANGE(SHAPE_CENTER_Y - 50, SHAPE_CENTER_Y + 50);

		do
		{
			// set initial velocity
			balls[index].tempVx = RAND_RANGE(-100, 100) / 30;
			balls[index].tempVy = RAND_RANGE(-100, 100) / 30;
		} while (balls[index].tempVx == 0 && balls[index].tempVy == 0);

		// set ball
		balls[index].setAniWord(rand() % 6);

	}

	// define points of shape
	VERTEX2D shape_vertices[10] =
	{ 328 - SHAPE_CENTER_X,60 - SHAPE_CENTER_Y,
	  574 - SHAPE_CENTER_X,162 - SHAPE_CENTER_Y,
	  493 - SHAPE_CENTER_X,278 - SHAPE_CENTER_Y,
	  605 - SHAPE_CENTER_X,384 - SHAPE_CENTER_Y,
	  484 - SHAPE_CENTER_X,433 - SHAPE_CENTER_Y,
	  306 - SHAPE_CENTER_X,349 - SHAPE_CENTER_Y,
	  150 - SHAPE_CENTER_X,413 - SHAPE_CENTER_Y,
	  28 - SHAPE_CENTER_X,326 - SHAPE_CENTER_Y,
	  152 - SHAPE_CENTER_X,281 - SHAPE_CENTER_Y,
	  73 - SHAPE_CENTER_X,138 - SHAPE_CENTER_Y };

	shape.xC = SHAPE_CENTER_X;
	shape.yC = SHAPE_CENTER_Y;
	shape.vx = 0;
	shape.vy = 0;
	shape.color = curPalette[250]; // green
	shape.copyVertex(shape_vertices, 10);

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

void Compute_Collisions(void)
{
	// this function computes if any ball has hit one of the edges of the polygon
	// if so the ball is bounced

	float length, s, t, s1x, s1y, s2x, s2y, p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y, xi, yi, npx, npy, Nx, Ny, Fx, Fy;


	for (int index = 0; index < NUM_BALLS; index++)
	{
		// first move ball
		balls[index].tempX += balls[index].tempVx;
		balls[index].tempY += balls[index].tempVy;

		// now project velocity vector forward and test for intersection with all lines of polygon shape

		// build up vector in direction of trajectory
		p0x = balls[index].tempX;
		p0y = balls[index].tempY;

#if 1 // this is the velocity vector used as segment 1
		p1x = balls[index].tempX + balls[index].tempVx;
		p1y = balls[index].tempY + balls[index].tempVy;
		s1x = p1x - p0x;
		s1y = p1y - p0y;

		// normalize and scale to 1.25*radius
		length = sqrt(s1x * s1x + s1y * s1y);
		s1x = 1.25 * BALL_RADIUS * s1x / length;
		s1y = 1.25 * BALL_RADIUS * s1y / length;
		p1x = p0x + s1x;
		p1y = p0y + s1y;


#endif

		// for each line try and intersect
		for (int line = 0; line < shape.getNumVertex(); line++)
		{
			// now build up vector based on line
			p2x = shape[line].x + shape.xC;
			p2y = shape[line].y + shape.yC;

			p3x = shape[(line + 1) % (shape.getNumVertex())].x + shape.xC;
			p3y = shape[(line + 1) % (shape.getNumVertex())].y + shape.yC;

			s2x = p3x - p2x;
			s2y = p3y - p2y;

#if 0 // this is the perp vector used as segment 1
			// normalize s2x, s2y to create a perpendicular collision vector from the ball center
			length = sqrt(s2x * s2x + s2y * s2y);
			s1x = BALL_RADIUS * s2y / length;
			s1y = -BALL_RADIUS * s2x / length;
			p1x = p0x + s1x;
			p1y = p0y + s1y;
#endif
			// compute s and t, the parameters
			s = (-s1y * (p0x - p2x) + s1x * (p0y - p2y)) / (-s2x * s1y + s1x * s2y);
			t = (s2x * (p0y - p2y) - s2y * (p0x - p2x)) / (-s2x * s1y + s1x * s2y);

			// test for valid range (0..1)
			if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
			{
				// find collision point based on s
				xi = p0x + s * s1x;
				yi = p0y + s * s1y;

				// now we know point of intersection, reflect ball at current location

				// N = (-I . N')*N'
				// F = 2*N + I
				npx = -s2y;
				npy = s2x;

				// normalize p
				length = sqrt(npx * npx + npy * npy);
				npx /= length;
				npy /= length;

				// compute N = (-I . N')*N'
				Nx = -(balls[index].tempVx * npx + balls[index].tempVy * npy) * npx;
				Ny = -(balls[index].tempVx * npx + balls[index].tempVy * npy) * npy;

				// compute F = 2*N + I
				Fx = 2 * Nx + balls[index].tempVx;
				Fy = 2 * Ny + balls[index].tempVy;

				// update velocity with results
				balls[index].tempVx = Fx;
				balls[index].tempVy = Fy;

				balls[index].tempX += balls[index].tempVx;
				balls[index].tempY += balls[index].tempVy;

				// make sound
				Ball_Sound();


				// break out of for line
				//break;

			}

		}

	}

}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	static int rotate = 0;
	BeginDrawOn();
	shape.draw();
	EndDrawOn();

	// have a little fun
	if (++rotate > 10)
	{
		shape.rotate(1);
		rotate = 0;
	}

	keyboard.read();

	// move the balls and compute collisions
	Compute_Collisions();

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

	gPrintf(10, 10, RGB(0, 255, 0), TEXT("Object to Contour Collision DEMO, Press <ESC> to Exit."));

	fpsSet.adjust();
	gPrintf(0, 0, RED_GDI, TEXT("%d"), fpsSet.get());
	Flush();

	// run collision algorithm here
	Compute_Collisions();
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


