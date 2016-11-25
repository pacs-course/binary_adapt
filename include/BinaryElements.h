#ifndef __BINARY_ELEMENT_H
#define __BINARY_ELEMENT_H

#include "Point.h"
#include "Element.h"

#include <memory> //std::shared_ptr
#include <functional> //std::function
#include <limits> //numeric_limits::max()

#include "libmesh/getpot.h" //GetPot
#include "libmesh/mesh.h"
#include "libmesh/libmesh.h"
#include <mesh_refinement.h>

#define ENABLE_IF_VERSION
//TODO: verify if it can be written an ENABLE_IF_VERSION-undefined compiling version of the class

using namespace std;

namespace BinaryTree
{
	template <size_t DIM>
		using function_ptr = shared_ptr<function<double(Point<DIM>)>>;


/*
	It contains s attribute and relatively to-access-method needed by the algorithm
	This is meant to be the type of the master (ghost) node of the binary tree, where I don't have a geometry and so I don't have to compute errors, but only to store informations for the node to be refined
*/
	class BinaryNode
	{
		public:
			BinaryNode();

			virtual BinaryNode* s() const
			{
				return _s_element;
			};

			virtual void s(BinaryNode* delta)
			{
				_s_element = delta;
			};

			virtual double E() const
			{
				return _E;
			};
			virtual void E (double val)
			{
				_E = val;
			};

			virtual double E_tilde () const
			{
				return _E_tilde;
			};
			virtual void E_tilde (double val)
			{
				_E_tilde = val;
			};

			virtual double q() const
			{
				return _q;
			};
			virtual void q(double val)
			{
				_q = val;
			};

			virtual BinaryNode* left() = 0;
			virtual BinaryNode* right() = 0;
			virtual BinaryNode* dad() = 0;

			virtual double projectionError() = 0;
			virtual void bisect(libMesh::MeshRefinement &mesh_refinement) = 0;

			virtual void	pLevel(size_t val) = 0;
			virtual size_t pLevel() const = 0;

		protected:
			virtual void updateProjectionError() = 0;

		protected:
			BinaryNode* _s_element;
			double _E;
			double _E_tilde;
			double _q;
	};

	struct BinarityMap
	{
		static bool check_binarity(libMesh::MeshBase& mesh);

		//TODO: introduce support for meshes containing different dimension elements (i.e. both hexes and quads in the same mesh)
		template <size_t DIM>
			static void make_binary(libMesh::MeshBase& mesh, function_ptr<DIM> f_ptr)
			{
#ifdef MYDEBUG
				cout << "Sono in make_binary" << endl;
#endif //MYDEBUG

/*
				The mesh can contain elements of different dimension (i.e. volume objects and boundary objects)
				The mesh_dimension() method, if multi-dimensional meshes are supported, returns the largest dimension of mesh elements.
				For the binary tree algorithm I'm interested only in highest dimensional objects
				TODO: test this multi-dimensional case
*/
				if (mesh.mesh_dimension() != DIM)
					throw invalid_argument("In make_binary DIM parameter is different from mesh dimension!");

				for (auto iter(mesh.elements_begin()); iter != mesh.elements_end(); ++iter)
				{
					auto& el_ptr(*iter);
					auto dad = el_ptr->parent();
					int index = -1;
					if (dad)
					{
						for (int i(0); i < static_cast<int> (dad->n_children()); ++i)
							if(dad->child(i) == el_ptr)
								index = i;
					}

					auto bin_ptr = BinarityMap::binarize_node<DIM> (el_ptr, f_ptr);
/*
					since Interval constructor uses the default copy constructor of the LibmeshGeometry, I don't want to destroy the underlying libmesh object **iter; because of this I don't use the insert_elem method of the mesh, but I simply reassign the *iter address. Note that if the LibmeshGeometry class implemented the copy constructor, this instruction would cause a memory leak, since I insert the copy without freeing the memory pointed by *iter
*/
					if (el_ptr != bin_ptr)
					{
						el_ptr = bin_ptr;

						if (dad)
							dad->replace_child(el_ptr, index);

						if (el_ptr->has_children())
							for (int i(0); i < static_cast<int> (el_ptr->n_children()); ++i)
								el_ptr->child(i)->set_parent(el_ptr);

#ifdef NEIGHBOR_REPLACEMENT
						bool neighbor_exist = true;
						while (neighbor_exist)
						{
							size_t i = 0;
							auto neigh = el_ptr->neighbor(i);
							if (!neigh)
								neighbor_exist = false;
							else
								//TODO:
							++i;
						}
#endif //NEIGHBOR_REPLACEMENT
					}
				}

				//TODO (maybe, otherwise evaluate the possibility to "unbinarize" the mesh after the refinement)

				mesh.prepare_for_use(/*skip_renumber =*/ false);

#ifdef MYDEBUG
				cout << "La mesh ha " << mesh.n_elem() << " miei elementi; li conto" << endl;
				for (auto iter = mesh.elements_begin(); iter != mesh.elements_end(); ++iter)
					cout << "Elemento numero " << (*iter)->id() << endl;
#endif //MYDEBUG

				/* After the binarization I update the map */
				_boolean_map[&mesh] = true;
			};

		template <size_t DIM>
			static libMesh::Elem* binarize_node(libMesh::Elem* el_ptr, function_ptr<DIM> f_ptr)
			{
				/*to silence warning by the compiler*/
				(void) el_ptr;
				(void) f_ptr;
				/* In the default case (unexpected DIM) it throws an exception; concrete behaviour for able-to-handle DIM is defined in the specializations */
				throw invalid_argument("Cannot handle an element of dimension " + to_string(DIM));
				return nullptr;
			};

//TODO: optimize the function in order to not check for all the elements if the cast has already been done (maybe give the element you want to binarize as input parameter, setted nullptr by default, and if it is nullptr check all the elements)
		template <size_t DIM, class BinaryClass, class LibmeshClass>
			static libMesh::Elem* template_node_binarization (libMesh::Elem* el_ptr, function_ptr<DIM> f_ptr)
			{
#ifdef THROW_EXCEPTION
				Binaryclass test1;
				LibmeshClass test2;
				if (test1.type() != test2.type())
					throw logic_error("Invalid combination of template parameter inside binarize_node");
#endif //THROW_EXCEPTION

/*
				The mesh can contain elements of different dimension (i.e. volume objects and boundary objects)
				Since for the binary tree algorithm I need only the DIM objects, I binarize only these ones
				TODO: verifity if returning the element as it is, if it has not the appropriate dimensionality, could cause problems
*/
				if (el_ptr->dim() != DIM)
					return el_ptr;

				BinaryClass* result = nullptr;
/*
				I check if the element has been already casted (i.e. in adaptation, we want to transform only the children but not the father);
				WARNING : if I don't add this two lines the functions generates a segmentation fault error when calling the map initialization inside the el->init() instruction; the first line of AffineMap::init() cannot access the nodes_ptr address memory
				TODO: verify why this happens
*/
				result = dynamic_cast<BinaryClass*> (el_ptr);
				if (result)
				{
#ifdef MYDEBUG
					cout << "E' gia' binario l'indirizzo : " << el_ptr << endl;
#endif //MYDEBUG
				}
				else
				{
#ifdef MYDEBUG
					cout << "Binarizzo l'indirizzo : " << el_ptr << endl;
#endif //MYDEBUG

					bool type_recognized = false;

					LibmeshClass* casted_ptr = dynamic_cast<LibmeshClass*> (el_ptr);
					unique_ptr<LibmeshClass> smart_ptr(casted_ptr);

#ifdef MYDEBUG
					cout << "Reso smart l'indirizzo : " << smart_ptr.get() << endl;
#endif //MYDEBUG
					if (smart_ptr)
					{
						type_recognized = true;

						/*WARNING:	when I go out from the for cicle libmesh detect a memory leak due to the destruction of casted_ptr
										without deleting the underlying object; the problem is that this object is also pointed by *iter, since it is
										an element of the mesh, so it cannot be destroyed by calling delete casted_ptr*/
						result = new BinaryClass(move(smart_ptr), f_ptr);
						result->init();
#ifdef MYDEBUG
						cout << "Vediamo se il mio elemento ha il mio metodo:" << endl;
						cout << result->dim() << endl;
						cout << "Sostituisco nella mesh l'elemento di ID=" << result->id() << endl;
#endif //MYDEBUG
#ifdef MYDEBUG
						cout << "Vediamo se nella mesh ho il mio elemento:" << endl;
						auto t = el_ptr->dim();
						cout << t << endl;
						cout << "Il mio elemento e' ancora lo stesso?" << endl;
						cout << result->dim() << endl;
#endif //MYDEBUG
#ifdef MYDEBUG
						cout << "Ho un oggetto binario all'indirizzo : " << el_ptr << endl;
#endif //MYDEBUG

					}
					if(!type_recognized)
						throw invalid_argument("Cannot convert the input libMesh element into requested type!");
				}

				return result;
			};

		static BinaryNode* as_binary(libMesh::Elem* el);

/*
		A map which tells me if an object of libMesh::Meshbase has been binarized
		The compulsory way to binarize an element is by the call of BinarityMap::make_binary<DIM> (mesh_to_be_binarized, f)
*/
		static map<libMesh::MeshBase*, bool> _boolean_map;
	};

	template <>
		libMesh::Elem* BinarityMap::binarize_node<1>(libMesh::Elem* el_ptr, function_ptr<1> f_ptr);

	template <>
		libMesh::Elem* BinarityMap::binarize_node<2>(libMesh::Elem* el_ptr, function_ptr<2> f_ptr);

	template <>
		libMesh::Elem* BinarityMap::binarize_node<3>(libMesh::Elem* el_ptr, function_ptr<3> f_ptr);


	template <size_t DIM>
		class AbstractBinaryElement : public BinaryNode
		{
			public:
				AbstractBinaryElement(function_ptr<DIM> f) :	BinaryNode(),
																			_f						(f),
																			_coeff				(),
																			_projectionError	(numeric_limits<double>::max()),
																			_error_updated		(false),
																			_tilde_error		(numeric_limits<double>::max())
				{};

				virtual void init()
				{
					updateProjectionError();
					
					auto daddy = dad();
					//TODO : check the correctness
					daddy == nullptr ?	_tilde_error = _projectionError :
												//TODO : optimize it
												_tilde_error = _projectionError * daddy->projectionError()
																	/
																	(_projectionError + daddy->projectionError());
					this->_E			= _projectionError;
					this->_E_tilde	= _tilde_error;
					this->_q			= _tilde_error;
				};

				double projectionError()
				{
					if (! _error_updated)
						updateProjectionError();

					return _projectionError;
				};
				
			protected:
				void projectionError(const double& val)
				{
					_projectionError = val;
				};

				virtual double projection(const Point<DIM>& point) = 0;

			protected:
				//function whom projection error has to be computed
				function_ptr<DIM> _f;
				//coefficients of the projection
				vector<double> _coeff;

				double _projectionError;
				bool	 _error_updated;
				double _tilde_error;
		};

	template <size_t DIM, Basis::basisType FETYPE = Basis::INVALID, class LibmeshGeometry = InvalidGeometry>
		class BinTreeElement : public AbstractBinaryElement<DIM>, public FElement<DIM, FETYPE, LibmeshGeometry>
		{
			public:
				BinTreeElement(	unique_ptr<LibmeshGeometry> element_ptr,
										function_ptr<DIM> f								) :	AbstractBinaryElement<DIM> (f),
																										FElement<DIM, FETYPE, LibmeshGeometry>(move(element_ptr))
				{};

				virtual ~BinTreeElement()
				{
#ifdef DESTRUCTOR_ALERT
					cout << "Distruggo BinTreeElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual void init() override
				{
					AbstractFElement<DIM, FETYPE>::init();
					AbstractBinaryElement<DIM>::init();
				};

				virtual BinaryNode* left()
				{
					return BinarityMap::as_binary(this->child(0));
				}
				virtual BinaryNode* right()
				{
					return BinarityMap::as_binary(this->child(1));
				};
				virtual BinaryNode* dad()
				{
					return BinarityMap::as_binary(this->parent());
				};

				//TODO:	studiarla meglio, magari spostando la "binarizzazione" da un'altra parte
				//			l'ideale sarebbe in MeshRefinement::add_elem o in elem::build, ma non sono definiti virtual e quindi l'override non funzionerebbe, perche' nell'algoritmo vengono chiamati sempre i metodi delle classi base
				virtual void bisect(libMesh::MeshRefinement &mesh_refinement) override
				{
#ifdef MYDEBUG
					cout << "Sono in BinElement::refine" << endl;
#endif //MYDEBUG
					this->set_refinement_flag(libMesh::Elem::REFINE);
#ifdef MYDEBUG
					cout << "Indirizzo dopo : " << &mesh_refinement << endl;
					cout << "Tipo mio : " << this->type() << endl;
					cout << "Tipo libmesh : " << LibmeshGeometry::type() << endl;
#endif //MYDEBUG

					libMesh::Elem::refine(mesh_refinement);
#ifdef MYDEBUG
					cout << "Effettuata la Elem::refine" << endl;
					cout << "L'indirizzo del primo figlio e' : " << this->child(0) << endl;
					cout << "L'indirizzo del secondo figlio e' : " << this->child(1) << endl;
#endif //MYDEBUG

					//TODO: optimize this step, it recomputes all the mesh elements, not only the two new children
					BinarityMap::make_binary<DIM>(mesh_refinement.get_mesh(), this->_f);
#ifdef MYDEBUG
					cout << "Binarizzati i nuovi elementi" << endl;
					cout << "L'indirizzo del primo figlio e' : " << this->child(0) << endl;
					cout << "L'indirizzo del secondo figlio e' : " << this->child(1) << endl;
					cout << "Visti come BinaryNode*" << endl;
					cout << "L'indirizzo del primo figlio e' : " << left() << endl;
					cout << "L'indirizzo del secondo figlio e' : " << right() << endl;
#endif //MYDEBUG
				};

				virtual void printCoefficients()
				{
					computeCoefficients();
					for (auto& c : this->_coeff)
						cout << c << "	";
					cout << endl;
				};

				virtual void	pLevel(size_t val)
				{
					AbstractFElement<DIM, FETYPE>::pLevel(val);
					this->_error_updated = false;
				};
				virtual size_t pLevel() const
				{
					return AbstractFElement<DIM, FETYPE>::pLevel();
				};

			protected:
				virtual void updateProjectionError() override
				{
					//TODO: let the error norm to be modifiable, maybe to be choosable at runtime 
/*
					L2 norm of the interpolation error
*/
					double err =  sqrt(	this->integrate	(
																			[&](const Point<DIM>& p){
																												double val = (*(this->_f))(p) - projection(p);
																												return val*val;
																											}
																		)
											);

					this->projectionError(err);
					this->_error_updated = true;
				};

				virtual double projection(const Point<DIM>& point) override
				{
					computeCoefficients();
					double result(0);
//					vector<double> basisEvaluation = this->evaluateBasis(point);

					//TODO: optimizable: use eigen dot product
					for (size_t i(0); i < this->_coeff.size(); ++i)
					{
						result += this->_coeff[i] * this->evaluateBasisFunction(i, point);
					}
					return result;
				};

			protected:
#ifdef ENABLE_IF_VERSION
				template <	Basis::basisType DUMMY = FETYPE,
								typename enable_if<	DUMMY == Basis::LEGENDRE,
															size_t
														>::type = 0
							>
					void computeCoefficients()
					{
						
						//I remember how many coefficients have been already computed
						size_t cursor = this->_coeff.size();
						//New number of coefficients
						size_t s = this->basisSize();
						if (cursor == s)
							//Coefficients already computed
							return;
						this->_coeff.resize(s);

						//I don't recompute already stored coefficients
						for(; cursor < s; ++cursor)
						{
							this->_coeff[cursor] = this->L2prod	(
																				*(this->_f),
																				[&](const Point<DIM>& p)	{
																														return this->evaluateBasisFunction(cursor, p);
																													}
																			);
						}
					};

				template <	Basis::basisType DUMMY = FETYPE,
								typename enable_if<	DUMMY == Basis::INVALID,
															size_t
														>::type = 0
							>
					void computeCoefficients()
					{
#ifdef MYDEBUG
						cout << "Trying to project function on INVALID finite element space!!!" << endl;
#endif //MYDEBUG
						throw bad_typeid();
					};

				//TODO: verify if there's a way to define a default case
				template <	Basis::basisType DUMMY = FETYPE,
								typename enable_if<	DUMMY != Basis::LEGENDRE && DUMMY != Basis::INVALID,
															size_t
														>::type = 0
							>
#endif //ENABLE_IF_VERSION
					void computeCoefficients()
					{
						throw invalid_argument("I don't know how to project function on this finite element space!!!");
					};
		};

#ifndef ENABLE_IF_VERSION
	template <size_t DIM, class LibmeshGeometry = libMesh::Edge2>
			void BinTreeElement<DIM, Basis::LEGENDRE, LibmeshGeometry>::computeCoefficients()
			{
				
				//I remember how many coefficients have been already computed
				size_t cursor = _coeff.size();
				//New number of coefficients
				size_t s = this->basisSize();
				if (cursor == s)
					//Coefficients already computed
					return;
				_coeff.resize(s);

				//I don't recompute already stored coefficients
				for(; cursor < s; ++cursor)
				{
					_coeff[cursor] = this->L2prod	(
																*_f,
																[&](const Point<DIM>& p)	{
																										return this->evaluateBasisFunction(cursor, p);
																									}
															);
				}
			};

	template <size_t DIM, class LibmeshGeometry = libMesh::Edge2>
			void BinTreeElement<DIM, Basis::INVALID, LibmeshGeometry>::computeCoefficients()
			{
#ifdef MYDEBUG
				cout << "Trying to project function on INVALID finite element space!!!" << endl;
#endif //MYDEBUG
				throw bad_typeid();
			};

#endif //ENABLE_IF_VERSION

	//TODO: use GetPot to select FElement basisType at runtime	
	using Interval = BinTreeElement<1, Basis::LEGENDRE, LibmeshInterval>;

	//It must be modified the way libMesh elements divides theirselves
	//The info about the procreation should be stored in the elem.embedding_matrix() method
	class Triangle : public BinTreeElement<2, Basis::LEGENDRE, LibmeshTriangle>
	{
		public:
			Triangle(unique_ptr<LibmeshTriangle> el_ptr, function_ptr<2> f) : BinTreeElement(move(el_ptr), f){};
			virtual ~Triangle(){};

			virtual float	embedding_matrix (const unsigned int child_num,
														const unsigned int child_node_num,
														const unsigned int parent_node_num) const override
			{
				//TODO: override the method to make it bisective
				(void) child_num;
				(void) child_node_num;
				(void) parent_node_num;
				return 0.0;
			};
	};

}; //namespace BinaryTree

#endif //__BINARY_ELEMENT_H
