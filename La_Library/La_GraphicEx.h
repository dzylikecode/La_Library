#pragma once
#include "La_GraphicBase.h"
#define randColor  RGB_DX(rand()%256,rand()%256,rand()%256,)
#define RED		   RGB_DX(255,0,0)
namespace GRAPHIC
{
	void BlitMatrix(int x, int y, COLOR* bitmapMatrix, int iwidth, int iheight, COLOR colorKey = 0);
}