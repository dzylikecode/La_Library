#include "La_MathBase.h"

// storage for our lookup tables
REAL cos_look[361];
REAL sin_look[361];


class MATHbase
{
public:
	MATHbase()
	{
		REAL theta;
		// generate the tables 0 - 360 inclusive
		for (int ang = 0; ang < 361; ang++)
		{
			theta = DEG_TO_RAD(ang);

			cos_look[ang] = cos(theta);
			sin_look[ang] = sin(theta);
		}
	}
}mathBase;

// U = p0 + t*S1
// V = p1 + s*S2
// U = V
//s = (-s1y * (p0x - p2x) + s1x * (p0y - p2y)) / (-s2x * s1y + s1x * s2y);
//t = (s2x * (p0y - p2y) - s2y * (p0x - p2x)) / (-s2x * s1y + s1x * s2y);
