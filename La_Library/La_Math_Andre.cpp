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





int Intersect(const PARMLINE2D& p1, const PARMLINE2D& p2, float& t1, float& t2)
{
	// this function computes the intersection of the two parametric 
	// line segments the function returns true if the segments 
	// intersect and sets the values of t1 and t2 to the t values that 
	// the intersection occurs on the lines p1 and p2 respectively, 
	// however, the function may send back t value not in the range [0,1]
	// this means that the segments don't intersect, but the lines that 
	// they represent do, thus a return of 0 means no intersection, a 
	// 1 means intersection on the segments and a 2 means the lines 
	// intersect, but not necessarily the segments and 3 means that 
	// the lines are the same, thus they intersect everywhere

	// basically we have a system of 2d linear equations, we need
	// to solve for t1, t2 when x,y are both equal (if that's possible)

	// step 1: test for parallel lines, if the direction vectors are 
	// scalar multiples then the lines are parallel and can't possible
	// intersect unless the lines overlap

	float det_p1p2 = (p1.v.x * p2.v.y - p1.v.y * p2.v.x);
	if (fabs(det_p1p2) <= EPSILON_E5)
	{
		// at this point, the lines either don't intersect at all
		// or they are the same lines, in which case they may intersect
		// at one or many points along the segments, at this point we 
		// will assume that the lines don't intersect at all, but later
		// we may need to rewrite this function and take into 
		// consideration the overlap and singular point exceptions
		return(PARM_LINE_NO_INTERSECT);
	}

	// step 2: now compute the t1, t2 values for intersection
	// we have two lines of the form
	// p    = p0    +  v*t, specifically
	// p1   = p10   + v1*t1

	// p1.x = p10.x + v1.x*t1
	// p1.y = p10.y + v1.y*t1

	// p2 = p20 + v2*t2
	// p2   = p20   + v2*t2

	// p2.x = p20.x + v2.x*t2
	// p2.y = p20.y + v2.y*t2
	// solve the system when x1 = x2 and y1 = y2
	// explained in chapter 4
	t1 = (p2.v.x * (p1.p0.y - p2.p0.y) - p2.v.y * (p1.p0.x - p2.p0.x)) / det_p1p2;

	t2 = (p1.v.x * (p1.p0.y - p2.p0.y) - p1.v.y * (p1.p0.x - p2.p0.x)) / det_p1p2;

	// test if the lines intersect on the segments
	if ((t1 >= 0) && (t1 <= 1) && (t2 >= 0) && (t2 <= 1))return(PARM_LINE_INTERSECT_IN_SEGMENT);
	else return(PARM_LINE_INTERSECT_OUT_SEGMENT);
}
int Intersect(const PARMLINE2D& p1, const PARMLINE2D& p2, VECTOR2D& pt)
{
	// this function computes the interesection of the two 
	// parametric line segments the function returns true if 
	// the segments interesect and sets the values of pt to the 
	// intersection point, however, the function may send back a 
	// value not in the range [0,1] on the segments this means 
	// that the segments don't intersect, but the lines that 
	// they represent do, thus a retun of 0 means no intersection, 
	// a 1 means intersection on the segments and a 2 means 
	// the lines intersect, but not necessarily the segments

	// basically we have a system of 2d linear equations, we need
	// to solve for t1, t2 when x,y are both equal (if that's possible)

	// step 1: test for parallel lines, if the direction vectors are 
	// scalar multiples then the lines are parallel and can't possible
	// intersect

	float t1, t2, det_p1p2 = (p1.v.x * p2.v.y - p1.v.y * p2.v.x);

	if (fabs(det_p1p2) <= EPSILON_E5)
	{
		// at this point, the lines either don't intersect at all
		// or they are the same lines, in which case they may intersect
		// at one or many points along the segments, at this point we 
		// will assume that the lines don't intersect at all, but later
		// we may need to rewrite this function and take into 
		// consideration the overlap and singular point exceptions
		return(PARM_LINE_NO_INTERSECT);

	}

	// step 2: now compute the t1, t2 values for intersection
	// we have two lines of the form
	// p    = p0    +  v*t, specifically
	// p1   = p10   + v1*t1

	// p1.x = p10.x + v1.x*t1
	// p1.y = p10.y + v1.y*t1

	// p2 = p20 + v2*t2
	// p2   = p20   + v2*t2

	// p2.x = p20.x + v2.x*t2
	// p2.y = p20.y + v2.y*t2
	// solve the system when x1 = x2 and y1 = y2
	// explained in chapter 4
	t1 = (p2.v.x * (p1.p0.y - p2.p0.y) - p2.v.y * (p1.p0.x - p2.p0.x))
		/ det_p1p2;

	t2 = (p1.v.x * (p1.p0.y - p2.p0.y) - p1.v.y * (p1.p0.x - p2.p0.x))
		/ det_p1p2;

	// now compute point of intersection
	pt.x = p1.p0.x + p1.v.x * t1;
	pt.y = p1.p0.y + p1.v.y * t1;

	// test if the lines intersect on the segments
	if ((t1 >= 0) && (t1 <= 1) && (t2 >= 0) && (t2 <= 1))return(PARM_LINE_INTERSECT_IN_SEGMENT);
	else return(PARM_LINE_INTERSECT_OUT_SEGMENT);

}
int Intersect(const PARMLINE3D& pline, const PLANE3D& plane, float& t, VECTOR3D& pt)
{
	// this function determines where the sent parametric line 
	// intersects the plane the function will project the line 
	// infinitely in both directions, to compute the intersection, 
	// but the line segment defined by p intersected the plane iff t e [0,1]
	// also the function returns 0 for no intersection, 1 for 
	// intersection of the segment and the plane and 2 for intersection 
	// of the line along the segment and the plane 3, the line lies 
	// in the plane

	// first test of the line and the plane are parallel, if so 
	// then they can't intersect unless the line lies in the plane!

	float plane_dot_line = Dot(pline.v, plane.n);

	if (fabs(plane_dot_line) <= EPSILON_E5)
	{
		// the line and plane are co-planer, does the line lie 
		// in the plane?
		if (fabs(Compute(pline.p0, plane)) <= EPSILON_E5)
			return(PARM_LINE_INTERSECT_EVERYWHERE);
		else
			return(PARM_LINE_NO_INTERSECT);
	}

	// from chapter 4 we know that we can solve for the t where 
	// intersection occurs by
	// a*(x0+vx*t) + b*(y0+vy*t) + c*(z0+vz*t) + d =0
	// t = -(a*x0 + b*y0 + c*z0 + d)/(a*vx + b*vy + c*vz)
	// x0,y0,z0, vx,vy,vz, define the line
	// d = (-a*xp0 - b*yp0 - c*zp0), xp0, yp0, zp0, define the point on the plane 

	t = -(plane.n.x * pline.p0.x +
		plane.n.y * pline.p0.y +
		plane.n.z * pline.p0.z -
		plane.n.x * plane.p0.x -
		plane.n.y * plane.p0.y -
		plane.n.z * plane.p0.z) / (plane_dot_line);

	// now plug t into the parametric line and solve for x,y,z
	pt.x = pline.p0.x + pline.v.x * (t);
	pt.y = pline.p0.y + pline.v.y * (t);
	pt.z = pline.p0.z + pline.v.z * (t);

	// test interval of t
	if (t >= 0.0 && t <= 1.0) return(PARM_LINE_INTERSECT_IN_SEGMENT);
	else return(PARM_LINE_INTERSECT_OUT_SEGMENT);
}
