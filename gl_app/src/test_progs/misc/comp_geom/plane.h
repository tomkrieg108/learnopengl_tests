#pragma once
#include "vector.h"

namespace geom
{
	template <typename coord_type>
	struct Plane
	{
		static_assert(std::is_floating_point_v<coord_type>, "Type must be float or double");
		using glm_vec3 = glm::vec<3, coord_type>;
		using glm_point3 = glm::vec<3, coord_type>;
		

		glm_vec3 normal;
		float d;		//ax + dy + cz = d  
		
		Plane(const glm_vec3& normal, float d) 
			: normal{ normal}, d{d}
		{ 
			this->normal = glm::normalize(normal);
		}

		//if plane is view from direction of normal, points will be CCW
		Plane(const glm_point3& p1, const glm_point3& p2, const glm_point3& p3)
		{
			auto p1_p2 = p2 - p1;
			auto p1_p3 = p3 - p1;

			normal = glm::cross(p1_p2, p1_p3);
			normal = glm::normalize(normal); //todo - does normlising change d?
			d = glm::dot(normal, p1);
		}
	};

	using Planef = Plane<float>;
	
}

