#ifndef __PROXY_H
#define __PROXY_H

#include <string>
#include <memory>
#include <iostream>
#include <type_traits> //is_same, enable_if

#include "AbstractFactory.h"

//TODO: remove the enable_if syntax if possible

using namespace std;
namespace GenericFactory
{
	/*! A simple proxy for registering into a factory.

	It provides the builder as static method 
	and the automatic registration mechanism.

	\param Factory The type of the factory.  
	\param ConcreteProduct is the derived (concrete) type to be 
	registered in the factory

	*/

	template <typename ConcreteProduct, typename Factory>
		class Proxy
		{
			public:
				using AbstractProduct_type	= typename Factory::AbstractProduct_type;
				using Identifier_type		= typename Factory::Identifier_type;
				using Return_type				= typename Factory::Return_type;
				using Factory_type			= Factory;

				//! The constructor does the registration.
				Proxy(Identifier_type const & name)
				{
					//get the factory. First time creates it.
					Factory_type& factory(Factory_type::Instance());
					// Insert the builder
					factory.add(name, Proxy<ConcreteProduct, Factory>::Build); //does not work
#ifdef MYDEBUG
					cout << "Added " << name << " to factory" << endl;
#endif //MYDEBUG
				};

				virtual ~Proxy() = default;

				//TODO: verify if there's a way to define a default case
				template <	typename DUMMY = Return_type,
								typename enable_if < !is_same<DUMMY, unique_ptr<AbstractProduct_type>>::value
															&&
															!is_same<DUMMY, shared_ptr<AbstractProduct_type>>::value,
															size_t
														 >::type = 0
							>
					static Return_type Build()
					{
						throw runtime_error("Builder unknown for this return type");
						return Return_type();
					};

				template <	typename DUMMY = Return_type,
								typename enable_if < is_same<DUMMY, unique_ptr<AbstractProduct_type>>::value,
															size_t
														 >::type = 0
							>
					static unique_ptr<AbstractProduct_type> Build()
					{
						return make_unique<ConcreteProduct>();
					};

				template <	typename DUMMY = Return_type,
								typename enable_if < is_same<DUMMY, shared_ptr<AbstractProduct_type>>::value,
															size_t
														 >::type = 0
							>
					static shared_ptr<AbstractProduct_type> Build()
					{
	#ifndef SINGLETON_ENABLED
						static shared_ptr<AbstractProduct_type> ptr = make_shared<ConcreteProduct>();
	#else 
						shared_ptr<AbstractProduct_type> ptr(ConcreteProduct::Instance());
	#endif //SINGLETON_ENABLED
						return ptr;
					};

			private:
				Proxy					(Proxy const &) = delete;
				Proxy& operator =	(Proxy const &) = delete;
		};


//	template <typename ConcreteProduct, typename Factory>
//		class ObjectProxy
//		{
//			public:
//				using AbstractProduct_type	= typename Factory::AbstractProduct_type;
//				using Identifier_type		= typename Factory::Identifier_type;
//				using Return_type				= typename Factory::Return_type;
//				using Factory_type			= Factory;

//				//! The constructor does the registration.
//				ObjectProxy(Identifier_type const & name)
//				{
//					//get the factory. First time creates it.
//					Factory_type& factory(Factory_type::Instance());
//					// Insert the builder
//					factory.add(name, &Proxy::Build);
//					cout << "Added " << name << " to factory"<<endl;
//				};

				//TODO: add a control on the return type
//				static unique_ptr<AbstractProduct_type> Build()
//				{
//					return make_unique<ConcreteProduct>();
//				};

//			private:
//				ObjectProxy					(Proxy const &) = delete;
//				ObjectProxy& operator =	(Proxy const &) = delete;
//		};

//	template <typename ConcreteProduct, typename Factory>
//		class SingletonProxy
//		{
//			public:
//				using AbstractProduct_type	= typename Factory::AbstractProduct_type;
//				using Identifier_type		= typename Factory::Identifier_type;
//				using Return_type				= typename Factory::Return_type;
//				using Factory_type			= Factory;

//				//! The constructor does the registration.
//				SingletonProxy(Identifier_type const & name)
//				{
//					//get the factory. First time creates it.
//					Factory_type& factory(Factory_type::Instance());
//					// Insert the builder
//					factory.add(name,&Proxy::Build);
//					cout << "Added "<< name << " to factory" << endl;
//				};

//				static shared_ptr<AbstractProduct_type> Build()
//				{
//#ifdef NOT_SINGLETON
//					static shared_ptr<AbstractProduct_type> ptr = make_shared<ConcreteProduct>();
//					return ptr;
//#endif //NOT_SINGLETON
//					return shared_ptr<AbstractProduct_type>(&ConcreteProduct::Instance());
//				};

//			private:
//				SingletonProxy					(SingletonProxy const &) = delete;
//				SingletonProxy& operator =	(SingletonProxy const &) = delete;
//		};


}//namespace GenericFactory


#endif //__PROXY_H
