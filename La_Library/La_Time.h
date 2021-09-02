/*
***************************************************************************************
*  ��    ��: 
*
*  ��    ��: LaDzy
*
*  ��    ��: mathbewithcode@gmail.com
*
*  ���뻷��: Visual Studio 2019
*
*  ����ʱ��: 2021/05/31 0:29:52
*  ����޸�: 
*
*  ��    ��: 
*
***************************************************************************************
*/
#pragma once
#include "La_WindowsBase.h"
#include <time.h>
#define TEST_FUNCTION(result,num,function)		{LARGE_INTEGER begin_time, freq, end_time; \
												QueryPerformanceFrequency(&freq); \
												QueryPerformanceCounter(&begin_time); \
												for (long test0 = 0; test0 < num; test0++) \
												{ \
													function; \
												} \
												QueryPerformanceCounter(&end_time); \
												result = ((double)(end_time.QuadPart - begin_time.QuadPart) * 1000.0) / (double)(freq.QuadPart); \
												}


namespace TIMEMaster
{
	extern LARGE_INTEGER tickPerSecond;// ÿ��ʱ�ӵδ�����----->�ǳ�ע��������ǳ���������Ϊ 0
	class TIMEGlobal
	{
	public:
		TIMEGlobal()
		{
			if (!QueryPerformanceFrequency(&tickPerSecond))
			{
				MessageErr(TEXT("Time Component Can't Work"));
			}
		}
	};
	void sleep(int ms);
}



class CLOCK
{
private:
	LARGE_INTEGER baseTick;		// ��ʱʹ��
	LARGE_INTEGER curTick;
	LARGE_INTEGER curFreq;
public:
	CLOCK() :baseTick{ 0 }, curTick{ 0 }{};
	void start()
	{
		
		QueryPerformanceCounter(&baseTick);
		curFreq = curTick = baseTick;
	}
	
	double recordABS()//���غ���
	{
		LARGE_INTEGER newTick;
		QueryPerformanceCounter(&newTick);
		return (1000.0 * (newTick.QuadPart - baseTick.QuadPart)) / TIMEMaster::tickPerSecond.QuadPart;
	}
	double record()
	{
		LARGE_INTEGER lastTick = curTick;
		QueryPerformanceCounter(&curTick);
		return (1000.0 * (curTick.QuadPart - lastTick.QuadPart)) / TIMEMaster::tickPerSecond.QuadPart;
	}
	double recordFrequencyABS()//��������λ
	{
		LARGE_INTEGER newTick;
		QueryPerformanceCounter(&newTick);
		return TIMEMaster::tickPerSecond.QuadPart / (double(newTick.QuadPart - baseTick.QuadPart));
	}
	double recordFrequency()
	{
		LARGE_INTEGER lastTick = curFreq;
		QueryPerformanceCounter(&curFreq);
		return TIMEMaster::tickPerSecond.QuadPart / (double(curFreq.QuadPart - lastTick.QuadPart));
	}
};

class FRAMECounter
{
private:
	UINT theFrame;		// ��ǰ֡��
	UINT defaultFrame;	// 0�������п���
	UINT defaultDt;		// ÿ֡��ʱ���
	CLOCK clock;
	double secondPerFrame;
	void Calculate()
	{
		defaultDt = defaultFrame ? UINT((1000.0 / defaultFrame) + 0.5) : 0;
	}
public:
	FRAMECounter(UINT frame = 60) :defaultFrame(frame), theFrame(0)
	{
		Calculate();
	}
	void start() { clock.start(); }
	void set(UINT fps)
	{
		defaultFrame = fps;
		Calculate();
	}
	UINT get()const { return theFrame; }
	double getTime()const { return secondPerFrame; }
	void adjust()
	{
		double tempCal = clock.recordFrequency();
		UINT curFrame = (UINT)tempCal;
		//֡������������
		//������� default ������Խ�����
		//Ҫ��Ȼ��ֻ��ʹ�õ�ǰ��֡��
		//0 ��ʹ�õ�ǰ��֡��
		if (defaultFrame == 0 || curFrame <= defaultFrame)
		{
			theFrame = curFrame;
			secondPerFrame = 0.001 / tempCal;
			return;
		}
		else
		{
			theFrame = defaultFrame;
			secondPerFrame = 1.0 / theFrame;
		}

		UINT ms = defaultDt - 1000 / curFrame;
		Sleep(ms); //���Ҳ�׷��ʲô�������� �������Ƶ� sleep ̫ռ��Դ��
		//TIMEMaster::sleep(ms / 2);
	}
};


inline bool IsLeapYear(int year)
{
	return year % 400 == 0 || year % 4 == 0 && year % 100 != 0;
}

namespace LADZY
{
	class TIMERecord
	{
	private:
		time_t nowTime;
		struct tm* tp;
	public:
		void inquire() 
		{ 
			nowTime = time(nullptr); 
			tp = localtime(&nowTime);//Ĭ��
		}
		//����1970��1��1��0ʱ������
		time_t getAbs()const { return nowTime; }
		void switchLocal()
		{
			tp = localtime(&nowTime);
		}
		void switchUTC()
		{
			tp = gmtime(&nowTime);
		}
		int  getYear()const { return tp->tm_year + 1900; }
		int  getMonth()const { return tp->tm_mon + 1; }
		int  getDay()const { return tp->tm_mday; }
		int  getHour()const { return tp->tm_hour; }
		int  getMin()const { return tp->tm_min; }
		int  getSec()const { return tp->tm_sec; }
		char* getString()const { return asctime(tp); }
	};


	class DATE
	{
	private:
		int year, month, day;
	private:
		int distance();
		void calFromFirstDay(int i);
	public:
		void next();
		void next(UINT i)
		{
			i += distance();
			calFromFirstDay(i);
		}
		void back();
		void back(int i);
		void set(int outYear, int outMonth, int outDay) { year = outYear; month = outMonth; day = outDay; }
	};
}