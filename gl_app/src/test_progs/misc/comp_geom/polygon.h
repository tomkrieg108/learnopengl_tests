#pragma once

#include "vector.h"
#include <list>
#include <vector>

namespace geom
{
	//Vid 25

	template<typename T, uint32_t Dim>
	struct Vertex
	{
		Point<T, Dim> point;
		Vertex<T, Dim>* next = nullptr;
		Vertex<T, Dim>* prev = nullptr;
		bool is_ear = false;
		bool is_processed = false;

		//specify points in CCW order
		Vertex(Point<T, Dim>& point, Vertex<T, Dim>* next, Vertex<T, Dim>* prev) :
			point{point}, next{next}, prev{prev}
		{}

		explicit Vertex(Point<T, Dim>& point) :
			point{ point }
		{}
	};

	template<typename T, uint32_t Dim>
	struct Edge
	{
		Vertex<T, Dim> v1;
		Vertex<T, Dim> v2;

		Edge(Vertex<T, Dim> v1, Vertex<T, Dim> v2) :
			v1{v1}, v2{v2}
		{}
	};


	template<typename T, uint32_t Dim>
	struct Polygon
	{
		std::vector<Vertex<T, Dim>*> vertex_list;

		explicit Polygon(std::list<Point<T, Dim>>& points)
		{
			const auto size = points.size();

			if (size < 3)
			{
				std::cout << "Must be at least 3 points to construct a polygon\n";
				return;
			}

			for (auto p : points)
				vertex_list.push_back(new Vertex{p});
			
			for (auto i = 0; i < size; ++i)
			{
				vertex_list[i]->next = vertex_list[(i + 1) % size];

				if (i != 0)
					vertex_list[i]->prev = vertex_list[i-1];
				else
					vertex_list[i]->prev = vertex_list[size-1];
			}
		}

	};
	
	using Vertex2d = Vertex<float, 2>;
	using Vertex3d = Vertex<float, 3>;

	using Edge2d = Edge<float, 2>;
	using Edge3d = Edge<float, 3>;

	using Polygon2d = Polygon<float,2>;
	using Polygon3d = Polygon<float, 3>;

}



