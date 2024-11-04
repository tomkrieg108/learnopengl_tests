#include "pch.h"
#include "vector.h"

namespace geom
{
	float Cross2D(glm::vec2 v1, glm::vec3 v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}

	float ScalarTripleProduct(glm::vec3 a, glm::vec3 b, glm::vec3 c)
	{
		//same as det[a b c] = volume of parallelapiped = 6 * vol of tetrahedron
		return glm::dot(a, glm::cross(b, c));
	}
}