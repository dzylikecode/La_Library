#include "La_Surface.h"

namespace GRAPHIC
{
	SURFACE graphicOut;  //由他来管理 back surface


	
	LPDIRECTDRAWSURFACE7 lpddsback = nullptr;
	namespace
	{
		bool bInitialize_Graphics = false;
		const DWORD SCREEN_COLOR_KEY = 0;
		const DWORD SCREEN_BACK_MEMORY = 0;

		LPDIRECTDRAWSURFACE7 lpddsprimary = nullptr;
		LPDIRECTDRAWCLIPPER  lpddclipper = nullptr;
		LPDIRECTDRAWCLIPPER  lpddclipperwin = nullptr;
	}
	
	extern LPDIRECTDRAW7		  lpdd7;

	int min_clip_x = 0,
		max_clip_x = 0,
		min_clip_y = 0,
		max_clip_y = 0;


	bool InitializeGraphics(HWND hwnd, int width, int height, bool bWindowed)
	{
		if (bInitialize_Graphics)
		{
			return true;
		}
		//首先创建7.0的接口object
		if (FAILED(DirectDrawCreateEx(nullptr, (LPVOID*)&lpdd7, IID_IDirectDraw7, nullptr)))
		{
			ERROR_MSG(ERR, TEXT("Create COM Direct Draw Failed"));
			return false;
		}

		//然后设置协作等级
		if (bWindowed)//针对窗口
		{
			if (FAILED(lpdd7->SetCooperativeLevel(hwnd, DDSCL_NORMAL)))
			{
				ERROR_MSG(ERR, TEXT("COM Direct Draw Sets Cooperative Level Failed"));
				return false;
			}
		}
		else//针对全屏
		{
			//DDSCL_ALLOWMODEX 允许mode X
			//DDSCL_ALLOWREBOOT 检测ctrl + alt + delete
			//多线程
			//EXCLUSIVE FULLSCREEN 连用，表示不能用GDI改写屏幕
			if (FAILED(lpdd7->SetCooperativeLevel(hwnd, DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_MULTITHREADED)))
			{
				ERROR_MSG(ERR, TEXT("COM Direct Draw Sets Cooperative Level Failed"));
				return false;
			}

			//设置显示模式
			if (FAILED(lpdd7->SetDisplayMode(width, height, 32, 0, 0)))
			{
				ERROR_MSG(ERR, TEXT("COM Direct Draw Sets Display Mode Failed"));
				return false;
			}
		}

		DDSURFACEDESC2 ddsd;
		INIT_STRUCT(ddsd);

		//创建主表面
		if (!bWindowed)//full screen
		{
			ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			//primary 可见
			//flip    有一个前端缓冲和后备缓冲
			//complex 翻转链
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

			//一个后备 双缓冲；也可2个 三缓冲
			ddsd.dwBackBufferCount = 1;
		}
		else
		{
			ddsd.dwFlags = DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			//windows mode 没有后备表面缓冲，可以自己设计生成
			//没有打开，这个有效符，也没什么赋值的，前面已经清零了
			//ddsd.dwBackBufferCount = 0;
		}

		if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsprimary, nullptr)))
		{
			ERROR_MSG(ERR, TEXT("Create Primary Surface Failed"));
			return false;
		}

#if LADZY_DEBUG
		//获取主表面的色素格式
		DDPIXELFORMAT ddpf;

		INIT_STRUCT(ddpf);

		lpddsprimary->GetPixelFormat(&ddpf);

		int dd_pixel_format = ddpf.dwRGBBitCount;
		//记录并查看
		ERROR_MSG(INFO, TEXT("\npixel format = %d"), dd_pixel_format);
		if (dd_pixel_format != sizeof(COLOR) * 8)
		{
			ERROR_MSG(ERR, TEXT("need 32 bits per pixel"), sizeof(COLOR) * 8);
			return false;
		}
#endif

		//设置后备缓冲区,获取相应的指针
		//创建主表面的时候已经要求有1个后备缓冲区了
		if (!bWindowed)
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;


			//关联到主表面
			if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
			{
				ERROR_MSG(ERR, TEXT("Attached Surface Failed!"));
				return false;
			}
		}
		else
		{
			//full screen 下可以用flip技术
			//window mode 下创建双缓冲就用blit
			//利用的不是后备表面，而是离屏表面，没有和前表面建立联系
			lpddsback = CreateSurface(width, height, SCREEN_COLOR_KEY, SCREEN_BACK_MEMORY);
		}

		graphicOut.modifyAllData(lpddsback, width, height, SCREEN_COLOR_KEY, SCREEN_BACK_MEMORY);


		//清空表面，用黑色做背景
		if (bWindowed)
		{
			//只是清除后备缓冲区就好了
			//主表面需要进行裁剪才能清除
			//要不然会清除整个桌面
			//dx 模式下， 它拥有的是整个桌面，不是单独的一个窗口
			FillSurfaceColor(lpddsback, SCREEN_COLOR_KEY, nullptr);
		}
		else
		{
			//前后全部清除
			FillSurfaceColor(lpddsprimary, SCREEN_COLOR_KEY, nullptr);
			FillSurfaceColor(lpddsback, SCREEN_COLOR_KEY, nullptr);
		}

		//设置软件裁剪区域
		//这是相对于表面而定的
		//窗口模式下，也就说明了离屏表面可以设置 裁剪器
		min_clip_x = 0;
		max_clip_x = width - 1;
		min_clip_y = 0;
		max_clip_y = height - 1;

		//裁剪应该关联到后备缓冲区,主表面不需要
		//裁剪器最好和主表面通大小

		RECT screen_rect = { 0, 0, width, height };
		lpddclipper = SurfaceAttachClipper(lpddsback, 1, &screen_rect);


		//windows 的裁剪器比较特殊
		//它的后表面也关联了裁剪器，前表面也要，前表面会移动
		if (bWindowed)
		{

			if (FAILED(lpdd7->CreateClipper(0, &lpddclipperwin, nullptr)))
			{
				ERROR_MSG(ERR, TEXT("Create Clipper Failed!"));
				return false;
			}
			//设置好后，会自动根据窗体来自动裁剪
			if (FAILED(lpddclipperwin->SetHWnd(0, hwnd)))
			{
				ERROR_MSG(ERR, TEXT("Connect Clip With Window Failed!"));
				return false;
			}
			//关联到主表面
			//这就说明，在back surface 上有一个窗口大小的裁剪区（相对于屏幕的原点）
			//当blt时，它会自动移动到窗口处
			if (FAILED(lpddsprimary->SetClipper(lpddclipperwin)))
			{
				ERROR_MSG(ERR, TEXT("Attach Clip To Primary Surface Failed!"));
				return false;
			}
		}

		bInitialize_Graphics = true;

		return true;
	}

	//全局变量代理者
	class GRAPHICMaster
	{
	public:
		~GRAPHICMaster()
		{
			if (bInitialize_Graphics)
			{
				//释放所有的COM组件
				if (lpddclipper)
				{
					lpddclipper->Release();
					lpddclipper = nullptr;
				}

				if (lpddclipperwin)
				{
					lpddclipperwin->Release();
					lpddclipperwin = nullptr;
				}

				/*if (lpddsback)
				{
					lpddsback->Release();
					lpddsback = nullptr;
				}*/

				if (lpddsprimary)
				{
					lpddsprimary->Release();
					lpddsprimary = nullptr;
				}

				if (lpdd7)
				{
					lpdd7->Release();
					lpdd7 = nullptr;
				}
			}
		}
	}graphicMaster;


}

