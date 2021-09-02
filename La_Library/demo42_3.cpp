

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
#include "La_3D.h"
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


SURFACE textures1[NUM_TEXT],  // holds source texture library  1
textures2[NUM_TEXT],  // holds source texture library  2
temp_text;            // temporary working texture


void StartUp(void)
{
	TSTRING path = TEXT("F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo42\\");

	SURFACE temp;

	temp.createFromBitmap(path + TEXT("OMPTXT128_24.BMP"));

	// now extract each 64x64x16 texture from the image
	for (int itext = 0; itext < NUM_TEXT; itext++)
	{
		textures1[itext].createFromSurface(TEXTSIZE, TEXTSIZE, temp, (itext % 4) * (TEXTSIZE + 1), (itext / 4) * (TEXTSIZE + 1));
	}

	// create temporary working texture (load with first texture to set loaded flags)
	temp_text = textures1[0];


	temp.createFromBitmap(path + TEXT("SEXTXT128_24.BMP"));


	// now extract each 128x128x16 texture from the image
	for (int itext = 0; itext < NUM_TEXT; itext++)
	{
		textures2[itext].createFromSurface(TEXTSIZE, TEXTSIZE, temp, (itext % 4) * (TEXTSIZE + 1), (itext / 4) * (TEXTSIZE + 1));
	}

	keyboard.create();

	fpsSet.set(30);
}

void GameBody(void)
{
	if (KEY_DOWN(VK_ESCAPE))
		gameBox.exitFromGameBody();

	graphicOut.fillColor();

	keyboard.read();

	static int   curr_texture1 = 0;  // source texture 1
	static int   curr_texture2 = 5;  // source texture 2
	static float alphaf = .5;  // alpha blending factor

	///////////////////////////////////////////
	// our little image processing algorithm :)

	//Pixel_dest[x][y]rgb = alpha    * pixel_source1[x][y]rgb + 
	//                      (1-alpha)* pixel_source2[x][y]rgb
	BeginDrawOn(&temp_text);
	BeginDrawOn(&textures1[curr_texture1]);
	BeginDrawOn(&textures2[curr_texture2]);

	COLOR* s1buffer = textures1[curr_texture1].getMemory();
	const int s1Lpitch = textures1[curr_texture1].getLpitch();
	COLOR* s2buffer = textures2[curr_texture2].getMemory();
	const int s2Lpitch = textures2[curr_texture2].getLpitch();
	COLOR* tbuffer = temp_text.getMemory();
	const int tLpitch = temp_text.getLpitch();

	// perform RGB transformation on bitmap
	for (int iy = 0; iy < temp_text.getHeight(); iy++)
		for (int ix = 0; ix < temp_text.getWidth(); ix++)
		{
			int rs1, gs1, bs1;   // used to extract the source rgb values
			int rs2, gs2, bs2;
			int rf, gf, bf;   // the final rgb terms

			// extract pixel from source bitmap
			COLOR s1pixel = s1buffer[iy * s1Lpitch + ix];

			// extract RGB values
			DisRGB(s1pixel, rs1, gs1, bs1);

			// extract pixel from lightmap bitmap
			COLOR s2pixel = s2buffer[iy * s2Lpitch + ix];

			// extract RGB values
			DisRGB(s2pixel, rs2, gs2, bs2);

			// alpha blend them
			rf = (alphaf * (float)rs1) + ((1 - alphaf) * (float)rs2);
			gf = (alphaf * (float)gs1) + ((1 - alphaf) * (float)gs2);
			bf = (alphaf * (float)bs1) + ((1 - alphaf) * (float)bs2);

			// test for overflow
			if (rf > 255) rf = 255;
			if (gf > 255) gf = 255;
			if (bf > 255) bf = 255;

			// rebuild RGB and test for overflow
			// and write back to buffer
			tbuffer[iy * tLpitch + ix] = RGB_DX(rf, gf, bf);
		}

	////////////////////////////////////////

	EndDrawOn(&temp_text);
	EndDrawOn(&textures1[curr_texture1]);
	EndDrawOn(&textures2[curr_texture2]);

	temp_text.drawOn((WINDOW_WIDTH / 2) - (TEXTSIZE / 2), (WINDOW_HEIGHT / 2) + (TEXTSIZE / 2));
	textures1[curr_texture1].drawOn((WINDOW_WIDTH / 2) - 4 * (TEXTSIZE / 2), (WINDOW_HEIGHT / 2) - 2 * (TEXTSIZE / 2));
	textures2[curr_texture2].drawOn((WINDOW_WIDTH / 2) + 2 * (TEXTSIZE / 2), (WINDOW_HEIGHT / 2) - 2 * (TEXTSIZE / 2));


	// test if user wants to change texture
	if (keyboard[DIK_RIGHT])
	{
		if (++curr_texture1 > (NUM_TEXT - 1))
			curr_texture1 = (NUM_TEXT - 1);

		Sleep(100);
	} // end if

	if (keyboard[DIK_LEFT])
	{
		if (--curr_texture1 < 0)
			curr_texture1 = 0;

		Sleep(100);
	}


	// test if user wants to change ligthmap texture
	if (keyboard[DIK_UP])
	{
		if (++curr_texture2 > (NUM_TEXT - 1))
			curr_texture2 = (NUM_TEXT - 1);

		Sleep(100);
	}

	if (keyboard[DIK_DOWN])
	{
		if (--curr_texture2 < 0)
			curr_texture2 = 0;

		Sleep(100);
	}

	// is user changing scaling factor
	if (keyboard[DIK_PGUP])
	{
		alphaf += .01;
		if (alphaf > 1)
			alphaf = 1;
		Sleep(10);
	}

	if (keyboard[DIK_PGDN])
	{
		alphaf -= .01;
		if (alphaf < 0)
			alphaf = 0;

		Sleep(10);
	}


	// draw title
	gPrintf(10, 4, RGB(255, 255, 255), TEXT("Use <RIGHT>/<LEFT> arrows to change texture 1."));
	gPrintf(10, 20, RGB(255, 255, 255), TEXT("Use <UP>/<DOWN> arrows to change the texture 2."));
	gPrintf(10, 36, RGB(255, 255, 255), TEXT("Use <PAGE UP>/<PAGE DOWN> arrows to change blending factor alpha."));
	gPrintf(10, 56, RGB(255, 255, 255), TEXT("Press <ESC> to Exit. "));

	// print stats
	gPrintf(10, WINDOW_HEIGHT - 20, RGB(255, 255, 255), TEXT("Texture 1: %d, Texture 2: %d, Blending factor: %f"), curr_texture1, curr_texture2, alphaf);


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