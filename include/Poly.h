#ifndef __POLY__HH
#define __POLY__HH

#include <vector>

using namespace Geometry;

	class Poly1D
	{
		public:
			using CoeffContainer = std::vector<double>;

//remember that the polinomial degree is one less the size of the _coefficients vector
		Poly1D();
		Poly1D(std::vector<double>&);
		Poly1D(std::size_t degree, double coeff = 0);

		double operator () (Point<1>&);
		double operator () (double&);
		
		std::size_t	degree ();
		void			degree (std::size_t& p, double& val = 0);

		private:
			CoeffContainer _coefficients;
	}

template <int DIM>
	class Poly
	{
		public:
			using CoeffContainer = std::vector<double>;
 
			Poly();
			Poly(std::vector<double>&);
			Poly(std::size_t, double);

			double operator() (Point<DIM> const& x) const
			{
				double result(0);
				for (auto x_i : x)
				{

				}
			};
			std::size_t getDegree() const;

		private:
			std::array <CoeffContainer, DIM> _coefficients;
	}


#endif //__POLY__HH
