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
*  创建时间: 2021/05/31 0:28:56
*  最后修改: 
*
*  简    介: 
*
***************************************************************************************
*/

#pragma once
#include "La_File.h"
#include "La_Console.h"
#include <sys/timeb.h>
#include <direct.h>

inline char* getTimeString()
{
	static char timeString[280] = { 0 };//AToW 会引用它，所以不能消失
	struct _timeb timebuffer = { 0 };
	//返回的字符串格式如下： 
	//Www Mmm dd hh : mm : ss yyyy 
	//Www 表示星期几
	//Mmm 是以字母表示的月份
	//dd 表示一月中的第几天
	//hh : mm: ss 表示时间
	//yyyy 表示年份
	char* timeline = ctime(&(timebuffer.time));
	//％hhu：％hhu的输出类型为用于输出一个unsigned short 短整型的数值
	//％hu：％hu的输出类型为用于输出一个unsigned short int 整型的数值
	//％hhu：％hhu的占用空间为一个字节
	//％hu：％hu的占用空间为两个字节
	sprintf(timeString, "%.19s.%hu, %s", timeline, timebuffer.millitm, &timeline[20]);
	return timeString;
}







#define LADZY_DEBUG			_DEBUG

#if		LADZY_DEBUG
#define LADZY_DEBUG_FILE	1
#define LADZY_DEBUG_CONSOLE 2
#define LADZY_DEBUG_BOTH	3		//也可以是使用 两个宏的或运算

#define LADZY_DEBUG_MODE	LADZY_DEBUG_BOTH

class errLOGMaster :public winFILE
{
public:
	bool create()
	{
		TSTRING path = AToT(_getcwd(nullptr, 0));
		path += TEXT("\\ErrLog");
		if (!winFILEMaster::Check(path))
		{
			winFILEMaster::CreateDir(path);
		}
		path += TEXT("\\error.txt");

		if (winFILE::open(path, "w"))
		{
			winFILE::insertUnicodePrefix();

			TSTRING timeString = AToT(getTimeString());

			TSTRING message;

			message << TEXT("\nOpening Error Output File (") << path << TEXT(") on ") << timeString << TEXT("\n");

			winFILE::write(message);
			winFILE::flush();

			return true;
		}
		return false;
	}
	~errLOGMaster()
	{
		winFILE::write(TEXT("\nClosing Error Output File."));
	}
};


extern errLOGMaster errLogMaster;


inline void WriteError(const TCHAR* message, ...)
{
	TCHAR err[MAX_BUFFER];
	GetVariableArgument(err, MAX_BUFFER, message);
	errLogMaster.write(err);
}



class errINFOMaster
{
private:
	int DebugNum = 0;
public:
	errINFOMaster();
	~errINFOMaster();
};


#define WriteInConsole		consoleOut.write
#define GetConsoleColor		consoleOut.getColor
#define SetConsoleColor		consoleOut.setColor


#define CONSOLE_LOG()						{WORD oldColor = GetConsoleColor(); WriteInConsole(ERR,"file: %s\nline %d:error code:%d\n\n",TEXT(__FILE__),__LINE__,GetLastError()); SetConsoleColor(oldColor);}
#define CONSOLE_MSG(mode,message, ...)		{WORD oldColor = GetConsoleColor(); WriteInConsole(mode,"file: %s\nline %d:" message "\n\n",TEXT(__FILE__),__LINE__, ##__VA_ARGS__); SetConsoleColor(oldColor);}
#define FILE_LOG()							{WriteError("file: %s\nline %d:error code:%d\n\n",TEXT(__FILE__),__LINE__,GetLastError());}
#define FILE_MSG(mode, message, ...)		{WriteError("file: %s\nline %d:" message "\n\n",TEXT(__FILE__),__LINE__, ##__VA_ARGS__);}
#define BOTH_LOG()							{CONSOLE_LOG(); FILE_LOG()}
#define BOTH_MSG(mode, message, ...)		{CONSOLE_MSG(mode,message, ##__VA_ARGS__);FILE_MSG(mode, message, ##__VA_ARGS__);}

#endif


#if		LADZY_DEBUG
#if		LADZY_DEBUG_MODE == LADZY_DEBUG_CONSOLE
#define ERROR_LOG()						CONSOLE_LOG()
#define ERROR_MSG(mode, message, ...)	CONSOLE_MSG(mode, message, ##__VA_ARGS__)
#elif	LADZY_DEBUG_MODE == LADZY_DEBUG_FILE
#define ERROR_LOG()						FILE_LOG()
#define ERROR_MSG(mode, message, ...)	FILE_MSG(mode, message, ##__VA_ARGS__)
#elif	LADZY_DEBUG_MODE == LADZY_DEBUG_BOTH
#define ERROR_LOG()						BOTH_LOG()
#define ERROR_MSG(mode, message, ...)	BOTH_MSG(mode, message, ##__VA_ARGS__)
#endif
#else
#define ERROR_LOG()					
#define ERROR_MSG(mode, message, ...)	
#endif


