#ifndef __BASE_BUILDER_H
#define __BASE_BUILDER_H

#include <vector>
#include <memory> //unique_ptr

#include "Polinomial.h"

namespace Basis
{
	class PoliBaseBuilderRule
	{
		public:
			PoliBaseBuilderRule(){};
			virtual vector<Polinomial<1>> operator() (size_t degree) = 0; 
	};

	class LegendreBuilder : public PoliBaseBuilderRule
	{
		public:
			LegendreBuilder() : PoliBaseBuilderRule(){};
			virtual vector<Polinomial<1>> operator() (size_t degree);
	};

	class TrivialBuilder : public PoliBaseBuilderRule
	{
		public:
			TrivialBuilder() : PoliBaseBuilderRule(){};
			virtual vector<Polinomial<1>> operator() (size_t degree);
	};

	class InvalidBuilder : public PoliBaseBuilderRule
	{
		public:
			InvalidBuilder() { throw invalid_argument("Trying to construct a builder object for an INVALID type basis");};
			/*the method will never be called since the constructor throws an exception*/
			virtual vector<Polinomial<1>> operator() (size_t degree) {
																							/*to avoid warning by the compiler*/
																							(void)degree;
																							return vector<Polinomial<1>>();
																						};
	};
	using PoliBaseBuilderRule_ptr = std::unique_ptr<PoliBaseBuilderRule>;

}//namespace Basis

#endif //__BASE_BUILDER_H
