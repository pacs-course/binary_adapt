#ifndef __BASE_BUILDER_H
#define __BASE_BUILDER_H

#include <vector>
#include <memory> //unique_ptr

#include "Polinomial.h"

class PoliBaseBuilderRule
{
	public:
		PoliBaseBuilderRule(){};
		virtual vector<Polinomial<1>> operator() (size_t degree) = 0; 
};

class Legendre : public PoliBaseBuilderRule
{
	public:
		Legendre() : PoliBaseBuilderRule(){};
		virtual vector<Polinomial<1>> operator() (size_t degree);
};

class Trivial : public PoliBaseBuilderRule
{
	public:
		Trivial() : PoliBaseBuilderRule(){};
		virtual vector<Polinomial<1>> operator() (size_t degree);
};

using PoliBaseBuilderRule_ptr = std::unique_ptr<PoliBaseBuilderRule>;
#endif //__BASE_BUILDER_H
