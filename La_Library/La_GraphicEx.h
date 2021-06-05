#pragma once
#include "La_GraphicBase.h"
#include "La_Sprite.h"
#define randColor	RGB_DX(rand()%256,rand()%256,rand()%256,)
#define RED_GDI		RGB(255,0,0)
#define GREEN_GDI	RGB(0,255,0)
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
}