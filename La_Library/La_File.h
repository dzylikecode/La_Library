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

class WINFILE
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

	WINFILE(void) :hFile(INVALID_HANDLE_VALUE) {};

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
	bool Creat(LPCTSTR fileName, bool bForce)
	{
		//采用独占模式，创建的时候
		hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, bForce ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		return true;
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
	int Write(LPCTSTR string, ...)
	{

	}
	LARGE_INTEGER GetSize(LPCTSTR fileName)
	{
		LARGE_INTEGER  fileSize = { 0 };
		bool bRe = GetFileSizeEx(hFile, &fileSize);
		if (!bRe)
		{
			MessageWarn(TEXT("文件未打开"));
		}
		return fileSize;
	}
	LARGE_INTEGER GetSize() { return GetSize(tstrFileName); }
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
	~WINFILE(void) { Close(); }
};


class STDFILE
{
private:
	FILE* file;
	ASTRING fileName;
	char timeString[280];
public:
	FILE* GetFile() { return file; }
	STDFILE() :file(nullptr) {};
	bool Open(const char* fileName, const char* mode = "w+") { return file = fopen(fileName, mode); }
	int Write(const char* string, ...)
	{
		char buffer[MAX_BUFFER] = { 0 };
		GetVariableArgumentA(buffer, MAX_BUFFER, string);
		int num = fprintf(file, buffer);
		fflush(file);
		return num;
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
	~STDFILE() { Close(); }
};