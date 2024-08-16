#include "pch.h"
#include "vector.h"

namespace jmk
{
	static Vector<float, 3> vec;

	static Vector<float, 3> vec2{ 1.0f, 2.0f, 3.0f };

	static void test()
	{
		Vec3f v1(1.0f, 1.0f, 1.0f);
		Vec3f v2(1.0f, 1.0f, 1.0f);

		Vec3f v3 = v1 + v2;
		Vec3f v4 = v1 - v2;

		float dp = Dot(v1, v2);
		Vec3f cp = Cross3D(v1, v2);


	}
}