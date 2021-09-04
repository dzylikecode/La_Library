/*
***************************************************************************************
*  程    序:
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/09/03 17:54:21
*  最后修改:
*
*  简    介: removebackface 选项有点问题,应该是 disable 的时候，过多导致数量不够，越界
*
***************************************************************************************
*/


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
#include "La_3D.h"
#include "La_Master.h"
#include "resource.h"

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

FRAMECounter fpsSet;
GAMEBox gameBox;

using namespace AUDIO;
using namespace GDI;
using namespace GRAPHIC;
using namespace INPUT_;

KEYBOARD keyboard;

#define WINDOW_WIDTH SCREEN_WIDTH
#define WINDOW_HEIGHT SCREEN_HEIGHT




void StartUp(void)
{
	keyboard.create();

	fpsSet.set(30);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	keyboard.read();

	// draw a randomly positioned gouraud triangle with 3 random vertex colors
	POLYF4DV2 face;

	// set the vertices
	face.tvlist[0].x = (int)RAND_RANGE(0, WINDOW_WIDTH - 1);
	face.tvlist[0].y = (int)RAND_RANGE(0, WINDOW_HEIGHT - 1);
	face.lit_color[0] = RGB_DX(RAND_RANGE(0, 255), RAND_RANGE(0, 255), RAND_RANGE(0, 255));

	face.tvlist[1].x = (int)RAND_RANGE(0, WINDOW_WIDTH - 1);
	face.tvlist[1].y = (int)RAND_RANGE(0, WINDOW_HEIGHT - 1);
	face.lit_color[1] = RGB_DX(RAND_RANGE(0, 255), RAND_RANGE(0, 255), RAND_RANGE(0, 255));

	face.tvlist[2].x = (int)(int)RAND_RANGE(0, WINDOW_WIDTH - 1);
	face.tvlist[2].y = (int)(int)RAND_RANGE(0, WINDOW_HEIGHT - 1);
	face.lit_color[2] = RGB_DX(RAND_RANGE(0, 255), RAND_RANGE(0, 255), RAND_RANGE(0, 255));

	BeginDrawOn();
	// draw the gouraud shaded triangle
	DrawGouraudTriangle(face);

	// unlock the back buffer
	EndDrawOn();

	// draw instructions
	gPrintf(0, 0, RGB(0, 255, 0), TEXT("Press ESC to exit."));

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
	/*gameBox.setWndProc(WindowProc);
	gameBox.setClassStyle(CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW);*/
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("键盘控制移动"));
	gameBox.attachMenu(IDR_ASCMENU);
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


