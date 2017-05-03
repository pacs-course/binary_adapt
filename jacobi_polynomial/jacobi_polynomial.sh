#!/bin/bash
#
g++ -c -I /include jacobi_polynomial.cpp -fPIC
if [ $? -ne 0 ]; then
  echo "Errors compiling jacobi_polynomial.cpp"
  exit
fi
#
echo "Library installed as ~jacobi_polynomials/jacobi_polynomial.o"
