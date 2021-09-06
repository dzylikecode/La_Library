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
*@brief：不同编码字符串转Unicode
*@pram：cpMbs：多字节字符串；wcpWcs：宽字符串；wcsBuffLen：宽字符串缓冲区大小（单位宽字符）；dEncodeType：多字节字符串编码类型，0：GBK，1：UTF8
*@ret:-1:出错；>=0：转换成功的字符个数
*@birth:created by dablelv on 20170804
*@revision:
********************************************/
int mbs2wcs(const char* cpMbs, wchar_t* wcpWcs, int wcsBuffLen, int dEncodeType)
{
	if (NULL == cpMbs || 0 == strlen(cpMbs)) return 0;

	//GBK转Unicode
	if (0 == dEncodeType)
	{
		if (NULL == setlocale(LC_ALL, "zh_CN.gbk"))		//设置转换为unicode前的编码为gbk编码		
			return -1;
	}
	//UTF8转Unicode
	if (1 == dEncodeType)
	{
		if (NULL == setlocale(LC_ALL, "zh_CN.utf8"))	//设置转换为unicode前的编码为utf8编码
			return -1;
	}

	int unicodeCNum = mbstowcs(NULL, cpMbs, 0);				//计算待转换的字符数
	if (unicodeCNum <= 0 || unicodeCNum >= wcsBuffLen)			//转换失败或宽字符串缓冲区大小不足
	{
		return -1;
	}
	unicodeCNum = mbstowcs(wcpWcs, cpMbs, wcsBuffLen - 1);	//进行转换，wcsBuffLen-1表示最大待转换的宽字符数，即宽字符串缓冲区大小
	return unicodeCNum;
}

/********************************************
*@brief：Unicode转指定编码字符串
*@pram：wcpWcs：宽字符串；cpMbs：多字节字符串缓冲区；dBuffLen：多字节字符串缓冲区大小（单位字节）；dEncodeType：多字节字符串编码类型，0：GBK，1：UTF8
*@ret:-1:出错；>=0：转换成功的字节个数
*@birth:created by dablelv on 20180114
*@revision:
********************************************/
int wcs2mbs(const wchar_t* wcpWcs, char* cpMbs, int dBuffLen, int dEncodeType)
{
	if (wcpWcs == NULL || wcslen(wcpWcs) == 0)
	{
		return 0;
	}

	//Unicode转GBK
	if (0 == dEncodeType)
	{
		if (NULL == setlocale(LC_ALL, "zh_CN.gbk"))		//设置目标字符串编码为gbk编码
			return -1;
	}
	//Unicode转UTF8
	if (1 == dEncodeType)
	{
		if (NULL == setlocale(LC_ALL, "zh_CN.utf8"))	//设置目标字符串编码为utf8编码
			return -1;
	}

	int dResultByteNum = wcstombs(NULL, wcpWcs, 0);		//计算待转换的字节数
	if (dResultByteNum <= 0 || dResultByteNum >= dBuffLen)
	{
		return -1;									//转换失败或多字节字符串缓冲区大小不足
	}
	wcstombs(cpMbs, wcpWcs, dBuffLen - 1);
	return dResultByteNum;
}
#endif


#endif
