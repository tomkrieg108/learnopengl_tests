#include "pch.h"
#include "core.h"
#include "vector.h"
#include "angle.h"
#include "polygon.h"
#include <cmath>
#include <numbers>


//Todo
//use instructor uses GTest - need to figure out how to use
namespace geom
{

	const float pi = std::acosf(-1.0f);

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

	void CreateSimplePloygon()
	{
		std::list<Point2d> points{ {2,6}, {3,7}, {4,8}, {5,7} };
		Polygon2d polygon2d{ points };
	}

	void ComputeHemisphereIntegral()
	{
		//If use float, sample size < about 0.001 starts giving incorrect results due to numerical limits
		double sampleDelta = 0.001;
		double nrSamples = 0.0;
		double calcuated_samples = (2.0 * M_PI) * (0.5 * M_PI) / (sampleDelta * sampleDelta);
		double I = 0;
		for (double theta = 0.0; theta < 2.0 * M_PI; theta += sampleDelta)
		{
			for (double phi = 0.0; phi < 0.5 * M_PI; phi += sampleDelta)
			{
				//I += cos(phi) * sin(phi) * sampleDelta * sampleDelta;
				I += cos(phi) * sin(phi);
				nrSamples++;
			}
		}
		// instead of doing this mult on each iteration in the loop, do once outside the loop - same thing!
		I *= sampleDelta * sampleDelta;

		std::cout << "I = " << I << "\n"; //Gives 3.142
		std::cout << "Samples: " << nrSamples << "\n";
		std::cout << "Calcuated Samples: " << calcuated_samples << "\n";


		//std::cout << "I = " << I / nrSamples << "\n"; //Gives ~ 0.32 = 1/(PI)
		//std::cout << "I = " << M_PI * I / nrSamples << "\n"; //Gives 1.0
	}
}