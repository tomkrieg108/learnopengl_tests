#include "pch.h"
#include "angle.h"

namespace geom
{
	float AngleLines2D(const Line2d& l1, const Line2d& l2)
	{
		return AngleLines(l1.direction, l2.direction);
	}

	float AngleLines3D(const Line3d& l1, const Line3d& l2)
	{
		return AngleLines(l1.direction, l2.direction);
	}

	float AngleLinePlane(const Line3d& line, const Planef& plane)
	{
		return 90 - AngleLines(line.direction, plane.normal);
	}

	float AnglePlanes(const Planef& p1, const Planef& p2)
	{
		return AngleLines(p1.normal, p2.normal);
	}
}