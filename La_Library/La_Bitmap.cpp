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

		//��ȡͷ�ļ���Ϣ
		_lread(file_handle, &bitmapfileheader, sizeof(BITMAPFILEHEADER));

		//�ж��Ƿ�Ϊ bitmap
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
			//8 λ��Ҫ��ȡ��ɫ��,��palette��
			_lread(file_handle, &palette, 256 * sizeof(PALETTEENTRY));

			//fix the reversed BGR RGBQUAD data format
			for (int i = 0; i < 256; i++)
			{
				//palette ����
				//���Ƕ���һ���ֽ�û�еߵ����ߵ��˵�ɫ��
				//���ڶ��ֽڵ���ɫ���򴢴�Ϊ bgr ��ʽ
				//���淽ʽ�Ǵ����ң����ϵ��£�Ҳ����ÿ����ɫ�ߵ��ˣ�����ɫ�Ĵ���û������
				colorConvert[i] = RGB_DX(palette[i].peBlue, palette[i].peGreen, palette[i].peRed);
			}
		}

		//�ҵ�ͼ����Ϣ�����
		_llseek(file_handle, -(int)(bitmapinfoheader.biSizeImage), SEEK_END);

		//ɾ��ԭ����ͼ��
		clear();

		UCHAR* temp_buffer = new UCHAR[bitmapinfoheader.biSizeImage];
		//���ͼ�εĴ�С����ͼƬ�Ĵ�С, ��������ð�
		UINT bitmap_image_square = bitmapinfoheader.biWidth * bitmapinfoheader.biHeight;

		buffer = new COLOR[bitmap_image_square];
		if (!temp_buffer || !buffer)
		{
			clear();
			delete[] temp_buffer;
			_lclose(file_handle);

			return false;
		}
		//��ȡ���ڴ浱��
		_lread(file_handle, temp_buffer, bitmapinfoheader.biSizeImage);


		if (bitmapinfoheader.biBitCount == 8)
		{
			for (ULONGLONG i = 0; i < bitmap_image_square; i++)
			{
				//ʵ���ϴ洢�Ǿ������� --> ��ɫ������
				buffer[i] = colorConvert[temp_buffer[i]];
			}
		}

		//if (bitmapinfoheader.biBitCount == 16)
		//{
		//	//convert each 24 bit RGB value into a 16 bit value
		//	for (int i = 0; i < bitmap_image_square; i++)
		//	{
		//		//�ߵ��ֽ��෴
		//		USHORT color = temp_buffer[2 * i + 1] << 8 | temp_buffer[2 * i];
		//		COLORREF tempColor;
		//		//		555
		//		if (bitmapinfoheader.biCompression == BI_RGB)
		//		{
		//			//extract RGB components ( !!! in BGR order !!! )
		//			//ע��������� ÿ���������� 8 
		//			//ÿ���ͱ�� 5 λ
		//			//// XRRRRRGG.GGGBBBBB 0x7C00 0x03E0 0x1F
		//			BYTE red = color & 0x7C00;
		//			BYTE green = color & 0x03E0;
		//			BYTE blue = color & 0x1F;

		//			BYTE alpha = color & 0x8000;

		//			red = (red * 255) / 31;
		//			green = (green * 255) / 31;
		//			blue = (blue * 255) / 31;

		//			//����������Ϣ
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
				//�ٴ�������������ɫ�� BGR ��ʽ
				buffer[i] = RGB_DX(temp_buffer[3 * i + 2], temp_buffer[3 * i + 1], temp_buffer[3 * i]);
			}
		}
		//���ת������Ҫ�޸���Ϣ
		bitmapinfoheader.biBitCount = 8 * sizeof(COLOR);
		bitmapinfoheader.biSizeImage = getWidth() * getHeight() * sizeof(COLOR);

		delete[] temp_buffer;

		_lclose(file_handle);

		flip();

		return true;
	}
}