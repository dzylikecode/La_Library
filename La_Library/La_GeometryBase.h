#pragma once
#include "La_Surface.h"
#include "La_Math.h"

namespace GRAPHIC
{

	inline COLOR SetPixel(int x, int y, COLOR color, SURFACE* surface = nullptr)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		return (*surface)(x, y, color);
	}

	inline void HLine(int x1, int x2, int y, COLORREF color, SURFACE* surface = nullptr)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		if (y > max_clip_x || y < min_clip_x)
		{
			return;
		}

		//统一从左到右
		if (x1 > x2)
		{
			int temp = x1;
			x1 = x2;
			x2 = x1;
		}

		if (x1 > max_clip_y || x2 < min_clip_y)
		{
			return;
		}

		//clip
		x1 = max(x1, min_clip_x);
		x2 = min(x2, max_clip_x);

		//draw the row of pixels
		memset((surface->getMemory() + y * surface->getLpitch() + x1), color, (x2 - x1 + 1) * sizeof(COLOR));
	}

	void VLine(int y1, int y2, int x, COLORREF color, SURFACE* surface = nullptr);

	void DrawLine(int x0, int y0, int x1, int y1, COLORREF color, SURFACE* surface = nullptr);

	inline void DrawRectangle(int x1, int y1, int x2, int y2, COLORREF color, SURFACE* surface = nullptr)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		//采取填充实心的办法
		//和清屏相同	
		RECT fill_area;

		fill_area.top = y1;
		fill_area.left = x1;
		fill_area.bottom = y2;
		fill_area.right = x2;

		DDBLTFX ddbltfx;
		INIT_STRUCT(ddbltfx);

		ddbltfx.dwFillColor = color;

		//注意这不是全局变量的lpdds
		surface->getlpdds()->Blt(&fill_area, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	}


	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface = nullptr);
	void FillTriangleFP(int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface = nullptr);
	/// <summary>
	/// 填充四边形,顺序是0 1 2 3，旋转
	///			0		1
	///			3		2
	/// 首先顺时针 0	1	3
	/// 再是	   1	2	3
	/// </summary>
	inline void FillQuadFP(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface = nullptr)
	{
		///			0		1
		///			3		2
		/// 首先顺时针 0	1	3
		/// 再是	   1	2	3
		FillTriangleFP(x0, y0, x1, y1, x3, y3, color, surface);
		FillTriangleFP(x1, y1, x2, y2, x3, y3, color, surface);
	}
	inline void FillQuad(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, COLOR color, SURFACE* surface = nullptr)
	{
		///			0		1
		///			3		2
		/// 首先顺时针 0	1	3
		/// 再是	   1	2	3
		FillTriangle(x0, y0, x1, y1, x3, y3, color, surface);
		FillTriangle(x1, y1, x2, y2, x3, y3, color, surface);
	}


	// a 2D vertex
	typedef struct VERTEX2D
	{
		REAL x, y; // the vertex
	} VERTEX2D;


}