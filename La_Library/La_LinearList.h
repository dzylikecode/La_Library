/*
***************************************************************************************
*  ��    ��: 
*
*  ��    ��: LaDzy
*
*  ��    ��: mathbewithcode@gmail.com
*
*  ���뻷��: Visual Studio 2019
*
*  ����ʱ��: 2021/05/20 11:10:46
*  ����޸�: 
*
*  ��    ��: 
*
***************************************************************************************
*/

#ifndef LA_LINEARLIST
#define LA_LINEARLIST

#include <cassert>

template <class T>
class ARRAY
{
private:
	T* list;
	int size;
public:
	ARRAY(int sz = 0);
	ARRAY(const ARRAY<T>& a);
	~ARRAY() { delete[] list; }
	ARRAY<T>& operator = (const ARRAY<T>& rhs);
	T& operator[] (int i) { assert(i >= 0 && i < size); return list[i]; }
	const T& operator[] (int i) const { assert(i >= 0 && i < size); return list[i]; }
	operator T* () { return list; }//��������ת��
	operator const T* () const { return list; }
	int GetSize() const { return size; }
	bool Resize(int sz);
};

template <class T>
ARRAY<T>::ARRAY(int sz)
{
	assert(sz >= 0);
	size = sz;
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
bool ARRAY<T>::Resize(int sz)
{
	assert(sz >= 0);
	if (sz == size)
		return true;
	T* newList = new T[sz];

	if (!newList)
		return false;

	int n = (sz < size) ? sz : size;
	for (int i = 0; i < n; i++)
		newList[i] = list[i];
	delete[]list;
	list = newList;
	size = sz;
	return true;
}

#endif
