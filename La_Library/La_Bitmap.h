#pragma once
#include "La_GraphicBase.h"


#define BITMAP_ID            (*(WORD*)"BM") // universal id for a bitmap

namespace GRAPHIC
{
	class BITMAP
	{
	private:
		BITMAPFILEHEADER bitmapfileheader; //描述文件结构的信息
		BITMAPINFOHEADER bitmapinfoheader;//描述图片的信息，包含调色板
		COLOR* buffer;
	public:
		bool load(LPCTSTR fileName);
		LONG getWidth()const { return bitmapinfoheader.biWidth; }
		LONG getHeight()const { return bitmapinfoheader.biHeight; }
		LONG getSizeByte()const { return bitmapinfoheader.biSizeImage; }
		void flip() { Flip((BYTE*)buffer, bitmapinfoheader.biWidth * (bitmapinfoheader.biBitCount / 8), bitmapinfoheader.biHeight); }
		void clear()
		{
			if (buffer)
			{
				delete[] buffer;
				buffer = nullptr;
			}
		}
		~BITMAP() { clear(); }
	};
}