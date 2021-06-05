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

const int mapWidth = 64;
const int mapHeight = 64;

//视野中 map 块的数量
const int viewMapH = SCREEN_WIDTH / mapWidth;
const int viewMapV = SCREEN_HEIGHT / mapHeight;

const int viewWidth = viewMapH * mapWidth;
const int viewHeight = viewMapV * mapHeight;

//世界为视野的 3 * 3倍
const int viewH = 3;
const int viewV = 3;

const int maxX = viewWidth * (viewH - 1);
const int maxY = viewHeight * (viewV - 1);

const int moveStep = 4;

const int textureNum = 10;

SURFACE texture[textureNum];

const char* worldMap[viewV * viewMapV] =
{
"111111111111111111111111111111",
"100000000000000000000000000001",
"100002222220000000000000077701",
"100002222223333333333000077701",
"100002222227777777773000070001",
"100002222227777777773000070001",
"100000000377777777773000070001",
"107777700377777777773000070001",
"177777770377777777773000770001",
"107777700377777777773007700001",
"100777770377777777773777000001",
"100000707377777777773000000001",
"100007777377777777773000000001",
"100000000302222777773000000001",
"100000000332222777773000000001",
"100000000002222333333000000001",
"100000666666666666666666600001",
"100000800000000000000000800001",
"100000800000000000000000800001",
"100000000000000000000000000001",
"111111111111111111111111111111",
};

int worldX = 0, worldY = 0;

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo7\\");

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("SCROLLTEXTURES_24.BMP"));
	for (int i = 0; i < textureNum; i++)
	{
		texture[i].createFromSurface(mapWidth, mapHeight, temp, (i % 4) * (mapWidth + 1) + 1, (i / 4) * (mapHeight + 1) + 1);
	}

	ResetClipper(0, 0, viewWidth, viewHeight);


	fpsSet.set(30);
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

	//映射对应的 map 区域
	int mapLeft = worldX / mapWidth;
	int mapTop = worldY / mapHeight;
	int mapRight = mapLeft + viewMapH - 1; //包括了自己
	int mapBottom = mapTop + viewMapV - 1;

	//考虑超出情况
	int offsetX = -(worldX % 64);
	int offsetY = -(worldY % 64);
	//有偏移，则下一个也要露出来
	if (offsetX)
		mapRight++;
	if (offsetY)
		mapBottom++;

	int textureX = offsetX;
	int textureY = offsetY;

	for (int indexY = mapTop; indexY <= mapBottom; indexY++)
	{
		for (int indexX = mapLeft; indexX <= mapRight; indexX++)
		{
			int index = worldMap[indexY][indexX] - '0';
			texture[index].drawOn(textureX, textureY, false);
			textureX += mapWidth;
		}
		textureX = offsetX;
		textureY += mapHeight;
	}

	gPrintf(8, 8, RGB(255, 255, 255), TEXT("USE ARROW KEYS TO MOVE RIGHT & LEFT, <ESC> to Exit."));
	gPrintf(8, SCREEN_HEIGHT - 32 - 24, RGB_DX(0, 255, 0), TEXT("World Position = [%d, %d]     "), worldX, worldY);
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


