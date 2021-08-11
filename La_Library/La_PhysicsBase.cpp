#include "La_PhysicsBase.h"
#include "La_Math.h"
namespace LADZY
{
	const REAL gravityConst = 6.67e-11;
	bool Collision_Test(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
	{
		int width1 = (w1 >> 1) - (w1 >> 3);
		int height1 = (h1 >> 1) - (h1 >> 3);

		int width2 = (w2 >> 1) - (w2 >> 3);
		int height2 = (h2 >> 1) - (h2 >> 3);

		int cx1 = x1 + width1;
		int cy1 = y1 + height1;

		int cx2 = x2 + width2;
		int cy2 = y2 + height2;

		int dx = abs(cx2 - cx1);
		int dy = abs(cy2 - cy1);

		if (dx < (width1 + width2) && dy < (height1 + height2))
		{
			return true;
		}

		return false;
	}

	void CollisionVelocity(REAL m1, REAL v1, REAL m2, REAL v2, REAL& v1Res, REAL& v2Res)
	{
		REAL delta = m1 - m2; REAL sigma = m1 + m2;
		v1Res = (2 * m2 * v2 + delta * v1) / sigma;
		v2Res = (2 * m1 * v1 + delta * v2) / sigma;
	}
}