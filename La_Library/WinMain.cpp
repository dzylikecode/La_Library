#define _CRT_SECURE_NO_WARNINGS
#include "La_Windows.h"
#include "La_Shell.h"


int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
{
	/*HRESULT hRet = URLDownloadToFile(NULL, _T("https://docs.microsoft.com/en-us/previous-versions/windows/internet-explorer/ie-developer/platform-apis/ms775123(v=vs.85)"), _T("G:\\123.html"), 0, NULL);
	if (hRet == S_OK)
	{
		MessageBox(NULL, _T("Download successfully"), _T("Information"), MB_OK | MB_ICONERROR);
	}
	else
	{
		DWORD ErrorCode = GetLastError();
		TCHAR buff[30] = {0};
		_stprintf(buff, _T("Error code: %d"), ErrorCode);
		MessageBox(NULL, buff, _T("Information"), MB_OK);
	}*/

	SHELL shell;
	bool ShellRet = shell.Select(TEXT("C:\\Windows\\System32\\cmd.exe"));

	if (!ShellRet)
	{
		DWORD ErrorCode = GetLastError();
		TCHAR buff[30] = { 0 };
		_stprintf(buff, _T("Error code: %d"), ErrorCode);
		MessageBox(NULL, buff, _T("Information"), MB_OK | MB_ICONERROR);
	}
	return 0;
}