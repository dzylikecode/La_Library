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
*  创建时间: 2021/05/20 11:10:24
*  最后修改: 
*
*  简    介: 
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
	HIDE = SW_HIDE,						 // 隐藏窗口，活动状态给令一个窗口
	MINIMIZE = SW_MINIMIZE,				 //最小化窗口，活动状态给令一个窗口
	RESTORE = SW_RESTORE,				 //用原来的大小和位置显示一个窗口，同时令其进入活动状态
	SHOW = SW_SHOW,						 //用当前的大小和位置显示一个窗口，同时令其进入活动状态
	SHOWMAXIMIZED = SW_SHOWMAXIMIZED,	 //最大化窗口，并将其激活
	SHOWMINIMIZED = SW_SHOWMINIMIZED,	 //最小化窗口，并将其激活
	SHOWMINNOACTIVE = SW_SHOWMINNOACTIVE,//最小化一个窗口，同时不改变活动窗口
	SHOWNA = SW_SHOWNA,					 //用当前的大小和位置显示一个窗口，不改变活动窗口
	SHOWNOACTIVATE = SW_SHOWNOACTIVATE,  //用最近的大小和位置显示一个窗口，同时不改变活动窗口
	SHOWNORMAL = SW_SHOWNORMAL			 //与SW_RESTORE相同
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

#define MessageInfo(message, ...)			MessageBoxPrintf(TEXT("Inform"), MB_OK | MB_ICONINFORMATION, message, ##__VA_ARGS__)
#define MessageErr(message, ...)			MessageBoxPrintf(TEXT("Error"), MB_OK | MB_ICONERROR, message, ##__VA_ARGS__)
#define MessageWarn(message, ...)			MessageBoxPrintf(TEXT("Warn"), MB_OK | MB_ICONWARNING, message, ##__VA_ARGS__)

int MessageBoxPrintf(const TCHAR* szCaption, UINT uType, const TCHAR* szFormat, ...);