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

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 768;


FRAMECounter fpsSet;
GAMEBox gameBox;

const int faceWidth = 8;
const int faceHeight = 8;
bool happy_bitmap[64] = { 0,0,0,0,0,0,0,0,
						  0,0,1,1,1,1,0,0,
						  0,1,0,1,1,0,1,0,
						  0,1,1,1,1,1,1,0,
						  0,1,0,1,1,0,1,0,
						  0,1,1,0,0,1,1,0,
						  0,0,1,1,1,1,0,0,
						  0,0,0,0,0,0,0,0 };

bool sad_bitmap[64] = { 0,0,0,0,0,0,0,0,
						0,0,1,1,1,1,0,0,
						0,1,0,1,1,0,1,0,
						0,1,1,1,1,1,1,0,
						0,1,1,0,0,1,1,0,
						0,1,0,1,1,0,1,0,
						0,0,1,1,1,1,0,0,
						0,0,0,0,0,0,0,0 };


COLOR happyBitmap[64] = { 0 };
COLOR sadBitmap[64] = { 0 };

class FACE
{
public:
	int x, y;
	int vx, vy;
	COLOR* lpdds;
	void draw()
	{
		GRAPHIC::BlitMatrix(x, y, lpdds, faceWidth, faceHeight);
	}
};

void InitFaceBitmap(COLOR* bitmap, bool* bitmapSource)
{
	for (int i = 0; i < faceWidth; i++)
	{
		for (int j = 0; j < faceHeight; j++)
		{
			if (bitmapSource[j * faceWidth + i])
				bitmap[j * faceWidth + i] = RGB_DX(255, 255, 0);
		}
	}
}

const int faceNum = 100;
const int counterMax = 200;
FACE  face[faceNum];
void StartUp(void)
{
	InitFaceBitmap(happyBitmap, happy_bitmap);
	InitFaceBitmap(sadBitmap, sad_bitmap);
	for (int i = 0; i < faceNum; i++)
	{
		face[i].x = rand() % SCREEN_WIDTH;
		face[i].y = rand() % SCREEN_HEIGHT;
		face[i].vx = RAND_RANGE(-2, 2);
		face[i].vy = RAND_RANGE(-2, 2);
		if (rand() % 2)
		{
			face[i].lpdds = happyBitmap;
		}
		else
		{
			face[i].lpdds = sadBitmap;
		}
	}
}


void GameBody(void)
{
	using namespace GRAPHIC;
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	//帧率秒表
	static int counter = 0;
	static bool bFaceChanged = false;

	if (++counter >= counterMax)
	{
		counter = 0;
		bFaceChanged = !bFaceChanged;
	}
	BeginDrawOn();

	for (int i = 0; i < faceNum; i++)
	{
		face[i].draw();

		face[i].x += face[i].vx;
		face[i].y += face[i].vy;

		if (face[i].x >= SCREEN_WIDTH)
			face[i].x = -faceWidth;
		else if (face[i].x < -faceWidth)
			face[i].x = SCREEN_WIDTH;

		if (face[i].y >= SCREEN_HEIGHT)
			face[i].y = -faceHeight;
		else if (face[i].y < -faceHeight)
			face[i].y = SCREEN_HEIGHT;

		if (bFaceChanged)
			if (rand() % 2)
			{
				face[i].lpdds = happyBitmap;
			}
			else
			{
				face[i].lpdds = sadBitmap;
			}
	}


	EndDrawOn();

	fpsSet.adjust();
	GRAPHIC::Flush();  //在此之前必须关闭 GDI 要不然 无法切换
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("绘制色素图形"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


