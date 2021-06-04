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

const int asteroidNum = 64;
POLYGON2D asteroid[asteroidNum];

void StartUp(void)
{
	VERTEX2D asteroidShape[8] = { 33,-3, 9,-18, -12,-9, -21,-12, -9,6, -15,15, -3,27, 21,21 };

	for (int i = 0; i < asteroidNum; i++)
	{
		asteroid[i].copyVertex(asteroidShape, 8);
		asteroid[i].xC = rand() % SCREEN_WIDTH;
		asteroid[i].yC = rand() % SCREEN_HEIGHT;
		asteroid[i].vx = -8 + rand() % 17;
		asteroid[i].vy = -8 + rand() % 17;
		asteroid[i].color = randColor;
	}
}


void GameBody(void)
{

	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	BeginDrawOn();

	for (int i = 0; i < asteroidNum; i++)
	{
		asteroid[i].translate();
		asteroid[i].rotate(5);
		asteroid[i].draw();
		//asteroid[i].drallFill();   //不是多边形不行
		asteroid[i].color = randColor;

		if (asteroid[i].xC > SCREEN_WIDTH + 100)
			asteroid[i].xC = -100;

		if (asteroid[i].yC > SCREEN_HEIGHT + 100)
			asteroid[i].yC = -100;

		if (asteroid[i].xC < -100)
			asteroid[i].xC = SCREEN_WIDTH + 100;

		if (asteroid[i].yC < -100)
			asteroid[i].yC = SCREEN_HEIGHT + 100;

	}

	EndDrawOn();
	fpsSet.adjust();
	Flush();  //在此之前必须关闭 GDI 要不然 无法切换
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("多边形"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


