#ifndef __BASE_BUILDER_H
#define __BASE_BUILDER_H

#include <vector>
#include <memory> //unique_ptr

#include "Polinomial.h"

namespace FiniteElements
{
	class PoliBaseBuilderRule
	{
		public:
			PoliBaseBuilderRule(){};
			virtual vector<Polinomial<1>> operator() (size_t degree) = 0; 
	};

	using PoliBaseBuilderRule_ptr = std::unique_ptr<PoliBaseBuilderRule>;

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
			InvalidBuilder();

			/*this method will never be called since the constructor throws an exception*/
			virtual vector<Polinomial<1>> operator() (size_t degree);
	};

}//namespace FiniteElements

#endif //__BASE_BUILDER_H
