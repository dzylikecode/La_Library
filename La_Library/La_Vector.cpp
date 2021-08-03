#include "La_Vector.h"

/// <summary>
/// ���ò�̫�Ծ�����Ҫ����������ݲ��Բ���
/// this function computes the distance from 0,0 to x,y with 3.5% error
/// </summary>
int DistanceFast(int x, int y)
{
	/// ����̩��չ������
	/// ������ y �ϴ�
	/// sqrt(x^2 + y^2) = y * ( 1 + (x / y)^2 ) ^(1/2) 
	/// ���� = y * ( 1 + (x / y)^ 2
	x = abs(x);
	y = abs(y);

	int mn = min(x, y);
	//������ y �ϴ�
	//-> 1 + x/y - 1/2 *x/y - 1/4 *x/y + 1/16 *x/y
	//-> 1 - 11/16 * u
	return (x + y - (mn >> 1) - (mn >> 2) + (mn >> 4));

	//һ�²��Թ� 100 * 100 ��������� ��� ��� 6%
	/*for (int i = 0; i < 10; i++)
	{
		REAL x = rand() % 100;
		REAL y = rand() % 100;
		REAL r1 = Distance_2D(x, y);
		REAL r2 = (int)Fast_Distance_2D((int)x, (int)y);
		if (fabs(r1) < 1e-8)
		{
			Write_In_Console(ERR, "real = 0\n");
			continue;
		}
		REAL err = fabs(r1 - r2) * 100 / r1;
		Write_In_Console(WARN, "real = %f, cal = %f, err = %f %%\n", r1, r2, err);
	}*/
}


REAL DistanceFast(REAL x, REAL y)
{
	// this function computes the distance from 0,0 to x,y with 3.5% error

	x = fabs(x);
	y = fabs(y);

	float mn = min(x, y);

	return(x + y - (mn / 2) - (mn / 4) + (mn / 8));

} 

/// <summary>
/// compute distance with 8% error
/// </summary>
REAL DistanceFast(REAL x, REAL y, REAL z)
{
	//����ʮ��������������
	int absx = fabs(x) * 1024;
	int absy = fabs(y) * 1024;
	int absz = fabs(z) * 1024;

	//����ֵ��
	int temp;

	if (absy < absx)
	{
		SWAP(absx, absy, temp);
	}

	if (absz < absy)
	{
		SWAP(absy, absz, temp);
	}

	if (absy < absx)
	{
		SWAP(absx, absy, temp);
	}

	int dist = absz + 11 * (absy >> 5) + (absx >> 2);

	//����Ϊԭ���Ĵ�С
	return (REAL)((dist) >> 10);

	// 100 * 100 * 100 ��������� ������ 8%
	/*for (int i = 0; i < 10; i++)
	{
		REAL x = rand() % 100;
		REAL y = rand() % 100;
		REAL z = rand() % 100;
		REAL r1 = Distance_3D(x, y, z);
		REAL r2 = (int)Fast_Distance_3D((int)x, (int)y, (int)z);
		if (fabs(r1) < 1e-8)
		{
			Write_In_Console(ERR, "real = 0\n");
			continue;
		}
		REAL err = fabs(r1 - r2) * 100 / r1;
		Write_In_Console(WARN, "real = %f, cal = %f, err = %f %%\n", r1, r2, err);
	}*/
}