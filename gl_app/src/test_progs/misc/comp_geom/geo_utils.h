#pragma once

#include "point.h"

namespace jmk
{
	// Return the area of the triangle defined by given 3 points in XY 2D space
	float areaTriangle2d(const Point2d& a, const Point2d& b, const Point2d& c);

	// Return the area of the triangle defined by given 3 points
	//float AreaTriangle3d(const Point2f& a, const Point2f& b, const Point2f& c);

	// Return integer indicating relative position of [Point c] related to segment [a b]
	int orientation2d(const Point2d& a, const Point2d& b, const Point2d& c);


	// Predicate to determine whether the [Point c] is left to the segment [a b]
	//bool left(const Point3d& a, const Point3d& b, const Point3d& c);

	// Predicate to determine whether the [Point c] is left to the segment [a b]
	bool left(const Point2d& a, const Point2d& b, const Point2d& c);
}