#include "La_MathBase.h"

// storage for our lookup tables
REAL cos_look[360];
REAL sin_look[360];


class MATHbase
{
public:
	MATHbase()
	{
		REAL theta;
		// generate the tables 0 - 360 inclusive
		for (int ang = 0; ang < 360; ang++)
		{
			theta = DEG_TO_RAD(ang);

			cos_look[ang] = cos(theta);
			sin_look[ang] = sin(theta);
		}
	}
};