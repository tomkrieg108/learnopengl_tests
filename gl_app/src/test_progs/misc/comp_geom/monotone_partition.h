#pragma once
#include <vector>
#include "vector.h"
#include "polygon_dcel.h"
#include "geo_utils.h"

namespace geom
{


	namespace jmk {
		void get_monotone_polygons(Polygon2dDECL* poly, std::vector<Polygon2dDECL*>& mono_polies);
	}

}

