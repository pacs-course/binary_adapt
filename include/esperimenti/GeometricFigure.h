#ifndef __GEOMETRIC_FIGURE_H
#define __GEOMETRIC_FIGURE_H

#include <array> //array

#include "Point.h"

using namespace std;
namespace Geometry
{
	template <size_t DIM, size_t N_VERT>
		class GeometricFigure
		{
			public:
				GeometricFigure()
				{
					Point<DIM> p;
					_vertices.fill(p);
				};

				GeometricFigure(array<Point<DIM>, N_VERT> input) : _vertices(input) {};

				const Point<DIM>& operator[] (size_t ind) const
				{
					return _vertices[ind];
				};
				Point<DIM>& operator[] (size_t ind)
				{
					return _vertices[ind];
				};
				

			private:
			//vertices must be ordered
				array<Point<DIM>, N_VERT> _vertices;
		};

	template <size_t DIM>
		class IperParallelepiped : GeometricFigure<DIM, static_cast<size_t>(pow(2, DIM))>
		{
			public:
//				static size_t size = static_cast<size_t>(pow(2, DIM));

				IperParallelepiped() : GeometricFigure<DIM, static_cast<size_t>(pow(2, DIM))>(){};
				IperParallelepiped(array<Point<DIM>, static_cast<size_t>(pow(2, DIM))> input) : GeometricFigure<DIM, static_cast<size_t>(pow(2, DIM))>(input) {};
			
		};

	using Interval = IperParallelepiped<1>;
	using Rectangle = IperParallelepiped<2>;
	using Parallelepiped = IperParallelepiped<3>;

	class Triangle : public GeometricFigure<2,3>
	{
		Triangle() : GeometricFigure<2,3>() {};
		Triangle(array<Point<2>, 3> input) : GeometricFigure<2,3>(input) {};
	};

	class Tetrahedron : public GeometricFigure<3,4>
	{
		Tetrahedron() : GeometricFigure<3,4>() {};
		Tetrahedron(array<Point<3>, 4> input) : GeometricFigure<3,4>(input) {};
	};



}
#endif //__GEOMETRIC_FIGURE_H
