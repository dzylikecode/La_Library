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


namespace
{
	int monthdays[2][13] = { {0,31,28,31,30,31,30,31,31,30,31,30,31},
						{0,31,29,31,30,31,30,31,31,30,31,30,31} }; 	//每个月的天数
}


namespace LADZY
{
	void DATE::next()
	{
		int leap = 0;        	//存放是否是闰年，初始假设pd->year不是闰年 
		if (IsLeapYear(year)) 	//判断pd所指日期那一年是否是闰年，如果是，则leap=1 
			leap = 1;
		if (day < monthdays[leap][month])	//如果pd所指日期不是月末
			day++;                        	//下一天日期只修改pd->day
		else if (month < 12)
		{  	//如果pd所指日期是月末，但不是年末，下一天日期为下月1日
			month++;
			day = 1;
		}
		else
		{//如果pd所指日期是年末，则下一天日期变为下一年的1月1日
			year++;
			month = 1;
			day = 1;
		}
	}

	int DATE::distance()
	{
		int sum = 0, leap = 0;   	//分别存放dd.year年后的天数、是否为闰年

		if (IsLeapYear(year))
			leap = 1;
		for (int i = 1; i < month; i++) 	//求1月至m-1月的总天数
		{
			sum += monthdays[leap][i];
		}
		sum += day;
		return sum;
	}

	void DATE::calFromFirstDay(int i)
	{
		int y, m, yeardays;

		y = year;
		yeardays = 365;
		if (IsLeapYear(y))     yeardays = 366;
		while (i > yeardays)
		{//通过循环执行days减去y年的天数，计算所求日期的年份
			i -= yeardays;
			y++;
			yeardays = 365;
			if (IsLeapYear(y))     yeardays = 366;
		}
		int leap = 0;
		if (IsLeapYear(y))     leap = 1;
		m = 1;
		while (i > monthdays[leap][m])
		{//通过循环执行y年m月天数，计算所求日期的月份
			i -= monthdays[leap][m];
			m++;
		}
		year = y; 	//分别为所求日期的年、月、日
		month = m;
		day = i;
	}

	void DATE::back()
	{
		int leap = 0;          	//存放是否是闰年，假设pd->year不是闰年 
		if (IsLeapYear(year))   	//判断pd所指日期那一年是否闰年，是则leap=1 
			leap = 1;
		if (day != 1)     	//如果pd所指日期不是1日 
			day--;      	//pd的日子减1即为前一天日期 
		else if (month > 1)
		{//如果pd所指日期不是1月，则某月1日前一天日期为前一月的月末 
			month--;
			day = monthdays[leap][month];
		}
		else
		{//如果pd所指日期是某年1月1日，则前一天日期为前一年12月31日
			year--;
			month = 12;
			day = 31;
		}
	}

	void DATE::back(int i)
	{
		int remainderDay = distance();
		month = 1; day = 1;//以 1 月 1 日为基准
		if (remainderDay >= i)
		{
			remainderDay -= i;
			next(remainderDay);
		}
		else
		{
			int yeardays = 365;
			do
			{
				year--;
				if (IsLeapYear(year))
					yeardays = 366;
				i -= yeardays;
			} while (i < 0);
			next(-i + remainderDay);
		}
	}
}