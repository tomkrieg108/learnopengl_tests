#pragma once

#include <cmath>
#include <cassert>

namespace jmk
{
#define EPSILON 0.000001
#define TOLERANCE 0.000001
#define ZERO 0.0

enum RELATIVE_POSITION {
		LEFT, RIGHT, BEYOND, BEHIND, BETWEEN, ORIGIN, DESTINATION
	};

	template <typename type>
	static bool IsEqual(type x, type y)
	{
		static_assert(std::is_floating_point_v<type>, "Type must be float or double");
		return std::fabs(x - y) < EPSILON;
	}

	static bool isEqualD(double x, double y)
	{
		return fabs(x - y) < TOLERANCE;
	}

	static bool _xor(bool x, bool y) {
		return x ^ y;
	}

	static float RadianceToDegrees(float radiance) {
		return radiance * 360 / (2 * M_PI);
	}
}

