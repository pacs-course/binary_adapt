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

namespace GenericFactory
{
	using namespace std;

	/**
		Generic utility to convert identifiers to string (if possible).
		Use type traits to identify the correct version.
	**/
	template<bool Convertible, typename Identifier>
	struct M_identifierAsString
	{
		static string value (Identifier const& id);
	};

	/**
		Partial specialization if not convertible
	**/
	template<typename Identifier>
	struct M_identifierAsString<false, Identifier>
	{
		static string value (Identifier const&)
		{
			return string ("CANNOT RESOLVE NAME");
		}
	};

	/**
		Partial specialization if convertible
	**/
	template<typename Identifier>
	struct M_identifierAsString<true, Identifier>
	{
		static string value (Identifier const& id)
		{
			return string (id);
		}
	};

	/**
		Utility to convert identifiers to string (if possible)
	**/
	template<typename Identifier>
	string identifierAsString (Identifier const& id)
	{
		return M_identifierAsString <is_convertible<Identifier, string>::value,
									 Identifier>::value (id);
	}

	/**
		Full specialization for geometric types enum
	**/
	template<>
	string identifierAsString<Geometry::ElementType> (Geometry::ElementType const&);
	/**
		Full specialization for basis types enum
	**/
	template<>
	string identifierAsString<FiniteElements::BasisType> (FiniteElements::BasisType
														  const&);

	/**
		A base class for Factory class.
		Needed by InstanceHolder class, it's the return type of a factory instance.
	**/
	class FactoryBase
	{
	  public:
		virtual ~FactoryBase()
		{};
	};

	/**
		Factory instances handler.
		It can be seen as a simplified factory of factories, with string key.
		It stores the instances of the factories,
		Factory::Instance() refers to this class to get its instance.
	**/
	class InstanceHolder
	{
	  public:
		/**
			Get instance. It throws exception if instance is not present in the map.
		**/
		static FactoryBase& FactoryInstance (const string&);
		/**
			Add instance to the map.
		**/
		static FactoryBase& AddInstance (const string&, unique_ptr<FactoryBase>);

	  private:
		/**
			map of factories instances, identified by string
		**/
		static map<string, unique_ptr<FactoryBase>> _holder;
	};


	/**
		A generic factory.

		It is implemented as a Singleton.
		The compulsory way to access a method is Factory::Instance().method().
		Typycally to access the factory one does
		\code
		auto& myFactory = Factory<A,I,B>::Instance();
		myFactory.add(...)
		\endcode
	**/
	template	<	typename AbstractProduct,
					typename Identifier,
					typename ReturnType
					>
	class Factory : public FactoryBase
	{
	  public:
		/** The container for the rules. **/
		using AbstractProduct_type = AbstractProduct;

		/** The identifier. **/
		using Identifier_type = Identifier;

		/** The builder type. **/
		using Builder_type = function<ReturnType()>;

		/** The return type. **/
		using Return_type = ReturnType;

		/**
			string identifier of the factory
		**/
		static const string Name()
		{
			string s1 = typeid (AbstractProduct).name();
			string s2 = typeid (Identifier).name();
			string s3 = typeid (ReturnType).name();
			return s1 + "#" + s2 + "#" + s3;
		}

		/**
			Get the instance of the factory
		**/
		static Factory& Instance()
		{
			string factory_name = Name();
			try
			{
				return dynamic_cast<Factory&> (InstanceHolder::FactoryInstance (factory_name));
			}
			catch (out_of_range&)
			{
				return dynamic_cast<Factory&> (InstanceHolder::AddInstance (factory_name,
																			move (unique_ptr<FactoryBase> (new Factory))));
			}
		};

		/**
			Get the rule with given name
			If ReturnType is a pointer, it is null if no rule is present.
		**/
		//TODO: use variadic templates in order to allow to pass parameters to the object creation rule
		Return_type create (Identifier const& name) const
		{
			auto f = _storage.find (name);
			if (f == _storage.end())
			{
				throw invalid_argument
				("Identifier " + identifierAsString (name)
							   + " is not stored in the factory");
			}
			else
			{
				return Return_type (f->second());
			}
		};

		/**
			Register the given rule.
			If a rule is already stored for the same key, it is overwritten.
		**/
		void add (Identifier const& name, Builder_type const& func)
		{
			auto f = this->_storage.insert (make_pair (name, func));
			if (f.second == false)
			{
				(this->_storage)[name] = func;
#ifdef VERBOSE
				clog << "Builder for key = "
					 << identifierAsString (name)
					 << " already present; previous one overwritten"
					 << endl;
#endif //VERBOSE
			}
		};

		/**
			A list of registered rules.
		**/
		vector<Identifier> registered() const
		{
			vector<Identifier> tmp;
			tmp.reserve (_storage.size());

			for (auto i = _storage.begin(); i != _storage.end(); ++i)
				tmp.push_back (i->first);

			return tmp;
		};
		/**
			Unregister a rule.
		**/
		void unregister (Identifier const& name)
		{
			_storage.erase (name);
		};

		/**
			default destructor
		**/
		virtual ~Factory()
		{};

	  private:
		/**
			constructor.
			Made private since it is a Singleton.
		**/
		Factory() {};
		/**
			copy constructor.
			Deleted since it is a Singleton.
		**/
		Factory (Factory const&) = delete;
		/**
			assignement operator.
			Deleted since it is a Singleton.
		**/
		Factory& operator = (Factory const&) = delete;

	  protected:
		using Container_type = map<Identifier, Builder_type>;
		/**
			Rules storage
		**/
		Container_type _storage;
	};


	/**
		Factory of objects.
		It returns a unique_ptr to the created object.
	**/
	template <typename AbstractProduct,
			  typename Identifier>
	using ObjectFactory =
		Factory<AbstractProduct, Identifier, unique_ptr<AbstractProduct>>;

	/**
		Factory of singletons.
		Factory for objects with no more than one instance wanted in the program.
		The builder shall ensure that two or more different instances are created.
		It is returned a shared_ptr to the instance.
	**/
	template <typename AbstractProduct,
			  typename Identifier>
	using SingletonFactory =
		Factory<AbstractProduct, Identifier, shared_ptr<AbstractProduct>>;

} //namespace GenericFactory


#endif //__FACTORY_H
