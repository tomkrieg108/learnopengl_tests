#include "pch.h"
#include "vector.h"

namespace jmk
{
	float crossProduct2d(Vector2f v1, Vector2f v2)
	{
		return v1[X] * v2[Y] - v1[Y] * v2[X];
	}

	Vector3f crossProduct3d(Vector3f v1, Vector3f v2)
	{
		float x, y, z;

		x = v1[Y] * v2[Z] - v2[Y]*v1[Z];
		y = -(v1[Z] * v2[X] - v1[Z] * v2[X]);
		z = v1[X] * v2[Y] - v2[X] * v1[Y];

		return Vector3f(x, y, z);
	}

	float scalarTripleProduct(Vector3f a, Vector3f b, Vector3f c)
	{
		return dotProduct(a, crossProduct3d(b, c));
	}
}