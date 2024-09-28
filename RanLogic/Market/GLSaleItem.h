#pragma once

#include <string>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/tr1/memory.hpp>

#include "../Item/GLItemCustom.h"

namespace private_market
{
	struct CHAINFO
	{
		DWORD ChaNum;
		int ServerGroup;

		CHAINFO()
			: ChaNum( 0 )
			, ServerGroup( 0 )
		{
		}

		CHAINFO( DWORD _ChaNum, int _ServerGroup )
			: ChaNum( _ChaNum )
			, ServerGroup( _ServerGroup )
		{
		}

		const CHAINFO& operator = ( const CHAINFO& rhs )
		{		
			ChaNum = rhs.ChaNum;
			ServerGroup = rhs.ServerGroup;

			return *this;
		}

		const bool operator < ( const CHAINFO& rhs ) const
		{
			if ( std::make_pair( ChaNum, ServerGroup ) < std::make_pair( rhs.ChaNum, rhs.ServerGroup ) )
				return true;
			return false;
		}

		const bool operator == ( const CHAINFO& rhs ) const
		{
			return ( ChaNum == rhs.ChaNum && ServerGroup == rhs.ServerGroup );
		}

		const bool operator != ( const CHAINFO& rhs ) const
		{
			return ( *this == rhs );
		}
	};

	typedef std::tr1::shared_ptr< SITEMCUSTOM > spItemcustom;

	// ���λ����� ���� �������� �����ϴ� �뵵
	// Ű�� ���� �������������� ���� �޸𸮻󿡼��� �����ϴ� guid �̴�, ���� �����ϴ�.
	// sqlite�� blob���� �����Ϸ��� �Ͽ�����, �켱 �̷��� �ϰ� �׽�Ʈ�� ���� �Ѵ�.
	class GLSaleItem
	{
	public:
		GLSaleItem();
		~GLSaleItem();

	public:
		typedef boost::unordered_map< std::string, spItemcustom > SALE_ITEM;
		typedef SALE_ITEM::iterator SALE_ITEM_ITER;
		typedef SALE_ITEM::value_type SALE_ITEM_VALUE;

		typedef std::multimap< CHAINFO, std::string > CHA_SALE_ITEM;
		typedef CHA_SALE_ITEM::iterator CHA_SALE_ITEM_ITER;
		typedef CHA_SALE_ITEM::value_type CHA_SALE_ITEM_VALUE;

	public:
		bool InsertSaleItem( UUID& sItemGUID, const SITEMCUSTOM& sItemcustom, DWORD ChaNum, bool ConsignmentSale, int ServerGroup );
		bool InsertSaleItem( const std::string& strGUID, const SITEMCUSTOM& sItemcustom, DWORD ChaNum, bool ConsignmentSale, int ServerGroup );

		bool DeleteSaleItem( UUID& sItemGUID, DWORD ChaNum, int ServerGroup );
		bool DeleteSaleItem( const std::string& strGUID, DWORD ChaNum, int ServerGroup );
		void DeleteSaleItem( DWORD ChaNum, int ServerGroup );
		void DeleteSaleItemAll();

		spItemcustom FindSaleItem( UUID& sItemGUID );
		spItemcustom FindSaleItem( const std::string& strGUID );

	protected:
		SALE_ITEM m_theContainer;
		SALE_ITEM m_ConsignmentSale;
		CHA_SALE_ITEM m_ForSearch;

	};

}
