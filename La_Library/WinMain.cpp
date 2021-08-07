
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

// this is the food
typedef struct MNM_TYP
{
	int x, y;     // position of mnm
	float energy;  // when this is 0, the mnm is dead

} MNM, * MNM_PTR;


typedef struct ANT_MEMORY_TYP
{

	float cell[16][16];     // the actual memory
	int   ilayer[16][16];   // used as an input layer to help display engine show
							// areas during communication and forgetfulness

} ANT_MEMORY, * ANT_MEMORY_PTR;




SURFACE background;


// defines for ants
#define NUM_ANTS         16 // just change this to whatever, but only 8 ants will be displayed
							// on HUD
#define ANT_ANIM_UP      0
#define ANT_ANIM_RIGHT   1
#define ANT_ANIM_DOWN    2
#define ANT_ANIM_LEFT    3
#define ANT_ANIM_DEAD    4

// states of ant
#define ANT_WANDERING             0   // moving around randomly
#define ANT_EATING                1   // at a mnm eating it
#define ANT_RESTING               2   // sleeping :)
#define ANT_SEARCH_FOOD           3   // hungry and searching for food

// these are the substates that occur during a search for food
#define ANT_SEARCH_FOOD_S1_SCAN         31  // substate 1
#define ANT_SEARCH_FOOD_S2_WANDER       32  // substate 2
#define ANT_SEARCH_FOOD_S3_VECTOR_2CELL 33  // substate 3
#define ANT_SEARCH_FOOD_S4_VECTOR_2FOOD 34  // substate 4
#define ANT_SEARCH_FOOD_S5              35  // substate 5
#define ANT_SEARCH_FOOD_S6              36  // substate 6
#define ANT_SEARCH_FOOD_S7              37  // substate 7

#define ANT_COMMUNICATING         4   // talking to another ant  
#define ANT_DEAD                  5   // this guy is dead, got too hungry

#define ANT_INDEX_HUNGER_LEVEL       0  // the current hunger level of ant
#define ANT_INDEX_HUNGER_TOLERANCE   1  // the death tolerance of hunger
#define ANT_INDEX_AI_STATE           2  // the artificial intelligence state
#define ANT_INDEX_AI_SUBSTATE        3  // generic substate
#define ANT_INDEX_DIRECTION          4  // direction of motion
#define ANT_INDEX_LAST_TALKED_WITH   5  // last ant talked to
#define ANT_INDEX_MNM_BEING_DEVOURED 6  // the index of mnm being eaten
#define ANT_INDEX_FOOD_TARGET_X      7  // x,y target position of cell or off actual food
#define ANT_INDEX_FOOD_TARGET_Y      8    
#define ANT_INDEX_FOOD_TARGET_ID     9  // the id of the actual piece of food

#define ANT_MEMORY_RESIDUAL_RATE     0.2 // inversely proportional to plasticity of ant memory

#define BITE_SIZE                    5 // bite size of one mouthful in energy units

// defines for food
#define NUM_MNMS         32

class ANT :public SPRITE
{
public:
	int hungerLevel;
	int hungerTolerance;
	int AI_State;
	int lastTalkWith;
	int direction;
	int timeWander;
	int timeDirection;
};

ANT  ants[NUM_ANTS],   // the ants
mnm;              // the little food mnm image
aniDICT antsDict;

MNM food[NUM_MNMS];            // the array of mnms

ANT_MEMORY ants_mem[NUM_ANTS]; // each ant has a 256 cell memory, 16x16 structure

SOUND nicedaySound;     // the ambient wind

void Init_Ants()
{
	for (int i = 0; i < NUM_ANTS; i++)
	{
		ants[i].x = rand() % 472;
		ants[i].y = rand() % SCREEN_HEIGHT;
		ants[i].hungerLevel = 0;
		ants[i].hungerTolerance = 2000 + rand() % 2000;
		ants[i].AI_State = ANT_WANDERING;
		ants[i].lastTalkWith = i;
		ants[i].timeWander = RAND_RANGE(150, 300);
		ants[i].direction = RAND_RANGE(ANT_ANIM_UP, ANT_ANIM_LEFT);
		ants[i].timeDirection = RAND_RANGE(10, 100);
		ants[i].setAniString(ants[i].direction);

		memset(&ants_mem[i], 0, sizeof(ANT_MEMORY));
	}
}

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo22\\");
	background.createFromBitmap(path + TEXT("SIDEWALK28.BMP"));

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("ANTIMG8.BMP"));
	
	
	antsDict.resize(9);
	for (int i = 0; i < 9; i++)
	{
		antsDict[i].createFromSurface(24, 24, temp, i * (24 + 1) + 1, 0 * (24 + 1) + 1);
	}
	ants[0].content.setDict(&antsDict);

	ants[0].x = 320; ants[0].y = 200;
 
	ants[0].content.resize(5);
	ants[0].content[ANT_ANIM_UP] = ANIM(0, 1);
	ants[0].content[ANT_ANIM_RIGHT] = ANIM(2, 3);
	ants[0].content[ANT_ANIM_DOWN] = ANIM(4, 5);
	ants[0].content[ANT_ANIM_LEFT] = ANIM(6, 7);
	ants[0].content[ANT_ANIM_DEAD] = ANIM(8);

	ants[0].setAniSpeed(3);
	ants[0].setAniString(ANT_ANIM_UP);

	for (int i = 1; i < NUM_ANTS; i++)
	{
		ants[i].clone(ants[0]);
	}

	Init_Ants();

	mnm.x = mnm.y = 0;
	static aniDICT mnmDict;
	mnmDict.resize(1);
	mnmDict[0].createFromSurface(8, 8, temp, 1, 141);
	mnm.content.setDict(&mnmDict);
	mnm.content.autoOrder();


	// position all the mnms
	int num_piles = 3 + rand() % 3;
	int curr_mnm = 0;

	for (int piles = 0; piles < num_piles; num_piles++)
	{
		// plop down some mnms at the pile position
		int pile_x = 32 + rand() % 400;
		int pile_y = rand() % 480;

		// compute number of mnms for pile
		int num_mnms_pile = 5 + rand() % 15;

		// now find a position for each
		for (int index = 0; index < num_mnms_pile; index++)
		{
			// select random position and energy level for mnm
			food[curr_mnm].x = pile_x + rand() % 20;
			food[curr_mnm].y = pile_y + rand() % 20;
			food[curr_mnm].energy = 600 + rand() % 1000;

			// increment total number of mnms thus far
			if (++curr_mnm >= NUM_MNMS)
				break;

		} 

		if (++curr_mnm >= NUM_MNMS)
			break;

	} 

	nicedaySound.create(path + TEXT("NICEDAY.WAV"));
	nicedaySound.play(true);

	
	ShowCursor(FALSE);

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


