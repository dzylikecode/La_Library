#define _LA_DEBUG_ON_
#include "La_Debug.h"
#include "La_Master.h"
int main(int argc, char* argv[])
{
	SetUnicodeInMainFunciton();
	DEBUG_MSG(TIME, "Äêºó");
	return argc;
}