#pragma once
#include "vector.h"

namespace geom
{
	template <class coord_type, uint32_t dim = 3>
	struct Line
	{
		static_assert(std::is_floating_point_v<coord_type>, "Type must be float or double");
		static_assert((dim == 2) || (dim == 3), "Line dimension must be 2 or 3");
		using glm_vec = glm::vec<dim, coord_type>;
		using glm_point = glm::vec<dim, coord_type>;

		Line(const glm_point& start_point, const glm_point& end_point) :
			start_point{ start_point }, end_point{ end_point }
		{
			direction = glm::normalize(end_point - start_point);
		}

		static Line LineFromPoints(const glm_point& start_point, const glm_point& end_point)
		{
			Line line{};
			line.start_point = start_point;
			line.end_point = end_point;
			line.direction = glm::normalize(end_point - start_point);
			return line;
		}
		static Line FromPointAndDirection(const glm_point& start_point, const glm_vec& direction)
		{
			Line line{};
			line.start_point = start_point;
			line.end_point = start_point + direction;
			line.direction = glm::normalize(direction);
			return line;
		}

		glm_point start_point;
		glm_point end_point;
		glm_vec direction; //normalized

	private:
		//todo - needs variable number of args
		Line(){};
	};

	using Line2d = Line<float, 2>;
	using Line3d = Line<float, 3>;
}
