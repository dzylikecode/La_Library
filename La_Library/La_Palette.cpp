#include "La_Palette.h"

namespace GRAPHIC
{
	laPALETTEMaster curPalette;
	void CopyPalette(laPALETTE& dest, laPALETTE& source)
	{
		for (int i = 0; i < PALETTE_NUM; i++)
		{
			dest[i] = source[i];
		}
	}
	bool laPALETTEMaster::load(LPCTCH fileName)
	{
		FILE* fp_file;
		ASTRING fName = TToA(fileName);
		if ((fp_file = fopen(fName, "r")) == nullptr)
		{
			return false;
		}

		//��ȡ�������ļ����е���Ϣ
		//����ķ�ʽ������ɫ���� �� ��־����ʽ
		for (int i = 0; i < PALETTE_NUM; i++)
		{
			BYTE red, green, blue, flag;
			fscanf(fp_file, "%d %d %d %d", &red, &green, &blue, &flag);
			palette[i] = RGB_DX(red, green, blue);
		}

		fclose(fp_file);

		return true;
	}
	bool laPALETTEMaster::save(LPCTCH fileName)
	{
		FILE* fp_file;
		ASTRING fName = TToA(fileName);
		if ((fp_file = fopen(fName, "r")) == nullptr)
		{
			return false;
		}

		//��ȡ�������ļ����е���Ϣ
		//����ķ�ʽ������ɫ���� �� ��־����ʽ
		for (int i = 0; i < PALETTE_NUM; i++)
		{
			BYTE red, green, blue, flag;
			red = GetRValue_DX(palette[i]);
			green = GetGValue_DX(palette[i]);
			blue = GetBValue_DX(palette[i]);
			flag = GetAValue_DX(palette[i]);
			fprintf(fp_file, "%d %d %d %d", &red, &green, &blue, &flag);
		}

		fclose(fp_file);

		return true;
	}
	void laPALETTEMaster::rotate(int beg, int end)//����ұ�����
	{
		int colorGap = end - beg + 1;
		COLOR temp = palette[end];
		for (int i = beg; i < end; i++)
		{
			palette[i + 1] = palette[i];
		}
		palette[beg] = temp;
	}
}