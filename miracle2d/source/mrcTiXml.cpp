
#include "mrcPrecompiled.h"
#include "mrcTiXml.h"
#include "TinyXml/tinyxml.h"

#include "mrcUtil/mrcFileSystem.h"

namespace mrc
{
	namespace tixml
	{
	
		#define toCString(str) (toXmlString(str).c_str())
		NString toXmlString(const WString& str)	{ return toNString(str); }
		NString toXmlString(const NString& str) { return str; }

		String fromXmlString(const NString& str)
		{
#ifdef _UNICODE
			return toWString(str);
#else
			return str;
#endif
		}

		//////////////////////////////////////////////////////////////////////////
		namespace detail
		{
			void setElementName(TiXmlElement* pEle,const String& eleName)
			{
				mrcASSERT(pEle);
				pEle->SetValue( toCString(eleName) );
			}
		
			String getElementName(TiXmlElement* pEle)
			{
				mrcASSERT(pEle);
				return toString(pEle->Value());
			}
	
			template<class T>
			void getElementDirectHelperInt(TiXmlElement* pEle,const String& eleName,T&  value)			
			{
				mrcASSERT( pEle->GetText() != 0 );
				if(!eleName.empty())
					mrcASSERT( getElementName(pEle) == eleName );
				value = (T) atoi(pEle->GetText());				
			}

			void getElementDirect(TiXmlElement* pEle,const String& eleName,uint8&  value)
			{
				getElementDirectHelperInt(pEle,eleName,value);
			}

			void getElementDirect(TiXmlElement* pEle,const String& eleName,uint16&  value)
			{
				getElementDirectHelperInt(pEle,eleName,value);
			}

			void getElementDirect(TiXmlElement* pEle,const String& eleName,uint32&  value)
			{
				getElementDirectHelperInt(pEle,eleName,value);
			}

			void getElementDirect(TiXmlElement* pEle,const String& eleName,int8&  value)
			{
				getElementDirectHelperInt(pEle,eleName,value);
			}

			void getElementDirect(TiXmlElement* pEle,const String& eleName,int16&  value)
			{
				getElementDirectHelperInt(pEle,eleName,value);
			}
			void getElementDirect(TiXmlElement* pEle,const String& eleName,int32&  value)
			{
				getElementDirectHelperInt(pEle,eleName,value);
			}

			void getElementDirect(TiXmlElement* pEle,const String& eleName,real&   value)
			{
				mrcASSERT( pEle->GetText() != 0 );
				if(!eleName.empty())
					mrcASSERT( getElementName(pEle) == eleName);
				value = (real)atof(pEle->GetText());	
			}

			void getElementDirect(TiXmlElement* pEle,const String& eleName,String& value)
			{
				mrcASSERT( pEle->GetText() != 0 );
				if(!eleName.empty())
					mrcASSERT( getElementName(pEle) == eleName);
				value = toString(pEle->GetText());	
			}

			void getElementDirect(TiXmlElement* pEle,const String& eleName,filesystem::Path& value)
			{
				String tempValue;
				getElementDirect(pEle,eleName,tempValue);
				value.assign(tempValue.begin(),tempValue.end());
			}

		}


		//////////////////////////////////////////////////////////////////////////
		
		TiXmlElement* createDocument(const String& startEleName,int32 version,SP_TiXmlDocument& spDoc)
		{
			spDoc.reset(mrcNEW TiXmlDocument);	
			TiXmlElement* pStartEle = insertElement(spDoc,startEleName);
			setAttribute(pStartEle,_mrcT("version"),version);
			return pStartEle;
		}

		void saveDocument(const SP_TiXmlDocument& spDoc , const String& fullPath )
		{
			bool ret = spDoc->SaveFile( toCString(fullPath) );
			mrcASSERT_MSG(ret,spDoc->ErrorDesc());
		}

		TiXmlElement* loadDocument(const String& fullPath,const String& startEleName,int32& version,
			SP_TiXmlDocument& spDoc)
		{
			spDoc.reset(mrcNEW TiXmlDocument);
			bool ret = spDoc->LoadFile( toCString(fullPath) );
			mrcASSERT_MSG(ret,spDoc->ErrorDesc());
			TiXmlElement* pStartEle = getChildElement(spDoc.get(),startEleName);
			version = getAttributeInt(pStartEle,_mrcT("version"));
			return pStartEle;
		}


		TiXmlElement* insertElement( TiXmlNode* pEle,const String& eleName )
		{
			return pEle->InsertEndChild(TiXmlElement(toCString(eleName)))->ToElement();
		}

		TiXmlElement* insertElement( TiXmlElement* pEle,const String& eleName )
		{
			return insertElement( static_cast<TiXmlNode*>(pEle) , eleName );
		}

		TiXmlElement* insertElement(const SP_TiXmlDocument& pEle,const String& eleName )
		{
			return insertElement( static_cast<TiXmlNode*>(pEle.get()),eleName );
		}

		void insertElement(TiXmlElement* pEle,const String& eleName,int8   value)
		{
			insertElement(pEle,eleName,toString((int32)value));
		}

		void insertElement(TiXmlElement* pEle,const String& eleName,int16  value)
		{
			insertElement(pEle,eleName,toString((int32)value));			
		}

		void insertElement(TiXmlElement* pEle,const String& eleName,int32 value)
		{
			insertElement(pEle,eleName,toString(value));
		}

		void insertElement(TiXmlElement* pEle,const String& eleName,uint8  value)
		{
			insertElement(pEle,eleName,toString((int32)value));			
		}

		void insertElement(TiXmlElement* pEle,const String& eleName,uint16 value)
		{
			insertElement(pEle,eleName,toString((int32)value));			
		}

		void insertElement(TiXmlElement* pEle,const String& eleName,uint32 value)
		{
			insertElement(pEle,eleName,toString((int32)value));	
		}

		void insertElement(TiXmlElement* pEle,const String& eleName,real value)
		{
			insertElement(pEle,eleName,toString(value));
		}

		void insertElement(TiXmlElement* pEle,const String& eleName,const String& value)
		{
			mrcASSERT(pEle);
			TiXmlElement* pChildEle = insertElement(pEle,eleName);
			pChildEle->InsertEndChild( TiXmlText( toCString(value) ) );
		}

		void insertElement(TiXmlElement* pEle,const String& eleName,const filesystem::Path& value)
		{
			String tempValue = value.string().c_str();
			insertElement(pEle,eleName,tempValue);
		}


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		void setAttribute(TiXmlElement* pEle,const String& attrName,const NString& attrValue)
		{
			mrcASSERT(pEle);
			pEle->SetAttribute(toCString(attrName),toCString(attrValue));
		}

		void setAttribute(TiXmlElement* pEle,const String& attrName,const WString& attrValue)
		{
			mrcASSERT(pEle);
			pEle->SetAttribute(toCString(attrName),toCString(attrValue));
		}

		void setAttribute( TiXmlElement* pEle,const String& attrName,int32 attrValue )
		{
			mrcASSERT(pEle);
			pEle->SetAttribute(toCString(attrName),attrValue);
		}

		int32 getAttributeInt( TiXmlElement* pEle,const String& attrName )
		{
			mrcASSERT(pEle);
			int32 temp;
			const char* pRet = pEle->Attribute(toCString(attrName),&temp);
			mrcASSERT(pRet);
			return temp;
		}

		real getAttributeReal( TiXmlElement* pEle,const String& attrName )
		{
			mrcASSERT(pEle);
			double temp;
			const char* pRet = pEle->Attribute(toCString(attrName),&temp);
			mrcASSERT(pRet);
			return (real)temp;
		}

		String getAttribute(TiXmlElement* pEle,const String& attrName)
		{
			mrcASSERT(pEle);
			if(const char* pValue = pEle->Attribute(toCString(attrName)) )
				return fromXmlString(pValue);

			return String();
		}

	//	namespace detail
	//	{
			int32  getAttribute(TiXmlElement* pEle,const String& attrName,int32 )
			{
				return getAttributeInt(pEle,attrName);
			}

			String  getAttribute(TiXmlElement* pEle,const String& attrName,String )
			{
				return tixml::getAttribute(pEle,attrName);
			}

			TiXmlElement* nextElement( TiXmlElement* pEle )
			{
				mrcASSERT(pEle);
				return pEle->NextSiblingElement();
			}
	//	}
		//////////////////////////////////////////////////////////////////////////
		static int32 getElementInt(TiXmlElement* pEle,const String& eleName)
		{
			mrcASSERT(pEle);
			TiXmlElement* pChildEle = getChildElement(pEle,eleName);
			mrcASSERT_MSG(pChildEle,eleName + _mrcT(" - child element is not exist") );
			mrcASSERT(pChildEle->GetText());
			return atoi(pChildEle->GetText());
		}

		void getElement(TiXmlElement* pEle,const String& eleName,uint8&  value)
		{
			value = (uint8) getElementInt(pEle,eleName);
		}
		void getElement(TiXmlElement* pEle,const String& eleName,uint16&  value)
		{
			value = (uint16) getElementInt(pEle,eleName);
		}

		void getElement(TiXmlElement* pEle,const String& eleName,uint32&  value)
		{
			value = (uint32) getElementInt(pEle,eleName);
		}

		void getElement(TiXmlElement* pEle,const String& eleName,int8&  value)
		{
			value = (int8) getElementInt(pEle,eleName);
		}

		void getElement(TiXmlElement* pEle,const String& eleName,int16&  value)
		{
			value = (int16) getElementInt(pEle,eleName);
		}
		
		void getElement(TiXmlElement* pEle,const String& eleName,int32&  value)
		{
			value = (int32) getElementInt(pEle,eleName);
		}
		
		void getElement(TiXmlElement* pEle,const String& eleName,real&   value)
		{
			mrcASSERT(pEle);
			TiXmlElement* pChildEle = getChildElement(pEle,eleName);
			mrcASSERT_MSG(pChildEle,eleName + _mrcT(" - child element is not exist") );
			if( pChildEle->GetText() )
				value = (real)atof(pChildEle->GetText());
		}

		void getElement(TiXmlElement* pEle,const String& eleName,String& value)
		{
			mrcASSERT(pEle);
			TiXmlElement* pChildEle = getChildElement(pEle,eleName);
			mrcASSERT_MSG(pChildEle,eleName + _mrcT(" - child element is not exist") );
			if( pChildEle->GetText() )
				value = toString(pChildEle->GetText());
		}

		void getElement(TiXmlElement* pEle,const String& eleName,filesystem::Path& value)
		{
			String tempValue;
			getElement(pEle,eleName,tempValue);
			value.assign(tempValue.begin(),tempValue.end());
		}

		TiXmlElement* getChildElement(TiXmlDocument* pNode,const String& eleName )
		{
			return getChildElement( static_cast<TiXmlNode*>(pNode) , eleName );
		}

		TiXmlElement* getChildElement(TiXmlElement* pNode,const String& eleName )
		{
			return getChildElement( static_cast<TiXmlNode*>(pNode) , eleName );
		}

		TiXmlElement* getChildElement(TiXmlNode* pNode,const String& eleName )
		{
			return pNode->FirstChildElement(toCString(eleName));
		}

		TiXmlElement* getChildElement(TiXmlElement* pNode)
		{
			return pNode->FirstChildElement();
		}


	
	}
}





