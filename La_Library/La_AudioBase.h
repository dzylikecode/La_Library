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
*  ����ʱ��: 2021/05/05 20:46:16
*  ����޸�:
*
*  ��    ��: ���Էḻ�Լ���֪ʶ����
*			 frequency: ������Ϣ�� Shannon's THeorem
*						�����ʵ� Nyquist frequency
*			 Amplitude: ���Ƚ����ȣ���Ϊ����ɢ����ֵ�������磬��Ҫת��Ϊģ��
*						��Ϊ�����ģ������
*			 ��Ƶĸ���� DirectDraw ���ƣ���һ�������壬��ʾ���ڻ������ŵģ�
*			 ��һ���������壬�ǵȴ������ŵ�
*
*
*			 ������������������Բ���Ҫ�����ֵ䣬���ǿ��Թ��� �ﾳ �� paragraphs
*
***************************************************************************************
*/
#pragma once
#include "La_Windows.h"
#include <windowsx.h> 
#include <mmsystem.h>
#include <dsound.h>
#include <dmusicc.h>
#include <dmusics.h>
#include <dmksctrl.h>
#include "dmusici.h"


//DSBCAPS_CTRLPAN      //ƽ�����
//DSBCAPS_CTRLVOLUME   //��������
//DSBCAPS_CTRLFREQUENCY //Ƶ�ʿ���
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
		void* buffer1;			//���� amplitude ���ֽ���ת��λ UCHAR USHORT ��������
		DWORD bufferLen1;		//���ֽ�����������ʹ�� USHOT ��ʱ�� Ӧ�� buffer1Len >> 1
		void* buffer2;
		DWORD bufferLen2;
		DWORD bufferSize;		// second * rate * anplitudeSpan * channels
		DWORD rate;				//Ƶ��
		WORD waveChannels;		//����������������
		WORD amplitudeSpan;		//���ֽڣ��������Ϊɫ��һ���Ķ��� 8 = 1, 16 = 2
	public:
		//11025 8 / 22050 16
		SOUND(void) :lpddS(nullptr), buffer1(nullptr), bufferLen1(0), buffer2(nullptr), bufferLen2(0), bufferSize(0), rate(11025), waveChannels(1), amplitudeSpan(8) {};
		
		void clear() { if (lpddS) { stop(); lpddS->Release(); lpddS = nullptr; } }
		~SOUND() { clear(); }
		bool create(DWORD size, DWORD dwRate = 11025, WORD iWaveChannels = 1, WORD iAmplitudeSpan = 8);
		bool create(LPCTCH fileName, ...);
		void play(bool bLoop) { lpddS->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0); }
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
		DWORD getRate() { return rate; }
		WORD  getChannels() { return waveChannels; }
		WORD  getAplitude() { return amplitudeSpan; }
		bool  isPlaying()
		{
			DWORD status;
			lpddS->GetStatus(&status);
			return (status == DSBSTATUS_PLAYING) ? true : false;
		}
	public: 
		friend void BeginComposeOn(SOUND* sound);
		friend void EndComposeOn(SOUND* sound);
	public:
		//������˥���Ĵ�С��0Ϊԭ����С (DSBVOLUME_MAX)��DSBVOLUME_MIN ��ʾ˥������С
		void setVolume(int volume) { lpddS->SetVolume(volume); }
		//�ı�طŵ�Ƶ��
		void setFrequency(DWORD fre) { lpddS->SetFrequency(fre); }
		/// <summary>
		/// ��������������ǿ��
		/// </summary>
		/// -10000��������˥��100dB���������������ǿ����֮��10000��������˥��100dB
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
		void clear()
		{
			if (dm_segment)
			{
				//����ж������
				dm_segment->SetParam(GUID_Unload, -1, 0, 0, (void*)dm_perf);
				dm_segment->Release();
				dm_segment = nullptr;
				dm_segstate = nullptr;
			}
		}
		~MUSIC() { clear(); }
	public:
		bool create(LPCTCH fileName, ...);
	public:
		bool isPlaying();
		bool play();
		bool stop();
	};


	void InitializeAudio(HWND hwnd);
}