#pragma once
#include "La_Vector.h"

template <typename T>
class MAXTRIX
{
private:
	VECTORV<VECTORH<T>> matrix;
	int col;
public:
	VECTORH<T>& operator[](int i) { return matrix[i]; }
	MAXTRIX<T>& operator*=(const MAXTRIX<T>& mat);
};