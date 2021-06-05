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
*  创建时间: 2021/06/01 2:39:55
*  最后修改: 
*
*  简    介: 
*
***************************************************************************************
*/

#pragma once
#include "La_GraphicBase.h"
#include "La_Bitmap.h"
namespace GRAPHIC
{
//创建的是离屏表面
//0 是 默认VRAM内存中
//DDCAPS_SYSTEMMEMORY 是系统内存
//也可以创建在显存当中
#define SURFACE_DEFAULT_M	0
#define SURFACE_LOCALVIDEO	DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM
#define SURFACE_NONLOCALVM	DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM

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
		HDC xdc;
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
		DWORD getColorKey(void)const { return colorKey; }
		HDC  getDC(void)const { return xdc; }
	public:
		HDC  startGDI(void){ lpddS->GetDC(&xdc); return xdc; }
		void endGDI(void)const{ lpddS->ReleaseDC(xdc); }
	public:
		//可以重新设置,但会清空
		bool create(int iWidth, int iHeight, DWORD dwColorKey, DWORD dwMemoryFlag = 0, bool bTransparent = true)
		{
			clear();
			transparent = bTransparent;
			width = iWidth; height = iHeight; colorKey = dwColorKey; memoryFlag = dwMemoryFlag;
			lpddS = CreateSurface(width, height, colorKey, memoryFlag);
			return lpddS;
		}
		bool createFromBitmap(const BITMAP& bitmap, DWORD dwColorKey, DWORD dwMemoryFlag = 0, bool bTransparent = true)
		{
			if (create(bitmap.getWidth(), bitmap.getHeight(), dwColorKey, dwMemoryFlag, bTransparent) && bitmap.isLoad())
			{
				BeginDrawOn(this);

				if (lpitch == width)
				{
					memcpy(memory, bitmap.getImage(), bitmap.getSizeByte());
				}
				else
				{
					COLOR* pdest = memory;
					COLOR* psource = bitmap.getImage();
					int bytesPerLine = lpitch * sizeof(COLOR);
					for (int i = 0; i < height; i++)
					{
						memcpy(pdest, psource, bytesPerLine);

						pdest += lpitch;
						psource += bitmap.getWidth();
					}
				}
				EndDrawOn(this);

				return true;
			}
			
			return false;
		}
		bool createFromBitmap(DWORD dwColorKey, LPCTSTR fileName, ...)
		{
			TCHAR path[MAX_PATH];
			GetVariableArgument(path, MAX_PATH, fileName);
			BITMAP bitmap;
			if (bitmap.load(path))
			{
				ERROR_MSG(ERR, TEXT("无法加载图片！"));
				return createFromBitmap(bitmap, dwColorKey);
			}
			return false;
		}
		bool createFromBitmap(LPCTSTR fileName, ...)
		{
			TCHAR path[MAX_PATH];
			GetVariableArgument(path, MAX_PATH, fileName);
			BITMAP bitmap;
			if (!bitmap.load(path))
			{
				ERROR_MSG(ERR, TEXT("Can not load bitmap"));
				return false;
			}

			return createFromBitmap(bitmap, 0);
		}
		bool createFromSurface(int iWidth, int iHeight, const SURFACE& source, int x, int y, DWORD dwColorKey = 0)
		{
			RECT source_rect = { x, y, x + iWidth - 1, y + iHeight - 1 };
			bool bRe = create(iWidth, iHeight, dwColorKey, memoryFlag, transparent);
			lpddS->Blt(nullptr, source.lpddS, &source_rect, DDBLT_WAIT, nullptr);
			return bRe;
		}
		void fillColor(DWORD color, RECT* rect = nullptr) { FillSurfaceColor(lpddS, color, rect); }
		void fillColor(RECT* rect = nullptr) { fillColor(colorKey, rect); }
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
		void copy(LPDIRECTDRAWSURFACE7 dest, int destX, int destY, LPDIRECTDRAWSURFACE7 source, int sourceX, int sourceY, int width, int height)
		{
			RECT source_rect = { sourceX, sourceY, sourceX + width, sourceY + height };
			RECT dest_rect = { destX, destY, destX + width, destY + height };
			dest->Blt(&dest_rect, source, &source_rect, DDBLT_WAIT, nullptr);
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
		void mirrorLR()
		{
			LPDIRECTDRAWSURFACE7 temp = CreateSurface(width, height, colorKey, memoryFlag);
			DDBLTFX ddBltFxMirror;
			INIT_STRUCT(ddBltFxMirror);
			ddBltFxMirror.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;
			temp->Blt(nullptr, lpddS, nullptr, DDBLT_WAIT | DDBLT_DDFX, &ddBltFxMirror);
			lpddS->Release();
			lpddS = temp;
		}
		SURFACE operator~(void)
		{
			SURFACE  temp = *this;
			temp.mirrorLR();
			return temp;
		}
		//上下翻转
		void mirrorUD()
		{
			LPDIRECTDRAWSURFACE7 temp = CreateSurface(width, height, colorKey, memoryFlag);
			DDBLTFX ddBltFxMirror;
			INIT_STRUCT(ddBltFxMirror);
			ddBltFxMirror.dwDDFX = DDBLTFX_MIRRORUPDOWN;
			temp->Blt(nullptr, lpddS, nullptr, DDBLT_WAIT | DDBLT_DDFX, &ddBltFxMirror);
			lpddS->Release();
			lpddS = temp;
		}
		SURFACE operator!(void)
		{
			SURFACE temp = *this;
			temp.mirrorUD();
			return temp;
		}
		COLOR operator()(UINT uiX, UINT uiY, COLOR color) { assert( uiX < width && uiY < height); return memory[uiY * lpitch + uiX] = color; }
		SURFACE& operator()(UINT uiX, UINT uiY) { assert(uiX < width && uiY < height); x = uiX; y = uiY; return*this; }
		SURFACE& operator<<(const SURFACE& sur)
		{
			RECT rect = { x, y, x + min(sur.width, width) - 1, y + min(sur.height, height) - 1 };
			lpddS->Blt(&rect, sur.lpddS, nullptr, sur.transparent ? (DDBLT_WAIT | DDBLT_KEYSRC) : DDBLT_WAIT, nullptr);
			return *this;
		}
		SURFACE& operator<<(const COLOR color) { (*this)(x, y, color); return *this; }
	public:
		void drawOn(int x, int y, int scaleWidth, int scaleHeight, bool bTransparent = true)
		{
			RECT rect = { x, y, x + scaleWidth - 1, y + scaleHeight - 1 };
			lpddsback->Blt(&rect, lpddS, nullptr, transparent ? (DDBLT_WAIT | DDBLT_KEYSRC) : DDBLT_WAIT, nullptr);
		}
		void drawOn(int x, int y, bool bTransparent = true) { drawOn(x, y, width, height, bTransparent); }
		void scroll(int dx, int dy)
		{
			scrollX(dx);
			scrollY(dy);
		}
		void scrollX(int dx)
		{
			if (dx == 0)
			{
				return;
			}

			LPDIRECTDRAWSURFACE7 tempsuface = CreateSurface(width, height, colorKey, memoryFlag);

			if (dx != 0)
			{
				dx %= width;

				if (dx > 0)
				{
					//先将左边的超出的部分复制过去
					copy(tempsuface, 0, 0, lpddS, width - dx, 0, dx, height);
					//再将剩下的补上
					copy(tempsuface, dx, 0, lpddS, 0, 0, width - dx, height);
				}
				else
				{
					dx = -dx;
					//先移动未超出的，再移动超出的
					copy(tempsuface, 0, 0, lpddS, dx, 0, width - dx, height);
					copy(tempsuface, width - dx, 0, lpddS, 0, 0, dx, height);

				}
			}

			lpddS->Release();
			lpddS = tempsuface;
		}
		void scrollY(int dy)
		{
			if (dy == 0)
			{
				return;
			}

			LPDIRECTDRAWSURFACE7 tempsuface = CreateSurface(width, height, colorKey, memoryFlag);

			if (dy != 0)
			{
				dy %= height;

				if (dy > 0)
				{
					copy(tempsuface, 0, 0, lpddS, 0, height - dy, width, dy);
					copy(tempsuface, 0, dy, lpddS, 0, 0, width, height - dy);
				}
				else
				{
					dy = -dy;
					copy(tempsuface, 0, 0, lpddS, 0, dy, width, height - dy);
					copy(tempsuface, 0, height - dy, lpddS, 0, 0, width, dy);
				}
			}

			lpddS->Release();
			lpddS = tempsuface;
		}
	public:
		friend void BeginDrawOn(SURFACE* surface);
		friend void EndDrawOn(SURFACE* surface);
	public:
		void clear(){ if (lpddS) { lpddS->Release(); lpddS = nullptr; } }
		~SURFACE() { clear(); }
	};


	extern SURFACE graphicOut;
#define INCLUDE_NULL_SURFACE(surface)	(surface ? surface : &graphicOut)

	
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
		surface->memory = nullptr; surface->lpitch = 0;
	}


	bool InitializeGraphics(HWND hwnd, int width, int height, bool bWindowed);

	void Flush();

	bool ScanColor(int x1, int y1, int x2, int y2, COLOR scan_start, COLOR scan_end, SURFACE* surface = nullptr);
	void WaitForVsync(void);


	extern int min_clip_x, max_clip_x, min_clip_y, max_clip_y;
}

