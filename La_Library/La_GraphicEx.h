#pragma once
#include "La_GraphicBase.h"
#include "La_Sprite.h"
#include "La_LinearList.h"
#define randColor	RGB_DX(rand()%256,rand()%256,rand()%256,)
#define RED_GDI		RGB(255,0,0)
#define GREEN_GDI	RGB(0,255,0)

#define SCREEN_DARKNESS  0         // fade to black
#define SCREEN_WHITENESS 1         // fade to white
#define SCREEN_SWIPE_X   2         // do a horizontal swipe
#define SCREEN_SWIPE_Y   3         // do a vertical swipe
#define SCREEN_DISOLVE   4         // a pixel dissolve
#define SCREEN_SCRUNCH   5         // a square compression
#define SCREEN_BLUENESS  6         // fade to blue
#define SCREEN_REDNESS   7         // fade to red
#define SCREEN_GREENNESS 8         // fade to green

namespace GRAPHIC
{
	void BlitMatrix(int x, int y, COLOR* bitmapMatrix, int iwidth, int iheight, COLOR colorKey = 0);

	class individeSPRITE :public SPRITE, public aniDICT
	{
	public:
		individeSPRITE()
		{
			content.setDict(this);
		}
	};

	void ScreenTransitions(int effect, int gap = 12);
}