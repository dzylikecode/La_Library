#include "La_GraphicBase.h"
#include "La_Log.h"
#pragma comment(lib, "ddraw.lib")
namespace GRAPHIC
{
	bool Flip(BYTE* image, int bytes_per_line, int height)
	{
		BYTE* buffer;

		if (!(buffer = new BYTE[height * bytes_per_line]))
		{
			return false;
		}

		memcpy(buffer, image, height * bytes_per_line);

		//flip vertically
		for (int i = 0; i < height; i++)
		{
			memcpy(&image[((height - 1) - i) * bytes_per_line], &buffer[i * bytes_per_line], bytes_per_line);
		}

		delete[] buffer;

		return true;
	}


	LPDIRECTDRAW7		 lpdd7 = nullptr;

	
	

	//�Լ��������������������Ǻ󱸻���
	LPDIRECTDRAWSURFACE7 CreateSurface(int width, int height, DWORD colorKey, DWORD surfaceMemoryStyle)
	{
		DDSURFACEDESC2 ddsd;
		INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

		ddsd.dwHeight = height;
		ddsd.dwWidth = width;

		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | surfaceMemoryStyle;

		LPDIRECTDRAWSURFACE7 lpddsurface;
		if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsurface, nullptr)))
		{
			ERROR_MSG(ERR, TEXT("Create Surface Failed"));
			return nullptr;
		}

		DDCOLORKEY color_key;
		color_key.dwColorSpaceLowValue = colorKey;
		color_key.dwColorSpaceHighValue = colorKey;

		//�������ɫ�ʿռ䣬��Ҫ��־ SPACE
		//�������� alpha ���Ӳ�����ɫ�ʵ���
		//Ŀ��ɫ�ʼ����൱��դ��
		if (FAILED(lpddsurface->SetColorKey(DDCKEY_SRCBLT, &color_key)))
		{
			ERROR_MSG(ERR, TEXT("Surface Sets the Color Key Failed"));
		}

		return lpddsurface;
	}


	LPDIRECTDRAWCLIPPER SurfaceAttachClipper(LPDIRECTDRAWSURFACE7 lpddsurface, int num_rects, LPRECT clip_list)
	{
		LPDIRECTDRAWCLIPPER lpddclipper;
		//��������ü���
		//��һ��������Ϊ0
		if (FAILED(lpdd7->CreateClipper(0, &lpddclipper, nullptr)))
		{
			ERROR_MSG(ERR, TEXT("Create Clipper Failed!"));
			return nullptr;
		}

		//׼���������ü����ϣ���Ҫ�õ��������ڴ棬һ��������Ϣͷ��һ�����ǲü�����
		LPRGNDATA region_data;

		region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + num_rects * sizeof(RECT));

		//�Ѳü����ݸ��Ƶ�����������
		memcpy(region_data->Buffer, clip_list, num_rects * sizeof(RECT));

		//set up fields of header
		region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
		region_data->rdh.iType = RDH_RECTANGLES;
		region_data->rdh.nCount = num_rects;
		region_data->rdh.nRgnSize = num_rects * sizeof(RECT);

		//�ҵ��ü��б�Ĳ���������ͷ�ļ���
		//�ڲ�Ԥ������ô��
		region_data->rdh.rcBound.left = 64000;
		region_data->rdh.rcBound.top = 64000;
		region_data->rdh.rcBound.right = -64000;
		region_data->rdh.rcBound.bottom = -64000;
		//��ʼ���Ҳ���
		for (int i = 0; i < num_rects; i++)
		{
			region_data->rdh.rcBound.left = min(region_data->rdh.rcBound.left, clip_list[i].left);
			region_data->rdh.rcBound.right = max(region_data->rdh.rcBound.right, clip_list[i].right);
			region_data->rdh.rcBound.top = min(region_data->rdh.rcBound.top, clip_list[i].top);
			region_data->rdh.rcBound.bottom = max(region_data->rdh.rcBound.bottom, clip_list[i].bottom);
		}

		//�����ݷ���
		if (FAILED(lpddclipper->SetClipList(region_data, 0)))
		{
			ERROR_MSG(ERR, TEXT("Set Clipper List Failed!"));
			free(region_data);
			return nullptr;
		}

		// now attach the clipper to the surface
		if (FAILED(lpddsurface->SetClipper(lpddclipper)))
		{
			// release memory and return error
			ERROR_MSG(ERR, TEXT("Attach the Clipper to the Surface Failed!"));
			free(region_data);
			return(NULL);
		}

		//�ͷ�����
		free(region_data);
		//���ӿڷ��ظ�������
		return lpddclipper;
	}

	





}