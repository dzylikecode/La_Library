
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
#define OPC_STOP 8  // stop for a moment
#define OPC_RAND 9  // select a random direction
#define OPC_END  -1 // end pattern

#define NUM_PATTERNS    4     // number of patterns in system

SURFACE background;
SOUND backSound;

individeSPRITE ghost;

individeSPRITE bot;
SOUND botSound;

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo20\\");
	background.createFromBitmap(path + TEXT("TREKB8.BMP"));

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("BOTS8.BMP"));
	bot.resize(16);
	for (int i = 0; i < 16; i++)
	{
		bot[i].createFromSurface(116, 60, temp, (i % 2) * (116 + 1) + 1, (i / 2) * (60 + 1) + 1);
	}

	bot.x = 320;
	bot.y = 200;
	bot.setAniSpeed(2);
	bot.content.resize(8);
	for (int i = 0; i < 8; i++)
	{
		bot.content[i] = ANIM(2 * i, 2 * i + 1);
	}

	botSound.create(path + TEXT("ENGINES.WAV"));
	botSound.play(true);


	fpsSet.set(60);
}
// patterns in opcode operand format
int pattern_1[] = { OPC_W, 10, OPC_NW, 10, OPC_N, 10, OPC_NE, 10,
				   OPC_E, 10, OPC_SE, 10, OPC_S, 10, OPC_SW, 10,
				   OPC_W, 10, OPC_RAND, 10,

				   OPC_W, 20, OPC_NW, 10, OPC_N, 20, OPC_NE, 10,
				   OPC_E, 20, OPC_SE, 10, OPC_S, 20, OPC_SW, 10,
				   OPC_W, 10, OPC_END,0 };


int pattern_2[] = { OPC_E, 20, OPC_W, 20, OPC_STOP, 20, OPC_NE, 10,
				   OPC_W, 10, OPC_NW, 10, OPC_SW, 20, OPC_NW, 20,
				   OPC_SW, 20, OPC_NW, 30, OPC_SW, 10, OPC_S, 50,
				   OPC_W, 2, OPC_NW, 2, OPC_N, 2, OPC_NE, 50,
				   OPC_E,2, OPC_SE,2, OPC_S,2, OPC_RAND, 10, OPC_END,0 };



int pattern_3[] = { OPC_N, 10, OPC_S, 10, OPC_N, 10, OPC_S, 10,
					OPC_E, 10, OPC_W, 10, OPC_E, 10, OPC_W, 10,
					OPC_NW, 10, OPC_N, 10, OPC_NE, 10, OPC_N, 10,
					OPC_STOP, 20, OPC_RAND, 5, OPC_E, 50, OPC_S, 50, OPC_W, 50,
					OPC_E, 10, OPC_E, 10, OPC_E, 10, OPC_NW, 100,
					OPC_STOP, 10, OPC_END,0 };


int pattern_4[] = { OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,

				   OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,

				   OPC_W, 100,
				   OPC_NW, 2,OPC_N, 2,OPC_NE, 2,
				   OPC_E, 100,
				   OPC_NE, 2,OPC_N, 2,OPC_NW, 2,

				   OPC_RAND, 10, OPC_RAND, 5,

				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100,

				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100,

				   OPC_SW, 2,OPC_S, 2,OPC_SE, 2,
				   OPC_E, 100,
				   OPC_SE, 2,OPC_S, 2,OPC_SW, 2,
				   OPC_W, 100, OPC_END,0 };
// master pattern array
int* patterns[NUM_PATTERNS] = { pattern_1, pattern_2, pattern_3, pattern_4 };

// used as a index to string lookup to help print out
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
						"OPC_END", };
void Bot_AI()
{
	static int opcode,  // current pattern opcode
		operand; // current operand 

	static int* curr_pattern = nullptr;// current pattern being processed

	static int bot_ip = 0,       // pattern instruction pointer for bot
		bot_counter = 0,       // counter of pattern control
		bot_pattern_index;   // the current pattern being executed

	// test if it's time to process a new instruction
	if (curr_pattern == nullptr)
	{
		// select a random pattern in pattern bank
		bot_pattern_index = rand() % NUM_PATTERNS;

		//随机选择一种模式
		curr_pattern = patterns[bot_pattern_index];

		// now reset instruction pointer
		bot_ip = 0;

		// reset counter
		bot_counter = 0;

	}

	// process next instruction if it's time
	if (--bot_counter <= 0)
	{
		// get next instruction
		//取出模式的指令
		//bot_ip 相当于 PC
		opcode = curr_pattern[bot_ip++];
		operand = curr_pattern[bot_ip++];

		// test what the opcode is
		// 翻译并且执行
		switch (opcode)
		{
		case OPC_E:
		{
			// set direction to east
			bot.vx = 6; bot.vy = 0;

			// set animation to east
			bot.setAniString(opcode);

			// set counter to instruction operand
			bot_counter = operand;//操作数的含义就是时间的意思

		} break;

		case OPC_NE:
		{
			// set direction to northeast
			bot.vx = 6; bot.vy = -6;

			// set animation to northeast
			bot.setAniString(opcode);

			// set counter to instruction operand
			bot_counter = operand;

		} break;

		case OPC_N:
		{
			// set direction to north
			bot.vx = 0; bot.vy = -6;

			// set animation to north
			bot.setAniString(opcode);

			// set counter to instruction operand
			bot_counter = operand;

		} break;

		case OPC_NW:
		{
			// set direction to northwest
			bot.vx = -6; bot.vy = -6;

			// set animation to northwest
			bot.setAniString(opcode);

			// set counter to instruction operand
			bot_counter = operand;

		} break;

		case OPC_W:
		{
			// set direction to west
			bot.vx = -6; bot.vy = 0;

			// set animation to west
			bot.setAniString(opcode);

			// set counter to instruction operand
			bot_counter = operand;

		} break;

		case OPC_SW:
		{
			// set direction to southwest
			bot.vx = -6; bot.vy = 6;

			// set animation to southwest
			bot.setAniString(opcode);

			// set counter to instruction operand
			bot_counter = operand;

		} break;

		case OPC_S:
		{
			// set direction to south
			bot.vx = 0; bot.vy = 6;

			// set animation to south
			bot.setAniString(opcode);

			// set counter to instruction operand
			bot_counter = operand;

		} break;

		case OPC_SE:
		{
			// set direction to southeast
			bot.vx = 6; bot.vy = 6;

			// set animation to southeast
			bot.setAniString(opcode);

			// set counter to instruction operand
			bot_counter = operand;

		} break;

		case OPC_STOP:
		{
			// stop motion
			bot.vx = 0; bot.vy = 0;

			// set counter to instruction operand
			bot_counter = operand;

		} break;

		case OPC_RAND:
		{
			// set direction randomly
			bot.vx = -4 + rand() % 9; bot.vy = -4 + rand() % 9;

			// set animation to south
			bot.setAniString(OPC_S);

			// set counter to instruction operand
			bot_counter = operand;

		} break;

		case OPC_END:
		{
			// stop motion
			bot.vx = 0; bot.vy = 0;

			// select a random pattern in pattern bank
			bot_pattern_index = rand() % NUM_PATTERNS;
			curr_pattern = patterns[bot_pattern_index];

			// now reset instruction pointer
			bot_ip = 0;

			// reset counter
			bot_counter = 0;

		} break;

		default: break;

		}

	}

	// draw stats
	gPrintf(10, 400, RGB(0, 255, 0), TEXT("Pattern #%d"), bot_pattern_index);

	gPrintf(10, 416, RGB(0, 255, 0), TEXT("Opcode=%s Operand=%d"), AToT(opcode_names[opcode]), operand);

	gPrintf(10, 432, RGB(0, 255, 0), TEXT("Instruction Ptr=%d "), bot_ip);

	gPrintf(10, 448, RGB(0, 255, 0), TEXT("Counter=%d "), bot_counter);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	Bot_AI();

	bot.animate();
	bot.move();
	bot.drawOn();


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


