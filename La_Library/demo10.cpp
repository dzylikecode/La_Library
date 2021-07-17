
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


SURFACE pointer;
SURFACE panel;
SURFACE canvas;
const int canvasWidth = 500;
const int canvasHeight = SCREEN_HEIGHT;
COLOR palette[256];

const int canvasGap = 3;
const int pointNum = 10;

enum STYLE
{
	SPRAY = 0,
	PENCIL = 1,
	ERASE = 2,
	EXIT = 3,
};

const int paletteWidth = 9;
const int paletteHeight = 9;

//8 * 32 = 256
const int palWidthNum = 8;
const int palHeightNum = 32;

const int paletteGapWidth = 16;
const int paletteGapHeight = 8;

//
const int paletteStartX = canvasWidth + paletteGapWidth;
const int paletteStartY = paletteGapHeight;
const int paletteEndX = paletteStartX + palWidthNum * paletteWidth;
const int paletteEndY = paletteStartY + palHeightNum * paletteHeight;

const int pointerWidth = 32;
const int pointerHeight = 34;

const int buttonNum = 4;
const int buttonWidth = 32;
const int buttonHeight = 34;

//button 的范围
const int buttonLeft = 500;
const int buttonRight = 500 + 100;
const int buttonTop = 344;
const int buttonBottom = 383 + buttonHeight;


const int curColorLeft = 533;
const int curColorTop = 306;
const int curColorRight = 533 + 34;
const int curColorBottom = 306 + 34;

class BUTTON
{
public:
	BUTTON() :onIcon(icon[1]), offIcon(icon[0]) {};
	bool state;
	int x, y;
	SURFACE icon[2];
	SURFACE& onIcon;
	SURFACE& offIcon;
	void show()
	{
		if (state)
			onIcon.drawOn(x, y);
		else
			offIcon.drawOn(x, y);
	}
}button[buttonNum];

using namespace INPUT_;
MOUSE mouse;

void StartUp(void)
{
	//输入的初始化
	mouse.create();

	TSTRING path = TEXT(".\\Resource\\demo10\\");
	SURFACE temp;
	temp.createFromBitmap(path + TEXT("PAINT.BMP"));

	pointer.createFromSurface(buttonWidth, buttonHeight, temp, (0) * (buttonWidth + 1) + 1, (2) * (buttonHeight + 1) + 1);

	for (int i = 0; i < 2 * buttonNum; i++)
	{
		button[i % buttonNum].icon[i / buttonNum].createFromSurface(32, 34, temp, (i % buttonNum) * (32 + 1) + 1, (i / buttonNum) * (34 + 1) + 1);
	}

	button[0].state = false;
	button[1].state = true;
	button[2].state = false;
	button[3].state = false;

	button[0].x = 509;
	button[0].y = 344;

	button[1].x = 559;
	button[1].y = 344;

	button[2].x = 509;
	button[2].y = 383;

	button[3].x = 559;
	button[3].y = 383;

	panel.createFromSurface(104, 424, temp, 150, 0);

	canvas.create(canvasWidth, canvasHeight, 0, false);

	for (int i = 0; i < 256; i++)
	{
		palette[i] = randColor;
	}
	ShowCursor(false);
	fpsSet.set(120);
}


void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	mouse.read();

	static int mouseX = 0;
	static int mouseY = 0;

	mouseX += mouse.getX();
	mouseY += mouse.getY();

	mouseX = max(0, min(SCREEN_WIDTH - 1, mouseX));
	mouseY = max(0, min(SCREEN_HEIGHT - 1, mouseY));

	static COLOR pointerCurColor = RGB_DX(0, 255, 0);

	BeginDrawOn(&canvas);
	//在画布范围内的时候
	if (mouseX > canvasGap && mouseX < (canvasWidth - canvasGap) && mouseY > canvasGap && mouseY < (canvasHeight - canvasGap))
	{
		if (mouse.getLButton())
		{
			if (button[PENCIL].state)//2*2作为点
			{
				SetPixel(mouseX, mouseY, pointerCurColor, &canvas);
				SetPixel(mouseX + 1, mouseY, pointerCurColor, &canvas);
				SetPixel(mouseX, mouseY + 1, pointerCurColor, &canvas);
				SetPixel(mouseX + 1, mouseY + 1, pointerCurColor, &canvas);
			}
			else if (button[SPRAY].state)
			{
				for (int index = 0; index < pointNum; index++)
				{
					int sx = mouseX + RAND_RANGE(-8, 9);
					int sy = mouseY + RAND_RANGE(-8, 9);

					if (sx > 0 && sx < canvasWidth && sy > 0 && sy < canvasHeight)
					{
						SetPixel(sx, sy, pointerCurColor, &canvas);
					}
				}
			}
		}
		else if (mouse.getRButton()) //表示清除
		{
			if (button[PENCIL].state)
			{
				SetPixel(mouseX, mouseY, 0, &canvas);
				SetPixel(mouseX + 1, mouseY, 0, &canvas);
				SetPixel(mouseX, mouseY + 1, 0, &canvas);
				SetPixel(mouseX + 1, mouseY + 1, 0, &canvas);
			}
			else if (button[SPRAY].state)
			{
				for (int index = 0; index < pointNum; index++)
				{
					int sx = mouseX + RAND_RANGE(-8, 9);
					int sy = mouseY + RAND_RANGE(-8, 9);

					if (sx > 0 && sx < canvasWidth && sy>0 && sy < canvasHeight)
					{
						SetPixel(sx, sy, 0, &canvas);
					}
				}
			}
		}
	}
	else if ((mouseX > paletteStartX) && (mouseX < paletteEndX) && mouseY > paletteStartY && mouseY < paletteEndY)
	{
		if (mouse.getLButton())
		{
			int cellX = (mouseX - (paletteStartX)) / paletteWidth;
			int cellY = (mouseY - (paletteStartY)) / paletteHeight;

			pointerCurColor = palette[cellY * palWidthNum + cellX];
		}
	}
	else if (mouseX > buttonLeft && mouseX < buttonRight && mouseY > buttonTop && mouseY < buttonBottom)
	{
		int  cur = 0;
		while (cur < buttonNum)
		{
			if (mouseX > button[cur].x && (mouseX < button[cur].x + buttonWidth) &&
				mouseY > button[cur].y && (mouseY < button[cur].y + buttonHeight))
			{
				break;
			}
			cur++;
		}


		switch (cur)
		{
		case SPRAY:
			if (mouse.getLButton())
			{
				button[SPRAY].state = true;
				//关闭其他的按钮 异或嘛
				button[PENCIL].state = false;
			}
			break;
		case PENCIL:
			if (mouse.getLButton())
			{
				button[PENCIL].state = true;
				button[SPRAY].state = false;
			}
			break;
		case ERASE:
			if (mouse.getLButton())
			{
				button[ERASE].state = true;
			}
			else
			{
				button[ERASE].state = false;
			}
			break;
		case EXIT:
			if (mouse.getLButton())
			{
				gameBox.exitFromGameBody();
			}
			break;
		}
	}
	EndDrawOn(&canvas);

	if (button[ERASE].state)
	{
		canvas.fillColor();
	}
	canvas.drawOn(0, 0, false);
	panel.drawOn(canvasWidth, 0, false);
	for (int i = 0; i < 256; i++)
	{
		int left = canvasWidth + paletteGapWidth + (i % 8) * paletteWidth;
		int top = 8 + (i / 8) * paletteHeight;
		DrawRectangle(left, top, left + 8, top + 8, palette[i]);
	}
	DrawRectangle(curColorLeft, curColorTop, curColorRight, curColorBottom, pointerCurColor);

	for (int index = 0; index < buttonNum; index++)
	{
		button[index].show();
	}

	gPrintf(0, 0, RGB(0, 255, 0), TEXT("Paint Version 2.0 - Press <ESC> to Exit."));
	gPrintf(8, SCREEN_HEIGHT - 16, RGB(0, 255, 0), TEXT("Pointer (%d,%d)"), mouseX, mouseY);

	pointer.drawOn(mouseX - pointerWidth / 2, mouseY - pointerHeight / 2);

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


