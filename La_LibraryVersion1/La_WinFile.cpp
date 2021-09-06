#include "La_WinFile.h"

namespace winFILEMaster
{
	//支持通配符
	bool EnumFiles(LPCTSTR lpszPath, LPCTSTR lpszType, winFILELIST& fileList)
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		_stprintf(szPath, TEXT("%s\\%s"), lpszPath, lpszType);

		////效率有些低好吧，要先得到信息，再复制到链表当中
		////不能先到链表中创建，然后得到消息，这样就不用复制了
		//WIN32_FIND_DATA FindData;
		WIN32_FIND_DATA FindData;

		HANDLE hFind = FindFirstFile(szPath, &FindData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		fileList.push_back(FindData);

		bool bRet = false;
		do
		{
			bRet = FindNextFile(hFind, &FindData);
			if (!bRet) break;

			fileList.push_back(FindData);

		} while (bRet);

		return true;
	}

	bool EnumFilesAll(LPCTSTR lpszPath, LPCTSTR lpszType, winFILELIST& fileList)
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		_stprintf(szPath, TEXT("%s\\%s"), lpszPath, lpszType);

		////效率有些低好吧，要先得到信息，再复制到链表当中
		////不能先到链表中创建，然后得到消息，这样就不用复制了
		//WIN32_FIND_DATA FindData;
		WIN32_FIND_DATA FindData;

		HANDLE hFind = FindFirstFile(szPath, &FindData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		fileList.push_back(FindData);

		bool bRet = false;
		do
		{
			bRet = FindNextFile(hFind, &FindData);
			if (!bRet) break;

			//注意要排除两个特殊的目录
				//如果第一次搜索是从父目录开始，那么父目录也有 .. 和 .
			if (_tcscmp(FindData.cFileName, TEXT(".")) == 0 || _tcscmp(FindData.cFileName, TEXT("..")) == 0) continue;
			if ((FindData.dwFileAttributes & DIR))
			{
				//文件夹 可以递归地查找下去
				TCHAR szChildPath[MAX_PATH] = { 0 };
				_stprintf(szChildPath, TEXT("%s\\%s"), lpszPath, FindData.cFileName);
				EnumFilesAll(szChildPath, lpszType, fileList);
			}
			else
			{
				fileList.push_back(FindData);
			}
		} while (bRet);
		return true;
	}
}