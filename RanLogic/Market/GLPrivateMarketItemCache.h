#pragma once

#include <string>
#include <vector>

#include "../../SigmaCore/Database/Sqlite/SQLite.h"

#include "./MarketDefine.h"

namespace private_market
{
	// �˻��� �������� ĳ���ϰ� �ִ´�.
	// ������ ����

	class GLSaleItem;
	struct sSaleItemSearchandSortBasic;
	struct sSaleItemSearchandSortName;
	struct sSaleItem;

	enum eSearchType
	{
		eSearchTypeBasic, 
		eSearchTypeName, 
	};

	class GLPrivateMarketItemCache : public CSingletone< GLPrivateMarketItemCache >
	{
	protected:
		friend CSingletone;

	public:
		GLPrivateMarketItemCache();
		~GLPrivateMarketItemCache();

	public:

		// �˻���û�� �ϰ� ���� �����͸� �ް� �ִ°�?
		bool IsReceiving() { return m_bReceiving; }
		void SetReceiving( bool bValue );

		void SetSearchCondition( sSaleItemSearchandSortBasic& sSearchBasic );
		void SetSearchCondition( sSaleItemSearchandSortName& sSearchName );

		DWORD GetLastReceiveStartTime() { return m_dwLastReceiveStartTime; }

		int IsCached( sSaleItemSearchandSortBasic& sSearchBasic );
		int IsCached( sSaleItemSearchandSortName& sSearchName );

		int GetItem( WORD wPage, std::vector< sSaleItem >& vecItem );
		int GetItem( std::vector< sSaleItem >& vecItem );
		int GetItem( UUID& sItemGUID, sSaleItem& sItem OUT );

		int InsertItem( sSaleItem& sItem );
		int DeleteItem( UUID& sItemGUID );

		void Reset();

	protected:
		int openDatabase();
		void closeDatabase();
		int createTable();
		int execute( const std::string& strQuery );
		WORD getPage();
		
	protected:
		SQLite::Database m_theCache;
		GLSaleItem* m_pSaleItem;
		sSaleItemSearchandSortBasic m_sSearchBasic;
		sSaleItemSearchandSortName m_sSearchName;
		std::vector< WORD > m_CachedPage;
		bool m_bReceiving;
		DWORD m_dwLastReceiveStartTime;
		eSearchType m_eCurrentSearchType;

	};
}

