#ifndef __FACTORY_H
#define __FACTORY_H

#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <type_traits>

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
			static string value (Identifier const & id)
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
// this does not work!
//	template<typename Identifier>
//		using identifierAsString(Identifier const &) =
//			M_identifierAsString<is_convertible<Identifier, string>::value, Identifier> (Identifier const &);


	/*! @brief A generic factory.
	 
	 It is implemented as a Singleton. The compulsory way to 
	 access a method is Factory::Instance().method().
	 Typycally to access the factory one does
	 \code
	 auto&  myFactory = Factory<A,I,B>::Instance();
	 myFactory.add(...)
	 \endcode
	*/

	template	<	typename AbstractProduct,
	 				typename Identifier,
				   typename ReturnType
				>
		class Factory
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

				static Factory& Instance()
				{
					static Factory f;
					return f;
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
						string out="Identifier " + identifierAsString(name) + " is not stored in the factory";
						throw invalid_argument(out);
					}
					else
					{
						return Return_type(f->second());
					}
				};

				//! Register the given rule.
				void add(Identifier const & name, Builder_type const & func)
				{
					auto f = _storage.insert(make_pair(name, func));
				 	if (f.second == false)
						throw invalid_argument("Double registration in Factory");
				};

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
				virtual ~Factory() = default;

			protected:
				using Container_type = map<Identifier,Builder_type>;

				//! Made private since it is a Singleton
				Factory() = default;

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
		using ObjectFactory		= Factory<AbstractProduct, Identifier, unique_ptr<AbstractProduct>>;

	template	<	typename AbstractProduct,
	 				typename Identifier
				>
		using SingletonFactory	= Factory<AbstractProduct, Identifier, shared_ptr<AbstractProduct>>;

}// end namespace


#endif //__FACTORY_H
