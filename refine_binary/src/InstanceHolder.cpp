#include "AbstractFactory.h"

#include <map>

using namespace std;

namespace GenericFactory
{
	//default initialization of the static attribute _holder; after the construction the map is empty
#ifndef BANANA
	map<string, unique_ptr<FactoryBase>> InstanceHolder::_holder;
#else //BANANA
	BananaMap InstanceHolder::_holder;
#endif //BANANA

	FactoryBase& InstanceHolder::FactoryInstance(const string& factory_name)
	{
#ifdef MYDEBUG
		cerr << "querying Factory " << factory_name << endl;
#endif //MYDEBUG
		return *(_holder.at(factory_name));
	};

	FactoryBase& InstanceHolder::AddInstance(const string& factory_name, unique_ptr<FactoryBase> instance)
	{
#ifdef MYDEBUG
		cerr << "registering Factory " << factory_name << " [" << instance.get() << "]" << endl;
#endif //MYDEBUG
		auto& ptr = _holder[factory_name];
		ptr = move(instance);
		return *(ptr);
	};
} //namespace GenericFactory
