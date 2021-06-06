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

POLYGON2D solidQuad;

void StartUp(void)
{
	VERTEX2D shipShape[4] = { -100,-100, 100,-100, 100,100, -100, 100 };


	solidQuad.copyVertex(shipShape, 4);
	solidQuad.xC = SCREEN_WIDTH / 2;
	solidQuad.yC = SCREEN_HEIGHT / 2;
	solidQuad.vx = 0;
	solidQuad.vy = 0;
	solidQuad.color = GREEN_GDI;
}


void GameBody(void)
{

	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();


	solidQuad.rotate(5);

	static UCHAR colorByte = 1;
	static UCHAR colorVel = 1;

	if (colorByte <= 0 || colorByte >= 255)
	{
		colorVel = -colorVel;
	}
	colorByte += colorVel;

	solidQuad.color = RGB_DX(0, colorByte, 0);

	graphicOut.fillColor();

	BeginDrawOn();
	solidQuad.drawFill();
	EndDrawOn();

	gPrintf(8, 8, RGB_DX(0, 255, 255), TEXT("Press <ESC> to exit."));

	fpsSet.adjust();
	Flush();  //在此之前必须关闭 GDI 要不然 无法切换
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("填充图形旋转"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


