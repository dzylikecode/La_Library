#include "La_WinGDI.h"

namespace
{
	bool bInitialize_GDI = false;

	HDC hdc;
	//font
	COLORREF font_Color;
	COLORREF back_Color;
	int mode;
	//pen
	//HPEN temp_pen = NULL;	///�Ǻǣ���ʲô�أ�ֱ��ѡһ֧�ʽ�ȥ���ˣ�����û��ϵͳ�ı�����
	int pen_width;
	int pen_color;
	int pen_style;

	//Brush
	bool bNullBrush = false; //����ɾ��ϵͳ�Ŀ��Ļ�ˢ


	static int penIDProduce = 0;
	static int brushIDProduce = 0;
}

namespace GDI
{
	void WINOut::operator<<(MANIPULATOR maniputor)
	{
		TextOut(hdc, x, y, content, content.getLength());
		content.clear();
	}
	WINOut& WINOut::operator()(int posX, int posY, COLORREF newColor)
	{
		x = posX; y = posY;
		SetTextColor(hdc, newColor);
		return *this;
	}

	WINOut winOut;

	void InitializeGDI(HWND hwnd)
	{
		if (bInitialize_GDI)
		{
			return;
		}
		bInitialize_GDI = true;
		hdc = GetDC(hwnd);
		//font
		back_Color = RGB(0, 0, 0);
		font_Color = RGB(0, 255, 0);
		mode = TRANSPARENT;

		pen_style = PS_SOLID;
		pen_width = 1;
		pen_color = RGB(0, 255, 0);

		//Ĭ����͸��ɫ��
		SetWinBKMode(true);
		SetWinTextColor(RGB(0, 255, 255));
	}

	bool WinPrintf(COLORREF color, int x, int y, LPCTSTR string, ...)
	{
		TCHAR buffer[MAX_BUFFER];

		GetVariableArgument(buffer, MAX_BUFFER, string);

		font_Color = color;

		SetTextColor(hdc, font_Color);
		SetBkColor(hdc, back_Color);
		SetBkMode(hdc, mode);

		return TextOut(hdc, x, y, buffer, lstrlen(buffer));
	}

	bool WinPrintf(int x, int y, LPCTSTR string, ...)
	{
		TCHAR buffer[MAX_BUFFER];

		GetVariableArgument(buffer, MAX_BUFFER, string);

		return TextOut(hdc, x, y, buffer, lstrlen(buffer));
	}

	HDC GetHdc()
	{
		return hdc;
	}

	COLORREF SetWinTextColor(COLORREF color)
	{
		return SetTextColor(hdc, color);
	}
	
	COLORREF SetWinTextBkColor(COLORREF color)
	{
		return SetBkColor(hdc, color);
	}

	int SetWinBKMode(bool bTransparent)
	{
		if (bTransparent)
		{
			return SetBkMode(hdc, TRANSPARENT);
		}
		else
		{
			return SetBkMode(hdc, OPAQUE);
		}
	}



	int PEN::curPenID = 0;

	PEN::PEN() :ID(++penIDProduce), style(PS_SOLID), width(1), color(RGB(0, 255, 255)) {};

	int BRUSH::curBrushID = 0;
	BRUSH::BRUSH() :ID(++brushIDProduce), color(RGB(0, 0, 0)), style(SOLID)
	{

	}
}

