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
*  ����ʱ��: 2021/05/20 11:10:35
*  ����޸�: 
*
*  ��    ��: 
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
	//������Ĭ�� ARRAY ��ʼ��������ͬʱҪע�� length ҲҪ��ʼ����Ȼ�� this �ͳ�ʼ�����
	//����Ҫע�⣬ð�ź�����ǳ�ʼ��
	STRING(const T* source) :length(0) { *this += source; }//��������ת�����൱�ڵ���
	STRING(const STRING<T>& source) :ARRAY<T>(source) { length = source.length; }
	STRING(int len) :ARRAY<T>(len + 1), length(len + 1) { zero(); }
	bool isEnd(const T word) { return word == endFlag; }
	int getLength(void)const { return length; }
	void UpdateLen() { length = getLength(*this); }//��ʹ��ָ��������д�ַ���ʱ�򲻻���� length ����Ҫ�Լ��ֶ�
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
	STRING<T, endFlag>& operator+=(const T* source)//��������ת��,�ʲ���Ҫ������ܲ���
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

	//�����ԭ����С����ô���һ���ֽڻ��ɿ��ַ�
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

			//һ��ʼû���ַ��������� size >= oldSize ʱ��û����
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