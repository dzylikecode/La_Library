
#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif
#include "La_IO.h"
#include "La_Debug.h"
#include <assert.h>

#define MAX_BUFFER		1024
#define GetVariableArgument(szBuffer,bufferSize, szFormat)   {va_list pArgList;\
															 va_start(pArgList, szFormat);\
															 _vsntprintf_s(szBuffer, bufferSize, szFormat, pArgList);\
															 va_end(pArgList);\
															 }

#define GetVariableArgumentA(szBuffer,bufferSize, szFormat)   {va_list pArgList;\
															 va_start(pArgList, szFormat);\
															 _vsnprintf_s(szBuffer, bufferSize, szFormat, pArgList);\
															 va_end(pArgList);\
															 }

#define GetVariableArgumentW(szBuffer,bufferSize, szFormat)   {va_list pArgList;\
															 va_start(pArgList, szFormat);\
															 _vsnwprintf_s(szBuffer, bufferSize, szFormat, pArgList);\
															 va_end(pArgList);\
															 }


FILE* errorFile = nullptr;

namespace
{
	tstring name;
}

bool CreateErrorFile(const TCHAR* outPath /* = nullptr */)
{
	if (!outPath)	name = TEXT("./Log/Error.txt");
	else			name = outPath;

	size_t pos = name.find_last_of(TEXT("/\\"));
	tstring fileName = name.substr(pos, name.length() - pos);
	tstring filePath = name.substr(0, pos);

	if (_taccess(filePath.c_str(), 0))
		if (_tmkdir(filePath.c_str()) != 0) return false;

	assert(errorFile = _tfopen(name.c_str(), TEXT("w+")));
	return true;
}

const TCHAR* GetErrorFilePosition() { return name.c_str(); }

void CloseErrorFile()
{
	if (errorFile)
	{
		fclose(errorFile);
		errorFile = nullptr;
	}
}


int MessageBoxPrintf(const TCHAR* szCaption, UINT uType, const TCHAR* szFormat, ...)
{
	TCHAR   szBuffer[MAX_BUFFER];
	GetVariableArgument(szBuffer, MAX_BUFFER, szFormat);
	MessageBeep(MB_ICONQUESTION);
	return MessageBox(NULL, szBuffer, szCaption, uType);
}
