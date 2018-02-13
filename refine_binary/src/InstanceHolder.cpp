#include "AbstractFactory.h"

#include <map>

using namespace std;

namespace GenericFactory
{
	/*	default initialization of the static attribute _holder; after the construction the map is empty */
	map<string, unique_ptr<FactoryBase>> InstanceHolder::_holder;

	FactoryBase& InstanceHolder::FactoryInstance (const string& factory_name)
	{
		return * (_holder.at (factory_name));
	}

	FactoryBase& InstanceHolder::AddInstance (const string& factory_name,
											  unique_ptr<FactoryBase> instance)
	{
		auto& ptr = _holder[factory_name];
		ptr = move (instance);
		return * (ptr);
	}
} //namespace GenericFactory
