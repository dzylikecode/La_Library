#ifndef _LA_TIME_BASE_
#include "La_Base.h"
#include <time.h>

//inline char* getTimeString()
//{
//	static char timeString[280] = { 0 };//AToW 会引用它，所以不能消失
//	struct _timeb timebuffer = { 0 };
//	//返回的字符串格式如下： 
//	//Www Mmm dd hh : mm : ss yyyy 
//	//Www 表示星期几
//	//Mmm 是以字母表示的月份
//	//dd 表示一月中的第几天
//	//hh : mm: ss 表示时间
//	//yyyy 表示年份
//	char* timeline = ctime(&(timebuffer.time));
//	//％hhu：％hhu的输出类型为用于输出一个unsigned short 短整型的数值
//	//％hu：％hu的输出类型为用于输出一个unsigned short int 整型的数值
//	//％hhu：％hhu的占用空间为一个字节
//	//％hu：％hu的占用空间为两个字节
//	sprintf(timeString, "%.19s.%hu, %s", timeline, timebuffer.millitm, &timeline[20]);
//	return timeString;
//}
#endif 





