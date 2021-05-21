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
*  创建时间: 2021/05/20 11:44:59
*  最后修改: 
*
*  简    介: 
*
***************************************************************************************
*/

#pragma once
#include "La_Windows.h"

class SHELL :private SHELLEXECUTEINFO
{
public:
	SHELL(void) { Clear(); }
	//Open(TEXT("explorer.exe"), TEXT("::{20d04fe0-3aea-1069-a2d8-08002b30309d}")); 打开 我的电脑
	bool Open(LPCTSTR fileNameOrDirectory, LPCTSTR parameters = nullptr);
	bool Run(LPCTSTR fileName, LPCTSTR parameters = nullptr);//以管理员权限运行
	bool Explore(LPCTSTR directory);
	bool Print(LPCTSTR fileName);//必须文档
	bool Edit(LPCTSTR fileName);
	bool Select(LPCTSTR fileNameOrDirectory);
	void SetShowStyle(SHOWSTYLE showStyle) { nShow = showStyle; }
	void Clear(void)
	{
		memset((SHELLEXECUTEINFO*)this, 0, sizeof(SHELLEXECUTEINFO));
		cbSize = sizeof(SHELLEXECUTEINFO);
		nShow = SHOW;
	}
	void SetWorkDirectory(LPCTSTR directory) { lpDirectory = directory; }
};