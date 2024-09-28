
#pragma once

#include "./GLItem_RandomGenOp_Utility.h"
#include "./GLItemDef.h"

struct SITEM;

class SITEMCUSTOM_ADDON
{
public:
	enum { ADDON_SIZE = 6 };
	union 
	{
		struct { ITEM::SADDON m_sADDON[6]; };
		struct 
		{ 
			ITEM::SADDON m_sADDON_0
  			 , m_sADDON_1
 			 , m_sADDON_2
  			 , m_sADDON_3
 			 , m_sADDON_4
 			 , m_sADDON_5;
		};
	};

public:
	SITEMCUSTOM_ADDON(void);
	virtual ~SITEMCUSTOM_ADDON(void);

	MSGPACK_DEFINE(
		  (int&)m_sADDON_0.emTYPE
		, m_sADDON_0.nVALUE
		, (int&)m_sADDON_1.emTYPE
		, m_sADDON_1.nVALUE
		, (int&)m_sADDON_2.emTYPE
		, m_sADDON_2.nVALUE
		, (int&)m_sADDON_3.emTYPE
		, m_sADDON_3.nVALUE
		, (int&)m_sADDON_4.emTYPE
		, m_sADDON_4.nVALUE
		, (int&)m_sADDON_5.emTYPE
		, m_sADDON_5.nVALUE);
	
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & (unsigned short&)m_sADDON_0.emTYPE;
		ar & m_sADDON_0.nVALUE;
		ar & (unsigned short&)m_sADDON_1.emTYPE;
		ar & m_sADDON_1.nVALUE;
		ar & (unsigned short&)m_sADDON_2.emTYPE;
		ar & m_sADDON_2.nVALUE;
		ar & (unsigned short&)m_sADDON_3.emTYPE;
		ar & m_sADDON_3.nVALUE;
		ar & (unsigned short&)m_sADDON_4.emTYPE;
		ar & m_sADDON_4.nVALUE;
		ar & (unsigned short&)m_sADDON_5.emTYPE;
		ar & m_sADDON_5.nVALUE;
	}

public:
	// 초기화 되었는지 확인; 여기서 초기화란 SITEMCUSTOM생성시 ADDON 데이터 초기화를 말함;
	bool IsInit(void) const;
	bool IsEmpty(void) const;

public:
	void DefaultInitialize( const SNATIVEID& sNID );
	void DefaultInitialize( const SITEM* pItem );
	bool Generate_RandomAddon( DWORD dwGenerateOp, const SITEM* const pItem, const ITEM_RANDOMOP_UTILL::FixOption& fixOption = ITEM_RANDOMOP_UTILL::FixOption(), SITEM* pItemStamp = NULL );

	void ResetAddon(void);
	DWORD GetOptType(DWORD dwIndex) const;
	const UINT GetOptIndex ( const BYTE _nOptionType ) const;

public:
	/// Datatyoe 이 바뀔 경우를 대비해 보정해준다;
	/// 보정이 필요없도록 저장 관리가 된는것이 더 좋은 방법이다;
	/// 이미 이렇게 구현되었으며 Datatype 이 변환될 일이 버그수정이후에는 거의 없으므로;
	/// 그대로 사용하자;

	/// 아래 함수는 GLItemSuit 에 있는 GetAddonValue Function, GetAddonValue_CompareExchange Function 과 같이 수정되어야 한다;
	/// 중복코드가 되지만 수정할 일이 거의 없는 부분이라 그냥 중복시켰다
	template < typename T >
	const T GetValue ( const int _nIndex, const bool _bInteger ) const
	{
		if ( _nIndex < 0 || ADDON_SIZE <= _nIndex )
			return static_cast< const T >( 0 );

		int nValue = 0;
		float fValue = 0.f;
		if ( _bInteger )
		{
			nValue = m_sADDON[ _nIndex ].nVALUE;
			if ( 100000 <= nValue || nValue <= -100000 )
				nValue = static_cast< int >( m_sADDON[ _nIndex ].fVALUE );

			if ( 100000 <= nValue || nValue <= -100000 )
				nValue = 0;
		}
		else
		{
			fValue = m_sADDON[ _nIndex ].fVALUE;
			if ( 100000.f <= fValue || fValue <= -100000.f )
				fValue = static_cast< float >( m_sADDON[ _nIndex ].nVALUE );

			if ( 100000.f <= fValue || fValue <= -100000.f )
				fValue = 0.f;
		}

		const T tReturnValue = static_cast< const T >( ( _bInteger ) ? nValue : fValue );

		return tReturnValue;
	}

	/// Addon Option Type 을 이용하여 반환한다;
	template <typename T>
	const T GetValue_OptType(const BYTE _nOptionType) const
	{
		const UINT nIndex = GetOptIndex(_nOptionType);
		if (nIndex < ADDON_SIZE)
		{
			return GetValue<T>(nIndex, ITEM::bAddonValueInteger[_nOptionType]);
		}

		return static_cast<T>(0);
	}
};