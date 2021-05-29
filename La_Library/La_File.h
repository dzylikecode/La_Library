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
*  创建时间: 2021/05/20 17:21:10
*  最后修改: 
*
*  简    介: 
*
***************************************************************************************
*/

#pragma once
#include "La_Windows.h"
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <dbghelp.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

typedef NODE<WIN32_FIND_DATA>		fileNODE;
typedef LINKEDLIST<WIN32_FIND_DATA> winFILELIST;

class winFILE
{
public:
	enum SHAREMODE :DWORD
	{
		SHARER = GENERIC_READ,
		SHAREW = GENERIC_READ,
		EXCLUSIVE = 0,
		SHAREB = GENERIC_READ | GENERIC_READ
	};
private:
	TSTRING	tstrFileName;
	HANDLE  hFile;
public:

	winFILE(void) :hFile(INVALID_HANDLE_VALUE) {};

	//------------------以下不需要打开文件就可以操作----------------------------
	bool Delete(LPCTSTR fileName, bool bForce = false)//强制表明，只读的文件也删除
	{
		DWORD dwRet = GetFileAttributes(fileName);

		if (dwRet == INVALID_FILE_ATTRIBUTES)
			return false;
		if (bForce)
		{
			if (dwRet & FILE_ATTRIBUTE_READONLY)
			{
				RESET_BIT(dwRet, FILE_ATTRIBUTE_READONLY);
				SetFileAttributes(fileName, dwRet);
			}
		}

		return DeleteFile(fileName);
	}
	bool Delete(bool bForce = false) { Delete(tstrFileName, bForce); }
	//强制表明，存在也进行覆盖,若非覆盖模式，且已经存在，则会返回 false
	//----复制到C盘权限不够，会失败
	bool Copy(LPCTSTR dest, LPCTSTR source, bool bForce = false) { return CopyFile(source, dest, !bForce); }
	bool Copy(LPCTSTR dest, bool bForce = false) { return Copy(dest, tstrFileName, bForce); }
	bool Cut(LPCTSTR dest, LPCTSTR source, bool bForce = false) //{ return Copy(dest, source, bForce) ? Delete(source, bForce) : false; }  担心运算顺序
	{
		if (Copy(dest, source, bForce))
			return Delete(source, bForce);
		return false;
	}
	bool Cut(LPCTSTR dest, bool bForce = false) { return Cut(dest, tstrFileName, bForce); }
	//可以是文件夹
	bool Move(LPCTSTR dest, LPCTSTR source) { return MoveFile(source, dest); }
	bool Move(LPCTSTR dest) { return Move(dest, tstrFileName); }
	bool Rename(LPCTSTR newName) { return Move(newName); }

	//-------------------------打开文件的操作---------------------------
	bool Create(LPCTSTR fileName, bool bForce)
	{
		//采用独占模式，创建的时候
		hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, bForce ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		return true;
	}
	BOOL CreateHideFile(LPCTSTR lpFilePath)
	{
		if (!lpFilePath || _tcslen(lpFilePath) < 3) return FALSE;

		HANDLE hFile = CreateFile(lpFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
		if (hFile == INVALID_HANDLE_VALUE) return FALSE;
	}
	//r  r & exist
	//w  w & always
	//a  a & exist
	//r+, w+, a+  -> r/w
	bool Open(LPCTSTR fileName, const char* mode, SHAREMODE shareMode = SHARER)
	{
		//采用独占模式，创建的时候
		DWORD	fileAccess;
		DWORD   fileDisposition;
		if (mode[0] == 'r' || mode[0] == 'a')
		{
			if (mode[0] == 'a')
			{
				fileAccess = GENERIC_WRITE;
			}
			else
			{
				fileAccess = GENERIC_READ;
			}

			fileDisposition = OPEN_EXISTING;
		}
		else if (mode[0] == 'w')
		{
			fileAccess = GENERIC_WRITE;

			fileDisposition = OPEN_ALWAYS;
		}
		else
		{
			return false;
		}

		if (mode[1] == '+')
		{
			fileAccess = GENERIC_READ | GENERIC_WRITE;
		}
		else
		{
			return false;
		}

		hFile = CreateFile(fileName, fileAccess, shareMode, nullptr, fileDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		return true;
	}
	int Write(LPCTSTR string, ...)//二进制
	{
		DWORD beWritten = 0;
		WriteFile(hFile, string, sizeof(string), &beWritten, nullptr);
		return beWritten;
	}
	int Write(TSTRING string, ...)
	{
		DWORD beWritten = 0;
		WriteFile(hFile, (TCHAR*)string, string.getSize()*sizeof(TCHAR), &beWritten, nullptr);
		return beWritten;
	}
	DWORD Write(void* buffer, int size)
	{
		DWORD beWritten = 0;
		WriteFile(hFile, buffer, size, &beWritten, nullptr);
		return beWritten;
	}
	DWORD Read(void* buffer, int size)
	{
		DWORD beRead = 0;
		ReadFile(hFile, buffer, size, &beRead, nullptr);
		return beRead;
	}
	bool Flush()
	{
		return FlushFileBuffers(hFile);
	}
	DWORD Seek(LONG low, PLONG high = nullptr, DWORD method = FILE_CURRENT)
	{
		return SetFilePointer(hFile, low, high, method);
	}
	bool Seek(LONGLONG distant, DWORD method = FILE_CURRENT, PLARGE_INTEGER newDist = nullptr)
	{
		LARGE_INTEGER distantStruct;
		distantStruct.QuadPart = distant;
		return SetFilePointerEx(hFile, distantStruct, newDist, method);
	}
	LONGLONG getSize(LPCTSTR fileName)//函数执行失败返回-1，否则返回文件大小
	{
		LARGE_INTEGER  fileSize = { 0 };
		if (!GetFileSizeEx(hFile, &fileSize))
		{
			return -1;
		}
		return fileSize.QuadPart;
	}
	LONGLONG getSize() { return getSize(tstrFileName); }
	bool Close(void) 
	{
		if (hFile != INVALID_HANDLE_VALUE)
		{
			bool bRe = CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
			return bRe;
		}
		return true;
	}
	~winFILE(void) { Close(); }
	
	bool Check(LPCTSTR fileOrdirectory)
	{
		return PathFileExists(fileOrdirectory);
	}

	//文件夹的操作
	bool CreateDir(LPCTSTR path, bool force = false)
	{
		if (force)
		{
			ASTRING pathString;
#ifdef UNICODE
			pathString = WToA(path);
#else
			pathString = path;
#endif
			return MakeSureDirectoryPathExists(pathString);
		}

		return CreateDirectory(path, nullptr);
	}
	BOOL CreateDeepDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
	{
		if (PathFileExists(lpPathName)) return TRUE;

		TCHAR szPath[MAX_PATH] = { 0 };
		TCHAR pszSrc[MAX_PATH] = { 0 };
		_tcscpy(pszSrc, lpPathName);
		TCHAR* pToken = _tcstok(pszSrc, _T("\\"));
		while (pToken)
		{
			_tcscat(szPath, pToken);
			_tcscat(szPath, _T("\\"));
			if (!PathFileExists(szPath))
			{
				if (!CreateDirectory(szPath, lpSecurityAttributes)) return FALSE;
			}

			pToken = _tcstok(NULL, _T("\\"));
		}

		return TRUE;
	}
	bool RemoveDir(LPCTSTR path, bool force = false)
	{
		if (force)
		{
			SHFILEOPSTRUCT FileOp;
			FileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
			FileOp.hNameMappings = NULL;
			FileOp.hwnd = NULL;
			FileOp.lpszProgressTitle = NULL;
			FileOp.pFrom = path;
			FileOp.pTo = NULL;
			FileOp.wFunc = FO_DELETE;
			return !SHFileOperation(&FileOp);
		}

		return RemoveDirectory(path);
	}

	//支持通配符
	BOOL EnumFiles(LPCTSTR lpszPath, LPCTSTR lpszType, winFILELIST& fileList)
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		_stprintf(szPath, _T("%s\\%s"), lpszPath, lpszType);

		////效率有些低好吧，要先得到信息，再复制到链表当中
		////不能先到链表中创建，然后得到消息，这样就不用复制了
		//WIN32_FIND_DATA FindData;
		fileNODE* FindData = new fileNODE;

		HANDLE hFind = FindFirstFile(szPath, &FindData->data);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			delete FindData;
			return FALSE;
		}

		fileList.insertBefore(FindData);

		BOOL bRet = FALSE;
		do
		{
			FindData = new fileNODE;
			bRet = FindNextFile(hFind, &FindData->data);
			if (!bRet) break;

			//if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			//{
			//	//文件夹
			//}
			//else
			//{
			//	if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
			//	cout << "文件：";
			//}

			fileList.insertBefore(FindData);

		} while (bRet);

		delete FindData;
		fileList.reset(0);
		return TRUE;
	}
};

//winFILELIST fileList;
//EnumFiles(TEXT("."), TEXT("*.*"), fileList);
//
//while (!fileList.endOfList())
//{
//	wcout << fileList.data().cFileName << endl;
//	fileList.next();
//}

class stdFILE
{
private:
	FILE* file;
	ASTRING fileName;
	char timeString[280];
public:
	FILE* GetFile() { return file; }
	stdFILE() :file(nullptr) {};
	bool Open(const char* fileName, const char* mode = "w+") { return file = fopen(fileName, mode); }
	int Write(const char* string, ...)
	{
		char buffer[MAX_BUFFER] = { 0 };
		GetVariableArgumentA(buffer, MAX_BUFFER, string);
		int num = fprintf(file, buffer);
		fflush(file);
		return num;
	}
	size_t Write(void* buffer, size_t elementSize, size_t elementCount)
	{
		return fwrite(buffer, elementSize, elementCount, file);
	}
	char* TimeString()
	{
		struct _timeb timebuffer;
		//返回的字符串格式如下： 
		//Www Mmm dd hh : mm : ss yyyy 
		//Www 表示星期几
		//Mmm 是以字母表示的月份
		//dd 表示一月中的第几天
		//hh : mm: ss 表示时间
		//yyyy 表示年份
		char* timeline = ctime(&(timebuffer.time));
		//％hhu：％hhu的输出类型为用于输出一个unsigned short 短整型的数值
		//％hu：％hu的输出类型为用于输出一个unsigned short int 整型的数值
		//％hhu：％hhu的占用空间为一个字节
		//％hu：％hu的占用空间为两个字节
		sprintf(timeString, "%.19s.%hu, %s", timeline, timebuffer.millitm, &timeline[20]);
		return timeString;
	}
	void Close() { if (file) { fclose(file); file = nullptr; } }
	~stdFILE() { Close(); }
};


