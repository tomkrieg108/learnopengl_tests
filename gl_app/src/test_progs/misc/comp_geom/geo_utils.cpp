#include "pch.h"
#include "core.h"
#include "vector.h"
#include "intersection.h"
#include "geo_utils.h"

namespace geom
{
	float AreaTriangle2d(const Point2d& a, const Point2d& b, const Point2d& c)
	{
		return 0.5f * ((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));
	}

	geom::RelativePos Orientation2d(const Point2d& a, const Point2d& b, const Point2d& c)
	{
		float area = AreaTriangle2d(a, b, c);

		if (area > 0 && area < Tolerance())
			area = 0.0;

		if (area < 0 && area >  Tolerance())
			area = 0.0;

		glm::vec2 ab = b - a;
		glm::vec2 ac = c - a;

		if (area > 0.0)
			return RelativePos::LEFT;
		if (area < 0.0)
			return RelativePos::RIGHT;
		if ((ab.x * ac.x < 0.0f) || (ab.y * ac.y < 0.0f))
			return RelativePos::BEHIND;
		if (ab.length() < ac.length())
			return RelativePos::BEYOND;
		if (a == c)
			return RelativePos::ORIGIN;
		if (b == c)
			return RelativePos::DESTINATION;

		return RelativePos::BETWEEN;
	}

	bool Collinear(const glm::vec3& a, const glm::vec3& b)
	{
		//checking that a.x/b.x = a.y/b.y = a.x/b.z
		auto v1 = a.x * b.y - a.y * b.x;
		auto v2 = a.y * b.z - a.z * b.y;
		auto v3 = a.x * b.z - a.z * b.x;
		return Equal(v1, 0) && Equal(v2, 0) && Equal(v3, 0);
	}

	bool Collinear(const Point3d& a, const Point3d& b, const Point3d& c)
	{
		auto AB = b - a;
		auto AC = c - a;
		return Collinear(AB, AC);
	}

	bool Coplanar(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
	{
		float volume = geom::ScalarTripleProduct(a, b, c);
		return geom::Equal(volume, 0);
	}

	bool Coplanar(const Point3d& a, const Point3d& b, const Point3d& c, const Point3d& d)
	{
		auto AB = b - a;
		auto AC = c - a;
		auto AD = d - a;
		float volume = geom::ScalarTripleProduct(AB, AC, AD);
		return geom::Equal(volume, 0);
	}

	
	bool Left(const Point2d& a, const Point2d& b, const Point2d& c)
	{
		return Orientation2d(a, b, c) == RelativePos::LEFT;
	}

	bool Beyond(const Point2d& a, const Point2d& b, const Point2d& c)
	{
		return Orientation2d(a, b, c) == RelativePos::BEYOND;
	}

	bool LeftOrBeyond(const Point2d& a, const Point2d& b, const Point2d& c)
	{
		return Left(a, b, c) || Beyond(a, b, c);
	}

	bool IsConvex(const Vertex2d* v0, const Vertex2d* v1, const Vertex2d* v2)
	{
		return LeftOrBeyond(v0->point, v1->point, v2->point);
	}

	static bool InteriorCheck(const Vertex2d* v1, const Vertex2d* v2)
	{
		if (LeftOrBeyond(v1->point, v1->next->point, v1->prev->point))
		{
			//v1 is convex
			return Left(v1->point, v2->point, v1->prev->point) &&
				Left(v2->point, v1->point, v1->prev->point);
		}
		//v1 is reflex
		return !LeftOrBeyond(v1->point, v2->point, v1->prev->point) &&
			LeftOrBeyond(v2->point, v1->point, v1->prev->point);
	}


	/*
Diagonal check - meaning a line between 2 vertices is entirely within the polygon (assuming CCW) - m

	-The line should not intersect with any other edges
	-If the start vertex of the line is a convex vertex, then 2 neighbours of the start vertex should lie on different sides of the line
	-If the start vertex is a reflex vertex (think this means internal angle > 180) then it should not be an exterior line (next vertex should not be on the left, prev vertex should not be on the right)
*/

	bool IsDiagonal(const Vertex2d* v1, const Vertex2d* v2, Polygon2d* poly)
	{
		bool prospect = true;
		std::vector<Vertex2d*> vertices;

		if (poly)
			vertices = poly->vertex_list;

		else
		{
			auto vertex_ptr = v1->next;
			vertices.push_back((Vertex2d*)v1);
			while (vertex_ptr != v1)
			{
				vertices.push_back(vertex_ptr);
				vertex_ptr = vertex_ptr->next;
			}
		}

		Vertex2d* current;
		Vertex2d* next;
		current = vertices[0];
		do
		{
			next = current->next;
			if(current != v1 && next != v1 && current != v2 && next != v2 &&
				geom::Lines2dIntersect(v1->point, v2->point, current->point, next->point))
			{
				prospect = false; //cannot be a diagonal
				break;
			}
			current = next;
		} while (current != vertices[0]);

		return prospect && InteriorCheck(v1, v2) && InteriorCheck(v2, v1);
	}
}