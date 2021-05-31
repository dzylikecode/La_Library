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

GRAPHIC::SURFACE backGround;
void StartUp(void)
{
	backGround.createFromBitmap(TEXT("landscape1_24.bmp"));
	//backGround.scroll(0, 30);
}

GAMEBox gameBox;

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();
	/*GRAPHIC::BeginDrawOn();
	for (int i = 0; i < 1000; i++)
		GRAPHIC::graphicOut(rand() % 640, rand() % 480, randColor);
	GRAPHIC::EndDrawOn();*/
	backGround.drawOn(0, 0);
	GRAPHIC::Flush();
}

int main(int argc, char* argv[])
{
	gameBox.setWndMode(false, true);
	gameBox.create(640, 480, TEXT("Äêºó"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


