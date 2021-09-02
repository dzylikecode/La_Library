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
*  ����ʱ��: 2021/05/20 11:10:24
*  ����޸�: 
*
*  ��    ��: 
*
***************************************************************************************
*/

#pragma once
#include <windows.h>
#include <tchar.h>
#include "La_String.h"

typedef STRING<TCHAR>	TSTRING;
typedef STRING<CHAR>	ASTRING;
typedef STRING<WCHAR>	WSTRING;

enum SHOWSTYLE :int
{
	HIDE = SW_HIDE,						 // ���ش��ڣ��״̬����һ������
	MINIMIZE = SW_MINIMIZE,				 //��С�����ڣ��״̬����һ������
	RESTORE = SW_RESTORE,				 //��ԭ���Ĵ�С��λ����ʾһ�����ڣ�ͬʱ�������״̬
	SHOW = SW_SHOW,						 //�õ�ǰ�Ĵ�С��λ����ʾһ�����ڣ�ͬʱ�������״̬
	SHOWMAXIMIZED = SW_SHOWMAXIMIZED,	 //��󻯴��ڣ������伤��
	SHOWMINIMIZED = SW_SHOWMINIMIZED,	 //��С�����ڣ������伤��
	SHOWMINNOACTIVE = SW_SHOWMINNOACTIVE,//��С��һ�����ڣ�ͬʱ���ı�����
	SHOWNA = SW_SHOWNA,					 //�õ�ǰ�Ĵ�С��λ����ʾһ�����ڣ����ı�����
	SHOWNOACTIVATE = SW_SHOWNOACTIVATE,  //������Ĵ�С��λ����ʾһ�����ڣ�ͬʱ���ı�����
	SHOWNORMAL = SW_SHOWNORMAL			 //��SW_RESTORE��ͬ
};

#define SET_BIT(word, bit_flag)		((word) |= (bit_flag))
#define RESET_BIT(word, bit_flag)	((word) &= ~(bit_flag))


#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)


#define MAX_BUFFER		1024
#define GetVariableArgument(szBuffer,bufferSize, szFormat)   {va_list pArgList;\
															 va_start(pArgList, szFormat);\
															 _vsntprintf_s(szBuffer, bufferSize, szFormat, pArgList);\
															 va_end(pArgList);\
															 }

#define GetVariableArgumentA(szBuffer,bufferSize, szFormat)   {va_list pArgList;\
															 va_start(pArgList, szFormat);\
															 _vsnprintf_s(szBuffer, bufferSize, szFormat, pArgList);\
															 va_end(pArgList);\
															 }

#define GetVariableArgumentW(szBuffer,bufferSize, szFormat)   {va_list pArgList;\
															 va_start(pArgList, szFormat);\
															 _vsnwprintf_s(szBuffer, bufferSize, szFormat, pArgList);\
															 va_end(pArgList);\
															 }

#define MessageInfo(message, ...)			MessageBoxPrintf(TEXT("Inform"), MB_OK | MB_ICONINFORMATION, message, ##__VA_ARGS__)
#define MessageErr(message, ...)			MessageBoxPrintf(TEXT("Error"), MB_OK | MB_ICONERROR, message, ##__VA_ARGS__)
#define MessageWarn(message, ...)			MessageBoxPrintf(TEXT("Warn"), MB_OK | MB_ICONWARNING, message, ##__VA_ARGS__)

int MessageBoxPrintf(const TCHAR* szCaption, UINT uType, const TCHAR* szFormat, ...);


WSTRING& AToW(__in LPCCH pszInBuf, WSTRING& result);
inline WSTRING&  AToW(__in LPCCH pszInBuf)
{
	static WSTRING temp; //��������ʧ������Ȼ����Ҫ�� static
	temp.clear();
	return AToW(pszInBuf, temp);
}
ASTRING& WToA(__in LPCWCH pszInBuf, ASTRING& result);
inline ASTRING& WToA(__in LPCWCH pszInBuf)
{
	static ASTRING temp;
	temp.clear();
	return WToA(pszInBuf, temp);
}

inline WSTRING& WToW(__in LPCWCH pszInBuf, WSTRING& result)
{
	return result = pszInBuf;
}
inline WSTRING&  WToW(__in LPCWCH pszInBuf)
{
	static WSTRING temp;
	temp.clear();
	return temp = pszInBuf;
}

inline ASTRING& AToA(__in LPCCH pszInBuf, ASTRING& result)
{
	return result = pszInBuf;
}
inline ASTRING& AToA(__in LPCCH pszInBuf)
{
	static ASTRING temp;
	temp.clear();
	return temp = pszInBuf;
}

#ifdef UNICODE
#define AToT    AToW
#define TToA	WToA
#define WToT	WToW
#define TToW	WToW
#else
#define AToT    AToA
#define TToA	AToA
#define WToT	WToA
#define TToW	AToW
#endif

#include <locale.h> //�ر�ͷ�ļ�
inline void StartWchar()
{
	setlocale(LC_ALL, ""); //����������� �������������
}



extern const BYTE UTF_16[3];
extern const BYTE UTF_16LE[3];
extern const BYTE UTF_16BE[3];

#define uniHead  UTF_16
