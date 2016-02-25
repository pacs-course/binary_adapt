#ifndef __BASIS__HH
#define __BASIS__HH


template <typename T, int DIM>
class Basis
{
 public:
 	Basis (){};
	double evaluate(size_t i, Point<DIM> x);
 private:
 	std::vector<T > _elements;
}

template <int DIM> //working in |R ^ DIM
class PolynomialBasis : Basis <Poly<DIM> >
{
 public:
	PolynomialBasis ();
}


#endif //__BASIS__HH

