#ifndef _LA_MASTER_
#define _LA_MASTER_


#include "La_Debug.h"
#ifdef _LA_DEBUG_ON_

#ifdef _LA_DEBUG_ON_BOTH_

class DEBUG_MASTER
{
public:
	DEBUG_MASTER()
	{
		consoleOut.create(TEXT("Debug"));
		CreateErrorFile(AToT(_LA_DEBUG_FILE_));
		WriteInConsole(TIME, "-----------------------%s---------------------------\n", TEXT(__DATE__));
		WriteInConsole(TIME, "\n-----------------------%s---------------------------\n", TEXT(__TIME__));
		WriteInConsole(NOTICE, "*****************Debug Start in file and console***********\n");
		WriteInConsole(NOTICE, "\n\nDebug File Message:\n");
		WriteInConsole(NOTICE, "File Name  ---> %s\n\n\n", GetErrorFilePosition());
	}
	~DEBUG_MASTER()
	{
		WriteInConsole(TIME, "\n-----------------------%s---------------------------\n", TEXT(__TIME__));
		WriteInConsole(NOTICE, "*****************Debug End in file and console***********\n");
		CloseErrorFile();
		consoleOut.close();
	}
} debugMaster;

#elif _LA_DEBUG_FILE_
class DEBUG_MASTER
{
public:
	DEBUG_MASTER() { CreateErrorFile(AToT(_LA_DEBUG_FILE_)); }
	~DEBUG_MASTER() { CloseErrorFile(); }
} debugMaster;
#else

class DEBUG_MASTER
{
public:
	DEBUG_MASTER()
	{
		consoleOut.create(TEXT("Debug"));
		WriteInConsole(TIME, "-----------------------%s---------------------------\n", TEXT(__DATE__));
		WriteInConsole(TIME, "\n-----------------------%s---------------------------\n", TEXT(__TIME__));
		WriteInConsole(NOTICE, "*****************Debug Start in console********************\n");
	}
	~DEBUG_MASTER()
	{
		WriteInConsole(TIME, "\n-----------------------%s---------------------------\n", TEXT(__TIME__));
		WriteInConsole(NOTICE, "*****************Debug End in console********************\n");
		consoleOut.close();
	}
} debugMaster;

#endif


#endif // !_LA_DEBUG_ON_


//毫无作用
//#ifdef UNICODE
//#include <fcntl.h>
//#include <io.h>
//#include "La_Base.h"
//class UNICODE_MASTER { public: UNICODE_MASTER() { setlocale(LC_CTYPE, ""); } };//必须得有这行 否则不能输出中文
//#endif

#endif // !_LA_MASTER_