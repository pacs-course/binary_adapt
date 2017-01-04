#ifndef __LEGENDRE_BASIS_H
#define __LEGENDRE_BASIS_H

#include "Basis.h"

#include "jacobi_polynomial.hpp"

#include <memory> //std::shared_ptr

namespace FiniteElements
{
	template <size_t dim>
		class LegendreBasis : public AbstractBasis<dim>
		{
			public:
				/* constructor */
				LegendreBasis() : AbstractBasis<dim>() {};

				LegendreBasis& operator = (const LegendreBasis& input_basis)
				{
					if (this != &input_basis)
					{
						//TODO
					}
					return *this;
				};

				LegendreBasis(const LegendreBasis& input_basis)
				{
					*this = input_basis;
				};

				/* In the Legendre case I can optimize this function */
				virtual vector<double> EvaluateBasis(size_t degree, const Geometry::Point<dim>& p) override
				{
					size_t length = this->ComputeSize(degree);
					this->UpdateSize(length);
					std::array< double*, dim> evaluations;
					auto p_iter = p.begin();
					auto eval_iter = evaluations.begin();
					for(size_t i(0); i < dim; ++i)
					{
						double x = *(p_iter++);
						*(eval_iter++) = j_polynomial(1, degree, 0, 0, &x);
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
					double result = (j_polynomial(1, index, 0, 0, &x))[index];
#ifdef MYDEBUG
					cout << "Eseguo la valutazione del polinomio di indice " << index << endl; 
					cout << "Valuto il punto : " << x << endl;
					cout << "Valutazione : " << result << endl;
					if (index == 3)
					{
						double ex_result = (0.5*(5*x*x*x - 3*x));
						cout << "La valutazione corretta dovrebbe essere : " << ex_result << endl;
						if (abs(ex_result-result) > 1E-14)
							cout << "Houston we have a problem!" << endl;
					}
#endif //MYDEBUG
/* manual evaluation version */					
//					double previous_result = 1.0;
//					double result = x;
//					if (index == 0)
//						return previous_result;
//						
//					for (size_t k = 1; k < index; ++k)
//					{
//						double k_val = static_cast<double>(k);
//						double temp = previous_result;
//						previous_result = result;
//						result = ( (2*k_val + 1) * x * result - k_val * temp) / (k_val + 1);
//					}
					return result;
				};
		};

} //namespace FiniteElements
#endif //__LEGENDRE_BASIS_H
