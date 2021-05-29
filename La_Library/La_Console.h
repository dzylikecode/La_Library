#pragma once
#include "La_Windows.h"
#include <stdio.h>
#define DEFAULT_CONSOLE_TITLE  TEXT("Debug")

//FOREGROUND_GREEN	前景色包含 绿色
//FOREGROUND_RED	前景色包含 红色
//FOREGROUND_INTENSITY	前景色加强
//BACKGROUND_BLUE	背景色包含 蓝色
//BACKGROUND_GREEN	背景色包含 绿色
//BACKGROUND_RED	背景色包含 红色
//BACKGROUND_INTENSITY	背景色加强
//COMMON_LVB_GRID_HORIZONTAL	顶部水平网格
//COMMON_LVB_GRID_LVERTICAL	左竖直网格
//COMMON_LVB_GRID_RVERTICAL	右竖直网格
//COMMON_LVB_UNDERSCORE	下划线

#define F_LOW_RED		( FOREGROUND_RED )
#define F_LOW_GREEN		( FOREGROUND_GREEN )
#define F_LOW_BLUE		( FOREGROUND_BLUE )
#define F_INTENSITY		( FOREGROUND_INTENSITY )

#define F_HIGH_RED		( FOREGROUND_RED   | FOREGROUND_INTENSITY )
#define F_HIGH_GREEN	( FOREGROUND_GREEN | FOREGROUND_INTENSITY )
#define F_HIGH_BLUE	    ( FOREGROUND_BLUE  | FOREGROUND_INTENSITY )

#define F_LOW_YELLOW	( FOREGROUND_RED   | FOREGROUND_GREEN )
#define F_HIGH_YELLOW	( FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY )

#define F_LOW_PURPLE	( FOREGROUND_RED   | FOREGROUND_BLUE )
#define F_HIGH_PURPLE	( FOREGROUND_RED   | FOREGROUND_BLUE | FOREGROUND_INTENSITY )

#define F_LOW_CYAN		( FOREGROUND_BLUE  | FOREGROUND_GREEN )
#define F_HIGH_CYAN		( FOREGROUND_BLUE  | FOREGROUND_GREEN | FOREGROUND_INTENSITY )

#define F_LOW_WITHTE	( FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE )
#define F_HIGH_WITHTE	( FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY )
#define F_GRAY			( FOREGROUND_INTENSITY )

//e enhance the color
#define F_COLOR(r, g, b, e)		  ( ( (r) ? F_LOW_RED   : 0 ) | \
								    ( (g) ? F_LOW_GREEN : 0 ) |	\
									( (b) ? F_LOW_BLUE	: 0 ) |	\
									( (e) ? F_INTENSITY : 0 )	)


#define B_LOW_RED		( BACKGROUND_RED )
#define B_LOW_GREEN		( BACKGROUND_GREEN )
#define B_LOW_BLUE		( BACKGROUND_BLUE )
#define B_INTENSITY		( BACKGROUND_INTENSITY )

#define B_HIGH_RED		( BACKGROUND_RED   | BACKGROUND_INTENSITY )
#define B_HIGH_GREEN	( BACKGROUND_GREEN | BACKGROUND_INTENSITY )
#define B_HIGH_BLUE		( BACKGROUND_BLUE  | BACKGROUND_INTENSITY )

#define B_LOW_YELLOW	( BACKGROUND_RED   | BACKGROUND_GREEN )
#define B_HIGH_YELLOW	( BACKGROUND_RED   | BACKGROUND_GREEN | BACKGROUND_INTENSITY )

#define B_LOW_PURPLE	( BACKGROUND_RED   | BACKGROUND_BLUE )
#define B_HIGH_PURPLE	( BACKGROUND_RED   | BACKGROUND_BLUE | BACKGROUND_INTENSITY )

#define B_LOW_CYAN		( BACKGROUND_BLUE  | BACKGROUND_GREEN )
#define B_HIGH_CYAN		( BACKGROUND_BLUE  | BACKGROUND_GREEN | BACKGROUND_INTENSITY )

#define B_LOW_WITHTE	( BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE )
#define B_HIGH_WITHTE	( BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE )
#define B_GRAY			( BACKGROUND_INTENSITY )
//e enhance the color
#define B_COLOR(r, g, b, e)		  ( ( (r) ? B_LOW_RED   : 0 ) | \
								    ( (g) ? B_LOW_GREEN : 0 ) |	\
									( (b) ? B_LOW_BLUE	: 0 ) |	\
									( (e) ? B_INTENSITY : 0 )	)


/// 其中SetConsoleTextAttribute设置了输出文本的颜色，
/// 第一个参数是前面取得的输出句柄，第二个参数是宏的值组合。
/// FOREGROUND前缀代表前景色，
/// BACKGROUND代表背景色。
/// RED、GREEN、BLUE是三原色分量，
/// INTENSITY代表颜色加强（变得更亮）

enum WriteMode :int
{
	BLANK = B_HIGH_WITHTE,
	TIME = F_HIGH_PURPLE,
	INFO = F_HIGH_GREEN,
	NOTICE = F_HIGH_CYAN,
	WARN = F_HIGH_YELLOW,
	ERR = F_HIGH_RED
};

class CONSOLE
{
private:
	TSTRING consoleName;
	HWND hConsole;
	FILE* stream;
	HANDLE hOutput;
	CONSOLE_SCREEN_BUFFER_INFO information;
public:
	CONSOLE() :stream(nullptr), information{ 0 }{};
	void setTitle(LPCTSTR name)
	{
		SetConsoleTitle(name);
		consoleName = name;
		hConsole = FindWindow(NULL, name);
	}
	bool create(LPCTSTR name = DEFAULT_CONSOLE_TITLE)
	{
		AllocConsole();
		setTitle(name);
		freopen_s(&stream, "conin$", "r+t", stdin);//重定向输入流
		freopen_s(&stream, "conout$", "w+t", stdout);//重定向输入流
		hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

		return true;
	}
	bool setColor(WORD color) { return SetConsoleTextAttribute(hOutput, color); }
	int write(WORD color, LPCTSTR string, ...)
	{
		setColor(color);
		TCHAR   szBuffer[MAX_BUFFER];
		GetVariableArgument(szBuffer, MAX_BUFFER, string);
		//fprintf(stream, buffer); //都可以
		return _tprintf(szBuffer);		//如果控制台删除了，printf 就会报错
	}
	//有效范围 0 - 100
	void hide()
	{
		CONSOLE_CURSOR_INFO cur = { 1, false };
		SetConsoleCursorInfo(hOutput, &cur);
	}
	void show(int size = 1)
	{
		CONSOLE_CURSOR_INFO cur = { size, true };
		SetConsoleCursorInfo(hOutput, &cur);
	}
	bool gotoXY(int x, int y)
	{
		COORD pos = { x, y };
		return SetConsoleCursorPosition(hOutput, pos);
	}
	bool resize(int width, int height)
	{
		COORD size = { width, height };
		return SetConsoleScreenBufferSize(hOutput, size);
	}
	bool resize(int x, int y, int width, int height)
	{
		SMALL_RECT rc = { x, y, width, height };
		SetConsoleWindowInfo(hOutput, true, &rc);
	}
	bool move(int x, int y)
	{
		return resize(x, y, x + getWidth(), y + getHeight());
	}
	int  getCursorX()
	{
		getInfo(information);
		return information.dwCursorPosition.X;
	}
	int  getCursorY()
	{
		getInfo(information);
		return information.dwCursorPosition.Y;
	}
	int  getX()
	{
		getInfo(information);
		return information.srWindow.Left;
	}
	int  getY()
	{
		getInfo(information);
		return information.srWindow.Top;
	}
	int  getWidth()
	{
		getInfo(information);
		return information.srWindow.Right - information.srWindow.Left;
	}
	int  getHeight()
	{
		getInfo(information);
		return information.srWindow.Bottom - information.srWindow.Top;
	}
	int  getBufferWidth()
	{
		getInfo(information);
		return information.dwSize.X;
	}
	int  getBufferHeight()
	{
		getInfo(information);
		return information.dwSize.Y;
	}
	WORD getColor()
	{
		getInfo(information);
		return information.wAttributes;
	}
	void clear()
	{
		system("cls");
	}
	void getInfo(CONSOLE_SCREEN_BUFFER_INFO& info)
	{
		GetConsoleScreenBufferInfo(hOutput, &info);
	}

	void close(){ FreeConsole(); }
	~CONSOLE() { close(); }
};

