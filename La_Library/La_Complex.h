#pragma once

template <typename T>
class COMPLEX 
{
public:
	T real;
	T image;

	COMPLEX(T r, T i) :real(r), image(i) {};
	COMPLEX(T r) :real(r), image(0) {};

	COMPLEX<T>  operator + (const COMPLEX<T>& c2) const{ return COMPLEX<T>(real + c2.real, image + c2.image); }
	COMPLEX<T>  operator - (const COMPLEX<T>& c2) const{ return COMPLEX<T>(real - c2.real, image - c2.image); }

	friend ostream& operator << (ostream& out, const COMPLEX<T>& c)
	{
		out << "(" << c.real << ", " << c.image << ")";
		return out;
	}



};


typedef COMPLEX<float>  fCOMPLEX;
typedef COMPLEX<double> dCOMPLEX;
