
#include "mrcPrecompiled.h"
#include "mrcUtil/mrcPolymorphFactory.h"
#include "mrcUtil/mrcSingleton.h"



namespace mrc
{
	PolyporphBaseObjectFactory& PolyporphBaseObjectFactory::getInstance()
	{
		typedef Loki::SingletonHolder<PolyporphBaseObjectFactory,Loki::CreateUsingNew,
			Loki::LongevityLifetime::DieIndependencyMgr > PolyporphBaseObjectFactoryS;

		return PolyporphBaseObjectFactoryS::Instance();
	}
}
