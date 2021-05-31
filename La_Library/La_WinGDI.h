/*
***************************************************************************************
*  ��    ��: 
*
*  ��    ��: LaDzy
*
*  ��    ��: mathbewithcode@gmail.com
*
*  ���뻷��: Visual Studio 2019
*
*  ����ʱ��: 2021/05/31 0:28:24
*  ����޸�: 
*
*  ��    ��: 
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
		endl			//��Ȼ��Ҳ��ʵ����һ�У������ڲ����α��¼�����������
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
		/// PS_NULL		���ɼ�
		/// PS_SOLID	
		/// PS_DASH		����
		/// PS_DOT
		/// PS_DASHDOT
		/// PS_DASHDOTDOT
	private:
		static int curPenID; //��ǰ�ڱ�ʹ�õ� id   0��ʾϵͳ�ı�����ʹ��
		const int ID; //��Ȼ����
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
			else //˵����ϵͳ��
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
			else //˵����ϵͳ��
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
			//����Ҫ��������ʱ��Ӧ����ϵͳ�ı�ѡ��
			//˵����ǰ ID����ѡ����
			if (curPenID == ID) 
			{
				DeleteObject(SelectObject(hdc, GetStockObject(WHITE_PEN)));
				curPenID = 0; //����ֻ�Ǿֲ�������ʧ�˶��ѣ�����Ҫ�չ��չ��������
			}
		}
	};


	class BRUSH
	{
	public:
		enum STYLE: int
		{
			BDIAGONAL = HS_BDIAGONAL,		///		\ ��״
			FDIAGONAL = HS_FDIAGONAL,		//		/
			DIAGCROSS = HS_DIAGCROSS,		//		X
			HORIZONTAL = HS_HORIZONTAL,		//		-
			VERTICAL = HS_VERTICAL,			//		|
			CROSS = HS_CROSS,				//		+
			SOLID = -1
		};
	private:
		static int curBrushID;  //int �Ļ� Ҳ���� �����
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
			else //˵����ϵͳ��
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
			else //˵����ϵͳ��
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
	
	//��Ҫ�������ã�����Ӧ���� static 
	//Ҳͦ�õģ����Գ�����������ʹ��ÿ��һ�ξͣ���ʧһ�Σ�Ȼ�� produce ������һ��
	//����ѭ�����£�˵����ʹ�� produce �����
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

