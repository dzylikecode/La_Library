#pragma once
#include "La_GraphicBase.h"


#define BITMAP_ID            (*(WORD*)"BM") // universal id for a bitmap

namespace GRAPHIC
{
	class BITMAP
	{
	private:
		BITMAPFILEHEADER bitmapfileheader; //�����ļ��ṹ����Ϣ
		BITMAPINFOHEADER bitmapinfoheader;//����ͼƬ����Ϣ��������ɫ��
		COLOR* buffer;
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
		BITMAP() :bitmapfileheader{ 0 }, bitmapinfoheader{ 0 }, buffer(nullptr){};
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