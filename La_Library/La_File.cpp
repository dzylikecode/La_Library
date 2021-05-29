#include "La_File.h"
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

		////Ч����Щ�ͺðɣ�Ҫ�ȵõ���Ϣ���ٸ��Ƶ�������
		////�����ȵ������д�����Ȼ��õ���Ϣ�������Ͳ��ø�����
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

			//ע��Ҫ�ų����������Ŀ¼
				//�����һ�������ǴӸ�Ŀ¼��ʼ����ô��Ŀ¼Ҳ�� .. �� .
			if (_tcscmp(FindData->data.cFileName, TEXT(".")) == 0 || _tcscmp(FindData->data.cFileName, TEXT("..")) == 0) continue;
			if ((FindData->data.dwFileAttributes & DIR))
			{
				//�ļ��� ���Եݹ�ز�����ȥ
				TCHAR szChildPath[MAX_PATH] = { 0 };
				_stprintf(szChildPath, TEXT("%s\\%s"), lpszPath, FindData->data.cFileName);
				EnumFilesAll(szChildPath, lpszType, fileList);
			}
			else
			{
				fileList.insertBefore(FindData);
			}
		} while (bRet);

		//���һ����Ȼʧ��
		delete FindData;
		fileList.reset(0);
		return true;
	}
}