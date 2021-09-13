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

#define Start_Clock()  fpsSet.start()

#define DDraw_Fill_Surface() GRAPHIC::graphicOut.fillColor()

#define Draw_Text_GDI(str, x, y, color, z)  GRAPHIC::gPrintf(x, y, color, AToT(str))


#define DDraw_Lock_Back_Surface GRAPHIC::BeginDrawOn

#define DDraw_Unlock_Back_Surface GRAPHIC::EndDrawOn

#define DDraw_Flip GRAPHIC::Flush


#define Wait_Clock()  fpsSet.adjust()

#define DDraw_Init()  

#define DInput_Init()

#define DSound_Init()

#define DMusic_Init()

#define Open_Error_File()

#define Build_Sin_Cos_Tables()

