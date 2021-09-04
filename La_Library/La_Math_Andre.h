#pragma once
#include <stdio.h>
#include <string>
#include <sstream>
#include <math.h>
#include <corecrt_math.h>
#include <string>
#include "La_MathBase.h"
#include "La_Vector.h"

// defines for parametric line intersections
#define PARM_LINE_NO_INTERSECT          0
#define PARM_LINE_INTERSECT_IN_SEGMENT  1
#define PARM_LINE_INTERSECT_OUT_SEGMENT 2
#define PARM_LINE_INTERSECT_EVERYWHERE  3
// 3x3 matrix /////////////////////////////////////////////
class MATRIX3X3
{
public:
	union
	{
		float M[3][3];
		struct
		{
			float M00, M01, M02;
			float M10, M11, M12;
			float M20, M21, M22;
		};
	};

public:

	MATRIX3X3() {};
	MATRIX3X3(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22)
	{
		M00 = m00; M01 = m01; M02 = m02;
		M10 = m10; M11 = m11; M12 = m12;
		M20 = m20; M21 = m21; M22 = m22;

	}
	void set(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22)
	{
		M00 = m00; M01 = m01; M02 = m02;
		M10 = m10; M11 = m11; M12 = m12;
		M20 = m20; M21 = m21; M22 = m22;

	}
};

class MATRIX1X3
{
public:
	union
	{
		float M[3]; 
		struct { float M00, M01, M02; };
	};

public:
};

class MATRIX3X2
{
public:
	union
	{
		float M[3][2];
		struct
		{
			float M00, M01;
			float M10, M11;
			float M20, M21;
		};
	};

public:
	MATRIX3X2() {};
	MATRIX3X2(
		float m00, float m01,
		float m10, float m11,
		float m20, float m21)
	{
		// this function fills a 3x2 matrix with the sent data in row major form
		M[0][0] = m00; M[0][1] = m01;
		M[1][0] = m10; M[1][1] = m11;
		M[2][0] = m20; M[2][1] = m21;
	}
	void set(
		float m00, float m01,
		float m10, float m11,
		float m20, float m21)
	{
		// this function fills a 3x2 matrix with the sent data in row major form
		M[0][0] = m00; M[0][1] = m01;
		M[1][0] = m10; M[1][1] = m11;
		M[2][0] = m20; M[2][1] = m21;
	}
};
class MATRIX1X2
{
public:
	union
	{
		float M[2];
		struct { float M00, M01; };
	};
public:
};


class MATRIX2X2
{
public:
	union
	{
		float M[2][2];
		struct
		{
			float M00, M01;
			float M10, M11;
		};
	};

public:
	MATRIX2X2() {};
	MATRIX2X2(
		float m00, float m01,
		float m10, float m11)
	{
		M00 = m00; M01 = m01;
		M10 = m10; M11 = m11;
	}
	void set(
		float m00, float m01,
		float m10, float m11)
	{
		M00 = m00; M01 = m01;
		M10 = m10; M11 = m11;
	}
};

class MATRIX1X4
{
public:
	union
	{
		float M[4];
		struct { float M00, M01, M02, M03; };
	};
public:
};

class MATRIX4X4
{
public:
	union
	{
		float M[4][4];
		struct
		{
			float M00, M01, M02, M03;
			float M10, M11, M12, M13;
			float M20, M21, M22, M23;
			float M30, M31, M32, M33;
		};

	};
public:
	MATRIX4X4() {};
	MATRIX4X4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		M00 = m00; M01 = m01; M02 = m02; M03 = m03;
		M10 = m10; M11 = m11; M12 = m12; M13 = m13;
		M20 = m20; M21 = m21; M22 = m22; M23 = m23;
		M30 = m30; M31 = m31; M32 = m32; M33 = m33;
	}

	void set(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		M00 = m00; M01 = m01; M02 = m02; M03 = m03;
		M10 = m10; M11 = m11; M12 = m12; M13 = m13;
		M20 = m20; M21 = m21; M22 = m22; M23 = m23;
		M30 = m30; M31 = m31; M32 = m32; M33 = m33;
	}
	operator const char* ()
	{
		std::stringstream str;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				str << M[i][j] << " ";
			}
			str << std::endl;
		}
		return str.str().c_str();
	}
};

class MATRIX4X3
{
public:
	union
	{
		float M[4][3];
		struct
		{
			float M00, M01, M02;
			float M10, M11, M12;
			float M20, M21, M22;
			float M30, M31, M32;
		};
	};
};
class VECTOR2D
{
public:
	union
	{
		float M[2];
		struct { float x, y; };
	};
public:
	VECTOR2D() {};
	VECTOR2D(float outX, float outY) :x(outX), y(outY) {};
	VECTOR2D(const VECTOR2D& v2) :x(v2.x), y(v2.y) {};
	VECTOR2D(const VECTOR2D& init, const VECTOR2D& term) :x(term.x - init.x), y(y = term.y - init.y) {};
	void set(const VECTOR2D& init, const VECTOR2D& term) { x = term.x - init.x; y = term.y - init.y; }
	void set(float outX, float outY) { x = outX; y = outY; }
public:
	VECTOR2D operator=(const VECTOR2D& v2) { if (this != &v2) { x = v2.x; y = v2.y; }return *this; }
public:
	operator const char* ()
	{
		static std::string msg = "[";

		for (int index = 0; index < 2; index++)
		{
			msg += std::to_string(M[index]);
			msg += ", ";
		}
		msg += "]";
		return msg.c_str();
	}
};

class VECTOR4D;

class VECTOR3D
{
public:
	union
	{
		float M[3];
		struct { float x, y, z; };
	};

public:
	VECTOR3D() {};
	VECTOR3D(float outX, float outY, float outZ) :x(outX), y(outY), z(outZ) {};
	VECTOR3D(const VECTOR3D& v3) :x(v3.x), y(v3.y), z(v3.z) {};
	VECTOR3D(const VECTOR3D& init, const VECTOR3D& term) :x(term.x - init.x), y(term.y - init.y), z(term.z - init.z) {};
	void set(const VECTOR3D& init, const VECTOR3D& term) { x = term.x - init.x; y = term.y - init.y; z = term.z - init.z; }
	void set(float outX, float outY, float outZ) { x = outX; y = outY; z = outZ; }
	//void divByW(const VECTOR4D& v4) { x = (v4).x / (v4).w; y = (v4).y / (v4).w; z = (v4).z / (v4).w; }
public:
	VECTOR3D operator=(const VECTOR3D& v3) { if (this != &v3) { x = v3.x; y = v3.y; z = v3.z; }return *this; }
};


// 4D homogeneous vector 齐次向量
class VECTOR4D
{
public:
	union
	{
		float M[4];
		struct { float x, y, z, w; };
	};
public:
	VECTOR4D() {};
	VECTOR4D(float outX, float outY, float outZ) :x(outX), y(outY), z(outZ), w(1.0) {};
	VECTOR4D(float outX, float outY, float outZ, float outW) :x(outX), y(outY), z(outZ), w(outW) {};
	VECTOR4D(const VECTOR4D& v4) :x(v4.x), y(v4.y), z(v4.z), w(v4.w) {};
	VECTOR4D(const VECTOR4D& init, const VECTOR4D& term) :x(term.x - init.x), y(term.y - init.y), z(term.z - init.z), w(1) {};
	void set(const VECTOR4D& init, const VECTOR4D& term) { x = term.x - init.x; y = term.y - init.y; z = term.z - init.z; w = 1; }
	void set(float outX, float outY, float outZ) { x = outX; y = outY; z = outZ; w = 1.0; }
	void divByW() { x /= w; y /= w; z /= w; }
	VECTOR4D operator=(const VECTOR4D& v4)
	{
		if (this != &v4)
		{
			x = v4.x; y = v4.y; z = v4.z; w = v4.w;
		}
		return *this;
	}
	VECTOR4D operator=(const VECTOR2D& v2)
	{
		x = v2.x; y = v2.y; z = 0; w = 1;
		return *this;
	}
};

// 2D parametric line /////////////////////////////////////////
class PARMLINE2D
{
public:
	VECTOR2D p0; // start point of parametric line
	VECTOR2D p1; // end point of parametric line
	VECTOR2D v;  // direction vector of line segment
				 // |v|=|p0->p1|
public:
	PARMLINE2D() {};
	PARMLINE2D(const VECTOR2D& p_init, const VECTOR2D& p_term) :p0(p_init), p1(p_term), v(p_init, p_term) {};
	void set(const VECTOR2D& p_init, const VECTOR2D& p_term) { p0 = p_init; p1 = p_term; v.set(p_init, p_term); }
};

// 3D parametric line /////////////////////////////////////////
class PARMLINE3D
{
public:
	VECTOR3D p0; // start point of parametric line
	VECTOR3D p1; // end point of parametric line
	VECTOR3D v;  // direction vector of line segment
				 // |v|=|p0->p1|
public:
	PARMLINE3D() {};
	PARMLINE3D(const VECTOR3D& p_init, const VECTOR3D& p_term) :p0(p_init), p1(p_term), v(p_init, p_term) {};
	void set(const VECTOR3D& p_init, const VECTOR3D& p_term) { p0 = p_init; p1 = p_term; v.set(p_init, p_term); }
};

// 3D plane ///////////////////////////////////////////////////
class PLANE3D
{
public:
	VECTOR3D p0; // point on the plane
	VECTOR3D n; // normal to the plane (not necessarily a unit vector)
public:
	PLANE3D() {};
	void set(VECTOR3D& p0, VECTOR3D& normal, bool normalize = false);
};


class POLAR2D// 2D polar coordinates ///////////////////////////////////////
{
public:
	float r;
	float theta;
};

// 3D cylindrical coordinates ////////////////////////////////
class CYLINDRICAL3D
{
public:
	float r;
	float theta;
	float z;
};

// 3D spherical coordinates //////////////////////////////////
class SPHERICAL3D
{
public:
	float p;      // rho, the distance to the point from the origin
	float theta;  // the angle from the z-axis and the line segment o->p
	float phi;    // the angle from the projection if o->p onto the x-y 
				  // plane and the x-axis
};

// 4d quaternion ////////////////////////////////////////////
// note the union gives us a number of ways to work with
// the components of the quaternion
class QUAT
{
public:
	union
	{
		float M[4];
		struct
		{
			float    q0;  // the real part
			VECTOR3D qv;  // the imaginary part xi+yj+zk
		};
		struct { float w, x, y, z; };
	};
public:
	QUAT() {};
	QUAT(float outW, float outX, float outY, float outZ) :w(outW), x(outX), y(outY), z(outZ) {};
	QUAT(const QUAT& qt) :w(qt.w), x(qt.x), y(qt.y), z(qt.w) {};
	QUAT(const VECTOR3D& v3) :w(0), x(v3.x), y(v3.y), z(v3.z) {};
public:
	QUAT operator*=(float k) { x *= k; y *= k; z *= k; w *= k; return *this; }
public:
	operator const char* ()
	{
#define ToString   std::to_string
		static std::string msg = ToString(w) + " + " + ToString(x) + "i + " + ToString(y) + "j + " + ToString(z) + "k";
#undef ToString
		return msg.c_str();
	}
};
// identity matrices
extern const MATRIX4X4 IMAT_4X4;
extern const MATRIX4X3 IMAT_4X3;
extern const MATRIX3X3 IMAT_3X3;
extern const MATRIX2X2 IMAT_2X2;

inline void Zero(MATRIX2X2& m) { memset((void*)(&m), 0, sizeof(MATRIX2X2)); }
inline void Zero(MATRIX3X3& m) { memset((void*)(&m), 0, sizeof(MATRIX3X3)); }
inline void Zero(MATRIX4X4& m) { memset((void*)(&m), 0, sizeof(MATRIX4X4)); }
inline void Zero(MATRIX4X3& m) { memset((void*)(&m), 0, sizeof(MATRIX4X3)); }


inline void Identity(MATRIX2X2& m) { memcpy((void*)(&m), (void*)&IMAT_2X2, sizeof(MATRIX2X2)); }
inline void Identity(MATRIX3X3& m) { memcpy((void*)(&m), (void*)&IMAT_3X3, sizeof(MATRIX3X3)); }
inline void Identity(MATRIX4X4& m) { memcpy((void*)(&m), (void*)&IMAT_4X4, sizeof(MATRIX4X4)); }
inline void Identity(MATRIX4X3& m) { memcpy((void*)(&m), (void*)&IMAT_4X3, sizeof(MATRIX4X3)); }


inline void Copy(MATRIX2X2& dest_mat, const MATRIX2X2& src_mat) { memcpy((void*)(&dest_mat), (void*)(&src_mat), sizeof(MATRIX2X2)); }
inline void Copy(MATRIX3X3& dest_mat, const MATRIX3X3& src_mat) { memcpy((void*)(&dest_mat), (void*)(&src_mat), sizeof(MATRIX3X3)); }
inline void Copy(MATRIX4X4& dest_mat, const MATRIX4X4& src_mat) { memcpy((void*)(&dest_mat), (void*)(&src_mat), sizeof(MATRIX4X4)); }
inline void Copy(MATRIX4X3& dest_mat, const MATRIX4X3& src_mat) { memcpy((void*)(&dest_mat), (void*)(&src_mat), sizeof(MATRIX4X3)); }


inline void Transpose(MATRIX3X3& m)
{
	MATRIX3X3 mt;
	mt.M00 = m.M00; mt.M01 = m.M10; mt.M02 = m.M20;
	mt.M10 = m.M01; mt.M11 = m.M11; mt.M12 = m.M21;
	mt.M20 = m.M02; mt.M21 = m.M12; mt.M22 = m.M22;
	memcpy((void*)&m, (void*)&mt, sizeof(MATRIX3X3));
}
inline void Transpose(MATRIX4X4& m)
{
	MATRIX4X4 mt;
	mt.M00 = m.M00; mt.M01 = m.M10; mt.M02 = m.M20; mt.M03 = m.M30;
	mt.M10 = m.M01; mt.M11 = m.M11; mt.M12 = m.M21; mt.M13 = m.M31;
	mt.M20 = m.M02; mt.M21 = m.M12; mt.M22 = m.M22; mt.M23 = m.M32;
	mt.M30 = m.M03; mt.M31 = m.M13; mt.M32 = m.M22; mt.M33 = m.M33;
	memcpy((void*)&m, (void*)&mt, sizeof(MATRIX4X4));
}
inline void Transpose(const MATRIX3X3& m, MATRIX3X3& mt)
{
	mt.M00 = m.M00; mt.M01 = m.M10; mt.M02 = m.M20;
	mt.M10 = m.M01; mt.M11 = m.M11; mt.M12 = m.M21;
	mt.M20 = m.M02; mt.M21 = m.M12; mt.M22 = m.M22;
}
inline void Transpose(const MATRIX4X4& m, MATRIX4X4& mt)
{
	mt.M00 = m.M00; mt.M01 = m.M10; mt.M02 = m.M20; mt.M03 = m.M30;
	mt.M10 = m.M01; mt.M11 = m.M11; mt.M12 = m.M21; mt.M13 = m.M31;
	mt.M20 = m.M02; mt.M21 = m.M12; mt.M22 = m.M22; mt.M23 = m.M32;
	mt.M30 = m.M03; mt.M31 = m.M13; mt.M32 = m.M22; mt.M33 = m.M33;
}


inline void SwapCol(MATRIX2X2& m, int c, const MATRIX1X2& v) { m.M[0][c] = v.M[0]; m.M[1][c] = v.M[1]; }
inline void SwapCol(MATRIX3X3& m, int c, const MATRIX1X3& v) { m.M[0][c] = v.M[0]; m.M[1][c] = v.M[1]; m.M[2][c] = v.M[2]; }
inline void SwapCol(MATRIX4X4& m, int c, const MATRIX1X4& v) { m.M[0][c] = v.M[0]; m.M[1][c] = v.M[1]; m.M[2][c] = v.M[2]; m.M[3][c] = v.M[3]; }
inline void SwapCol(MATRIX4X3& m, int c, const MATRIX1X4& v) { m.M[0][c] = v.M[0]; m.M[1][c] = v.M[1]; m.M[2][c] = v.M[2]; m.M[3][c] = v.M[3]; }

// note the 4D vector sets w=1
inline void Zero(VECTOR2D& v) { (v).x = (v).y = 0.0; }
inline void Zero(VECTOR3D& v) { (v).x = (v).y = (v).z = 0.0; }
inline void Zero(VECTOR4D& v) { (v).x = (v).y = (v).z = 0.0; (v).w = 1.0; }
inline void Zero(QUAT& q) { (q).x = (q).y = (q).z = (q).w = 0.0; }



inline void		Add(const QUAT& q1, const QUAT& q2, QUAT& qsum) { qsum.x = q1.x + q2.x; qsum.y = q1.y + q2.y; qsum.z = q1.z + q2.z; qsum.w = q1.w + q2.w; }
inline void		Add(const VECTOR2D& va, const VECTOR2D& vb, VECTOR2D& vsum) { vsum.x = va.x + vb.x; vsum.y = va.y + vb.y; }
inline VECTOR2D Add(const VECTOR2D& va, const VECTOR2D& vb) { VECTOR2D vsum; vsum.x = va.x + vb.x; vsum.y = va.y + vb.y; return(vsum); }
inline void		Add(const VECTOR3D& va, const VECTOR3D& vb, VECTOR3D& vsum) { vsum.x = va.x + vb.x; vsum.y = va.y + vb.y; vsum.z = va.z + vb.z; }
inline VECTOR3D Add(const VECTOR3D& va, const VECTOR3D& vb) { VECTOR3D vsum; vsum.x = va.x + vb.x; vsum.y = va.y + vb.y; vsum.z = va.z + vb.z; return(vsum); }
inline void		Add(const VECTOR4D& va, const VECTOR4D& vb, VECTOR4D& vsum) { vsum.x = va.x + vb.x; vsum.y = va.y + vb.y; vsum.z = va.z + vb.z; vsum.w = 1; }
inline VECTOR4D Add(const VECTOR4D& va, const VECTOR4D& vb) { VECTOR4D vsum; vsum.x = va.x + vb.x; vsum.y = va.y + vb.y; vsum.z = va.z + vb.z; vsum.w = 1;	return(vsum); }
inline void		Add(const MATRIX2X2& ma, const MATRIX2X2& mb, MATRIX2X2& msum) { msum.M00 = ma.M00 + mb.M00; msum.M01 = ma.M01 + mb.M01; msum.M10 = ma.M10 + mb.M10; msum.M11 = ma.M11 + mb.M11; }
void		Add(const MATRIX3X3& ma, const MATRIX3X3& mb, MATRIX3X3& msum);
void		Add(const MATRIX4X4& ma, const MATRIX4X4& mb, MATRIX4X4& msum);

inline QUAT operator+(const QUAT& q1, const QUAT& q2) { QUAT qsum; Add(q1, q2, qsum); return qsum; }

inline void		Sub(const QUAT& q1, const QUAT& q2, QUAT& qdiff) { qdiff.x = q1.x - q2.x; qdiff.y = q1.y - q2.y; qdiff.z = q1.z - q2.z; qdiff.w = q1.w - q2.w; }
inline void		Sub(const VECTOR2D& va, const VECTOR2D& vb, VECTOR2D& vdiff) { vdiff.x = va.x - vb.x; vdiff.y = va.y - vb.y; }
inline VECTOR2D Sub(const VECTOR2D& va, const VECTOR2D& vb) { VECTOR2D vdiff; vdiff.x = va.x - vb.x; vdiff.y = va.y - vb.y; return(vdiff); }
inline void		Sub(const VECTOR3D& va, const VECTOR3D& vb, VECTOR3D& vdiff) { vdiff.x = va.x - vb.x; vdiff.y = va.y - vb.y; vdiff.z = va.z - vb.z; }
inline VECTOR3D Sub(const VECTOR3D& va, const VECTOR3D& vb) { VECTOR3D vdiff; vdiff.x = va.x - vb.x; vdiff.y = va.y - vb.y; vdiff.z = va.z - vb.z; return(vdiff); }
inline void		Sub(const VECTOR4D& va, const VECTOR4D& vb, VECTOR4D& vdiff) { vdiff.x = va.x - vb.x; vdiff.y = va.y - vb.y; vdiff.z = va.z - vb.z; vdiff.w = 1; }
inline VECTOR4D Sub(const VECTOR4D& va, const VECTOR4D& vb) { VECTOR4D vdiff; vdiff.x = va.x - vb.x; vdiff.y = va.y - vb.y; vdiff.z = va.z - vb.z; vdiff.w = 1; return(vdiff); }

//共轭
inline void Conjugate(const QUAT& q, QUAT& qconj) { qconj.x = -q.x; qconj.y = -q.y; qconj.z = -q.z; qconj.w = q.w; }

void	  Mul(const MATRIX3X3& ma, const MATRIX3X3& mb, MATRIX3X3& result);
void	  Mul(const MATRIX1X3& ma, const MATRIX3X3& mb, MATRIX1X3& result);
void	  Mul(const MATRIX1X2& ma, const MATRIX3X2& mb, MATRIX1X2& result);
void	  Mul(const QUAT& q1, const QUAT& q2, QUAT& qprod);
inline void	  Mul(const QUAT& q1, const  QUAT& q2, const QUAT& q3, QUAT& qprod) { QUAT qtmp; Mul(q1, q2, qtmp); Mul(qtmp, q3, qprod); }
inline void   Mul(const MATRIX2X2& ma, const MATRIX2X2& mb, MATRIX2X2& mprod) { mprod.M00 = ma.M00 * mb.M00 + ma.M01 * mb.M10; mprod.M01 = ma.M00 * mb.M01 + ma.M01 * mb.M11; mprod.M10 = ma.M10 * mb.M00 + ma.M11 * mb.M10; mprod.M11 = ma.M10 * mb.M01 + ma.M11 * mb.M11; }
void   Mul(const VECTOR3D& va, const MATRIX3X3& mb, VECTOR3D& vprod);
void   Mul(const MATRIX4X4& ma, const MATRIX4X4& mb, MATRIX4X4& mprod);
void   Mul(const MATRIX1X4& ma, const MATRIX4X4& mb, MATRIX1X4& mprod);
void   Mul(const VECTOR3D& va, const MATRIX4X4& mb, VECTOR3D& vprod);
void   Mul(const VECTOR3D& va, const MATRIX4X3& mb, VECTOR3D& vprod);
void   Mul(const VECTOR4D& va, const MATRIX4X4& mb, VECTOR4D& vprod);
void   MulFast(const VECTOR4D& va, const MATRIX4X4& mb, VECTOR4D& vprod);
inline FIXP16 Mul(const FIXP16 fp1, const FIXP16 fp2)
{
	// this function computes the product fp_prod = fp1*fp2
	// using 64 bit math, so as not to loose precission
	FIXP16 fp_prod; // return the product
	_asm {
		mov eax, fp1      // move into eax fp2
		imul fp2          // multiply fp1*fp2
		shrd eax, edx, 16 // result is in 32:32 format 
						  // residing at edx:eax
						  // shift it into eax alone 16:16
		// result is sitting in eax
	}
}

inline void Scale(const float k, const QUAT& q, QUAT& qs) { qs.x = k * q.x; qs.y = k * q.y; qs.z = k * q.z; qs.w = k * q.w; }
inline void Scale(const float k, QUAT& q) { q.x *= k; q.y *= k; q.z *= k; q.w *= k; }
inline void Scale(const float k, const VECTOR2D& va, VECTOR2D& vscaled) { vscaled.x = k * va.x; vscaled.y = k * va.y; }
inline void Scale(const float k, VECTOR2D& va) { va.x *= k; va.y *= k; }
inline void Scale(const float k, VECTOR3D& va) { va.x *= k; va.y *= k; va.z *= k; }
inline void Scale(const float k, const VECTOR3D& va, VECTOR3D& vscaled) { vscaled.x = k * va.x; vscaled.y = k * va.y; vscaled.z = k * va.z; }
inline void Scale(const float k, VECTOR4D& va) { va.x *= k; va.y *= k; va.z *= k; va.w = 1; }
inline void Scale(const float k, const VECTOR4D& va, VECTOR4D& vscaled) { vscaled.x = k * va.x; vscaled.y = k * va.y; vscaled.z = k * va.z; vscaled.w = 1; }

//单位四元数的逆
inline void InverseUnit(const QUAT& q, QUAT& qi) { qi.w = q.w; qi.x = -q.x; qi.y = -q.y; qi.z = -q.z; }
inline void InverseUnit(QUAT& q) { q.x = -q.x; q.y = -q.y; q.z = -q.z; }
//四元数的逆
inline void Inverse(const QUAT& q, QUAT& qi) { float norm2_inv = 1.0 / (q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z); qi.w = q.w * norm2_inv; qi.x = -q.x * norm2_inv; qi.y = -q.y * norm2_inv; qi.z = -q.z * norm2_inv; }
inline void Inverse(QUAT& q) { float norm2_inv = 1.0 / (q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z); q.w = q.w * norm2_inv; q.x = -q.x * norm2_inv; q.y = -q.y * norm2_inv; q.z = -q.z * norm2_inv; }
bool Inverse(const MATRIX2X2& m, MATRIX2X2& mi);
bool Inverse(const MATRIX3X3& m, MATRIX3X3& mi);
bool Inverse(const MATRIX4X4& m, MATRIX4X4& mi);

inline FIXP16 Div(FIXP16 fp1, FIXP16 fp2)
{
	// this function computes the quotient fp1/fp2 using
	// 64 bit math, so as not to loose precision

	_asm
	{
		mov eax, fp1      // move dividend into eax
		cdq               // sign extend it to edx:eax
		shld edx, eax, 16 // now shift 16:16 into position in edx
		sal eax, 16       // and shift eax into position since the
						  // shld didn't move it -- DUMB! uPC
						  idiv fp2          // do the divide
											// result is sitting in eax     
	}
}

inline float Dot(const VECTOR3D& va, const VECTOR3D& vb) { return((va.x * vb.x) + (va.y * vb.y) + (va.z * vb.z)); }
inline float Dot(const VECTOR2D& va, const VECTOR2D& vb) { return((va.x * vb.x) + (va.y * vb.y)); }
inline float Dot(const VECTOR4D& va, const VECTOR4D& vb) { return((va.x * vb.x) + (va.y * vb.y) + (va.z * vb.z)); }


inline void		Cross(const VECTOR3D& va, const VECTOR3D& vb, VECTOR3D& vn) { vn.x = ((va.y * vb.z) - (va.z * vb.y)); vn.y = -((va.x * vb.z) - (va.z * vb.x)); vn.z = ((va.x * vb.y) - (va.y * vb.x)); }
inline VECTOR3D Cross(const VECTOR3D& va, const VECTOR3D& vb) { VECTOR3D vn; vn.x = ((va.y * vb.z) - (va.z * vb.y)); vn.y = -((va.x * vb.z) - (va.z * vb.x)); vn.z = ((va.x * vb.y) - (va.y * vb.x)); return(vn); }
inline void		Cross(const VECTOR4D& va, const VECTOR4D& vb, VECTOR4D& vn) { vn.x = ((va.y * vb.z) - (va.z * vb.y)); vn.y = -((va.x * vb.z) - (va.z * vb.x)); vn.z = ((va.x * vb.y) - (va.y * vb.x)); vn.w = 1; }
inline VECTOR4D Cross(const VECTOR4D& va, const VECTOR4D& vb) { VECTOR4D vn; vn.x = ((va.y * vb.z) - (va.z * vb.y)); vn.y = -((va.x * vb.z) - (va.z * vb.x)); vn.z = ((va.x * vb.y) - (va.y * vb.x)); vn.w = 1; return(vn); }


inline float Length(const QUAT& q) { return(sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z)); }
inline float Length(const VECTOR4D& va) { return(sqrtf(va.x * va.x + va.y * va.y + va.z * va.z)); }
inline float Length(const VECTOR2D& va) { return(sqrtf(va.x * va.x + va.y * va.y)); }
inline float Length(const VECTOR3D& va) { return((float)sqrtf(va.x * va.x + va.y * va.y + va.z * va.z)); }
inline float Length2(const QUAT& q) { return(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z); }
inline float LengthFast(const VECTOR2D& va) { return((float)DistanceFast(va.x, va.y)); }
inline float LengthFast(const VECTOR3D& va) { return(DistanceFast(va.x, va.y, va.z)); }
inline float LengthFast(VECTOR4D& va) { return(DistanceFast(va.x, va.y, va.z)); }

inline float Cos(const VECTOR2D& va, const VECTOR2D& vb) { return(Dot(va, vb) / (Length(va) * Length(vb))); }
inline float Cos(const VECTOR3D& va, const VECTOR3D& vb) { return(Dot(va, vb) / (Length(va) * Length(vb))); }
inline float Cos(const VECTOR4D& va, const VECTOR4D& vb) { return(Dot(va, vb) / (Length(va) * Length(vb))); }

inline float Det(const MATRIX2X2& m) { return(m.M00 * m.M11 - m.M01 * m.M10); }
inline float Det(const MATRIX3X3& m) { return(m.M00 * (m.M11 * m.M22 - m.M21 * m.M12) - m.M01 * (m.M10 * m.M22 - m.M20 * m.M12) + m.M02 * (m.M10 * m.M21 - m.M20 * m.M11)); }


inline void Normalize(const QUAT& q, QUAT& qn) { float qlength_inv = 1.0 / (sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z)); qn.w = q.w * qlength_inv; qn.x = q.x * qlength_inv; qn.y = q.y * qlength_inv; qn.z = q.z * qlength_inv; }
inline void Normalize(QUAT& q) { float qlength_inv = 1.0 / (sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z)); q.w *= qlength_inv; q.x *= qlength_inv; q.y *= qlength_inv; q.z *= qlength_inv; }
inline void Normalize(VECTOR2D& va) { float length = sqrtf(va.x * va.x + va.y * va.y); if (length < EPSILON_E5)return; float length_inv = 1 / length; va.x = va.x * length_inv; va.y = va.y * length_inv; }
inline void Normalize(const VECTOR2D& va, VECTOR2D& vn) { Zero(vn); float length = (float)sqrtf(va.x * va.x + va.y * va.y); if (length < EPSILON_E5)return; float length_inv = 1 / length; vn.x = va.x * length_inv; vn.y = va.y * length_inv; }
inline void Normalize(VECTOR3D& va) { float length = sqrtf(va.x * va.x + va.y * va.y + va.z * va.z); if (length < EPSILON_E5)return; float length_inv = 1 / length; va.x *= length_inv; va.y *= length_inv; va.z *= length_inv; }
inline void Normalize(const VECTOR3D& va, VECTOR3D& vn) { Zero(vn); float length = Length(va); if (length < EPSILON_E5)	return; float length_inv = 1.0 / length; vn.x = va.x * length_inv; vn.y = va.y * length_inv; vn.z = va.z * length_inv; }
inline void Normalize(VECTOR4D& va) { float length = sqrtf(va.x * va.x + va.y * va.y + va.z * va.z); if (length < EPSILON_E5)return; float length_inv = 1.0 / length; va.x *= length_inv; va.y *= length_inv; va.z *= length_inv; va.w = 1; }
inline void Normalize(const VECTOR4D& va, VECTOR4D& vn) { Zero(vn); float length = sqrt(va.x * va.x + va.y * va.y + va.z * va.z); if (length < EPSILON_E5)return; float length_inv = 1.0 / length; vn.x = va.x * length_inv; vn.y = va.y * length_inv; vn.z = va.z * length_inv; vn.w = 1; }



void ThetaToQUAT(const VECTOR3D& v, const float theta, QUAT& q);

void ThetaToQuat(const VECTOR4D& v, const float theta, QUAT& q);

void QuatToTheta(const QUAT& q, VECTOR3D& v, float& theta);

void EulerZYXToQuat(QUAT& q, float theta_z, float theta_y, float theta_x);

float SinFast(float theta);
float CosFast(float theta);


inline void PolarToVector(const POLAR2D& polar, VECTOR2D& rect) { rect.x = polar.r * cosf(polar.theta); rect.y = polar.r * sinf(polar.theta); }
inline void PolarToVector(const POLAR2D& polar, float& x, float& y) { x = polar.r * cosf(polar.theta); y = polar.r * sinf(polar.theta); }
inline void VectorToPolar(const VECTOR2D& rect, POLAR2D& polar) { polar.r = sqrtf((rect.x * rect.x) + (rect.y * rect.y)); polar.theta = atanf(rect.y / rect.x); }
inline void VectorToPolar(const VECTOR2D& rect, float& r, float& theta) { r = sqrtf((rect.x * rect.x) + (rect.y * rect.y)); theta = atanf(rect.y / rect.x); }


inline void CylindricalToVector(const CYLINDRICAL3D& cyl, VECTOR3D& rect) { rect.x = cyl.r * cosf(cyl.theta); rect.y = cyl.r * sinf(cyl.theta); rect.z = cyl.z; }
inline void CylindricalToVector(const CYLINDRICAL3D& cyl, float& x, float& y, float& z) { x = cyl.r * cosf(cyl.theta); y = cyl.r * sinf(cyl.theta);	z = cyl.z; }
inline void VectorToCylindrical(const VECTOR3D& rect, CYLINDRICAL3D& cyl) { cyl.r = sqrtf((rect.x * rect.x) + (rect.y * rect.y)); cyl.theta = atanf(rect.y / rect.x); cyl.z = rect.z; }
inline void VectorToCylindrical(const VECTOR3D& rect, float& r, float& theta, float& z) { r = sqrtf((rect.x * rect.x) + (rect.y * rect.y)); theta = atanf(rect.y / rect.x); z = rect.z; }


inline void SphericalToVector(const SPHERICAL3D& sph, VECTOR3D& rect) { float r; r = sph.p * sinf(sph.phi); rect.z = sph.p * cosf(sph.phi); rect.x = r * cosf(sph.theta); rect.y = r * sinf(sph.theta); }
inline void SphericalToVector(const SPHERICAL3D& sph, float& x, float& y, float& z) { float r; r = sph.p * sinf(sph.phi); z = sph.p * cosf(sph.phi); x = r * cosf(sph.theta); y = r * sinf(sph.theta); }
inline void VectorToSpherical(const VECTOR3D& rect, SPHERICAL3D& sph) { sph.p = sqrtf((rect.x * rect.x) + (rect.y * rect.y) + (rect.z * rect.z)); sph.theta = atanf(rect.y / rect.x); float r = sph.p * sinf(sph.phi); sph.phi = asinf(r / sph.p); }
inline void VectorToSpherical(const VECTOR3D& rect, float& p, float& theta, float& phi) { p = sqrtf((rect.x * rect.x) + (rect.y * rect.y) + (rect.z * rect.z)); theta = atanf(rect.y / rect.x); float r = sqrtf((rect.x * rect.x) + (rect.y * rect.y)); phi = asinf(r / (p)); }

bool LinearEquation(const MATRIX2X2& A, MATRIX1X2& X, const MATRIX1X2& B);
bool LinearEquation(const MATRIX3X3& A, MATRIX1X3& X, const MATRIX1X3& B);

//计算直线p在 t 处的坐标
inline void Compute(const PARMLINE2D& p, const float t, VECTOR2D& pt) { pt.x = p.p0.x + p.v.x * t; pt.y = p.p0.y + p.v.y * t; }
inline void Compute(const PARMLINE3D& p, const float t, VECTOR3D& pt) { pt.x = p.p0.x + p.v.x * t; pt.y = p.p0.y + p.v.y * t; pt.z = p.p0.z + p.v.z * t; }
//计算点是在向量的正面还是负面
inline float Compute(const VECTOR3D& pt, const PLANE3D& plane) { float hs = plane.n.x * (pt.x - plane.p0.x) + plane.n.y * (pt.y - plane.p0.y) + plane.n.z * (pt.z - plane.p0.z); return(hs); }


//判断两线是否相交
int Intersect(const PARMLINE2D& p1, const PARMLINE2D& p2, float& t1, float& t2);
int Intersect(const PARMLINE2D& p1, const PARMLINE2D& p2, VECTOR2D& pt);
//线面是否相交
int Intersect(const PARMLINE3D& pline, const PLANE3D& plane, float& t, VECTOR3D& pt);

inline void PLANE3D::set(VECTOR3D& p0, VECTOR3D& normal, bool normalize)
{
	p0 = p0;
	if (!normalize)n = normal;
	else Normalize(normal, n);
}