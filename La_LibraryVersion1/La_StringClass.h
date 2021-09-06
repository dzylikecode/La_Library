#ifndef _LA_STRING_CLASS_

#define _LA_STRING_CLASS_

#include "La_StringBase.h"
#include <string>
#include <sstream>
#include "La_Array.h"

template <class T, int endFlag = 0>
class STRING_BASIC :public ARRAY<T>
{
#define temSTRING STRING_BASIC<T, endFlag>
#define temARRAY  ARRAY<T>
private:
	bool isEnd(const T word) { return word == endFlag; }
	int getLength(const T* source)
	{
		if (!source) return 0;
		int i = 0;
		while (!isEnd(*source++)) i++;
		return i;
	}
public:
	STRING_BASIC(void) {};
	//������Ĭ�� ARRAY ��ʼ��������ͬʱҪע�� length ҲҪ��ʼ����Ȼ�� this �ͳ�ʼ�����
	//����Ҫע�⣬ð�ź�����ǳ�ʼ��
	STRING_BASIC(const T* source) { *this += source; }//��������ת�����൱�ڵ���
	STRING_BASIC(const temSTRING& source) :temARRAY(source) {};
public:
	/*temSTRING operator=(const T* source){*(temARRAY)this = so }*/
public:
	temSTRING& operator+=(const T* source)//��������ת��,�ʲ���Ҫ������ܲ���
	{
		int length = getLength(*this);
		int newSize = length + getLength(source) + 1;
		assert(this->temARRAY::resize(newSize, true));
		for (int i = length; i < newSize; i++) (*this)[i] = *source++;
		return *this;
	}
	inline friend temSTRING operator+(const T* source, const temSTRING& sourceString) { temSTRING temp = source;  return temp += sourceString; }
	inline temSTRING  operator+ (const T* source) { temSTRING temp = *this; return temp += source; }
	//�����ԭ����С����ô���һ���ֽڻ��ɿ��ַ�


	bool operator==(const T* target)
	{
		const T* temp = *this;
		while (!isEnd(*target) || !isEnd(*temp))
			if (*temp++ != *target++) return false;
		return true;
	}
	inline friend bool operator==(const T* target, const temSTRING& source) { return source == target; }
	inline bool operator!=(const T* target) { return !(*this == target); }
	inline friend bool operator!=(const T* target, const temSTRING& source) { return source != target; }

#undef temARRAY
#undef temSTRING
};

using ASTRING = STRING_BASIC<char>;
using WSTRING = STRING_BASIC<wchar_t>;
using TSTRING = STRING_BASIC<TCHAR>;


WSTRING& AToW(__in LPCCH pszInBuf, WSTRING& result);
ASTRING& WToA(__in LPCWCH pszInBuf, ASTRING& result);

//��������ʧ������Ȼ����Ҫ�� static
inline WSTRING& AToW(__in LPCCH pszInBuf) { static WSTRING temp; return AToW(pszInBuf, temp); }
inline ASTRING& WToA(__in LPCWCH pszInBuf) { static ASTRING temp; return WToA(pszInBuf, temp); }
inline WSTRING& WToW(__in LPCWCH pszInBuf, WSTRING& result) { return result = pszInBuf; }
inline WSTRING& WToW(__in LPCWCH pszInBuf) { static WSTRING temp; return temp = pszInBuf; }
inline ASTRING& AToA(__in LPCCH pszInBuf, ASTRING& result) { return result = pszInBuf; }
inline ASTRING& AToA(__in LPCCH pszInBuf) { static ASTRING temp; return temp = pszInBuf; }


#ifdef UNICODE
using tstring = std::wstring;
using tstringstream = std::wstringstream;

#define AToT    AToW
#define TToA	WToA
#define WToT	WToW
#define TToW	WToW


#else
using tstring = std::string;
using tstringstream = std::stringstream;

#define AToT    AToA
#define TToA	AToA
#define WToT	WToA
#define TToW	AToW
#endif






#endif



