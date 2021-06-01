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

template <class T, int endFlag = 0>
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
	STRING(int len) :ARRAY<T>(len + 1), length(len + 1) { zero(); }
	bool isEnd(const T word) { return word == endFlag; }
	int getLength(void)const { return length; }
	void UpdateLen() { length = getLength(*this); }//当使用指针往里面写字符的时候不会更新 length ，需要自己手动
	int getLength(const T* source)
	{
		if (!source)
		{
			return 0;
		}
		int i = 0;
		while (!isEnd(*source++))
			i++;
		return i;
	}
	STRING<T, endFlag>& operator+=(const T* source)//可以类型转化,故不需要用类接受参数
	{
		length = getLength(*this);
		int newSize = length + getLength(source) + 1;
		assert(this->ARRAY<T>::resize(newSize));
		for (int i = length; i < newSize; i++)
			(*this)[i] = *source++;

		length = newSize - 1;
		return *this;
	}
	friend STRING<T, endFlag> operator+(const T* source, const STRING<T, endFlag>& sourceString)
	{
		return sourceString + source;
	}
	STRING<T, endFlag>  operator+ (const T* source) { STRING<T, endFlag> temp = *this; return temp += source; }
	STRING<T, endFlag>& operator<<(const T* source)
	{
		return (*this += source);
	}
	void zero(void)
	{
		for (int i = 0; i < this->getSize(); i++)
			(*this)[i] = endFlag;
	}
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
				(*this)[0] = endFlag;
				length = 0;
				return true;
			}

			if (size < oldSize)
			{
				length = oldSize - 1;
				(*this)[length] = endFlag;
			}
		}
		
		return false;
	}
	bool relength(int len) { return resize(len + 1); }
	bool operator==(const T* target)
	{
		const T* temp = *this;
		while (!isEnd(*target) || !isEnd(*temp))
			if (*temp++ != *target++)
				return false;
		return true;
	}
	friend bool operator==(const T* target, const STRING<T, endFlag>& source)
	{
		return source == target;
	}
	bool operator!=(const T* target) { return !(*this == target); }
	friend bool operator!=(const T* target, const STRING<T, endFlag>& source)
	{
		return source != target;
	}
};

#endif