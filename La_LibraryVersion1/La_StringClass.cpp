#include "La_StringClass.h"

WSTRING& AToW(__in LPCCH pszInBuf, WSTRING& result)
{
	int nInSize = lstrlenA(pszInBuf);
	int pnOutSize = MultiByteToWideChar(NULL, NULL, pszInBuf, nInSize, nullptr, 0);// ��ȡ��ת���ַ����Ļ����������С
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
	return result;
}


ASTRING& WToA(__in LPCWCH pszInBuf, ASTRING& result)
{
	int nInSize = lstrlenW(pszInBuf);
	int pnOutSize = WideCharToMultiByte(NULL, NULL, pszInBuf, nInSize, nullptr, 0, NULL, NULL);// ��ȡ��ת���ַ����Ļ����������С
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
	return result;
}
