#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include "La_Windows.h"
#include "La_WinGDI.h"
#include "La_GameBox.h"
#include "La_Graphic.h"

using namespace std;

#define randColor	RGB_DX(rand()%256, rand()%256, rand()%256)
#define RED			RGB_DX(255, 0, 0)

GRAPHIC::SURFACE backGround;
FRAMECounter fpsSet;
void StartUp(void)
{

}

GAMEBox gameBox;

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	GRAPHIC::BeginDrawOn();
	GRAPHIC::DrawLine(-10, -10, 240, 240, RED);
	GRAPHIC::EndDrawOn();
	fpsSet.adjust();
	GRAPHIC::Flush();  //�ڴ�֮ǰ����ر� GDI Ҫ��Ȼ �޷��л�
}

int main(int argc, char* argv[])
{
	GRAPHIC::aniSTRING aniString = GRAPHIC::InitAniString(0, 1);
	//gameBox.setWndMode(false, true);
	gameBox.create(640, 480, TEXT("���"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


