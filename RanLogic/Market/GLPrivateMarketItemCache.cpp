#include "pch.h"

#include <algorithm>

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "./GLSaleItem.h"
#include "./GLPrivateMarketItemCache.h"

#include "../../SigmaCore/DebugInclude.h"

namespace private_market
{
	GLPrivateMarketItemCache::GLPrivateMarketItemCache()
		: m_bReceiving( false )
		, m_dwLastReceiveStartTime( 0 )
		, m_eCurrentSearchType( eSearchTypeBasic )
	{
		m_pSaleItem = new GLSaleItem;
		openDatabase();
	}

	GLPrivateMarketItemCache::~GLPrivateMarketItemCache()
	{
		SAFE_DELETE( m_pSaleItem );
	}

	void GLPrivateMarketItemCache::SetReceiving( bool bValue )
	{
		m_bReceiving = bValue;
		m_dwLastReceiveStartTime = ::GetTickCount();
	}

	void GLPrivateMarketItemCache::SetSearchCondition( sSaleItemSearchandSortBasic& sSearchBasic )
	{
		m_sSearchBasic = sSearchBasic;
		m_CachedPage.push_back( sSearchBasic.wPage );
		m_eCurrentSearchType = eSearchTypeBasic;
	}

	void GLPrivateMarketItemCache::SetSearchCondition( sSaleItemSearchandSortName& sSearchName )
	{
		m_sSearchName = sSearchName;
		m_CachedPage.push_back( sSearchName.wPage );
		m_eCurrentSearchType = eSearchTypeName;
	}

	int GLPrivateMarketItemCache::IsCached( sSaleItemSearchandSortBasic& sSearchBasic )
	{
		// 같은 검색 조건인지 확인
		if ( m_sSearchBasic != sSearchBasic )
		{
			return eItemNotCached;
		}

		// 같은 검색 조건이다. 이제 페이지 확인
		std::vector< WORD >::iterator it = std::find( m_CachedPage.begin(), m_CachedPage.end(), sSearchBasic.wPage );
		if ( it == m_CachedPage.end() )
		{
			// 요청한 페이지는 캐시되어 있지 않다.
			return eItemNotCachedPage;
		}

		return eItemCached;
	}

	int GLPrivateMarketItemCache::IsCached( sSaleItemSearchandSortName& sSearchName )
	{
		// 같은 검색 조건인지 확인
		if ( m_sSearchName != sSearchName )
		{
			return eItemNotCached;
		}

		// 같은 검색 조건이다. 이제 페이지 확인
		std::vector< WORD >::iterator it = std::find( m_CachedPage.begin(), m_CachedPage.end(), sSearchName.wPage );
		if ( it == m_CachedPage.end() )
		{
			// 요청한 페이지는 캐시되어 있지 않다.
			return eItemNotCachedPage;
		}

		return eItemCached;
	}

	int GLPrivateMarketItemCache::GetItem( WORD wPage, std::vector< sSaleItem >& vecItem )
	{
		std::string strItemCacheSelectQuery( 
			sc::string::format( 
			"\
			SELECT \
			ChaNum, TurnNum, Price, GUID, ChaName, \
			MapMainID, MapSubID, SellType, ExpirationDate \
			FROM ItemCache \
			WHERE Page = %1%;", 
			wPage ) );

		SQLite::TablePtr pItemCacheTable( m_theCache.QuerySQL2( strItemCacheSelectQuery.c_str() ) );
		if ( !pItemCacheTable.m_pTable )
		{
			return eNullTablePointer;
		}

		SQLite::Table& rItemCacheTable = pItemCacheTable();
		int nRowCount = rItemCacheTable.GetRowCount();

		if ( nRowCount < 1 )
		{
			return eNotExistItemCache;
		}

		DWORD dwSellerChaDbNum = 0;
		DWORD dwItemTurnNum = 1;
		LONGLONG llItemPrice = 0;
		std::string strGUID;
		UUID sItemGUID;
		SITEMCUSTOM sItemcustom;
		SNATIVEID sSellerMapID;
		std::string strSellerName;
        int nSellType;
        __time64_t ExpirationDate;

		for ( int loop = 0; loop < nRowCount; ++loop )
		{
			dwSellerChaDbNum = rItemCacheTable.GetValue< DWORD >( 0 );
			dwItemTurnNum = rItemCacheTable.GetValue< DWORD >( 1 );
			llItemPrice = rItemCacheTable.GetValue< LONGLONG >( 2 );
			strGUID = rItemCacheTable.GetValue< std::string >( 3 );
			strSellerName = rItemCacheTable.GetValue< std::string >( 4 );
			sSellerMapID.wMainID = rItemCacheTable.GetValue< WORD >( 5 );
			sSellerMapID.wSubID = rItemCacheTable.GetValue< WORD >( 6 );
            nSellType = rItemCacheTable.GetValue< int >( 7 );
            ExpirationDate = rItemCacheTable.GetValue< __time64_t >( 8 );

			spItemcustom spItem = m_pSaleItem->FindSaleItem( strGUID );
			if ( spItem )
			{
				sItemcustom = *spItem.get();
			}

			sc::string::stringToUUID( const_cast< char* >( strGUID.c_str() ), sItemGUID );
			sSaleItem sItem( dwSellerChaDbNum, dwItemTurnNum, llItemPrice, sItemGUID, sItemcustom, sSellerMapID, strSellerName.c_str(), nSellType, ExpirationDate );
			vecItem.push_back( sItem );

			rItemCacheTable.GoNext();
		}

		return eSuccess;
	}

	int GLPrivateMarketItemCache::GetItem( std::vector< sSaleItem >& vecItem )
	{
		return GetItem( getPage(), vecItem );
	}

	int GLPrivateMarketItemCache::GetItem( UUID& sItemGUID, sSaleItem& sItem OUT )
	{
		std::string strGUID = sc::string::uuidToString( sItemGUID );
		std::string strItemCacheSelectQuery( 
			sc::string::format( 
			"\
			SELECT \
			ChaNum, TurnNum, Price, ChaName, MapMainID, \
			MapSubID, SellType \
			FROM ItemCache \
			WHERE GUID = '%1%';", 
			strGUID.c_str() ) );

		SQLite::TablePtr pItemCacheTable( m_theCache.QuerySQL2( strItemCacheSelectQuery.c_str() ) );
		if ( !pItemCacheTable.m_pTable )
		{
			return eNullTablePointer;
		}

		SQLite::Table& rItemCacheTable = pItemCacheTable();
		int nRowCount = rItemCacheTable.GetRowCount();

		if ( nRowCount < 1 )
		{
			return eNotExistItemCache;
		}

		std::string strSellerName;

		sItem.dwSellerChaDbNum = rItemCacheTable.GetValue< DWORD >( 0 );
		sItem.dwItemTurnNum = rItemCacheTable.GetValue< DWORD >( 1 );
		sItem.llItemPrice = rItemCacheTable.GetValue< LONGLONG >( 2 );

		strSellerName = rItemCacheTable.GetValue< std::string >( 3 );
		sItem.SetName( strSellerName.c_str() );

		sItem.sSellerMapID.wMainID = rItemCacheTable.GetValue< WORD >( 4 );
		sItem.sSellerMapID.wSubID = rItemCacheTable.GetValue< WORD >( 5 );
        sItem.nSellType = rItemCacheTable.GetValue< int >( 6 );

		spItemcustom spItem = m_pSaleItem->FindSaleItem( sItemGUID );
		if ( spItem )
		{
			sItem.sItemcustom = *spItem.get();
		}

		sItem.sItemGUID = sItemGUID;

		return eSuccess;
	}

	int GLPrivateMarketItemCache::InsertItem( sSaleItem& sItem )
	{
        WORD wPage = getPage();
        std::string strGUID = sc::string::uuidToString( sItem.sItemGUID );
        std::string strItemCacheInsertQuery( 
            sc::string::format( 
            "\
            INSERT INTO ItemCache( \
            ChaNum,     ChaName,    MapMainID,  MapSubID,   Price, \
            TurnNum,    GUID,       Page,       SellType    ) \
            VALUES( %1%, '%2%', %3%, %4%, %5%, %6%, '%7%', %8%, %9% );", 
            sItem.dwSellerChaDbNum, sItem.szSellerName, sItem.sSellerMapID.wMainID, sItem.sSellerMapID.wSubID, sItem.llItemPrice, 
            sItem.dwItemTurnNum, strGUID.c_str(), wPage, sItem.nSellType ) );

		int nItemCacheInsertResult = execute( strItemCacheInsertQuery );
		if ( SQLITE_OK != nItemCacheInsertResult )
		{
			return eItemCacheInsertFail;
		}

		// 실제 아이템정보 insert
		bool bInsert = false;
		if ( m_pSaleItem )
		{
			// strGUID 가 중복되지만 않는다면 insert 된다.
			bInsert = m_pSaleItem->InsertSaleItem( strGUID, sItem.sItemcustom, 0, false, 0 );
		}

		return bInsert == true ? eSuccess : eItemCacheInsertFail;
	}

	int GLPrivateMarketItemCache::DeleteItem( UUID& sItemGUID )
	{
		std::string strGUID = sc::string::uuidToString( sItemGUID );
		std::string strItemCacheDeleteQuery( 
			sc::string::format( 
			"\
			DELETE FROM ItemCache \
			WHERE GUID = '%1%';", 
			strGUID.c_str() ) );

		int nSaleItemDeleteResult = execute( strItemCacheDeleteQuery );
		if ( SQLITE_OK != nSaleItemDeleteResult )
		{
			return eSaleItemDeleteFail;
		}

		bool bDelete = false;
		if ( m_pSaleItem )
		{
			bDelete = m_pSaleItem->DeleteSaleItem( strGUID, 0, 0 );
		}

		return bDelete == true ? eSuccess : eItemCacheDeleteFail;
	}

	void GLPrivateMarketItemCache::Reset()
	{
		m_sSearchBasic = sSaleItemSearchandSortBasic();
		m_sSearchName = sSaleItemSearchandSortName();
		m_pSaleItem->DeleteSaleItemAll();

		std::string strItemCacheDeleteQuery( "DELETE FROM ItemCache;" );
		execute( strItemCacheDeleteQuery );
	}

	int GLPrivateMarketItemCache::openDatabase()
	{
		int nResult = m_theCache.Open( ":memory:" );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( "GLPrivateMarketItemCache openDatabase m_theCache critical error: %1%", m_theCache.GetLastError() ) );
			sc::ErrorMessageBox( strMessage, false );
			return nResult;
		}

		return createTable();
	}

	void GLPrivateMarketItemCache::closeDatabase()
	{
		if ( m_theCache.IsOpen() )
		{
			m_theCache.Close();
		}
	}

	int GLPrivateMarketItemCache::createTable()
	{
		std::string strItemCacheCreateQuery(
			"\
			DROP TABLE IF EXISTS ItemCache; \
			CREATE TABLE ItemCache( \
			ChaNum INTEGER NOT NULL, \
			ChaName TEXT NOT NULL, \
			MapMainID INTEGER NOT NULL, \
			MapSubID INTEGER NOT NULL, \
			Price INTEGER NOT NULL, \
			TurnNum INTEGER NOT NULL, \
			GUID TEXT NOT NULL, \
			Page INTEGER NOT NULL, \
            SellType INTEGER NOT NULL, \
			PRIMARY KEY( GUID ) ); \
			CREATE UNIQUE INDEX Idx_ItemCache_GUID ON ItemCache ( GUID ); \
			CREATE INDEX Idx_ItemCache_Page ON ItemCache ( Page );" );

		int nResult = execute( strItemCacheCreateQuery );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( 
				"GLPrivateMarketItemCache createTable error expression : %1% error : %2%", 
				strItemCacheCreateQuery.c_str(), m_theCache.GetLastError() ) );
			sc::ErrorMessageBox( strMessage, false );
			return nResult;
		}

		return nResult;
	}

	int GLPrivateMarketItemCache::execute( const std::string& strQuery )
	{
		if ( strQuery.empty() )
		{
			return eStringEmpty;
		}

		int nResult = m_theCache.ExecuteSQL( strQuery );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( 
				"GLPrivateMarketItemCache execute error expression : %1% error : %2%", 
				strQuery.c_str(), m_theCache.GetLastError() ) );
			sc::writeLogError( strMessage );
		}

		return nResult;
	}

	WORD GLPrivateMarketItemCache::getPage()
	{
		WORD wPage = 1;
		if ( eSearchTypeBasic == m_eCurrentSearchType )
		{
			wPage = m_sSearchBasic.wPage;
		}
		else if ( eSearchTypeName == m_eCurrentSearchType )
		{
			wPage = m_sSearchName.wPage;
		}

		return wPage;
	}
}
