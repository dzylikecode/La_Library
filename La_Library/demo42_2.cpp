

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

#define NUM_LMAP           4   // number of light map textures


SURFACE textures[NUM_TEXT],   // holds our texture library 
lightmaps[NUM_LMAP],  // holds our light map textures
temp_text;            // temporary working texture


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


	temp.createFromBitmap(path + TEXT("LIGHTMAPS128_24.BMP"));


	// now extract each 128x128x16 texture from the image
	for (int itext = 0; itext < NUM_LMAP; itext++)
	{
		// create the bitmap
		lightmaps[itext].createFromSurface(TEXTSIZE, TEXTSIZE, temp, (itext % 4) * (TEXTSIZE + 1), (itext / 4) * (TEXTSIZE + 1));
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

	static int   curr_texture = 7;   // currently active texture
	static int   curr_lightmap = 1;   // current light map
	static float scalef = .5; // texture ambient scale factor

	///////////////////////////////////////////
	// our little image processing algorithm :)

	// Pixel_dest[x,y]rgb = pixel_source[x,y]rgb * ambient + 
	//                      pixel_source[x,y]rgb * light_map[x,y]rgb
	BeginDrawOn(&temp_text);
	BeginDrawOn(&textures[curr_texture]);
	BeginDrawOn(&lightmaps[curr_lightmap]);

	COLOR* sbuffer = textures[curr_texture].getMemory();
	const int sLpitch = textures[curr_texture].getLpitch();
	COLOR* lbuffer = lightmaps[curr_lightmap].getMemory();
	const int lLpitch = lightmaps[curr_lightmap].getLpitch();
	COLOR* tbuffer = temp_text.getMemory();
	const int tLpitch = temp_text.getLpitch();

	// perform RGB transformation on bitmap
	for (int iy = 0; iy < temp_text.getHeight(); iy++)
		for (int ix = 0; ix < temp_text.getWidth(); ix++)
		{
			int rs, gs, bs;   // used to extract the source rgb values
			int rl, gl, bl; // light map rgb values
			int rf, gf, bf;   // the final rgb terms

			// extract pixel from source bitmap
			COLOR spixel = sbuffer[iy * sLpitch + ix];

			// extract RGB values
			DisRGB(spixel, rs, gs, bs);

			// extract pixel from lightmap bitmap
			COLOR lpixel = lbuffer[iy * lLpitch + ix];

			// extract RGB values
			DisRGB(lpixel, rl, gl, bl);

			// ambient base texture term + modulation term
			rf = (scalef * (float)rs) + ((float)rs * (float)rl / (float)64);
			gf = (scalef * (float)gs) + ((float)gs * (float)rl / (float)64);
			bf = (scalef * (float)bs) + ((float)bs * (float)rl / (float)64);

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
	EndDrawOn(&textures[curr_texture]);
	EndDrawOn(&lightmaps[curr_lightmap]);

	temp_text.drawOn((WINDOW_WIDTH / 2) - (TEXTSIZE / 2), (WINDOW_HEIGHT / 2) + (TEXTSIZE / 2));
	textures[curr_texture].drawOn((WINDOW_WIDTH / 2) - 4 * (TEXTSIZE / 2), (WINDOW_HEIGHT / 2) - 2 * (TEXTSIZE / 2));
	lightmaps[curr_lightmap].drawOn((WINDOW_WIDTH / 2) + 2 * (TEXTSIZE / 2), (WINDOW_HEIGHT / 2) - 2 * (TEXTSIZE / 2));


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


	// test if user wants to change ligthmap texture
	if (keyboard[DIK_UP])
	{
		if (++curr_lightmap > (NUM_LMAP - 1))
			curr_lightmap = (NUM_LMAP - 1);

		Sleep(100);
	}

	if (keyboard[DIK_DOWN])
	{
		if (--curr_lightmap < 0)
			curr_lightmap = 0;

		Sleep(100);
	}

	// is user changing scaling factor
	if (keyboard[DIK_PGUP])
	{
		scalef += .01;
		if (scalef > 10)
			scalef = 10;
		Sleep(10);
	}

	if (keyboard[DIK_PGDN])
	{
		scalef -= .01;
		if (scalef < 0)
			scalef = 0;

		Sleep(10);
	}


	// draw title
	gPrintf(10, 4, RGB(255, 255, 255), TEXT("Use <RIGHT>/<LEFT> arrows to change texture."));
	gPrintf(10, 20, RGB(255, 255, 255), TEXT("Use <UP>/<DOWN> arrows to change the light map texture."));
	gPrintf(10, 36, RGB(255, 255, 255), TEXT("Use <PAGE UP>/<PAGE DOWN> arrows to change ambient scale factor."));
	gPrintf(10, 56, RGB(255, 255, 255), TEXT("Press <ESC> to Exit. "));

	// print stats
	gPrintf(10, WINDOW_HEIGHT - 20, RGB(255, 255, 255), TEXT("Texture #%d, Ligtmap #%d, Ambient Scaling Factor: %f"), curr_texture, curr_lightmap, scalef);


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