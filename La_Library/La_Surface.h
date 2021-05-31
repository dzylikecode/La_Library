#pragma once
#include "La_GraphicBase.h"
#include "La_Bitmap.h"
namespace GRAPHIC
{
	extern LPDIRECTDRAWSURFACE7 lpddsback;

	class SURFACE
	{
	private:
		LPDIRECTDRAWSURFACE7 lpddS;
		COLOR* memory;
		int width, height;
		int lpitch;
		DWORD colorKey;
		DWORD memoryFlag;
		int x, y;
	public:
		bool transparent;
	public:
		SURFACE(void) :lpddS(nullptr), memory(nullptr), width(0), height(0), lpitch(0), colorKey(0), memoryFlag(0), x(0), y(0), transparent(true) {};
		SURFACE(int iWidth, int iHeight, DWORD dwColorKey, DWORD flag = 0) :memory(nullptr), lpitch(0), memoryFlag(flag), x(0), y(0), transparent(true)
		{
			create(iWidth, iHeight, dwColorKey, memoryFlag);
			fillColor(dwColorKey);
		}
	public:
		void modifyAllData(LPDIRECTDRAWSURFACE7 lpddSurface, int iWidth, int iHeight, DWORD dwColorKey, DWORD flag) 
		{
			lpddS = lpddSurface; width = iWidth; height = iHeight; colorKey = dwColorKey; memoryFlag = flag;
		}
	public:
		LPDIRECTDRAWSURFACE7 getlpdds(void)const { return lpddS; }
		COLOR* getMemory(void)const { return memory; }
		UINT getLpitch(void)const { return lpitch; }
		UINT getWidth(void)const { return width; }
		UINT getHeight(void)const { return height; }
		int  getX(void)const { return x; }
		int  getY(void)const { return y; }
	public:
		//可以重新设置,但会清空
		bool create(int iWidth, int iHeight, DWORD dwColorKey, DWORD dwMemoryFlag, bool bTransparent = false)
		{
			clear();
			transparent = bTransparent;
			width = iWidth; height = iHeight; colorKey = dwColorKey; memoryFlag = dwMemoryFlag;
			lpddS = CreateSurface(width, height, colorKey, memoryFlag);
			return lpddS;
		}
		bool createFromBitmap(const BITMAP& bitmap);
		bool createFromBitmap(LPCTSTR fileName, ...)
		{
			TCHAR path[MAX_PATH];
			GetVariableArgument(path, MAX_PATH, fileName);
			BITMAP bitmap;
			bitmap.load(fileName);
			return createFromBitmap(bitmap);
		}
		void fillColor(DWORD color, RECT* rect = nullptr) { FillSurfaceColor(lpddS, color, rect); }
		void reset(int iWidth, int iHeight, DWORD dwColorKey, DWORD dwMemoryFlag, bool bTransparent = false)
		{
			transparent = bTransparent;
			width = iWidth; height = iHeight; colorKey = dwColorKey; memoryFlag = dwMemoryFlag;
			LPDIRECTDRAWSURFACE7 temp = CreateSurface(width, height, colorKey, memoryFlag);
			temp->Blt(nullptr, lpddS, nullptr, DDBLT_WAIT, nullptr);
			lpddS->Release();
			lpddS = temp;
		}
		void resize(int iWidth, int iHeight)
		{
			reset(iWidth, iHeight, colorKey, memoryFlag);
		}
	private:
		void copy(const SURFACE& sur)
		{
			transparent = sur.transparent;
			width = sur.width; height = sur.height; colorKey = sur.colorKey; memoryFlag = sur.memoryFlag;
			memory = nullptr; lpitch = 0; x = 0; y = 0;
			lpddS = CreateSurface(width, height, colorKey, memoryFlag);
			lpddS->Blt(nullptr, sur.lpddS, nullptr, DDBLT_WAIT, nullptr);
		}
	public:
		SURFACE(const SURFACE& surface) { copy(surface); }
		SURFACE& operator=(const SURFACE& sur)
		{
			if (this != &sur)
			{
				copy(sur);
			}
			return *this;
		}
		//翻转内容
		//左右翻转
		SURFACE operator~(void);
		//上下翻转
		SURFACE operator!(void);
		COLOR operator()(UINT uiX, UINT uiY, COLOR color) { assert(uiX >= 0 && uiX < width&& uiY >= 0 && uiY < height); return memory[uiY * lpitch + uiX] = color; }
		SURFACE& operator()(UINT uiX, UINT uiY) { assert(uiX >= 0 && uiX < width&& uiY >= 0 && uiY < height); x = uiX; y = uiY; return*this; }
		SURFACE& operator<<(const SURFACE& sur)
		{
			RECT rect = { x, y, x + sur.width - 1, y + sur.height - 1 };
			lpddS->Blt(&rect, sur.lpddS, nullptr, sur.transparent ? (DDBLT_WAIT | DDBLT_KEYSRC) : DDBLT_WAIT, nullptr);
			return *this;
		}
		SURFACE& operator<<(const COLOR color) { (*this)(x, y, color); return *this; }
	public:
		void drawOn(int x, int y, bool scaleWidth, bool scaleHeight, bool bTransparent = true)
		{
			RECT rect = { x, y, x + scaleWidth - 1, y + scaleHeight - 1 };
			lpddsback->Blt(&rect, lpddS, nullptr, transparent ? (DDBLT_WAIT | DDBLT_KEYSRC) : DDBLT_WAIT, nullptr);
		}
		void drawOn(int x, int y, bool bTransparent = true) { drawOn(x, y, width, height, bTransparent); }
		/*void scroll(int dx, int dy);*/
	public:
		friend void BeginDrawOn(SURFACE* surface);
		friend void EndDrawOn(SURFACE* surface);
	public:
		void clear(){ if (lpddS) { lpddS->Release(); lpddS = nullptr; } }
		~SURFACE() { clear(); }
	};


	extern SURFACE graphicOut;
#define INCLUDE_NULL_SURFACE(surface)	(surface ? surface : &graphicOut)

	inline COLOR SetPixel(int x, int y, COLOR color, SURFACE* surface = nullptr)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		return (*surface)(x, y, color);
	}
	inline void BeginDrawOn(SURFACE* surface = nullptr)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		DDSURFACEDESC2 ddsd;
		INIT_STRUCT(ddsd);

		surface->lpddS->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr);
		surface->memory = (COLOR*)ddsd.lpSurface;
		surface->lpitch = ddsd.lPitch >> 2;
	}
	inline void EndDrawOn(SURFACE* surface = nullptr)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		surface->lpddS->Unlock(nullptr);
	}

	bool InitializeGraphics(HWND hwnd, int width, int height, bool bWindowed);
}

