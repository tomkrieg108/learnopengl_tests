#include "pch.h"
#include "vector.h"

namespace jmk
{
	float Cross2D(Vec2f v1, Vec2f v2)
	{
		return v1[X] * v2[Y] - v1[Y] * v2[X];
	}

	Vec3f Cross3D(Vec3f v1, Vec3f v2)
	{
		float x, y, z;

		x = v1[Y] * v2[Z] - v2[Y]*v1[Z];
		y = -(v1[Z] * v2[X] - v1[Z] * v2[X]);
		z = v1[X] * v2[Y] - v2[X] * v1[Y];

		return Vec3f(x, y, z);
	}
}