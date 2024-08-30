#include "pch.h"
#include "core.h"
#include "vector.h"
#include "point.h"

namespace jmk
{
	float areaTriangle2d(const Point2d& a, const Point2d& b, const Point2d& c)
	{
		return 0.5f * ((b[X] - a[X]) * (c[Y] - a[Y]) - (c[X] - a[X]) * (b[Y] - a[Y]));
	}

	int orientation2d(const Point2d& a, const Point2d& b, const Point2d& c)
	{
		float area = areaTriangle2d(a, b, c);

		if (area > 0 && area < EPSILON)
			area = 0.0;

		if (area < 0 && area >  EPSILON)
			area = 0.0;

		Vector2f ab = b - a;
		Vector2f ac = c - a;

		if (area > 0.0)
			return LEFT;
		if (area < 0.0)
			return RIGHT;
		if ((ab[X] * ac[X] < 0.0) || (ab[Y] * ac[Y] < 0.0))
			return BEHIND;
		if (ab.magnitude() < ac.magnitude())
			return BEYOND;
		if (a == c)
			return ORIGIN;
		if (b == c)
			return DESTINATION;
		return BETWEEN;

		return 0;
	}

	/*double jmk::areaTriangle3d(const Point2f& a, const Point2f& b, const Point2f& c)
	{
		float x_, y_, z_;

		Vec3f AB = b - a;
		Vec3f AC = c - a;

		x_ = AB[Y] * AC[Z] - AB[Z] * AC[Y];
		y_ = AB[X] * AC[Z] - AB[Z] * AC[X];
		z_ = AB[X] * AC[Y] - AB[Y] * AC[X];

		float sum_of_powers = std::powf(x_, 2.0) + std::powf(y_, 2.0) + std::powf(z_, 2.0);
		float root = std::sqrtf(sum_of_powers);
		return root / 2;
	}*/

	/*bool left(const Point3d& a, const Point3d& b, const Point3d& c)
	{
		return orientation3d(a, b, c) == RELATIVE_POSITION::LEFT;
	}*/

	bool left(const Point2d& a, const Point2d& b, const Point2d& c)
	{
		return orientation2d(a, b, c) == RELATIVE_POSITION::LEFT;
	}
}