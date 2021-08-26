#include "La_Math_Andre.h"

// identity matrices
const MATRIX4X4 IMAT_4X4 = { 1,0,0,0,
							0,1,0,0,
							0,0,1,0,
							0,0,0,1 };
const MATRIX4X3 IMAT_4X3 = { 1,0,0,
							0,1,0,
							0,0,1,
							0,0,0, };
const MATRIX3X3 IMAT_3X3 = { 1,0,0,
							0,1,0,
							0,0,1 };
const MATRIX2X2 IMAT_2X2 = { 1,0,
							0,1 };


void Add(const MATRIX3X3& ma, const MATRIX3X3& mb, MATRIX3X3& msum)
{
	for (int row = 0; row < 3; row++)
		for (int col = 0; col < 3; col++)
			msum.M[row][col] = ma.M[row][col] + mb.M[row][col];
}
void Add(const MATRIX4X4& ma, const MATRIX4X4& mb, MATRIX4X4& msum)
{
	for (int row = 0; row < 4; row++)
		for (int col = 0; col < 4; col++)
			msum.M[row][col] = ma.M[row][col] + mb.M[row][col];
}


void Mul(const MATRIX3X3& ma, const MATRIX3X3& mb, MATRIX3X3& result)
{
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			float sum = 0; 
			for (int index = 0; index < 3; index++)
				sum += (ma.M[row][index] * mb.M[index][col]);
			result.M[row][col] = sum;
		}
	}
}
void Mul(const MATRIX1X3& ma, const MATRIX3X3& mb, MATRIX1X3& result)
{
	for (int col = 0; col < 3; col++)
	{
		float sum = 0; 
		for (int index = 0; index < 3; index++)
			sum += (ma.M[index] * mb.M[index][col]);
		result.M[col] = sum;
	}
}
void Mul(const MATRIX1X2& ma, const MATRIX3X2& mb, MATRIX1X2& result)
{
	for (int col = 0; col < 2; col++)
	{
		float sum = 0; 
		int index;
		for (index = 0; index < 2; index++)
			sum += (ma.M[index] * mb.M[index][col]);
		sum += mb.M[index][col];
		result.M[col] = sum;
	}
}
void Mul(const QUAT& q1, const QUAT& q2, QUAT& qprod)
{
	// this function multiplies two quaternions

	// this is the brute force method
	//qprod->w = q1->w*q2->w - q1->x*q2->x - q1->y*q2->y - q1->z*q2->z;
	//qprod->x = q1->w*q2->x + q1->x*q2->w + q1->y*q2->z - q1->z*q2->y;
	//qprod->y = q1->w*q2->y - q1->x*q2->z + q1->y*q2->w - q1->z*q2->x;
	//qprod->z = q1->w*q2->z + q1->x*q2->y - q1->y*q2->x + q1->z*q2->w;

	// this method was arrived at basically by trying to factor the above
	// expression to reduce the # of multiplies

	float prd_0 = (q1.z - q1.y) * (q2.y - q2.z);
	float prd_1 = (q1.w + q1.x) * (q2.w + q2.x);
	float prd_2 = (q1.w - q1.x) * (q2.y + q2.z);
	float prd_3 = (q1.y + q1.z) * (q2.w - q2.x);
	float prd_4 = (q1.z - q1.x) * (q2.x - q2.y);
	float prd_5 = (q1.z + q1.x) * (q2.x + q2.y);
	float prd_6 = (q1.w + q1.y) * (q2.w - q2.z);
	float prd_7 = (q1.w - q1.y) * (q2.w + q2.z);

	float prd_8 = prd_5 + prd_6 + prd_7;
	float prd_9 = 0.5 * (prd_4 + prd_8);

	// and finally build up the result with the temporary products

	qprod.w = prd_0 + prd_9 - prd_5;
	qprod.x = prd_1 + prd_9 - prd_8;
	qprod.y = prd_2 + prd_9 - prd_7;
	qprod.z = prd_3 + prd_9 - prd_6;
}


void Mul(const VECTOR3D& va, const MATRIX3X3& mb, VECTOR3D& vprod)
{
	for (int col = 0; col < 3; col++)
	{
		float sum = 0;
		for (int row = 0; row < 3; row++)
			sum += (va.M[row] * mb.M[row][col]);
		vprod.M[col] = sum;
	}
}
void Mul(const MATRIX4X4& ma, const MATRIX4X4& mb, MATRIX4X4& mprod)
{
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			float sum = 0; 
			for (int index = 0; index < 4; index++)
				sum += (ma.M[row][index] * mb.M[index][col]);
			mprod.M[row][col] = sum;
		} 
	}
} 
void Mul(const MATRIX1X4& ma, const MATRIX4X4& mb, MATRIX1X4& mprod)
{
	for (int col = 0; col < 4; col++)
	{
		float sum = 0;
		for (int row = 0; row < 4; row++)
			sum += (ma.M[row] * mb.M[row][col]);
		mprod.M[col] = sum;

	}
}
void Mul(const VECTOR3D& va, const MATRIX4X4& mb, VECTOR3D& vprod)
{
	for (int col = 0; col < 3; col++)
	{
		float sum = 0; 
		int row;
		for (row = 0; row < 3; row++)
			sum += (va.M[row] * mb.M[row][col]);
		sum += mb.M[row][col];
		vprod.M[col] = sum;
	}
}
void Mul(const VECTOR3D& va, const MATRIX4X3& mb, VECTOR3D& vprod)
{
	for (int col = 0; col < 3; col++)
	{
		float sum = 0; 
		int row;
		for (row = 0; row < 3; row++)
			sum += (va.M[row] * mb.M[row][col]);
		sum += mb.M[row][col];
		vprod.M[col] = sum;
	}
}
void Mul(const VECTOR4D& va, const MATRIX4X4& mb, VECTOR4D& vprod)
{
	for (int col = 0; col < 4; col++)
	{
		float sum = 0; 
		for (int row = 0; row < 4; row++)
			sum += (va.M[row] * mb.M[row][col]);
		vprod.M[col] = sum;
	}
}
void MulFast(const VECTOR4D& va, const MATRIX4X4& mb, VECTOR4D& vprod)
{
	for (int col = 0; col < 3; col++)
	{
		float sum = 0;
		for (int row = 0; row < 4; row++)
			sum += (va.M[row] * mb.M[row][col]);
		vprod.M[col] = sum;
	}
	vprod.M[3] = va.M[3];
}


bool Inverse(const MATRIX2X2& m, MATRIX2X2& mi)
{
	// compute determinate
	float det = (m.M00 * m.M11 - m.M01 * m.M10);

	// if determinate is 0 then inverse doesn't exist
	if (fabs(det) < EPSILON_E5)
		return(false);

	float det_inv = 1.0 / det;

	mi.M00 = m.M11 * det_inv;
	mi.M01 = -m.M01 * det_inv;
	mi.M10 = -m.M10 * det_inv;
	mi.M11 = m.M00 * det_inv;

	return(true);
}
bool Inverse(const MATRIX3X3& m, MATRIX3X3& mi)
{
	// first compute the determinate to see if there is 
	// an inverse
	float det = m.M00 * (m.M11 * m.M22 - m.M21 * m.M12) -
		m.M01 * (m.M10 * m.M22 - m.M20 * m.M12) +
		m.M02 * (m.M10 * m.M21 - m.M20 * m.M11);

	if (fabs(det) < EPSILON_E5)
		return(false);

	// compute inverse to save divides
	float det_inv = 1.0 / det;

	// compute inverse using m-1 = adjoint(m)/det(m)
	mi.M00 = det_inv * (m.M11 * m.M22 - m.M21 * m.M12);
	mi.M10 = -det_inv * (m.M10 * m.M22 - m.M20 * m.M12);
	mi.M20 = det_inv * (m.M10 * m.M21 - m.M20 * m.M11);

	mi.M01 = -det_inv * (m.M01 * m.M22 - m.M21 * m.M02);
	mi.M11 = det_inv * (m.M00 * m.M22 - m.M20 * m.M02);
	mi.M21 = -det_inv * (m.M00 * m.M21 - m.M20 * m.M01);

	mi.M02 = det_inv * (m.M01 * m.M12 - m.M11 * m.M02);
	mi.M12 = -det_inv * (m.M00 * m.M12 - m.M10 * m.M02);
	mi.M22 = det_inv * (m.M00 * m.M11 - m.M10 * m.M01);

	return true;
}
bool Inverse(const MATRIX4X4& m, MATRIX4X4& mi)
{
	// computes the inverse of a 4x4, assumes that the last
	// column is [0 0 0 1]t
	float det = (m.M00 * (m.M11 * m.M22 - m.M12 * m.M21) -
		m.M01 * (m.M10 * m.M22 - m.M12 * m.M20) +
		m.M02 * (m.M10 * m.M21 - m.M11 * m.M20));

	// test determinate to see if it's 0
	if (fabs(det) < EPSILON_E5)
		return(false);

	float det_inv = 1.0f / det;

	mi.M00 = det_inv * (m.M11 * m.M22 - m.M12 * m.M21);
	mi.M01 = -det_inv * (m.M01 * m.M22 - m.M02 * m.M21);
	mi.M02 = det_inv * (m.M01 * m.M12 - m.M02 * m.M11);
	mi.M03 = 0.0f; // always 0

	mi.M10 = -det_inv * (m.M10 * m.M22 - m.M12 * m.M20);
	mi.M11 = det_inv * (m.M00 * m.M22 - m.M02 * m.M20);
	mi.M12 = -det_inv * (m.M00 * m.M12 - m.M02 * m.M10);
	mi.M13 = 0.0f; // always 0

	mi.M20 = det_inv * (m.M10 * m.M21 - m.M11 * m.M20);
	mi.M21 = -det_inv * (m.M00 * m.M21 - m.M01 * m.M20);
	mi.M22 = det_inv * (m.M00 * m.M11 - m.M01 * m.M10);
	mi.M23 = 0.0f; // always 0

	mi.M30 = -(m.M30 * mi.M00 + m.M31 * mi.M10 + m.M32 * mi.M20);
	mi.M31 = -(m.M30 * mi.M01 + m.M31 * mi.M11 + m.M32 * mi.M21);
	mi.M32 = -(m.M30 * mi.M02 + m.M31 * mi.M12 + m.M32 * mi.M22);
	mi.M33 = 1.0f; // always 1
	return(true);
}


float SinFast(float theta)
{
	// convert angle to 0-359  // make angle positive
	theta = fmodf(theta, 360); if (theta < 0) theta += 360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int = (int)theta; float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(sin_look[theta_int] + theta_frac * (sin_look[theta_int + 1] - sin_look[theta_int]));
}
float CosFast(float theta)
{
	// convert angle to 0-359  // make angle positive
	theta = fmodf(theta, 360); if (theta < 0) theta += 360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int = (int)theta; float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(cos_look[theta_int] + theta_frac * (cos_look[theta_int + 1] - cos_look[theta_int]));
}

void ThetaToQUAT(const VECTOR3D& v, const float theta, QUAT& q)
{
	// initializes a quaternion based on a 3d direction vector and angle
	// note the direction vector must be a unit vector
	// and the angle is in rads

	float theta_div_2 = (0.5) * theta; // compute theta/2

	// compute the quaterion, note this is from chapter 4
	// pre-compute to save time
	float sinf_theta = sinf(theta_div_2);

	q.x = sinf_theta * v.x;
	q.y = sinf_theta * v.y;
	q.z = sinf_theta * v.z;
	q.w = cosf(theta_div_2);
}

void ThetaToQuat(const VECTOR4D& v, const float theta, QUAT& q)
{
	// initializes a quaternion based on a 4d direction vector and angle
	// note the direction vector must be a unit vector
	// and the angle is in rads

	float theta_div_2 = (0.5) * theta; // compute theta/2

	// compute the quaterion, note this is from chapter 4

	// pre-compute to save time
	float sinf_theta = sinf(theta_div_2);

	q.x = sinf_theta * v.x;
	q.y = sinf_theta * v.y;
	q.z = sinf_theta * v.z;
	q.w = cosf(theta_div_2);
}

void QuatToTheta(const QUAT& q, VECTOR3D& v, float& theta)
{
	// this function converts a unit quaternion into a unit direction
	// vector and rotation angle about that vector

	// extract theta
	theta = acosf(q.w);

	// pre-compute to save time
	float sinf_theta_inv = 1.0 / sinf(theta);

	// now the vector
	v.x = q.x * sinf_theta_inv;
	v.y = q.y * sinf_theta_inv;
	v.z = q.z * sinf_theta_inv;

	// multiply by 2
	theta *= 2;
}

void EulerZYXToQuat(QUAT& q, float theta_z, float theta_y, float theta_x)
{
	// this function intializes a quaternion based on the zyx
	// multiplication order of the angles that are parallel to the
	// zyx axis respectively, note there are 11 other possibilities
	// this is just one, later we may make a general version of the
	// the function

	// precompute values
	float cos_z_2 = 0.5 * cosf(theta_z);
	float cos_y_2 = 0.5 * cosf(theta_y);
	float cos_x_2 = 0.5 * cosf(theta_x);

	float sin_z_2 = 0.5 * sinf(theta_z);
	float sin_y_2 = 0.5 * sinf(theta_y);
	float sin_x_2 = 0.5 * sinf(theta_x);

	// and now compute quaternion
	q.w = cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2;
	q.x = cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2;
	q.y = cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2;
	q.z = sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2;
}


bool LinearEquation2X2(const MATRIX2X2& A, MATRIX1X2& X, const MATRIX1X2& B)
{
	// solves the system AX=B and computes X=A(-1)*B
	float det_A = Det(A);

	// test if det(A) is zero, if so then there is no solution
	if (fabs(det_A) < EPSILON_E5)
		return(false);

	//克拉默法则
	MATRIX2X2 work_mat;

	Copy(work_mat, A);
	SwapCol(work_mat, 0, B);
	float det_ABx = Det(work_mat);
	X.M00 = det_ABx / det_A;

	Copy(work_mat, A);
	SwapCol(work_mat, 1, B);
	float det_ABy = Det(work_mat);
	X.M01 = det_ABy / det_A;

	return(true);
}


bool LinearEquation(const MATRIX3X3& A, MATRIX1X3& X, const MATRIX1X3& B)
{
	// solves the system AX=B and computes X=A(-1)*B
	// by using cramers rule and determinates

	// step 1: compute determinate of A
	float det_A = Det(A);

	// test if det(a) is zero, if so then there is no solution
	if (fabs(det_A) < EPSILON_E5)
		return(0);

	// step 2: create x,y,z numerator matrices by taking A and
	// replacing each column of it with B(transpose) and solve
	MATRIX3X3 work_mat;

	Copy(work_mat, A);
	SwapCol(work_mat, 0, B);
	float det_ABx = Det(work_mat);
	X.M00 = det_ABx / det_A;

	Copy(work_mat, A);
	SwapCol(work_mat, 1, B);
	float det_ABy = Det(work_mat);
	X.M01 = det_ABy / det_A;

	Copy(work_mat, A);
	SwapCol(work_mat, 2, B);
	float det_ABz = Det(work_mat);
	X.M02 = det_ABz / det_A;

	return(true);
}

