
#ifndef _mrcSerialization_h_
#define _mrcSerialization_h_

#ifdef _MSC_VER
#pragma warning( disable : 4308 )
#endif

#include "mrcStdContainer.h"
#include "mrcString.h"

#include "mrcSerialization/mrcSerializationOrderedVector.h"
#include "mrcSerialization/mrcSerializationVector.h"
#include "mrcSerialization/mrcSerializationUnorderedMap.h"
#include "mrcSerialization/mrcSerializationDeque.h"
#include "mrcSerialization/mrcSerializationMap.h"
#include "mrcSerialization/mrcSerializationSet.h"

#include "mrcSerialization/mrcSerializationAutoPtr.h"
#include <boost/serialization/shared_ptr.hpp>


//#include <boost/serialization/string.hpp>

#include <boost/serialization/level.hpp>

BOOST_CLASS_IMPLEMENTATION(mrc::NString, boost::serialization::primitive_type)
BOOST_CLASS_IMPLEMENTATION(mrc::WString, boost::serialization::primitive_type)



#include <boost/serialization/nvp.hpp>
namespace mrc
{

	template<class SERIALIZATION,class T>
	inline
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
		const
#endif
		void make_nvp(SERIALIZATION& serial,const char * name, T & t)
	{
		serial & boost::serialization::make_nvp(name, t);
	}


	template<class SERIALIZATION>
	inline
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
	const
#endif
		void make_nvp(SERIALIZATION& serial,const char * name,WString& t)
	{
		NString temp = toNString(t);
		make_nvp(serial,name,temp);
	}
}


#include <boost/serialization/utility.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/assume_abstract.hpp>


#include <boost/archive/polymorphic_xml_iarchive.hpp>
#include <boost/archive/polymorphic_xml_oarchive.hpp>

 // 어차피  부스트 시리얼라이즈 저장 포멧은 uft-8 이므로 이런건 버리자 , 문제도 있고
//#include <boost/archive/polymorphic_xml_wiarchive.hpp>
//#include <boost/archive/polymorphic_xml_woarchive.hpp>










#ifdef _MSC_VER
#pragma warning( default : 4308 )
#endif





#endif //////////////////////////////////////////////////////////////////////////