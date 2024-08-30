#include "pch.h"
#include "vector.h"

namespace jmk
{
	static Vector<float, 3> vec;

	static Vector<float, 3> vec3{ 1.0f, 2.0f, 3.0f };

	static void test()
	{
		Vector3f v1(1.0f, 1.0f, 1.0f);
		Vector3f v2(1.0f, 1.0f, 1.0f);

		Vector3f v3 = v1 + v2;
		Vector3f v4 = v1 - v2;

		float dp = dotProduct(v1, v2);
		Vector3f cp = crossProduct3d(v1, v2);


	}
}