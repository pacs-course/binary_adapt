#ifndef __LEGENDRE_BASIS_H
#define __LEGENDRE_BASIS_H

#include "Basis.h"

#include <memory> //std::shared_ptr

//jacobi_polynomial.hpp need the "using namespace std" before the include
//I'm in a header file, I don't want to put "using namespace std" outside the namespace FiniteElements
using namespace std;
#include "jacobi_polynomial.hpp"

namespace FiniteElements
{
	unique_ptr<double[]> ManualOneDLegendreEvaluation(size_t index, double x);

	template <size_t dim>
		class LegendreBasis : public TensorialBasis<dim>
		{
			public:
				/* constructor */
				LegendreBasis() : TensorialBasis<dim>() {};

				LegendreBasis& operator = (const LegendreBasis& input_basis)
				{
					if (this != &input_basis)
					{
						TensorialBasis<dim>::Copy(input_basis);
					}
					return *this;
				};

				LegendreBasis(const LegendreBasis& input_basis)
				{
					*this = input_basis;
				};

//#define MYVERSION
				/* In the Legendre case I can optimize this function thanks to the jacobi_polynomial working principle */
				virtual vector<double> EvaluateBasis(size_t degree, const Geometry::Point<dim>& p) override
				{
					size_t length = this->ComputeSize(degree);
					this->UpdateSize(length);
					std::vector<std::unique_ptr<double[]>> evaluations;
					auto p_iter = p.begin();
					for(size_t i(0); i < dim; ++i)
					{
						double x = *(p_iter++);
#ifndef MYVERSION
						evaluations.push_back(std::move(std::unique_ptr<double[]> (j_polynomial(1, degree, 0, 0, &x))));
#else //MYVERSION
						evaluations.push_back(std::move(ManualOneDLegendreEvaluation(degree, x)));
#endif //MYVERSION
					}

					vector<double> result;
					for( size_t i(0); i < length; ++i)
					{
						double tot(1);
						for (size_t j(0); j < dim; ++j)
							tot *= evaluations[j][this->_tensorial_indexes[i][j]];
						result.push_back(tot);
					};

					return result;
				};


			protected:
				virtual double OneDEvaluation(size_t index, double x)const override
				{
					static bool called = true;
					if (called)
					{
						cerr << "WARNING: OneDEvaluation method in LegendreBasis is deprecated because of inefficiency" << endl;
						called = false;
					}
#ifndef MYVERSION 
					unique_ptr<double[]> eval(j_polynomial(1, index, 0, 0, &x));
					double result = eval[index];
#else //MYVERSION
					unique_ptr<double[]> eval = move(ManualOneDLegendreEvaluation(index, x));
					double result = eval[index];
#endif //MYVERSION
					return result;
				};
		};

		unique_ptr<double[]> ManualOneDLegendreEvaluation(size_t index, double x)
		{
			unique_ptr<double[]> result(new double[index+1]);
			double previous_value = 1.0;
			double value = x;
			result[0] = previous_value;
			if (index >= 1)
				result[1] = value;
				
			for (size_t k = 1; k < index; ++k)
			{
				double k_val = static_cast<double>(k);
				double temp = previous_value;
				previous_value = value;
				value = ( (2*k_val + 1) * x * value - k_val * temp) / (k_val + 1);
				result[k + 1] = value;
			}
			return move(result);
		};
} //namespace FiniteElements
#endif //__LEGENDRE_BASIS_H
