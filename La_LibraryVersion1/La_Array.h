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
*  创建时间: 2021/05/20 11:10:46
*  最后修改:
*
*  简    介:
*
***************************************************************************************
*/

#ifndef _LA_ARRAY_
#define _LA_ARRAY_
#include <assert.h>

template <class T>
class ARRAY
{
private:
	T* list;
	int size;
public:
	ARRAY(int sz = 0);
	ARRAY(const ARRAY<T>& a);
	~ARRAY() { clear(); }
	ARRAY<T>& operator = (const ARRAY<T>& rhs);
	T& operator[] (int i) { assert(i >= 0 && i < size); return list[i]; }
	const T& operator[] (int i) const { assert(i >= 0 && i < size); return list[i]; }
	operator T* () { return list; }//重载类型转化
	operator const T* () const { return list; }
	int getSize() const { return size; }
	void clear()
	{
		if (list)
		{
			delete[] list;
			list = nullptr;
			size = 0;
		}
	}
	bool resize(int sz, bool bStore = false); //默认不保留原来的内容
};

template <class T>
ARRAY<T>::ARRAY(int sz) :list(nullptr)
{
	size = sz;
	if (sz == 0)
		return;
	assert(sz > 0);
	list = new T[size];
}

template <class T>
ARRAY<T>::ARRAY(const ARRAY<T>& a)
{
	size = a.size;
	list = new T[size];
	for (int i = 0; i < size; i++)
		list[i] = a.list[i];
}

template <class T>
ARRAY<T>& ARRAY<T>::operator =(const ARRAY<T>& rhs)
{
	if (&rhs != this)
	{
		if (size != rhs.size)
		{
			delete[]list;
			size = rhs.size;
			list = new T[size];
		}
		for (int i = 0; i < size; i++)
			list[i] = rhs.list[i];
	}
	return *this;
}

template <class T>
bool ARRAY<T>::resize(int sz, bool bStore)
{
	if (sz == size)
		return true;

	if (sz == 0)
	{
		size = 0;
		delete[] list;
		list = nullptr;
		return true;
	}
	assert(sz > 0);
	T* newList = new T[sz];

	if (!newList)
		return false;

	if (bStore)
	{
		int n = (sz < size) ? sz : size;
		for (int i = 0; i < n; i++)
			newList[i] = list[i];
	}

	delete[]list;
	list = newList;
	size = sz;
	return true;
}


#endif
