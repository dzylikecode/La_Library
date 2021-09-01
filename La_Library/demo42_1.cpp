

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
#include "La_Master.h"

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

#define WINDOW_WIDTH SCREEN_WIDTH
#define WINDOW_HEIGHT SCREEN_HEIGHT


#define TEXTSIZE           128 // size of texture mxm
#define NUM_TEXT           12  // number of textures


SURFACE textures[12], // holds our texture library 
temp_text;    // temporary working texture

const int textX = (WINDOW_WIDTH / 2) - (TEXTSIZE / 2);
const int textY = (WINDOW_HEIGHT / 2) - (TEXTSIZE / 2);


void StartUp(void)
{
	TSTRING path = TEXT("F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo42\\");

	SURFACE temp;

	temp.createFromBitmap(path + TEXT("OMPTXT128_24.BMP"));

	// now extract each 64x64x16 texture from the image
	for (int itext = 0; itext < NUM_TEXT; itext++)
	{
		textures[itext].createFromSurface(TEXTSIZE, TEXTSIZE, temp, (itext % 4) * (TEXTSIZE + 1), (itext / 4) * (TEXTSIZE + 1));
	}

	// create temporary working texture (load with first texture to set loaded flags)
	temp_text = textures[0];


	keyboard.create();

	fpsSet.set(30);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	keyboard.read();

	static int   curr_texture = 0;   // currently active texture
	static float scalef = 1.0; // texture scaling factor

	temp_text = textures[curr_texture];

	///////////////////////////////////////////
// our little image processing algorithm :)
//Cmodulated = s*C1 = (s*r1, s*g1, s*b1)
	BeginDrawOn(&temp_text);

	COLOR* pBuffer = temp_text.getMemory();
	const int lpitch = temp_text.getLpitch();

	// perform RGB transformation on bitmap
	for (int iy = 0; iy < temp_text.getHeight(); iy++)
		for (int ix = 0; ix < temp_text.getWidth(); ix++)
		{
			// extract pixel
			COLOR pixel = pBuffer[iy * lpitch + ix];

			int ri = GetRValue_DX(pixel);
			int gi = GetGValue_DX(pixel);
			int bi = GetBValue_DX(pixel);

			// perform scaling operation and test for overflow
			if ((ri = (float)ri * scalef) > 255) ri = 255;
			if ((gi = (float)gi * scalef) > 255) gi = 255;
			if ((bi = (float)bi * scalef) > 255) bi = 255;

			// rebuild RGB and test for overflow
			// and write back to buffer
			pBuffer[iy * lpitch + ix] = RGB_DX(ri, gi, bi);

		}

	EndDrawOn(&temp_text);

	temp_text.drawOn(textX, textY);

	// test if user wants to change texture
	if (keyboard[DIK_RIGHT])
	{
		if (++curr_texture > (NUM_TEXT - 1))
			curr_texture = (NUM_TEXT - 1);

		Sleep(100);
	}

	if (keyboard[DIK_LEFT])
	{
		if (--curr_texture < 0)
			curr_texture = 0;

		Sleep(100);
	}

	// is user changing scaling factor
	if (keyboard[DIK_UP])
	{
		scalef += .01;
		if (scalef > 10)
			scalef = 10;
		Sleep(10);
	}

	if (keyboard[DIK_DOWN])
	{
		scalef -= .01;
		if (scalef < 0)
			scalef = 0;

		Sleep(10);
	}


	// draw title
	gPrintf(10, 10, RGB(255, 255, 255), TEXT("Use <RIGHT>/<LEFT> arrows to change texture, <UP>/<DOWN> arrows to change scale factor."));
	gPrintf(10, 32, RGB(255, 255, 255), TEXT("Press <ESC> to Exit. "));
	// print stats
	gPrintf(10, WINDOW_HEIGHT - 20, RGB(255, 255, 255), TEXT("Texture #%d, Scaling Factor: %f"), curr_texture, scalef);

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