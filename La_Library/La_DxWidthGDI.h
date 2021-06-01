#pragma once
#include "La_WinGDI.h"
#include "La_Surface.h"

namespace GRAPHIC
{

	inline void BeginGDI(void)
	{
		GDI::hdcOld = GDI::hdcCur;
		GDI::hdcCur = GRAPHIC::graphicOut.startGDI();
	}

	inline void EndGDI(void)
	{
		GRAPHIC::graphicOut.endGDI();
		GDI::hdcCur = GDI::hdcOld;
	}


	inline void gPrintf(int x, int y, COLORREF color, LPCTSTR message, ...)
	{
		BeginGDI();

		TCHAR buffer[MAX_BUFFER];

		GetVariableArgument(buffer, MAX_BUFFER, message);
		GDI::WinPrintf(x, y, color, buffer);

		EndGDI();
	}


}