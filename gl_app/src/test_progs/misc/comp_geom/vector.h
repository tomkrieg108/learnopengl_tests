#pragma once

#include "core.h"
#include <glm/glm.hpp>


namespace geom
{
#if 0
	template <typename T, uint32_t Dim>
	struct Point
	{
		static_assert(std::is_floating_point_v<coord_type>, "Type must be float or double");
		static_assert((dim == 2) || (dim == 3), "Point dimension must be 2 or 3");
		glm::vec<Dim, T> point;

		Point();

		//need variable number of args
		//Point()
	};
#endif

	//Points are position vextors
	template <typename T, uint32_t Dim>
	using Point = glm::vec<Dim, T>;

	using Point2d = glm::vec2;
	using Point3d = glm::vec3;
	
	//Apparently these are required somewhere
	template <typename T, uint32_t Dim>
	bool operator < (glm::vec<Dim,T> v1, glm::vec<Dim, T> v2)
	{
		for (auto i = 0; i < Dim; ++i)
		{
			if (v1[i] < v2[i])
				return true;
			if (v1[i] > v2[i])
				return false;
		}
		return false;
	}

	template <typename T, uint32_t Dim>
	bool operator > (glm::vec<Dim, T> v1, glm::vec<Dim, T> v2)
	{
		return !(v1 < v2);
	}

	//ToDo (z comp = 0)
	//float Cross2D(glm::vec3 v1, glm::vec3 v2);

	float Cross2D(glm::vec2 v1, glm::vec3 v2);

	//Same as determinant
	float ScalarTripleProduct(glm::vec3 a, glm::vec3 b, glm::vec3 c);
}

