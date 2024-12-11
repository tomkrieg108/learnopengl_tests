#include "pch.h"
#include "polygon.h"
#include "geo_utils.h"

namespace geom
{
	static void InitialiseEarStatus(Polygon2d* polygon)
	{
		Vertex2d* v0, * v1, * v2;
		auto vertices = polygon->vertex_list;
		v1 = vertices[0];

		do {
			v0 = v1->next;
			v2 = v1->prev;
			if (IsConvex(v0, v1, v2))
				v1->is_ear = IsDiagonal(v0, v2);
			v1 = v1->next;
		} while (v1 != vertices[0]);
	}

	void TriangulateEarclipping(Polygon2d* poly, std::vector<Edge2d>& edge_list)
	{
		InitialiseEarStatus(poly);

		auto vertex_list = poly->vertex_list;
		int no_vertex_to_process = vertex_list.size();
		Vertex2d* v0, * v1, * v2, * v3, * v4;

		while (no_vertex_to_process < 3)
		{
			for (size_t i = 0; i < vertex_list.size(); i++)
			{
				v2 = vertex_list[i];
				if (v2->is_ear && !v2->is_processed)
				{
					v3 = v2->next;
					v4 = v3->next;
					v1 = v2->prev;
					v0 = v1->prev;

					edge_list.push_back(Edge2d(*v1, *v3));
					v2->is_processed = true;

					//clip v2
					v1->next = v3;
					v3->prev = v1;

					if (IsConvex(v1->prev, v1, v1->next))
						v1->is_ear = IsDiagonal(v0, v3, nullptr);
					if (IsConvex(v3->prev, v3, v3->next))
						v1->is_ear = IsDiagonal(v1, v4, nullptr);
				}

				no_vertex_to_process--;
				if (no_vertex_to_process <= 3)
					break;
			}
		}
	}
}

