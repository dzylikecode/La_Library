
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

//物理
const int FRICTION = 0.1;


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



// size of universe
const int UNIVERSE_MIN_X = -8000;
const int UNIVERSE_MAX_X = 8000;
const int UNIVERSE_MIN_Y = -8000;
const int UNIVERSE_MAX_Y = 8000;


int play_x, play_y;

class PLAYER
{
public:
	REAL x, y;
	REAL vx, vy;
}player;

const int starNum = 256;


class STAR
{
private:
	enum PLANE
	{
		PLANE0 = 0,
		PLANE1 = 1,
		PLANE2 = 2,
	};
	static const int color0 = 8;
	static const int color1 = 7;
	static const int color2 = 15;
public:
	int x, y;
private:
	int color;
	PLANE plane;
public:
	bool create()
	{
		switch (rand() % 3)		//展现不同层次	
		{
		case PLANE0:
			color = color0;
			plane = PLANE0;
			break;
		case PLANE1:
			color = color1;
			plane = PLANE1;
			break;
		case PLANE2:
			color = color1;
			plane = PLANE1;
			break;
		}
		x = rand() % SCREEN_WIDTH;  
		y = rand() % SCREEN_HEIGHT;  
		return true;
	}
	void move()
	{
		int plane_0_dx = -int(player.vx) >> 2;
		int plane_0_dy = -int(player.vy) >> 2;

		int plane_1_dx = -int(player.vx) >> 1;
		int plane_1_dy = -int(player.vy) >> 1;

		int plane_2_dx = -int(player.vx);
		int plane_2_dy = -int(player.vy);

		//根据不同的视角，不同的加速
		// test which star field star is in so it is translated with correct
		// perspective

		switch (plane)
		{
		case PLANE0:
			// move the star based on differential motion of player
			// far plane is divided by 4
			x += plane_0_dx;
			y += plane_0_dy;
			break;
		case PLANE1:
			x += plane_1_dx;
			y += plane_1_dy;
			break;
		case PLANE2:
			x += plane_2_dx;
			y += plane_2_dy;
			break;
		}

		if (x >= SCREEN_WIDTH)
		{
			x -= SCREEN_WIDTH;
		}
		else if (x < 0)
		{
			x += SCREEN_WIDTH;
		}

		if (y >= SCREEN_HEIGHT)
		{
			y -= SCREEN_HEIGHT;
		}
		else if (y < 0)
		{
			y += SCREEN_HEIGHT;
		}
	}
};


const int particleNum = 128;
class PARTICLE
{
public:
	enum STATE
	{
		DEAD = 0,
		ALIVE = 1,
	};
	enum TYPE
	{
		FLICKER = 0,
		FADE = 1,
	};
	enum inCOLOR
	{
		RED = 0,
		GREEN = 1,
		BLUE = 2,
		WHITE = 3,
	};
private:
	static const int startRed = 32;
	static const int startGREEN = 96;
	static const int startBLUE = 144;
	static const int startWHITE = 16;
	static const int endRed = 47;
	static const int endGREEN = 111;
	static const int endBLUE = 159;
	static const int endWHITE = 31;
private:
	STATE state;
	TYPE type;
	int counter;
	int counterMax;
	int colorStart;
	int colorEnd;
	int colorCur;
public:
	int x, y;
	int vx, vy;
public:
	PARTICLE() :state(DEAD), type(FADE), x(0), y(0), vx(0), vy(0), counter(0), counterMax(0),
		colorCur(0), colorStart(0), colorEnd(0) {};
	void reset()
	{
		state = DEAD;
		type = FADE;
		x = y = 0;
		vx = vy = 0;
		counterMax = counter = 0;
		colorCur = colorEnd = colorStart = 0;
	}
	bool create(TYPE outType, inCOLOR color, int outCounterMax, int outX, int outY, int outVx, int outVy)
	{
		state = ALIVE;
		type = outType;
		x = outX;
		y = outY;
		vx = outVx;
		vy = outVy;
		counter = 0;
		counterMax = outCounterMax;

		switch (color)
		{
		case inCOLOR::RED:
			colorStart = startRed;
			colorEnd = endRed;
			break;
		case inCOLOR::GREEN:
			colorStart = startGREEN;
			colorEnd = endGREEN;
			break;
		case inCOLOR::BLUE:
			colorStart = startBLUE;
			colorEnd = endBLUE;
			break;
		case inCOLOR::WHITE:
			colorStart = startWHITE;
			colorEnd = endWHITE;
			break;
		}

		if (type == FLICKER)
		{
			colorCur = RAND_RANGE(colorStart, colorEnd);
		}
		else if (type == FADE)
		{
			colorCur = colorStart;
		}
		return true;
	}
	void draw()
	{
		if (state == DEAD)
			return;
		
		//显示在雷达中心位置的多少处
		int posX = x - player.x + SCREEN_WIDTH / 2;
		int posY = y - player.y + SCREEN_HEIGHT / 2;

		if (posX >= SCREEN_WIDTH || posX < 0 || posY >= SCREEN_HEIGHT || posY < 0)
			return;
		GRAPHIC::SetPixel(posX, posY, colorCur);
	}
	void move()
	{
		if (state == DEAD)
			return;
		x += vx;
		y += vy;

		if (type == FLICKER)
		{
			colorCur = RAND_RANGE(colorStart, colorEnd);

			if (++counter >= counterMax)
			{
				state = DEAD;
			}
		}
		else if (type == FADE)
		{
			if (++counter >= counterMax)
			{
				counter = 0;
				if (++colorCur > colorEnd)//逐渐褪色，直到颜色湮灭，就死亡
				{
					state = DEAD;
				}
			}
		}
	}
};


class groupPARTICLE :LINKEDList<PARTICLE>
{
public:
	bool create(PARTICLE::TYPE outType, PARTICLE::inCOLOR color, int outCounterMax, int outX, int outY, int outVx, int outVy, int numParticles)
	{
		PARTICLE temp;
		while (numParticles--)
		{
			int ang = rand() % 16;

			REAL vel = 2 + rand() % 4;
			temp.create(outType, color, outCounterMax,
				outX + RAND_RANGE(-4, 5), outY + RAND_RANGE(-4, 5),
				outVx + cos_look16[ang] * vel, outVy + sin_look16[ang] * 16);
			insertFront(temp);
		}
		return true;
	}
	void drawAndMove()
	{
		BeginDrawOn();
		reset();
		while (!endOfList())
		{
			data().draw();
			data().move();
			next();
		}
		EndDrawOn();
	}
};

const TSTRING path = TEXT(".\\Resource\\demo18\\");

SURFACE Andre;//呵呵，作者的照片，我是服了


void StartUp(void)
{
	SURFACE temp;
	temp.createFromBitmap(path + TEXT("OUTART/ENEMYAI.DAT"));
	Andre.createFromSurface(216, 166, temp, 320 - 216 / 2, 240 - 166 / 2);



	
	temp.createFromBitmap(path + TEXT("BATS8_2.BMP"));
	bat.resize(5);
	for (int i = 0; i < 5; i++)
	{
		bat[i].createFromSurface(16, 16, temp, i * (16 + 1) + 1, 0 * (16 + 1) + 1);
	}

	bat.x = 320;
	bat.y = 200;
	bat.setAniSpeed(2);
	bat.content.resize(1);
	bat.content[0] = ANIM(0, 1, 2, 3);

	batSound.create(path + TEXT("BAT.WAV"));
	batSound.play(true);

	temp.createFromBitmap(path + TEXT("GHOSTS8.BMP"));
	ghost.resize(3);
	for (int i = 0; i < 3; i++)
	{
		ghost[i].createFromSurface(64, 100, temp, i * (64 + 1) + 1, 0 * (100 + 1) + 1);
	}

	ghost.x = 100;
	ghost.y = 200;
	ghost.setAniSpeed(10);
	ghost.content.resize(1);
	ghost.content[0] = ANIM(0, 1, 2, 1);


	backSound.create(path + TEXT("WIND.WAV"));
	backSound.play(true);

	keyboard.create();

	fpsSet.set(60);
}

void Bat_AI()
{
	if (ghost.x > bat.x)
		bat.x += 2;
	else if (ghost.x < bat.x)
		bat.x -= 2;

	// now y-axis
	if (ghost.y > bat.y)
		bat.y += 2;
	else if (ghost.y < bat.y)
		bat.y -= 2;

	// check boundaries
	if (bat.x >= SCREEN_WIDTH)
		bat.x = -bat.getWidth();
	else if (bat.x < -bat.getWidth())
		bat.x = SCREEN_WIDTH;

	if (bat.y >= SCREEN_HEIGHT)
		bat.y = -bat.getHeight();
	else if (bat.y < -bat.getHeight())
		bat.y = SCREEN_HEIGHT;
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);
	
	keyboard.read();

	Bat_AI();

	bat.animate();
	bat.drawOn();

	// allow player to move
	if (keyboard.keyboard_state[DIK_RIGHT])
		ghost.x += 4;
	else if (keyboard.keyboard_state[DIK_LEFT])
		ghost.x -= 4;

	if (keyboard.keyboard_state[DIK_UP])
		ghost.y -= 4;
	else if (keyboard.keyboard_state[DIK_DOWN])
		ghost.y += 4;

	if (ghost.x >= SCREEN_WIDTH)
		ghost.x = -ghost.getWidth();
	else if (ghost.x < -ghost.getWidth())
		ghost.x = SCREEN_WIDTH;

	if (ghost.y >= SCREEN_HEIGHT)
		ghost.y = -ghost.getHeight();
	else if (ghost.y < -ghost.getHeight())
		ghost.y = SCREEN_HEIGHT;

	ghost.animate();
	ghost.drawOn();


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


