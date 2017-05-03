#include "AbstractFactory.h"

#include <map>


namespace GenericFactory
{
	class InstanceHolder : public map<string, FactoryBase*>
	{
		public:
			~InstanceHolder()
			{
				for (auto inst : (*this))
					delete inst.second;
	    	};
    };

	FactoryBase* FactoryInstance(const string& factory_name, FactoryBase* instance)
	{
		static InstanceHolder instances;
		if (instance)
			instances[factory_name] = instance;

		return instances[factory_name];
	}
} //namespace GenericFactory
