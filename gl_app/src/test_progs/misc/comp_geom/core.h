#pragma once

#include <cmath>
#include <cassert>

namespace jmk
{
#define Epsilon = 0.0000001

	template <typename type>
	static bool IsEqual(type x, type y)
	{
		static_assert(std::is_floating_point_v<type>, "Type must be float or double");
		return std::fabs(x - y) < Epsilon;
	}
}

