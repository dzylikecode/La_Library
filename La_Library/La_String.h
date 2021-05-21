/*
***************************************************************************************
*  程    序: 
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/05/20 11:10:35
*  最后修改: 
*
*  简    介: 
*
***************************************************************************************
*/

#ifndef LA_STRING
#define LA_STRING

#include "La_LinearList.h"

template <class T>
class STRING :public ARRAY<T>
{
private:
	int lenth;
public:
	STRING(void) :lenth(0) {};
	//调用了默认 ARRAY 初始化，但是同时要注意 length 也要初始化，然后 this 就初始化完成
	//所以要注意，冒号后面才是初始化
	STRING(const T* source) :lenth(0) { *this += source; }//可以类型转化，相当于等于
	STRING(const STRING<T>& source) :ARRAY<T>(source) { lenth = source.lenth; }
	STRING(int len) :ARRAY<T>(len + 1), lenth(len + 1) { Zero(); }
	int StrLen(void)const { return lenth; }
	int StrLen(const T* source);
	STRING<T>& operator+=(const T* source);//可以类型转化,故不需要用类接受参数
	friend STRING<T> operator+(const T* source, const STRING<T>& sourceString);
	STRING<T>  operator+ (const T* source) { STRING<T> temp = *this; return temp += source; }
	void Zero(void);
	void Empty(void) { this->ARRAY::Empty(); lenth = 0; }

	bool operator==(const T* target);
	friend bool operator==(const T* target, const STRING<T>& source);
	bool operator!=(const T* target) { return !(*this == target); }
	friend bool operator!=(const T* target, const STRING<T>& source);
};


template <class T>
int STRING<T>::StrLen(const T* source)
{
	int i = 0;
	while (*source++)
		i++;
	return i;
}

template <class T>
STRING<T>& STRING<T>::operator+=(const T* source)
{
	int newSize = lenth + StrLen(source) + 1;
	assert(this->Resize(newSize));
	for (int i = lenth; i < newSize; i++)
		(*this)[i] = *source++;

	lenth = newSize - 1;
	return *this;
}
template <class T>
inline STRING<T> operator+(const T* source, const STRING<T>& sourceString)
{
	return sourceString + source;
}

template <class T>
void STRING<T>::Zero(void)
{
	for (int i = 0; i < this->GetSize(); i++)
		(*this)[i] = '\0';
}

template <class T>
bool STRING<T>::operator==(const T* target)
{
	const T* temp = *this;
	while (*target || *temp)
		if (*temp++ != *target++)
			return false;
	return true;
}

template <class T>
inline bool operator==(const T* target, const STRING<T>& source)
{
	return source == target;
}

template <class T>
inline bool operator!=(const T* target, const STRING<T>& source)
{
	return source != target;
}

#endif