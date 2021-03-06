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
*  创建时间: 2021/05/31 0:28:44
*  最后修改: 
*
*  简    介: 
*
***************************************************************************************
*/

#pragma once
#include "La_WindowsBase.h"
#include "La_Log.h"

#define FULL_SCREEN_X	GetSystemMetrics(SM_CXSCREEN)
#define FULL_SCREEN_Y	GetSystemMetrics(SM_CYSCREEN)

typedef void (*EXTEND_FUNCTION)(void);



class GAMEBox
{
private:
	WNDCLASSEX wndclassEx;
	bool bWindowed;
	bool bDX;
	bool bCreated;
	bool bExit;
	HINSTANCE hInst;
	HWND hwnd;
	EXTEND_FUNCTION exInitialize;
	EXTEND_FUNCTION exShutDown;
	EXTEND_FUNCTION gameBody;
public:
	GAMEBox();
	void setClassStyle(UINT style) { wndclassEx.style = style; }
	void loadCursor(int Macro) { wndclassEx.hCursor = LoadCursor(wndclassEx.hInstance, MAKEINTRESOURCE(Macro)); }
	void loadCursor(LPCTSTR filePath) { wndclassEx.hCursor = LoadCursorFromFile(filePath); }
	void loadSmallIcon(int Macro) { wndclassEx.hIconSm = LoadIcon(wndclassEx.hInstance, MAKEINTRESOURCE(Macro)); }
	void loadSmallIcon(LPCTSTR filePath) { wndclassEx.hIconSm = LoadIcon(wndclassEx.hInstance, filePath); }
	void loadIcon(int Macro) { wndclassEx.hIcon = LoadIcon(wndclassEx.hInstance, MAKEINTRESOURCE(Macro)); }
	void loadIcon(LPCTSTR filePath) { wndclassEx.hIcon = LoadIcon(wndclassEx.hInstance, filePath); }
	void setWndProc(WNDPROC wndProc) { wndclassEx.lpfnWndProc = wndProc; }
	void setWndBkColor(int Brush_Macro) { wndclassEx.hbrBackground = (HBRUSH)GetStockObject(Brush_Macro); }
	void setWndMode(bool bWindowMode, bool bDXMode) { bWindowed = bWindowMode; bDX = bDXMode; }
	bool create(int width, int height, LPCTSTR title, bool bTopLeft = false);
	bool startCommuication();
	bool attachMenu(LPCTSTR lpMenuName) { HMENU hMenu = LoadMenu(hInst, lpMenuName); return SetMenu(hwnd, hMenu); }
	bool attachMenu(int Macro) { HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(Macro)); return SetMenu(hwnd, hMenu); }
	HWND getHwnd()const { return hwnd; }
	HINSTANCE getHInstance()const { return hInst; }
	void setGameStart(EXTEND_FUNCTION exFunction) { exInitialize = exFunction; }
	void setGameBody(EXTEND_FUNCTION exFunction) { gameBody = exFunction; }
	void setGameEnd(EXTEND_FUNCTION exFunction) { exShutDown = exFunction; }
	void exitFromGameBody() { bExit = true; }
	POINT getOriginXY()const
	{
		POINT point = { 0, 0 };
		//获得客户区所在的位置
		ClientToScreen(hwnd, &point);
		return point;
	}
};