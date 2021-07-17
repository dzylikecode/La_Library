
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

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


FRAMECounter fpsSet;
GAMEBox gameBox;

using namespace GRAPHIC;

const int mushroomNum = 4;
SURFACE mushroom[mushroomNum];
individeSPRITE blaster;
SURFACE background;


using namespace INPUT_;
JOYSTICK joystick;


class MISSILE
{
public:
	int x, y;
	bool state;
	MISSILE() :state(false) {};
	bool start()
	{
		if (!state)
		{
			state = true;
			x = blaster.x + 16;
			y = blaster.y - 4;

			return true;
		}

		return false;
	}

	bool move()
	{
		if (state)
		{
			if ((y -= 10) < 0)
			{
				state = false;
			}
			return true;
		}
		return false;
	}
	bool draw()
	{
		if (state)
		{
			BeginDrawOn();
			DrawLine(x, y, x, y + 6, RGB(0, 255, 0));
			EndDrawOn();
			return true;
		}
		return false;
	}
}missile;

void StartUp(void)
{
	//输入的初始化
	joystick.create();

	TSTRING path = TEXT(".\\Resource\\demo11\\");
	SURFACE temp;	
	temp.createFromBitmap(path + TEXT("MUSH.BMP"));


	for (int i = 0; i < mushroomNum; i++)
	{
		mushroom[i].createFromSurface(32, 32, temp, (i) * (32 + 1) + 1, (0) * (32 + 1) + 1);
	}
	blaster.resize(3);
	for (int i = 0; i < 3; i++)
	{
		blaster[i].createFromSurface(32, 32, temp, (i) * (32 + 1) + 1, (1) * (32 + 1) + 1);
	}
	blaster.content.resize(1);
	blaster.content[0] = ANIM(0, 1, 2, 1, 0);
	blaster.setAniSpeed(3);
	blaster.x = 320;
	blaster.y = 400;
	blaster.vx = blaster.vy = 0;

	blaster.reset(SPRITE::LOOP);

	background.createFromBitmap(path + TEXT("GRASS.BMP"));
	
	// create the random mushroom patch
	for (int index = 0; index < 50; index++)
	{
		// select a mushroom
		int mush = rand() % 4;

		// set mushroom to random position
		int x = (rand() % (SCREEN_WIDTH - 32) / 32) * 32;
		int y = (rand() % (SCREEN_HEIGHT - 128) / 32) * 32;

		mushroom[mush].drawOn(background, x, y);
	}

	ShowCursor(false);
	fpsSet.set(120);
}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	background.drawOn(0, 0, false);

	joystick.read();

	blaster.x += joystick.joy_state.lX;
	blaster.y += joystick.joy_state.lY;

	blaster.x = max(0, min(SCREEN_WIDTH - 32, blaster.x));
	blaster.y = max(SCREEN_HEIGHT - 128, min(SCREEN_HEIGHT - 32, blaster.y));

	if (joystick.joy_state.rgbButtons[0])
		missile.start();

	missile.move();
	missile.draw();


	//辣椒准备眨眼睛
	if ((rand() % 100) == 50)
		blaster.setAniString(0);

	blaster.animate();
	blaster.drawOn();


	gPrintf(0, 0, RGB(255, 255, 255), TEXT("Make My Centipede!"));

	gPrintf(0, SCREEN_HEIGHT - 40, RGB(255, 255, 50), TEXT("Joystick Name & Vendor: %s"), joystick.getName());

	gPrintf(0, SCREEN_HEIGHT - 20, RGB(255, 255, 50), TEXT("Joystick Stats: X-Axis=%d, Y-Axis=%d, buttons(%d,%d,%d,%d,%d,%d,%d,%d)"),
		joystick.joy_state.lX, joystick.joy_state.lY,
		joystick.joy_state.rgbButtons[0],
		joystick.joy_state.rgbButtons[1],
		joystick.joy_state.rgbButtons[2],
		joystick.joy_state.rgbButtons[3],
		joystick.joy_state.rgbButtons[4],
		joystick.joy_state.rgbButtons[5],
		joystick.joy_state.rgbButtons[6],
		joystick.joy_state.rgbButtons[7]);
	gPrintf(0, 20, RGB(255, 255, 50), TEXT("Joystick Stats: Z-Axis=%d, RX-Axis=%d, RY-Axis=%d, RZ-Axis=%d, S0=%d, S1=%d"),
		joystick.joy_state.lZ,
		//控制方向的键，xbox 是右边的游戏杆
		joystick.joy_state.lRx, joystick.joy_state.lRy, joystick.joy_state.lRz,
		joystick.joy_state.rglSlider[0], joystick.joy_state.rglSlider[2]);

	fpsSet.adjust();
	gPrintf(0, 0, RED_GDI, TEXT("%d"), fpsSet.get());
	Flush();  //在此之前必须关闭 GDI 要不然 无法切换
}

int main(int argc, char* argv[])
{
	//gameBox.setWndMode(false, true);
	gameBox.create(SCREEN_WIDTH, SCREEN_HEIGHT, TEXT("键盘控制移动"));
	gameBox.setGameStart(StartUp);
	gameBox.setGameBody(GameBody);
	gameBox.startCommuication();
	return argc;
}


