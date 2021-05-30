#include "La_Log.h"

#if LADZY_DEBUG
errLOGMaster errLogMaster;
CONSOLE errConsoleMaster;
errINFOMaster errInfoMaster;

errINFOMaster::errINFOMaster()
{
#if LADZY_DEBUG
	if (DebugNum++)
	{
		return;
	}
#if LADZY_DEBUG_MODE == LADZY_DEBUG_CONSOLE
	errConsoleMaster.create();
	WriteInConsole(TIME, TEXT("-----------------------%s---------------------------\n"), TEXT(__DATE__));
	WriteInConsole(TIME, TEXT("\n-----------------------%s---------------------------\n"), TEXT(__TIME__));
	WriteInConsole(NOTICE, TEXT("*****************Debug Start in console********************\n"));
	errConsoleMaster.restoreColor();
	return TRUE;
#elif LADZY_DEBUG_MODE == LADZY_DEBUG_FILE
	errLogMaster.create();
#elif LADZY_DEBUG_MODE == LADZY_DEBUG_BOTH
	errConsoleMaster.create();
	WriteInConsole(TIME, TEXT("-----------------------%s---------------------------\n"), TEXT(__DATE__));
	WriteInConsole(TIME, TEXT("\n-----------------------%s---------------------------\n"), TEXT(__TIME__));
	WriteInConsole(NOTICE, TEXT("*****************Debug Start in file and console***********\n"));
	if (errLogMaster.create())
	{
		WriteInConsole(NOTICE, TEXT("\n\nDebug File Message:\n"));
		//WriteInConsole(WARN, "\tWork Directory: %s \n\tFile Name  ---> %s\n\n\n", _getcwd(NULL, 0), ".\\ErrorLog\\Error.txt");
		WriteInConsole(NOTICE, TEXT("File Name  ---> %s\n\n\n"), errLogMaster.getFileName());
	}
	else
	{
		WriteInConsole(WARN, TEXT("\n\n Debug File Message:\n"));
		WriteInConsole(ERR, TEXT("\t\t>>>>>>>>>>>>>Create ' Error Log ' Failed<<<<<<<<<<<<<\n"));
	}
#endif
#endif
}
errINFOMaster::~errINFOMaster()
{
#if LADZY_DEBUG
	if (!(DebugNum--))//当前为1，则说明准备释放，不会提示，如果当前已经为0，则会提示
	{
		MessageErr(TEXT("Debug End!"));
		return;
	}
#if LADZY_DEBUG_MODE == LADZY_DEBUG_CONSOLE

	WriteInConsole(TIME, TEXT("\n-----------------------%s---------------------------\n"), TEXT(__TIME__));
	WriteInConsole(NOTICE, TEXT("*****************Debug End in console********************\n"));
#elif LADZY_DEBUG_MODE == LADZY_DEBUG_FILE

#elif LADZY_DEBUG_MODE == LADZY_DEBUG_BOTH
	WriteInConsole(TIME, TEXT("\n-----------------------%s---------------------------\n"), TEXT(__TIME__));
	WriteInConsole(NOTICE, TEXT("*****************Debug End in file and console***********\n"));
#endif
#endif
}


#endif