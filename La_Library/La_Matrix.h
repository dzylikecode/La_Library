#pragma once
#include "La_Vector.h"

template <typename T>
class MAXTRIX
{
private:
	vVECTOR<hVECTOR<T>> matrix;
	int row, col;
public:
	MAXTRIX(int r, int c);
	hVECTOR<T>& operator[](int i) { return matrix[i]; }
	MAXTRIX<T> operator*(const MAXTRIX<T>& mat)const;
	MAXTRIX<T> operator&(const MAXTRIX<T>& mat)const;
	void Zero();
};

template <typename T>
MAXTRIX<T>::MAXTRIX(int r, int c) :row(r), col(c)
{
	matrix.resize(row);
	for (int i = 0; i < row; i++)
	{
		matrix[i].resize(col);
	}
	Zero();
}

template <typename T>
void MAXTRIX<T>::Zero()
{
	for (int i = 0; i < row; i++)
		matrix[i].Zero();
}

template <typename T>
MAXTRIX<T> MAXTRIX<T>::operator*(const MAXTRIX<T>& mat)const
{
	assert(col == mat.row);
	MAXTRIX<T> temp(row, mat.col);

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < mat.col; j++)
		{
			for (int k = 0; k < col; k++)
			{
				temp[i][j] += matrix[i][k] * mat[k][j];
			}
		}
	}

	return temp;
}


template <typename T>
MAXTRIX<T> MAXTRIX<T>::operator&(const MAXTRIX<T>& mat)const
{
	assert(row == mat.row && col == mat.col);
	MAXTRIX<T> temp(row, col);

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < mat.col; j++)
		{
			temp[i][j] = matrix[i][j] * mat[i][j];
		}
	}

	return temp;
}

