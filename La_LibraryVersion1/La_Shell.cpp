#include "La_Shell.h"
#include "La_String.h"

bool SHELL::open(LPCTSTR fileNameOrDirectory, LPCTSTR parameters)
{
	lpVerb = TEXT("open");
	lpFile = fileNameOrDirectory;
	lpParameters = parameters;
	bool result = ShellExecuteEx(this);
	lpParameters = nullptr;
	return result;
}

bool SHELL::run(LPCTSTR fileName, LPCTSTR parameters)
{
	lpVerb = TEXT("runas");
	lpFile = fileName;
	lpParameters = parameters;
	bool result = ShellExecuteEx(this);
	lpParameters = nullptr;
	return result;
}

bool SHELL::explore(LPCTSTR Directory)
{
	lpVerb = TEXT("explore");
	lpFile = Directory;
	return ShellExecuteEx(this);
}
bool SHELL::print(LPCTSTR fileName)
{
	lpVerb = TEXT("print");
	lpFile = fileName;
	return ShellExecuteEx(this);
}

bool SHELL::edit(LPCTSTR fileName)
{
	lpVerb = TEXT("edit");
	lpFile = fileName;
	return ShellExecuteEx(this);
}

bool SHELL::select(LPCTSTR fileNameOrDirectory)
{
	TSTRING string(TEXT("/select,"));
	string += fileNameOrDirectory;
	return open(TEXT("explorer.exe"), string);
}