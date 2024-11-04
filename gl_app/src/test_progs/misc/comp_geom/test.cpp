#include "pch.h"
#include "core.h"
#include "vector.h"
#include "angle.h"


//Todo
//use instructor uses GTest - need to figure out how to use
namespace geom
{
	void AngleLines2DTest1()
	{
		glm::vec2 line1_dir{ 2,1 };
		glm::vec2 line2_dir{ -2,1 };

		Point2d line1_point{ 0,3 };
		Point2d line2_point{ 0,5 };

		geom::Line2d l1 = Line2d::FromPointAndDirection(line1_point, line1_dir);
		geom::Line2d l2 = Line2d::FromPointAndDirection(line2_point, line2_dir);

		std::cout << "AngleLines2DTest1: ";
		std::cout << AngleLines2D(l1, l2) << "\n";
	}


	void AngleLines3DTest1()
	{
		Point3d line1_point1{ 1,5,5 };
		Point3d line1_point2{ 1.5,-5,-2 };

		Point3d line2_point1{ -3, -3, 0 };
		Point3d line2_point2{ 3,5,2 };

		Line3d l1 = Line3d::LineFromPoints(line1_point1, line1_point2);
		Line3d l2 = Line3d::LineFromPoints(line2_point1, line2_point2);

		std::cout << "AngleLines3DTest1: ";
		std::cout << AngleLines3D(l1, l2) << "\n";
	}

	void AngleLinePlaneTest1()
	{
		const Point3d line1_point1{ -3,-4,-5 };
		const Point3d line1_point2{ 4,3,5 };
		const glm::vec3 plane_normal{ -14.26, 9.32, 18.89 };

		const auto line_dir = line1_point1 - line1_point2;
		const Line3d line = Line3d::FromPointAndDirection(line1_point1, line_dir);
		const Planef plane{ plane_normal, 0 };
		const auto result = AngleLinePlane(line, plane);
		std::cout << "AngleLinePlaneTest1: ";
		std::cout << AngleLinePlane(line, plane) << "\n";
	}
}