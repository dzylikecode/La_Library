#pragma once
#include "La_Sprite.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>


namespace GRAPHIC
{
	
	class IMAGE_GIF
	{
	public:
		aniDICT frame;
		bool load(LPCTSTR fileName, ...);
	};
}