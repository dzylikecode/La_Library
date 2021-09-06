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
*  ����ʱ��: 2021/09/05 11:02:09
*  ����޸�:
*
*  ��    ��: ���������������ģ�
* #define _LA_DEBUG_ON_ ��ʼ����
* Ĭ��ʹ�ÿ���̨�����Ϣ
* #define _LA_DEBUG_FILE_   "error.txt"
* ���������ļ�����
* #define _LA_DEBUG_ON_BOTN_
* ͬʱ���
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


//Ĭ���Ǳ�׼���
#ifndef _LA_DEBUG_FILE_
#define DEBUG_MSG  DEBUG_CONSOLE
#else
#include "La_FileBase.h"
extern FILE* errorFile;
//Ĭ�����ڵ�ǰĿ¼���½�һ���ļ���
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
//�и����ã�������������㣬��������飬��������
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
