#include "pch.h"
#include "geo_utils.h"
#include "intersection.h"

namespace geom {

	bool Lines2dIntersect(const Point2d& a, const Point2d& b,
		const Point2d& c, const Point2d& d)
	{
		// if one of the end points of a segment is in between other segment endpoints we consider it as intersection.
		if (Orientation2d(a, b, c) == RelativePos::BETWEEN
			|| Orientation2d(a, b, d) == RelativePos::BETWEEN
			|| Orientation2d(c, d, a) == RelativePos::BETWEEN
			|| Orientation2d(c, d, b) == RelativePos::BETWEEN)
		{
			return true;
		}

		return Xor (Left(a, b, c), Left(a, b, d)) &&
			Xor(Left(c, d, a), Left(c, d, b));
	}

	//The same method as described by Penny - math course lec 47, also in notes
	bool GetLines2dIntersectPoint(const Point2d& a, const Point2d& b,
		const Point2d& c, const Point2d& d, Point2d& out_intersect_point)
	{
		glm::vec2 AB = b - a;
		glm::vec2 CD = d - c;

		//Normal vector to CD
		glm::vec2 normal{ CD.y, -CD.x };

		//Denominator = n.(b-a)
		auto denominator = glm::dot(normal, AB);

		if (!Equal(denominator, 0))
		{
			auto AC = c - a;
			auto numerator = glm::dot(normal, AC);
			auto t = numerator / denominator;
			out_intersect_point.x = a.x + t * AB.x;
			out_intersect_point.y = a.y + t * AB.y;
			return true;
		}
		else
		{
			// Lines are parallel or colinear
			return false;
		}
	}

	//
	bool GetLines2dIntersectPoint(const Line2d& line1, const Line2d& line2, Point2d& out_intersect_point)
	{
		Point2d l1_start = line1.start_point;
		Point2d l1_end = line1.end_point;
		//Point2d l1_end = l1_start + line1.direction_vector;
		
		Point2d l2_start = line2.start_point;
		Point2d l2_end = line2.end_point;
		//Point2d l2_end = l2_start + line2.direction_vector;

		return GetLines2dIntersectPoint(l1_start, l1_end, l2_start, l2_end, out_intersect_point);
	}

	bool Intersection(const Line3d& line, const Planef& plane, Point3d& point)
	{
		//Derived ch20
		auto n = plane.normal;
		auto D = plane.d;
		auto d = line.direction;
		auto p = line.start_point;

		auto nd = glm::dot(n, d);
		if (geom::Equal(nd, 0))
			return false; //parallel or colliner

		auto t = (-1*dot(n,p) + D) / nd;
		point.x = p.x + t * d.x;
		point.y = p.y + t * d.y;
		point.z = p.z + t * d.z;

		return true;
	}

	bool Intersection(const Planef& p1, const Planef& p2, Line3d& l)
	{
		//Derived cha21 *check*

		auto n1 = p1.normal;
		auto n2 = p2.normal;
		auto d1 = p1.d;
		auto d2 = p2.d;

		auto direction = glm::cross(n1, n2);

		if (geom::Equal(glm::length(direction), 0))
			return false;

		auto dot_n1_n2 = glm::dot(n1, n2);
		auto denom = dot_n1_n2 - 1;
		auto a = (d2 - dot_n1_n2 - d1) / denom;
		auto b = (d1 - dot_n1_n2 - d2) / denom;
		auto point = n1 * a + n2 * b;	//point on the intersecting line

		Line3d line{ n1, n1 + direction };
		l = line;
		return true;
	}

}