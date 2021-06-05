//"绘制色素图形"
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include "La_Windows.h"
#include "La_WinGDI.h"
#include "La_GameBox.h"
#include "La_Graphic.h"
#include "La_GraphicEx.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


FRAMECounter fpsSet;
GAMEBox gameBox;

using namespace GRAPHIC;

SURFACE background[3];

const int gravityVel = 1;
const COLOR floorColor = RGB_DX(41, 231, 41);
int skeletonStep = 2;

class SKELETON :public individeSPRITE
{
public:
	enum DIRECTION :int
	{
		EAST = 0,
		NEAST = 1,
		NORTH = 2,
		NWEST = 3,
		WEST = 4,
		SWEST = 5,
		SOUTH = 6,
		SEAST = 7,
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

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo6\\");
	for (int i = 0; i < 3; i++)
	{
		background[i].createFromBitmap(path + TEXT("landscape%d_24.bmp"), i + 1);
	}

	skeleton.resize(32);
	skeleton.content.resize(8);
	for (int i = 0; i < 8; i++)
	{
		SURFACE temp;
		temp.createFromBitmap(path + TEXT("quensp%d_24.bmp"), i);
		skeleton[i * 4 + 0].createFromSurface(72, 74, temp, 0 * (72 + 1) + 1, 0 * (74 + 1) + 1);
		skeleton[i * 4 + 1].createFromSurface(72, 74, temp, 1 * (72 + 1) + 1, 0 * (74 + 1) + 1);
		skeleton[i * 4 + 2].createFromSurface(72, 74, temp, 2 * (72 + 1) + 1, 0 * (74 + 1) + 1);
		skeleton[i * 4 + 3].createFromSurface(72, 74, temp, 0 * (72 + 1) + 1, 1 * (74 + 1) + 1);
		skeleton.content[i] = ANIM(i * 4 + 0, i * 4 + 1, i * 4 + 0, i * 4 + 2);
	}
	skeleton.vx = skeleton.vy = 0;
	skeleton.x = 16;
	skeleton.y = 255;
	skeleton.setAniSpeed(4);


	fpsSet.set(30);
}


void GameBody(void)
{

	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	static int curBackground = 0;
	background[curBackground].drawOn(0, 0, false);


	if (KEY_DOWN(VK_LEFT))
	{
		skeletonStep = -abs(skeletonStep);
		skeleton.x += skeletonStep;
		if (skeleton.getState() != SKELETON::WEST)		//不能破坏原来的次序
			skeleton.setState(SKELETON::WEST);			//每次设置的时候都会从初始语句开始
		skeleton.bMove = true;
	}
	else if (KEY_DOWN(VK_RIGHT))
	{
		skeletonStep = abs(skeletonStep);
		skeleton.x += skeletonStep;
		if (skeleton.getState() != SKELETON::EAST)
			skeleton.setState(SKELETON::EAST);
		skeleton.bMove = true;
	}

	skeleton.y += gravityVel;

	BeginDrawOn();
	//当扫描到地板的时候
	while (ScanColor(skeleton.x + 16, skeleton.y + 24, skeleton.x + skeleton.getWidth() - 32, skeleton.y + skeleton.getHeight() - 12, floorColor, floorColor))
	{
		skeleton.y--;
	}
	EndDrawOn();

	if (skeleton.x > SCREEN_WIDTH - skeleton.getWidth() / 2)
	{
		skeleton.x -= skeletonStep;
		if (curBackground < 2)
		{
			curBackground++;
			skeleton.x = -skeleton.getWidth() / 2;
		}
	}
	else if (skeleton.x < -skeleton.getWidth() / 2)
	{
		skeleton.x -= skeletonStep;
		if (curBackground > 0)
		{
			curBackground--;
			skeleton.x = SCREEN_WIDTH - skeleton.getWidth() / 2;
		}
	}
	skeleton.animate();
	skeleton.bMove = false;
	skeleton.drawOn();

	gPrintf(8, 8, RGB_DX(255, 255, 255), TEXT("USE ARROW KEYS TO MOVE RIGHT & LEFT, <ESC> to Exit."));
	fpsSet.adjust();
	gPrintf(0, 0, RED, TEXT("%d"), fpsSet.get());
	Flush();  //在此之前必须关闭 GDI 要不然 无法切换
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("加载图片"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


