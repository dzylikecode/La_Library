
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

typedef struct WAYPOINT_TYP
{
	float x, y;

} WAYPOINT, * WAYPOINT_PTR;



SURFACE background;

SOUND sound[2];
int wind_sound_id = -1;        // the ambient wind
int car_sound_id = -1;         // the engine of the car

int vector_display_on = 1;     // used to toggle the informational vector rendering


#define NUM_WAYPOINTS 30 // number of waypoints in path
WAYPOINT path[NUM_WAYPOINTS] = {

{332,122},
{229,108},
{155,97},
{104,100},
{67,119},
{46,159},
{55,229},
{74,283},
{132,364},
{206,407},
{268,412},
{291,405},
{303,379},
{312,274},
{336,244},
{383,233},
{417,240},
{434,278},
{426,328},
{407,388},
{418,415},
{452,429},
{501,419},
{534,376},
{562,263},
{562,188},
{556,112},
{530,100},
{484,97},
{404,116}, };




int Find_Nearest_Waypoint(float x, float y)
{
	// this function finds the nearest waypoint to the sent position

	int near_id = 0;
	int near_dist = 1000,
		test_dist;

	for (int index = 0; index < NUM_WAYPOINTS; index++)
	{
		// is this waypoint closer?
		if ((test_dist = DistanceFast(path[index].x - x, path[index].y - y)) < near_dist)
		{
			// set as nearest waypoint
			near_id = index;
			near_dist = test_dist;
		}

	}

	// test if user want to see all those lines
	if (vector_display_on == 1)
	{
		// draw it
		BeginDrawOn();
		GRAPHIC::DrawLine(path[near_id].x, path[near_id].y - 8, path[near_id].x, path[near_id].y + 8, curPalette[250]);

		GRAPHIC::DrawLine(path[near_id].x - 8, path[near_id].y, path[near_id].x + 8, path[near_id].y, curPalette[250]);
		EndDrawOn();
	}
	// return it
	return(near_id);

}

void Draw_Waypoints(int mode = 1)
{
	// this function draws the waypoints and the path network

	// lock back surface
	BeginDrawOn();

	for (int index = 0; index < NUM_WAYPOINTS; index++)
	{
		// draw network line too?
		if (mode > 0)
			GRAPHIC::DrawLine(path[index].x, path[index].y,
				path[(index + 1) % NUM_WAYPOINTS].x, path[(index + 1) % NUM_WAYPOINTS].y,
				curPalette[249]); // video buffer and memory pitch

		GRAPHIC::SetPixel(path[index].x, path[index].y, curPalette[250]);
		GRAPHIC::SetPixel(path[index].x + 1, path[index].y, curPalette[250]);
		GRAPHIC::SetPixel(path[index].x, path[index].y + 1, curPalette[250]);
		GRAPHIC::SetPixel(path[index].x + 1, path[index].y + 1, curPalette[250]);
	}


	EndDrawOn();
}

class CAR :public SPRITE
{
public:
	float wX, wY;
};

CAR car;

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo23\\");

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("RACETRACK.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("SIMIMG.BMP"));

	static aniDICT carDict;
	carDict.resize(16);
	for (int i = 0; i < 16; i++)
	{
		carDict[i].createFromSurface(16, 16, temp, i * (16 + 1) + 1, 3 * (16 + 1) + 1);
	}
	car.content.setDict(&carDict);
	car.content.autoOrder();
	car.x = 334; car.y = 64;

	car.wX = car.x; car.wY = car.y;
	car.setAniWord(12);

	wind_sound_id = 0;
	car_sound_id = 1;


	sound[wind_sound_id].create(path + TEXT("WIND.WAV"));
	sound[wind_sound_id].play(true);

	sound[car_sound_id].create(path + TEXT("CARIDLE.WAV"));
	sound[car_sound_id].play(true);

	ShowCursor(FALSE);
	keyboard.create();

	fpsSet.set(60);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!

	static float speed = 0; // speed of car

	static int curr_waypoint = -1,
		test_waypoint;

	static int display_debounce = 0; // used to debounce keyboard

	int ai_on = 1; // used to enable display ai based on user taking controls


	background.drawOn(0, 0, false);
	// draw the way points
	if (vector_display_on == 1)
		Draw_Waypoints();

	// read keyboard
	keyboard.read();

	// is user toggling vector display
	if (keyboard[DIK_D] && !display_debounce)
	{
		display_debounce = 1;
		vector_display_on = -vector_display_on;
	}

	if (!keyboard[DIK_D])
		display_debounce = 0;

	// these controls not really needed since AI drives...

	// allow player to move
	if (keyboard[DIK_RIGHT])
	{
		if (++car.getCurWord() > 15)
			car.setAniWord(0);

		// turn off ai this cycle
		ai_on = 0;
	}
	else if (keyboard[DIK_LEFT])
	{
		if (--car.getCurWord() < 0)
			car.setAniWord(15);

		// turn off ai this cycle
		ai_on = 0;
	}

	if (keyboard[DIK_UP])
	{
		if ((speed += 0.1) > 4)
			speed = 4.0;

		// turn off ai this cycle
		ai_on = 0;
	}
	else if (keyboard[DIK_DOWN])
	{
		speed -= 0.1;

		// turn off ai this cycle
		ai_on = 0;
	}

	// ai speed control -- pretty dumb huh :)
	// better to tie it to angle that it's turning
	if (ai_on)
		if ((speed += 0.05) > 4)
			speed = 4.0;

	// apply friction
	if ((speed -= 0.01) < 0)
		speed = 0;

	// make engine sound
	sound[car_sound_id].setFrequency(11000 + speed * 2000);

	// let ai turn the car

	// find nearest waypoint first
	test_waypoint = (Find_Nearest_Waypoint(car.wX, car.wY) + 1) % NUM_WAYPOINTS;

	// test if this is a new waypoint and not the last
	if (test_waypoint != curr_waypoint)
		curr_waypoint = test_waypoint;

	// compute vector toward waypoint
	float wx = (path[curr_waypoint].x - car.wX);
	float wy = (path[curr_waypoint].y - car.wY);

	// normalize wx, wy
	float length = DistanceFast(float(wx + 0.5), float(wy + 0.5));
	wx /= length;
	wy /= length;

	// compute angle in radians
	float car_angle = ((-90 + car.getDictIndex() * 22.5) * PI) / 180;

	// compute velocity vector of car (use look up for cos, sin in real life
	float xv = cos(car_angle);
	float yv = sin(car_angle);

	// draw vectors
	if (vector_display_on == 1)
	{
		BeginDrawOn();
		GRAPHIC::DrawLine(car.wX, car.wY, path[curr_waypoint].x, path[curr_waypoint].y,
			curPalette[250]);

		GRAPHIC::DrawLine(car.wX, car.wY, car.wX + 16 * xv, car.wY + 16 * yv,
			curPalette[246]);
		EndDrawOn();
	}

	// now turn car into waypoint direction, get sign of cross product
	// between each vector
	float sign_cross = (xv * wy - yv * wx);


	if (ai_on)
	{
		// test the sign to determine which way to turn plus a little slosh
		if (sign_cross > 0.3)
		{
			if (++car.getCurWord() > 15)
				car.setAniWord(0);

			gPrintf(320, 460, RGB(255, 255, 255), TEXT("AI Turning %f RIGHT"), sign_cross);

		}
		else if (sign_cross < -0.3)
		{
			if (--car.getCurWord() < 0)
				car.setAniWord(15);

			gPrintf(320, 460, RGB(255, 255, 255), TEXT("AI Turning %f LEFT"), sign_cross);

		}
		else
		{
			gPrintf(320, 460, RGB(255, 255, 255), TEXT("AI Turning 0 %f"), sign_cross);

		}
	}
	else
	{
		gPrintf(320, 460, RGB(255, 255, 255), TEXT("AI Disabled by user override %f"), sign_cross);
	}


	// adjust floating position
	car.wX += (xv * speed);
	car.wY += (yv * speed);

	// test for off screen -- user could cause this
	if (car.wX > SCREEN_WIDTH)
		car.wX = 0;
	else if (car.wX < 0)
		car.wX = SCREEN_WIDTH;

	if (car.wY > SCREEN_HEIGHT)
		car.wY = 0;
	else if (car.wY < 0)
		car.wY = SCREEN_HEIGHT;

	// position sprite on pixel center
	car.x = (0.5 + car.wX - 8);
	car.y = (0.5 + car.wY - 8);

	// draw the car
	car.drawOn();

	// show nearest way point
	gPrintf(64, 460, RGB(255, 255, 255), TEXT("Nearest Waypoint %d"), Find_Nearest_Waypoint(car.wX, car.wY));

	gPrintf(8, 8, RGB(255, 255, 255), TEXT("Path following demo. Use arrow keys to override AI, <D> to toggle info, and <ESC> to Exit."));

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

	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("¼üÅÌ¿ØÖÆÒÆ¶¯"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


