#pragma once

#include "vector.h"
#include "point.h"

namespace jmk
{
	
	template<class coord_type, size_t dim = Dim3>
	class LineStd {
		Vector<coord_type, dim> point;
		Vector<coord_type, dim> dir;
		Vector<coord_type, dim> second;
		float d;

	public:
		LineStd() {}

		LineStd(Vector<coord_type, dim>& p1, Vector<coord_type, dim>& p2, bool points = false) {
			if (points) {
				dir = p2 - p1;
				second = p2;
			}
			else
				dir = p2;

			dir.normalize();
			point = p1;
		}

		Vector<coord_type, dim> getPoint() const;

		Vector<coord_type, dim> getSecondPoint() const;

		Vector<coord_type, dim> getDir() const;

		float getD() const;

		void setDirection(Vector<coord_type, dim>& _dir);

		void setPoint(Vector<coord_type, dim>& _point);

		void setD(float value);

	};

	template<class coord_type, size_t dim>
	inline Vector<coord_type, dim> LineStd<coord_type, dim>::getPoint()const
	{
		return point;
	}

	template<class coord_type, size_t dim>
	inline Vector<coord_type, dim> LineStd<coord_type, dim>::getSecondPoint() const
	{
		return second;
	}

	template<class coord_type, size_t dim>
	inline Vector<coord_type, dim> LineStd<coord_type, dim>::getDir() const
	{
		return dir;
	}

	template<class coord_type, size_t dim>
	inline float LineStd<coord_type, dim>::getD() const
	{
		return d;
	}

	template<class coord_type, size_t dim>
	inline void LineStd<coord_type, dim>::setDirection(Vector<coord_type, dim>& _dir)
	{
		dir = _dir;
	}

	template<class coord_type, size_t dim>
	inline void LineStd<coord_type, dim>::setPoint(Vector<coord_type, dim>& _point)
	{
		point = _point;
	}

	template<class coord_type, size_t dim>
	inline void LineStd<coord_type, dim>::setD(float value)
	{
		d = value;
	}

	using Line2dStd = LineStd<float, Dim2>;
	using Line3dStd = LineStd<float, Dim3>;

	using Line2d = LineStd<float, Dim2>;
	using Line3d = LineStd<float, Dim3>;
}