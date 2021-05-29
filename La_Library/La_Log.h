#pragma once
#include "La_File.h"
#include "La_Console.h"
#include <sys/timeb.h>
#include <direct.h>

inline char* getTimeString()
{
	static char timeString[280] = { 0 };//AToW �������������Բ�����ʧ
	struct _timeb timebuffer = { 0 };
	//���ص��ַ�����ʽ���£� 
	//Www Mmm dd hh : mm : ss yyyy 
	//Www ��ʾ���ڼ�
	//Mmm ������ĸ��ʾ���·�
	//dd ��ʾһ���еĵڼ���
	//hh : mm: ss ��ʾʱ��
	//yyyy ��ʾ���
	char* timeline = ctime(&(timebuffer.time));
	//��hhu����hhu���������Ϊ�������һ��unsigned short �����͵���ֵ
	//��hu����hu���������Ϊ�������һ��unsigned short int ���͵���ֵ
	//��hhu����hhu��ռ�ÿռ�Ϊһ���ֽ�
	//��hu����hu��ռ�ÿռ�Ϊ�����ֽ�
	sprintf(timeString, "%.19s.%hu, %s", timeline, timebuffer.millitm, &timeline[20]);
	return timeString;
}



class errLOGMaster :public winFILE
{
public:
	errLOGMaster()
	{
		TSTRING path;
		ConvertToTCHAR(path, _getcwd(nullptr, 0));
		path += TEXT("\\ErrLog");
		if (!winFILEMaster::Check(path))
		{
			winFILEMaster::CreateDir(path);
		}
		path += TEXT("\\error.txt");
		winFILE::open(path, "w");
		winFILE::insertUnicodePrefix();

		TSTRING timeString;
		ConvertToTCHAR(timeString, getTimeString());

		TSTRING message;

		message << TEXT("\nOpening Error Output File (") << path << TEXT(") on ") << timeString << TEXT("\n");

		winFILE::write(message);
		winFILE::flush();
	}
	~errLOGMaster()
	{
		winFILE::write(TEXT("\nClosing Error Output File."));
	}
};


extern errLOGMaster errLogMaster;




