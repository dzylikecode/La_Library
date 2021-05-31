#pragma once
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#ifndef INITGUID
#define INITGUID  //使用一些预定义好的 guid
#endif
#include "La_Windows.h"
#include <ddraw.h>

#define INIT_STRUCT(st)		{memset(&st, 0, sizeof(st)); st.dwSize = sizeof(st);}

typedef  UINT  COLOR;

#define RGB_DX(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#define GetBValue_DX(rgb)      (LOBYTE(rgb))
#define GetGValue_DX(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetRValue_DX(rgb)      (LOBYTE((rgb)>>16))

namespace GRAPHIC
{
	bool Flip(BYTE* image, int bytes_per_line, int height);

	

	LPDIRECTDRAWSURFACE7 CreateSurface(int width, int height, DWORD colorKey, DWORD surfaceMemoryStyle);
	inline void FillSurfaceColor(LPDIRECTDRAWSURFACE7 lpddsurface, DWORD color, RECT* client)
	{
		DDBLTFX ddbltfx;//用来控制blt的操作信息

		INIT_STRUCT(ddbltfx);

		//设置填充的颜色
		ddbltfx.dwFillColor = color;

		//利用blit来填充
		lpddsurface->Blt(client, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	}
	LPDIRECTDRAWCLIPPER SurfaceAttachClipper(LPDIRECTDRAWSURFACE7 lpddsurface, int num_rects, LPRECT clip_list);



}