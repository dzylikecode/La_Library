#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include "La_Windows.h"
#include "La_WinGDI.h"
#include "La_GameBox.h"

using namespace std;

void GameBody(void)
{
	GDI::winOut(30, 0) << TEXT("Hello, World") << GDI::end;
	GDI::DrawCircle(RGB(0, 255, 255), 90, 30, 30);
}

int main(int argc, char* argv[])
{
	GAMEBox gameBox;
	gameBox.create(300, 200, TEXT("Äêºó"));

	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


