

#ifndef _mrcFactory_h_
#define _mrcFactory_h_

#include "mrcConfig.h"
#include "mrcStdContainer.h"
#include <typeinfo.h>

namespace mrc
{
	
	class FactoryNull{};

	template<class TYPE>
	class Factory 
	{
	public:
		typedef ptr_size KEY;
	protected:
		Factory() {}
	public:
		typedef mrcAutoPtr<TYPE> (*funcCreateNewFunction)(); 
		typedef map<KEY,funcCreateNewFunction> FactoryMap;

		//////////////////////////////////////////////////////////////////////////
		template<class IMPL>
		mrcAutoPtr<TYPE> createObject() const
		{
			if( typeid(FactoryNull) == typeid(IMPL) )
				return mrcAutoPtr<TYPE>();

			FactoryMap::const_iterator it = mFactoryMap.find(reinterpret_cast<KEY>(typeid(IMPL).name()));
			mrcASSERT_MSG(it != mFactoryMap.end(), NString("not exist key : ") + typeid(IMPL).name());
			return it->second();
		}

		template<class IMPL>
		bool registerCreator()
		{
			std::pair<FactoryMap::iterator,bool> ret = mFactoryMap.insert( 
				std::make_pair(reinterpret_cast<KEY>(typeid(IMPL).name()),&Factory<TYPE>::createNewFunction<IMPL>) );
			mrcASSERT_MSG(ret.second,NString("overloaded key : ") + typeid(IMPL).name());
			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		template<class IMPL>
		mrcAutoPtr<TYPE> createObject(const IMPL& impl) const
		{
			if( typeid(FactoryNull) == typeid(IMPL) )
				return mrcAutoPtr<TYPE>();

			KEY key = reinterpret_cast<KEY>(typeid(impl).name());
			FactoryMap::const_iterator it = mFactoryMap.find(key);
			mrcASSERT_MSG(it != mFactoryMap.end(), NString("not exist key : ") + typeid(impl).name());
			return it->second();
		}

		template<class IMPL,class TYPE_KEY>
		bool registerCreator()
		{
			std::pair<FactoryMap::iterator,bool> ret = mFactoryMap.insert( 
				std::make_pair(reinterpret_cast<KEY>(typeid(TYPE_KEY).name()),&Factory<TYPE>::createNewFunction<IMPL>) );
			mrcASSERT_MSG(ret.second,NString("overloaded key : ") + typeid(TYPE_KEY).name());
			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

	private:
		template<class IMPL>
		static mrcAutoPtr<TYPE> createNewFunction() { return mrcAutoPtr<TYPE>(mrcNEW IMPL); }

	private:
		FactoryMap mFactoryMap;
	};

#define MIRACLE_FRIEND_FACTORY template<class TYPE> friend class Factory


#define MIRACLE_REGISTER_FACTORY( FACTORY_TYPE , ... ) \
	bool MIRACLE_MAKE_TEMP_NAME(FACTORY_TYPE) = FACTORY_TYPE::getInstance().registerCreator<__VA_ARGS__>()
}




#endif