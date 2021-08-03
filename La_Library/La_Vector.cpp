#include "La_Vector.h"

/// <summary>
/// 觉得不太对劲，需要重新设计数据测试测试
/// this function computes the distance from 0,0 to x,y with 3.5% error
/// </summary>
int DistanceFast(int x, int y)
{
	/// 利用泰勒展开即有
	/// 不妨设 y 较大
	/// sqrt(x^2 + y^2) = y * ( 1 + (x / y)^2 ) ^(1/2) 
	/// 近似 = y * ( 1 + (x / y)^ 2
	x = abs(x);
	y = abs(y);

	int mn = min(x, y);
	//不妨设 y 较大
	//-> 1 + x/y - 1/2 *x/y - 1/4 *x/y + 1/16 *x/y
	//-> 1 - 11/16 * u
	return (x + y - (mn >> 1) - (mn >> 2) + (mn >> 4));

	//一下测试过 100 * 100 的随机里面 误差 最大 6%
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
	//扩大十倍，用整数运算
	int absx = fabs(x) * 1024;
	int absy = fabs(y) * 1024;
	int absz = fabs(z) * 1024;

	//绝对值化
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

	//返回为原来的大小
	return (REAL)((dist) >> 10);

	// 100 * 100 * 100 的随机里面 误差最大 8%
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