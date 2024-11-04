#pragma once

#include "line.h"
#include "plane.h"

namespace geom
{
	float Distance(const Line3d& line, const Point3d& C);

	float distance(const Planef& p, Point3d& Q);

}