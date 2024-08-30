#include "pch.h"
#include "geo_utils.h"
#include "intersection.h"

namespace jmk {

	bool intersect(const jmk::Point2d& a, const jmk::Point2d& b, const jmk::Point2d& c, const jmk::Point2d& d) {

		// if one of the end points of a segment is in between other segment endpoints we consider it as intersection.
		if (jmk::orientation2d(a, b, c) == jmk::BETWEEN
			|| jmk::orientation2d(a, b, d) == jmk::BETWEEN
			|| jmk::orientation2d(c, d, a) == jmk::BETWEEN
			|| jmk::orientation2d(c, d, b) == jmk::BETWEEN)
		{
			return true;
		}

		return jmk::_xor(jmk::left(a, b, c), jmk::left(a, b, d)) && jmk::_xor(left(c, d, a), left(c, d, b));
	}

	bool intersect(jmk::Point2d& a, jmk::Point2d& b, jmk::Point2d& c, jmk::Point2d& d, jmk::Point2d& interseciton)
	{
		Vector2f AB = b - a;
		Vector2f CD = d - c;

		//Normal vector to CD
		Vector2f n(CD[Y], -CD[X]);

		//Denominator = n.(b-a)
		auto deno = dotProduct(n, AB);

		if (!isEqualD(deno, ZERO))
		{
			auto AC = c - a;
			auto nume = dotProduct(n, AC);
			auto t = nume / deno;

			auto x = a[X] + t * AB[X];
			auto y = a[Y] + t * AB[Y];

			interseciton.assign(X, x);
			interseciton.assign(Y, y);
			return true;
		}
		else
		{
			// Lines are parallel or colinear
			return false;
		}
	}

	bool intersect(Line2d& l1, Line2d& l2, Point2d& pi) {
		Vector2f l1_start = l1.getPoint();
		Vector2f l1_end = l1_start + l1.getDir();

		Vector2f l2_start = l2.getPoint();
		Vector2f l2_end = l2_start + l2.getDir();

		return intersect(l1_start, l1_end, l2_start, l2_end, pi);
		
	}
}