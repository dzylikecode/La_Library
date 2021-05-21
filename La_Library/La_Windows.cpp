#include "La_Windows.h"



int MessageBoxPrintf(const TCHAR* szCaption, UINT uType, const TCHAR* szFormat, ...)
{
	TCHAR   szBuffer[MAX_BUFFER];
	GetVariableArgument(szBuffer,MAX_BUFFER, szFormat);
	MessageBeep(MB_ICONQUESTION);
	return MessageBox(NULL, szBuffer, szCaption, uType);
}