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
*  ����ʱ��: 2021/05/20 11:44:59
*  ����޸�:
*
*  ��    ��:
*
***************************************************************************************
*/
#ifndef _LA_SHELL_
#define _LA_SHELL_

#include "La_WindowBase.h"
class SHELL :private SHELLEXECUTEINFO
{
public:
	SHELL(void) { clear(); }
	//Open(TEXT("explorer.exe"), TEXT("::{20d04fe0-3aea-1069-a2d8-08002b30309d}")); �� �ҵĵ���
	bool open(LPCTSTR fileNameOrDirectory, LPCTSTR parameters = nullptr);
	bool run(LPCTSTR fileName, LPCTSTR parameters = nullptr);//�Թ���ԱȨ������
	bool explore(LPCTSTR directory);
	bool print(LPCTSTR fileName);//�����ĵ�
	bool edit(LPCTSTR fileName);
	bool select(LPCTSTR fileNameOrDirectory);
	void setShowStyle(int showStyle) { nShow = showStyle; }
	void clear(void)
	{
		memset((SHELLEXECUTEINFO*)this, 0, sizeof(SHELLEXECUTEINFO));
		cbSize = sizeof(SHELLEXECUTEINFO);
		nShow = SW_SHOW;
	}
	void setWorkDirectory(LPCTSTR directory) { lpDirectory = directory; }
};

#endif // !_LA_SHELL_