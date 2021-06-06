
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

//用它来当播放器,或者一个模板
individeSPRITE	generator;
individeSPRITE	alien;
individeSPRITE  beacon;

const int multiViewX = 10;
const int multiViewY = 10;

const int universeX = multiViewX * SCREEN_WIDTH;
const int universeY = multiViewY * SCREEN_HEIGHT;

const int maxX = (multiViewX - 1) * SCREEN_WIDTH;
const int maxY = (multiViewY - 1) * SCREEN_HEIGHT;

const int numObject = 256;

int worldX = 0, worldY = 0;
const int moveStep = 8;

struct OBJECT
{
	int x, y;
	enum TYPE :int
	{
		GENERATOR = 0,
		BEACON = 1,
		ALIEN = 2,
	}type;
}object[numObject];

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo8\\");

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("GENS4.BMP"));
	generator.resize(4);
	for (int i = 0; i < 4; i++)
	{
		generator[i].createFromSurface(72, 84, temp, (i % 4) * (72 + 1) + 1, (i / 4) * (84 + 1) + 1);
	}
	generator.content.resize(1);
	generator.content[0] = ANIM(0, 1, 2, 3);
	generator.setAniSpeed(26);

	temp.createFromBitmap(path + TEXT("ALIENS0.BMP"));
	alien.resize(8);
	for (int i = 0; i < 8; i++)
	{
		alien[i].createFromSurface(56, 30, temp, (i % 5) * (56 + 1) + 1, (i / 5) * (30 + 1) + 1);
	}
	alien.content.resize(1);
	alien.content[0] = ANIM(0, 1, 2, 3, 4, 5, 6, 7);
	alien.setAniSpeed(12);

	temp.createFromBitmap(path + TEXT("PIPE0.BMP"));
	beacon.resize(8);
	for (int i = 0; i < 8; i++)
	{
		beacon[i].createFromSurface(42, 36, temp, (i % 6) * (42 + 1) + 1, (i / 6) * (36 + 1) + 1);
	}
	beacon.content.resize(1);
	beacon.content[0] = ANIM(0, 1, 2, 3, 4, 5, 6, 7);
	beacon.setAniSpeed(10);


	for (int i = 0; i < numObject; i++)
	{
		object[i].x = rand() % universeX;
		object[i].y = rand() % universeY;
		object[i].type = (OBJECT::TYPE)(rand() % 3);
	}

	fpsSet.set(60);
}


void GameBody(void)
{

	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();



	if (KEY_DOWN(VK_RIGHT))
	{
		if ((worldX += moveStep) >= maxX)
			worldX = maxX - 1;

	}
	else if (KEY_DOWN(VK_LEFT))
	{
		if ((worldX -= moveStep) < 0)
			worldX = 0;
	}
	if (KEY_DOWN(VK_UP))
	{
		if ((worldY -= moveStep) < 0)
			worldY = 0;

	}
	else if (KEY_DOWN(VK_DOWN))
	{
		if ((worldY += moveStep) >= maxY)
			worldY = maxY - 1;
	}

	int visible = 0;
	individeSPRITE* pObject = nullptr;

	//遍历所有的对象显示-------当然有可见判断
	for (int i = 0; i < numObject; i++)
	{
		switch (object[i].type)
		{
		case OBJECT::ALIEN:
			pObject = &alien;
			break;
		case OBJECT::BEACON:
			pObject = &beacon;
			break;
		case OBJECT::GENERATOR:
			pObject = &generator;
			break;
		}

		int left = object[i].x - worldX;
		int top = object[i].y - worldY;
		int right = left + pObject->getWidth() - 1;
		int bottom = top + pObject->getWidth() - 1;

		if (top > SCREEN_HEIGHT || left > SCREEN_WIDTH || right < 0 || bottom < 0)
			continue;
		else
		{
			pObject->x = left;
			pObject->y = top;
			pObject->drawOn();

			visible++;
		}
	}
	//播放器动画同步更新
	alien.animate();
	beacon.animate();
	generator.animate();

	gPrintf(8, 8, RGB(255, 255, 255), TEXT("USE ARROW KEYS TO MOVE RIGHT & LEFT, <ESC> to Exit."));
	gPrintf(8, SCREEN_HEIGHT - 32 - 24, RGB(0, 255, 0), TEXT("World Position = [%d, %d]  Objects Visible = %d, Objects Clipped = %d   "), worldX, worldY, visible, 256 - visible);
	fpsSet.adjust();
	gPrintf(0, 0, RED_GDI, TEXT("%d"), fpsSet.get());
	Flush();  //在此之前必须关闭 GDI 要不然 无法切换
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("稀疏卷轴"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


