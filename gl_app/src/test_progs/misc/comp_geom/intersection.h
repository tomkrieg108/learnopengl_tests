#pragma once
// Include functions to calculate intersection between different geometric promitives
#include "point.h"
#include "line.h"
#include "plane.h"
//#include "segment.h"
//#include "polygon.h"
//#include "polyhedron.h"

namespace jmk {

	//// Return true if 2d segments represeted by points are intersecting. Store the intersecting point in final argument
	//// 1 - start point of first segment
	//// 2 - end point of first segmenet
	//// 3 - start point of second segment
	//// 4 - end point of second segment
	//// 5 - intersection point
	bool intersect(jmk::Point2d&, jmk::Point2d&, jmk::Point2d&, jmk::Point2d&, jmk::Point2d&);

	//// Return true if 2d segments represeted by points are intersecting. Store the intersecting point in final argument
	//// 1 - start point of first segment
	//// 2 - end point of first segmenet
	//// 3 - start point of second segment
	//// 4 - end point of second segment
	//// 5 - intersection point
	bool intersect(jmk::Point2d&, jmk::Point2d&, jmk::Point2d&, jmk::Point2d&, jmk::Point2d&);

	// Return true if two 2d lines are intersecting and store the intersection point in final argument
	// 1 - First line
	// 2 - Second line
	bool intersect(jmk::Line2d&, jmk::Line2d&, jmk::Point2d&);
}