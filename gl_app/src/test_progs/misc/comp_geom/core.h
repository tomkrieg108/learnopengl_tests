#pragma once

#include <cmath>
#include <cassert>

namespace geom
{
	enum RelativePos 
	{
		LEFT, RIGHT, BEYOND, BEHIND, BETWEEN, ORIGIN, DESTINATION
	};

	constexpr float Tolerance(const float factor = 10.0f)
	{
		return std::numeric_limits<float>::epsilon() * factor;
	}
	
	inline bool Equal(float v1, float v2)
	{
		return std::fabs(v1 - v2) < Tolerance();
	}

		/*template <typename type>
		static inline bool IsEqual(type x, type y)
		{
			static_assert(std::is_floating_point_v<type>, "Type must be float or double");
			return std::fabs(x - y) < EPSILON;
		}*/

	inline bool Xor(bool x, bool y) {
		return x ^ y;
	}
}

