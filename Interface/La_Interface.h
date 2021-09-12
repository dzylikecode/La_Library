#pragma once
#include <stdio.h>
#include "La_Windows.h"
#include "La_WinGDI.h"
#include "La_GameBox.h"
#include "La_Graphic.h"
#include "La_GraphicEx.h"
#include "La_Input.h"
#include "La_Audio.h"

#define Write_Error   printf

#define MIN   min
#define MAX	  max

inline void Draw_Clip_Line16(int x0, int y0, int x1, int y1, int color, UCHAR* dest_buffer, int lpitch)
{
	using namespace GRAPHIC;
	DrawLine(x0, y0, x1, y1, color, (SURFACE*)dest_buffer);
}

extern int debug_polys_rendered_per_frame;