

#ifndef _mrcInitializer_h_
#define _mrcInitializer_h_

#include "../mrcConfig.h"
#include "../mrcAssert.h"
#include <typeinfo>
#include <ctype.h>

namespace mrc
{	
	// 상속 금지 클래스
	template<class T>
	class cNonDerivable
	{
		friend T;
	private:
		cNonDerivable() {}
		cNonDerivable(const cNonDerivable&) {}
	};

	// 사용법
// 	class B : public virtual cNonDerivable<B>
// 	{
// 
// 	};



#ifdef _MSC_VER
	inline NString getClassName(const char* className)
	{
		const char* begin_it = className;
		const char* end_it =  className + strlen(begin_it);

		mrcASSERT(begin_it != end_it);
		const char* rit = end_it - 1;
		const char* rend_it = begin_it - 1;

		for ( ;rit != rend_it; --rit)
		{
// 			if( *rit == '>' )
// 			{
// 				end_it = rit;
// 				continue;
// 			}
			mrcASSERT( *rit != '>' );

			if( isalnum(*rit) == 0 )
				return NString(++rit,end_it);
		}

		mrcASSERT(false);
		return NString();
	}
#else
#error make NString getFactoryClassName(const T& instance)
#endif

	template<class T>
	inline NString getClassName_NoOriginal(const T& t) { return getClassName(typeid(T).name()); }
	template<class T>
	inline NString getClassName_NoOriginal(const mrcAutoPtr<T>& t) { return getClassName(typeid(T).name()); }
	template<class T>
	inline NString getClassName_NoOriginal(const mrcSharedPtr<T>& t) { return getClassName(typeid(T).name()); }


	template<class T>
	inline NString getClassName(const T& t) { return getClassName(typeid(t).name()); }

	template<class T>
	inline NString getClassName(const mrcAutoPtr<T>& t) { return getClassName(typeid(*t).name()); }
	template<class T>
	inline NString getClassName(const mrcSharedPtr<T>& t) { return getClassName(typeid(*t).name()); }


	//////////////////////////////////////////////////////////////////////////

	class IPolymorphBaseObject
	{
	public:
		IPolymorphBaseObject() {}

		virtual ~IPolymorphBaseObject() {}
	};

	//////////////////////////////////////////////////////////////////////////

	class PolyporphBaseObjectFactory 
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(PolyporphBaseObjectFactory);
//		MIRACLE_FRIEND_CLASS_SINGLETON;
	

		PolyporphBaseObjectFactory() {}
	public:
		static PolyporphBaseObjectFactory& getInstance();

		typedef mrcAutoPtr<IPolymorphBaseObject> (*funcNewBaseObject)(); 
		typedef map<NString,funcNewBaseObject> FactoryMap;

		template<class T>
		mrcAutoPtr<T> createObject(const NString& key) const
		{
			FactoryMap::const_iterator it = mFactoryMap.find(key);
			mrcASSERT_MSG(it != mFactoryMap.end(), "not exist key : " + key);
			
			mrcAutoPtr<IPolymorphBaseObject> apObj = it->second();
			mrcASSERT(apObj.get());
			// 아래 코드는 
			static_cast<T*>(apObj.get());
			T* pChange = dynamic_cast<T*>(apObj.release());
			mrcASSERT(pChange);
			return mrcAutoPtr<T>(pChange);
		}

		template<class T>
		bool registerNewFunction()
		{
			NString key = getClassName<T>();

			std::pair<FactoryMap::iterator,bool> ret = mFactoryMap.insert( std::make_pair(key,&createNewFunction<T>) );
			mrcASSERT_MSG(ret.second,"overloaded key");
			return true;
		}

		template<class T>
		NString getRegisterClassName() 
		{
			NString key = getClassName<T>();
			FactoryMap::iterator it = mFactoryMap.find(key);
			if( it == mFactoryMap.end() )
				return NString();
			
			return key;
		}

		bool isRegisterClassName(const NString& className)
		{
			FactoryMap::iterator it = mFactoryMap.find(className);
			return it != mFactoryMap.end();
		}

	private:
		template<class T>
		inline NString getClassName() { return mrc::getClassName( typeid(T).name() ); }

		template<class T>
		static mrcAutoPtr<IPolymorphBaseObject> createNewFunction() 
		{
			return mrcAutoPtr<IPolymorphBaseObject>( mrcNEW T ); 
		}

	protected:
		FactoryMap mFactoryMap;
	};

#define MIRACLE_REGISTER_POLYMORPH(TYPE) bool ___ ## TYPE ## _ret = mrc::PolyporphBaseObjectFactory::getInstance().registerNewFunction<TYPE>()

}



#endif