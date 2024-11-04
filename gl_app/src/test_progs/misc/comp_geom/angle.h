#pragma once

#include "line.h"
#include "plane.h"

namespace geom
{
	template <uint32_t Dim, typename T>
	float AngleLines(const glm::vec<Dim, T>& v1, const  glm::vec < Dim, T>& v2)
	{
		const auto dot = glm::dot(v1, v2); //these already normalised
		const auto theta = glm::acos(fabs(dot));
		return glm::degrees(theta);
	}

	//Returns angle in degrees - lines in 2D can either be parallel or intersect
	float AngleLines2D(const Line2d& l1, const Line2d& l2);

	//3D lines in 2D can either be parallel or intersect or skewed
	float AngleLines3D(const Line3d& l1, const Line3d& l2);

	float AngleLinePlane(const Line3d& line, const Planef& plane);

	float AnglePlanes(const Planef& p1, const Planef& p2);
}