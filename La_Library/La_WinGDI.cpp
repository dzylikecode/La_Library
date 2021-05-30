#include "La_WinGDI.h"

namespace
{
	bool bInitialize_GDI = false;

	static int penIDProduce = 0;
	static int brushIDProduce = 0;

	HWND hwndGDI;
}

namespace GDI
{
	HDC hdc;

	WINOut winOut;

	void InitializeGDI(HWND hwnd)
	{
		if (bInitialize_GDI)
		{
			return;
		}
		
		hwndGDI = hwnd;
		hdc = GetDC(hwndGDI);
		//默认是透明色嘛
		SetWinBKMode(true);
		SetWinTextColor(RGB(0, 255, 255));

		bInitialize_GDI = true;
	}

	bool WinPrintf(COLORREF color, int x, int y, LPCTSTR string, ...)
	{
		TCHAR buffer[MAX_BUFFER];

		GetVariableArgument(buffer, MAX_BUFFER, string);

		SetWinTextColor(color);

		return TextOut(hdc, x, y, buffer, lstrlen(buffer));
	}

	bool WinPrintf(int x, int y, LPCTSTR string, ...)
	{
		TCHAR buffer[MAX_BUFFER];

		GetVariableArgument(buffer, MAX_BUFFER, string);

		return TextOut(hdc, x, y, buffer, lstrlen(buffer));
	}

	int PEN::curPenID = 0;

	PEN::PEN() :ID(++penIDProduce), style(PS_SOLID), width(1), color(RGB(0, 255, 255)) {};

	int BRUSH::curBrushID = 0;
	BRUSH::BRUSH() :ID(++brushIDProduce), color(RGB(0, 0, 0)), style(SOLID)
	{

	}


	class GDIMaster
	{
	public:
		~GDIMaster()
		{
			if (bInitialize_GDI)
			{
				ReleaseDC(hwndGDI, hdc);
			}
		}
	}gdiMaster;

}

