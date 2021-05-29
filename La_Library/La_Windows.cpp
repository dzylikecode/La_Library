#include "La_Windows.h"



int MessageBoxPrintf(const TCHAR* szCaption, UINT uType, const TCHAR* szFormat, ...)
{
	TCHAR   szBuffer[MAX_BUFFER];
	GetVariableArgument(szBuffer,MAX_BUFFER, szFormat);
	MessageBeep(MB_ICONQUESTION);
	return MessageBox(NULL, szBuffer, szCaption, uType);
}


WSTRING AToW(__in LPCCH pszInBuf)
{
	int nInSize = lstrlenA(pszInBuf);
	int pnOutSize = MultiByteToWideChar(NULL, NULL, pszInBuf, nInSize, nullptr, 0);// ��ȡ��ת���ַ����Ļ����������С
	WSTRING temp;
	if (pnOutSize == 0)
		return temp;
	pnOutSize++;
	temp.resize(pnOutSize);
	if (MultiByteToWideChar(NULL, NULL, pszInBuf, nInSize, temp, pnOutSize) == 0)
	{
		temp.clear();
		return temp;
	}
	temp[pnOutSize - 1] = '\0';
	temp.UpdateLen();
	return temp;
}


ASTRING WToA(__in LPCWCH pszInBuf)
{
	int nInSize = lstrlenW(pszInBuf);
	int pnOutSize = WideCharToMultiByte(NULL, NULL, pszInBuf, nInSize, nullptr, 0, NULL, NULL);// ��ȡ��ת���ַ����Ļ����������С
	ASTRING temp;
	if (pnOutSize == 0)
		return temp;
	pnOutSize++;
	temp.resize(pnOutSize);
	if (WideCharToMultiByte(NULL, NULL, pszInBuf, nInSize, temp, pnOutSize, 0, nullptr) == 0)
	{
		temp.clear();
		return temp;
	}
	temp[pnOutSize - 1] = '\0';
	temp.UpdateLen();
	return temp;
}


