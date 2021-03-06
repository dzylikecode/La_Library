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
*  创建时间: 2021/05/05 20:46:16
*  最后修改:
*
*  简    介: 可以丰富自己的知识啦，
*			 frequency: 采样信息的 Shannon's THeorem
*						采样率的 Nyquist frequency
*			 Amplitude: 幅度解析度，因为是离散的数值，是数电，需要转化为模电
*						更为逼真的模拟声音
*			 设计的概念和 DirectDraw 相似，有一个主缓冲，表示正在混音播放的，
*			 有一个辅助缓冲，是等待被播放的
*
*
*			 声音本身就是流，所以不需要构造字典，但是可以构造 语境 嘛 paragraphs
*
***************************************************************************************
*/
#pragma once
#ifndef INITGUID
#define INITGUID  //使用一些预定义好的 guid
#endif
#include <windows.h>
#include <windowsx.h> 
#include <mmsystem.h>
#include <dsound.h>
#include <dmusicc.h>
#include <dmusics.h>
#include <dmksctrl.h>
#include "dmusici.h"
#include "La_Windows.h"
#include "La_Log.h"
#pragma comment(lib, "winmm.lib")

//DSBCAPS_CTRLPAN      //平衡控制
//DSBCAPS_CTRLVOLUME   //音量控制
//DSBCAPS_CTRLFREQUENCY //频率控制
//DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY)
// direct x 7.0 compatibility
#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME )
#endif

namespace AUDIO
{
	class SOUND
	{
	private:
		LPDIRECTSOUNDBUFFER lpddS;
		void* buffer1;			//根据 amplitude 的字节来转化位 UCHAR USHORT 等来解释
		DWORD bufferLen1;		//是字节数，所以在使用 USHOT 的时候， 应该 buffer1Len >> 1
		void* buffer2;
		DWORD bufferLen2;
		DWORD bufferSize;		// second * rate * anplitudeSpan * channels
		DWORD rate;				//频率
		WORD waveChannels;		//单声道或者立体声
		WORD amplitudeSpan;		//是字节，可以理解为色深一样的东西 8 = 1, 16 = 2
	public:
		//11025 8 / 22050 16
		SOUND(void) :lpddS(nullptr), buffer1(nullptr), bufferLen1(0), buffer2(nullptr), bufferLen2(0), bufferSize(0), rate(11025), waveChannels(1), amplitudeSpan(8) {};
		
		void clear() { if (lpddS) { stop(); lpddS->Release(); lpddS = nullptr; } }
		~SOUND() { clear(); }
		bool create(DWORD size, DWORD dwRate = 11025, WORD iWaveChannels = 1, WORD iAmplitudeSpan = 8);
		bool create(LPCTCH fileName, ...);
		void play(bool bLoop = false) { lpddS->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0); }
		void stop() { lpddS->Stop(); }
		/// <summary>
		/// Initially, the duplicate buffer will have the same parameters as the original buffer. 
		/// However, the application can change the parameters of each buffer independently, 
		/// and each can be played or stopped without affecting the other.
		/// The buffer memory is released when the last object referencing it is released.
		/// There is a known issue with volume levels of duplicated buffers.
		/// The duplicated buffer will play at full volume unless you change the volume to a different value than the original buffer's volume setting. 
		/// If the volume stays the same (even if you explicitly set the same volume in the duplicated buffer with a IDirectSoundBuffer8::SetVolume call), 
		/// the buffer will play at full volume regardless. To work around this problem, 
		/// immediately set the volume of the duplicated buffer to something slightly different than what it was, even if you change it one millibel.
		/// The volume may then be immediately set back again to the original desired value.
		/// </summary>
		void cloneFrom(const SOUND& source);
	public:
		DWORD getRate()const { return rate; }
		WORD  getChannels()const { return waveChannels; }
		WORD  getAplitude()const { return amplitudeSpan; }
		bool  isPlaying()
		{
			DWORD status;
			lpddS->GetStatus(&status);
			return (status == DSBSTATUS_PLAYING) ? true : false;
		}
		HRESULT getStatus()
		{
			DWORD status;
			lpddS->GetStatus(&status);
			return status;
		}
		void* getBuffer1() { return buffer1; }
		DWORD getLen1()const { return bufferLen1; }
		void* getBuffer2() { return buffer2; }
		DWORD getLen2()const { return bufferLen2; }
	public:
		void  copyContent(const BYTE* buffer, DWORD size)
		{
			CopyMemory(buffer1, buffer, bufferLen1);
			if (bufferLen1 < size)
				CopyMemory(buffer2, buffer + bufferLen1, bufferLen2);
		}
	public: 
		friend void BeginComposeOn(SOUND* sound);
		friend void EndComposeOn(SOUND* sound);
	public:
		//给的是衰减的大小，0为原本大小 (DSBVOLUME_MAX)，DSBVOLUME_MIN 表示衰减到最小
		void setVolume(int volume) { lpddS->SetVolume(volume); }
		//改变回放的频率
		void setFrequency(DWORD fre) { lpddS->SetFrequency(fre); }
		/// <summary>
		/// 设置左右声道的强弱
		/// </summary>
		/// -10000是右声道衰减100dB，听起来就是左边强，反之，10000是左声道衰减100dB
		void setPan(LONG pan) { lpddS->SetPan(pan); }
		void setPosition(DWORD curPosition = 0){ lpddS->SetCurrentPosition(curPosition); }

	};


	inline void BeginComposeOn(SOUND* sound)
	{
		if (sound->lpddS->Lock(0, sound->bufferSize, (LPVOID*)&sound->buffer1, &sound->bufferLen1, (LPVOID*)&sound->buffer2, &sound->bufferLen2, DSBLOCK_ENTIREBUFFER) != DS_OK)
		{
			ERROR_MSG(ERR, TEXT("Lock Sound Failed!"));
		}
	}
	inline void EndComposeOn(SOUND* sound)
	{
		if (sound->lpddS->Unlock(sound->buffer1, sound->bufferLen1, sound->buffer2, sound->bufferLen2) != DS_OK)
		{
			ERROR_MSG(ERR, TEXT("Unlock Sound Failed!"));
		}
	}

	class MUSIC
	{
	private:
		IDirectMusicSegment* dm_segment;  // the direct music segment
		IDirectMusicSegmentState* dm_segstate; // the state of the segment
	public:
		MUSIC() :dm_segment(nullptr), dm_segstate(nullptr) {};
		void clear();
		~MUSIC() { clear(); }
	public:
		bool create(LPCTCH fileName, LPCTCH workDirectory = nullptr);
	public:
		bool isPlaying();
		void play();
		void stop();
	};


	void InitializeAudio(HWND hwnd);
	void CloseAudio();

	class WAVE
	{
	private:
		TSTRING fileName;
		DWORD fdwSound;
	public:
		void load(int Macro)
		{
			fileName = MAKEINTRESOURCE(Macro);
			fdwSound = SND_RESOURCE;
		}
		void load(LPCTSTR outFileName)
		{
			fileName = outFileName;
			fdwSound = SND_FILENAME;
		}
		void play(bool bLoop = false, bool bAsync = true)
		{
			DWORD flag = fdwSound;
			if (bAsync)
			{
				flag |= SND_ASYNC;
			}
			else
			{
				flag |= SND_SYNC;
			}

			if (bLoop)
			{
				flag |= SND_LOOP;
			}

			PlaySound(fileName, NULL, flag);
		}
		void stop()
		{
			PlaySound(fileName, NULL, SND_PURGE);
		}
		void stopAll()
		{
			PlaySound(nullptr, NULL, SND_PURGE);
		}
	};
}