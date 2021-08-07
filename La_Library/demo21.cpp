
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

SURFACE background;

individeSPRITE skeleton;
SOUND skeletonSound;

individeSPRITE bat;
SOUND batSound;

int skelaton_anims[8][4] = { {0,1,0,2},
							 {0 + 4,1 + 4,0 + 4,2 + 4},
							 {0 + 8,1 + 8,0 + 8,2 + 8},
							 {0 + 12,1 + 12,0 + 12,2 + 12},
							 {0 + 16,1 + 16,0 + 16,2 + 16},
							 {0 + 20,1 + 20,0 + 20,2 + 20},
							 {0 + 24,1 + 24,0 + 24,2 + 24},
							 {0 + 28,1 + 28,0 + 28,2 + 28}, };

// directional instructions
#define OPC_E    0  // move west
#define OPC_NE   1  // move northeast
#define OPC_N    2  // move north
#define OPC_NW   3  // move northwest
#define OPC_W    4  // move west
#define OPC_SW   5  // move southwest
#define OPC_S    6  // move south
#define OPC_SE   7  // move southeast

// special instructions
#define OPC_STOP        8  // stop for a moment
#define OPC_RAND        9  // select a random direction
#define OPC_TEST_DIST   10 // test distance 
#define OPC_END        -1  // end pattern

int opcode,                   // general opcode
operand;                  // general operand

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo21\\");
	background.createFromBitmap(path + TEXT("DUNGEON.BMP"));

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("BATS8_2.BMP"));
	bat.resize(5);
	for (int i = 0; i < 5; i++)
	{
		bat[i].createFromSurface(16, 16, temp, i * (16 + 1) + 1, 0 * (16 + 1) + 1);
	}

	bat.x = 320;
	bat.y = 200;
	bat.setAniSpeed(2);
	bat.content.autoOrder();

	batSound.create(path + TEXT("BAT.WAV"));
	batSound.play(true);


	skeleton.resize(4 * 8);
	skeleton.content.resize(8);
	for (int i = 0; i < 8; i++)
	{
		temp.createFromBitmap(path + TEXT("SKELSP%d.BMP"), i);
		skeleton[4 * i + 0].createFromSurface(56, 72, temp, (0) * (56 + 1) + 1, (0) * (72 + 1) + 1);
		skeleton[4 * i + 1].createFromSurface(56, 72, temp, (1) * (56 + 1) + 1, (0) * (72 + 1) + 1);
		skeleton[4 * i + 2].createFromSurface(56, 72, temp, (2) * (56 + 1) + 1, (0) * (72 + 1) + 1);
		skeleton[4 * i + 3].createFromSurface(56, 72, temp, (1) * (56 + 1) + 1, (1) * (72 + 1) + 1);

		skeleton.content[i] = ANIM(0 + 4 * i, 1 + 4 * i, 0 + 4 * i, 2 + 4 * i);
	}

	skeleton.x = 0; skeleton.y = 128;

	skeleton.setAniSpeed(4);


	skeletonSound.create(path + TEXT("LAUGH.WAV"));

	keyboard.create();

	fpsSet.set(60);
}

#define NUM_PATTERNS    4     // number of patterns in system




#define MIN_LONELINESS  100 // minimum distance before skeleton
// gets lonely for its pet bat
const char* opcode_names[] = { "OPC_E",
						"OPC_NE",
						"OPC_N",
						"OPC_NW",
						"OPC_W",
						"OPC_SW",
						"OPC_S",
						"OPC_SE",
						"OPC_STOP",
						"OPC_RAND",
						"OPC_TEST_DIST" };
// patterns in opcode operand format
int pattern_1[] = { OPC_W, 10, OPC_NW, 10, OPC_N, 10, OPC_NE, 10,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_E, 10, OPC_SE, 10, OPC_S, 10, OPC_SW, 10,
				   OPC_W, 10, OPC_RAND, 10,
				   OPC_TEST_DIST, 50, // a distance test                   
				   OPC_W, 20, OPC_NW, 10, OPC_N, 20, OPC_NE, 10,
				   OPC_E, 20, OPC_SE, 10, OPC_S, 20, OPC_SW, 10,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_W, 10, OPC_END,0 };


int pattern_2[] = { OPC_E, 20, OPC_W, 20, OPC_STOP, 20, OPC_NE, 10,
				   OPC_TEST_DIST, 50, // a distance test                   
				   OPC_W, 10, OPC_NW, 10, OPC_SW, 20, OPC_NW, 20,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SW, 20, OPC_NW, 30, OPC_SW, 10, OPC_S, 50,
				   OPC_TEST_DIST, 50, // a distance test                   
				   OPC_W, 2, OPC_NW, 2, OPC_N, 2, OPC_NE, 50,  OPC_TEST_DIST, 50, // a distance test
				   OPC_E,2, OPC_SE,2, OPC_S,2, OPC_RAND, 10, OPC_END,0 };



int pattern_3[] = { OPC_N, 10, OPC_S, 10, OPC_N, 10, OPC_S, 10,
					OPC_TEST_DIST, 50, // a distance test                    
					OPC_E, 10, OPC_W, 10, OPC_E, 10, OPC_W, 10,
					OPC_TEST_DIST, 50, // a distance test
					OPC_NW, 10, OPC_N, 10, OPC_NE, 10, OPC_N, 10,
					OPC_TEST_DIST, 60, // a distance test
					OPC_STOP, 20, OPC_RAND, 5, OPC_E, 50, OPC_S, 50, OPC_W, 50,
					OPC_TEST_DIST, 50, // a distance test                    
					OPC_E, 10, OPC_E, 10, OPC_E, 10, OPC_NW, 100,   OPC_TEST_DIST, 60, // a distance test
					OPC_STOP, 10, OPC_END,0 };


int pattern_4[] = { OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,
				   OPC_TEST_DIST, 50, // a distance test                    
				   OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,
				   OPC_TEST_DIST, 50, // a distance test                    
				   OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_RAND, 10, OPC_RAND, 5,

				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_TEST_DIST, 50, // a distance test
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100, OPC_END,0 };

// master pattern array
int* patterns[NUM_PATTERNS] = { pattern_1, pattern_2, pattern_3, pattern_4 };

void Skeleton_AI()
{
	// this function controls the AI of the skeleton and the pattern
	// processing


	// these are local defines to help compute the direction to move the
	// skeleton if it needs to hunt down bat, note they are all in power
	// of 2 order, hence, they are mutually exclusive

#define WEST_BIT     1
#define EAST_BIT     2
#define NORTH_BIT    4 
#define SOUTH_BIT    8 

	static int* curr_pattern = nullptr;

	static int	skelaton_ip = 0,       // pattern instruction pointer for skeleton
		skelaton_counter = 0,       // counter of pattern control
		skelaton_pattern_index;   // the current pattern being executed


// test if it's time to process a new instruction
	if (curr_pattern == nullptr)
	{
		// select a random pattern in pattern bank
		skelaton_pattern_index = rand() % NUM_PATTERNS;
		curr_pattern = patterns[skelaton_pattern_index];

		// now reset instruction pointer
		skelaton_ip = 0;

		// reset counter
		skelaton_counter = 0;

	} // end if

 // process next instruction if it's time
	if (--skelaton_counter <= 0)
	{
		// get next instruction
		opcode = curr_pattern[skelaton_ip++];
		operand = curr_pattern[skelaton_ip++];

		// test what the opcode is
		switch (opcode)
		{
		case OPC_E:
		{
			// set direction to east
			skeleton.vx = 3; skeleton.vy = 0;

			// set animation to east
			skeleton.setAniString(opcode);

			// set counter to instruction operand
			skelaton_counter = operand;

		} break;

		case OPC_NE:
		{
			skeleton.vx = 3; skeleton.vy = -3;

			skeleton.setAniString(opcode);

			skelaton_counter = operand;

		} break;

		case OPC_N:
		{
			skeleton.vx = 0; skeleton.vy = -3;

			skeleton.setAniString(opcode);

			skelaton_counter = operand;

		} break;

		case OPC_NW:
		{
			skeleton.vx = -3; skeleton.vy = -3;

			skeleton.setAniString(opcode);

			skelaton_counter = operand;

		} break;

		case OPC_W:
		{
			skeleton.vx = -3; skeleton.vy = 0;

			skeleton.setAniString(opcode);

			skelaton_counter = operand;

		} break;

		case OPC_SW:
		{
			skeleton.vx = -3; skeleton.vy = 3;

			skeleton.setAniString(opcode);

			skelaton_counter = operand;

		} break;

		case OPC_S:
		{
			skeleton.vx = 0; skeleton.vy = 3;

			skeleton.setAniString(opcode);

			skelaton_counter = operand;

		} break;

		case OPC_SE:
		{
			skeleton.vx = 3; skeleton.vy = 3;

			skeleton.setAniString(opcode);

			skelaton_counter = operand;

		} break;

		case OPC_STOP:
		{
			// stop motion
			skeleton.vx = 0; skeleton.vy = 0;

			skelaton_counter = operand;

		} break;

		case OPC_RAND:
		{
			skelaton_counter = 0;

		} break;

		case OPC_TEST_DIST:
		{
			// test distance between bat and skeleton
			// if bat is too far, then move toward bat

			int dx = (bat.x - skeleton.x);
			int dy = (bat.y - skeleton.y);

			// test distance against minimum loneliness
			if (sqrt((dx * dx) + (dy * dy)) > MIN_LONELINESS)
			{
				// the skeleton needs to be pointed toward the bat (player)
				// this is a bit hard because we need to point the skeleton
				// in 1 of 8 directions, instead of just giving him a velocity
				// to solve the problem well break it up into a dx and a dy and then
				// use a look up table to set everything up right
				skeletonSound.play(false);

				int direction = 0; // the bit encoded direction

				// first east-west
				if (bat.x > skeleton.x)
					direction |= EAST_BIT;
				else if (bat.x < skeleton.x)
					direction |= WEST_BIT;

				// now north-south
				if (bat.y > skeleton.y)
					direction |= SOUTH_BIT;
				else if (bat.y < skeleton.y)
					direction |= NORTH_BIT;

				// test final direction, note this could be compressed into
				// another look up table, but this is simpler 
				switch (direction)
				{
				case WEST_BIT:
				{
					// set motion

					skeleton.vx = -3; skeleton.vy = 0;
					// set animation 
					skeleton.setAniString(OPC_W);


					// set counter to instruction operand
					skelaton_counter = operand;

				} break;

				case EAST_BIT:
				{

					skeleton.vx = 3; skeleton.vy = 0;
					skeleton.setAniString(OPC_E);


					skelaton_counter = operand;

				} break;

				case NORTH_BIT:
				{

					skeleton.vx = 0; skeleton.vy = -3;
					skeleton.setAniString(OPC_N);


					skelaton_counter = operand;

				} break;

				case SOUTH_BIT:
				{

					skeleton.vx = 0; skeleton.vy = 3;
					skeleton.setAniString(OPC_S);


					skelaton_counter = operand;
				} break;

				case (NORTH_BIT | WEST_BIT):
				{

					skeleton.vx = -3; skeleton.vy = -3;
					skeleton.setAniString(OPC_NW);


					skelaton_counter = operand;
				} break;

				case (NORTH_BIT | EAST_BIT):
				{

					skeleton.vx = 3; skeleton.vy = -3;
					skeleton.setAniString(OPC_NE);


					skelaton_counter = operand;
				} break;

				case (SOUTH_BIT | WEST_BIT):
				{

					skeleton.vx = -3; skeleton.vy = 3;
					skeleton.setAniString(OPC_SW);


					skelaton_counter = operand;
				} break;

				case (SOUTH_BIT | EAST_BIT):
				{

					skeleton.vx = 3; skeleton.vy = 3;
					skeleton.setAniString(OPC_SE);


					skelaton_counter = operand;
				} break;

				default: break;

				}

			}

		} break;

		case OPC_END:
		{
			// stop motion
			skeleton.vx = 0; skeleton.vy = 0;

			// select a random pattern in pattern bank
			skelaton_pattern_index = rand() % NUM_PATTERNS;
			curr_pattern = patterns[skelaton_pattern_index];

			// now reset instruction pointer
			skelaton_ip = 0;

			// reset counter
			skelaton_counter = 0;

		} break;

		default: break;

		}

	}


	// draw stats
	gPrintf(10, 400, RGB_DX(0, 255, 0), TEXT("Pattern #%d"), skelaton_pattern_index);

	if (opcode >= 0)
		gPrintf(10, 416, RGB_DX(0, 255, 0), TEXT("Opcode=%s Operand=%d"), AToT(opcode_names[opcode]), operand);

	gPrintf(10, 432, RGB_DX(0, 255, 0), TEXT("Instruction Ptr=%d "), skelaton_ip);

	gPrintf(10, 448, RGB_DX(0, 255, 0), TEXT("Counter=%d "), skelaton_counter);
}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	keyboard.read();

	Skeleton_AI();

	bat.animate();

	// the animate the skeleton unless its stopped
	if (opcode != OPC_STOP)
		skeleton.animate();

	// allow player to move
	if (keyboard[DIK_RIGHT])
		bat.x += 4;
	else if (keyboard[DIK_LEFT])
		bat.x -= 4;

	if (keyboard[DIK_UP])
		bat.y -= 4;
	else if (keyboard[DIK_DOWN])
		bat.y += 4;

	// test if player is off screen, if so wrap around
	if (bat.x >= SCREEN_WIDTH)
		bat.x = -bat.getWidth();
	else if (bat.x < -bat.getWidth())
		bat.x = SCREEN_WIDTH;

	if (bat.y >= SCREEN_HEIGHT)
		bat.y = -bat.getHeight();
	else if (bat.y < -bat.getHeight())
		bat.y = SCREEN_HEIGHT;

	skeleton.move();

	bat.drawOn();
	skeleton.drawOn();


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


