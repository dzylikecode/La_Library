
#define _CRT_SECURE_NO_WARNINGS
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

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

FRAMECounter fpsSet;
GAMEBox gameBox;

using namespace AUDIO;
using namespace GDI;
using namespace GRAPHIC;
using namespace INPUT_;

KEYBOARD keyboard;

SURFACE background;
SOUND backSound;

individeSPRITE ghost;

individeSPRITE bat;
SOUND batSound;

void StartUp(void)
{
	TSTRING path = TEXT(".\\Resource\\demo17\\");
	background.createFromBitmap(path + TEXT("GHOSTBACK.BMP"));

	SURFACE temp;
	temp.createFromBitmap(path + TEXT("BATS8_2.BMP"));
	bat.resize(5);
	for (int i = 0; i < 5; i++)
	{
		bat[i].createFromSurface(16, 16, temp, i * (16 + 1) + 1, 0 * (16 + 1) + 1);
	}

	bat.x = 320;
	bat.y = 200;
	bat.setAniSpeed(2);
	bat.content.resize(1);
	bat.content[0] = ANIM(0, 1, 2, 3);

	batSound.create(path + TEXT("BAT.WAV"));
	batSound.play(true);

	temp.createFromBitmap(path + TEXT("GHOSTS8.BMP"));
	ghost.resize(3);
	for (int i = 0; i < 3; i++)
	{
		ghost[i].createFromSurface(64, 100, temp, i * (64 + 1) + 1, 0 * (100 + 1) + 1);
	}

	ghost.x = 100;
	ghost.y = 200;
	ghost.setAniSpeed(10);
	ghost.content.resize(1);
	ghost.content[0] = ANIM(0, 1, 2, 1);


	backSound.create(path + TEXT("WIND.WAV"));
	backSound.play(true);

	keyboard.create();

	fpsSet.set(60);
}

void Bat_AI()
{
	if (ghost.x > bat.x)
		bat.x += 2;
	else if (ghost.x < bat.x)
		bat.x -= 2;

	// now y-axis
	if (ghost.y > bat.y)
		bat.y += 2;
	else if (ghost.y < bat.y)
		bat.y -= 2;

	// check boundaries
	if (bat.x >= SCREEN_WIDTH)
		bat.x = -bat.getWidth();
	else if (bat.x < -bat.getWidth())
		bat.x = SCREEN_WIDTH;

	if (bat.y >= SCREEN_HEIGHT)
		bat.y = -bat.getHeight();
	else if (bat.y < -bat.getHeight())
		bat.y = SCREEN_HEIGHT;
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	keyboard.read();

	Bat_AI();

	bat.animate();
	bat.drawOn();

	// allow player to move
	if (keyboard.keyboard_state[DIK_RIGHT])
		ghost.x += 4;
	else if (keyboard.keyboard_state[DIK_LEFT])
		ghost.x -= 4;

	if (keyboard.keyboard_state[DIK_UP])
		ghost.y -= 4;
	else if (keyboard.keyboard_state[DIK_DOWN])
		ghost.y += 4;

	if (ghost.x >= SCREEN_WIDTH)
		ghost.x = -ghost.getWidth();
	else if (ghost.x < -ghost.getWidth())
		ghost.x = SCREEN_WIDTH;

	if (ghost.y >= SCREEN_HEIGHT)
		ghost.y = -ghost.getHeight();
	else if (ghost.y < -ghost.getHeight())
		ghost.y = SCREEN_HEIGHT;

	ghost.animate();
	ghost.drawOn();


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

	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("¼üÅÌ¿ØÖÆÒÆ¶¯"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.setGameEnd(GameClose);
	gameBox.startCommuication();
	return argc;
}


