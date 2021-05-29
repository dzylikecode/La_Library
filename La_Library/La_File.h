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
*  ����ʱ��: 2021/05/20 17:21:10
*  ����޸�: 
*
*  ��    ��: 
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

	//------------------���²���Ҫ���ļ��Ϳ��Բ���----------------------------
	bool Delete(LPCTSTR fileName, bool bForce = false)//ǿ�Ʊ�����ֻ�����ļ�Ҳɾ��
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
	//ǿ�Ʊ���������Ҳ���и���,���Ǹ���ģʽ�����Ѿ����ڣ���᷵�� false
	//----���Ƶ�C��Ȩ�޲�������ʧ��
	bool Copy(LPCTSTR dest, LPCTSTR source, bool bForce = false) { return CopyFile(source, dest, !bForce); }
	bool Copy(LPCTSTR dest, bool bForce = false) { return Copy(dest, tstrFileName, bForce); }
	bool Cut(LPCTSTR dest, LPCTSTR source, bool bForce = false) //{ return Copy(dest, source, bForce) ? Delete(source, bForce) : false; }  ��������˳��
	{
		if (Copy(dest, source, bForce))
			return Delete(source, bForce);
		return false;
	}
	bool Cut(LPCTSTR dest, bool bForce = false) { return Cut(dest, tstrFileName, bForce); }
	//�������ļ���
	bool Move(LPCTSTR dest, LPCTSTR source) { return MoveFile(source, dest); }
	bool Move(LPCTSTR dest) { return Move(dest, tstrFileName); }
	bool Rename(LPCTSTR newName) { return Move(newName); }

	//-------------------------���ļ��Ĳ���---------------------------
	bool Create(LPCTSTR fileName, bool bForce)
	{
		//���ö�ռģʽ��������ʱ��
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
		//���ö�ռģʽ��������ʱ��
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
	int Write(LPCTSTR string, ...)//������
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
	LONGLONG getSize(LPCTSTR fileName)//����ִ��ʧ�ܷ���-1�����򷵻��ļ���С
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

	//�ļ��еĲ���
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

	//֧��ͨ���
	BOOL EnumFiles(LPCTSTR lpszPath, LPCTSTR lpszType, winFILELIST& fileList)
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		_stprintf(szPath, _T("%s\\%s"), lpszPath, lpszType);

		////Ч����Щ�ͺðɣ�Ҫ�ȵõ���Ϣ���ٸ��Ƶ�������
		////�����ȵ������д�����Ȼ��õ���Ϣ�������Ͳ��ø�����
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
			//	//�ļ���
			//}
			//else
			//{
			//	if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
			//	cout << "�ļ���";
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
		//���ص��ַ�����ʽ���£� 
		//Www Mmm dd hh : mm : ss yyyy 
		//Www ��ʾ���ڼ�
		//Mmm ������ĸ��ʾ���·�
		//dd ��ʾһ���еĵڼ���
		//hh : mm: ss ��ʾʱ��
		//yyyy ��ʾ���
		char* timeline = ctime(&(timebuffer.time));
		//��hhu����hhu���������Ϊ�������һ��unsigned short �����͵���ֵ
		//��hu����hu���������Ϊ�������һ��unsigned short int ���͵���ֵ
		//��hhu����hhu��ռ�ÿռ�Ϊһ���ֽ�
		//��hu����hu��ռ�ÿռ�Ϊ�����ֽ�
		sprintf(timeString, "%.19s.%hu, %s", timeline, timebuffer.millitm, &timeline[20]);
		return timeString;
	}
	void Close() { if (file) { fclose(file); file = nullptr; } }
	~stdFILE() { Close(); }
};


