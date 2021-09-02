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
#include "La_WindowsBase.h"
#include <stdio.h>
#include <time.h>
#include <dbghelp.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Dbghelp.lib")

typedef NODE<WIN32_FIND_DATA>		fileNODE;
typedef LINKEDList<WIN32_FIND_DATA> winFILELIST;


namespace winFILEMaster
{
	enum ATTRIBUTION
	{
		DIR = FILE_ATTRIBUTE_DIRECTORY,
		SYS = FILE_ATTRIBUTE_SYSTEM,
		HIDE = FILE_ATTRIBUTE_HIDDEN,
		ROF = FILE_ATTRIBUTE_READONLY,
	};
	bool EnumFiles(LPCTSTR lpszPath, LPCTSTR lpszType, winFILELIST& fileList);
	bool EnumFilesAll(LPCTSTR lpszPath, LPCTSTR lpszType, winFILELIST& fileList);
	//强制表明，存在也进行覆盖,若非覆盖模式，且已经存在，则会返回 false
	//----复制到C盘权限不够，会失败
	inline bool Delete(LPCTSTR fileName, bool bForce = false)//强制表明，只读的文件也删除
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
	inline bool Copy(LPCTSTR dest, LPCTSTR source, bool bForce = false) { return CopyFile(source, dest, !bForce); }
	inline bool Cut(LPCTSTR dest, LPCTSTR source, bool bForce = false) //{ return Copy(dest, source, bForce) ? Delete(source, bForce) : false; }  担心运算顺序
	{
		if (Copy(dest, source, bForce))
			return Delete(source, bForce);
		return false;
	}
	//可以是文件夹
	inline bool move(LPCTSTR dest, LPCTSTR source) { return MoveFile(source, dest); }
	inline bool Rename(LPCTSTR newName, LPCTSTR fileName){ return MoveFile(fileName, newName); }

	//文件夹的操作
	inline bool CreateDir(LPCTSTR path, bool force = false)
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
	inline BOOL CreateDeepDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
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
	inline bool RemoveDir(LPCTSTR path, bool force = false)
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
	inline bool Check(LPCTSTR fileOrdirectory)
	{
		return PathFileExists(fileOrdirectory);
	}
}

class winFILE
{
public:
	enum SHAREMODE :DWORD
	{
		SHARER = FILE_SHARE_READ,
		SHAREW = FILE_SHARE_WRITE,
		EXCLUSIVE = 0,
		SHAREB = FILE_SHARE_READ | FILE_SHARE_WRITE
	};
	enum FILEPOSITION
	{
		BEGIN = FILE_BEGIN,
		CURRENT = FILE_CURRENT,
		END = FILE_END
	};
private:
	TSTRING	tstrFileName;
	HANDLE  hFile;
public:

	winFILE(void) :hFile(INVALID_HANDLE_VALUE) {};
	HANDLE getFile()const { return hFile; }
	const TCHAR* getFileName()const { return tstrFileName; }
	//-------------------------打开文件的操作---------------------------
	bool create(LPCTSTR fileName, bool bForce)
	{
		//采用独占模式，创建的时候
		hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, bForce ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		tstrFileName = fileName;
		return true;
	}
	bool createHideFile(LPCTSTR fileName)
	{
		if (!fileName || _tcslen(fileName) < 3) return false;

		HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
		if (hFile == INVALID_HANDLE_VALUE) return false;
		tstrFileName = fileName;
		return true;
	}
	//r  r & exist
	//w  w & always
	//a  a & exist
	//r+, w+, a+  -> r/w
	bool open(LPCTSTR fileName, const char* mode, SHAREMODE shareMode = SHARER)
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

		if (mode[1] != '\0')
		{
			if (mode[1] == '+')
			{
				fileAccess = GENERIC_READ | GENERIC_WRITE;
			}
			else
			{
				return false;
			}
		}

		hFile = CreateFile(fileName, fileAccess, shareMode, nullptr, fileDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD hre = GetLastError();
			MessageWarn(TEXT("error code: %d"), hre);
			return false;
		}
		if (mode[1] == 'a')
		{
			seek(0, FILE_END);
		}
		tstrFileName = fileName;
		return true;
	}
	void insertUnicodePrefix()
	{
		seek(0, BEGIN);
		writeBinary(uniHead, sizeof(uniHead));
	}
	void skipUnicodePrefix()
	{
		seek(sizeof(uniHead), BEGIN);
	}
	int write(const TSTRING& string)
	{
		DWORD beWritten = 0, beWrittenTotal = 0;
		DWORD size = string.getLength(); //测试了一下，没法子把空字符写入
		while (beWrittenTotal < size)
		{
			WriteFile(hFile, ((const TCHAR*)string + beWrittenTotal), (size - beWrittenTotal) * sizeof(TCHAR), &beWritten, nullptr);
			beWrittenTotal += beWritten / sizeof(TCHAR);
		}
		return beWritten;
	}
	DWORD writeBinary(const BYTE* buffer, DWORD size)
	{
		DWORD beWritten = 0, beWrittenTotal = 0;
		while (beWrittenTotal < size)
		{
			WriteFile(hFile, (buffer + beWrittenTotal), (size - beWrittenTotal), &beWritten, nullptr);
			beWrittenTotal += beWritten;
		}
		return beWritten;
	}
	DWORD read(BYTE* buffer, DWORD size)
	{
		DWORD beRead = 0, beReadTotal = 0;
		while (beReadTotal < size)
		{
			ReadFile(hFile, buffer + beReadTotal, size - beReadTotal, &beRead, nullptr);
			beReadTotal += beRead;
		}
		
		return beRead;
	}
	bool flush()
	{
		return FlushFileBuffers(hFile);
	}
	bool seek(LONGLONG distant, DWORD method = FILE_CURRENT, PLARGE_INTEGER newDist = nullptr)
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
	bool close(void) 
	{
		if (hFile != INVALID_HANDLE_VALUE)
		{
			bool bRe = CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
			return bRe;
		}
		return true;
	}
	~winFILE(void) { close(); }
};


class stdFILE
{
private:
	FILE* file;
	ASTRING fileName;
public:
	FILE* getFile() { return file; }
	stdFILE() :file(nullptr) {};
	bool open(const char* fileName, const char* mode = "w+") { return file = fopen(fileName, mode); }
	int write(const char* string, ...)
	{
		char buffer[MAX_BUFFER] = { 0 };
		GetVariableArgumentA(buffer, MAX_BUFFER, string);
		int num = fprintf(file, buffer);
		fflush(file);
		return num;
	}
	size_t writeBinary(void* buffer, size_t elementSize, size_t elementCount)
	{
		return fwrite(buffer, elementSize, elementCount, file);
	}
	bool flush() { return fflush(file); }
	void close() { if (file) { fclose(file); file = nullptr; } }
	~stdFILE() { close(); }
};


