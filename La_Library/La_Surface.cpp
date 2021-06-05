#include "La_Surface.h"

namespace GRAPHIC
{
	SURFACE graphicOut;  //���������� back surface


	
	LPDIRECTDRAWSURFACE7 lpddsback = nullptr;
	namespace
	{
		bool bInitialize_Graphics = false;
		const DWORD SCREEN_COLOR_KEY = 0;
		const DWORD SCREEN_BACK_MEMORY = 0;

		LPDIRECTDRAWSURFACE7 lpddsprimary = nullptr;
		LPDIRECTDRAWCLIPPER  lpddclipper = nullptr;
		LPDIRECTDRAWCLIPPER  lpddclipperwin = nullptr;

		bool windowed = true;
		HWND globalHwnd;
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
		//���ȴ���7.0�Ľӿ�object
		if (FAILED(DirectDrawCreateEx(nullptr, (LPVOID*)&lpdd7, IID_IDirectDraw7, nullptr)))
		{
			ERROR_MSG(ERR, TEXT("Create COM Direct Draw Failed"));
			return false;
		}

		//Ȼ������Э���ȼ�
		if (bWindowed)//��Դ���
		{
			if (FAILED(lpdd7->SetCooperativeLevel(hwnd, DDSCL_NORMAL)))
			{
				ERROR_MSG(ERR, TEXT("COM Direct Draw Sets Cooperative Level Failed"));
				return false;
			}
		}
		else//���ȫ��
		{
			//DDSCL_ALLOWMODEX ����mode X
			//DDSCL_ALLOWREBOOT ���ctrl + alt + delete
			//���߳�
			//EXCLUSIVE FULLSCREEN ���ã���ʾ������GDI��д��Ļ
			if (FAILED(lpdd7->SetCooperativeLevel(hwnd, DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_MULTITHREADED)))
			{
				ERROR_MSG(ERR, TEXT("COM Direct Draw Sets Cooperative Level Failed"));
				return false;
			}

			//������ʾģʽ
			if (FAILED(lpdd7->SetDisplayMode(width, height, 32, 0, 0)))
			{
				ERROR_MSG(ERR, TEXT("COM Direct Draw Sets Display Mode Failed"));
				return false;
			}
		}

		DDSURFACEDESC2 ddsd;
		INIT_STRUCT(ddsd);

		//����������
		if (!bWindowed)//full screen
		{
			ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			//primary �ɼ�
			//flip    ��һ��ǰ�˻���ͺ󱸻���
			//complex ��ת��
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

			//һ���� ˫���壻Ҳ��2�� ������
			ddsd.dwBackBufferCount = 1;
		}
		else
		{
			ddsd.dwFlags = DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			//windows mode û�к󱸱��滺�壬�����Լ��������
			//û�д򿪣������Ч����Ҳûʲô��ֵ�ģ�ǰ���Ѿ�������
			//ddsd.dwBackBufferCount = 0;
		}

		if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsprimary, nullptr)))
		{
			ERROR_MSG(ERR, TEXT("Create Primary Surface Failed"));
			return false;
		}

#if LADZY_DEBUG
		//��ȡ�������ɫ�ظ�ʽ
		DDPIXELFORMAT ddpf;

		INIT_STRUCT(ddpf);

		lpddsprimary->GetPixelFormat(&ddpf);

		int dd_pixel_format = ddpf.dwRGBBitCount;
		//��¼���鿴
		ERROR_MSG(INFO, TEXT("\npixel format = %d"), dd_pixel_format);
		if (dd_pixel_format != sizeof(COLOR) * 8)
		{
			ERROR_MSG(ERR, TEXT("need 32 bits per pixel"), sizeof(COLOR) * 8);
			return false;
		}
#endif

		//���ú󱸻�����,��ȡ��Ӧ��ָ��
		//�����������ʱ���Ѿ�Ҫ����1���󱸻�������
		if (!bWindowed)
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;


			//������������
			if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
			{
				ERROR_MSG(ERR, TEXT("Attached Surface Failed!"));
				return false;
			}
		}
		else
		{
			//full screen �¿�����flip����
			//window mode �´���˫�������blit
			//���õĲ��Ǻ󱸱��棬�����������棬û�к�ǰ���潨����ϵ
			lpddsback = CreateSurface(width, height, SCREEN_COLOR_KEY, SCREEN_BACK_MEMORY);
		}

		graphicOut.modifyAllData(lpddsback, width, height, SCREEN_COLOR_KEY, SCREEN_BACK_MEMORY);


		//��ձ��棬�ú�ɫ������
		if (bWindowed)
		{
			//ֻ������󱸻������ͺ���
			//��������Ҫ���вü��������
			//Ҫ��Ȼ�������������
			//dx ģʽ�£� ��ӵ�е����������棬���ǵ�����һ������
			FillSurfaceColor(lpddsback, SCREEN_COLOR_KEY, nullptr);
		}
		else
		{
			//ǰ��ȫ�����
			FillSurfaceColor(lpddsprimary, SCREEN_COLOR_KEY, nullptr);
			FillSurfaceColor(lpddsback, SCREEN_COLOR_KEY, nullptr);
		}

		//��������ü�����
		//��������ڱ��������
		//����ģʽ�£�Ҳ��˵������������������� �ü���
		min_clip_x = 0;
		max_clip_x = width - 1;
		min_clip_y = 0;
		max_clip_y = height - 1;

		//�ü�Ӧ�ù������󱸻�����,�����治��Ҫ
		//�ü�����ú�������ͨ��С

		RECT screen_rect = { 0, 0, width, height };
		lpddclipper = SurfaceAttachClipper(lpddsback, 1, &screen_rect);


		//windows �Ĳü����Ƚ�����
		//���ĺ����Ҳ�����˲ü�����ǰ����ҲҪ��ǰ������ƶ�
		if (bWindowed)
		{

			if (FAILED(lpdd7->CreateClipper(0, &lpddclipperwin, nullptr)))
			{
				ERROR_MSG(ERR, TEXT("Create Clipper Failed!"));
				return false;
			}
			//���úú󣬻��Զ����ݴ������Զ��ü�
			if (FAILED(lpddclipperwin->SetHWnd(0, hwnd)))
			{
				ERROR_MSG(ERR, TEXT("Connect Clip With Window Failed!"));
				return false;
			}
			//������������
			//���˵������back surface ����һ�����ڴ�С�Ĳü������������Ļ��ԭ�㣩
			//��bltʱ�������Զ��ƶ������ڴ�
			if (FAILED(lpddsprimary->SetClipper(lpddclipperwin)))
			{
				ERROR_MSG(ERR, TEXT("Attach Clip To Primary Surface Failed!"));
				return false;
			}
		}

		bInitialize_Graphics = true;
		windowed = bWindowed;
		globalHwnd = hwnd;

		return true;
	}

	//ȫ�ֱ���������
	class GRAPHICMaster
	{
	public:
		~GRAPHICMaster()
		{
			if (bInitialize_Graphics)
			{
				//�ͷ����е�COM���
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
				graphicOut.clear(); //����Ҫע��˳���Ҫ�����ʱ���ͷ�

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


	void Flush()
	{
		//��������ſ���
		if (!windowed)//full screen ����ʹ��ҳ�潻������
		{
			//nullptr �Ǻ�����������ĺ󱸻���
			//Ҳ��ָ������һ������
			//Ĭ����1�δ�ֱ��̣����ԣ�| DDFLIP_INTERVAL2) 2��
			//ʲô��������ô��
			/*while (FAILED(lpddsprimary->Flip(nullptr, DDFLIP_WAIT)))
				continue;*/
				/*lpddsprimary->Flip(nullptr, DDFLIP_WAIT);*/


					//�����������Ǳ���Ĳü�������ǰ�����޹أ�ֱ�� fast
					/*RECT source_rect = { 0, 0, width, height };
					lpddsprimary->BltFast(0, 0, lpddsback, &source_rect, DDBLTFAST_WAIT);*/
					//-----------�ѿ���û blt Fast ����


				//��֪��Ϊʲô���ұ߻���һ��ϸϸ����ɫ ����� nullptr
				//���������Ҳ�Ƕ��һ�ٰ�����Ϊ�����õı����ȣ��ü���ȣ��Լ�
				//���ߵ������Ⱦ�����ô��ѽ���治֪������ϵ���Щ�����Ķ�����
				//RECT source_rect = { min_clip_x, min_clip_y, max_clip_x, max_clip_y };
			lpddsprimary->Blt(nullptr, lpddsback, nullptr, DDBLT_WAIT, nullptr);

		}
		else
		{
			//Windows mode ����blt
			RECT dest_rect;
			POINT point = { 0, 0 };
			//��ÿͻ������ڵ�λ��
			ClientToScreen(globalHwnd, &point);
			//���󱸻�������ת��
			dest_rect.left = point.x;
			dest_rect.top = point.y;

			dest_rect.right = dest_rect.left + graphicOut.getWidth();
			dest_rect.bottom = dest_rect.top + graphicOut.getHeight();
#if LADZY_DEBUG
			//�ü����������棬�� blt
			if (FAILED(lpddsprimary->Blt(&dest_rect, lpddsback, nullptr, DDBLT_WAIT, nullptr)))
			{
				ERROR_MSG(ERR, TEXT("Blt Failed!"));
				return;
			}
#else
			lpddsprimary->Blt(&dest_rect, lpddsback, nullptr, DDBLT_WAIT, nullptr);
#endif
		}

	}


	void WaitForVsync(void)
	{
#if LADZY_DEBUG
		//ʹ��ϵͳ�ȴ���ֱ����һ����ֱ�հ����ڿ�ʼ������դ�����ײ�ʱ��
		if (FAILED(lpdd7->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0)))
		{
			ERROR_MSG(ERR, TEXT("Can't Wait For V sync"));
			return;
		}
#else
		lpdd7->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
#endif
	}



/// <summary>
/// ��ĳһ����Χɨ���Ƿ����ĳ���������ɫ
/// </summary>
/// <param name="scan_start">
/// ��ɫ����Ŀ���
/// </param>
/// <param name="scan_end">
/// ��ɫ����Ľ�β
/// </param>
	bool ScanColor(int x1, int y1, int x2, int y2, COLOR scan_start, COLOR scan_end, SURFACE* surface)
	{
		surface = INCLUDE_NULL_SURFACE(surface);
		//ɨ��ķ�Χ
		//clip rectangle
		x1 = max(min_clip_x, min(max_clip_x, x1));
		x2 = max(min_clip_x, min(max_clip_x, x2));

		y1 = max(min_clip_y, min(max_clip_y, y1));
		y2 = max(min_clip_y, min(max_clip_y, y2));

		//��ʼ̽�������
		int scan_lpitch = surface->getLpitch();
		COLOR* scan_buffer = surface->getMemory() + y1 * scan_lpitch;

		for (int scan_y = y1; scan_y <= y2; scan_y++)
		{
			for (int scan_x = x1; scan_x <= x2; scan_x++)
			{
				if (scan_buffer[scan_x] >= scan_start && scan_buffer[scan_x] <= scan_end)
				{
					return true;
				}
			}

			scan_buffer += scan_lpitch;
		}
		return false;
	}

	void ResetClipper(int left, int top, int right, int bottom)
	{
		if (lpddclipper)
		{
			lpddclipper->Release();
			lpddclipper = nullptr;
		}

		RECT screen_rect = { left, top, right, bottom };
		lpddclipper = SurfaceAttachClipper(lpddsback, 1, &screen_rect);
	}
}

