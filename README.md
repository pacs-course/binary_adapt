 binaryAdapt - C++
 =================

 This library provides a template implementation of the binary tree approximation algorithm
 for mesh hp-refinement depicted in:

 ["Peter Binev. Tree approximation for hp-adaptivity. 2015."](http://imi.cas.sc.edu/papers/293/)

 The library consists of a series of shared objects and a set of 'examples' to show how to use the library.

 For both libraries and binaries, two versions are created:
   * a normal, optimized version
   * a debug version, suitable to allow symbolic debugging with standard Linux tools (e.g., gdb)

 ## Contents

 1. Building the software
 2. Usage
 3. Known limitations
 4. Bug reports


 ## Building the software

 ### Requirements

 Base requirements to build the package:

  * GNU-compatible Make or gmake
  * POSIX-standard shell
  * A 2014 ISO C++ standard compliant GNU compiler

 ### Dependencies

 Before the software can be built, the following packages must be installed (inside brackets the tested version):

  * [Eigen](http://eigen.tuxfamily.org) [version 3.3.4]
  * [GetPot](http://getpot.sourceforge.net) [version 1.1.17]
  * [GTest](https://github.com/google/googletest)
  * [MuParser](http://beltoforion.de/article.php?a=muparserx) [version 2.2.5]
  * [LibMesh](http://libmesh.github.io/) [version 1.1.0]

 Note: the LibMesh library must have been built without MPI support
 (refer to LibMesh installation instructions: configure --disable-mpi)

  * [JacobiPolynomial](http://people.sc.fsu.edu/~jburkardt/cpp_src/jacobi_polynomial/jacobi_polynomial.html)
  * [SandiaRules](http://people.sc.fsu.edu/~jburkardt/cpp_src/sandia_rules/sandia_rules.html)

 Note: these last two are libraries released as source only, so they have been included in binaryAdapt release,
 together with suggested instructions to compile them.
 It is understood that the user can provide his own installation of these libraries. 

 Moreover, optional packages are needed to visualize the results of the examples:

  * [Octave](https://www.gnu.org/software/octave/) [version 4.0.0]
  * [gnuplot](http://www.gnuplot.info/)[version 5.0]

 ### Pre-installation Configuration

 unzip the distribution archive somewhere
 edit the Makefile.inc and set the environment variables therein to the correct paths for the dependent libraries.

 ### Installation
 
 Do:

	- make

	- make check (optional, runs the test programs)

	- make install

 The "make install" step will create the libraries in the 'lib' folder.
 Modify the installation destination by setting the InstallDir environment variable in Makefile.inc

 To generate the documentation do:

	- make doc

 To generate the example binaries do:

	- make examples


 ## Usage


 ### Post-installation Configuration
	

 After installation, the software is ready to use.
 The user can modify its internal working parameters editing the following configuration files:

 * binary_tree.conf : configure the parameters of the algorithm;
 * quadrature_rules/mesh_quadrature/include/mesh_quadrature.conf : configure the libmesh quadrature rule;
 * quadrature_rules/sandia_quadrature/include/sandia_quadrature.conf : configure the sandia quadrature rule;
 * interpolating_functions/include/interpolating_functions.conf : configure the muparser expression [ remember to set the muparser functor in binary_tree.conf in order to use muparser framework ]

 Pass as main parameter your own configuration file in alternative to the default one binary_tree.conf;
 other configuration files are not replaceable.

 ### Running the examples

 The software is distributed with four examples.
 They are created in the folders 'example/example[n]/bin', with [n] ranging from 1 to 4,
 in both normal and debug version, and can be launched with the commands:

	- ./Example_run.sh [n]
	- ./Example_debug.sh [n]

 ## Known limitations

 * libMesh redefines GetPot classes without including them in proper namespace,
	  so using togheter GetPot and libMesh library is not possible at the moment
	  because of this conflict which causes unpredictable behaviour of the program.
	  Bug has been reported to libMesh developers, and private communication started
	  to fix the problem: at the moment a patch is under development.
	  The patched libMesh will allow the user to configure libMesh with
	  --with-getpot-namepace=libMesh flag, so that any conflict will be avoided.
	  Until the patch release the libMesh version of getpot has to be used also
	  by plugins which do not need any other libMesh object.
 * At the end of each example a "Memory leak detected!" warning is printed at standard output.
	  This is due to the libMesh mode of operation, which assumes that every libMesh instruction
	  is called inside a LibMeshInit block. The correctness of the implementation,
	  and so the absence of any memory leakage, has been verified via valgrind tool.
	  To make the check do:

	- ./Example_valgrind.sh [n]
 

 ## Bug reports, Feature requests

 To notify me in case there are any bugs,
 or to give suggestions about the add of new features in the library,
 my email address is:

 simone.carriero92@gmail.com

 Thanks for using this software.
 Enjoy !


 December 2017, 
 Simone Carriero

