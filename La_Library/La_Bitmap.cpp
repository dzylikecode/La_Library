#include "La_Bitmap.h"


namespace GRAPHIC
{
	bool BITMAP::load(LPCTSTR fileName)
	{
		HFILE file_handle;
		OFSTRUCT file_data = { 0 };

		if ((file_handle = OpenFile(TToA(fileName), &file_data, OF_READ)) == HFILE_ERROR)
		{
			return false;
		}

		//读取头文件信息
		_lread(file_handle, &bitmapfileheader, sizeof(BITMAPFILEHEADER));

		//判断是否为 bitmap
		if (bitmapfileheader.bfType != BITMAP_ID)
		{
			_lclose(file_handle);

			return false;
		}

		_lread(file_handle, &bitmapinfoheader, sizeof(BITMAPINFOHEADER));
		PALETTEENTRY palette[256];
		DWORD colorConvert[256];
		if (bitmapinfoheader.biBitCount == 8)
		{
			//8 位需要读取调色板,到palette中
			_lread(file_handle, &palette, 256 * sizeof(PALETTEENTRY));

			//fix the reversed BGR RGBQUAD data format
			for (int i = 0; i < 256; i++)
			{
				//palette 反的
				//但是对于一个字节没有颠倒，颠倒了调色板
				//对于多字节的颜色，则储存为 bgr 格式
				//储存方式是从左到右，从上到下，也就是每行颜色颠倒了，但颜色的次序没有问题
				colorConvert[i] = RGB_DX(palette[i].peBlue, palette[i].peGreen, palette[i].peRed);
			}
		}

		//找到图像信息的入口
		_llseek(file_handle, -(int)(bitmapinfoheader.biSizeImage), SEEK_END);

		//删除原来的图像
		clear();

		UCHAR* temp_buffer = new UCHAR[bitmapinfoheader.biSizeImage];
		//获得图形的大小而非图片的大小, 担心溢出好吧
		UINT bitmap_image_square = bitmapinfoheader.biWidth * bitmapinfoheader.biHeight;

		buffer = new COLOR[bitmap_image_square];
		if (!temp_buffer || !buffer)
		{
			clear();
			delete[] temp_buffer;
			_lclose(file_handle);

			return false;
		}
		//读取到内存当中
		_lread(file_handle, temp_buffer, bitmapinfoheader.biSizeImage);


		if (bitmapinfoheader.biBitCount == 8)
		{
			for (ULONGLONG i = 0; i < bitmap_image_square; i++)
			{
				//实际上存储是就是索引 --> 调色板索引
				buffer[i] = colorConvert[temp_buffer[i]];
			}
		}

		//if (bitmapinfoheader.biBitCount == 16)
		//{
		//	//convert each 24 bit RGB value into a 16 bit value
		//	for (int i = 0; i < bitmap_image_square; i++)
		//	{
		//		//高低字节相反
		//		USHORT color = temp_buffer[2 * i + 1] << 8 | temp_buffer[2 * i];
		//		COLORREF tempColor;
		//		//		555
		//		if (bitmapinfoheader.biCompression == BI_RGB)
		//		{
		//			//extract RGB components ( !!! in BGR order !!! )
		//			//注意这个缩放 每个分量除以 8 
		//			//每个就变成 5 位
		//			//// XRRRRRGG.GGGBBBBB 0x7C00 0x03E0 0x1F
		//			BYTE red = color & 0x7C00;
		//			BYTE green = color & 0x03E0;
		//			BYTE blue = color & 0x1F;

		//			BYTE alpha = color & 0x8000;

		//			red = (red * 255) / 31;
		//			green = (green * 255) / 31;
		//			blue = (blue * 255) / 31;

		//			//保留掩码信息
		//			tempColor = RGB(red, green, blue) | (alpha << 24);
		//		}
		//		//		565
		//		if (bitmapinfoheader.biCompression == BI_BITFIELDS)
		//		{
		//			// RRRRRGGG.GGGBBBBB 0xF800 0x07E0 0x1F
		//			BYTE red = color & 0xF800;
		//			BYTE green = color & 0x07E0;
		//			BYTE blue = color & 0x1F;

		//			red = (red * 255) / 31;
		//			green = (green * 255) / 63;
		//			blue = (blue * 255) / 31;

		//			tempColor = RGB(red, green, blue);
		//		}

		//		buffer[i] = color;
		//	}

		//}


		if (bitmapinfoheader.biBitCount == 24)
		{

			for (ULONGLONG i = 0; i < bitmap_image_square; i++)
			{
				//再次申明，它的颜色是 BGR 格式
				buffer[i] = RGB_DX(temp_buffer[3 * i + 2], temp_buffer[3 * i + 1], temp_buffer[3 * i]);
			}
		}
		//完成转化，需要修改信息
		bitmapinfoheader.biBitCount = 8 * sizeof(COLOR);
		bitmapinfoheader.biSizeImage = getWidth() * getHeight() * sizeof(COLOR);

		delete[] temp_buffer;

		_lclose(file_handle);

		flip();

		return true;
	}
}