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
	bool Creat(LPCTSTR fileName, bool bForce)
	{
		//���ö�ռģʽ��������ʱ��
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
	int Write(LPCTSTR string, ...)
	{

	}
	LARGE_INTEGER GetSize(LPCTSTR fileName)
	{
		LARGE_INTEGER  fileSize = { 0 };
		bool bRe = GetFileSizeEx(hFile, &fileSize);
		if (!bRe)
		{
			MessageWarn(TEXT("�ļ�δ��"));
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
	~STDFILE() { Close(); }
};