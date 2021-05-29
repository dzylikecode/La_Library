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
	int length;
public:
	STRING(void) :length(0) {};
	//调用了默认 ARRAY 初始化，但是同时要注意 length 也要初始化，然后 this 就初始化完成
	//所以要注意，冒号后面才是初始化
	STRING(const T* source) :length(0) { *this += source; }//可以类型转化，相当于等于
	STRING(const STRING<T>& source) :ARRAY<T>(source) { length = source.length; }
	STRING(int len) :ARRAY<T>(len + 1), length(len + 1) { Zero(); }
	int StrLen(void)const { return length; }
	void UpdateLen() { length = StrLen(*this); }//当使用指针往里面写字符的时候不会更新 length ，需要自己手动
	int StrLen(const T* source);
	STRING<T>& operator+=(const T* source);//可以类型转化,故不需要用类接受参数
	friend STRING<T> operator+(const T* source, const STRING<T>& sourceString);
	STRING<T>  operator+ (const T* source) { STRING<T> temp = *this; return temp += source; }
	void Zero(void);
	void clear(void) { this->ARRAY<T>::clear(); length = 0; }

	//如果比原来的小，那么最后一个字节会变成空字符
	bool resize(int size)
	{
		int oldSize = this->getSize();
		if (this->ARRAY<T>::resize(size))
		{
			if (size == 0)
			{
				length = 0;
				return true;
			}

			//一开始没有字符串，担心 size >= oldSize 时，没处理
			if (oldSize == 0)
			{
				(*this)[0] = '\0';
				length = 0;
				return true;
			}

			if (size < oldSize)
			{
				length = oldSize - 1;
				(*this)[length] = '\0';
			}
		}
		
		return false;
	}
	bool ReLen(int len) { return resize(len + 1); }
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
	int newSize = length + StrLen(source) + 1;
	assert(this->ARRAY<T>::resize(newSize));
	for (int i = length; i < newSize; i++)
		(*this)[i] = *source++;

	length = newSize - 1;
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
	for (int i = 0; i < this->getSize(); i++)
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