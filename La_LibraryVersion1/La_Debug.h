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
*  创建时间: 2021/09/05 11:02:09
*  最后修改:
*
*  简    介: 我想的是设计这样的，
* #define _LA_DEBUG_ON_ 开始调试
* 默认使用控制台输出信息
* #define _LA_DEBUG_FILE_   "error.txt"
* 这会输出到文件当中
* #define _LA_DEBUG_ON_BOTN_
* 同时输出
*
***************************************************************************************
*/

#ifndef _LA_DEBUG_
#define _LA_DEBUG_

#ifdef _LA_DEBUG_ON_

#include "La_Base.h"
#include "La_String.h"
#include "La_IO.h"

#define DEBUG_CONSOLE	WriteInConsole

#define MessageInfo(message, ...)			MessageBoxPrintf(TEXT("Inform"), MB_OK | MB_ICONINFORMATION, message, ##__VA_ARGS__)
#define MessageErr(message, ...)			MessageBoxPrintf(TEXT("Error"), MB_OK | MB_ICONERROR, message, ##__VA_ARGS__)
#define MessageWarn(message, ...)			MessageBoxPrintf(TEXT("Warn"), MB_OK | MB_ICONWARNING, message, ##__VA_ARGS__)

int MessageBoxPrintf(const TCHAR* szCaption, UINT uType, const TCHAR* szFormat, ...);


//默认是标准输出
#ifndef _LA_DEBUG_FILE_
#define DEBUG_MSG  DEBUG_CONSOLE
#else
#include "La_FileBase.h"
extern FILE* errorFile;
//默认是在当前目录下新建一个文件夹
bool CreateErrorFile(const TCHAR* outPath = nullptr);
const TCHAR* GetErrorFilePosition();
void CloseErrorFile();
#define WriteInFile(mode, message, ...)  ftprintf(errorFile, TEXT(message), ##__VA_ARGS__)

#define DEBUG_FILE		WriteInFile
#define DEBUG_MSG		DEBUG_FILE

#endif

#ifdef _LA_DEBUG_ON_BOTH_ 

#ifndef _LA_DEBUG_FILE_
#error "Not define _LA_DEBUG_FILE_"
#endif

#undef  DEBUG_MSG
//有副作用，如果在里面运算，则会算两遍，比如自增
#define DEBUG_MSG(mode, message, ...)  {DEBUG_CONSOLE(mode,message, ##__VA_ARGS__); DEBUG_FILE(mode,message, ##__VA_ARGS__);}
#endif

#else

#define DEBUG_CONSOLE()
#define DEBUG_FILE()
#define DEBUG_MSG()


#define MessageInfo()			
#define MessageErr()			
#define MessageWarn()			

#endif






#endif
