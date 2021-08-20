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
*  创建时间: 2021/08/19 0:34:08
*  最后修改: 
*
*  简    介: 人们总是希望在任何地方使用自己定义的类，但在广泛地使用向量类
* 之前，没有人知道哪些操作将被频繁用到。所以人们总是倾向于向自己的类中添加过多的内容，
* 尤其是过多地重载运算符
* 
* 信息屏蔽：有利于维护成员之间的关系“非变量”，但要慎重考虑
* 简洁和效率同样beautiful
* 
* 没有证明表明所谓的“优化”真正显著地提高了性能
* 95%的时间消耗在5%的代码上
* 过早的优化是一切罪恶的来源---优化那些非瓶颈的代码，使得代码复杂化，却没有得到相应的回报
*
***************************************************************************************
*/

#pragma once
/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Vector3.h - Declarations for 3D vector class
//
// Visit gamemath.com for the latest version of this file.
//
// For additional comments, see Chapter 6.
//
/////////////////////////////////////////////////////////////////////////////
#include <math.h>

/////////////////////////////////////////////////////////////////////////////
//
// class Vector3 - a simple 3D vector class
//
/////////////////////////////////////////////////////////////////////////////

class Vector3 
{
public:

	// Public representation:  Not many options here.

	float x, y, z;

	// Constructors

		// Default constructor leaves vector in
		// an indeterminate state

	Vector3() {}

	// Copy constructor

	Vector3(const Vector3& a) : x(a.x), y(a.y), z(a.z) {}

	// Construct given three values

	Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}

	// Standard object maintenance

		// Assignment.  We adhere to C convention and
		// return reference to the lvalue

	Vector3& operator =(const Vector3& a) 
	{
		x = a.x; y = a.y; z = a.z;
		return *this;
	}

	// Check for equality

	bool operator ==(const Vector3& a) const 
	{
		return x == a.x && y == a.y && z == a.z;
	}

	bool operator !=(const Vector3& a) const 
	{
		return x != a.x || y != a.y || z != a.z;
	}


	// Vector operations

		// Set the vector to zero

	void zero() { x = y = z = 0.0f; }

	// Unary minus returns the negative of the vector

	Vector3 operator -() const { return Vector3(-x, -y, -z); }

	// Binary + and - add and subtract vectors

	Vector3 operator +(const Vector3& a) const 
	{
		return Vector3(x + a.x, y + a.y, z + a.z);
	}

	Vector3 operator -(const Vector3& a) const 
	{
		return Vector3(x - a.x, y - a.y, z - a.z);
	}

	// Multiplication and division by scalar

	Vector3 operator *(float a) const 
	{
		return Vector3(x * a, y * a, z * a);
	}

	Vector3 operator /(float a) const 
	{
		float	oneOverA = 1.0f / a; // NOTE: no check for divide by zero here
		return Vector3(x * oneOverA, y * oneOverA, z * oneOverA);
	}

	// Combined assignment operators to conform to
	// C notation convention

	Vector3& operator +=(const Vector3& a) 
	{
		x += a.x; y += a.y; z += a.z;
		return *this;
	}

	Vector3& operator -=(const Vector3& a) 
	{
		x -= a.x; y -= a.y; z -= a.z;
		return *this;
	}

	Vector3& operator *=(float a) 
	{
		x *= a; y *= a; z *= a;
		return *this;
	}

	Vector3& operator /=(float a) 
	{
		float	oneOverA = 1.0f / a;
		x *= oneOverA; y *= oneOverA; z *= oneOverA;
		return *this;
	}

	// Normalize the vector

	void	normalize() 
	{
		float magSq = x * x + y * y + z * z;
		if (magSq > 0.0f) { // check for divide-by-zero
			float oneOverMag = 1.0f / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}

	// Vector dot product.  We overload the standard
	// multiplication symbol to do this

	float operator *(const Vector3& a) const 
	{
		return x * a.x + y * a.y + z * a.z;
	}
};

/////////////////////////////////////////////////////////////////////////////
//
// Nonmember functions
//
/////////////////////////////////////////////////////////////////////////////

// Compute the magnitude of a vector
//向量的模长
inline float vectorMag(const Vector3& a) 
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

// Compute the cross product of two vectors

inline Vector3 crossProduct(const Vector3& a, const Vector3& b) 
{
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

// Scalar on the left multiplication, for symmetry

inline Vector3 operator *(float k, const Vector3& v) 
{
	return Vector3(k * v.x, k * v.y, k * v.z);
}

// Compute the distance between two points

inline float distance(const Vector3& a, const Vector3& b) 
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrt(dx * dx + dy * dy + dz * dz);
}

// Compute the distance between two points, squared.  Often useful
// when comparing distances, since the square root is slow

inline float distanceSquared(const Vector3& a, const Vector3& b) 
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return dx * dx + dy * dy + dz * dz;
}

/////////////////////////////////////////////////////////////////////////////
//
// Global variables
//
/////////////////////////////////////////////////////////////////////////////

// We provide a global zero vector constant

extern const Vector3 kZeroVector;

/////////////////////////////////////////////////////////////////////////////

