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

POLYGON2D ship;

void StartUp(void)
{
	VERTEX2D shipShape[24] =
	{ 1,11,
	 2,8,
	 1,7,
	 1,-1,
	 3,-1,
	 3,-2,
	 11,-3,
	 11,-6,
	 3,-7,
	 2,-8,
	 1,-8,
	 1,-7,
	 -1,-7,
	 -1,-8,
	 -2,-8,
	 -3,-7,
	 -11,-6,
	 -11,-3,
	 -3,-2,
	 -3,-1,
	 -1,-1,
	 -1,7,
	 -2,8,
	 -1, 11 };


	ship.copyVertex(shipShape, 24);
	ship.xC = SCREEN_WIDTH / 2;
	ship.yC = SCREEN_HEIGHT / 2;
	ship.vx = 0;
	ship.vy = 0;
	ship.color = GREEN;
}


void GameBody(void)
{

	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	if (KEY_DOWN('A'))
		ship.scale(1.1, 1.1);
	else if (KEY_DOWN('S'))
		ship.scale(0.9, 0.9);

	if (KEY_DOWN('Z'))
		ship.rotate(-5);
	else if (KEY_DOWN('X'))
		ship.rotate(5);

	if (KEY_DOWN(VK_RIGHT))
		ship.translate(5, 0);
	else if (KEY_DOWN(VK_LEFT))
		ship.translate(-5, 0);


	if (KEY_DOWN(VK_UP))
		ship.translate(0, -5);
	else if (KEY_DOWN(VK_DOWN))
		ship.translate(0, 5);

	graphicOut.fillColor();

	BeginDrawOn();
	ship.draw();
	EndDrawOn();

	gPrintf(8, SCREEN_HEIGHT - 16, RGB_DX(0, 255, 255), TEXT("(x,y) = [%d, %d]     "), ship.xC, ship.yC);
	gPrintf(0, 50, RGB_DX(0, 255, 255), TEXT("<A> and <S> - Scale    <Z> and <X> - Rotate     <Arrows> - Translate     <ESC> - Exit"));

	fpsSet.adjust();
	Flush();  //在此之前必须关闭 GDI 要不然 无法切换
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("几何线条绘制飞机"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


