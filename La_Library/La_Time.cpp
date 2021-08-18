#include "La_Time.h"


namespace TIMEMaster
{
	LARGE_INTEGER tickPerSecond = { 0 };
	TIMEGlobal timeGlobal;

	void sleep(int ms)
	{
		if (ms == 0) return;
		LARGE_INTEGER start, end;

		//��ȡ��ʼʱ��
		QueryPerformanceCounter(&start);
		while (1)
		{
			//��õ�ǰ��ʱ��
			QueryPerformanceCounter(&end);
			//if ((double(end.QuadPart - start.QuadPart) / double(tickPerSecond.QuadPart)) > (double(ms) / 1000.0))
			if ((end.QuadPart - start.QuadPart) * 1000 > ms * tickPerSecond.QuadPart) //��ʽ�ı���
			{
				return;
			}
		}
	}
}


namespace
{
	int monthdays[2][13] = { {0,31,28,31,30,31,30,31,31,30,31,30,31},
						{0,31,29,31,30,31,30,31,31,30,31,30,31} }; 	//ÿ���µ�����
}


namespace LADZY
{
	void DATE::next()
	{
		int leap = 0;        	//����Ƿ������꣬��ʼ����pd->year�������� 
		if (IsLeapYear(year)) 	//�ж�pd��ָ������һ���Ƿ������꣬����ǣ���leap=1 
			leap = 1;
		if (day < monthdays[leap][month])	//���pd��ָ���ڲ�����ĩ
			day++;                        	//��һ������ֻ�޸�pd->day
		else if (month < 12)
		{  	//���pd��ָ��������ĩ����������ĩ����һ������Ϊ����1��
			month++;
			day = 1;
		}
		else
		{//���pd��ָ��������ĩ������һ�����ڱ�Ϊ��һ���1��1��
			year++;
			month = 1;
			day = 1;
		}
	}

	int DATE::distance()
	{
		int sum = 0, leap = 0;   	//�ֱ���dd.year�����������Ƿ�Ϊ����

		if (IsLeapYear(year))
			leap = 1;
		for (int i = 1; i < month; i++) 	//��1����m-1�µ�������
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
		{//ͨ��ѭ��ִ��days��ȥy��������������������ڵ����
			i -= yeardays;
			y++;
			yeardays = 365;
			if (IsLeapYear(y))     yeardays = 366;
		}
		int leap = 0;
		if (IsLeapYear(y))     leap = 1;
		m = 1;
		while (i > monthdays[leap][m])
		{//ͨ��ѭ��ִ��y��m�������������������ڵ��·�
			i -= monthdays[leap][m];
			m++;
		}
		year = y; 	//�ֱ�Ϊ�������ڵ��ꡢ�¡���
		month = m;
		day = i;
	}

	void DATE::back()
	{
		int leap = 0;          	//����Ƿ������꣬����pd->year�������� 
		if (IsLeapYear(year))   	//�ж�pd��ָ������һ���Ƿ����꣬����leap=1 
			leap = 1;
		if (day != 1)     	//���pd��ָ���ڲ���1�� 
			day--;      	//pd�����Ӽ�1��Ϊǰһ������ 
		else if (month > 1)
		{//���pd��ָ���ڲ���1�£���ĳ��1��ǰһ������Ϊǰһ�µ���ĩ 
			month--;
			day = monthdays[leap][month];
		}
		else
		{//���pd��ָ������ĳ��1��1�գ���ǰһ������Ϊǰһ��12��31��
			year--;
			month = 12;
			day = 31;
		}
	}

	void DATE::back(int i)
	{
		int remainderDay = distance();
		month = 1; day = 1;//�� 1 �� 1 ��Ϊ��׼
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