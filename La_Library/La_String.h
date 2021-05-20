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
	STRING(const T* source) { *this += source; }
	STRING(const STRING<T>& source) :ARRAY<T>(source) { lenth = source.lenth; }
	STRING(int len) :ARRAY<T>(len + 1), lenth(len + 1) { Zero(); }
	int StrLen(void)const { return lenth; }
	STRING<T>& operator+=(const T* source);//可以类型转化,故不需要用类接受参数
	friend STRING<T> operator+(const T* source, const STRING<T>& sourceString);
	STRING<T>	operator+ (const T* source) { STRING<T> temp = *this; return temp += source; }
	void Zero(void);
	void Clear(void) { Resize(0); lenth = 0; }
};

template <class T>
STRING<T>& STRING<T>::operator+=(const T* source)
{
	int newSize = lenth + lstrlen(source) + 1;
	assert(Resize(newSize));
	for (int i = lenth; i < newSize; i++)
		(*this)[i] = *source++;

	lenth = newSize - 1;
	return *this;
}
template <class T>
STRING<T> operator+(const T* source, const STRING<T>& sourceString)
{
	return sourceString + source;
}

template <class T>
void STRING<T>::Zero(void)
{
	for (int i = 0; i < GetSize(); i++)
		(*this)[i] = '\0';
}

#endif