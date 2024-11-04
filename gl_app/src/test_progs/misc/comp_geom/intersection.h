#pragma once

#include "vector.h"
#include "line.h"
#include "plane.h"

namespace geom {

	//a -> b is line1
	//c -> d is line2
	bool Lines2dIntersect(const Point2d& a, const Point2d& b,
		const Point2d& c, const Point2d& d);


	//// Return true if 2d segments represeted by points are intersecting. Store the intersecting point in final argument
	//// 1 - start point of first segment
	//// 2 - end point of first segmenet
	//// 3 - start point of second segment
	//// 4 - end point of second segment
	//// 5 - intersection point
	bool GetLines2dIntersectPoint(const Point2d& p1, const Point2d& p2, 
		const Point2d& p3, const Point2d& p4, Point2d& out_intersect_point);


	bool GetLines2dIntersectPoint(const Line2d& line1, const Line2d& line2, Point2d& out_intersect_point);

	//output in param 3
	bool Intersection(const Line3d&, const Planef&, Point3d&);

	bool Intersection(const Planef&, const Planef&, Line3d&);
		
}