
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
	// �ʱ�ȭ �Ǿ����� Ȯ��; ���⼭ �ʱ�ȭ�� SITEMCUSTOM������ ADDON ������ �ʱ�ȭ�� ����;
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
	/// Datatyoe �� �ٲ� ��츦 ����� �������ش�;
	/// ������ �ʿ������ ���� ������ �ȴ°��� �� ���� ����̴�;
	/// �̹� �̷��� �����Ǿ����� Datatype �� ��ȯ�� ���� ���׼������Ŀ��� ���� �����Ƿ�;
	/// �״�� �������;

	/// �Ʒ� �Լ��� GLItemSuit �� �ִ� GetAddonValue Function, GetAddonValue_CompareExchange Function �� ���� �����Ǿ�� �Ѵ�;
	/// �ߺ��ڵ尡 ������ ������ ���� ���� ���� �κ��̶� �׳� �ߺ����״�
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

	/// Addon Option Type �� �̿��Ͽ� ��ȯ�Ѵ�;
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