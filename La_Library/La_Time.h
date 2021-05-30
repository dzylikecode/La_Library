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
	UINT get(){ return theFrame; }
	void adjust()
	{
		UINT curFrame = (UINT)clock.recordFrequency();
		//֡������������
		//������� default ������Խ�����
		//Ҫ��Ȼ��ֻ��ʹ�õ�ǰ��֡��
		//0 ��ʹ�õ�ǰ��֡��
		if (defaultFrame == 0 || curFrame <= defaultFrame)
		{
			theFrame = curFrame;
			return;
		}
		else
		{
			theFrame = defaultFrame;
		}

		UINT ms = defaultDt - 1000 / curFrame;
		Sleep(ms); //���Ҳ�׷��ʲô�������� �������Ƶ� sleep ̫ռ��Դ��
	}
};
