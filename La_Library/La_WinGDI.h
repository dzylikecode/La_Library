#pragma once
#include "La_Windows.h"




namespace GDI
{
	enum MANIPULATOR
	{
		end,
		endl			//当然我也会实现下一行，反正内部有游标记录嘛，慢慢来好了
	};

	class WINOut
	{
	private:
		TSTRING content;
		int x, y;
	public:
		WINOut() :x(0), y(0) {};
		WINOut& operator()(int posX, int posY) { x = posX; y = posY; return *this; }
		WINOut& operator()(int posX, int posY, COLORREF newColor);
		WINOut& operator<<(LPCTSTR string) { content << string; return *this; }
		void    operator<<(MANIPULATOR maniputor);
	};

	extern WINOut winOut;

	void InitializeGDI(HWND hwnd);

	bool WinPrintf(COLORREF color, int x, int y, LPCTSTR string, ...);
	bool WinPrintf(int x, int y, LPCTSTR string, ...);
	HDC GetHdc();
	COLORREF SetWinTextColor(COLORREF color);
	COLORREF SetWinTextBkColor(COLORREF color);
	int SetWinBKMode(bool bTransparent);



	class PEN
	{
	public:
		/// style
		/// PS_NULL		不可见
		/// PS_SOLID	
		/// PS_DASH		虚线
		/// PS_DOT
		/// PS_DASHDOT
		/// PS_DASHDOTDOT
	private:
		static int curPenID; //当前在被使用的 id   0表示系统的笔正在使用
		int ID;
		int style;
		int width;
		COLORREF color;
	public:
		PEN();
		int getID()const { return ID; }
		static int GetWorkID() { return curPenID; }
		int select()
		{
			if (curPenID)
			{
				DeleteObject(SelectObject(hdc, CreatePen(style, width, color)));
			}
			else //说明是系统的
			{
				SelectObject(hdc, CreatePen(style, width, color));
			}
			int oldID = curPenID;
			curPenID = ID;
			return oldID;
		}
		// white pen
		// black pen
		// null  pen
		static int Select(int systemPen)
		{
			if (curPenID)
			{
				DeleteObject(SelectObject(hdc, GetStockObject(systemPen)));
			}
			else //说明是系统的
			{
				SelectObject(hdc, GetStockObject(systemPen));
			}
			int oldID = curPenID;
			curPenID = 0;
			return oldID;
		}
		int setWidth(int penWidth) { width = penWidth; return select(); }
		int setStyle(int penStyle) { style = penStyle; return select(); }
		int setColor(COLORREF penColor) { color = penColor; return select(); }
		int setAttr(int penWidth, int penStyle, COLORREF penColor)
		{
			width = penWidth;
			style = penStyle;
			color = penColor;
			return select();
		}
		int  getStyle()const { return style; }
		int  getWidth()const { return width; }
		COLORREF getColor()const { return color; }
		~PEN()
		{
			//当它要被析构的时候，应该让系统的笔选入
			//说明当前 ID　被选入了
			if (curPenID == ID) 
			{
				DeleteObject(SelectObject(hdc, GetStockObject(WHITE_PEN)));
				curPenID = 0; //可能只是局部变量消失了而已，所以要照顾照顾这个变量
			}
		}
	};


	class BRUSH
	{
	public:
		enum STYLE: int
		{
			BDIAGONAL = HS_BDIAGONAL,		///		\ 形状
			FDIAGONAL = HS_FDIAGONAL,		//		/
			DIAGCROSS = HS_DIAGCROSS,		//		X
			HORIZONTAL = HS_HORIZONTAL,		//		-
			VERTICAL = HS_VERTICAL,			//		|
			CROSS = HS_CROSS,				//		+
			SOLID = -1
		};
	private:
		static int curBrushID;
		int ID;
		COLORREF color;
		STYLE style;
	public:
		BRUSH();
		int getID()const { return ID; }
		static int GetWorkID() { return curBrushID; }
		int select()
		{
			if (curBrushID)
			{
				if (style == SOLID)
				{
					DeleteObject(SelectObject(hdc, CreateSolidBrush(color)));
				}
				else
				{
					DeleteObject(SelectObject(hdc, CreateHatchBrush(style, color)));
				}
				
			}
			else //说明是系统的
			{
				if (style == SOLID)
				{
					SelectObject(hdc, CreateSolidBrush(color));
				}
				else
				{
					SelectObject(hdc, CreateHatchBrush(style, color));
				}
			}
			int oldID = curBrushID;
			curBrushID = ID;
			return oldID;
		}
		//black brush
		//dkgray brush		dark
		//gray
		//hollow
		//ltgray			light
		//null
		//white
		static int Select(int systemBrush)
		{
			if (curBrushID)
			{
				DeleteObject(SelectObject(hdc, GetStockObject(systemBrush)));

			}
			else //说明是系统的
			{
				SelectObject(hdc, GetStockObject(systemBrush));
			}
			int oldID = curBrushID;
			curBrushID = 0;
			return oldID;
		}
		int setColor(COLORREF brushColor) { color = brushColor; return select(); }
		int setStyle(STYLE brushStyle) { style = brushStyle; return select(); }
		int setAttr(COLORREF brushColor, STYLE brushStyle) { style = brushStyle; color = brushColor; return select(); }
		COLORREF getColor()const { return color; }
		STYLE getStyle()const { return style; }
		~BRUSH()
		{
			if (curBrushID == ID)
			{
				DeleteObject(SelectObject(hdc, GetStockObject(BLACK_BRUSH)));
				curBrushID = 0;
			}
		}
	};

}

