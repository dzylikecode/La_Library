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
*  创建时间: 2021/05/27 17:26:41
*  最后修改: 
*
*  简    介: 
*
***************************************************************************************
*/

#pragma once
#include "La_LinearList.h"
#include "La_MathBase.h"
#include <assert.h>
template <typename T>
class VECTOR :public ARRAY<T>
{
public:
	T operator*(const VECTOR<T>& v1)const;
	void zero();
};

template <typename T>
T VECTOR<T>::operator*(const VECTOR<T>& v1)const
{
	assert(this->getSize() == v1.getSize());
	T sum = 0;
	for (int i = 0; i < this->getSize(); i++)
	{
		sum += *this[i] * v1[i];
	}
	return sum;
}

template <typename T>
void VECTOR<T>::zero()
{
	for (int i = 0; i < this->getSize(); i++)
	{
		*this[i] = 0;
	}
}

template <typename T>
class hVECTOR :public VECTOR<T>
{

};


template <typename T>
class vVECTOR :public VECTOR<T>
{

};

template <typename T>
REAL Distance(T x, T y) { return sqrt(x * x + y * y); }
template <typename T>
REAL Distance(T x, T y, T z) { return sqrt(x * x + y * y + z * z); }


int DistanceFast(int x, int y);
REAL DistanceFast(REAL x, REAL y);
REAL DistanceFast(REAL x, REAL y, REAL z);

