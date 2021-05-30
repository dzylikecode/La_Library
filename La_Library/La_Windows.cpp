#include "La_WindowsBase.h"


const BYTE UTF_16[3] = { 0xFE, 0xFF, 0xFE };
const BYTE UTF_16LE[3] = { 0xFE, 0xFF, 0x2D };
const BYTE UTF_16BE[3] = { 0xFE, 0xFF,  0x4E };

int MessageBoxPrintf(const TCHAR* szCaption, UINT uType, const TCHAR* szFormat, ...)
{
	TCHAR   szBuffer[MAX_BUFFER];
	GetVariableArgument(szBuffer,MAX_BUFFER, szFormat);
	MessageBeep(MB_ICONQUESTION);
	return MessageBox(NULL, szBuffer, szCaption, uType);
}


WSTRING& AToW(__in LPCCH pszInBuf, WSTRING& result)
{
	int nInSize = lstrlenA(pszInBuf);
	int pnOutSize = MultiByteToWideChar(NULL, NULL, pszInBuf, nInSize, nullptr, 0);// 获取待转换字符串的缓冲区所需大小
	if (pnOutSize == 0)
		return result;
	pnOutSize++;
	result.resize(pnOutSize);
	if (MultiByteToWideChar(NULL, NULL, pszInBuf, nInSize, result, pnOutSize) == 0)
	{
		result.clear();
		return result;
	}
	result[pnOutSize - 1] = '\0';
	result.UpdateLen();
	return result;
}


ASTRING& WToA(__in LPCWCH pszInBuf, ASTRING& result)
{
	int nInSize = lstrlenW(pszInBuf);
	int pnOutSize = WideCharToMultiByte(NULL, NULL, pszInBuf, nInSize, nullptr, 0, NULL, NULL);// 获取待转换字符串的缓冲区所需大小
	if (pnOutSize == 0)
		return result;
	pnOutSize++;
	result.resize(pnOutSize);
	if (WideCharToMultiByte(NULL, NULL, pszInBuf, nInSize, result, pnOutSize, 0, nullptr) == 0)
	{
		result.clear();
		return result;
	}
	result[pnOutSize - 1] = '\0';
	result.UpdateLen();
	return result;
}


