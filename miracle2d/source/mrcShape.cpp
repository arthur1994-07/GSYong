
#include "mrcPrecompiled.h"
#include "mrcShape.h"

#include "mrcTixml.h"

namespace mrc
{

#define MIRACLE_XML_NAME(X) _mrcT(#X),m ## X
#define MIRACLE_XML_SERIALIZE_Point(SERIALIZE) \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(X));\
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(Y));

	void Point::saveXml( TiXmlElement* pDataEle ) const
	{
		MIRACLE_XML_SERIALIZE_Point(tixml::insertElement);
	}

	void Point::loadXml( TiXmlElement* pDataEle )
	{
		MIRACLE_XML_SERIALIZE_Point(tixml::getElement);
	}

#undef MIRACLE_XML_NAME
#undef MIRACLE_XML_SERIALIZE_Point


#define MIRACLE_XML_NAME(X) _mrcT(#X),m ## X
#define MIRACLE_XML_SERIALIZE_Rect(SERIALIZE) \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(Left));\
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(Top));\
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(Right));\
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(Bottom));

	void Rect::saveXml( TiXmlElement* pDataEle ) const
	{
		MIRACLE_XML_SERIALIZE_Rect(tixml::insertElement);
	}

	void Rect::loadXml( TiXmlElement* pDataEle )
	{
		MIRACLE_XML_SERIALIZE_Rect(tixml::getElement);
	}
	//////////////////////////////////////////////////////////////////////////


	void Rect_real::saveXml( TiXmlElement* pDataEle ) const
	{
		MIRACLE_XML_SERIALIZE_Rect(tixml::insertElement);
	}

	void Rect_real::loadXml( TiXmlElement* pDataEle )
	{
		MIRACLE_XML_SERIALIZE_Rect(tixml::getElement);
	}
#undef MIRACLE_XML_NAME
#undef MIRACLE_XML_SERIALIZE_Rect


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

#define MIRACLE_XML_NAME(X) _mrcT(#X),m ## X
#define MIRACLE_XML_SERIALIZE_Triangle(SERIALIZE) \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(Points),_mrcT("Point"));

	void Triangle::saveXml( TiXmlElement* pDataEle ) const
	{
		MIRACLE_XML_SERIALIZE_Triangle(tixml::insertElement);
	}

	void Triangle::loadXml( TiXmlElement* pDataEle )
	{
		MIRACLE_XML_SERIALIZE_Triangle(tixml::getElement);
	}
	//////////////////////////////////////////////////////////////////////////
#undef MIRACLE_XML_NAME
#undef MIRACLE_XML_SERIALIZE_Triangle


}
