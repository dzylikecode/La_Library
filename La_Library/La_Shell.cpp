#include "La_Shell.h"

bool SHELL::Open(LPCTSTR fileNameOrDirectory, LPCTSTR parameters)
{
	lpVerb = TEXT("open");
	lpFile = fileNameOrDirectory;
	lpParameters = parameters;
	bool result = ShellExecuteEx(this);
	lpParameters = nullptr;
	return result;
}

bool SHELL::Run(LPCTSTR fileName, LPCTSTR parameters)
{
	lpVerb = TEXT("runas");
	lpFile = fileName;
	lpParameters = parameters;
	bool result = ShellExecuteEx(this);
	lpParameters = nullptr;
	return result;
}

bool SHELL::Explore(LPCTSTR Directory)
{
	lpVerb = TEXT("explore");
	lpFile = Directory;
	return ShellExecuteEx(this);
}
bool SHELL::Print(LPCTSTR fileName)
{
	lpVerb = TEXT("print");
	lpFile = fileName;
	return ShellExecuteEx(this);
}

bool SHELL::Edit(LPCTSTR fileName)
{
	lpVerb = TEXT("edit");
	lpFile = fileName;
	return ShellExecuteEx(this);
}

bool SHELL::Select(LPCTSTR fileNameOrDirectory)
{
	TSTRING string(TEXT("/select,"));
	string += fileNameOrDirectory;
	return Open(TEXT("explorer.exe"), string);
}