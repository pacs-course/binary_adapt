#include "AbstractFactory.h"

#include <map>

#include "Functor.h"

namespace GenericFactory
{
	class InstanceHolder : public map<string, std::unique_ptr<FactoryBase>>
	{
		public:
			~InstanceHolder()
			{
//#ifdef MYDEBUG
//				cerr << "Destroying InstanceHolder" << endl;
//				for (auto& pair : (*this))
//				{
//					cerr << "I have a pointer with address: " << pair.second.get() << endl;
//				 	cerr << "With key: " << pair.first << endl;

//					cerr << "Il tipo e' " << typeid(pair.second.get()).name() << endl;
//					auto ptr = dynamic_cast<BinaryTree::FunctionsFactory<1>*>(pair.second.get());
//					if(ptr)
//					{
//						cerr << "I try to manually destroy it" << endl;
//						ptr->~FactoryBase();
//					}
//					else
//						cerr <<"il cast non ha funzionato" << endl;

					//SU QUESTA ISTRUZIONE HO UN SEGMENTATION FAULT!!!
//					pair.second->~FactoryBase();
					//questa e' una possibile soluzione, ma causo memory leak
//					pair.second.release();
//				}
//#endif //MYDEBUG
	    	};
    };

	FactoryBase* FactoryInstance(const string& factory_name, FactoryBase* instance)
	{
		static InstanceHolder instances;
		if (instance)
		{
			instances[factory_name] = std::move(std::unique_ptr<FactoryBase>(instance));
#ifdef MYDEBUG
			cerr << "Aggiunta istanza con nome: " << factory_name << endl;
#endif //MYDEBUG
		}
		return instances[factory_name].get();
	}
} //namespace GenericFactory
