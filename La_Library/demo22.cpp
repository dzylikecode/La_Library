
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
	int AI_SubState;
	int lastTalkWith;
	int direction;
	int timeWander;
	int timeDirection;
	int beginDevoured;
	int foodTargetX;
	int foodTargetY;
	int foodID;
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

	GRAPHIC::BITMAP bitmap;
	bitmap.load(path + TEXT("SIDEWALK28.BMP"));
	laPALETTE setPal;
	bitmap.getPalette(setPal);
	curPalette.set(setPal);
	background.createFromBitmap(bitmap);

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

	//食物区域
	for (int piles = 0; piles < num_piles; num_piles++)
	{
		// plop down some mnms at the pile position
		int pile_x = 32 + rand() % 400;
		int pile_y = rand() % 480;

		// compute number of mnms for pile
		int num_mnms_pile = 5 + rand() % 15;

		// now find a position for each
		//食物区域散落
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


void Draw_Ants(void)
{
	// this function draws all the ants

	for (int index = 0; index < NUM_ANTS; index++)
	{
		// draw the image
		ants[index].x -= 8; ants[index].y -= 8; // center ant
		ants[index].drawOn();
		ants[index].x += 8; ants[index].y += 8; // fix center

		// draw a little number above ant
		gPrintf(ants[index].x, ants[index].y - 16, RGB(0, 255, 0), TEXT("%d"), index);

		// animate the ant
		if (ants[index].AI_State == ANT_WANDERING ||
			ants[index].AI_State == ANT_SEARCH_FOOD ||
			ants[index].AI_State == ANT_DEAD)
			ants[index].animate();

	}

}

float Food_Near_Ant(int cell_x, int cell_y)
{
	// this functions scan all the food in the universe and tests if any is
	// close to ant in this cell, if so the energy level of the food is scaled 
	// and summed to the "memory" strength of that particular geographical location
	// in the ants memory
	// this algorithm is totally inefficient, in real life this was be called only
	// once and all the cell based positions would be pre-computed since the mnms never
	// move, but this is just to show you how you would do it if they could move...


	float food_sum = 0; // used to tally up food in cell sector

	// is this mnm in the current cell?
	for (int index = 0; index < NUM_MNMS; index++)
	{
		// is this mnm still there
		if (food[index].energy > 0)
		{
			//感应区域，大概 30*30， 相对性计算蚂蚁感应区的食物总量
			// compute cell position, this is dumb, but needed if mnms can move, which
			// they could if you give the ants the ability to pick them up :)
			int mnm_x = food[index].x / 30;
			int mnm_y = food[index].y / 30;

			// is this within the same cell
			if (mnm_x == cell_x && mnm_y == cell_y)
				food_sum += (food[index].energy / 5);

		}
	}
	// now send it back 
	return(food_sum);
}

int Max_Food_In_Cell(int cell_x, int cell_y, int* food_x, int* food_y)
{
	// this function finds the exact location of the mnm with the highest energy in a cell

	float max_food = 0;   // used to tally up food in cell sector
	int max_food_id = 0; // used to track winner

	// is this mnm in the current cell?
	for (int index = 0; index < NUM_MNMS; index++)
	{
		// is this mnm still there
		if (food[index].energy > 0)
		{
			// compute cell position, this is dumb, but needed if mnms can move, which
			// they could if you give the ants the ability to pick them up :)
			int mnm_x = food[index].x / 30;
			int mnm_y = food[index].y / 30;

			// is this within the same cell
			if (mnm_x == cell_x && mnm_y == cell_y)
			{
				// is this higher energy
				if (food[index].energy > max_food)
				{
					// set this as new food
					max_food_id = index;
					max_food = food[index].energy;
					*food_x = food[index].x;
					*food_y = food[index].y;
				}

			}

		}

	}

	// now send it back 
	return(max_food_id);

}


//状态百分比分布然后再求一个随机
int Select_State_Rand(int state1, int prob1,
	int state2, int prob2,
	int state3, int prob3,
	int state4, int prob4,
	int state5, int prob5,
	int state6, int prob6)
{
	// this function simply selects one of state1...state6 based on the probability
	// of each state, if probi is 0 then the state is not considered

	int curr_elem = 0,   // tracks next entry to place in table
		state_prob[100]; // used to hold generated probability look up

	// build probability table
	for (int index = 0; index < prob1; index++)
		state_prob[curr_elem++] = state1;

	for (int index = 0; index < prob2; index++)
		state_prob[curr_elem++] = state2;

	for (int index = 0; index < prob3; index++)
		state_prob[curr_elem++] = state3;

	for (int index = 0; index < prob4; index++)
		state_prob[curr_elem++] = state4;

	for (int index = 0; index < prob5; index++)
		state_prob[curr_elem++] = state5;

	for (int index = 0; index < prob6; index++)
		state_prob[curr_elem++] = state6;

	// now select a state
	return(state_prob[rand() % 100]);

}

void Set_New_State(int new_state, int index, int var1 = 0, int var2 = 0)
{
	// this function sets the state of the ant to new_state

		   // reset all state info
	ants[index].AI_State = 0;
	ants[index].AI_SubState = 0;
	ants[index].timeWander = 0;
	ants[index].timeDirection = 0;

	// now set new state info
	switch (new_state)
	{
	case ANT_WANDERING:      // moving around randomly
	{
		// set the ai state of ant
		ants[index].AI_State = ANT_WANDERING;

		// set how long to wander
		ants[index].timeWander = RAND_RANGE(150, 300);

		// set direction
		ants[index].direction = RAND_RANGE(ANT_ANIM_UP, ANT_ANIM_LEFT);

		// time in that direction
		ants[index].timeDirection = RAND_RANGE(10, 100);

		// start animation
		ants[index].setAniString(ants[index].direction);

	} break;

	case ANT_EATING:         // at a mnm eating it
	{
		// set state to eating
		ants[index].AI_State = ANT_EATING;

		// start eating the mnm indexed by var1 
		ants[index].beginDevoured = var1;

		// counters not used in this state

	} break;

	case ANT_RESTING:        // sleeping :)
	{
		// set the ai state of ant
		ants[index].AI_State = ANT_RESTING;

		// set how long to rest
		ants[index].timeWander = RAND_RANGE(50, 200);


	} break;

	case ANT_SEARCH_FOOD:    // hungry and searching for food
	{
		// set state to search for food
		ants[index].AI_State = ANT_SEARCH_FOOD;

		// start off by scanning for food
		ants[index].AI_SubState = ANT_SEARCH_FOOD_S1_SCAN;

		// initialize targets tp 0
		ants[index].foodTargetX = 0;
		ants[index].foodTargetY = 0;


	} break;

	case ANT_COMMUNICATING:  // talking to another ant  
	{

		// set the ai state of ant
		ants[index].AI_State = ANT_COMMUNICATING;

		// set how long to communicate
		ants[index].timeWander = RAND_RANGE(60, 90);

		// when counter 2 hits 5 then exchange memory
		// thus about 5-10% of the memories will be exchanged
		ants[index].timeDirection = 0;

	} break;

	case ANT_DEAD:           // this guy is dead, got too hungry
	{
		// set the ai state of ant
		ants[index].AI_State = ANT_DEAD;

		// set direction
		ants[index].direction = ANT_ANIM_DEAD;

		// start animation
		ants[index].setAniString(ants[index].direction);

	} break;

	default: break;

	}

}


void Move_Ants(void)
{
	// this function moves all the ants and processes the ai

	// up, right, down, left
	static int ant_movements_x[4] = { 0,2,0,-2 };
	static int ant_movements_y[4] = { -2,0,2, 0 };

	static int clear_ilayer = 0; // tracks when to clear the input memory layer

	int select_new_state = -1;

	for (int index = 0; index < NUM_ANTS; index++)
	{
		// reset new state selector
		select_new_state = -1;

		// what state is ant in?
		switch (ants[index].AI_State)
		{
		case ANT_WANDERING:      // moving around randomly
		{
			// in this state the ant selects random directions and then
			// walks for some period in that direction, if during the ants
			// walk, it stumbles across some food, then it will remember
			// the rough position of the food in int memory
			// burns 1 unit per cycle 

			// move the ant
			ants[index].x += ant_movements_x[ants[index].direction];
			ants[index].y += ant_movements_y[ants[index].direction];

			// test if ant is done with direction and needs a new one
			if (--ants[index].timeDirection < 0)
			{
				// set direction
				ants[index].direction = RAND_RANGE(ANT_ANIM_UP, ANT_ANIM_LEFT);

				// time in this new direction
				ants[index].timeDirection = RAND_RANGE(10, 100);

				// start animation
				ants[index].setAniString(ants[index].direction);

			}

			// burn food
			ants[index].hungerLevel++;

			// update memory with presence of food
			int ant_cell_x = ants[index].x / 30;
			int ant_cell_y = ants[index].y / 30;

			// this updates the i,jth memory cell in ant with info about food
			ants_mem[index].cell[ant_cell_x][ant_cell_y] =
				ANT_MEMORY_RESIDUAL_RATE * ants_mem[index].cell[ant_cell_x][ant_cell_y] +
				(1 - ANT_MEMORY_RESIDUAL_RATE) * Food_Near_Ant(ant_cell_x, ant_cell_y);

			// test if we are done with this state and need a new one?
			if (--ants[index].timeWander < 0)
			{
				// select either rest or wander, search will pre-empt with logic following
				// if hungry, state, probability of state, must sum to 100
				select_new_state = Select_State_Rand(ANT_WANDERING, 70,
					ANT_EATING, 0,
					ANT_RESTING, 30,
					ANT_SEARCH_FOOD, 0,
					ANT_COMMUNICATING, 0,
					ANT_DEAD, 0);
			}


			// test for pre-empt into communication mode
			for (int dialog_ant = 0; dialog_ant < NUM_ANTS; dialog_ant++)
			{

				// try and talk if this isn't the same ant just talked to and this ant
				// is either resting or wandering
				if (((dialog_ant != index) && (ants[index].lastTalkWith != dialog_ant)) &&
					((ants[dialog_ant].AI_State == ANT_RESTING) ||
						(ants[dialog_ant].AI_State == ANT_WANDERING)))
				{
					// are they close enough to talk?
					if ((abs(ants[index].x - ants[dialog_ant].x) < 8) &&
						(abs(ants[index].y - ants[dialog_ant].y) < 8))
					{
						// set both ants to communicate mode
						Set_New_State(ANT_COMMUNICATING, index);
						Set_New_State(ANT_COMMUNICATING, dialog_ant);

						// set communicate partners for chat
						ants[index].lastTalkWith = dialog_ant;
						ants[dialog_ant].lastTalkWith = index;

						break;
					}

				}

			}


			// test for pre-empt into search mode if hunger is 75% tolerance
			if (ants[index].hungerLevel > (0.75 * ants[index].hungerTolerance))
			{
				select_new_state = ANT_SEARCH_FOOD;
			}


		} break;

		case ANT_EATING:         // at a mnm eating it
		{
			// in this state the ant is eating and at rest, the ant will
			// eat from a single mnm until it puts itself to 50% of its hunger tolerance
			// and then stop or if the food runs out it will stop
			// ants eat at a rate of 5 energy units per cycle 
			// burns 1 unit per cycle

			// eat the mnm up until its gone or the hunger level drops to 50%

			// decrease food supply
			food[ants[index].beginDevoured].energy -= BITE_SIZE;

			// transfer to ant and decrease hunger level BITE_SIZE units 
			ants[index].hungerLevel -= BITE_SIZE;

			// is food supply depleted?
			if (food[ants[index].beginDevoured].energy < 0)
			{
				food[ants[index].beginDevoured].energy = 0;

				// transfer to search state or rest a sec
				select_new_state = Select_State_Rand(ANT_WANDERING, 0,
					ANT_EATING, 0,
					ANT_RESTING, 30,
					ANT_SEARCH_FOOD, 70,
					ANT_COMMUNICATING, 0,
					ANT_DEAD, 0);


			}
			// test if done eating?, i.e. hunger < 50% tolerance
			else if (ants[index].hungerLevel < (0.50 * ants[index].hungerTolerance))
			{
				// switch to another state
				select_new_state = Select_State_Rand(ANT_WANDERING, 50,
					ANT_EATING, 0,
					ANT_RESTING, 30,
					ANT_SEARCH_FOOD, 20,
					ANT_COMMUNICATING, 0,
					ANT_DEAD, 0);


			}

		} break;

		case ANT_RESTING:        // sleeping :)
		{
			// ant is simply resting and burns 1 unit per cycle                 

			// test if we are done with this state and need a new one?
			if (--ants[index].timeWander < 0)
			{
				// select either rest or wander, search will pre-empt with logic following
				// if hungry, state, probability of state, must sum to 100
				select_new_state = Select_State_Rand(ANT_WANDERING, 90,
					ANT_EATING, 0,
					ANT_RESTING, 10,
					ANT_SEARCH_FOOD, 0,
					ANT_COMMUNICATING, 0,
					ANT_DEAD, 0);
			}

			// burn food
			ants[index].hungerLevel++;


			// test for pre-empt into search mode if hunger is 50% tolerance
			if (ants[index].hungerLevel > (ants[index].hungerTolerance >> 1))
			{
				select_new_state = ANT_SEARCH_FOOD;
			}



		} break;

		case ANT_SEARCH_FOOD:    // hungry and searching for food
		{
			// in this state the ant is looking for food based on its memory
			// if the memory is blank then random walks
			// if the ant gets to a location and cant find any food where its
			// memory found some then the memory for that food location is degraded by 1
			// searching takes 2 units of energy per cycle

#if 0
			ants[index].varsI[ANT_INDEX_AI_STATE] = ANT_SEARCH_FOOD;

			// start off by scanning for food
			ants[index].varsI[ANT_INDEX_AI_SUBSTATE] = ANT_SEARCH_FOOD_S1_SCAN;

			// initialize targets tp 0
			ants[index].varsI[ANT_INDEX_FOOD_TARGET_X] = 0;
			ants[index].varsI[ANT_INDEX_FOOD_TARGET_Y] = 0;

#endif

			// test substate
			switch (ants[index].AI_SubState)
			{

			case ANT_SEARCH_FOOD_S1_SCAN:
			{
				// this state is transient and doesn't persist, so
				// no energy expended by it

				// scan for a "hot" cell
				float max_energy = 0;
				int cell_x = 0, cell_y = 0;

				for (int index_x = 0; index_x < 16; index_x++)
					for (int index_y = 0; index_y < 16; index_y++)
						// does this cell have more food?
						if (ants_mem[index].cell[index_x][index_y] > max_energy)
						{
							// update new max
							max_energy = ants_mem[index].cell[index_x][index_y];
							cell_x = index_x; cell_y = index_y;

						}

				// test max and see if we have a winner?
				if (max_energy > 0)
				{
					// vector to this cell
					ants[index].AI_SubState = ANT_SEARCH_FOOD_S3_VECTOR_2CELL;

					// send to cell center
					ants[index].foodTargetX = 30 * cell_x + 15;
					ants[index].foodTargetY = 30 * cell_y + 15;

					// set counters to 0
					ants[index].timeWander = ants[index].timeDirection = 0;

				}
				else
				{
					// go into wander mode, no knowledge of food
					// vector to this cell
					ants[index].AI_SubState = ANT_SEARCH_FOOD_S2_WANDER;

					// set counters 
					ants[index].timeWander = RAND_RANGE(100, 150);
					ants[index].timeDirection = 0;

				}

			} break;

			case ANT_SEARCH_FOOD_S2_WANDER:
			{
				// wander and look for food, when done wandering go back and scan

				// burn food
				ants[index].hungerLevel += 1;

				// move the ant
				ants[index].x += ant_movements_x[ants[index].direction];
				ants[index].y += ant_movements_y[ants[index].direction];

				// test if ant is done with direction and needs a new one
				if (--ants[index].timeDirection < 0)
				{
					// set direction
					ants[index].direction = RAND_RANGE(ANT_ANIM_UP, ANT_ANIM_LEFT);

					// time in this new direction
					ants[index].timeDirection = RAND_RANGE(10, 100);

					// start animation
					ants[index].setAniString(ants[index].direction);

				}

				// update memory with presence of food
				int ant_cell_x = ants[index].x / 30;
				int ant_cell_y = ants[index].y / 30;

				// this updates the i,jth memory cell in ant with info about food
				float food_near_ant = Food_Near_Ant(ant_cell_x, ant_cell_y);

				ants_mem[index].cell[ant_cell_x][ant_cell_y] =
					ANT_MEMORY_RESIDUAL_RATE * ants_mem[index].cell[ant_cell_x][ant_cell_y] +
					(1 - ANT_MEMORY_RESIDUAL_RATE) * food_near_ant;

				// test if we just bumped into some food
				if (food_near_ant > 0)
				{
					// find highest source of food
					int food_x = -1;
					int food_y = -1;

					// find the highest food source in cell
					int food_id = Max_Food_In_Cell(ant_cell_x, ant_cell_y, &food_x, &food_y);

					// pre-empt into vector 2 food
					ants[index].AI_SubState = ANT_SEARCH_FOOD_S4_VECTOR_2FOOD;

					// send to exact position
					ants[index].foodTargetX = food_x;
					ants[index].foodTargetY = food_y;

					// set target id of food
					ants[index].foodID = food_id;

					// set counters to 0
					ants[index].timeWander = ants[index].timeDirection = 0;

				}

				// test if we are done with this state and need to go back to scan
				else if (--ants[index].timeWander < 0)
				{
					// go back to scan state                            
					ants[index].AI_SubState = ANT_SEARCH_FOOD_S1_SCAN;

				}


			} break;

			case ANT_SEARCH_FOOD_S3_VECTOR_2CELL:
			{
				// this substate vectors the ant to the center of the cell, once
				// there the ant "looks" to see if there actually is any food, if so
				// vectors to it, else goes back and scans

				// burn food
				ants[index].hungerLevel += 2;

				// pick direction
				int dx = ants[index].foodTargetX - ants[index].x;
				int dy = ants[index].foodTargetY - ants[index].y;

				// pick maxium delta to move in
				if (abs(dx) >= abs(dy))
				{
					// x dominate
					if (dx > 0)
					{
						// move right 
						if (ants[index].direction != ANT_ANIM_RIGHT)
						{
							// start animation right
							ants[index].direction = ANT_ANIM_RIGHT;
							ants[index].setAniString(ants[index].direction);
						}
						ants[index].x += ant_movements_x[ants[index].direction];
					}
					else if (dx < 0)
					{
						// move left
						if (ants[index].direction != ANT_ANIM_LEFT)
						{
							// start animation left
							ants[index].direction = ANT_ANIM_LEFT;
							ants[index].setAniString(ants[index].direction);
						}
						ants[index].x += ant_movements_x[ants[index].direction];

					}

				}
				else
				{
					// y dominate
					if (dy > 0)
					{
						// move down
						if (ants[index].direction != ANT_ANIM_DOWN)
						{
							// start animation down
							ants[index].direction = ANT_ANIM_DOWN;
							ants[index].setAniString(ants[index].direction);
						}

						ants[index].y += ant_movements_y[ants[index].direction];
					}
					else if (dy < 0)
					{
						// move up
						if (ants[index].direction != ANT_ANIM_UP)
						{
							// start animation down
							ants[index].direction = ANT_ANIM_UP;
							ants[index].setAniString(ants[index].direction);
						}

						ants[index].y += ant_movements_y[ants[index].direction];
					}

				}

				// update memory image ????

				  // update memory with presence of food
				int ant_cell_x = ants[index].x / 30;
				int ant_cell_y = ants[index].y / 30;

				// this updates the i,jth memory cell in ant with info about food
				float food_near_ant = Food_Near_Ant(ant_cell_x, ant_cell_y);

				ants_mem[index].cell[ant_cell_x][ant_cell_y] =
					ANT_MEMORY_RESIDUAL_RATE * ants_mem[index].cell[ant_cell_x][ant_cell_y] +
					(1 - ANT_MEMORY_RESIDUAL_RATE) * food_near_ant;

				// now test if target reached
				if (abs(ants[index].x - ants[index].foodTargetX) < 4 &&
					abs(ants[index].y - ants[index].foodTargetY) < 4)
				{
					// center of cell reached, now find the biggest piece of food and
					// vector to it, if none exist then go back to scan

				   // compute cell position
					int ant_cell_x = ants[index].x / 30;
					int ant_cell_y = ants[index].y / 30;

					// this updates the i,jth memory cell in ant with info about food
					float food_near_ant = Food_Near_Ant(ant_cell_x, ant_cell_y);

					// test if we just bumped into some food
					if (food_near_ant > 0)
					{
						// find highest source of food
						int food_x = -1;
						int food_y = -1;

						// find the highest food source in cell
						int food_id = Max_Food_In_Cell(ant_cell_x, ant_cell_y, &food_x, &food_y);

						// pre-empt into vector 2 food
						ants[index].AI_SubState = ANT_SEARCH_FOOD_S4_VECTOR_2FOOD;

						// send to exact position
						ants[index].foodTargetX = food_x;
						ants[index].foodTargetY = food_y;

						// set target id of food
						ants[index].foodID = food_id;

						// set counters to 0
						ants[index].timeWander = ants[index].timeDirection = 0;

					}
					else
					{
						// set mode to scan
						ants[index].AI_SubState = ANT_SEARCH_FOOD_S1_SCAN;
					}

				}

			} break;


			case ANT_SEARCH_FOOD_S4_VECTOR_2FOOD:
			{
				// this substate vectors the ant to the exact x,y of the food 
				// once there, tests to see if there is food there, if so eats it,
				// else go to scan

				// burn food
				ants[index].hungerLevel += 2;

				// pick direction
				int dx = ants[index].foodTargetX - ants[index].x;
				int dy = ants[index].foodTargetY - ants[index].y;

				// pick maxium delta to move in
				if (abs(dx) >= abs(dy))
				{
					// x dominate
					if (dx > 0)
					{
						// move right 
						if (ants[index].direction != ANT_ANIM_RIGHT)
						{
							// start animation right
							ants[index].direction = ANT_ANIM_RIGHT;
							ants[index].setAniString(ants[index].direction);
						}

						ants[index].x += ant_movements_x[ants[index].direction];

					}
					else if (dx < 0)
					{
						// move left
						if (ants[index].direction != ANT_ANIM_LEFT)
						{
							// start animation left
							ants[index].direction = ANT_ANIM_LEFT;
							ants[index].setAniString(ants[index].direction);
						}
						ants[index].x += ant_movements_x[ants[index].direction];

					}

				}
				else
				{
					// y dominate
					if (dy > 0)
					{
						// move down
						if (ants[index].direction != ANT_ANIM_DOWN)
						{
							// start animation down
							ants[index].direction = ANT_ANIM_DOWN;
							ants[index].setAniString(ants[index].direction);
						}

						ants[index].y += ant_movements_y[ants[index].direction];

					}
					else if (dy < 0)
					{
						// move up
						if (ants[index].direction != ANT_ANIM_UP)
						{
							// start animation down
							ants[index].direction = ANT_ANIM_UP;
							ants[index].setAniString(ants[index].direction);
						}

						ants[index].y += ant_movements_y[ants[index].direction];

					}

				}

				// update memory image ????

				  // update memory with presence of food
				int ant_cell_x = ants[index].x / 30;
				int ant_cell_y = ants[index].y / 30;

				// this updates the i,jth memory cell in ant with info about food
				float food_near_ant = Food_Near_Ant(ant_cell_x, ant_cell_y);

				ants_mem[index].cell[ant_cell_x][ant_cell_y] =
					ANT_MEMORY_RESIDUAL_RATE * ants_mem[index].cell[ant_cell_x][ant_cell_y] +
					(1 - ANT_MEMORY_RESIDUAL_RATE) * food_near_ant;


				// now test if target reached
				if (abs(ants[index].x - ants[index].foodTargetX) < 4 &&
					abs(ants[index].y - ants[index].foodTargetY) < 4)
				{
					// food reached, now find the biggest piece of food and
					// vector to it, if none exist then go back to scan

					// is there any food left?
					if (ants[index].foodID > 0)
					{
						// thank god!
						Set_New_State(ANT_EATING, index, ants[index].foodID, 0);
					}
					else
					{
						// go back to scan
						ants[index].AI_SubState = ANT_SEARCH_FOOD_S1_SCAN;

					}

				}

			} break;

			case ANT_SEARCH_FOOD_S5: break;
			case ANT_SEARCH_FOOD_S6: break;
			case ANT_SEARCH_FOOD_S7: break;

			default: break;
			}

		} break;

		case ANT_COMMUNICATING:  // talking to another ant  
		{
			// the ants sit and talk for a little bit, each ant picks
			// 16 memory cells from the matrix of 16x16 and then averages
			// the info, that is, they exchange information
			// this takes 1 unit of energy

			// time to exchange memory?
			if (++ants[index].timeDirection >= 3)
			{
				// reset counter
				ants[index].timeDirection = 0;

				// select cell to exchange and exchange memory imprints
				int memx = rand() % 16;
				int memy = rand() % 16;

				// sum with partners experience
				ants_mem[index].cell[memx][memy] +=
					0.5 * ants_mem[ants[index].lastTalkWith].cell[memx][memy];

				// illuminate green to show communication
				ants_mem[index].ilayer[memx][memy] = 250;

			}

			// test if we are done with this state and need a new one?
			if (--ants[index].timeWander < 0 || ants[ants[index].lastTalkWith].AI_State != ANT_COMMUNICATING)
			{
				// select either rest or wander, search will pre-empt with logic following
				// if hungry, state, probability of state, must sum to 100
				select_new_state = Select_State_Rand(ANT_WANDERING, 90,
					ANT_EATING, 0,
					ANT_RESTING, 10,
					ANT_SEARCH_FOOD, 0,
					ANT_COMMUNICATING, 0,
					ANT_DEAD, 0);
			}


			// burn food
			ants[index].hungerLevel++;

			// test for pre-empt into search mode if hunger is 50% tolerance
			if (ants[index].hungerLevel > (ants[index].hungerTolerance >> 1))
			{
				select_new_state = ANT_SEARCH_FOOD;
			}

		} break;

		case ANT_DEAD:           // this guy is dead, got too hungry
		{
			// the any just sits there and is worm food, no enery expended          


		} break;

		default: break;

		}

		// was there a forced state transition?
		if (ants[index].AI_State != ANT_DEAD && (ants[index].hungerLevel >= ants[index].hungerTolerance))
		{
			// dead ant
			select_new_state = ANT_DEAD;

		}

		///////////////////////////////////////////////////////////////////////////

				// add a little forgetfulness, select a memory at random and weaken it
		int memx = rand() % 16;
		int memy = rand() % 16;

		ants_mem[index].ilayer[memx][memy] = 249; // illuminate red to show degradation

		if (ants_mem[index].cell[memx][memy] > 5)
		{
			ants_mem[index].cell[memx][memy] -= rand() % 5;
		}
		//////////////////////////////////////////////////////////////////////////

		// set new state if needed 
		if (select_new_state >= 0)
		{
			// set the state
			Set_New_State(select_new_state, index);

		}
		////////////////////////////////////////////////////

		   // clear the memory matrix
		if (clear_ilayer >= 5)
			memset(ants_mem[index].ilayer, 0, 1024);


		// this point is where the "physics" system need to operate
		// have any of the state changes or motions caused a physical problem
		// in the universe, we could do it in each state, but this is simpler

		// test for out of bounds
		if (ants[index].x > 472)
		{
			// bounce back a bit
			ants[index].x -= 4;

			// turn to new heading
			switch (rand() % 3)
			{
			case 0: ants[index].direction = ANT_ANIM_LEFT; break;
			case 1: ants[index].direction = ANT_ANIM_UP; break;
			case 2: ants[index].direction = ANT_ANIM_DOWN; break;
			default: break;
			}

			// add a little time to counters     
			ants[index].timeDirection += RAND_RANGE(30, 60);
			ants[index].timeWander += RAND_RANGE(15, 30);

			// start animation
			ants[index].setAniString(ants[index].direction);

		}
		else if (ants[index].x <= 16)
		{
			// bounce back a bit
			ants[index].x += 4;

			// turn to new heading
			switch (rand() % 3)
			{
			case 0: ants[index].direction = ANT_ANIM_RIGHT; break;
			case 1: ants[index].direction = ANT_ANIM_UP; break;
			case 2: ants[index].direction = ANT_ANIM_DOWN; break;
			default: break;
			}

			// add a little time to counter     
			ants[index].timeDirection += RAND_RANGE(30, 60);
			ants[index].timeWander += RAND_RANGE(15, 30);

			// start animation
			ants[index].setAniString(ants[index].direction);

		}

		if (ants[index].y < 0)
			ants[index].y = SCREEN_HEIGHT;
		else if (ants[index].y > SCREEN_HEIGHT)
			ants[index].y = 0;

		///////////////////////////////////////////////////////////////////////////////////

	}

	// update global memory clear counter
	if (++clear_ilayer > 5)
		clear_ilayer = 0;

}

void Draw_Food(void)
{
	// draw the food

	int index;

	for (index = 0; index < NUM_MNMS; index++)
	{
		if (food[index].energy > 0)
		{
			// set position of mnm
			mnm.x = food[index].x - 4; mnm.y = food[index].y - 4;
			mnm.drawOn();

		}

	}

}


void Draw_Info(void)
{
	// this function draws the state information for 8 ants

	static const TCHAR* state_to_text[6] = { TEXT("Wander"), TEXT("Eating"), TEXT("Resting"), TEXT("SearchFood"), TEXT("Talking"), TEXT("Dead") };

	static int intensity_to_index[16] = { 0,65,94,151,166,175,203,208,221,227,233,236,239,242,245,245 };

#define DELTA_LINE 14

	for (int index = 0; index < NUM_ANTS; index++)
	{
		int px = 480;
		int py = 8 + index * 60;


		// only show infor for first 8 ants
		if (index < 8)
		{
			// print name of ant and state
			gPrintf(px, py, RGB(0, 255, 0), TEXT("Ant%d=%s,%d"), index, state_to_text[ants[index].AI_State], ants[index].AI_SubState);


			// print counters
			gPrintf(px, py + DELTA_LINE, RGB(0, 255, 0), TEXT("C1:%d,C2:%d"), ants[index].timeWander, ants[index].timeDirection);

			// print hunger level and tolerance

			gPrintf(px, py + 2 * DELTA_LINE, RGB(0, 255, 0), TEXT("Hunger:%d,Tol:%d"), ants[index].hungerLevel, ants[index].hungerTolerance);

			// draw memory, the brighter the green dot, the stronger the memory
			DrawRectangle(px + 124, py, px + 124 + 18, py + 18, curPalette[0]);

			// dont draw memory if ant dead
			if (ants[index].AI_State != ANT_DEAD)
			{
				BeginDrawOn();
				for (int x = 0; x < 16; x++)
					for (int y = 0; y < 16; y++)
					{
						// get memory and convert into pixel value to disply
						// dark means low recall, bright white means strong memory of food
						float mem_strength = ants_mem[index].cell[x][y];

						// each mnm is worth on average 500, so if a cell has a 2500 then we know there is 
						// a lot of food, use this as the highest strength
						if (mem_strength > 2500)
							mem_strength = 2500;

						// normalize and scale to color range
						int color_intensity = (int)((mem_strength / 2500) * 15);
						GRAPHIC::SetPixel(px + 124 + x + 1, py + y + 1, curPalette[intensity_to_index[color_intensity]]);

						int active_pixel = ants_mem[index].ilayer[x][y];

						if (active_pixel)
							// scan input layer to see where degradation and communication have taken place
							GRAPHIC::SetPixel(px + 124 + x + 1, py + y + 1, curPalette[active_pixel]);
					}

				EndDrawOn();
			}
			else
				// draw flat line :)
			{
				BeginDrawOn();
				HLine(px + 124, px + 124 + 18, py + 9, curPalette[249]);
				EndDrawOn();
			}

		}

	}

}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);


	Move_Ants();
	Draw_Food();
	Draw_Ants();
	Draw_Info();


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


