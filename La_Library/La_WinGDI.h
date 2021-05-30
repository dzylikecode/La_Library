#pragma once
#include "La_Windows.h"




namespace GDI
{
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
		/// PS_NULL		���ɼ�
		/// PS_SOLID	
		/// PS_DASH		����
		/// PS_DOT
		/// PS_DASHDOT
		/// PS_DASHDOTDOT
	private:
		static int curPenID; //��ǰ�ڱ�ʹ�õ� id   0��ʾϵͳ�ı�����ʹ��
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

}

