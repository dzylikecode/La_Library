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
*  ����ʱ��: 2021/05/31 0:29:30
*  ����޸�:
*
*  ��    ��:
*
***************************************************************************************
*/

#ifndef _LA_CONSOLE_
#define _LA_CONSOLE_

#include <windows.h>
#include "La_String.h"
//FOREGROUND_GREEN	ǰ��ɫ���� ��ɫ
//FOREGROUND_RED	ǰ��ɫ���� ��ɫ
//FOREGROUND_INTENSITY	ǰ��ɫ��ǿ
//BACKGROUND_BLUE	����ɫ���� ��ɫ
//BACKGROUND_GREEN	����ɫ���� ��ɫ
//BACKGROUND_RED	����ɫ���� ��ɫ
//BACKGROUND_INTENSITY	����ɫ��ǿ
//COMMON_LVB_GRID_HORIZONTAL	����ˮƽ����
//COMMON_LVB_GRID_LVERTICAL	����ֱ����
//COMMON_LVB_GRID_RVERTICAL	����ֱ����
//COMMON_LVB_UNDERSCORE	�»���

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
#define B_BLACK			( 0 )
//e enhance the color
#define B_COLOR(r, g, b, e)		  ( ( (r) ? B_LOW_RED   : 0 ) | \
								    ( (g) ? B_LOW_GREEN : 0 ) |	\
									( (b) ? B_LOW_BLUE	: 0 ) |	\
									( (e) ? B_INTENSITY : 0 )	)


/// ����SetConsoleTextAttribute����������ı�����ɫ��
/// ��һ��������ǰ��ȡ�õ����������ڶ��������Ǻ��ֵ��ϡ�
/// FOREGROUNDǰ׺����ǰ��ɫ��
/// BACKGROUND��������ɫ��
/// RED��GREEN��BLUE����ԭɫ������
/// INTENSITY������ɫ��ǿ����ø�����


class CONSOLE
{
private:
	tstring consoleName;
	FILE* stream;
	HANDLE hOutput;
	CONSOLE_SCREEN_BUFFER_INFO information;
public:
	CONSOLE() :stream(nullptr), information{ 0 }{};
	void setTitle(const TCHAR* name) { SetConsoleTitle(name); consoleName = name; }
	bool create(const TCHAR* name = nullptr);
	bool setColor(WORD color) { return SetConsoleTextAttribute(hOutput, color); }
	bool resetColor() { return SetConsoleTextAttribute(hOutput, F_HIGH_WITHTE | B_BLACK); }
	//��Ч��Χ 0 - 100
	void hideCursor() { CONSOLE_CURSOR_INFO cur = { 1, false }; SetConsoleCursorInfo(hOutput, &cur); }
	void showCursor(DWORD size = 1) { CONSOLE_CURSOR_INFO cur = { size, true }; SetConsoleCursorInfo(hOutput, &cur); }
	bool gotoXY(SHORT x, SHORT y) { COORD pos = { x, y }; return SetConsoleCursorPosition(hOutput, pos); }
	bool resizeBuffer(SHORT width, SHORT height) { COORD size = { width, height }; return SetConsoleScreenBufferSize(hOutput, size); }
	bool resize(SHORT x, SHORT y, SHORT width, SHORT height) { SMALL_RECT rc = { x, y, x + width, y + height }; return SetConsoleWindowInfo(hOutput, true, &rc); }
	bool resize(SHORT width, SHORT height) { SMALL_RECT rc = { 0, 0, width, height }; return SetConsoleWindowInfo(hOutput, true, &rc); }
	bool move(int x, int y) { return resize(x, y, x + getWidth(), y + getHeight()); }
	int  getCursorX() { getInfo(information); return information.dwCursorPosition.X; }
	int  getCursorY() { getInfo(information); return information.dwCursorPosition.Y; }
	int  getX() { getInfo(information); return information.srWindow.Left; }
	int  getY() { getInfo(information); return information.srWindow.Top; }
	int  getWidth() { getInfo(information); return information.srWindow.Right - information.srWindow.Left; }
	int  getHeight() { getInfo(information); return information.srWindow.Bottom - information.srWindow.Top; }
	int  getBufferWidth() { getInfo(information); return information.dwSize.X; }
	int  getBufferHeight() { getInfo(information); return information.dwSize.Y; }
	WORD getColor() { getInfo(information); return information.wAttributes; }
	void clear() { system("cls"); }
	void getInfo(CONSOLE_SCREEN_BUFFER_INFO& info) { GetConsoleScreenBufferInfo(hOutput, &info); }

	void close() { FreeConsole(); }
	~CONSOLE() { close(); }
};


extern CONSOLE consoleOut;

#define BLANK		B_HIGH_WITHTE
#define TIME		F_HIGH_PURPLE
#define INFO		F_HIGH_GREEN
#define NOTICE		F_HIGH_CYAN
#define WARN		F_HIGH_YELLOW
#define ERR			F_HIGH_RED

#define GetConsoleColor    consoleOut.getColor
#define SetConsoleColor    consoleOut.setColor
#define WriteInConsole(mode, message, ...)	{WORD oldColor = GetConsoleColor();\
											 SetConsoleColor(mode);\
											 _tprintf(TEXT(message), ##__VA_ARGS__);\
											 SetConsoleColor(oldColor);}

#endif