#pragma once

//#include <array>
//#include <iostream>

#include "core.h"

namespace jmk
{
	enum { Dim2 = 2, Dim3 = 3};
	enum {X = 0, Y = 1, Z = 2};
	
	template <typename coord_type, size_t Dim>
	class Vector
	{

	public:

		using VecCoords = std::array<coord_type, Dim>;
		using Vec = Vector<coord_type, Dim>;

		template <typename coord_type, size_t Dim>
		friend coord_type Dot(Vector<coord_type, Dim> vec1, Vector<coord_type, Dim> vec2);

		Vector() {}

		Vector(const VecCoords& coords) : m_coords{ coords } {}

		Vector(coord_type x, coord_type y, coord_type z) 
		{
			static_assert(Dim == 3, "Vector dimension must be 3");
			m_coords = VecCoords({ x,y,z });
		}

		Vector(coord_type x, coord_type y)
		{
			static_assert(Dim == 2, "Vector dimension must be 2");
			m_coords = VecCoords({ x,y });
		}

		bool operator==(const Vec& other) const
		{
			for (auto i = 0; i < Dim; i++)
			{
				if (!IsEqual(m_coords[i], other.m_coords[i]))
					return false;
			}
			return true;
		}

		bool operator !=(const Vec& other) const
		{
			return !(*this == other);
		}

		Vec operator+(const Vec& other) const
		{
			Vec result;
			for ( auto i = 0; i < Dim; i++)
				result.m_coords[i] = m_coords[i] + other.m_coords[i];
			
			return result;
		}

		Vec operator-(const Vec& other) const
		{
			Vec result;
			for ( auto i = 0; i < Dim; i++)
				result.m_coords[i] = m_coords[i] - other.m_coords[i];

			return result;
		}

		bool operator<(const Vec& other) const
		{
			for (auto i = 0; i < Dim; i++)
			{
				if (m_coords[i] < other.m_coords[i])
					return true;
				if (m_coords[i] > other.m_coords[i])
					return false;
			}
			return false;
		}

		bool operator>(const Vec& other) const
		{
			for (auto i = 0; i < Dim; i++)
			{
				if (m_coords[i] > other.m_coords[i])
					return true;
				if (m_coords[i] < other.m_coords[i])
					return false;
			}
			return false;
		}

		coord_type operator [](uint32_t idx) const
		{
			assert(idx < m_coords.size());
			return m_coords[idx];
		}

		void Assign(int idx, coord_type value)
		{
			assert(idx < m_coords.size());
			m_coords[idx] = value;
		}

		
	private:

		static_assert(std::is_floating_point_v<coord_type>, "Vector type must be float or double");
		static_assert(Dim >= 2, "Vector dimension must be at least 2");

		VecCoords m_coords{ 0.0 };

	};

	using Vec2f = Vector<float, 2>;
	using Vec3f = Vector<float, 3>;

	template <typename coord_type, size_t Dim>
	coord_type Dot(Vector<coord_type, Dim> vec1, Vector< coord_type, Dim> vec2)
	{
		assert(vec1.m_coords.size() == vec2.m_coords.size());
		coord_type value{ 0 };
		for (auto i = 0; i < Dim; i++)
			value+= vec1.m_coords[i] * vec2.m_coords[i];
		
		return value;
	}

	float Cross2D(Vec2f v1, Vec2f v2);
	Vec3f Cross3D(Vec3f v1, Vec3f v2);

}