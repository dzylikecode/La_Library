#include "La_File.h"
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
		fileNODE* FindData = new fileNODE;

		HANDLE hFind = FindFirstFile(szPath, &FindData->data);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			delete FindData;
			return false;
		}

		fileList.insertBefore(FindData);

		bool bRet = false;
		do
		{
			FindData = new fileNODE;
			bRet = FindNextFile(hFind, &FindData->data);
			if (!bRet) break;

			fileList.insertBefore(FindData);

		} while (bRet);

		delete FindData;
		fileList.reset(0);
		return true;
	}

	bool EnumFilesAll(LPCTSTR lpszPath, LPCTSTR lpszType, winFILELIST& fileList)
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		_stprintf(szPath, TEXT("%s\\%s"), lpszPath, lpszType);

		////效率有些低好吧，要先得到信息，再复制到链表当中
		////不能先到链表中创建，然后得到消息，这样就不用复制了
		//WIN32_FIND_DATA FindData;
		fileNODE* FindData = new fileNODE;

		HANDLE hFind = FindFirstFile(szPath, &FindData->data);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			delete FindData;
			return false;
		}

		fileList.insertBefore(FindData);

		bool bRet = false;
		do
		{
			FindData = new fileNODE;
			bRet = FindNextFile(hFind, &FindData->data);
			if (!bRet) break;

			//注意要排除两个特殊的目录
				//如果第一次搜索是从父目录开始，那么父目录也有 .. 和 .
			if (_tcscmp(FindData->data.cFileName, TEXT(".")) == 0 || _tcscmp(FindData->data.cFileName, TEXT("..")) == 0) continue;
			if ((FindData->data.dwFileAttributes & DIR))
			{
				//文件夹 可以递归地查找下去
				TCHAR szChildPath[MAX_PATH] = { 0 };
				_stprintf(szChildPath, TEXT("%s\\%s"), lpszPath, FindData->data.cFileName);
				EnumFilesAll(szChildPath, lpszType, fileList);
			}
			else
			{
				fileList.insertBefore(FindData);
			}
		} while (bRet);

		//最后一个必然失败
		delete FindData;
		fileList.reset(0);
		return true;
	}
}