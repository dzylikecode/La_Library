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

