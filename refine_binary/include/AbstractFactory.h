#ifndef __FACTORY_H
#define __FACTORY_H

#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

#include "BinaryTreeHelper.h"

using namespace std;
namespace GenericFactory
{
	//! Generic utility to convert identifiers to string (if possible)
	/*! 
		Use type traits to identify the correct version
	*/
	template<bool Convertible, typename Identifier>
		struct
		M_identifierAsString
		{
			static string value (Identifier const & id);
		};

	//! Partial specialization if not convertible
	template<typename Identifier>
		struct
		M_identifierAsString<false,Identifier>
		{
			static string value (Identifier const &)
			{
				return string("CANNOT RESOLVE NAME");
			}
		};

	//! Partial specialization if convertible
	template<typename Identifier> 
		struct
		M_identifierAsString<true,Identifier>
		{
			static string value (Identifier const & id)
			{
				return string(id);
		 	}
		};

	//! Utility to convert identifiers to string (if possible)
	template<typename Identifier>
		string identifierAsString(Identifier const & id)
		{
			return M_identifierAsString<is_convertible<Identifier, string>::value,Identifier>::value(id);
		};

	//! Full specialization for my enums
	template<>
		string identifierAsString<Geometry::ElementType>(Geometry::ElementType const &);
	template<>
		string identifierAsString<FiniteElements::BasisType>(FiniteElements::BasisType const &);

	/*! @brief A generic factory.
	 
	 It is implemented as a Singleton. The compulsory way to 
	 access a method is Factory::Instance().method().
	 Typycally to access the factory one does
	 \code
	 auto&  myFactory = Factory<A,I,B>::Instance();
	 myFactory.add(...)
	 \endcode
	*/

	class FactoryBase
	{
		public:
		virtual ~FactoryBase()
		{
#ifdef DESTRUCTOR_ALERT
			cerr << "Destroying FactoryBase [" << this << "]" << endl;
#endif //DESTRUCTOR_ALERT
		};
#ifdef BANANA
		virtual void BananaCall() const
		{
			cerr << "qualcosa non va! chiamata la BananaCall() della classe FactoryBase" << endl;
		}
	};

	class BananaMap : public map<string, unique_ptr<FactoryBase>>
	{
		public:
		~BananaMap()
		{
			cerr << "BananaMap: destroying " << size() << " registered factories" << endl;
			for (auto f = begin(); f != end(); ++f)
			{
				 cerr << "BananaMap: accessing [" << f->second.get() << "]" << endl;
				 f->second->BananaCall();
			}
		}
#endif //BANANA
	};

	class InstanceHolder
	{
		public:
			static FactoryBase& FactoryInstance(const string&);
			static FactoryBase& AddInstance(const string&, unique_ptr<FactoryBase>);
		private:
#ifndef BANANA
			static map<string, unique_ptr<FactoryBase>> _holder;
#else //BANANA
			static BananaMap _holder;
#endif //BANANA
	};

	template	<	typename AbstractProduct,
	 				typename Identifier,
				   typename ReturnType
				>
		class Factory : public FactoryBase
		{
			public:
				//! The container for the rules.
				using AbstractProduct_type	= AbstractProduct;
				//! The identifier.
				using Identifier_type		= Identifier;
				//! The builder type.
				using Builder_type			= function<ReturnType()>;
				//! The return type.
				using Return_type				= ReturnType;

				static const string Name()
				{
				    string s1 = typeid(AbstractProduct).name();
				    string s2 = typeid(Identifier).name();
				    string s3 = typeid(ReturnType).name();
				    return s1 + "#" + s2 + "#" + s3;
				}

				static Factory& Instance()
				{
					string factory_name = Name();
					try
					{
						return dynamic_cast<Factory&>(InstanceHolder::FactoryInstance(factory_name));
					}
					catch(out_of_range&)
					{
						return dynamic_cast<Factory&>(InstanceHolder::AddInstance(factory_name, move(unique_ptr<FactoryBase>(new Factory))));
					}
				};

				//! Get the rule with given name
				/*!
				If ReturnType is a pointer, it is null if no rule is present.
				*/
				//TODO: use variadic templates in order to allow to pass parameters to the object creation rule
				Return_type create(Identifier const & name) const
				{
					auto f = _storage.find(name);
					if (f == _storage.end())
					{
						throw invalid_argument("Identifier " + identifierAsString(name) + " is not stored in the factory");
					}
					else
					{
						return Return_type(f->second());
					}
				};

				//! Register the given rule.
				void add(Identifier const & name, Builder_type const & func)
				{
					auto f = this->_storage.insert(make_pair(name, func));
				 	if (f.second == false)
					{
						(this->_storage)[name] = func;
#ifdef VERBOSE
						clog << "Builder for key = " << identifierAsString(name) << " already present; previous one overwritten" << endl;
#endif //VERBOSE
					}
				};
#ifdef BANANA
				virtual void BananaCall() const override
				{
				    cerr << "chiamata la BananaCall() della classe derivata [" << this << "]" << endl;
				}
#endif //BANANA
				//! Returns a list of registered rules.
				vector<Identifier> registered() const
				{
					vector<Identifier> tmp;
					tmp.reserve(_storage.size());

					for(auto i = _storage.begin(); i != _storage.end(); ++i)
						tmp.push_back(i->first);

					return tmp;
				};

				//! Unregister a rule.
				void unregister(Identifier const & name)
				{
					_storage.erase(name);
				};

				//! Destructor
				virtual ~Factory()
				{
#ifdef DESTRUCTOR_ALERT
					cerr << "Destroying Factory [" << this << "]" << endl;
#endif //DESTRUCTOR_ALERT
				};

			protected:
				using Container_type = map<Identifier,Builder_type>;

				//! Made private since it is a Singleton
				Factory() {};

				//! Deleted since it is a Singleton
				Factory					(Factory const&) = delete;
				//! Deleted since it is a Singleton
				Factory& operator =	(Factory const&) = delete;

				//! It contains the actual object factory.
				Container_type _storage;
		};


	template	<	typename AbstractProduct,
	 				typename Identifier
				>
		using ObjectFactory = Factory<AbstractProduct, Identifier, unique_ptr<AbstractProduct>>;

	template	<	typename AbstractProduct,
	 				typename Identifier
				>
		using SingletonFactory = Factory<AbstractProduct, Identifier, shared_ptr<AbstractProduct>>;

} //namespace GenericFactory


#endif //__FACTORY_H
