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
		COLOR colPalette[PALETTE_NUM];
		bool  bColor8;
	private:
		void copy(const BITMAP& bm)
		{
			if (!buffer)
			{
				return;
			}
			bitmapfileheader = bm.bitmapfileheader;
			bitmapinfoheader = bm.bitmapinfoheader;
			buffer = new COLOR[bm.getWidth() * bm.getHeight()];
			memcpy(buffer, bm.buffer, bm.getSizeByte());
		}
	public:
		BITMAP() :bitmapfileheader{ 0 }, bitmapinfoheader{ 0 }, buffer(nullptr), bColor8(false){};
		BITMAP(const BITMAP& bm) :bitmapfileheader{ 0 }, bitmapinfoheader{ 0 }, buffer(nullptr){ copy(bm); }
		BITMAP& operator=(const BITMAP& bm)
		{
			if (this != &bm)
			{
				copy(bm);
			}
			return *this;
		}
		bool load(LPCTSTR fileName);
		LONG getWidth()const { return bitmapinfoheader.biWidth; }
		LONG getHeight()const { return bitmapinfoheader.biHeight; }
		LONG getSizeByte()const { return bitmapinfoheader.biSizeImage; }
		COLOR* getImage()const { return buffer; }
		bool isLoad()const { return buffer; }
		bool isColor8()const { return bColor8; }
		void getPalette(laPALETTE& outPalette);
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