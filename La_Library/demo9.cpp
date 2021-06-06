
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

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


FRAMECounter fpsSet;
GAMEBox gameBox;

using namespace GRAPHIC;

SURFACE background;

const int gravityVel = 1;
const COLOR wallColor = RGB_DX(239, 8, 0);
const int skeletonStep = 2;

class SKELETON :public individeSPRITE
{
public:
	enum DIRECTION :int
	{
		EAST = 0,
		NE = 1,
		NORTH = 2,
		NW = 3,
		WEST = 4,
		SW = 5,
		SOUTH = 6,
		SE = 7,
	};
	bool bMove;
public:
	SKELETON()
	{
		bMove = false;
	}
	void setState(DIRECTION dirState) { setAniString(dirState); }
	void animate() { if (bMove) SPRITE::animate(); }
	DIRECTION getState()const { return (DIRECTION)getCurString(); }
}skeleton;

using namespace INPUT_;
KEYBOARD keyboard;

void StartUp(void)
{
	keyboard.create();

	TSTRING path = TEXT(".\\Resource\\demo9\\");
	background.createFromBitmap(path + TEXT("REACTOR2.BMP"));

	skeleton.resize(32);
	skeleton.content.resize(8);
	for (int i = 0; i < 8; i++)
	{
		SURFACE temp;
		temp.createFromBitmap(path + TEXT("SKELSP%d.BMP"), i);
		skeleton[i * 4 + 0].createFromSurface(56, 72, temp, 0 * (56 + 1) + 1, 0 * (72 + 1) + 1);
		skeleton[i * 4 + 1].createFromSurface(56, 72, temp, 1 * (56 + 1) + 1, 0 * (72 + 1) + 1);
		skeleton[i * 4 + 2].createFromSurface(56, 72, temp, 2 * (56 + 1) + 1, 0 * (72 + 1) + 1);
		skeleton[i * 4 + 3].createFromSurface(56, 72, temp, 0 * (56 + 1) + 1, 1 * (72 + 1) + 1);
		skeleton.content[i] = ANIM(i * 4 + 0, i * 4 + 1, i * 4 + 0, i * 4 + 2);
	}
	skeleton.vx = skeleton.vy = 0;
	skeleton.x = 0;
	skeleton.y = 128;
	skeleton.setAniSpeed(12);


	fpsSet.set(120);
}


enum DIRECT :int
{
	EAST = 1,
	WEST = 1 << 1,
	NORTH = 1 << 2,
	SOUTH = 1 << 3,
	NE = NORTH | EAST,
	NW = NORTH | WEST,
	SE = SOUTH | EAST,
	SW = SOUTH | WEST,
};

DIRECT direct = EAST;

int dirX = skeletonStep;
int dirY = 0;

void GameBody(void)
{

	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	keyboard.read();

	if (keyboard[DIK_LEFT])
	{
		direct = WEST;
		dirX = -skeletonStep;
		skeleton.bMove = true;
	}
	else if (keyboard[DIK_RIGHT])
	{
		direct = EAST;
		dirX = skeletonStep;
		skeleton.bMove = true;
	}
	else
	{
		dirX = 0;
	}

	if (keyboard[DIK_UP])
	{
		direct = DIRECT(direct | NORTH);
		dirY = -skeletonStep;
		skeleton.bMove = true;
	}
	else if (keyboard[DIK_DOWN])
	{
		direct = DIRECT(direct | SOUTH);
		dirY = skeletonStep;
		skeleton.bMove = true;
	}
	else
	{
		dirY = 0;
	}

	switch (direct)
	{
	case EAST:
		if (skeleton.getState() != SKELETON::EAST)
			skeleton.setState(SKELETON::EAST);
		break;
	case WEST:
		if (skeleton.getState() != SKELETON::WEST)
			skeleton.setState(SKELETON::WEST);
		break;
	case NORTH:
		if (skeleton.getState() != SKELETON::NORTH)
			skeleton.setState(SKELETON::NORTH);
		break;
	case SOUTH:
		if (skeleton.getState() != SKELETON::SOUTH)
			skeleton.setState(SKELETON::SOUTH);
		break;
	case NW:
		if (skeleton.getState() != SKELETON::NW)
			skeleton.setState(SKELETON::NW);
		break;
	case NE:
		if (skeleton.getState() != SKELETON::NE)
			skeleton.setState(SKELETON::NE);
		break;
	case SW:
		if (skeleton.getState() != SKELETON::SW)
			skeleton.setState(SKELETON::SW);
		break;
	case SE:
		if (skeleton.getState() != SKELETON::SE)
			skeleton.setState(SKELETON::SE);
		break;
	}
	skeleton.x += dirX;
	skeleton.y += dirY;

	if (skeleton.bMove)
	{
		skeleton.animate();
		BeginDrawOn();
		//当碰到障碍时
		if (ScanColor(skeleton.x + 16, skeleton.y + 16, skeleton.x + skeleton.getWidth() - 16, skeleton.y + skeleton.getHeight() - 16, wallColor, wallColor))
		{
			skeleton.x -= dirX;
			skeleton.y -= dirY;
		}
		EndDrawOn();


		if (skeleton.x < 0 || skeleton.x > SCREEN_WIDTH - skeleton.getWidth())
		{
			skeleton.x -= dirX;
		}
		if (skeleton.y < 0 || skeleton.y > SCREEN_HEIGHT - skeleton.getHeight())
		{
			skeleton.y -= dirY;
		}
	}

	skeleton.bMove = false;
	skeleton.drawOn();

	gPrintf(0, 0, RGB(0, 255, 0), TEXT("USE ARROW KEYS TO MOVE, <ESC> to Exit."));
	gPrintf(0, SCREEN_HEIGHT - 32, RGB(0, 255, 0), TEXT("I STILL HAVE A BONE TO PICK!"));
	fpsSet.adjust();
	gPrintf(0, 0, RED_GDI, TEXT("%d"), fpsSet.get());
	Flush();  //在此之前必须关闭 GDI 要不然 无法切换
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


