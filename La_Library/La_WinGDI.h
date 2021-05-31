/*
***************************************************************************************
*  程    序: 
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/05/31 0:28:24
*  最后修改: 
*
*  简    介: 
*
***************************************************************************************
*/

#pragma once
#include "La_WindowsBase.h"




namespace GDI
{
	extern HDC hdc;

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
		WINOut& operator()(int posX, int posY, COLORREF newColor)
		{
			x = posX; y = posY;
			SetTextColor(hdc, newColor);
			return *this;
		}
		WINOut& operator<<(LPCTSTR string) { content << string; return *this; }
		void    operator<<(MANIPULATOR maniputor)
		{
			TextOut(hdc, x, y, content, content.getLength());
			content.clear();
		}
	};

	extern WINOut winOut;

	void InitializeGDI(HWND hwnd);

	bool WinPrintf(COLORREF color, int x, int y, LPCTSTR string, ...);
	bool WinPrintf(int x, int y, LPCTSTR string, ...);
	inline HDC GetHdc() { return hdc; }
	inline COLORREF SetWinTextColor(COLORREF color) { return SetTextColor(hdc, color); }
	inline COLORREF SetWinTextBkColor(COLORREF color) { return SetBkColor(hdc, color); }
	inline int SetWinBKMode(bool bTransparent) { return bTransparent ? SetBkMode(hdc, TRANSPARENT) : SetBkMode(hdc, OPAQUE); }



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
		const int ID; //显然更好
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
		static int curBrushID;  //int 的话 也不怕 溢出吧
		const int ID;
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

	inline COLORREF SetPixel(int x, int y, COLORREF color) { return ::SetPixel(hdc, x, y, color); }

	inline bool MoveTo(int x, int y, POINT* lastPosition = nullptr) { return MoveToEx(hdc, x, y, lastPosition); }

	inline bool LineTo(int x, int y) { return ::LineTo(hdc, x, y); }

	inline void DrawLine(int start_x, int start_y, int end_x, int end_y) { MoveTo(start_x, start_y); LineTo(end_x, end_y); }

	inline void DrawLine(COLORREF color, int start_x, int start_y, int end_x, int end_y) { PEN pen; pen.setColor(color); DrawLine(start_x, start_y, end_x, end_y); }

	inline bool Rectangle(int left, int top, int right, int bottom) { return Rectangle(hdc, left, top, right, bottom); }

	inline void FrameRect(COLORREF color, const RECT* rect)
	{
		HBRUSH temp_brush = CreateSolidBrush(color);

		::FrameRect(hdc, rect, temp_brush);

		DeleteObject(temp_brush);
	}
	inline void FrameRect(COLORREF color, int left, int top, int right, int bottom)
	{
		RECT rect = { left, top, right, bottom };
		FrameRect(color, &rect);
	}

	inline void FillRect(COLORREF color, const RECT* rect)
	{
		HBRUSH temp_brush = CreateSolidBrush(color);

		::FillRect(hdc, rect, temp_brush);

		DeleteObject(temp_brush);
	}
	inline void FillRect(COLORREF color, int left, int top, int right, int bottom)
	{
		RECT rect = { left, top, right, bottom };
		FillRect(color, &rect);
	}
	
	//需要持续作用，所以应该是 static 
	//也挺好的，可以持续管理，不会使得每用一次就，消失一次，然后 produce 就自增一下
	//那样循环几下，说不定使得 produce 溢出呢
	inline void Solid(COLORREF color){ static PEN pen; pen.setColor(color); static BRUSH brush; brush.setColor(color); }
	inline void Hollow(COLORREF color){ static PEN pen; pen.setColor(color); BRUSH::Select(NULL_BRUSH); }

	inline bool DrawEllipse(int left, int top, int right, int bottom) { return ::Ellipse(hdc, left, top, right, bottom); }
	inline bool DrawEllipse(COLORREF color, int left, int top, int right, int bottom) { Solid(color); return ::Ellipse(hdc, left, top, right, bottom); }
	inline bool DrawHollowEllipse(COLORREF color, int left, int top, int right, int bottom) { Hollow(color); return ::Ellipse(hdc, left, top, right, bottom); }

	inline bool DrawCircle(COLORREF color, int x, int y, int r)
	{
		int left = x - r;
		int right = x + r;
		int top = y - r;
		int bottom = y + r;

		return DrawEllipse(color, left, top, right, bottom);
	}
	inline bool DrawHollowCircle(COLORREF color, int x, int y, int r)
	{
		int left = x - r;
		int right = x + r;
		int top = y - r;
		int bottom = y + r;

		return DrawHollowEllipse(color, left, top, right, bottom);
	}

	inline bool DrawPolygon(const POINT* point, int count) { return ::Polygon(hdc, point, count); }
	inline bool DrawPolygon(COLORREF color, const POINT* point, int count) { Solid(color);  return ::Polygon(hdc, point, count); }
	inline bool DrawHollowPolygon(COLORREF color, const POINT* point, int count) { Hollow(color);  return ::Polygon(hdc, point, count); }


}

