#ifndef _LA_STRING_BASE_
#define _LA_STRING_BASE_
#include "La_Base.h"


#ifdef WIN32
#include <tchar.h>


#else
#ifdef UNICODE
typedef  wchar_t  TCHAR;
#else
typedef  char     TCHAR;
#endif


#endif

#if 0
#include <locale.h>
#include <stdlib.h>

/********************************************
*@brief����ͬ�����ַ���תUnicode
*@pram��cpMbs�����ֽ��ַ�����wcpWcs�����ַ�����wcsBuffLen�����ַ�����������С����λ���ַ�����dEncodeType�����ֽ��ַ����������ͣ�0��GBK��1��UTF8
*@ret:-1:����>=0��ת���ɹ����ַ�����
*@birth:created by dablelv on 20170804
*@revision:
********************************************/
int mbs2wcs(const char* cpMbs, wchar_t* wcpWcs, int wcsBuffLen, int dEncodeType)
{
	if (NULL == cpMbs || 0 == strlen(cpMbs)) return 0;

	//GBKתUnicode
	if (0 == dEncodeType)
	{
		if (NULL == setlocale(LC_ALL, "zh_CN.gbk"))		//����ת��Ϊunicodeǰ�ı���Ϊgbk����		
			return -1;
	}
	//UTF8תUnicode
	if (1 == dEncodeType)
	{
		if (NULL == setlocale(LC_ALL, "zh_CN.utf8"))	//����ת��Ϊunicodeǰ�ı���Ϊutf8����
			return -1;
	}

	int unicodeCNum = mbstowcs(NULL, cpMbs, 0);				//�����ת�����ַ���
	if (unicodeCNum <= 0 || unicodeCNum >= wcsBuffLen)			//ת��ʧ�ܻ���ַ�����������С����
	{
		return -1;
	}
	unicodeCNum = mbstowcs(wcpWcs, cpMbs, wcsBuffLen - 1);	//����ת����wcsBuffLen-1��ʾ����ת���Ŀ��ַ����������ַ�����������С
	return unicodeCNum;
}

/********************************************
*@brief��Unicodeתָ�������ַ���
*@pram��wcpWcs�����ַ�����cpMbs�����ֽ��ַ�����������dBuffLen�����ֽ��ַ�����������С����λ�ֽڣ���dEncodeType�����ֽ��ַ����������ͣ�0��GBK��1��UTF8
*@ret:-1:����>=0��ת���ɹ����ֽڸ���
*@birth:created by dablelv on 20180114
*@revision:
********************************************/
int wcs2mbs(const wchar_t* wcpWcs, char* cpMbs, int dBuffLen, int dEncodeType)
{
	if (wcpWcs == NULL || wcslen(wcpWcs) == 0)
	{
		return 0;
	}

	//UnicodeתGBK
	if (0 == dEncodeType)
	{
		if (NULL == setlocale(LC_ALL, "zh_CN.gbk"))		//����Ŀ���ַ�������Ϊgbk����
			return -1;
	}
	//UnicodeתUTF8
	if (1 == dEncodeType)
	{
		if (NULL == setlocale(LC_ALL, "zh_CN.utf8"))	//����Ŀ���ַ�������Ϊutf8����
			return -1;
	}

	int dResultByteNum = wcstombs(NULL, wcpWcs, 0);		//�����ת�����ֽ���
	if (dResultByteNum <= 0 || dResultByteNum >= dBuffLen)
	{
		return -1;									//ת��ʧ�ܻ���ֽ��ַ�����������С����
	}
	wcstombs(cpMbs, wcpWcs, dBuffLen - 1);
	return dResultByteNum;
}
#endif


#endif
