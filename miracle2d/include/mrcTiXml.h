

#ifndef _mrcTixml_h_
#define _mrcTixml_h_

#include "mrcConfig.h"
#include "mrcUtil/mrcPolymorphFactory.h"
#include "mrcUtil/mrcFileSystem.h"

#include <boost/mpl/identity.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_array.hpp>

namespace mrc
{
	namespace tixml
	{
		struct MetaArg
		{
			MetaArg(TiXmlElement* pEle,const String& name,const String& arrayItemName) : 
				mEle(pEle),mName(name),mArrayItemName(arrayItemName) {}

			TiXmlElement* mEle;
			const String  mName;
			const String  mArrayItemName;
		};

		// Tixml 구현을 완전히 숨기자
		typedef mrcSharedPtr<TiXmlDocument> SP_TiXmlDocument;
		
		TiXmlElement* createDocument(const String& startEleName,int32 version,SP_TiXmlDocument& spDoc); 
		void saveDocument(const SP_TiXmlDocument& spDoc, const String& fullPath);

		TiXmlElement* loadDocument(const String& fullPath,const String& startEleName,int32& version,
			SP_TiXmlDocument& spDoc);

		// Tixml의 구현을 완전히 숨기기 위해 ... 밑에것을 다 만듬 
		// (포인터만 쓰는 입장에서는 상속인지 모르므로 다 만들어줘야함)
		TiXmlElement* insertElement(const SP_TiXmlDocument& pEle,const String& eleName );
		TiXmlElement* insertElement(TiXmlElement* pEle,const String& eleName );
		TiXmlElement* insertElement(TiXmlNode* pEle,const String& eleName );

		TiXmlElement* nextElement(TiXmlElement* pEle);


		void setAttribute(TiXmlElement* pEle,const String& attrName,const NString& attrValue);
		void setAttribute(TiXmlElement* pEle,const String& attrName,const WString& attrValue);
		void setAttribute(TiXmlElement* pEle,const String& attrName,int32 attrValue);

		int32   getAttributeInt(TiXmlElement* pEle,const String& attrName);
		real    getAttributeReal(TiXmlElement* pEle,const String& attrName);
		String  getAttribute(TiXmlElement* pEle,const String& attrName);

		int32   getAttribute(TiXmlElement* pEle,const String& attrName,int32 );
		String  getAttribute(TiXmlElement* pEle,const String& attrName,String );


		namespace detail
		{
			void    setElementName(TiXmlElement* pEle,const String& eleName);
			String  getElementName(TiXmlElement* pEle);


			//////////////////////////////////////////////////////////////////////////
			template<class VALUE>
			void saveXml(TiXmlElement* pEle,const VALUE& value)
			{
				String baseType =    toString(getClassName_NoOriginal(value));
				String derivedType = toString(getClassName(value));

				mrcASSERT(!baseType.empty());
				mrcASSERT(!derivedType.empty());

				bool isRegister = PolyporphBaseObjectFactory::getInstance().isRegisterClassName(toNString(derivedType));

				if( baseType != derivedType )
				{
					mrcASSERT(isRegister);
				}

				if(getElementName(pEle).empty())
					setElementName(pEle,baseType);

				if( !derivedType.empty() )
				{
					if( baseType != derivedType )
						setAttribute(pEle,_mrcT("class_type"), derivedType );
				}
			
				refPtr(value).saveXml(pEle);
			}

			template<class KEY,class VALUE>
			void saveXml(TiXmlElement* pEle,const std::pair<KEY,VALUE>& pairData)
			{
				setAttribute(pEle,_mrcT("id"),pairData.first);
				saveXml(pEle,pairData.second);
			}
		} // detail
		
		
		
		void insertElement(TiXmlElement* pEle,const String& eleName,int8   value);
		void insertElement(TiXmlElement* pEle,const String& eleName,int16  value);
		void insertElement(TiXmlElement* pEle,const String& eleName,int32  value);
		void insertElement(TiXmlElement* pEle,const String& eleName,uint8  value);
		void insertElement(TiXmlElement* pEle,const String& eleName,uint16 value);
		void insertElement(TiXmlElement* pEle,const String& eleName,uint32 value);

		void insertElement(TiXmlElement* pEle,const String& eleName,real value);
		void insertElement(TiXmlElement* pEle,const String& eleName,const String& value);
		void insertElement(TiXmlElement* pEle,const String& eleName,const filesystem::Path& value);

// 		template<class VALUE>
// 		void insertElement(TiXmlElement* pEle,const String& eleName,const VALUE& value)
// 		{
// 			// 유저 임의의 이름은 등록된 클래스 이름이면 곤란( 잘못 쓰일수 있으므로 ) 
// 			mrcASSERT(!PolyporphBaseObjectFactory::getInstance().isRegisterClassName(toNString(eleName)));
// 
// 			TiXmlElement* pItemEle = insertElement(pEle, eleName);
// 			detail::saveXml(pItemEle,value);
// 		}

		template<class VALUE>
		void insertElement(TiXmlElement* pEle,const String& eleName,const VALUE& value,const String& arrayItemName = _mrcT(""))
		{
			typedef 
				typename boost::mpl::eval_if<boost::is_array<VALUE>,
				boost::mpl::identity< detail::__insert::ArrayTYPE<VALUE> >,
				//else 
				typename  boost::mpl::eval_if<boost::is_enum<VALUE>,
				boost::mpl::identity< detail::__insert::EnumTYPE<VALUE> >,

				boost::mpl::identity< detail::__insert::AnyTYPE<VALUE> > > >::type typeX;

			typeX::invoke(MetaArg(pEle,eleName,arrayItemName),value);
		}

	
		//////////////////////////////////////////////////////////////////////////

		template<class VALUE>
		void insertElement(TiXmlElement* pEle,const String& eleName,
			const vector<VALUE>& container,const String& containerItemName = _mrcT("") )
		{
			TiXmlElement* pContainerEle = insertElement(pEle,eleName);
			for (vector<VALUE>::const_iterator it = container.begin(); it != container.end(); ++it)
			{
				insertElement(pContainerEle,containerItemName,*it);
			}
		}

		template<class VALUE>
		void insertElement(TiXmlElement* pEle,const String& eleName,
			const list<VALUE>& container,const String& containerItemName = _mrcT("")  )
		{
			TiXmlElement* pContainerEle = insertElement(pEle,eleName);
			for (list<VALUE>::const_iterator it = container.begin(); it != container.end(); ++it)
			{
				insertElement(pContainerEle,containerItemName,*it);
			}
		}

		template<class VALUE>
		void insertElement(TiXmlElement* pEle,const String& eleName,
			const deque<VALUE>& container,const String& containerItemName = _mrcT("") )
		{
			TiXmlElement* pContainerEle = insertElement(pEle,eleName);
			for (deque<VALUE>::const_iterator it = container.begin(); it != container.end(); ++it)
			{
				insertElement(pContainerEle,containerItemName,*it);
			}
		}

		template<class KEY,class VALUE>
		void insertElement(TiXmlElement* pEle,const String& eleName,
			const map<KEY,VALUE>& container,const String& containerItemName = _mrcT("")  )
		{
			TiXmlElement* pContainerEle = insertElement(pEle,eleName);
			for (map<KEY,VALUE>::const_iterator it = container.begin(); it != container.end(); ++it)
			{
				insertElement(pContainerEle,containerItemName,*it);
			}
		}

		template<class KEY,class VALUE>
		void insertElement(TiXmlElement* pEle,const String& eleName,
			const hash_map<KEY,VALUE>& container,const String& containerItemName = _mrcT("")  )
		{
			TiXmlElement* pContainerEle = insertElement(pEle,eleName);
			for (hash_map<KEY,VALUE>::const_iterator it = container.begin(); it != container.end(); ++it)
			{
				insertElement(pContainerEle,containerItemName,*it);
			}
		}

		template<class VALUE>
		void insertElement(TiXmlElement* pEle,const String& eleName,
			const ordered_vector<VALUE>& container,const String& containerItemName = _mrcT("") )
		{
			TiXmlElement* pContainerEle = insertElement(pEle,eleName);
			for (ordered_vector<VALUE>::const_iterator it = container.begin(); it != container.end(); ++it)
			{
				insertElement(pContainerEle,containerItemName,*it);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		TiXmlElement* getChildElement(TiXmlDocument* pEle,const String& eleName );
		TiXmlElement* getChildElement(TiXmlElement* pEle,const String& eleName );
		TiXmlElement* getChildElement(TiXmlNode* pEle,const String& eleName );
		TiXmlElement* getChildElement(TiXmlElement* pEle);
	

		void getElement(TiXmlElement* pEle,const String& eleName,uint8&  value);
		void getElement(TiXmlElement* pEle,const String& eleName,uint16&  value);
		void getElement(TiXmlElement* pEle,const String& eleName,uint32&  value);

		void getElement(TiXmlElement* pEle,const String& eleName,int8&  value);
		void getElement(TiXmlElement* pEle,const String& eleName,int16&  value);
		void getElement(TiXmlElement* pEle,const String& eleName,int32&  value);

		void getElement(TiXmlElement* pEle,const String& eleName,real&   value);
		void getElement(TiXmlElement* pEle,const String& eleName,String& value);
		void getElement(TiXmlElement* pEle,const String& eleName,filesystem::Path& value);


		namespace detail
		{
	
			inline void KEY_ASSERT(int32 value) { mrcASSERT(value != -1); }
			inline void KEY_ASSERT(const String& value) { mrcASSERT(!value.empty()); }

			void getElementDirect(TiXmlElement* pEle,const String& eleName,uint8&  value);
			void getElementDirect(TiXmlElement* pEle,const String& eleName,uint16&  value);
			void getElementDirect(TiXmlElement* pEle,const String& eleName,uint32&  value);

			void getElementDirect(TiXmlElement* pEle,const String& eleName,int8&  value);
			void getElementDirect(TiXmlElement* pEle,const String& eleName,int16&  value);
			void getElementDirect(TiXmlElement* pEle,const String& eleName,int32&  value);

			void getElementDirect(TiXmlElement* pEle,const String& eleName,real&   value);
			void getElementDirect(TiXmlElement* pEle,const String& eleName,String& value);
			void getElementDirect(TiXmlElement* pEle,const String& eleName,filesystem::Path& value);



			template<class VALUE>
			mrcAutoPtr<VALUE> MakeNew(TiXmlElement* pEle)
			{
				mrcASSERT(pEle);
				mrcAutoPtr<VALUE> value;
				NString derivedType = toNString( tixml::getAttribute(pEle,_mrcT("class_type")));
				NString type;
				// 클래스 타입이 따로 없다면 속성값이름으로 다시 도전
				if(!derivedType.empty())
					type = derivedType;
				else				
					type = toNString( getElementName(pEle) );

				mrcASSERT( !type.empty() );

				value = mrcAutoPtr<VALUE>( PolyporphBaseObjectFactory::getInstance().createObject<VALUE>(type) );

				// 추상 클래스 포인터가 올 경우 컴파일 할수가 없다 , tixml에 부모 포인터를 쓰고 싶다면 
				// PolymorphBaseObject를 반드시 상속 받아 처리하자
				// 			else
				// 				value = mrcAutoPtr<VALUE>(new VALUE);

				return value;
			}
		
			template<class KEY,class VALUE>
			void getElementDirect(TiXmlElement* pEle,const String& eleName,std::pair<KEY,mrcSharedPtr<VALUE> >& pairValue)
			{
				mrcASSERT(pEle);
				KEY id = getAttribute(pEle,_mrcT("id"),KEY() );
				KEY_ASSERT(id);

				pairValue.first = id;
				pairValue.second = MakeNew<VALUE>(pEle);

				refPtr(pairValue.second).loadXml(pEle);
			}

			template<class VALUE>
			void getElementDirect(TiXmlElement* pEle,const String& eleName,mrcAutoPtr<VALUE>& value)
			{
				value = MakeNew<VALUE>(pEle);
				refPtr(value).loadXml(pEle);
			}

			template<class VALUE>
			void getElementDirect(TiXmlElement* pEle,const String& eleName,mrcSharedPtr<VALUE>& value)
			{
				mrcAutoPtr<VALUE> apValue;
				getChildElement(pEle,apValue);
				value = apValue;
			}

			template<class VALUE>
			void getElementDirect(TiXmlElement* pEle,const String& eleName,VALUE& value)
			{
				value.loadXml(pEle);
			}
		} // detail
		
		template<class KEY,class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,std::pair<KEY,mrcSharedPtr<VALUE> >& pairValue)
		{
			pEle = getChildElement(pEle,eleName);
			detail::getElementDirect(pEle,eleName,pairValue);
		}

		template<class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,mrcAutoPtr<VALUE>& value)
		{
			pEle = getChildElement(pEle,eleName);
			detail::getElementDirect(pEle,eleName,value);
		}

		template<class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,mrcSharedPtr<VALUE>& value)
		{
			mrcAutoPtr<VALUE> apValue;
			getElement(pEle,eleName,value);
			value = apValue;
		}

// 		template<class VALUE>
// 		void getElement(TiXmlElement* pEle,const String& eleName,VALUE& value)
// 		{
//  			pEle = getChildElement(pEle,eleName);
//  			detail::getElementDirect(pEle,eleName,value);
// 		}


		template<class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,VALUE& value,const String& arrayItemName = _mrcT(""))
		{
			typedef 
				typename boost::mpl::eval_if<boost::is_array<VALUE>,
				boost::mpl::identity< detail::__get::ArrayTYPE<VALUE> >,
				//else 
				typename  boost::mpl::eval_if<boost::is_enum<VALUE>,
				boost::mpl::identity< detail::__get::EnumTYPE<VALUE> >,

				boost::mpl::identity< detail::__get::AnyTYPE<VALUE> > > >::type typeX;

			typeX::invoke(MetaArg(pEle,eleName,arrayItemName),value);

		}

		template<class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,
			vector<VALUE>& container,const String& containerItemName = _mrcT("") )
		{
			container.clear();
			typedef vector<VALUE> CONTAINER;
			TiXmlElement* pContainerEle = getChildElement(pEle,eleName);
			mrcASSERT_MSG(pContainerEle, toNString(eleName)  + " => unknown container name " );

			for (TiXmlElement* pItemEle = getChildElement(pContainerEle); 
				pItemEle != NULL; pItemEle = nextElement(pItemEle) )
			{
				CONTAINER::value_type typeValue;
				detail::getElementDirect(pItemEle,containerItemName,typeValue);
				container.push_back(typeValue);
			}
		}

		template<class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,
			list<VALUE>& container,const String& containerItemName = _mrcT("") )
		{
			container.clear();
			typedef list<VALUE> CONTAINER;
			TiXmlElement* pContainerEle = getChildElement(pEle,eleName);
			mrcASSERT_MSG(pContainerEle, toNString(eleName)  + " => unknown container name " );

			for (TiXmlElement* pItemEle = getChildElement(pContainerEle); 
				pItemEle != NULL; pItemEle = nextElement(pItemEle) )
			{
				CONTAINER::value_type typeValue;
				detail::getElementDirect(pItemEle,containerItemName,typeValue);
				container.push_back(typeValue);
			}
		}
	
		template<class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,
			deque<VALUE>& container,const String& containerItemName = _mrcT("") )
		{
			container.clear();
			typedef deque<VALUE> CONTAINER;
			TiXmlElement* pContainerEle = getChildElement(pEle,eleName);
			mrcASSERT_MSG(pContainerEle, toNString(eleName)  + " => unknown container name " );

			for (TiXmlElement* pItemEle = getChildElement(pContainerEle); 
				pItemEle != NULL; pItemEle = nextElement(pItemEle) )
			{
				CONTAINER::value_type typeValue;
				detail::getElementDirect(pItemEle,containerItemName,typeValue);
				container.push_back(typeValue);
			}
		}

		template<class KEY,class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,
			map<KEY,VALUE>& container,const String& containerItemName = _mrcT("") )
		{
			container.clear();
			typedef map<KEY,VALUE> CONTAINER;
			TiXmlElement* pContainerEle = getChildElement(pEle,eleName);
			mrcASSERT_MSG(pContainerEle, toNString(eleName)  + " => unknown container name " );

			for (TiXmlElement* pItemEle = getChildElement(pContainerEle); 
				pItemEle != NULL; pItemEle = nextElement(pItemEle) )
			{
				std::pair<CONTAINER::key_type,CONTAINER::mapped_type> typeValue;
				detail::getElementDirect(pItemEle,containerItemName,typeValue);
				container.insert(typeValue);
			}
		}

		template<class KEY,class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,
			hash_map<KEY,VALUE>& container,const String& containerItemName = _mrcT("") )
		{
			container.clear();
			typedef hash_map<KEY,VALUE> CONTAINER;
			TiXmlElement* pContainerEle = getChildElement(pEle,eleName);
			mrcASSERT_MSG(pContainerEle, toNString(eleName)  + " => unknown container name " );

			for (TiXmlElement* pItemEle = getChildElement(pContainerEle); 
				pItemEle != NULL; pItemEle = nextElement(pItemEle) )
			{
				std::pair<CONTAINER::key_type,CONTAINER::mapped_type> typeValue;
				detail::getElementDirect(pItemEle,containerItemName,typeValue);
				container.insert(typeValue);
			}
		}

		template<class VALUE>
		void getElement(TiXmlElement* pEle,const String& eleName,
			ordered_vector<VALUE>& container,const String& containerItemName = _mrcT("") )
		{
			container.clear();
			typedef ordered_vector<VALUE> CONTAINER;
			TiXmlElement* pContainerEle = getChildElement(pEle,eleName);
			mrcASSERT_MSG(pContainerEle, toNString(eleName)  + " => unknown container name " );

			for (TiXmlElement* pItemEle = getChildElement(pContainerEle); 
				pItemEle != NULL; pItemEle = nextElement(pItemEle) )
			{
				CONTAINER::value_type typeValue;
				detail::getElementDirect(pItemEle,containerItemName,typeValue);
				container.insert(typeValue);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		

		namespace detail
		{
			namespace __insert
			{
				template<typename T>
				struct ArrayTYPE
				{
					static void invoke(const MetaArg& arg, const T& arrayValue)
					{
						TiXmlElement* pContainerEle = insertElement(arg.mEle,arg.mName);
						for (int32 i = 0; i < ARRAYSIZE(arrayValue); ++i)
						{
							insertElement(pContainerEle,arg.mArrayItemName,arrayValue[i]);
						}
					}
				};

				template<typename T>
				struct EnumTYPE
				{
					static void invoke(const MetaArg& arg,const T& enumValue)
					{
						insertElement(arg.mEle,arg.mName,(int32)enumValue);
					}
				};

				template<typename T>
				struct AnyTYPE
				{
					static void invoke(const MetaArg& arg,const T& value)
					{
						// 유저 임의의 이름은 등록된 클래스 이름이면 곤란( 잘못 쓰일수 있으므로 ) 
						mrcASSERT(!PolyporphBaseObjectFactory::getInstance().
							isRegisterClassName(toNString(arg.mName)));

						TiXmlElement* pItemEle = insertElement(arg.mEle,arg.mName);
						detail::saveXml(pItemEle,value);
					}
				};
			}

			namespace __get
			{
				template<typename T>
				struct ArrayTYPE
				{
					static void invoke(const MetaArg& arg, T& arrayValue)
					{
						TiXmlElement* pContainerEle = getChildElement(arg.mEle,arg.mName);
						mrcASSERT_MSG(pContainerEle, toNString(arg.mName)  + " => unknown array name " );

						int32 index=0;
						for (TiXmlElement* pItemEle = getChildElement(pContainerEle); 
							pItemEle != NULL; pItemEle = nextElement(pItemEle),++index )
						{
							mrcASSERT_MSG( index < ARRAYSIZE(arrayValue) ,  "array size overflow" );

							detail::getElementDirect(pItemEle,arg.mArrayItemName,arrayValue[index]);
						}
					}
				};

				template<typename T>
				struct EnumTYPE
				{
					static void invoke(const MetaArg& arg,T& enumValue)
					{
						TiXmlElement* pEle = getChildElement(arg.mEle,arg.mName);
						int32 value;
						detail::getElementDirect(pEle,arg.mEle,value);
						enumValue = (T)value;
					}
				};

				template<typename T>
				struct AnyTYPE
				{
					static void invoke(const MetaArg& arg,T& anyValue)
					{
						TiXmlElement* pEle = getChildElement(arg.mEle,arg.mName);
						detail::getElementDirect(pEle,arg.mName,anyValue);								
					}
				};
			}
		}
	}
}



#endif