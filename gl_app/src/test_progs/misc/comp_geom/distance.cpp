#include "pch.h"
#include "distance.h"

namespace geom
{
	float Distance(const Line3d& line, const Point3d& C)
	{
		//derived formula - see vid 18 t1 - v.(Y-a) v is the unit vector on the line, a is a known point on the line, want to get distnce from point Y to line

		auto AC = C - line.start_point;
		auto t = glm::dot(line.direction, AC);

		auto xt = line.start_point + line.direction * t;
		auto dist_vect = xt - C;
		return glm::length(dist_vect);

		
		//rays: t< 0 closest point is the start point on the ray
		//segment t<0 => start point, t>0=> end point
	}

	//Derived in vid 19
	float distance(const Planef& p, Point3d& Q)
	{
		auto result = glm::dot(p.normal, Q) - p.d;
		return result;

		//Note: return value is positive ig Q is on the positive side of the place otherwise negative.  So can use as an orientation check
	}
}