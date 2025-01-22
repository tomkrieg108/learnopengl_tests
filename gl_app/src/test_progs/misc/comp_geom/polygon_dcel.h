#pragma once

#include "vector.h"
#include <list>
#include <vector>

namespace geom
{
	static int _id = 1;

	template<typename T, uint32_t Dim>
	struct EdgeDCEL;

	template<typename T, uint32_t Dim>
	struct FaceDCEL;

	template<typename T, uint32_t Dim>
	struct PolygonDCEL;

	template<typename T, uint32_t Dim>
	struct VertexDCEL
	{
		Point<T, Dim> point;
		EdgeDCEL<T, Dim>* incident_edge = nullptr; //Incident edge to the vertex

		explicit VertexDCEL(Point<T, Dim>& point) :
			point{point}
		{}

		void Print()
		{
			//assumes 2d!
			std::count << "(" << point.x << "," << point.y << ")\n";
		}
	};

	template<typename T, uint32_t Dim>
	struct EdgeDCEL
	{
		VertexDCEL<T, Dim>* origin = nullptr;
		EdgeDCEL<T, Dim>* twin = nullptr;
		EdgeDCEL<T, Dim>* next = nullptr;
		EdgeDCEL<T, Dim>* prev = nullptr;
		FaceDCEL<T, Dim>* incident_face = nullptr;
		int id;

		EdgeDCEL() { id = -1; }

		explicit EdgeDCEL(VertexDCEL<T, Dim>* origin) :
			origin{origin}
		{}

		VertexDCEL<T, Dim>* Destination()
		{
			return twin->origin;
		}

		void Print()
		{
			std::cout << "This point pointer" << this << "\n";
			std::cout << "Origin : "; this->origin->print();
			std::cout << "Twin pointer" << this->twin << "\n";
			std::cout << "Next pointer" << this->next << "\n";
			std::cout << "Prev pointer" << this->prev << "\n";
		}

	};

	template<typename T, uint32_t Dim>
	struct FaceDCEL
	{
		EdgeDCEL<T, Dim>* outer = nullptr; //pointer to 1 of the CCW edges
		std::vector<EdgeDCEL<T, Dim>*> inner; //pointer to a helf egde of each hole polygon CW order

		std::vector<EdgeDCEL<T, Dim>*> GetEdgeList()
		{
			std::vector<EdgeDCEL<T, dim>*> edge_list;
			if (outer) {
				auto edge_ptr = outer;
				auto next_ptr = outer->next;
				edge_list.push_back(edge_ptr);
				edge_ptr->origin->print();
				while (next_ptr != edge_ptr) {
					edge_list.push_back(next_ptr);
					next_ptr = next_ptr->next;
				}
			}
			return edge_list;
		}

		std::vector<Point<T, Dim>> GetPoints()
		{
			std::vector<Point<float, T>> point_list;
			if (outer) {
				auto edge_ptr = outer;
				auto next_ptr = outer->next;
				point_list.push_back(edge_ptr->origin->point);
				while (next_ptr != edge_ptr) {
					point_list.push_back(next_ptr->origin->point);
					next_ptr = next_ptr->next;
				}
			}
			return point_list;
		}

		void Print() {
			if (outer)
			{
				auto edge_ptr = outer;
				auto next_ptr = outer->next;

				edge_ptr->origin->print();
				while (next_ptr != edge_ptr) {
					next_ptr->origin->print();
					next_ptr = next_ptr->next;
				}
			}
		}

	};

	using Vertex2dDCEL = VertexDCEL<float, 2U>;
	using Edge2dDCEL = EdgeDCEL<float, 2U>;
	using Polygon2dDECL = PolygonDCEL<float, 2U>;

	/*typedef VertexDCEL<float, 2U>	Vertex2dDCEL;
	typedef EdgeDCEL<float, 2U>	Edge2dDCEL;
	typedef PolygonDCEL<float, 2U>	Polygon2d;*/

	template<typename T, uint32_t Dim>
	struct PolygonDCEL
	{

		using PointNf = Point<T, Dim>;

		std::vector<VertexDCEL<T, Dim>*> vertex_list;
		std::vector<EdgeDCEL<T, Dim>*> edge_list;
		std::vector<FaceDCEL<T, Dim>*> face_list;

		EdgeDCEL<T, Dim>* empty_edge = new EdgeDCEL<T, Dim>();

		explicit PolygonDCEL(std::vector< PointNf>& _points)
		{
			int size = _points.size();
			// Polygon should have atleast tree vertices.
			if (size < 3)
				return;

			for (size_t i = 0; i < _points.size(); i++) {
				vertex_list.push_back(new VertexDCEL<T, Dim>(_points[i]));
			}

			for (size_t i = 0; i <= vertex_list.size() - 2; i++) {
				auto hfedge = new EdgeDCEL<T, Dim>(vertex_list[i]);
				auto edge_twin = new EdgeDCEL<T, Dim>(vertex_list[i + 1]);

				vertex_list[i]->incident_edge = hfedge;

				hfedge->twin = edge_twin;
				edge_twin->twin = hfedge;

				edge_list.push_back(hfedge);
				edge_list.push_back(edge_twin);
			}

			auto hfedge = new EdgeDCEL<T, Dim>(vertex_list.back());
			auto edge_twin = new EdgeDCEL<T, Dim>(vertex_list.front());

			hfedge->twin = edge_twin;
			edge_twin->twin = hfedge;
			edge_list.push_back(hfedge);
			edge_list.push_back(edge_twin);

			vertex_list[vertex_list.size() - 1]->incident_edge = hfedge;

			// Set the prev and next for the element middle of the list ( 2 : size- 2)
			for (size_t i = 2; i <= edge_list.size() - 3; i++) {

				if (i % 2 == 0) // Even case. Counter clockwise edges
				{
					edge_list[i]->next = edge_list[i + 2];
					edge_list[i]->prev = edge_list[i - 2];
				}
				else           // Odd case. Clockwise edges
				{
					edge_list[i]->next = edge_list[i - 2];
					edge_list[i]->prev = edge_list[i + 2];
				}
			}

			edge_list[0]->next = edge_list[2];
			edge_list[0]->prev = edge_list[edge_list.size() - 2];
			edge_list[1]->next = edge_list[edge_list.size() - 1];
			edge_list[1]->prev = edge_list[3];

			edge_list[edge_list.size() - 2]->next = edge_list[0];
			edge_list[edge_list.size() - 2]->prev = edge_list[edge_list.size() - 4];
			edge_list[edge_list.size() - 1]->next = edge_list[edge_list.size() - 3];
			edge_list[edge_list.size() - 1]->prev = edge_list[1];

			// Configure the faces.
			FaceDCEL<T, Dim>* f1 = new FaceDCEL<T, Dim>();
			FaceDCEL<T, Dim>* f2 = new FaceDCEL<T, Dim>();

			f1->outer = edge_list[0];
			// f2 is unbounded face which wrap the f1. So f1 is a hole in f2. So have clockwise edges in innder edge list
			f2->inner.push_back(edge_list[1]);

			face_list.push_back(f1);
			face_list.push_back(f2);

			f1->outer->incident_face = f1;
			EdgeDCEL<T, Dim>* edge = f1->outer->next;
			while (edge != f1->outer)
			{
				edge->incident_face = f1;
				edge = edge->next;
			}

			// f2->inner has halfedges connect in clockwise order
			f2->inner[0]->incident_face = f2;
			edge = f2->inner[0]->next;
			while (edge != f2->inner[0])
			{
				edge->incident_face = f2;
				edge = edge->next;
			}
		}

		void PolygonDCEL<T, Dim>::GetEdgesWithSamefaceAndGivenOrigins(
			VertexDCEL<T, Dim>* _v1, VertexDCEL<T, Dim>* _v2,
			EdgeDCEL<T, Dim>** edge_leaving_v1, EdgeDCEL<T, Dim>** edge_leaving_v2)
		{
			std::vector<EdgeDCEL<T, Dim>*> edges_with_v1_ori, edges_with_v2_ori;

			// Get all the edges with orgin _v1
			auto v1_inci_edge = _v1->incident_edge;
			edges_with_v1_ori.push_back(v1_inci_edge);

			auto next_edge = v1_inci_edge->twin->next;
			while (next_edge != v1_inci_edge) {
				edges_with_v1_ori.push_back(next_edge);
				next_edge = next_edge->twin->next;
			}

			// Get all the edges with orgin _v2
			auto v2_inci_edge = _v2->incident_edge;
			edges_with_v2_ori.push_back(v2_inci_edge);

			next_edge = v2_inci_edge->twin->next;
			while (next_edge != v2_inci_edge)
			{
				edges_with_v2_ori.push_back(next_edge);
				next_edge = next_edge->twin->next;
			}

			// Get two edges, one with origin v1 and other with origin v2 and incident to same face
			for (auto ev1 : edges_with_v1_ori) {
				for (auto ev2 : edges_with_v2_ori) {
					if (ev1->incident_face->outer != nullptr) {
						if (ev1->incident_face == ev2->incident_face) {
							*edge_leaving_v1 = ev1;
							*edge_leaving_v2 = ev2;
							return;
						}
					}
				}
			}

		}

		//Insert edge vetween v1 and v2
		bool Split(VertexDCEL<T, Dim>* _v1, VertexDCEL<T, Dim>* _v2)
		{
			// Find two edges with given points as origins and are in same face.
			EdgeDCEL<T, Dim>* edge_oriV1;
			EdgeDCEL<T, Dim>* edge_oriV2;
			GetEdgesWithSamefaceAndGivenOrigins(_v1, _v2, &edge_oriV1, &edge_oriV2);

			if (edge_oriV1->id == -1 || edge_oriV2->id == -1)
				return false;						// Cannot find a edges with same face with ori _v1, _v2

			// If the vertices are adjucent we can return.
			if (edge_oriV1->next->origin == _v2 || edge_oriV1->prev->origin == _v2)
				return false;

			// Later we can delete this entry
			FaceDCEL<T, Dim>* previous_face = edge_oriV1->incident_face;

			auto half_edge1 = new EdgeDCEL<T, Dim>(_v1);
			auto half_edge2 = new EdgeDCEL<T, Dim>(_v2);

			half_edge1->twin = half_edge2;
			half_edge2->twin = half_edge1;
			half_edge1->next = edge_oriV2;
			half_edge2->next = edge_oriV1;

			half_edge1->prev = edge_oriV1->prev;
			half_edge2->prev = edge_oriV2->prev;

			half_edge1->next->prev = half_edge1;
			half_edge2->next->prev = half_edge2;
			half_edge1->prev->next = half_edge1;
			half_edge2->prev->next = half_edge2;

			FaceDCEL<T, Dim>* new_face1 = new FaceDCEL<T, Dim>();
			new_face1->outer = half_edge1;
			half_edge1->incident_face = new_face1;
			auto temp_edge = half_edge1->next;
			while (temp_edge != half_edge1) {
				temp_edge->incident_face = new_face1;
				temp_edge = temp_edge->next;
			}

			FaceDCEL<T, Dim>* new_face2 = new FaceDCEL<T, Dim>();
			new_face2->outer = half_edge2;
			half_edge2->incident_face = new_face2;
			temp_edge = half_edge2->next;
			while (temp_edge != half_edge2) {
				temp_edge->incident_face = new_face2;
				temp_edge = temp_edge->next;
			}

			face_list.push_back(new_face1);
			face_list.push_back(new_face2);

			auto itr = std::find(face_list.begin(), face_list.end(), previous_face);

			if (itr != face_list.end()) {
				face_list.erase(itr);
				delete previous_face;
			}

			return true;
		}

		//Join the 2 faces separated by v1, v2
		bool Join(VertexDCEL<T, Dim>* v1, VertexDCEL<T, Dim>* v2)
		{
			return false;
		}

		VertexDCEL<T, Dim>* PolygonDCEL<T, Dim>::GetVertex(PointNf& _point)
		{
			for (size_t i = 0; i < vertex_list.size(); i++) {
				if (_point == vertex_list[i]->point)
					return vertex_list[i];
			}
			return nullptr;
		}


		struct Vertex2DSortTBLR 
		{
			bool operator()(Vertex2dDCEL* ref1, Vertex2dDCEL* ref2) {
				auto a = ref1->point;
				auto b = ref2->point;
				if ((a[Y] > b[Y])
					|| (a[Y] == b[Y]) && (a[X] < b[X]))
				{
					return true;
				}
				return false;
			}
		};

	};
}