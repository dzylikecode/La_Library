#include "La_WinFile.h"

namespace winFILEMaster
{
	//֧��ͨ���
	bool EnumFiles(LPCTSTR lpszPath, LPCTSTR lpszType, winFILELIST& fileList)
	{
		TCHAR szPath[MAX_PATH] = { 0 };
		_stprintf(szPath, TEXT("%s\\%s"), lpszPath, lpszType);

		////Ч����Щ�ͺðɣ�Ҫ�ȵõ���Ϣ���ٸ��Ƶ�������
		////�����ȵ������д�����Ȼ��õ���Ϣ�������Ͳ��ø�����
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

		////Ч����Щ�ͺðɣ�Ҫ�ȵõ���Ϣ���ٸ��Ƶ�������
		////�����ȵ������д�����Ȼ��õ���Ϣ�������Ͳ��ø�����
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

			//ע��Ҫ�ų����������Ŀ¼
				//�����һ�������ǴӸ�Ŀ¼��ʼ����ô��Ŀ¼Ҳ�� .. �� .
			if (_tcscmp(FindData.cFileName, TEXT(".")) == 0 || _tcscmp(FindData.cFileName, TEXT("..")) == 0) continue;
			if ((FindData.dwFileAttributes & DIR))
			{
				//�ļ��� ���Եݹ�ز�����ȥ
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