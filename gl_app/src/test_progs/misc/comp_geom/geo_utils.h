#pragma once

#include "core.h"
#include "vector.h"
#include "polygon.h"


namespace geom
{
	// Return the area of the triangle defined by given 3 points in XY 2D space
	float AreaTriangle2d(const Point2d& a, const Point2d& b, const Point2d& c);

	// Return the area of the triangle defined by given 3 points
	//float AreaTriangle3d(const Point2f& a, const Point2f& b, const Point2f& c);

	// Return integer indicating relative position of [Point c] related to segment [a b]
	geom::RelativePos Orientation2d(const Point2d& a, const Point2d& b, const Point2d& c);

	bool Collinear(const glm::vec3& a, const glm::vec3& b);

	bool Collinear(const Point3d& a, const Point3d& b, const Point3d& c);

	bool Coplanar(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

	bool Coplanar(const Point3d& a, const Point3d& b, const Point3d& c, const Point3d& d);
	
	// Predicate to determine whether the [Point c] is left to the segment [a b]
	bool Left(const Point2d& a, const Point2d& b, const Point2d& c);

	bool IsDiagonal(const Vertex2d* v1, const Vertex2d* v2, Polygon2d* poly = nullptr);
}