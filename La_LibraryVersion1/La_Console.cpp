#include "LA_Console.h"

CONSOLE consoleOut;

bool CONSOLE::create(const TCHAR* name)
{
	AllocConsole();

	if (!name) setTitle(TEXT("New Console"));
	else       setTitle(name);

	freopen_s(&stream, "conin$", "r+t", stdin);//重定向输入流
	freopen_s(&stream, "conout$", "w+t", stdout);//重定向输入流
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	return true;
}