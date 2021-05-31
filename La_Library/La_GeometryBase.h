#pragma once
#include "La_Surface.h"

namespace GRAPHIC
{
	inline COLOR SetPixel(int x, int y, COLOR color, SURFACE* surface = nullptr)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		return (*surface)(x, y, color);
	}



}