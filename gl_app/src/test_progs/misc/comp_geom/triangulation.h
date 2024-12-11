#pragma once
#include "polygon.h"
#include "geo_utils.h"

namespace geom
{
	void TriangulateEarclipping(Polygon2d* poly, std::vector<Edge2d>& edge_list);
}
