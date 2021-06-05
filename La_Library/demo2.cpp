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

SURFACE background;

aniDICT alienDict;
const int alienNum = 3;

class ALIEN :public SPRITE
{
public:
	REAL scale;
}alien[3];

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo2\\");
	background.createFromBitmap(path + TEXT("alley8.bmp"));
	SURFACE temp;
	temp.createFromBitmap(path + TEXT("dedsp0.bmp"));
	alienDict.resize(3);
	for (int i = 0; i < 3; i++)
	{
		alienDict[i].createFromSurface(72, 80, temp, i * 73 + 1, 1);
	}

	aniSTRING sequnce = ANIM(0, 1, 0, 2);



	alien[0].y = 116 - 72;
	alien[0].vx = 0.3 * (rand() % 4);


	alien[1].y = 246 - 72;
	alien[1].vx = 0.5 * (rand() % 4) + 0.5;


	alien[2].y = 382 - 72;
	alien[2].vx = 0.3 * (rand() % 4) + 0.3;

	for (int i = 0; i < alienNum; i++)
	{
		alien[i].content.setDict(&alienDict);
		alien[i].content.resize(1);
		alien[i].content[0] = sequnce;

		alien[i].scale = ((REAL)(1 + rand() % 20)) / 10;
		alien[i].x = rand() % SCREEN_WIDTH;
		alien[i].y += (72 - alien[i].scale * 72);
		alien[i].vy = 0;

		alien[i].setAniSpeed(10);
	}

	fpsSet.set(120);
}


void GameBody(void)
{

	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	for (int i = 0; i < 3; i++)
	{
		alien[i].animate();
		alien[i].move();
		int width = alien[i].getWidth() * alien[i].scale;
		int height = alien[i].getHeight() * alien[i].scale;
		alien[i].drawOn(width, height);
	}

	fpsSet.adjust();
	gPrintf(0, 0, RED_GDI, TEXT("%d"), fpsSet.get());
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


