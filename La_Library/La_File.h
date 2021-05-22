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

class DFILE
{
private:
	TSTRING	tstrFileName;
	HANDLE  hFile;
public:

	DFILE(void) :hFile(INVALID_HANDLE_VALUE) {};

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
	~DFILE(void) { Close(); }
};