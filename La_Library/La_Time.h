/*
***************************************************************************************
*  程    序: 
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/05/31 0:29:52
*  最后修改: 
*
*  简    介: 
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
	extern LARGE_INTEGER tickPerSecond;// 每秒时钟滴答声数----->非常注意的是它是除数，可能为 0
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
	LARGE_INTEGER baseTick;		// 计时使用
	LARGE_INTEGER curTick;
	LARGE_INTEGER curFreq;
public:
	CLOCK() :baseTick{ 0 }, curTick{ 0 }{};
	void start()
	{
		
		QueryPerformanceCounter(&baseTick);
		curFreq = curTick = baseTick;
	}
	
	double recordABS()//返回毫秒
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
	double recordFrequencyABS()//以秒做单位
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
	UINT theFrame;		// 当前帧数
	UINT defaultFrame;	// 0代表不进行控制
	UINT defaultDt;		// 每帧的时间差
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
		//帧率是愈大愈快
		//如果大于 default ，则可以降下来
		//要不然，只能使用当前的帧率
		//0 是使用当前的帧率
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
		Sleep(ms); //暂且不追求什么高性能了 不用自制的 sleep 太占资源了
	}
};
