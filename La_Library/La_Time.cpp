#include "La_Time.h"


namespace TIMEMaster
{
	LARGE_INTEGER tickPerSecond = { 0 };
	TIMEGlobal timeGlobal;

	void sleep(int ms)
	{
		if (ms == 0) return;
		LARGE_INTEGER start, end;

		//获取初始时间
		QueryPerformanceCounter(&start);
		while (1)
		{
			//获得当前的时间
			QueryPerformanceCounter(&end);
			//if ((double(end.QuadPart - start.QuadPart) / double(tickPerSecond.QuadPart)) > (double(ms) / 1000.0))
			if ((end.QuadPart - start.QuadPart) * 1000 > ms * tickPerSecond.QuadPart) //上式的变形
			{
				return;
			}
		}
	}
}
