#include "La_AudioBase.h"
#include "La_Log.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")

#define INIT_STRUCT(st) {memset(&st, 0, sizeof(st)); st.dwSize = sizeof(st);}

// Convert from MultiByte format to Unicode using the following macro:
#define MULTI_TO_WIDE( x,y )  MultiByteToWideChar( CP_ACP,MB_PRECOMPOSED, y,-1,x,_MAX_PATH)


namespace
{
	LPDIRECTSOUND lpds = nullptr;
	DWORD soundFlag = DSBCAPS_CTRLDEFAULT | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;

	IDirectMusicPerformance* dm_perf = nullptr;
	IDirectMusicLoader* dm_loader = nullptr;
}

namespace AUDIO
{
	//声道：　单声道、双声道
	//对于单声道声音文件，采样数据为八位的短整数（short int 00H - FFH）；
	//对于双声道立体声声音文件，每次采样数据为一个16位的整数（int），高八位(左声道)和低八位(右声道)分别代表两个声道
	//采样位数：8位、16位（2 ^ 8与2 ^ 16）
	//采样频率：声音信号在“模→数”转换过程中单位时间内采样的次数。采样值是指每一次采样周期内声音模拟信号的积分值。
	//22.05KHz、44.1KHz、48KHz

	/// <summary>
	/// Direct music 是完全 COM 化的组件，必须使用 COM 方法，且不需要 lib
	/// 初始化的时候会检测 sound 是否初始化，因为要用到 sound ，
	/// 如果没有初始化，就会自己在内部创建一个
	/// 因而同时使用两者的时候要求 先 initial sound then music
	/// </summary>
	bool MusicInit(HWND hwnd)
	{
		if (FAILED(CoInitialize(nullptr)))
		{
			ERROR_MSG(ERR, TEXT("COM Initialize failed!"));
			return false;
		}

		//创建对象
		//内部会创建一个 IDirectMusic
		if (FAILED(CoCreateInstance(CLSID_DirectMusicPerformance, nullptr, CLSCTX_INPROC, IID_IDirectMusicPerformance, (LPVOID*)&dm_perf)))
		{
			ERROR_MSG(ERR, TEXT("COM Create Instance failed!"));
			return false;
		}

		//initialize the performance, check if direct sound is on-line. if so,
		//use the direct sound object to initialize the COM, 
		//otherwise create a new one
		// 非显示创建 nullptr
		if (FAILED(dm_perf->Init(nullptr, lpds, hwnd)))
		{
			ERROR_MSG(ERR, TEXT("Music Initialize failed!"));

			return false;
		}

		//添加端口
		//使用默认的软件合成器
		if (FAILED(dm_perf->AddPort(nullptr)))
		{
			ERROR_MSG(ERR, TEXT("Music Add Port failed!"));
			return false;
		}
		//创建一个文件加载对象
		if (FAILED(CoCreateInstance(CLSID_DirectMusicLoader, nullptr, CLSCTX_INPROC, IID_IDirectMusicLoader, (LPVOID*)&dm_loader)))
		{
			ERROR_MSG(ERR, TEXT("Music Loader Create failed!"));
			return false;
		}

		return true;
	}
	void MUSICClose()
	{
		if (dm_perf)
		{
			dm_perf->Stop(nullptr, nullptr, 0, 0);
		}
		// If there is any music playing, stop it. This is 
		// not really necessary, because the music will stop when
		// the instruments are unloaded or the performance is    
		// closed down.

		if (dm_perf)
		{
			dm_perf->CloseDown();
			dm_perf->Release();
		}

		if (dm_loader)
		{
			dm_loader->Release();
		}

		//release COM
		CoUninitialize();
	}

	void InitializeAudio(HWND hwnd)
	{
		//创建 COM 对象，一个 COM 对象对应一个硬件 （符合 COM 即插即用的思维）
		//多个声卡需要多个 COM 对象
		//缺省就是默认
		if (DirectSoundCreate(nullptr, &lpds, nullptr) != DS_OK)
		{
			ERROR_MSG(ERR, TEXT("Create COM DirectX Sound Failed!"));
			return;
		}
		// set cooperation level to normal priority
		//设置协作等级
		//有些方式可以看看，
		//This level has the smoothest multitasking and resource-sharing behavior,
		//but because it does not allow the primary buffer format to change, 
		//output is restricted to the default 44kHz, 16-bit, stereo format.
		if (lpds->SetCooperativeLevel(hwnd, DSSCL_NORMAL) != DS_OK)
		{
			ERROR_MSG(ERR, TEXT("Create COM DirectX Sound Failed!"));
			return;
		}

		MusicInit(hwnd);
	}

	void CloseAudio()
	{
		MUSICClose();
		if (lpds)
		{
			lpds->Release();
			lpds = nullptr;
		}
	}

	bool SOUND::create(DWORD size, DWORD dwRate /* = 11025 */, WORD iWaveChannels /* = 1 */, WORD iAmplitudeSpan /* = 8 */)
	{
		clear();
		bufferSize = size;
		waveChannels = iWaveChannels;
		rate = dwRate;
		amplitudeSpan = iAmplitudeSpan;

		WAVEFORMATEX pcmwf = { 0 };
		pcmwf.wFormatTag = WAVE_FORMAT_PCM;  //pulse code modulation
		pcmwf.nChannels = waveChannels;//单声道或双声道

		//Sample rate, in samples per second (hertz). 
		//If wFormatTag is WAVE_FORMAT_PCM, then common values for nSamplesPerSec are 8.0 kHz, 11.025 kHz, 22.05 kHz, and 44.1 kHz. 
		//For non-PCM formats, this member must be computed according to the manufacturer's specification of the format tag.
		pcmwf.nSamplesPerSec = rate;
		pcmwf.wBitsPerSample = amplitudeSpan;

		pcmwf.nBlockAlign = pcmwf.nChannels * (pcmwf.wBitsPerSample >> 3);
		pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;

		pcmwf.cbSize = 0;

		DSBUFFERDESC dsbd;
		INIT_STRUCT(dsbd);

		dsbd.dwFlags = soundFlag;
		dsbd.dwBufferBytes = bufferSize;
		dsbd.lpwfxFormat = &pcmwf;

		if (FAILED(lpds->CreateSoundBuffer(&dsbd, &lpddS, nullptr)))
		{
			ERROR_MSG(ERR, TEXT("Create Sound Buffer Failed!"));
			return false;
		}
		return true;
	}
	bool SOUND::create(LPCTCH fileName, ...)
	{
		clear();
		TCHAR path[MAX_PATH];
		GetVariableArgument(path, MAX_PATH, fileName);
		// chunk
		MMCKINFO parent, child;

		//set up chunk info structure
		parent.ckid = (FOURCC)0;
		parent.cksize = 0;
		parent.fccType = (FOURCC)0;
		parent.dwDataOffset = 0;
		parent.dwFlags = 0;

		//copy data
		child = parent;

		HMMIO hwav;
		//打开文件
		if ((hwav = mmioOpen(path, nullptr, MMIO_READ | MMIO_ALLOCBUF)) == NULL)
		{
			ERROR_MSG(ERR, TEXT("Can Not Open WAV File"));
			return false;
		}

		//descend into the RIFF
		parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

		if (mmioDescend(hwav, &parent, NULL, MMIO_FINDRIFF))
		{
			ERROR_MSG(ERR, TEXT("Can Not Descend Parent Part"));
			mmioClose(hwav, 0);

			//no wave section
			return false;
		}

		//descend to the WAVEfmt
		child.ckid = mmioFOURCC('f', 'm', 't', ' ');

		if (mmioDescend(hwav, &child, &parent, 0))
		{
			ERROR_MSG(ERR, TEXT("Can Not Descend Child Part"));
			mmioClose(hwav, 0);

			//no format section
			return false;
		}

		WAVEFORMATEX wfmtx;

		if (mmioRead(hwav, (HPSTR)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx))
		{
			ERROR_MSG(ERR, TEXT("Not A Format WAV File"));
			mmioClose(hwav, 0);

			//no wave format data
			return false;
		}

		if (wfmtx.wFormatTag != WAVE_FORMAT_PCM)
		{
			ERROR_MSG(ERR, TEXT("Data Format is Not a PCM!"));
			mmioClose(hwav, 0);

			//not the right data format
			return false;
		}

		if (mmioAscend(hwav, &child, 0))
		{
			ERROR_MSG(ERR, TEXT("Ascend up one level Failed"));

			mmioClose(hwav, 0);

			//couldn't ascend
			return false;
		}

		//descend to the data chunk
		child.ckid = mmioFOURCC('d', 'a', 't', 'a');

		if (mmioDescend(hwav, &child, &parent, MMIO_FINDCHUNK))
		{
			ERROR_MSG(ERR, TEXT("Can Not Find the Chunk"));
			mmioClose(hwav, 0);

			return false;
		}

		//now read the data and set up the direct sound buffer

		BYTE* snd_buffer = (BYTE*)malloc(child.cksize * sizeof(BYTE));

		mmioRead(hwav, (HPSTR)snd_buffer, child.cksize);

		mmioClose(hwav, 0);

		create(child.cksize, wfmtx.nSamplesPerSec, wfmtx.nChannels, wfmtx.wBitsPerSample);

		BeginComposeOn(this);
		CopyMemory(buffer1, snd_buffer, bufferLen1);
		CopyMemory(buffer2, snd_buffer + bufferLen1, bufferLen2);
		EndComposeOn(this);

		return true;
	}
	void SOUND::cloneFrom(const SOUND& source)
	{
		clear();
		bufferSize = source.bufferSize;
		rate = source.rate;
		waveChannels = source.waveChannels;
		amplitudeSpan = source.amplitudeSpan;
		buffer1 = source.buffer1;
		bufferLen1 = source.bufferLen1;
		buffer2 = source.buffer2;
		bufferLen2 = source.bufferLen2;

		lpds->DuplicateSoundBuffer(source.lpddS, &lpddS);
	}


	bool MUSIC::create(LPCTCH fileName, LPCTCH workDirectory/* = nullptr*/)
	{
		if (!workDirectory)
		{
			//找到文件所在的目录
			char szDir[MAX_PATH];
			WCHAR wszDir[MAX_PATH];

			//Gets the current working directory.
			if (_getcwd(szDir, MAX_PATH) == nullptr)
			{
				ERROR_MSG(ERR, TEXT("get current directory failed"));

				return false;
			}

			//统一转化为宽字符处理
			MULTI_TO_WIDE(wszDir, szDir);

			//tell the loader where to look for files
			if (FAILED(dm_loader->SetSearchDirectory(GUID_DirectMusicAllTypes, wszDir, false)))
			{
				ERROR_MSG(ERR, TEXT("Can Not Set Current Directory As Working Directory!"));
				return false;
			}
		}
		else
		{
			if (FAILED(dm_loader->SetSearchDirectory(GUID_DirectMusicAllTypes, TToW(workDirectory), false)))
			{
				ERROR_MSG(ERR, TEXT("Can Not Set Current Directory As Working Directory!"));
				return false;
			}
		}

		WSTRING wfilename = TToW(fileName);


		DMUS_OBJECTDESC ObjDesc;

		INIT_STRUCT(ObjDesc);
		ObjDesc.guidClass = CLSID_DirectMusicSegment;
		wcscpy(ObjDesc.wszFileName, wfilename);
		ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;

		// load the object and query it for the IDirectMusicSegment interface
		// This is done in a single call to IDirectMusicLoader::GetObject
		// note that loading the object also initializes the tracks and does 
		// everything else necessary to get the MIDI data ready for playback.

		IDirectMusicSegment* pSegment = nullptr;
		if (FAILED(dm_loader->GetObject(&ObjDesc, IID_IDirectMusicSegment, (LPVOID*)&pSegment)))
		{
			ERROR_MSG(ERR, TEXT("Can Not Set Current Directory As Working Directory!"));
			return false;
		}

		// ensure that the segment plays as a standard MIDI file
		// you now need to set a parameter on the band track
		// Use the IDirectMusicSegment::SetParam method and let 
		// DirectMusic find the track by passing -1 (or 0xFFFFFFFF) in the dwGroupBits method parameter.
		if (FAILED(pSegment->SetParam(GUID_StandardMIDIFile, -1, 0, 0, (LPVOID*)&dm_perf)))
		{
			ERROR_MSG(ERR, TEXT("Set Music Param Failed!"));
			return false;
		}

		// This step is necessary because DirectMusic handles program changes and 
		// bank selects differently for standard MIDI files than it does for MIDI 
		// content authored specifically for DirectMusic. 
		// The GUID_StandardMIDIFile parameter must be set before the instruments are downloaded. 

		// The next step is to download the instruments. 
		// This is necessary even for playing a simple MIDI file 
		// because the default software synthesizer needs the DLS data 
		// for the General MIDI instrument set
		// If you skip this step, the MIDI file will play silently.
		// Again, you call SetParam on the segment, this time specifying the GUID_Download parameter:

		if (FAILED(pSegment->SetParam(GUID_Download, -1, 0, 0, (void*)dm_perf)))
		{
			ERROR_MSG(ERR, TEXT("Set Downloaded Music Param Failed!"));
			return false;
		}


		dm_segment = pSegment;
		dm_segstate = nullptr;

		return true;
	}
	void MUSIC::clear()
	{
		if (dm_segment)
		{
			//首先卸载数据
			dm_segment->SetParam(GUID_Unload, -1, 0, 0, (void*)dm_perf);
			dm_segment->Release();
			dm_segment = nullptr;
			dm_segstate = nullptr;
		}
	}
	bool MUSIC::isPlaying() { return(dm_perf->IsPlaying(dm_segment, nullptr) == S_OK) ? true : false; }
	void MUSIC::play()
	{
		//play segment and force tracking of state variable
		dm_perf->PlaySegment(dm_segment, 0, 0, &dm_segstate);
	}
	void MUSIC::stop(){ dm_perf->Stop(dm_segment, nullptr, 0, 0); }

	class AUDIOMaster
	{
	public:
		~AUDIOMaster() { CloseAudio(); }
	}audioMaster;
}