#include "pch.h"

#include <map>
#include <boost/timer.hpp>

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/ServiceProviderDefine.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/File/FileUtil.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Market/MarketDefine.h"
#include "../../RanLogic/Market/GLSaleItem.h"
#include "../../RanLogic/GLStringTable.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "./GLPrivateMarketSearchSQLite.h"

#include "../../SigmaCore/DebugInclude.h"
#include "../../SigmaCore/String/StringUtil.h"

namespace private_market
{
	const DWORD g_FrameMoveDelay = 5000;
	const int g_HoldItemDeleteSecond = 15;

    const char* g_szConsignmentSaleItemSearchQuery = "\
                                          SELECT \
                                            SaleItems.SellerChaNum, \
                                            SaleItems.TurnNum, \
                                            SaleItems.Price, \
                                            SaleItems.GUID, \
                                            SaleItems.ConsigmentSallerName, \
                                            SaleItems.SellType, \
                                            SaleItems.ExpirationDate, \
                                            Sellers.ChaName, \
                                            Sellers.MapMainID, \
                                            Sellers.MapSubID \
                                          FROM SaleItems LEFT OUTER JOIN Sellers \
                                          ON SaleItems.SellerChaNum = Sellers.ChaNum AND SaleItems.ServerGroup = Sellers.ServerGroup";
                                          
    const char* g_szConsignmentSaleItemCountQuery = "\
                                         SELECT COUNT( SaleItems.GUID ) \
                                         FROM SaleItems LEFT OUTER JOIN Sellers \
                                         ON SaleItems.SellerChaNum = Sellers.ChaNum AND SaleItems.ServerGroup = Sellers.ServerGroup";

	GLPrivateMarketSearchSQLite::GLPrivateMarketSearchSQLite()
	: m_bChannelCheck( false )
	, m_bSchoolCheck( false )
	, m_bAllPage( false )
	, m_wSearchItemNumberPerPage( 8 )
	, m_wLogNumberPerPage( 10 )
	, m_wLogNumberMaxPerCharacter( 80 )
	, m_dwFrameMoveTimer( 0 )
	, m_bTriggerSaleLogBackup( TRUE )
	, m_nSearchOption( 1 )
	{
		openDatabase();
		m_pSaleItem = new GLSaleItem();
		m_pLastLogNum = new GLLastLogNumMgr();

		m_dwSaleLogFrameMoveTimer = timeGetTime();
	}

	GLPrivateMarketSearchSQLite::~GLPrivateMarketSearchSQLite()
	{
		closeDatabase();
		SAFE_DELETE( m_pSaleItem );
		SAFE_DELETE( m_pLastLogNum );
	}

	int GLPrivateMarketSearchSQLite::InsertSeller( 
		DWORD dwChaDbNum, 
		const char* szChaName, 
		const SNATIVEID& sMapID, 
		WORD wSchool, 
		int nChannel, 
		DWORD dwFieldSlot, 
		int nServerGroup )
	{
		int nResult = SQLITE_OK;
		std::string strSellerInsertQuery( 
			sc::string::format( 
			"\
			INSERT INTO Sellers( \
			ChaNum, ChaName, MapMainID, MapSubID, School, Channel, FieldSlot, ServerGroup ) \
			VALUES( %1%, '%2%', %3%, %4%, %5%, %6%, %7%, %8% );", 
			dwChaDbNum, szChaName, sMapID.wMainID, sMapID.wSubID, wSchool, nChannel, dwFieldSlot, nServerGroup ) );

		nResult = execute( strSellerInsertQuery, false );
		if ( SQLITE_OK != nResult )
		{
			std::string strSellerUpdateQuery( 
				sc::string::format( 
				"\
				UPDATE Sellers \
				SET ChaName = '%1%', MapMainID = %2%, MapSubID = %3%, School = %4%, Channel = %5%, FieldSlot = %6%, ServerGroup = %7% \
				WHERE ChaNum = %8%;", 
				szChaName, sMapID.wMainID, sMapID.wSubID, wSchool, nChannel, dwFieldSlot, nServerGroup, dwChaDbNum ) );
			
			nResult = execute( strSellerUpdateQuery );
		}

		return nResult;
	}

	int GLPrivateMarketSearchSQLite::InsertSaleItem( 
		DWORD dwChaDbNum, 
		UUID& sItemGUID, 
		const SITEMCUSTOM& sItemcustom,
		const DWORD dwTurnNum, 
		const LONGLONG& llPrice,
		const float fCommissionRate,
        __time64_t ExpirationDate, 
		int nServerGroup )
	{
		// 판매자 정보
		std::string strSellerSelectQuery( 
			sc::string::format( 
			"\
			SELECT ChaName, MapMainID, MapSubID, School, Channel, FieldSlot \
			FROM Sellers \
			WHERE ChaNum = %1% AND ServerGroup = %2%;", 
			dwChaDbNum, nServerGroup ) );

		SQLite::TablePtr pSellersTable( m_SaleItem.QuerySQL2( strSellerSelectQuery.c_str() ) );
		if ( !pSellersTable.m_pTable )
		{
			return eNullTablePointer;
		}

		SQLite::Table& rSellersTable = pSellersTable();
		int nRowCount = rSellersTable.GetRowCount();

        // 판매자를 검사한다.
		if ( nRowCount != 1 )
		{
			return eNotExistSeller;
		}

		std::string strChaName = rSellersTable.GetValue< std::string >( 0 );
		WORD wMapMainID = rSellersTable.GetValue< WORD >( 1 );
		WORD wMapSubID = rSellersTable.GetValue< WORD >( 2 );
		WORD wSchool = rSellersTable.GetValue< WORD >( 3 );
		int nChannel = rSellersTable.GetValue< int >( 4 );
		DWORD dwFieldSlot = rSellersTable.GetValue< DWORD >( 5 );

        return InsertSaleItem( 
            strChaName, wMapMainID, wMapSubID, wSchool, nChannel, 
            dwFieldSlot, dwChaDbNum, sItemGUID, sItemcustom, dwTurnNum, 
            llPrice, fCommissionRate, eSellType_PrivateMarket, strChaName, ExpirationDate, nServerGroup );

	}

    int GLPrivateMarketSearchSQLite::InsertConsigmentSaleItem( 
        std::string strChaName,
        DWORD dwChaDbNum, 
        UUID& sItemGUID, 
        const SITEMCUSTOM& sItemcustom, 
        const DWORD dwTurnNum, 
        const LONGLONG& llPrice, 
        const float fCommissionRate, 
        eSellType emSellType, 
        __time64_t ExpirationDate, 
		int nServerGroup )
    {
		/*
		std::string strChaName, 
		WORD wMapMainID, 
		WORD wMapSubID, 
		WORD wSchool, 
		int nChannel, 
		DWORD dwFieldSlot, 
		DWORD dwChaDbNum, 
		UUID& sItemGUID, 
		const SITEMCUSTOM& sItemcustom, 
		const DWORD dwTurnNum, 
		const LONGLONG& llPrice, 
		const float fCommissionRate, 
		eSellType emSellType, 
		std::string strConsignmentSalllerName,
		__time64_t ExpirationDate, 
		int nServerGroup
		*/

        return InsertSaleItem( 
            strChaName, 0, 0, 0, 0, 
            0, dwChaDbNum, sItemGUID, sItemcustom, dwTurnNum,
            llPrice, fCommissionRate, emSellType, strChaName, ExpirationDate, nServerGroup );
	}

    int GLPrivateMarketSearchSQLite::DeleteItem( UUID& sItemGUID )
    {
		std::string strGUID = sc::string::uuidToString( sItemGUID );

		// 판매자 가져오기
		DWORD dwSellerChaDbNum = 0;
		int nServerGroup = 0;
		{
			std::string strSaleItemsSelectQuery = 
				sc::string::format( "\
									SELECT SellerChaNum, ServerGroup \
									FROM SaleItems \
									WHERE GUID = '%1%';", 
									strGUID.c_str() );

			SQLite::TablePtr pSaleItemsTable( m_SaleItem.QuerySQL2( strSaleItemsSelectQuery.c_str() ) );
			if ( pSaleItemsTable.m_pTable )
			{
				SQLite::Table& rSaleItemsTable = pSaleItemsTable();
				int nSaleItemsRowCount = rSaleItemsTable.GetRowCount();
				if ( 0 != nSaleItemsRowCount )
				{
					dwSellerChaDbNum = rSaleItemsTable.GetValue< DWORD >( 0 );
					nServerGroup = rSaleItemsTable.GetValue< int >( 1 );
				}
			}
		}

        std::string strSaleItemDeleteQuery( 
            sc::string::format( 
            "\
            DELETE FROM SaleItems \
            WHERE GUID = '%1%';", 
            strGUID.c_str() ) );

        int nSaleItemDeleteResult = execute( strSaleItemDeleteQuery );
        if ( SQLITE_OK != nSaleItemDeleteResult )
        {
            return eSaleItemDeleteFail;
        }

		// 아이템 삭제
		m_pSaleItem->DeleteSaleItem( strGUID, dwSellerChaDbNum, nServerGroup );

        return eSuccess;
    }

	int GLPrivateMarketSearchSQLite::DeleteItem( DWORD dwChaDbNum, int nServerGroup )
	{
		std::string strSaleItemDeleteQuery( 
			sc::string::format( 
			"\
			DELETE FROM SaleItems \
			WHERE SellerChaNum = %1% and SellType = %2% and ServerGroup = %3%;", 
			dwChaDbNum, eSellType_PrivateMarket, nServerGroup ) );

		int nSaleItemDeleteResult = execute( strSaleItemDeleteQuery );
		if ( SQLITE_OK != nSaleItemDeleteResult )
		{
			return eSaleItemDeleteFail;
		}

		std::string strHoldItemDeleteQuery( 
			sc::string::format( 
			"\
			DELETE FROM HoldItems \
			WHERE ( SellerChaNum = %1% AND SellerServerGroup = %2% ) OR ( BuyerChaNum = %3% AND BuyerServerGroup = %4% );", 
			dwChaDbNum, nServerGroup, dwChaDbNum, nServerGroup ) );

		int nHoldItemDeleteResult = execute( strHoldItemDeleteQuery );
		if ( SQLITE_OK != nHoldItemDeleteResult )
		{
			return eHoldItemDeleteFail;
		}

		if ( m_pLastLogNum )
		{
			m_pLastLogNum->DeleteLastLogNum( dwChaDbNum, nServerGroup );
		}

		if ( m_pSaleItem )
		{
			m_pSaleItem->DeleteSaleItem( dwChaDbNum, nServerGroup );
		}

		// 판매자 삭제
		std::string strSellerDeleteQuery( 
			sc::string::format( 
			"\
			DELETE FROM Sellers \
			WHERE ChaNum = %1% AND ServerGroup = %2%;", 
			dwChaDbNum, nServerGroup ) );
		execute( strSellerDeleteQuery );

		return eSuccess;
	}

	int GLPrivateMarketSearchSQLite::GetSaleItems( 
		WORD wPage, 
		int nChannel, 
		WORD wSchool, 
		sSaleItemSearchandSortBasic& sSearch, 
		std::vector< sSaleItem >& vecSaleItems, 
		WORD& wTotalCount, 
		int nServerGroup )
	{
        WORD wCount = 0;
		boost::timer SaleItemSelectTimer;

        std::string strQuery;
 		std::string strCountQuery( "" );

        // 위탁판매 검색용
        CleanUpConsigmentItem(); //검색전에 정리한다.

        strCountQuery.clear();

        strQuery = makeSearchQuery( wPage, nChannel, wSchool, sSearch, strCountQuery, nServerGroup, true );
        {
            {
                if ( 1 == wPage )
                {
                    SQLite::TablePtr pSaleItemsTable( m_SaleItem.QuerySQL2( strCountQuery.c_str() ) );
                    if ( !pSaleItemsTable.m_pTable )
                    {
                        return eNullTablePointer;
                    }

					SQLite::Table& rSaleItemsTable = pSaleItemsTable();
					if ( rSaleItemsTable.GetRowCount() == 0 )
					{
						return eNotExistSaleItem;
					}

                    wCount += rSaleItemsTable.GetValue< WORD >( 0 );
                }
            }
        }

        selectConsignmentSale( strQuery, vecSaleItems );

		double fElapsed = SaleItemSelectTimer.elapsed();
		if ( fElapsed > 1.0f )
		{
			sc::writeLogWarn( sc::string::format( "GetSaleItems taken %1% second.", fElapsed ) );
		}

        wTotalCount = wCount;

		return eSuccess;
	}

	int GLPrivateMarketSearchSQLite::GetSaleItems( 
		WORD wPage, 
		int nChannel, 
		WORD wSchool, 
		std::string& strItemName, 
		int nSortOption, 
		int nSortMethod, 
		std::vector< sSaleItem >& vecSaleItems, 
		WORD& wTotalCount, 
		int nServerGroup )
	{
        WORD wCount = 0;

		boost::timer SaleItemSelectTimer;

		if ( strItemName.empty() )
		{
			return eNotExistSaleItem;
		}

		// 사용자가 보낸 문자열을 검사하도록 ' 문자열이 있으면 sql 구문 에러가 발생한다.
		sc::string::replaceForDb( strItemName );
		
        std::string strSaleItemSearchQueryTemp( g_szConsignmentSaleItemSearchQuery );

        // 위탁판매 검색
        {
            CleanUpConsigmentItem();
            if ( 1 == wPage )
            {
				// 이거 필요
                std::string strCountQueryTemp( g_szConsignmentSaleItemCountQuery );
                strCountQueryTemp += " WHERE Name LIKE '%%%1%%%'";
                strCountQueryTemp += sc::string::format( " AND is_searchable( SaleItems.SellType, SaleItems.ServerGroup, %1%, %2% ) = 1", nServerGroup, m_nSearchOption );
                std::string strCountQuery( sc::string::format( strCountQueryTemp, strItemName.c_str() ) );
				strCountQuery += ";";

                SQLite::TablePtr pSaleItemsTable( m_SaleItem.QuerySQL2( strCountQuery.c_str() ) );
                if ( !pSaleItemsTable.m_pTable )
                {
                    return eNullTablePointer;
                }

                SQLite::Table& rSaleItemsTable = pSaleItemsTable();
                if ( rSaleItemsTable.GetRowCount() == 0 )
                {
                    return eNotExistSaleItem;
                }

                wCount += rSaleItemsTable.GetValue< WORD >( 0 );
            }
        }

        strSaleItemSearchQueryTemp += " WHERE Name LIKE '%%%1%%%'";
		strSaleItemSearchQueryTemp += sc::string::format( " AND is_searchable( SaleItems.SellType, SaleItems.ServerGroup, %1%, %2% ) = 1", nServerGroup, m_nSearchOption );
        strSaleItemSearchQueryTemp += getOrderBy( nSortOption, nSortMethod );

        if ( false == m_bAllPage )
        {
            strSaleItemSearchQueryTemp += getLimit( wPage, m_wSearchItemNumberPerPage );
        }

        strSaleItemSearchQueryTemp += ";";

        std::string strSaleItemSearchQuery;
        strSaleItemSearchQuery = std::string( sc::string::format( strSaleItemSearchQueryTemp, strItemName.c_str() ) );
        selectConsignmentSale( strSaleItemSearchQuery, vecSaleItems );

        wTotalCount = wCount;
		return eSuccess;
	}

	static void canEquipFunc( sqlite3_context* ctx, int argc, sqlite3_value** argv )
	{
		DWORD dwItemValue = static_cast< DWORD >( sqlite3_value_int( argv[0] ) );
		int nClass = sqlite3_value_int( argv[1] );
		int nSex = sqlite3_value_int( argv[2] );
		int nResult = 0;

		switch ( nClass )
		{
		case eItemRequireClassAll:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						nResult = 1;
					}
					break;
				case eItemRequireSexMan:
					{
						if ( 
							( dwItemValue & GLCC_FIGHTER_M ) || 
							( dwItemValue & GLCC_ARMS_M ) || 
							( dwItemValue & GLCC_ARCHER_M ) || 
							( dwItemValue & GLCC_SPIRIT_M ) || 
							( dwItemValue & GLCC_EXTREME_M ) || 
							( dwItemValue & GLCC_SCIENTIST_M ) || 
							( dwItemValue & GLCC_ASSASSIN_M ) ||
							( dwItemValue & GLCC_TRICKER_M ) ||
							( dwItemValue & GLCC_ACTOR_M)	)
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( 
							( dwItemValue & GLCC_FIGHTER_W ) || 
							( dwItemValue & GLCC_ARMS_W ) || 
							( dwItemValue & GLCC_ARCHER_W ) || 
							( dwItemValue & GLCC_SPIRIT_W ) || 
							( dwItemValue & GLCC_EXTREME_W ) || 
							( dwItemValue & GLCC_SCIENTIST_W ) || 
							( dwItemValue & GLCC_ASSASSIN_W ) ||
							( dwItemValue & GLCC_TRICKER_W ) ||
							( dwItemValue & GLCC_ACTOR_W )
							)
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		case eItemRequireClassFighter:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						if ( dwItemValue & GLCC_FIGHTER )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexMan:
					{
						if ( dwItemValue & GLCC_FIGHTER_M )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( dwItemValue & GLCC_FIGHTER_W )
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		case eItemRequireClassArms:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						if ( dwItemValue & GLCC_ARMS )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexMan:
					{
						if ( dwItemValue & GLCC_ARMS_M )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( dwItemValue & GLCC_ARMS_W )
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		case eItemRequireClassArcher:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						if ( dwItemValue & GLCC_ARCHER )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexMan:
					{
						if ( dwItemValue & GLCC_ARCHER_M )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( dwItemValue & GLCC_ARCHER_W )
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		case eItemRequireClassSpirit:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						if ( dwItemValue & GLCC_SPIRIT )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexMan:
					{
						if ( dwItemValue & GLCC_SPIRIT_M )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( dwItemValue & GLCC_SPIRIT_W )
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		case eItemRequireClassExtreme:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						if ( dwItemValue & GLCC_EXTREME )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexMan:
					{
						if ( dwItemValue & GLCC_EXTREME_M )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( dwItemValue & GLCC_EXTREME_W )
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		case eItemRequireClassScientist:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						if ( dwItemValue & GLCC_SCIENTIST )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexMan:
					{
						if ( dwItemValue & GLCC_SCIENTIST_M )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( dwItemValue & GLCC_SCIENTIST_W )
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		case eItemRequireClassAssassin:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						if ( dwItemValue & GLCC_ASSASSIN )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexMan:
					{
						if ( dwItemValue & GLCC_ASSASSIN_M )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( dwItemValue & GLCC_ASSASSIN_W )
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		case eItemRequireClassTricker:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						if ( dwItemValue & GLCC_TRICKER )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexMan:
					{
						if ( dwItemValue & GLCC_TRICKER_M )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( dwItemValue & GLCC_TRICKER_W )
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		case eItemRequireClassActor:
			{
				switch ( nSex )
				{
				case eItemRequireSexAll:
					{
						if ( dwItemValue & GLCC_ACTOR )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexMan:
					{
						if ( dwItemValue & GLCC_ACTOR_M )
						{
							nResult = 1;
						}
					}
					break;
				case eItemRequireSexWoman:
					{
						if ( dwItemValue & GLCC_ACTOR_W )
						{
							nResult = 1;
						}
					}
					break;
				}
			}
			break;
		}

		// context에 결과값 리턴
		sqlite3_result_int( ctx, nResult );
	}

	static void IsSearchableFunc( sqlite3_context* ctx, int argc, sqlite3_value** argv )
	{
		int nSellType = sqlite3_value_int( argv[0] );
		int nSellerServerGroup = sqlite3_value_int( argv[1] );
		int nBuyerServerGroup = sqlite3_value_int( argv[2] );
		int nSearchOption = sqlite3_value_int( argv[3] );
		int nResult = 0;

		if ( nSellerServerGroup == nBuyerServerGroup )
		{
			nResult = 1;
		}

		// nResult 가 1이면 검색가능해진다.
		switch ( nSearchOption )
		{
		case 0:
			{
				// 다른 서버 그룹의 일반 거래, 포인트 거래 모두 검색 가능
				if ( nSellerServerGroup != nBuyerServerGroup )
				{
					nResult = 1;
				}
			}
			break;
		case 1:
			{
				// 다른 서버 그룹의 포인트 거래 검색 가능
				if ( eSellType_ConsignmentSale_RanPoint == nSellType && nSellerServerGroup != nBuyerServerGroup )
				{
					nResult = 1;
				}
			}
			break;
		case 2:
			{
				// 다른 서버 그룹의 아이템 검색 불가
			}
			break;
		}

		// context에 결과값 리턴
		sqlite3_result_int( ctx, nResult );
	}

	static void IsSearchableServerGroupFunc( sqlite3_context* ctx, int argc, sqlite3_value** argv )
	{
		int nSellerServerGroup = sqlite3_value_int( argv[0] );
		int nBuyerServerGroup = sqlite3_value_int( argv[1] );
		int nSearchOption = sqlite3_value_int( argv[2] );
		int nResult = 0;

		if ( nSellerServerGroup == nBuyerServerGroup )
		{
			nResult = 1;
		}

		// nResult 가 1이면 검색가능해진다.
		switch ( nSearchOption )
		{
		case 0:
			{
				// 다른 서버 그룹의 일반 거래, 포인트 거래 모두 검색 가능
				if ( nSellerServerGroup != nBuyerServerGroup )
				{
					nResult = 1;
				}
			}
			break;
		case 1:
			{
				// 다른 서버 그룹의 포인트 거래 검색 가능
			}
			break;
		case 2:
			{
				// 다른 서버 그룹의 아이템 검색 불가
			}
			break;
		}

		// context에 결과값 리턴
		sqlite3_result_int( ctx, nResult );
	}

	static void IsSearchableSellTypeFunc( sqlite3_context* ctx, int argc, sqlite3_value** argv )
	{
		int nSellType = sqlite3_value_int( argv[0] );
		int nSearchOption = sqlite3_value_int( argv[1] );
		int nResult = 0;

		// nResult 가 1이면 검색가능해진다.
		switch ( nSearchOption )
		{
		case 0:
			{
				// 다른 서버 그룹의 일반 거래, 포인트 거래 모두 검색 가능
				nResult = 1;
			}
			break;
		case 1:
			{
				// 다른 서버 그룹의 포인트 거래 검색 가능
				if ( eSellType_ConsignmentSale_RanPoint == nSellType )
				{
					nResult = 1;
				}
			}
			break;
		case 2:
			{
				// 다른 서버 그룹의 아이템 검색 불가
			}
			break;
		}

		// context에 결과값 리턴
		sqlite3_result_int( ctx, nResult );
	}

    int GLPrivateMarketSearchSQLite::PrepareItemBuy( 
        UUID& sItemGUID,
        DWORD dwItemTurnNum, 
        DWORD dwFieldSlot, 
        DWORD dwChaDbNum, 
        const char* szName, 
        LONGLONG& llPriceTotal OUT,
        int& nSellType OUT, 
		int nServerGroup )
    {

        std::string strBuyerName( "empty" );
        if ( NULL != szName )
        {
            strBuyerName = szName;
        }

        // 한번에 하나씩 거래 가능
        std::string strHoldItemsBuyerSelectQuery( 
            sc::string::format( 
            "\
            SELECT SellerChaNum \
            FROM HoldItems \
            WHERE BuyerChaNum = %1% AND BuyerServerGroup = %2%;", 
            dwChaDbNum, nServerGroup ) );

        SQLite::TablePtr pHoldItemsBuyerTable( m_SaleItem.QuerySQL2( strHoldItemsBuyerSelectQuery.c_str() ) );
        if ( !pHoldItemsBuyerTable.m_pTable )
        {
            return eNullTablePointer;
        }

        SQLite::Table& rHoldItemsBuyerTable = pHoldItemsBuyerTable();
        int nHoldItemsBuyerRowCount = rHoldItemsBuyerTable.GetRowCount();
        if ( nHoldItemsBuyerRowCount > 0 )
        {
            // 이미 거래중이다.
            return eAlreadyBuy;
        }

        // 거래중인 아이템인가?
        std::string strGUID = sc::string::uuidToString( sItemGUID );
        std::string strHoldItemsSelectQuery( 
            sc::string::format( 
            "\
            SELECT SellerChaNum \
            FROM HoldItems \
            WHERE GUID = '%1%';", 
            strGUID.c_str() ) );

        SQLite::TablePtr pHoldItemsTable( m_SaleItem.QuerySQL2( strHoldItemsSelectQuery.c_str() ) );
        if ( !pHoldItemsTable.m_pTable )
        {
            return eNullTablePointer;
        }

        SQLite::Table& rHoldItemsTable = pHoldItemsTable();
        int nHoldItemsRowCount = rHoldItemsTable.GetRowCount();
        if ( nHoldItemsRowCount > 0 )
        {
            // 이미 누군가 거래중인 아이템이다.
            return eIsHoldItem;
        }
			
		// 구매할 수 있는 아이템인가? 패킷 조작으로 구매할 수 있는 것 방지
		std::string strValidateQuery( 
			sc::string::format( 
			"\
			SELECT SellerChaNum \
			FROM SaleItems \
			WHERE GUID = '%1%' AND is_searchable( SaleItems.SellType, SaleItems.ServerGroup, %2%, %3% ) = 1;", 
			strGUID.c_str(), nServerGroup, m_nSearchOption ) );

		SQLite::TablePtr pValidateSaleItemsTable( m_SaleItem.QuerySQL2( strValidateQuery.c_str() ) );
		if ( !pValidateSaleItemsTable.m_pTable )
		{
			return eNullTablePointer;
		}

		SQLite::Table& rValidateSaleItemsTable = pValidateSaleItemsTable();
		int nValidateSaleItemsRowCount = rValidateSaleItemsTable.GetRowCount();
		if ( nValidateSaleItemsRowCount == 0 )
		{
			// 구매할 수 없는 아이템이다.
			return eWrongItem;
		}

        // 판매타입 갖고온다.
        std::string strSaleItemsSelectQuery;  
        int nError = GetSellType( sItemGUID, nSellType );
        if( nError != eSuccess )
            return nError;

        if( nSellType == eSellType_PrivateMarket )
        {
            // 기존 쿼리
            strSaleItemsSelectQuery = 
                sc::string::format( "\
                SELECT SaleItems.SellerChaNum, SaleItems.TurnNum, SaleItems.Price, Sellers.Channel, Sellers.FieldSlot,\
                SaleItems.CommissionRate, Sellers.ChaName, Sellers.ServerGroup \
                FROM SaleItems INNER JOIN Sellers \
				ON ( SaleItems.SellerChaNum = Sellers.ChaNum AND SaleItems.ServerGroup = Sellers.ServerGroup ) \
                WHERE GUID = '%1%';", 
                strGUID.c_str() );
        }
        else
        {
            // 위탁판매의 경우 판매자가 없기때문에 쿼리가 다르다.
            strSaleItemsSelectQuery = 
                sc::string::format( "\
                SELECT SellerChaNum, TurnNum, Price, 0, 0,\
                CommissionRate, ConsigmentSallerName, ServerGroup \
                FROM SaleItems \
                WHERE GUID = '%1%';", 
                strGUID.c_str() );
        }

        SQLite::TablePtr pSaleItemsTable( m_SaleItem.QuerySQL2( strSaleItemsSelectQuery.c_str() ) );
        if ( !pSaleItemsTable.m_pTable )
        {
            return eNullTablePointer;
        }

        SQLite::Table& rSaleItemsTable = pSaleItemsTable();
        int nSaleItemsRowCount = rSaleItemsTable.GetRowCount();
        if ( 0 == nSaleItemsRowCount )
        {
            // 아이템이 없다.
            return eNotExistSaleItem;
        }

        DWORD dwRemainItemTurnNum = rSaleItemsTable.GetValue< DWORD >( 1 );

        // 구매하려고 하는 개수만큼 아이템이 있나?
        if ( dwItemTurnNum > dwRemainItemTurnNum )
        {
            return eItemLack;
        }

        DWORD dwSellerChaDbNum = rSaleItemsTable.GetValue< DWORD >( 0 );
        if ( dwChaDbNum == dwSellerChaDbNum )
        {
            return eCantBuyMyItem;
        }

        LONGLONG llItemPrice = rSaleItemsTable.GetValue< LONGLONG >( 2 );
        int nSellerChannel = rSaleItemsTable.GetValue< int >( 3 );
        DWORD dwSellerFieldSlot = rSaleItemsTable.GetValue< DWORD >( 4 );
        float fCommissionRate = rSaleItemsTable.GetValue< float >( 5 );
        std::string SellerName = rSaleItemsTable.GetValue< std::string >( 6 );
		int nSellerServerGroup = rSaleItemsTable.GetValue< int >( 7 );

		/*
        // ANSI코드->유니코드->UTF8 로 변경 해야 바인드 해줄 때 깨지지 않는다.
        std::string strBuyerNameUtf8( "default" );    
        strBuyerNameUtf8 = sc::string::ansi_to_utf8( strBuyerName );
		*/

        // Hold 시킨다.
        std::string strHoldItemInsertQuery( 
            sc::string::format( 
            "\
            INSERT INTO HoldItems( \
            GUID, SellerChaNum, SellerFieldSlot, SellerChannel, TurnNum, \
            Price, BuyerChaNum, BuyerFieldSlot, BuyerChaName, CommissionRate, \
            SellType, ConsigmentSallerName, SellerServerGroup, BuyerServerGroup ) \
            VALUES( '%1%', %2%, %3%, %4%, %5%, %6%, %7%, %8%, '%9%', %10%, %11%, '%12%', %13%, %14% );", 
            strGUID, dwSellerChaDbNum, dwSellerFieldSlot, nSellerChannel, dwItemTurnNum, 
            llItemPrice, dwChaDbNum, dwFieldSlot, strBuyerName.c_str(), fCommissionRate,
            nSellType, SellerName.c_str(), nSellerServerGroup, nServerGroup ) );

        int nHoldItemInsertResult = execute( strHoldItemInsertQuery );
        if ( SQLITE_OK != nHoldItemInsertResult )
        {
            return eHoldItemInsertFail;
        }

        llPriceTotal = llItemPrice * dwItemTurnNum;

        return eSuccess;
    }

	int GLPrivateMarketSearchSQLite::DeleteHoldItem( UUID& sItemGUID )
	{
		std::string strGUID = sc::string::uuidToString( sItemGUID );
		std::string strHoldItemDeleteQuery( 
			sc::string::format( 
			"\
			DELETE FROM HoldItems \
			WHERE GUID = '%1%';", 
			strGUID.c_str() ) );

		int nHoldItemDeleteResult = execute( strHoldItemDeleteQuery );
		if ( SQLITE_OK != nHoldItemDeleteResult )
		{
			return eHoldItemDeleteFail;
		}

		return eSuccess;
	}

	int GLPrivateMarketSearchSQLite::GetHoldItem( UUID& sItemGUID, sHoldItem& sItem )
	{
		/*
		std::string strHoldItemsCreateQuery(
		"\
		CREATE TABLE HoldItems( \
		GUID TEXT NOT NULL, \
		SellerChaNum INTEGER NOT NULL, \
		SellerFieldSlot INTEGER NOT NULL, \
		SellerChannel INTEGER NOT NULL, \
		SellerServerGroup INTEGER NOT NULL, \
		TurnNum INTEGER NOT NULL, \
		Price INTEGER NOT NULL, \
		BuyerChaNum INTEGER NOT NULL, \
		BuyerFieldSlot INTEGER NOT NULL, \
		BuyerChaName TEXT NOT NULL, \
		BuyerServerGroup INTEGER NOT NULL, \
		InsertDate DATE NOT NULL DEFAULT( datetime( 'now', 'localtime' ) ), \
		CommissionRate FLOAT NOT NULL, \
		SellType INTEGER NOT NULL, \
		ConsigmentSallerName TEXT NOT NULL, \
		PRIMARY KEY( GUID ) ); \
		CREATE UNIQUE INDEX Idx_HoldItems_GUID ON HoldItems ( GUID ); \
		CREATE INDEX Idx_HoldItems_SellerChaNum  ON HoldItems ( SellerChaNum ); \
		CREATE INDEX Idx_HoldItems_BuyerChaNum  ON HoldItems ( BuyerChaNum );" );
		*/

		std::string strGUID = sc::string::uuidToString( sItemGUID );
		std::string strHoldItemsSelectQuery( 
			sc::string::format( 
			"\
			SELECT \
			SellerChaNum, SellerFieldSlot, SellerChannel, TurnNum, Price, \
			BuyerChaNum, BuyerFieldSlot, SellerServerGroup, BuyerServerGroup \
			FROM HoldItems \
			WHERE GUID = '%1%';", 
			strGUID.c_str() ) );

		SQLite::TablePtr pHoldItemsTable( m_SaleItem.QuerySQL2( strHoldItemsSelectQuery.c_str() ) );
		if ( !pHoldItemsTable.m_pTable )
		{
			return eNullTablePointer;
		}

		SQLite::Table& rHoldItemsTable = pHoldItemsTable();
		int nHoldItemsRowCount = rHoldItemsTable.GetRowCount();
		if ( 0 == nHoldItemsRowCount )
		{
			return eNotExistHoldItem;
		}

		sItem.dwSellerChaDbNum = rHoldItemsTable.GetValue< DWORD >( 0 );
		sItem.dwSellerFieldSlot = rHoldItemsTable.GetValue< DWORD >( 1 );
		sItem.nSellerChannel = rHoldItemsTable.GetValue< int >( 2 );
		sItem.dwItemTurnNum = rHoldItemsTable.GetValue< DWORD >( 3 );
		sItem.llItemPrice = rHoldItemsTable.GetValue< LONGLONG >( 4 );
		sItem.dwBuyerChaDbNum = rHoldItemsTable.GetValue< DWORD >( 5 );
		sItem.dwBuyerFieldSlot = rHoldItemsTable.GetValue< DWORD >( 6 );
		sItem.nSellerServerGroup = rHoldItemsTable.GetValue< int >( 7 );
		sItem.nBuyerServerGroup = rHoldItemsTable.GetValue< int >( 8 );

		return eSuccess;
	}

	int GLPrivateMarketSearchSQLite::UpdateItem( UUID& sItemGUID, DWORD dwItemTurnNum )
	{
		std::string strGUID = sc::string::uuidToString( sItemGUID );
		std::string strSaleItemUpdateQuery( 
			sc::string::format( 
			"\
			UPDATE SaleItems \
			SET TurnNum = %1% \
			WHERE GUID = '%2%';", 
			dwItemTurnNum, 
			strGUID.c_str() ) );

		int nSaleItemUpdateResult = execute( strSaleItemUpdateQuery );
		if ( SQLITE_OK != nSaleItemUpdateResult )
		{
			return eSaleItemUpdateFail;
		}

		return eSuccess;
	}

	bool GLPrivateMarketSearchSQLite::IsHoldItem( UUID& sItemGUID )
	{
		std::string strGUID = sc::string::uuidToString( sItemGUID );
		std::string strHoldItemsSelectQuery( 
			sc::string::format( 
			"\
			SELECT SellerChaNum \
			FROM HoldItems \
			WHERE GUID = '%1%';", 
			strGUID.c_str() ) );

		SQLite::TablePtr pHoldItemsTable( m_SaleItem.QuerySQL2( strHoldItemsSelectQuery.c_str() ) );
		if ( !pHoldItemsTable.m_pTable )
		{
			return false;
		}

		SQLite::Table& rHoldItemsTable = pHoldItemsTable();
		int nHoldItemsRowCount = rHoldItemsTable.GetRowCount();
		if ( nHoldItemsRowCount > 0 )
		{
			// 이미 누군가 거래중인 아이템이다.
			return true;
		}

		return false;
	}

	int GLPrivateMarketSearchSQLite::InsertHoldItem( UUID& sItemGUID, DWORD dwFieldSlot, DWORD dwChaDbNum, const char* szName, int nServerGroup )
	{
        std::string strGUID = sc::string::uuidToString( sItemGUID );

        std::string strQuery( 
            sc::string::format( 
            "\
            SELECT SellType \
            FROM SaleItems \
            WHERE GUID = '%1%';", 
            strGUID.c_str() ) );

        SQLite::TablePtr pSaleItemsTable( m_SaleItem.QuerySQL2( strQuery.c_str() ) );
        if ( !pSaleItemsTable.m_pTable )
        {
            return eNullTablePointer;
        }

        SQLite::Table& rSaleItemsTable = pSaleItemsTable();
        int nSaleItemsRowCount = rSaleItemsTable.GetRowCount();
        if ( 0 == nSaleItemsRowCount )
        {
            // 아이템이 없다.
            return eNotExistSaleItem;
        }

        int nSellType = rSaleItemsTable.GetValue< int >( 0 );

        // 아이템이 존재하는가?
        if ( nSellType == eSellType_PrivateMarket )
        {
            // 기존 쿼리
            strQuery = sc::string::format( 
                "\
                SELECT SaleItems.SellerChaNum, SaleItems.TurnNum, SaleItems.Price, Sellers.Channel, Sellers.FieldSlot, \
                SaleItems.CommissionRate, Sellers.ChaName, SaleItems.ServerGroup \
                FROM SaleItems INNER JOIN Sellers \
                ON SaleItems.SellerChaNum = Sellers.ChaNum AND SaleItems.ServerGroup = Sellers.ServerGroup \
                WHERE GUID = '%1%';", 
                strGUID.c_str() );
        }
        else
        {
            // 위탁판매는 쿼리가 다르다
            strQuery = sc::string::format( 
                "\
                SELECT SellerChaNum, TurnNum, Price, 0, 0, \
                CommissionRate, ConsigmentSallerName, ServerGroup \
                FROM SaleItems \
                WHERE GUID = '%1%';", 
                strGUID.c_str() );
        }

        return InsertHoldItem( strGUID, dwFieldSlot, dwChaDbNum, szName, strQuery, nSellType, nServerGroup );

	}
    
	int GLPrivateMarketSearchSQLite::InsertSaleLog( UUID& sItemGUID )
	{
		spItemcustom spItem = m_pSaleItem->FindSaleItem( sItemGUID );
		if ( NULL == spItem )
		{
			return eSaleItemNULL;
		}

		std::string strGUID = sc::string::uuidToString( sItemGUID );
		std::string strHoldItemsSelectQuery( 
			sc::string::format( 
			"\
			SELECT \
			SellerChaNum, ConsigmentSallerName, BuyerChaNum, BuyerChaName, Price, \
			TurnNum, CommissionRate, SellType, SellerServerGroup, BuyerServerGroup \
            FROM HoldItems \
			WHERE GUID = '%1%';", 
			strGUID.c_str() ) );

		SQLite::TablePtr pHoldItemsTable( m_SaleItem.QuerySQL2( strHoldItemsSelectQuery.c_str() ) );
		if ( !pHoldItemsTable.m_pTable )
		{
			return eNullTablePointer;
		}

		SQLite::Table& rHoldItemsTable = pHoldItemsTable();
		int nHoldItemsRowCount = rHoldItemsTable.GetRowCount();
		if ( 0 == nHoldItemsRowCount )
		{
			return eNotExistHoldItem;
		}

		std::string strSellerChaName( "seller" ), strBuyerChaName( "buyer" );
		DWORD dwSellerChaDbNum = rHoldItemsTable.GetValue< DWORD >( 0 );
        strSellerChaName = rHoldItemsTable.GetValue< std::string >( 1 );		

		// 위탁판매 추가되면서 Inner Join이 Left Join으로 변경되어서 NULL 가능해짐.
		if ( strSellerChaName == "" || strSellerChaName.empty() == true )
			strSellerChaName = "Unknown";

		DWORD dwBuyerChaDbNum = rHoldItemsTable.GetValue< DWORD >( 2 );
		strBuyerChaName = rHoldItemsTable.GetValue< std::string >( 3 );
		LONGLONG llItemPrice = rHoldItemsTable.GetValue< LONGLONG >( 4 );
		DWORD dwItemTurnNum = rHoldItemsTable.GetValue< DWORD >( 5 );
		float fCommissionRate = rHoldItemsTable.GetValue< float >( 6 );
        int nSellType = rHoldItemsTable.GetValue< int >( 7 );
		int nSellerServerGroup = rHoldItemsTable.GetValue< int >( 8 );
		int nBuyerServerGroup = rHoldItemsTable.GetValue< int >( 9 );

		std::string strSaleLogInsertQuery( 
			sc::string::format( 
			"\
			INSERT INTO SaleLogV2( \
			SellerChaNum, SellerChaName, BuyerChaNum, BuyerChaName, ItemMainID, \
			ItemSubID, Money, TurnNum, CommissionRate, SellType, \
			SellerServerGroup, BuyerServerGroup ) \
			VALUES( %1%, '%2%', %3%, '%4%', %5%, %6%, %7%, %8%, %9%, %10%, %11%, %12% );", 
			dwSellerChaDbNum, strSellerChaName.c_str(), dwBuyerChaDbNum, strBuyerChaName.c_str(), spItem->GetBoxWrappedItemID().wMainID, 
			spItem->GetBoxWrappedItemID().wSubID, llItemPrice * dwItemTurnNum, dwItemTurnNum, fCommissionRate, nSellType, nSellerServerGroup, nBuyerServerGroup ) );

		int nSaleLogInsertResult = executeForSaleLog( strSaleLogInsertQuery );
		if ( SQLITE_OK != nSaleLogInsertResult )
		{
			return eSaleLogInsertFail;
		}

		return eSuccess;
	}

	void GLPrivateMarketSearchSQLite::FrameMove()
	{
		FrameMoveSaleLog();

		DWORD dwCurrentTime = ::GetTickCount();
		if ( ( dwCurrentTime - m_dwFrameMoveTimer ) < g_FrameMoveDelay )
			return;

		m_dwFrameMoveTimer = dwCurrentTime;

		// 홀드 아이템 삭제 용도
		deleteHoldItems();
	}

	int GLPrivateMarketSearchSQLite::GetSaleLog( DWORD dwChaDbNum, std::vector< sSaleLog >& vecSaleLog, int nServerGroup )
	{
		boost::timer SaleLogSelectTimer;

		// 마지막으로 가져간 로그 번호 가져오기
		LONGLONG llLastLogNum = m_pLastLogNum->GetLastLogNum( dwChaDbNum, nServerGroup );

		std::string strSaleLogSelectQuery( "\
										   SELECT \
										   SellerChaNum, SellerChaName, BuyerChaNum, BuyerChaName, ItemMainID, \
										   ItemSubID, Money, TurnNum, seq, strftime( '%H:%M', SaleDate ) AS Date, \
										   strftime( '%Y.%m.%d', SaleDate ) AS Hour, CommissionRate, SellType \
										   FROM SaleLogV2 " );

		/*
		// 원래 이거였는데 제한 없이 가져가는 거로 변경되어 있다.
		sc::string::format( 
		" WHERE ( SellerChaNum = %1% OR BuyerChaNum = %2% ) AND seq > %3% \
		ORDER BY seq DESC", 
		dwChaDbNum, dwChaDbNum, llLastLogNum ) );
		*/

		std::string strWhereContent( 
			sc::string::format( 
			" WHERE ( ( SellerChaNum = %1% AND SellerServerGroup = %2% ) OR ( BuyerChaNum = %3% AND BuyerServerGroup = %4% ) ) \
			order by SaleDate DESC ", 
			dwChaDbNum, nServerGroup, dwChaDbNum, nServerGroup ) );

		strSaleLogSelectQuery += strWhereContent;

		// 거래로그는 한번에 보내주고, 대신 딜레이를 길게 한다.
		strSaleLogSelectQuery += getLimit( 1, m_wLogNumberMaxPerCharacter );
		strSaleLogSelectQuery += ";";

		SQLite::TablePtr pSaleLogTable( m_SaleLog.QuerySQL2( strSaleLogSelectQuery.c_str() ) );
		if ( !pSaleLogTable.m_pTable )
		{
			return eNullTablePointer;
		}

		SQLite::Table& rSaleLogTable = pSaleLogTable();
		int nSaleLogRowCount = rSaleLogTable.GetRowCount();
		if ( 0 == nSaleLogRowCount )
		{
			return eNotExistSaleLog;
		}

		for ( int loop = 0; loop < nSaleLogRowCount; ++loop )
		{
			sSaleLog sLog;
			sLog.dwSellerChaDbNum = rSaleLogTable.GetValue< DWORD >( 0 );
			sLog.strSellerChaName = rSaleLogTable.GetValue< std::string >( 1 );
			sLog.dwBuyerChaDbNum = rSaleLogTable.GetValue< DWORD >( 2 );
			sLog.strBuyerChaName = rSaleLogTable.GetValue< std::string >( 3 );
			sLog.sItemID.wMainID = rSaleLogTable.GetValue< WORD >( 4 );
			sLog.sItemID.wSubID = rSaleLogTable.GetValue< WORD >( 5 );
			sLog.llMoney = rSaleLogTable.GetValue< LONGLONG >( 6 );
			sLog.dwItemTurnNum = rSaleLogTable.GetValue< DWORD >( 7 );

			if ( 0 == loop )
			{
				llLastLogNum = rSaleLogTable.GetValue< LONGLONG >( 8 );
			}
			
			sLog.strDate = rSaleLogTable.GetValue< std::string >( 9 );
			sLog.strHour = rSaleLogTable.GetValue< std::string >( 10 );
			sLog.fCommission = rSaleLogTable.GetValue< float >( 11 );
            sLog.nSellType = rSaleLogTable.GetValue< int >( 12 );

			vecSaleLog.push_back( sLog );

			rSaleLogTable.GoNext();
		}

		// 마지막으로 가져간 로그 번호 세팅
		m_pLastLogNum->SetLastLogNum( dwChaDbNum, nServerGroup, llLastLogNum );

		double fElapsed = SaleLogSelectTimer.elapsed();
		if ( fElapsed > 1.0f )
		{
			sc::writeLogWarn( sc::string::format( "GetSaleLog taken %1% second.", fElapsed ) );
		}

		return eSuccess;
	}

	void GLPrivateMarketSearchSQLite::UpdateBasicOption( 
		bool bChannelCheck, 
		bool bSchoolCheck, 
		bool bAllPage, 
		WORD wSearchItemNumberPerPage, 
		WORD wLogNumberPerPage, 
		WORD wLogNumberMaxPerCharacter, 
		int nSearchOption )
	{
		if ( m_bChannelCheck != bChannelCheck )
		{
			m_bChannelCheck = bChannelCheck;
		}

		if ( m_bSchoolCheck != bSchoolCheck )
		{
			m_bSchoolCheck = bSchoolCheck;
		}

		if ( m_bAllPage != bAllPage )
		{
			m_bAllPage = bAllPage;
		}

		if ( m_wSearchItemNumberPerPage != wSearchItemNumberPerPage )
		{
			m_wSearchItemNumberPerPage = wSearchItemNumberPerPage;
		}

		if ( m_wLogNumberPerPage != wLogNumberPerPage )
		{
			m_wLogNumberPerPage = wLogNumberPerPage;
		}

		if ( m_wLogNumberMaxPerCharacter != wLogNumberMaxPerCharacter )
		{
			m_wLogNumberMaxPerCharacter = wLogNumberMaxPerCharacter;
		}

		if ( m_nSearchOption != nSearchOption )
		{
			m_nSearchOption = nSearchOption;
		}
	}

	void GLPrivateMarketSearchSQLite::GetBasicOption( 
		bool& bChannelCheck, 
		bool& bSchoolCheck, 
		bool& bAllPage, 
		WORD& wSearchItemNumberPerPage, 
		WORD& wLogNumberPerPage, 
		WORD& wLogNumberMaxPerCharacter, 
		int& nSearchOption )
	{
		bChannelCheck = m_bChannelCheck;
		bSchoolCheck = m_bSchoolCheck;
		bAllPage = m_bAllPage;
		wSearchItemNumberPerPage = m_wSearchItemNumberPerPage;
		wLogNumberPerPage = m_wLogNumberPerPage;
		wLogNumberMaxPerCharacter = m_wLogNumberMaxPerCharacter;
		nSearchOption = m_nSearchOption;
	}

	void GLPrivateMarketSearchSQLite::UpdateChannelOption( bool bChannelCheck )
	{
		m_bChannelCheck = bChannelCheck;
	}

	void GLPrivateMarketSearchSQLite::UpdateSchoolOption( bool bSchoolCheck )
	{
		m_bSchoolCheck = bSchoolCheck;
	}

	void GLPrivateMarketSearchSQLite::UpdatePageOption( bool bAllPage )
	{
		m_bAllPage = bAllPage;
	}

	void GLPrivateMarketSearchSQLite::UpdateSearchOption( int nValue )
	{
		m_nSearchOption = nValue;
	}

	int GLPrivateMarketSearchSQLite::DeleteSeller( DWORD dwSellerFieldSlot )
	{
		// 필드서버가 다운되었을 때 캐시서버에서 처리를 해주지 않아서
		// 실제 해당 필드서버에서는 개인상점이 모두 삭제되었으나
		// 캐시서버에서는 모르고 있었다. 필드서버가 죽을 때 처리를 추가한다.
		// Sellers FieldSlot
		// SaleItems 
		// HoldItems SellerFieldSlot

		std::string strSellerSelectQuery( 
			sc::string::format( 
			"\
			SELECT ChaNum, ServerGroup \
			FROM Sellers \
			WHERE FieldSlot = %1%;", 
			dwSellerFieldSlot ) );

		SQLite::TablePtr pSellersTable( m_SaleItem.QuerySQL2( strSellerSelectQuery.c_str() ) );
		if ( !pSellersTable.m_pTable )
		{
			return eNullTablePointer;
		}

		SQLite::Table& rSellersTable = pSellersTable();
		int nRowCount = rSellersTable.GetRowCount();
		for ( int loop = 0; loop < nRowCount; ++loop )
		{
			DWORD dwSellerChaDbNum = 0;
			int nServerGroup = 0;

			dwSellerChaDbNum = rSellersTable.GetValue< DWORD >( 0 );
			nServerGroup = rSellersTable.GetValue< int >( 1 );
			DeleteItem( dwSellerChaDbNum, nServerGroup );

			rSellersTable.GoNext();
		}

		std::string strSellerDeleteQuery( 
			sc::string::format( 
			"\
			DELETE FROM Sellers \
			WHERE FieldSlot = %1%;", 
			dwSellerFieldSlot ) );
		execute( strSellerDeleteQuery );

		return eSuccess;
	}

	void GLPrivateMarketSearchSQLite::FrameMoveSaleLog()
	{
		// 3시간마다 로그 삭제한다.
		DWORD dwCurrentTime = timeGetTime();
		if ( ( dwCurrentTime - m_dwSaleLogFrameMoveTimer ) >= 10800000 )
		{
			struct tm today;
			time_t ltime;
			time( &ltime );
			localtime_s( &today, &ltime);

			// private_market_log.db3 백업
			{
				SUBPATH Path( SP_OFFICE_TEST );
				std::string strFrom = sc::getAppPath();
				strFrom.append( Path.Cache() );
				strFrom.append( "\\private_market_log.db3" );

				char szFileName[255] = {0};
				_tcsftime( szFileName, 255, "private_market_log_%y%m%d%H%M.db3",&today );
				std::string strTo = sc::getAppPath();
				strTo.append( Path.Cache() );
				strTo.append( "\\" );
				strTo.append( szFileName );
				sc::file::copyFile( strFrom, strTo );
			}

			// 3시간이전 데이터 삭제
			std::string strDelSaleLogQuery( "DELETE FROM SaleLogV2 WHERE SaleDate < date('now', '-3 hour');" );
			executeForSaleLog( strDelSaleLogQuery );

			m_dwSaleLogFrameMoveTimer = dwCurrentTime;
		}

		/*struct tm today;
		time_t ltime;
		time( &ltime );
		localtime_s( &today, &ltime);

		if ( m_bTriggerSaleLogBackup && today.tm_hour == 5 && today.tm_min == 0 )
		{
			m_bTriggerSaleLogBackup = FALSE;

			// private_market_log.db3 백업
			{
				SUBPATH Path( SP_OFFICE_TEST );
				std::string strFrom = sc::getAppPath();
				strFrom.append( Path.Cache() );
				strFrom.append( "\\private_market_log.db3" );

				char szFileName[255] = {0};
				_tcsftime( szFileName, 255, "private_market_log_%y%m%d%H%M.db3",&today );
				std::string strTo = sc::getAppPath();
				strTo.append( Path.Cache() );
				strTo.append( "\\" );
				strTo.append( szFileName );
				sc::file::copyFile( strFrom, strTo );
			}

			// 1일이전 데이터 삭제
			std::string strDelSaleLogQuery( "DELETE FROM SaleLogV2 WHERE SaleDate < date('now', '-1 day');" );
			executeForSaleLog( strDelSaleLogQuery );
		}

		if ( m_bTriggerSaleLogBackup == FALSE && today.tm_hour == 5 && today.tm_min > 0 )
		{
			m_bTriggerSaleLogBackup = TRUE;
		}*/
	}

	int GLPrivateMarketSearchSQLite::openDatabase()
	{
		SUBPATH Path( SP_OFFICE_TEST );
		std::string strFile = sc::getAppPath();
		strFile.append( Path.Cache() );
		strFile.append( "\\private_market.db3" );
		int nResult = m_SaleItem.Open( strFile.c_str() );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( "GLPrivateMarketSearchSQLite openDatabase m_SaleItem critical error: %1%", nResult/*m_SaleItem.GetLastError()*/ ) );
			sc::ErrorMessageBox( strMessage, false );
			return nResult;
		}

		std::string strFilePath = sc::getAppPath();
		strFilePath.append( Path.Cache() );
		strFilePath.append( "\\private_market_log.db3" );

		// 기존 private_market_log.db3 백업
		struct tm today;
		time_t ltime;
		time( &ltime );
		localtime_s( &today, &ltime);
		char szFileName[255] = {0};
		_tcsftime( szFileName, 255, "private_market_log_%y%m%d%H%M.db3",&today );
		std::string strTargetFilePath = sc::getAppPath();
		strTargetFilePath.append( Path.Cache() );
		strTargetFilePath.append( "\\" );
		strTargetFilePath.append( szFileName );
		sc::file::copyFile( strFilePath, strTargetFilePath );

		nResult = m_SaleLog.Open( strFilePath.c_str() );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( "GLPrivateMarketSearchSQLite openDatabase m_SaleLog critical error: %1%", nResult/*m_SaleLog.GetLastError()*/ ) );
			sc::ErrorMessageBox( strMessage, false );
			return nResult;
		}

		return createTable();
	}

	void GLPrivateMarketSearchSQLite::closeDatabase()
	{
		finalizeSQLStatement();

		if ( m_SaleItem.IsOpen() )
		{
			m_SaleItem.Close();
		}

		if ( m_SaleLog.IsOpen() )
		{
			m_SaleLog.Close();
		}
	}

	int GLPrivateMarketSearchSQLite::createTable()
	{
		// 테이블에 default 값 추가 해야 한다.
		int nResult = SQLITE_OK;

		std::string strDropQuery( 
			"\
			DROP TABLE IF EXISTS Sellers; \
			DROP TABLE IF EXISTS SaleItems; \
			DROP TABLE IF EXISTS HoldItems; \
			PRAGMA auto_vacuum = 1; \
			VACUUM;" );
		execute( strDropQuery );

		std::string strLogDropQuery( 
			"\
			PRAGMA auto_vacuum = 1; \
			VACUUM;" );
		executeForSaleLog( strLogDropQuery );

		std::string strSellersCreateQuery(
			"\
			CREATE TABLE Sellers( \
			ChaNum INTEGER NOT NULL, \
			ChaName TEXT NOT NULL, \
			MapMainID INTEGER NOT NULL, \
			MapSubID INTEGER NOT NULL, \
			School INTEGER NOT NULL, \
			Channel INTEGER NOT NULL, \
			FieldSlot INTEGER NOT NULL, \
			ServerGroup INTEGER NOT NULL, \
			PRIMARY KEY( ChaNum, ServerGroup ) ); \
			CREATE INDEX Idx_Sellers_ChaNum ON Sellers ( ChaNum );" );

		nResult = execute( strSellersCreateQuery );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( 
				"GLPrivateMarketSearchSQLite createTable error expression : %1% error : %2%", 
				strSellersCreateQuery.c_str(), nResult/*m_SaleItem.GetLastError()*/ ) );
			sc::ErrorMessageBox( strMessage, false );
			return nResult;
		}

		std::string strSaleItemsCreateQuery(
			"\
			CREATE TABLE SaleItems( \
			SellerChaNum INTEGER NOT NULL, \
			SellerChannel INTEGER NOT NULL, \
			SellerSchool INTEGER NOT NULL, \
			Price INTEGER NOT NULL, \
			TurnNum INTEGER NOT NULL, \
			GUID TEXT NOT NULL, \
			Type INTEGER NOT NULL, \
			WeaponType INTEGER NOT NULL, \
			ArmorType INTEGER NOT NULL, \
			TimeLimit INTEGER NOT NULL, \
			RequireLevel INTEGER NOT NULL, \
			RequirePower INTEGER NOT NULL, \
			RequireDex INTEGER NOT NULL, \
			RequireSpirit INTEGER NOT NULL, \
			RequireClassSex INTEGER NOT NULL, \
			Rank INTEGER NOT NULL, \
			BasicDamage INTEGER NOT NULL, \
			BasicRequireSP INTEGER NOT NULL, \
			BasicDefence INTEGER NOT NULL, \
			BasicHitrate INTEGER NOT NULL, \
			BasicAvoidrate INTEGER NOT NULL, \
			BasicGrindingDamage INTEGER NOT NULL, \
			BasicGrindingDefence INTEGER NOT NULL, \
			BasicExpMultiple INTEGER NOT NULL, \
			BasicAllResist INTEGER NOT NULL, \
			BasicFireResist INTEGER NOT NULL, \
			BasicIceResist INTEGER NOT NULL, \
			BasicElecResist INTEGER NOT NULL, \
			BasicPoisonResist INTEGER NOT NULL, \
			BasicSpiritResist INTEGER NOT NULL, \
			AddonRandomHitrate INTEGER NOT NULL, \
			AddonRandomAvoidRate INTEGER NOT NULL, \
			AddonRandomDamage INTEGER NOT NULL, \
			AddonRandomDefence INTEGER NOT NULL, \
			AddonRandomHp INTEGER NOT NULL, \
			AddonRandomMp INTEGER NOT NULL, \
			AddonRandomSp INTEGER NOT NULL, \
			AddonRandomStatsPow INTEGER NOT NULL, \
			AddonRandomStatsStr INTEGER NOT NULL, \
			AddonRandomStatsSpi INTEGER NOT NULL, \
			AddonRandomStatsDex INTEGER NOT NULL, \
			AddonRandomStatsInt INTEGER NOT NULL, \
			AddonRandomStatsSta INTEGER NOT NULL, \
			AddonRandomPA INTEGER NOT NULL, \
			AddonRandomSA INTEGER NOT NULL, \
			AddonRandomMA INTEGER NOT NULL, \
			VarRandomHpRate FLOAT NOT NULL, \
			VarRandomMpRate INTEGER NOT NULL, \
			VarRandomSpRate INTEGER NOT NULL, \
			VarRandomHpMpSpRate INTEGER NOT NULL, \
			VarRandomMoveSpeedRate INTEGER NOT NULL, \
			VarAttackSpeedRate FLOAT NOT NULL, \
			VarCriticalRate INTEGER NOT NULL, \
			VarCrushingRate INTEGER NOT NULL, \
			VarRandomHp INTEGER NOT NULL, \
			VarRandomMp INTEGER NOT NULL, \
			VarRandomSp INTEGER NOT NULL, \
			VarRandomHpMpSp INTEGER NOT NULL, \
			VarRandomMoveSpeed INTEGER NOT NULL, \
			VarAttackSpeed FLOAT NOT NULL, \
			VarCritical INTEGER NOT NULL, \
			VarCrushing INTEGER NOT NULL, \
			BlowType INTEGER NOT NULL, \
			CommissionRate FLOAT NOT NULL, \
			Name TEXT NOT NULL, \
            SellType INTEGER NOT NULL, \
            ConsigmentSallerName TEXT NOT NULL, \
            ExpirationDate INTEGER NOT NULL, \
			ServerGroup INTEGER NOT NULL, \
			PRIMARY KEY( GUID ) ); \
			CREATE UNIQUE INDEX Idx_SaleItems_GUID ON SaleItems ( GUID ); \
			CREATE INDEX Idx_SaleItems_SellerChaNum ON SaleItems ( SellerChaNum ); \
			CREATE INDEX Idx_SaleItems_SellerChannel ON SaleItems ( SellerChannel ); \
			CREATE INDEX Idx_SaleItems_SellerSchool ON SaleItems ( SellerSchool ); \
			CREATE INDEX Idx_SaleItems_Type ON SaleItems ( Type ); \
			CREATE INDEX Idx_SaleItems_WeaponType ON SaleItems ( WeaponType ); \
			CREATE INDEX Idx_SaleItems_ArmorType ON SaleItems ( ArmorType ); \
			CREATE INDEX Idx_SaleItems_TimeLimit ON SaleItems ( TimeLimit ); \
			CREATE INDEX Idx_SaleItems_RequirePower ON SaleItems ( RequirePower ); \
			CREATE INDEX Idx_SaleItems_RequireDex ON SaleItems ( RequireDex ); \
			CREATE INDEX Idx_SaleItems_RequireSpirit ON SaleItems ( RequireSpirit ); \
			CREATE INDEX Idx_SaleItems_Rank ON SaleItems ( Rank ); \
			CREATE INDEX Idx_SaleItems_BasicDamage ON SaleItems ( BasicDamage ); \
			CREATE INDEX Idx_SaleItems_BasicRequireSP ON SaleItems ( BasicRequireSP ); \
			CREATE INDEX Idx_SaleItems_BasicDefence ON SaleItems ( BasicDefence ); \
			CREATE INDEX Idx_SaleItems_BasicHitrate ON SaleItems ( BasicHitrate ); \
			CREATE INDEX Idx_SaleItems_BasicAvoidrate ON SaleItems ( BasicAvoidrate ); \
			CREATE INDEX Idx_SaleItems_BasicGrindingDamage ON SaleItems ( BasicGrindingDamage ); \
			CREATE INDEX Idx_SaleItems_BasicGrindingDefence ON SaleItems ( BasicGrindingDefence ); \
			CREATE INDEX Idx_SaleItems_BasicExpMultiple ON SaleItems ( BasicExpMultiple ); \
			CREATE INDEX Idx_SaleItems_BasicAllResist ON SaleItems ( BasicAllResist ); \
			CREATE INDEX Idx_SaleItems_BasicFireResist ON SaleItems ( BasicFireResist ); \
			CREATE INDEX Idx_SaleItems_BasicIceResist ON SaleItems ( BasicIceResist ); \
			CREATE INDEX Idx_SaleItems_BasicElecResist ON SaleItems ( BasicElecResist ); \
			CREATE INDEX Idx_SaleItems_BasicPoisonResist ON SaleItems ( BasicPoisonResist ); \
			CREATE INDEX Idx_SaleItems_BasicSpiritResist ON SaleItems ( BasicSpiritResist ); \
			CREATE INDEX Idx_SaleItems_AddonRandomHitrate ON SaleItems ( AddonRandomHitrate ); \
			CREATE INDEX Idx_SaleItems_AddonRandomAvoidRate ON SaleItems ( AddonRandomAvoidRate ); \
			CREATE INDEX Idx_SaleItems_AddonRandomDamage ON SaleItems ( AddonRandomDamage ); \
			CREATE INDEX Idx_SaleItems_AddonRandomDefence ON SaleItems ( AddonRandomDefence ); \
			CREATE INDEX Idx_SaleItems_AddonRandomHp ON SaleItems ( AddonRandomHp ); \
			CREATE INDEX Idx_SaleItems_AddonRandomMp ON SaleItems ( AddonRandomMp ); \
			CREATE INDEX Idx_SaleItems_AddonRandomSp ON SaleItems ( AddonRandomSp ); \
			CREATE INDEX Idx_SaleItems_AddonRandomStatsPow ON SaleItems ( AddonRandomStatsPow ); \
			CREATE INDEX Idx_SaleItems_AddonRandomStatsStr ON SaleItems ( AddonRandomStatsStr ); \
			CREATE INDEX Idx_SaleItems_AddonRandomStatsSpi ON SaleItems ( AddonRandomStatsSpi ); \
			CREATE INDEX Idx_SaleItems_AddonRandomStatsDex ON SaleItems ( AddonRandomStatsDex ); \
			CREATE INDEX Idx_SaleItems_AddonRandomStatsInt ON SaleItems ( AddonRandomStatsInt ); \
			CREATE INDEX Idx_SaleItems_AddonRandomStatsSta ON SaleItems ( AddonRandomStatsSta ); \
			CREATE INDEX Idx_SaleItems_AddonRandomPA ON SaleItems ( AddonRandomPA ); \
			CREATE INDEX Idx_SaleItems_AddonRandomSA ON SaleItems ( AddonRandomSA ); \
			CREATE INDEX Idx_SaleItems_AddonRandomMA ON SaleItems ( AddonRandomMA ); \
			CREATE INDEX Idx_SaleItems_VarRandomHpRate ON SaleItems ( VarRandomHpRate ); \
			CREATE INDEX Idx_SaleItems_VarRandomMpRate ON SaleItems ( VarRandomMpRate ); \
			CREATE INDEX Idx_SaleItems_VarRandomSpRate ON SaleItems ( VarRandomSpRate ); \
			CREATE INDEX Idx_SaleItems_VarRandomHpMpSpRate ON SaleItems ( VarRandomHpMpSpRate ); \
			CREATE INDEX Idx_SaleItems_VarRandomMoveSpeedRate ON SaleItems ( VarRandomMoveSpeedRate ); \
			CREATE INDEX Idx_SaleItems_VarAttackSpeedRate ON SaleItems ( VarAttackSpeedRate ); \
			CREATE INDEX Idx_SaleItems_VarCriticalRate ON SaleItems ( VarCriticalRate ); \
			CREATE INDEX Idx_SaleItems_VarCrushingRate ON SaleItems ( VarCrushingRate ); \
			CREATE INDEX Idx_SaleItems_VarRandomHp ON SaleItems ( VarRandomHp ); \
			CREATE INDEX Idx_SaleItems_VarRandomMp ON SaleItems ( VarRandomMp ); \
			CREATE INDEX Idx_SaleItems_VarRandomSp ON SaleItems ( VarRandomSp ); \
			CREATE INDEX Idx_SaleItems_VarRandomHpMpSp ON SaleItems ( VarRandomHpMpSp ); \
			CREATE INDEX Idx_SaleItems_VarRandomMoveSpeed ON SaleItems ( VarRandomMoveSpeed ); \
			CREATE INDEX Idx_SaleItems_VarAttackSpeed ON SaleItems ( VarAttackSpeed ); \
			CREATE INDEX Idx_SaleItems_VarCritical ON SaleItems ( VarCritical ); \
			CREATE INDEX Idx_SaleItems_VarCrushing ON SaleItems ( VarCrushing ); \
			CREATE INDEX Idx_SaleItems_Name ON SaleItems ( Name ); \
			CREATE INDEX Idx_SaleItems_Price ON SaleItems ( Price ); \
            CREATE INDEX Idx_SaleItems_SellType ON SaleItems ( SellType ); \
			CREATE INDEX Idx_SaleItems_ServerGroup ON SaleItems ( ServerGroup ); \
            CREATE INDEX Idx_SaleItems_ConsigmentSallerName ON SaleItems ( ConsigmentSallerName ); \
            " );

		nResult = execute( strSaleItemsCreateQuery );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( 
				"GLPrivateMarketSearchSQLite createTable error expression : %1% error : %2%", 
				strSaleItemsCreateQuery.c_str(), nResult/*m_SaleItem.GetLastError()*/ ) );
			sc::ErrorMessageBox( strMessage, false );
			return nResult;
		}

		std::string strHoldItemsCreateQuery(
			"\
			CREATE TABLE HoldItems( \
			GUID TEXT NOT NULL, \
			SellerChaNum INTEGER NOT NULL, \
			SellerFieldSlot INTEGER NOT NULL, \
			SellerChannel INTEGER NOT NULL, \
			SellerServerGroup INTEGER NOT NULL, \
			TurnNum INTEGER NOT NULL, \
			Price INTEGER NOT NULL, \
			BuyerChaNum INTEGER NOT NULL, \
			BuyerFieldSlot INTEGER NOT NULL, \
			BuyerChaName TEXT NOT NULL, \
			BuyerServerGroup INTEGER NOT NULL, \
			InsertDate DATE NOT NULL DEFAULT( datetime( 'now', 'localtime' ) ), \
			CommissionRate FLOAT NOT NULL, \
            SellType INTEGER NOT NULL, \
            ConsigmentSallerName TEXT NOT NULL, \
			PRIMARY KEY( GUID ) ); \
			CREATE UNIQUE INDEX Idx_HoldItems_GUID ON HoldItems ( GUID ); \
			CREATE INDEX Idx_HoldItems_SellerChaNum  ON HoldItems ( SellerChaNum ); \
			CREATE INDEX Idx_HoldItems_BuyerChaNum  ON HoldItems ( BuyerChaNum );" );

		nResult = execute( strHoldItemsCreateQuery );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( 
				"GLPrivateMarketSearchSQLite createTable error expression : %1% error : %2%", 
				strHoldItemsCreateQuery.c_str(), nResult/*m_SaleItem.GetLastError()*/ ) );
			sc::ErrorMessageBox( strMessage, false );
			return nResult;
		}

		std::string strSaleLogCreateQuery(
			"\
			CREATE TABLE IF NOT EXISTS SaleLogV2( \
			seq INTEGER PRIMARY KEY AUTOINCREMENT, \
			SaleDate DATE NOT NULL DEFAULT( datetime( 'now', 'localtime' ) ), \
			SellerChaNum INTEGER NOT NULL, \
			SellerChaName TEXT NOT NULL, \
			SellerServerGroup INTEGER NOT NULL, \
			BuyerChaNum INTEGER NOT NULL, \
			BuyerChaName TEXT NOT NULL, \
			BuyerServerGroup INTEGER NOT NULL, \
			ItemMainID INTEGER NOT NULL, \
			ItemSubID INTEGER NOT NULL, \
			Money INTEGER NOT NULL, \
			TurnNum INTEGER NOT NULL, \
			CommissionRate FLOAT NOT NULL, \
            SellType INTEGER NOT NULL ); \
			CREATE UNIQUE INDEX IF NOT EXISTS Idx_SaleLog_seq ON SaleLogV2 ( seq ); \
			CREATE INDEX IF NOT EXISTS Idx_SaleLog_SellerChaNum ON SaleLogV2 ( SellerChaNum ); \
			CREATE INDEX IF NOT EXISTS Idx_SaleLog_BuyerChaNum ON SaleLogV2 ( BuyerChaNum );" );

		nResult = executeForSaleLog( strSaleLogCreateQuery );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( 
				"GLPrivateMarketSearchSQLite createTable error expression : %1% error : %2%", 
				strSaleLogCreateQuery.c_str(), nResult/*m_SaleLog.GetLastError()*/ ) );
			sc::ErrorMessageBox( strMessage, false );
			return nResult;
		}

		// 사용자 정의 함수 등록
		sqlite3_create_function( m_SaleItem.GetPtr(), "can_equip", 3, SQLITE_ANY, NULL, &canEquipFunc, NULL, NULL );
		sqlite3_create_function( m_SaleItem.GetPtr(), "is_searchable", 4, SQLITE_ANY, NULL, &IsSearchableFunc, NULL, NULL );
		sqlite3_create_function( m_SaleItem.GetPtr(), "is_searchable_servergroup", 3, SQLITE_ANY, NULL, &IsSearchableServerGroupFunc, NULL, NULL );
		sqlite3_create_function( m_SaleItem.GetPtr(), "is_searchable_selltype", 2, SQLITE_ANY, NULL, &IsSearchableSellTypeFunc, NULL, NULL );

		prepareSQLStatement();

		return nResult;
	}

	void GLPrivateMarketSearchSQLite::prepareSQLStatement()
	{
		m_strSaleItemInsertQuery = "\
		   INSERT INTO SaleItems( \
		   SellerChaNum,        SellerChannel,          SellerSchool,           Price,                  TurnNum,            \
		   GUID,                Type,                   WeaponType,             ArmorType,              TimeLimit,          \
		   RequireLevel,        RequirePower,           RequireDex,             RequireSpirit,          RequireClassSex,    \
		   Name,                Rank,                   BasicDamage,            BasicRequireSP,         BasicDefence,       \
		   BasicHitrate,        BasicAvoidrate,         BasicGrindingDamage,    BasicGrindingDefence,   BasicExpMultiple,   \
		   BasicAllResist,      BasicFireResist,        BasicIceResist,         BasicElecResist,        BasicPoisonResist,  \
		   BasicSpiritResist,   AddonRandomHitrate,     AddonRandomAvoidRate,   AddonRandomDamage,      AddonRandomDefence, \
		   AddonRandomHp,       AddonRandomMp,          AddonRandomSp,          AddonRandomStatsPow,    AddonRandomStatsStr,\
		   AddonRandomStatsSpi, AddonRandomStatsDex,    AddonRandomStatsInt,    AddonRandomStatsSta,    AddonRandomPA,      \
		   AddonRandomSA,       AddonRandomMA,          VarRandomHpRate,        VarRandomMpRate,        VarRandomSpRate,    \
		   VarRandomHpMpSpRate, VarRandomMoveSpeedRate, VarAttackSpeedRate,     VarCriticalRate,        VarCrushingRate,    \
		   VarRandomHp,         VarRandomMp,            VarRandomSp,            VarRandomHpMpSp,        VarRandomMoveSpeed, \
		   VarAttackSpeed,      VarCritical,            VarCrushing,            BlowType,               CommissionRate,     \
           SellType,            ConsigmentSallerName,   ExpirationDate,         ServerGroup ) \
		   VALUES( \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ?, ?, \
		   ?, ?, ?, ? );";

		sqlite3_prepare_v2( m_SaleItem.GetPtr(), m_strSaleItemInsertQuery.c_str(), -1, &m_stmtSaleItemInsert, NULL );
	}

	void GLPrivateMarketSearchSQLite::finalizeSQLStatement()
	{
		sqlite3_finalize( m_stmtSaleItemInsert );
	}

	int GLPrivateMarketSearchSQLite::execute( const std::string& strQuery, bool bDisplayError /*= true*/ )
	{
		if ( strQuery.empty() )
		{
			return eStringEmpty;
		}

		int nResult = SQLITE_OK;
		nResult = m_SaleItem.ExecuteSQL( strQuery );
		if ( SQLITE_OK != nResult && true == bDisplayError )
		{
			std::string strMessage( 
				sc::string::format( 
				"GLPrivateMarketSearchSQLite execute error expression : %1% error : %2%", 
				strQuery.c_str(), nResult/*m_SaleItem.GetLastError()*/ ) );
			sc::writeLogError( strMessage );
		}

		return nResult;
	}

	int GLPrivateMarketSearchSQLite::executeForSaleLog( const std::string& strQuery )
	{
		if ( strQuery.empty() )
		{
			return eStringEmpty;
		}

		int nResult = SQLITE_OK;
		nResult = m_SaleLog.ExecuteSQL( strQuery );
		if ( SQLITE_OK != nResult )
		{
			std::string strMessage( 
				sc::string::format( 
				"GLPrivateMarketSearchSQLite execute error expression : %1% error : %2%", 
				strQuery.c_str(), nResult/*m_SaleLog.GetLastError()*/ ) );
			sc::writeLogError( strMessage );
		}

		return nResult;
	}

	template< typename T >
	void DeduplicationVector( std::vector< T >& vecContainer )
	{
		// 중복제거
		std::map< int, T > mapFilter;
		size_t nBasicSize = vecContainer.size();
		for ( size_t loop = 0; loop < nBasicSize; ++loop )
		{
			mapFilter.insert( std::map< int, T >::value_type( vecContainer[loop].nType, vecContainer[loop] ) );
		}

		vecContainer.clear();
		std::map< int, T >::iterator iterBegin = mapFilter.begin();
		std::map< int, T >::iterator iterEnd = mapFilter.end();
		for ( ; iterBegin != iterEnd; ++iterBegin )
		{
			T& sContent = ( *iterBegin ).second;
			vecContainer.push_back( sContent );
		}
	}

	void GLPrivateMarketSearchSQLite::validateSearchOption( sSaleItemSearchandSortBasic& sSearch )
	{
		// nWeaponType 이 eItemWeaponTypeAll 가 아니라면 

		// vecBasic 중복제거
		DeduplicationVector< sSaleItemSearchOptionFloat >( sSearch.vecBasic );

		// vecAddonVarRandom 중복제거
		DeduplicationVector< sSaleItemSearchOptionFloat >( sSearch.vecAddonVarRandom );
	}

	std::string GLPrivateMarketSearchSQLite::makeSearchQuery( 
		WORD wPage, 
		int nChannel, 
		WORD wSchool, 
		sSaleItemSearchandSortBasic& sSearch, 
		std::string& strCountQuery,
		int nServerGroup, 
        bool bConsignmentSale )
	{
		validateSearchOption( sSearch );

		bool bAndNeed = false;
		bool bWhereNeed = false;

		std::string strWhere;
		std::string strTemp;

        if ( true == getWhereChannel( strTemp, nChannel ) )
        {
            bWhereNeed = true;
            strWhere += strTemp;
            bAndNeed = true;
        }

        if ( true == getWhereSchool( strTemp, wSchool ) )
        {
            bWhereNeed = true;
            if ( true == bAndNeed ) 
            {
                strWhere += " AND";
            }
            strWhere += strTemp;
            bAndNeed = true;
        }

		if ( true == getWhereTurnNum( strTemp, sSearch.dwItemTurnNum ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed ) 
			{
				strWhere += " AND";
			}
			strWhere += strTemp;
			bAndNeed = true;
		}

		if ( true == getWhereClassAndSex( strTemp, sSearch.nRequireClass, sSearch.nRequireSex ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed ) 
			{
				strWhere += " AND";
			}
			strWhere += strTemp;
			bAndNeed = true;
		}

		if ( true == getWhereSearchable( strTemp, nServerGroup ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed ) 
			{
				strWhere += " AND";
			}
			strWhere += strTemp;
			bAndNeed = true;
		}

		if ( true == getWhereType( strTemp, sSearch.nType ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed ) 
			{
				strWhere += " AND";
			} 
			strWhere += strTemp;
			bAndNeed = true;
		}

		if ( true == getWhereWeaponType( strTemp, sSearch.nWeaponType ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed )
			{
				strWhere += " AND";
			}
			strWhere += strTemp;
			bAndNeed = true;
		}

		if ( true == getWhereArmorType( strTemp, sSearch.nArmorType ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed )
			{
				strWhere += " AND";
			}
			strWhere += strTemp;
			bAndNeed = true;
		}

		if ( true == getWhereTimelimit( strTemp, sSearch.bTimelimit ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed ) 
			{
				strWhere += " AND";
			}
			strWhere += strTemp;
			bAndNeed = true;
		}

		if ( true == getWhereRequire( strTemp, sSearch.nRequire, sSearch.nRequireValue ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed )
			{
				strWhere += " AND";
			}
			strWhere += strTemp;
			bAndNeed = true;
		}

		if ( true == getWhereRank( strTemp, sSearch.nRank ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed )
			{
				strWhere += " AND";
			}
			strWhere += strTemp;
			bAndNeed = true;
		}

		size_t nBasicSize = sSearch.vecBasic.size();
		for ( size_t loop = 0; loop < nBasicSize; ++loop )
		{
			if ( true == getWhereBasic( strTemp, sSearch.vecBasic[loop].nType, sSearch.vecBasic[loop].fValue ) )
			{
				bWhereNeed = true;
				if ( true == bAndNeed )
				{
					strWhere += " AND";
				}
				strWhere += strTemp;
				bAndNeed = true;
			}
		}

		size_t nAddonVarRandom = sSearch.vecAddonVarRandom.size();
		for ( size_t loop = 0; loop < nAddonVarRandom; ++loop )
		{
			if ( true == getWhereAddonVarRandom( strTemp, sSearch.vecAddonVarRandom[loop].nType, sSearch.vecAddonVarRandom[loop].fValue ) )
			{
				bWhereNeed = true;
				if ( true == bAndNeed )
				{
					strWhere += " AND";
				}
				strWhere += strTemp;
				bAndNeed = true;
			}
		}

		if ( true == getWhereSerachSellType( strTemp, sSearch.nSearchSellType ) )
		{
			bWhereNeed = true;
			if ( true == bAndNeed ) 
			{
				strWhere += " AND";
			} 
			strWhere += strTemp;
			bAndNeed = true;
		}

		if( true == getWhereItemName( strTemp, sSearch.strItemName ))
		{
			bWhereNeed = true;
			if ( true == bAndNeed ) 
			{
				strWhere += " AND";
			} 
			strWhere += strTemp;
			bAndNeed = true;
		}

		std::string strSaleItemSearchQuery(g_szConsignmentSaleItemSearchQuery);
        strCountQuery = std::string(g_szConsignmentSaleItemCountQuery);
        		
		if ( true == bWhereNeed )
		{
			strSaleItemSearchQuery += " WHERE";
			strSaleItemSearchQuery += strWhere;

			strCountQuery += " WHERE";
			strCountQuery += strWhere;
		}

		strSaleItemSearchQuery += getOrderBy( sSearch.nSortOption, sSearch.nSortMethod );
		
		if ( false == m_bAllPage )
		{
			strSaleItemSearchQuery += getLimit( wPage, m_wSearchItemNumberPerPage );
		}

		strCountQuery += ';';

		strSaleItemSearchQuery += ";";

		return strSaleItemSearchQuery;
	}

	bool GLPrivateMarketSearchSQLite::getWhereChannel( std::string& strQuery, int nChannel )
	{
		// 채널 체크 기능 사용하지 않는다.
		/*
		bool bReturn = false;
		if ( m_bChannelCheck )
		{
			bReturn = true;
			strQuery = sc::string::format( " SellerChannel = %1%", nChannel );
		}
		
		return bReturn;
		*/

		return false;
	}

	bool GLPrivateMarketSearchSQLite::getWhereSchool( std::string& strQuery, WORD wSchool )
	{
		bool bReturn = false;
		if ( m_bSchoolCheck )
		{
			bReturn = true;
			strQuery = sc::string::format( " SellerSchool = %1%", wSchool );
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereTurnNum( std::string& strQuery, DWORD dwItemTurnNum )
	{
		bool bReturn = false;
		if ( dwItemTurnNum > 0 )
		{
			bReturn = true;
			strQuery = sc::string::format( " TurnNum >= %1%", dwItemTurnNum );
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereClassAndSex( std::string& strQuery, int nClass, int nSex )
	{
		bool bReturn = true;
		strQuery = sc::string::format( " can_equip( RequireClassSex, %1%, %2% ) = 1", nClass, nSex );

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereSearchable( std::string& strQuery, int nBuyerServerGroup )
	{
		bool bReturn = true;
		strQuery = sc::string::format( " is_searchable( SaleItems.SellType, SaleItems.ServerGroup, %1%, %2% ) = 1", nBuyerServerGroup, m_nSearchOption );
		/*strQuery = sc::string::format( " ( is_searchable_servergroup( SaleItems.ServerGroup, %1% ) = 1 OR is_searchable_selltype( SaleItems.SellType ) = 1 )", nBuyerServerGroup );*/

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereType( std::string& strQuery, int nType )
	{
		bool bReturn = false;
		if ( eItemTypeAll != nType )
		{
			bReturn = true;
			strQuery = sc::string::format( " Type = %1%", nType );
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereWeaponType( std::string& strQuery, int nWeaponType )
	{
		// 아이템타입이 eItemTypeWeapon 일 경우 eItemWeaponType 값을 확인할 수 있다는 전제가 있다.
		// 이러한 것들은 미리 검증을 해놓기로 한다. 따로 각각의 함수안에서 확인하지는 않는다.
		bool bReturn = false;
		if ( eItemWeaponTypeAll != nWeaponType )
		{
			bReturn = true;
			strQuery = sc::string::format( " WeaponType = %1%", nWeaponType );
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereArmorType( std::string& strQuery, int nArmorType )
	{
		bool bReturn = false;
		if ( eItemArmorTypeAll != nArmorType )
		{
			bReturn = true;
			strQuery = sc::string::format( " ArmorType = %1%", nArmorType );
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereTimelimit( std::string& strQuery, bool bTimelimit )
	{
		bool bReturn = false;
		if ( true == bTimelimit )
		{
			bReturn = true;
			strQuery = " TimeLimit = 1";
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereItemName( std::string& strQuery, const std::string& strItemName )
	{
		bool bReturn = false;

		if( !strItemName.empty() )
		{
			bReturn = true;

			strQuery = sc::string::format( " Name LIKE '%%%1%%%'", strItemName.c_str() );
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereRequire( std::string& strQuery, int nRequire, int nRequireValue )
	{
		bool bReturn = true;
		switch ( nRequire )
		{
		case eItemRequireLevel:
			strQuery = sc::string::format( " RequireLevel >= %1%", nRequireValue );
			break;
		case eItemRequirePow:
			strQuery = sc::string::format( " RequirePower >= %1%", nRequireValue );
			break;
		case eItemRequireDex:
			strQuery = sc::string::format( " RequireDex >= %1%", nRequireValue );
			break;
		case eItemRequireSpi:
			strQuery = sc::string::format( " RequireSpirit >= %1%", nRequireValue );
			break;
		default:
			bReturn = false;
			break;
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereRank( std::string& strQuery, int nRank )
	{
		bool bReturn = false;
		if ( eItemRankAll != nRank )
		{
			bReturn = true;
			strQuery = sc::string::format( " Rank = %1%", nRank );
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereBasic( std::string& strQuery, int nBasic, float fBasicValue )
	{
		bool bReturn = true;
		switch ( nBasic )
		{
		case eItemBasicDamage:
			strQuery = sc::string::format( " BasicDamage >= %1%", fBasicValue );
			break;
		case eItemBasicRequireSP:
			strQuery = sc::string::format( " BasicRequireSP >= %1%", fBasicValue );
			break;
		case eItemBasicDefence:
			strQuery = sc::string::format( " BasicDefence >= %1%", fBasicValue );
			break;
		case eItemBasicHitrate:
			strQuery = sc::string::format( " BasicHitrate >= %1%", fBasicValue );
			break;
		case eItemBasicAvoidrate:
			strQuery = sc::string::format( " BasicAvoidrate >= %1%", fBasicValue );
			break;
		case eItemBasicGrindingDamage:
			strQuery = sc::string::format( " BasicGrindingDamage >= %1%", fBasicValue );
			break;
		case eItemBasicGrindingDefence:
			strQuery = sc::string::format( " BasicGrindingDefence >= %1%", fBasicValue );
			break;
		case eItemBasicExpMultiple:
			strQuery = sc::string::format( " BasicExpMultiple >= %1%", fBasicValue );
			break;
		case eItemBasicAllResist:
			strQuery = sc::string::format( " BasicAllResist >= %1%", fBasicValue );
			break;
		case eItemBasicFireResist:
			strQuery = sc::string::format( " BasicFireResist >= %1%", fBasicValue );
			break;
		case eItemBasicIceResist:
			strQuery = sc::string::format( " BasicIceResist >= %1%", fBasicValue );
			break;
		case eItemBasicElecResist:
			strQuery = sc::string::format( " BasicElecResist >= %1%", fBasicValue );
			break;
		case eItemBasicPoisonResist:
			strQuery = sc::string::format( " BasicPoisonResist >= %1%", fBasicValue );
			break;
		case eItemBasicSpiritResist:
			strQuery = sc::string::format( " BasicSpiritResist >= %1%", fBasicValue );
			break;

		default:
			bReturn = false;
			break;
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereAddonVarRandom( std::string& strQuery, int nAddonVarRandom, float fAddonVarRandomValue )
	{
		bool bReturn = true;
		switch ( nAddonVarRandom )
		{
		case eItemAddonRandomHitrate:
			strQuery = sc::string::format( " AddonRandomHitrate >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomAvoidrate:
			strQuery = sc::string::format( " AddonRandomAvoidRate >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomDamage:
			strQuery = sc::string::format( " AddonRandomDamage >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomDefence:
			strQuery = sc::string::format( " AddonRandomDefence >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomHp:
			strQuery = sc::string::format( " AddonRandomHp >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomMp:
			strQuery = sc::string::format( " AddonRandomMp >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomSp:
			strQuery = sc::string::format( " AddonRandomSp >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomStatsPow:
			strQuery = sc::string::format( " AddonRandomStatsPow >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomStatsStr:
			strQuery = sc::string::format( " AddonRandomStatsStr >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomStatsSpi:
			strQuery = sc::string::format( " AddonRandomStatsSpi >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomStatsDex:
			strQuery = sc::string::format( " AddonRandomStatsDex >= %1%", fAddonVarRandomValue );
			break;
		/*
		// #CRM 1059 개인상점 시스템 필터 타입 삭제 요청
		// http://crm.mincoms.com/Ticket/TicketView/1056
		case eItemAddonRandomStatsInt:
			strQuery = sc::string::format( " AddonRandomStatsInt >= %1%", fAddonVarRandomValue );
			break;
		*/
		case eItemAddonRandomStatsSta:
			strQuery = sc::string::format( " AddonRandomStatsSta >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomPA:
			strQuery = sc::string::format( " AddonRandomPA >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomSA:
			strQuery = sc::string::format( " AddonRandomSA >= %1%", fAddonVarRandomValue );
			break;
		case eItemAddonRandomMA:
			strQuery = sc::string::format( " AddonRandomMA >= %1%", fAddonVarRandomValue );
			break;

		case eItemVarRandomHprate:
			strQuery = sc::string::format( " VarRandomHpRate >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarRandomMprate:
			strQuery = sc::string::format( " VarRandomMpRate >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarRandomSprate:
			strQuery = sc::string::format( " VarRandomSpRate >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarRandomHpMpSprate:
			strQuery = sc::string::format( " VarRandomHpMpSpRate >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarRandomMoveSpeedrate:
			strQuery = sc::string::format( " VarRandomMoveSpeedRate >= %1%", fAddonVarRandomValue );
			break;

		case eItemVarAttackSpeedrate:
			strQuery = sc::string::format( " VarAttackSpeedRate >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarCriticalrate:
			strQuery = sc::string::format( " VarCriticalRate >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarCrushingrate:
			strQuery = sc::string::format( " VarCrushingRate >= %1%", fAddonVarRandomValue );
			break;

		case eItemVarRandomHp:
			strQuery = sc::string::format( " VarRandomHp >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarRandomMp:
			strQuery = sc::string::format( " VarRandomMp >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarRandomSp:
			strQuery = sc::string::format( " VarRandomSp >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarRandomHpMpSp:
			strQuery = sc::string::format( " VarRandomHpMpSp >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarRandomMoveSpeed:
			strQuery = sc::string::format( " VarRandomMoveSpeed >= %1%", fAddonVarRandomValue );
			break;

		case eItemVarAttackSpeed:
			strQuery = sc::string::format( " VarAttackSpeed >= %1%", fAddonVarRandomValue );
			break;
		case eItemVarCritical:
			strQuery = sc::string::format( " VarCritical >= %1%", fAddonVarRandomValue );
			break;
		/*
		// #CRM 1059 개인상점 시스템 필터 타입 삭제 요청
		// http://crm.mincoms.com/Ticket/TicketView/1056
		case eItemVarCrushing:
			strQuery = sc::string::format( " VarCrushing >= %1%", fAddonVarRandomValue );
			break;
		*/
		case eItemStateBlow:
			strQuery = sc::string::format( " BlowType = %1%", fAddonVarRandomValue );
			break;

		default:
			bReturn = false;
			break;
		}

		return bReturn;
	}

	bool GLPrivateMarketSearchSQLite::getWhereSerachSellType( std::string& strQuery, int nSerachSellType )
	{
		bool bReturn = false;

		switch( nSerachSellType )
		{
		case eSearchMoneyType_Gold:
			bReturn = true;
			strQuery = sc::string::format( " ( SellType = %1% or SellType =%2% )", eSellType_PrivateMarket, eSellType_ConsignmentSale_Gold );
			break;
		case eSearchMoneyType_RanPoint:
			bReturn = true;
			strQuery = sc::string::format( " SellType = %1%", eSellType_ConsignmentSale_RanPoint );
			break;
		}

		return bReturn;
	}

	std::string GLPrivateMarketSearchSQLite::getOrderBy( int nOption, int nMethod )
	{
		std::string strOrder = " ORDER BY";

		switch ( nOption )
		{
		case eItemSortOptionName:
			strOrder = strOrder + " Name";
			break;
		case eItemSortOptionPrice:
			strOrder = strOrder + " Price";
			break;
		default:
			strOrder = strOrder + " Name";
			break;
		}

		switch ( nMethod )
		{
		case eItemSortMethodAsc:
			strOrder = strOrder + " ASC";
			break;
		case eItemSortMethodDesc:
			strOrder = strOrder + " DESC";
			break;
		default:
			strOrder = strOrder + " ASC";
			break;
		}

		return strOrder;
	}

	std::string GLPrivateMarketSearchSQLite::getLimit( WORD wPage, WORD wSelRowNum )
	{
		if ( 1 > wPage )
		{
			wPage = 1;
		}

		WORD wSkipRow   = ( wPage - 1 ) * wSelRowNum;
		std::string strQuery = sc::string::format( " LIMIT %1%, %2%", wSkipRow, wSelRowNum );

		return strQuery;
	}

    bool GLPrivateMarketSearchSQLite::selectConsignmentSale( std::string& strQuery, std::vector< sSaleItem >& vecSaleItems )
    {
		SQLite::TablePtr pSaleItemsTable( m_SaleItem.QuerySQL2( strQuery.c_str() ) );
		if ( !pSaleItemsTable.m_pTable )
		{
			return false;
		}

		SQLite::Table& rSaleItemsTable = pSaleItemsTable();
		int nRowCount = rSaleItemsTable.GetRowCount();
		if ( nRowCount < 1 )
		{
			return false;
		}

		DWORD dwSellerChaDbNum = 0;
		DWORD dwItemTurnNum = 1;
		LONGLONG llItemPrice = 0;
		std::string strGUID;
		UUID sItemGUID;
		SITEMCUSTOM sItemcustom;
		std::string strSellerName;
        int nSellType;
        __time64_t ExpirationDate;
        SNATIVEID sSellerMapID;

		for ( int loop = 0; loop < nRowCount; ++loop )
		{
			dwSellerChaDbNum = rSaleItemsTable.GetValue< DWORD >( 0 );
			dwItemTurnNum = rSaleItemsTable.GetValue< DWORD >( 1 );
			llItemPrice = rSaleItemsTable.GetValue< LONGLONG >( 2 );
			strGUID = rSaleItemsTable.GetValue< std::string >( 3 );
			strSellerName = rSaleItemsTable.GetValue< std::string >( 4 );
            nSellType = rSaleItemsTable.GetValue<int>( 5 );
            ExpirationDate = rSaleItemsTable.GetValue< __time64_t >( 6 );
            sSellerMapID = NATIVEID_NULL();

            if( nSellType == eSellType_PrivateMarket )
            {
                strSellerName = rSaleItemsTable.GetValue< std::string >( 7 );
                sSellerMapID.wMainID = rSaleItemsTable.GetValue< WORD >( 8 );
                sSellerMapID.wSubID = rSaleItemsTable.GetValue< WORD >( 9 );
            }
            else if( nSellType == eSellType_ConsignmentSale_RanPoint || nSellType == eSellType_ConsignmentSale_Gold )
            {
                if( ExpirationDate <= CTime::GetCurrentTime().GetTime() )
                {
                    rSaleItemsTable.GoNext();
                    continue;
                }
            }

            spItemcustom spItem = m_pSaleItem->FindSaleItem( strGUID );
            if ( spItem )
            {
                sItemcustom = *spItem.get();
            }

            sc::string::stringToUUID( const_cast< char* >( strGUID.c_str() ), sItemGUID );

            sSaleItem sItem( dwSellerChaDbNum, dwItemTurnNum, llItemPrice, sItemGUID, sItemcustom, sSellerMapID, strSellerName.c_str(), nSellType, ExpirationDate );
            vecSaleItems.push_back( sItem );
	
			rSaleItemsTable.GoNext();
		}

		return true;
    }

	void GLPrivateMarketSearchSQLite::deleteHoldItems()
	{
		std::string strHoldItemDeleteQuery( "\
											DELETE FROM HoldItems \
											WHERE ( strftime( '%s', 'now' ) - strftime( '%s', InsertDate ) )" );

		std::string strWhereContent( 
			sc::string::format( 
			" > %1%;", 
			g_HoldItemDeleteSecond ) );

		strHoldItemDeleteQuery += strWhereContent;

		execute( strHoldItemDeleteQuery );
	}

    int GLPrivateMarketSearchSQLite::GetSellType( UUID& sItemGUID, int& nSellType )
    {
        std::string strGUID = sc::string::uuidToString( sItemGUID );

        std::string strQuery = sc::string::format( 
            "SELECT SellType FROM SaleItems WHERE GUID = '%1%';",
            strGUID.c_str() );

        SQLite::TablePtr pSaleItemsTable( m_SaleItem.QuerySQL2( strQuery.c_str() ) );
        if ( !pSaleItemsTable.m_pTable )
            return eNullTablePointer;

        SQLite::Table& rSaleItemsTable = pSaleItemsTable();
        if ( rSaleItemsTable.GetRowCount() == 0 )
            return eNotExistSaleItem;

        nSellType = rSaleItemsTable.GetValue< int >( 0 );

        return eSuccess;
    }

    int GLPrivateMarketSearchSQLite::InsertSaleItem( 
        std::string strChaName, 
        WORD wMapMainID, 
        WORD wMapSubID, 
        WORD wSchool, 
        int nChannel, 
        DWORD dwFieldSlot, 
        DWORD dwChaDbNum, 
        UUID& sItemGUID, 
        const SITEMCUSTOM& sItemcustom, 
        const DWORD dwTurnNum, 
        const LONGLONG& llPrice, 
        const float fCommissionRate, 
        eSellType emSellType, /*= eSellType_ConsignmentSale_RanPoint */ 
        std::string strConsignmentSalllerName,
        __time64_t ExpirationDate, 
		int nServerGroup )
    {
        boost::timer SaleItemInsertTimer;

        SITEM* pItembase = GLItemMan::GetInstance().GetItem( sItemcustom.GetBoxWrappedItemID() );
        if ( NULL == pItembase )
        {
            return eWrongItem;
        }

        // 아이템 종류
        int nType = eItemTypeAll;

        {
            switch ( pItembase->sBasicOp.emItemType )
            {
            case ITEM_SUIT:
			case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
                {
                    switch ( pItembase->sSuitOp.emSuit )
                    {
                    case SUIT_HEADGEAR:
                    case SUIT_UPPER:
                    case SUIT_LOWER:
                    case SUIT_HAND:
                    case SUIT_FOOT:
                    case SUIT_NECK:
                    case SUIT_WRIST:
                    case SUIT_FINGER:
                    case SUIT_WAISTBAND:
                    case SUIT_EARRING:
                    case SUIT_ACCESSORIES:
                        {
							if ( true == ( sItemcustom.IsDisguise() != NULL ) )
                            {
                                nType = eItemTypeCostume;
                            }
                            else
                            {
                                nType = eItemTypeArmor;
                            }
                        }
                        break;
                    case SUIT_HANDHELD:
                        {
                            nType = eItemTypeWeapon;
                        }
                        break;
                    case SUIT_PET_A:
                    case SUIT_PET_B:
                        {
                            nType = eItemTypePet;
                        }
                        break;
                    case SUIT_VEHICLE_SKIN:
                    case SUIT_VEHICLE_PARTS_A:
                    case SUIT_VEHICLE_PARTS_B:
                    case SUIT_VEHICLE_PARTS_C:
                    case SUIT_VEHICLE_PARTS_D:
                    case SUIT_VEHICLE_PARTS_E:
                    case SUIT_VEHICLE_PARTS_F:
                        {
                            nType = eItemTypeVehicle;
                        }
                        break;
                    }
                }
                break;

            case ITEM_SKILL:
                {
                    nType = eItemTypeSkillBook;
                }
                break;

            case ITEM_RECALL:
                {
                    nType = eItemTypeRecall;
                }
                break;

            case ITEM_GRINDING:
            case ITEM_ANTI_DISAPPEAR:
            case ITEM_INC_GRINDINGRATE:
            case ITEM_ANTI_RESET:
                {
                    nType = eItemTypeGrinding;
                }
                break;

            case ITEM_PET_CARD:
            case ITEM_PET_FOOD:
            case ITEM_PET_RENAME:
            case ITEM_PET_COLOR:
            case ITEM_PET_STYLE:
            case ITEM_PET_SKILL:
            case ITEM_PET_REVIVE:
            case ITEM_PET_SKIN_PACK:
            case ITEM_PET_DUALSKILL:
                {
                    nType = eItemTypePet;
                }
                break;

            case ITEM_VEHICLE:
            case ITEM_VEHICLE_OIL:
            case ITEM_VEHICLE_COLOR:
            case ITEM_BIKE_BOOST_CARD:
                {
                    nType = eItemTypeVehicle;
                }
                break;

            case ITEM_SKP_STAT_RESET_A:
            case ITEM_LOUDSPEAKER:
            case ITEM_FIRECRACKER:
            case ITEM_QITEM:
            case ITEM_RENAME:
            case ITEM_REVIVE:
            case ITEM_SMS:
            case ITEM_PRODUCT_RESET:
                {
                    nType = eItemTypeFunctional;
                }
                break;

            case ITEM_CLEANSER:
            case ITEM_DISJUNCTION:
            case ITEM_HAIR:
            case ITEM_FACE:
            case ITEM_HAIRSHOP:
            case ITEM_HAIR_STYLE:
            case ITEM_HAIR_COLOR:
            case ITEM_COSTUM_COLOR_CH_CD:
                {
                    nType = eItemTypeCostumeExtra;
                }
                break;

            case ITEM_BOX:
            case ITEM_RANDOMITEM:
                {
                    nType = eItemTypeBox;
                }
                break;

            case ITEM_TELEPORT_CARD:
            case ITEM_PRODUCT_BOOK:
                {
                    nType = eItemTypeDocument;
                }
                break;

            case ITEM_TICKET:
            case ITEM_CHARACTER_CARD:
            case ITEM_INVEN_CARD:
            case ITEM_STORAGE_CARD:
            case ITEM_STORAGE_CONNECT:
            case ITEM_PRIVATEMARKET:
            case ITEM_2FRIEND:
            case ITEM_REMODEL:
            case ITEM_VIETNAM_ITEMGET:
            case ITEM_VIETNAM_EXPGET:
            case ITEM_GENDER_CHANGE:
            case ITEM_GARBAGE_CARD:
            case ITEM_FACE_STYLE:
            case ITEM_TAXI_CARD:
            case ITEM_NPC_RECALL:
            case ITEM_POSTBOX_CARD:
            case ITEM_POINT_CARD_REFUND:
            case ITEM_POINT_CARD_NOT_REFUND:
                {
                    nType = eItemTypeCard;
                }
                break;

			case ITEM_EFFECT_RELEASE_CURE:
            case ITEM_ARROW:
            case ITEM_CURE:
            case ITEM_KEY:
            case ITEM_CHARM:
            case ITEM_CLUBCALL:
            case ITEM_MATERIALS:
            case ITEM_BULLET:
            case ITEM_LUNCHBOX:
                {
                    nType = eItemTypeConsumeExtra;
                }
                break;
            case ITEM_CONSIGMENT_SALE_GAME_MONEY:
                {
                    nType = eItemTypeGameMoney;
                }
                break;
            }
 
        }

        // 필요 레벨
        int nRequireLevel = 0;

        {
            nRequireLevel = static_cast< int >( pItembase->sBasicOp.wReqLevelDW );
        }

        // 필요 힘
        int nRequirePow = 0;

        {
            nRequirePow = static_cast< int >( pItembase->sBasicOp.sReqStats.wPow );
        }

        // 필요 민첩
        int nRequireDex = 0;

        {
            nRequireDex = static_cast< int >( pItembase->sBasicOp.sReqStats.wDex );
        }

        // 필요 정신
        int nRequireSpi = 0;

        {
            nRequireSpi = static_cast< int >( pItembase->sBasicOp.sReqStats.wSpi );
        }

        // 기간제 아이템인가?
        int nTimeLimit = 0;

        {
			nTimeLimit = ( sItemcustom.IsTimeLimit() ) == true ? 1 : 0;
        }

        // 아이템 등급
        int nRank = eItemRankAll;

        {
            /*
			EMITEM_LEVEL_NORMAL		= 0,	// 일반;
			EMITEM_LEVEL_HIGH		= 1,	// 고급;
			EMITEM_LEVEL_RARE		= 2,	// 희귀;
			EMITEM_LEVEL_UNIQUE		= 3,	// 영웅;
			EMITEM_LEVEL_EPIC		= 4,	// 고대;
			EMITEM_LEVEL_LEGENDARY	= 5,	// 전설;

			EMITEM_LEVEL_NSIZE		= 6,
            */
            switch ( pItembase->sBasicOp.emLevel )
            {
            case EMITEM_LEVEL_NORMAL:
                nRank = eItemRankNomal;
                break;
            case EMITEM_LEVEL_HIGH:
                nRank = eItemRankHigh;
                break;
            case EMITEM_LEVEL_RARE:
                nRank = eItemRankRare;
                break;
            case EMITEM_LEVEL_UNIQUE:
                nRank = eItemRankUnique;
                break;
            case EMITEM_LEVEL_EPIC:
                nRank = eItemRankEpic;
                break;
			case EMITEM_LEVEL_LEGENDARY:
				nRank = eItemRankLegendary;
				break;
            }
        }

        // 착용 클래스 / 성별
        DWORD dwEquipClassSex = 0;
        {
            dwEquipClassSex = pItembase->sBasicOp.dwReqCharClass;
        }

        // 아이템 종류 -> 무기 -> 공격속성
        int nWeaponType = eItemWeaponTypeAll;

        {
            /*
            ITEMATT_NOTHING		= 0,	//	착용무기없음.

            ITEMATT_SWORD		= 1,	//	검.
            ITEMATT_SABER		= 2,	//	도.

            ITEMATT_DAGGER		= 3,	//	단검.
            ITEMATT_SPEAR		= 4,	//	창.
            ITEMATT_STICK		= 5,	//	몽둥이.
            ITEMATT_GWON		= 6,	//	권.
            ITEMATT_BOW			= 7,	//	활.
            ITEMATT_THROW		= 8,	//	투척.

            ITEMATT_PISTOL		= 9,	//	권총
            ITEMATT_RAILGUN		= 10,	//	소총1
            ITEMATT_PORTALGUN	= 11,	//	소총2

            ITEMATT_HAMMER      = 12,   // 해머.
            ITEMATT_DUALSPEAR   = 13,   // 듀얼 스피어.
            ITEMATT_SIDE        = 14,   // 사이드.

            ITEMATT_NOCARE		= 15,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
            ITEMATT_NSIZE		= 15,	//	종류갯수.
            */

            switch ( pItembase->sSuitOp.emAttack )
            {
            case ITEMATT_SWORD:
                nWeaponType = eItemWeaponTypeSword;
                break;

            case ITEMATT_SABER:
                nWeaponType = eItemWeaponTypeSaber;
                break;

            case ITEMATT_DAGGER:
                nWeaponType = eItemWeaponTypeDagger;
                break;

            case ITEMATT_SPEAR:
                nWeaponType = eItemWeaponTypeSpear;
                break;

            case ITEMATT_STICK:
                nWeaponType = eItemWeaponTypeStick;
                break;

            case ITEMATT_GWON:
                nWeaponType = eItemWeaponTypeGwon;
                break;

            case ITEMATT_BOW:
                nWeaponType = eItemWeaponTypeBow;
                break;

            case ITEMATT_THROW:
                nWeaponType = eItemWeaponTypeThrow;
                break;

            case ITEMATT_PISTOL:
                nWeaponType = eItemWeaponTypePistol;
                break;

            case ITEMATT_RAILGUN:
                nWeaponType = eItemWeaponTypeRailgun;
                break;

            case ITEMATT_PORTALGUN:
                nWeaponType = eItemWeaponTypePortalgun;
                break;

            case ITEMATT_SIDE:
                nWeaponType = eItemWeaponTypeSide;
                break;

            case ITEMATT_DUALSPEAR:
                nWeaponType = eItemWeaponTypeDualspear;
                break;

            case ITEMATT_THROWING_KNIFE:
                nWeaponType = eItemWeaponTypeThrowingKnife;
                break;
			
			case ITEMATT_EXTREME_GLOVE:
				nWeaponType = eItemWeaponTypeExtremeGlove;
				break;

			case ITEMATT_TRICK_STICK:
				nWeaponType = eItemWeaponTypeTrickStick;
				break;

			case ITEMATT_TRICK_BOX:
				nWeaponType = eItemWeaponTypeTrickBox;
				break;

			case ITEMATT_TRICK_WHIP:
				nWeaponType = eItemWeaponTypeTrickWhip;
				break;

			case ITEMATT_ACTOR_SHIELD:
				nWeaponType = eItemWeaponTypeActorShield;
				break;

			case ITEMATT_ACTOR_HAMMER:
				nWeaponType = eItemWeaponTypeActorHammer;
				break;

			case ITEMATT_ACTOR_UMBRELLA:
				nWeaponType = eItemWeaponTypeActorUmbrella;
				break;

			case ITEMATT_WEAPON_STR:
				nWeaponType = eItemWeaponTypeWeaponStr;
				break;

			case ITEMATT_WEAPON_DEX:
				nWeaponType = eItemWeaponTypeWeaponDex;
				break;

			case ITEMATT_WEAPON_SPI:
				nWeaponType = eItemWeaponTypeWeaponSpi;
				break;
            }
        }

        // 아이템 종류 -> 방어구 -> 착용위치
        int nArmorType = eItemArmorTypeAll;

        {
            /*
            SUIT_HEADGEAR		= 0,
            SUIT_UPPER			= 1,
            SUIT_LOWER			= 2,
            SUIT_HAND			= 3,
            SUIT_FOOT			= 4,

            SUIT_HANDHELD		= 5,

            SUIT_NECK			= 6,
            SUIT_WRIST			= 7,

            SUIT_FINGER			= 8,

            SUIT_PET_A			= 9,	// PetData
            SUIT_PET_B			= 10,
            SUIT_VEHICLE		= 11,	// 탈것

            SUIT_VEHICLE_SKIN		= 12,	// 탈것 중 보드 스킨
            SUIT_VEHICLE_PARTS_A	= 13,	// 탈것 중 보드 스킨
            SUIT_VEHICLE_PARTS_B	= 14,	// 탈것 중 보드 파츠
            SUIT_VEHICLE_PARTS_C	= 15,
            SUIT_VEHICLE_PARTS_D	= 16,
            SUIT_VEHICLE_PARTS_E	= 17,
            SUIT_VEHICLE_PARTS_F	= 18,
            SUIT_VEHICLE_PARTS_SIZE = 6,

            SUIT_WAISTBAND      = 19,   //허리띠
            SUIT_EARRING        = 20,   //귀걸이
            SUIT_ACCESSORIES    = 21,   //장신구

            SUIT_NSIZE			= 22,
            */

            switch ( pItembase->sSuitOp.emSuit )
            {
            case SUIT_HEADGEAR:
                nArmorType = eItemArmorTypeHeadgear;
                break;

            case SUIT_UPPER:
                nArmorType = eItemArmorTypeUpper;
                break;

            case SUIT_LOWER:
                nArmorType = eItemArmorTypeLower;
                break;

            case SUIT_HAND:
                nArmorType = eItemArmorTypeHand;
                break;

            case SUIT_FOOT:
                nArmorType = eItemArmorTypeFoot;
                break;

            case SUIT_NECK:
                nArmorType = eItemArmorTypeNeck;
                break;

            case SUIT_WRIST:
                nArmorType = eItemArmorTypeWrist;
                break;

            case SUIT_FINGER:
                nArmorType = eItemArmorTypeFinger;
                break;

            case SUIT_WAISTBAND:
                nArmorType = eItemArmorTypeWaistband;
                break;

			/*
			// #CRM 1059 개인상점 시스템 필터 타입 삭제 요청
			// http://crm.mincoms.com/Ticket/TicketView/1056
			case SUIT_EARRING:
				nArmorType = eItemArmorTypeEarring;
				break;

			case SUIT_ACCESSORIES:
				nArmorType = eItemArmorTypeAccessories;
				break;
			*/

			// 20160622 다시 해제
			case SUIT_EARRING:
				nArmorType = eItemArmorTypeEarring;
				break;

			case SUIT_ACCESSORIES:
				nArmorType = eItemArmorTypeAccessories;
				break;
            }
        }

        // 기본 데미지
        int nBasicDamage = 0;

        // 기본 필요 sp
        int nBasicReqquireSP = 0;

        // 기본 방어력
        int nBasicDefence = 0;

        // 기본 명중률
        int nBasicHitrate = 0;

        // 기본 회피율
        int nBasicAvoidrate = 0;

        // 기본 공격 연마
        int nBasicGrindingDamage = 0;

        // 기본 방어 연마
        int nBasicGrindingdefence = 0;

        // 기본 경험치
        float fExpMultiple = 0.0f;

        // 저항력 불
        int nFireResist = 0;

        // 저항력 얼음
        int nIceResist = 0;

        // 저항력 전기
        int nElectricResist = 0;

        // 저항력 독
        int nPoisonResist = 0;

        // 저항력 정기
        int nSpiritResist = 0;

        // 모든 저항력
        int nAllResist = 0;

        {
            //nBasicDamage = static_cast< int >( pItembase->sSuitOp.gdDamage.wLow );
			nBasicDamage = static_cast< int >( sItemcustom.GetBasicAttackDamage().GetLow() );
            nBasicReqquireSP = static_cast< int >( pItembase->sSuitOp.wReqSP );
            //nBasicDefence = pItembase->sSuitOp.nDefense;
			nBasicDefence = sItemcustom.GetBasicDefence();
            nBasicHitrate = pItembase->sSuitOp.nHitRate;
            nBasicAvoidrate = pItembase->sSuitOp.nAvoidRate;
            nBasicGrindingDamage = static_cast< int >( sItemcustom.cDAMAGE );
            nBasicGrindingdefence = static_cast< int >( sItemcustom.cDEFENSE );
            fExpMultiple = pItembase->sBasicOp.fExpMultiple;
			fExpMultiple = (float)(fExpMultiple - 1 + FLT_EPSILON) * 100.0f;
            nFireResist = pItembase->sSuitOp.sResist.nFire;
            nIceResist = pItembase->sSuitOp.sResist.nIce;
            nElectricResist = pItembase->sSuitOp.sResist.nElectric;
            nPoisonResist = pItembase->sSuitOp.sResist.nPoison;
            nSpiritResist = pItembase->sSuitOp.sResist.nSpirit;

            if ( 
                nFireResist > 0 && 
                nIceResist > 0 && 
                nElectricResist > 0 && 
                nPoisonResist > 0 && 
                nSpiritResist > 0 )
            {
                std::vector< int > vecResist;
                vecResist.push_back( nFireResist );
                vecResist.push_back( nIceResist );
                vecResist.push_back( nElectricResist );
                vecResist.push_back( nPoisonResist );
                vecResist.push_back( nSpiritResist );

                std::vector< int >::iterator iterMin =  std::min_element(vecResist.begin(), vecResist.end());
                nAllResist = *iterMin;
            }
        }        

        int nAddonRandomHitrate = 0;
        int nAddonRandomAvoidrate = 0;
        int nAddonRandomDamage = 0;
        int nAddonRandomDefence = 0;
        int nAddonRandomHp = 0;
        int nAddonRandomMp = 0;
        int nAddonRandomSp = 0;
        int nAddonRandomStatsPow = 0;
        int nAddonRandomStatsStr = 0;
        int nAddonRandomStatsSpi = 0;
        int nAddonRandomStatsDex = 0;
        int nAddonRandomStatsInt = 0;
        int nAddonRandomStatsSta = 0;
        int nAddonRandomPA = 0;
        int nAddonRandomSA = 0;
        int nAddonRandomMA = 0;

        float fVarRandomHpRate = 0.0f;
        float fVarRandomMpRate = 0.0f;
        float fVarRandomSpRate = 0.0f;
        float fVarRandomHpMpSpRate = 0.0f;
        float fVarRandomMoveSpeedRate = 0.0f;
        float fVarAttackSpeedRate = 0.0f;
        float fVarCriticalRate = 0.0f;
        float fVarCrushingBlowRate = 0.0f;

        int nVarRandomHp = 0;
        int nVarRandomMp = 0;
        int nVarRandomSp = 0;
        int nVarRandomHpMpSp = 0;
        int nVarRandomMoveSpeed = 0;
        float fVarAttackSpeed = 0.0f;
        int nVarCritical = 0;
        int nVarCrushingBlow = 0;
        int nBlowType = eItemStateBlowNone;

        {
            for ( int loop = 0; loop < ITEM::SSUIT::ADDON_SIZE; ++loop )
            {
                switch ( pItembase->sSuitOp.sADDON[loop].emTYPE )
                {
                case EMADD_NONE:
                    break;

                case EMADD_HITRATE:
                    nAddonRandomHitrate += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_AVOIDRATE:
                    nAddonRandomAvoidrate += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;

                case EMADD_DAMAGE:
                    nAddonRandomDamage += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_DEFENSE:
                    nAddonRandomDefence += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;

                case EMADD_HP:
                    nAddonRandomHp += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_MP:
                    nAddonRandomMp += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_SP:
                    nAddonRandomSp += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;

                case EMADD_STATS_POW:
                    nAddonRandomStatsPow += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_STATS_STR:
                    nAddonRandomStatsStr += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_STATS_SPI:
                    nAddonRandomStatsSpi += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_STATS_DEX:
                    nAddonRandomStatsDex += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_STATS_INT:
                    nAddonRandomStatsInt += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_STATS_STA:
                    nAddonRandomStatsSta += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;

                case EMADD_PA:
                    nAddonRandomPA += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_SA:
                    nAddonRandomSA += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
                case EMADD_MA:	// 찜찜한데 우선 이렇게 계산해본다.
                    nAddonRandomMA += pItembase->sSuitOp.sADDON[loop].nVALUE;
                    break;
				case EMADDEX_INCR_ATTACK_SPEED:
					fVarAttackSpeedRate = pItembase->sSuitOp.sADDON[loop].fVALUE;
					fVarAttackSpeedRate *= COMMENT::ITEMADDON_VAR_SCALE[EMADDEX_INCR_ATTACK_SPEED];
					break;
				case EMADDEX_INCR_CRITICAL_RATE:
					fVarCriticalRate = pItembase->sSuitOp.sADDON[loop].fVALUE;
					fVarCriticalRate *= COMMENT::ITEMADDON_VAR_SCALE[EMADDEX_INCR_CRITICAL_RATE];
					break;
				case EMADDEX_INCR_CRUSHING_BLOW:
					fVarCrushingBlowRate = pItembase->sSuitOp.sADDON[loop].fVALUE;
					fVarCrushingBlowRate *= COMMENT::ITEMADDON_VAR_SCALE[EMADDEX_INCR_CRUSHING_BLOW];
					break;
				case EMADDEX_INC_HP:
					nVarRandomHp += static_cast< int >( pItembase->sSuitOp.sADDON[loop].fVALUE );
					break;
				case EMADDEX_INC_MP:
					nVarRandomMp += static_cast< int >( pItembase->sSuitOp.sADDON[loop].fVALUE );
					break;
				case EMADDEX_INC_SP:
					nVarRandomSp += static_cast< int >( pItembase->sSuitOp.sADDON[loop].fVALUE );
					break;
				case EMADDEX_INC_AP:
					nVarRandomHpMpSp += static_cast< int >( pItembase->sSuitOp.sADDON[loop].fVALUE );
					break;
				case EMADDEX_INC_MOVE_SPEED:
					nVarRandomMoveSpeed += static_cast< int >( pItembase->sSuitOp.sADDON[loop].fVALUE );
					break;
				case EMADDEX_INC_ATTACK_SPEED:
					fVarAttackSpeed += ( pItembase->sSuitOp.sADDON[loop].fVALUE + FLT_EPSILON );
					break;
				case EMADDEX_INC_CRITICAL_RATE:
					nVarCritical += static_cast< int >( pItembase->sSuitOp.sADDON[loop].fVALUE );
					break;
				case EMADDEX_INC_CRUSHING_BLOW:
					nVarCrushingBlow += static_cast< int >( pItembase->sSuitOp.sADDON[loop].fVALUE );
					break;
                };
            }

            nAddonRandomHp += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_HP ) );
            nAddonRandomMp += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_MP ) );
            nAddonRandomSp += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_SP ) );

            nAddonRandomPA += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_MELEE ) );
            nAddonRandomSA += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_RANGE ) );

            // 찜찜한데 우선 이렇게 계산해본다.
            nAddonRandomMA += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_MAGIC ) );

            nAddonRandomStatsPow += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_POW ) );
            nAddonRandomStatsStr += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_STR ) );
            nAddonRandomStatsSpi += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_SPI ) );
            nAddonRandomStatsDex += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_DEX ) );
            nAddonRandomStatsSta += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_STA ) );

            nAddonRandomHitrate += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_RATE_HIT ) );
            nAddonRandomHitrate += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_HIT ) );

            nAddonRandomAvoidrate += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_RATE_AVOID ) );
            nAddonRandomAvoidrate += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_AVOID ) );

            {
                //GLPADATA sDAMAGE = pItembase->sSuitOp.gdDamage;
				GLPADATA sDAMAGE = sItemcustom.GetBasicAttackDamage();
                float fRATE  = sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_RATE_DAMAGE );
                float fVALUE = sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_DAMAGE );

                if ( 0.0f != fRATE || 0.0f != fVALUE )
                {
                    //sDAMAGE.wLow = static_cast< WORD >( pItembase->sSuitOp.gdDamage.wLow * ( 100.0f + fRATE ) * 0.01f );
					sDAMAGE.wLow = static_cast< WORD >( sItemcustom.GetBasicAttackDamage().GetLow() * ( 100.0f + fRATE ) * 0.01f );
                    sDAMAGE.VAR_PARAM( static_cast< int >( fVALUE ) );

                    nAddonRandomDamage += static_cast< int >( sDAMAGE.wLow );
                }
            }

            {
                //short nDEFENSE = pItembase->sSuitOp.nDefense;
				short nDEFENSE = sItemcustom.GetBasicDefence();

                float fRATE  = sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_RATE_DEFENCE );
                float fVALUE = sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_DEFENCE );

                if ( 0.0f != fRATE || 0.0f != fVALUE )
                {
                    nDEFENSE = static_cast< short >( nDEFENSE * ( 100.0f + fRATE ) * 0.01f ) + static_cast< short >( fVALUE );

                    nAddonRandomDefence += nDEFENSE;
                }
            }
			
			fVarRandomHpRate = sItemcustom.GETINCHP();
			fVarRandomHpRate *= COMMENT::ITEMVAR_SCALE[EMVAR_HP];
			fVarRandomMpRate = sItemcustom.GETINCMP();
			fVarRandomMpRate *= COMMENT::ITEMVAR_SCALE[EMVAR_MP];
			fVarRandomSpRate = sItemcustom.GETINCSP();
			fVarRandomSpRate *= COMMENT::ITEMVAR_SCALE[EMVAR_SP];
			fVarRandomHpMpSpRate = sItemcustom.GETINCAP();
			fVarRandomHpMpSpRate *= COMMENT::ITEMVAR_SCALE[EMVAR_AP];
			fVarRandomMoveSpeedRate = sItemcustom.GETMOVESPEEDR();
			fVarRandomMoveSpeedRate *= COMMENT::ITEMVAR_SCALE[EMVAR_MOVE_SPEED];

            nVarRandomHp += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_HP ) );
            nVarRandomMp += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_MP ) );
            nVarRandomSp += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_VALUE_SP ) );
            nVarRandomMoveSpeed += static_cast< int >( sItemcustom.GETOptVALUE( RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE ) );

            /*
            EMBLOW_NONE		= 0,	//	상태 이상 유발 없음.
            EMBLOW_NUMB		= 1,	//	마비.
            EMBLOW_STUN		= 2,	//	기절.
            EMBLOW_STONE		= 3,	//	석화.
            EMBLOW_BURN			= 4,	//	화염.
            EMBLOW_FROZEN		= 5,	//	냉동.
            EMBLOW_SINGLE		= 5,	//	단독으로만 발생하는 "상태이상" 효과.

            EMBLOW_MAD			= 6,	//	착란.
            EMBLOW_POISON		= 7,	//	중독.
            EMBLOW_CURSE		= 8,	//	저주.
            EMBLOW_PANT		= 9,	//	헐떡임.

            EMBLOW_SIZE			= 10,
            EMBLOW_MULTI		= 4,
            */
            switch ( pItembase->sSuitOp.sBLOW.emTYPE )
            {
            case EMBLOW_NONE:
                nBlowType = eItemStateBlowNone;
                break;
            case EMBLOW_NUMB:
                nBlowType = eItemStateBlowNumb;
                break;
            case EMBLOW_STUN:
                nBlowType = eItemStateBlowStun;
                break;
            case EMBLOW_STONE:
                nBlowType = eItemStateBlowStone;
                break;
            case EMBLOW_BURN:
                nBlowType = eItemStateBlowBurn;
                break;
            case EMBLOW_FROZEN:
                nBlowType = eItemStateBlowFrozen;
                break;
            case EMBLOW_MAD:
                nBlowType = eItemStateBlowMad;
                break;
            case EMBLOW_POISON:
                nBlowType = eItemStateBlowPoison;
                break;
            case EMBLOW_CURSE:
                nBlowType = eItemStateBlowCurse;
                break;
            }
        }

        // 개수, 사용가능량
        // dwTurnNum 으로 대체

        // 아이템이름
        std::string strItemName("default");
        std::string strItemNameUtf8("default");

        std::string strChaNameUtf8("RanOnline");

        {
            strItemName = pItembase->GetNameStr();
        }

        if ( false == m_SaleItem.BeginTransaction() )
        {
            return false;
        }
        else
        {
            // ANSI코드->유니코드->UTF8 로 변경 해야 바인드 해줄 때 깨지지 않는다.
            strItemNameUtf8 = sc::string::ansi_to_utf8( strItemName );
            strChaNameUtf8 = sc::string::ansi_to_utf8( strConsignmentSalllerName );
        }

        sqlite3_bind_int( m_stmtSaleItemInsert, 1, dwChaDbNum );
        sqlite3_bind_int( m_stmtSaleItemInsert, 2, nChannel );
        sqlite3_bind_int( m_stmtSaleItemInsert, 3, wSchool );
        sqlite3_bind_int64( m_stmtSaleItemInsert, 4, llPrice );
        sqlite3_bind_int( m_stmtSaleItemInsert, 5, dwTurnNum );

        std::string strGUID = sc::string::uuidToString( sItemGUID );
        sqlite3_bind_text( m_stmtSaleItemInsert, 6, strGUID.c_str(), strGUID.size(), SQLITE_STATIC );
        sqlite3_bind_int( m_stmtSaleItemInsert, 7, nType );
        sqlite3_bind_int( m_stmtSaleItemInsert, 8, nWeaponType );
        sqlite3_bind_int( m_stmtSaleItemInsert, 9, nArmorType );
        sqlite3_bind_int( m_stmtSaleItemInsert, 10, nTimeLimit );

        sqlite3_bind_int( m_stmtSaleItemInsert, 11, nRequireLevel );
        sqlite3_bind_int( m_stmtSaleItemInsert, 12, nRequirePow );
        sqlite3_bind_int( m_stmtSaleItemInsert, 13, nRequireDex );
        sqlite3_bind_int( m_stmtSaleItemInsert, 14, nRequireSpi );
        sqlite3_bind_int( m_stmtSaleItemInsert, 15, static_cast< int >( dwEquipClassSex ) );

        sqlite3_bind_text( m_stmtSaleItemInsert, 16, strItemNameUtf8.c_str(), strItemNameUtf8.length(), SQLITE_STATIC );
        sqlite3_bind_int( m_stmtSaleItemInsert, 17, nRank );
        sqlite3_bind_int( m_stmtSaleItemInsert, 18, nBasicDamage );
        sqlite3_bind_int( m_stmtSaleItemInsert, 19, nBasicReqquireSP );
        sqlite3_bind_int( m_stmtSaleItemInsert, 20, nBasicDefence );

        sqlite3_bind_int( m_stmtSaleItemInsert, 21, nBasicHitrate );
        sqlite3_bind_int( m_stmtSaleItemInsert, 22, nBasicAvoidrate );
        sqlite3_bind_int( m_stmtSaleItemInsert, 23, nBasicGrindingDamage );
        sqlite3_bind_int( m_stmtSaleItemInsert, 24, nBasicGrindingdefence );
        sqlite3_bind_double( m_stmtSaleItemInsert, 25, fExpMultiple );

        sqlite3_bind_int( m_stmtSaleItemInsert, 26, nAllResist );
        sqlite3_bind_int( m_stmtSaleItemInsert, 27, nFireResist );
        sqlite3_bind_int( m_stmtSaleItemInsert, 28, nIceResist );
        sqlite3_bind_int( m_stmtSaleItemInsert, 29, nElectricResist );
        sqlite3_bind_int( m_stmtSaleItemInsert, 30, nPoisonResist );

        sqlite3_bind_int( m_stmtSaleItemInsert, 31, nSpiritResist );
        sqlite3_bind_int( m_stmtSaleItemInsert, 32, nAddonRandomHitrate );
        sqlite3_bind_int( m_stmtSaleItemInsert, 33, nAddonRandomAvoidrate );
        sqlite3_bind_int( m_stmtSaleItemInsert, 34, nAddonRandomDamage );
        sqlite3_bind_int( m_stmtSaleItemInsert, 35, nAddonRandomDefence );

        sqlite3_bind_int( m_stmtSaleItemInsert, 36, nAddonRandomHp );
        sqlite3_bind_int( m_stmtSaleItemInsert, 37, nAddonRandomMp );
        sqlite3_bind_int( m_stmtSaleItemInsert, 38, nAddonRandomSp );
        sqlite3_bind_int( m_stmtSaleItemInsert, 39, nAddonRandomStatsPow );
        sqlite3_bind_int( m_stmtSaleItemInsert, 40, nAddonRandomStatsStr );

        sqlite3_bind_int( m_stmtSaleItemInsert, 41, nAddonRandomStatsSpi );
        sqlite3_bind_int( m_stmtSaleItemInsert, 42, nAddonRandomStatsDex );
        sqlite3_bind_int( m_stmtSaleItemInsert, 43, nAddonRandomStatsInt );
        sqlite3_bind_int( m_stmtSaleItemInsert, 44, nAddonRandomStatsSta );
        sqlite3_bind_int( m_stmtSaleItemInsert, 45, nAddonRandomPA );

        sqlite3_bind_int( m_stmtSaleItemInsert, 46, nAddonRandomSA );
        sqlite3_bind_int( m_stmtSaleItemInsert, 47, nAddonRandomMA );
        sqlite3_bind_double( m_stmtSaleItemInsert, 48, fVarRandomHpRate );
        sqlite3_bind_double( m_stmtSaleItemInsert, 49, fVarRandomMpRate );
        sqlite3_bind_double( m_stmtSaleItemInsert, 50, fVarRandomSpRate );

        sqlite3_bind_double( m_stmtSaleItemInsert, 51, fVarRandomHpMpSpRate );
        sqlite3_bind_double( m_stmtSaleItemInsert, 52, fVarRandomMoveSpeedRate );
        sqlite3_bind_double( m_stmtSaleItemInsert, 53, fVarAttackSpeedRate );
        sqlite3_bind_double( m_stmtSaleItemInsert, 54, fVarCriticalRate );
        sqlite3_bind_double( m_stmtSaleItemInsert, 55, fVarCrushingBlowRate );

        sqlite3_bind_int( m_stmtSaleItemInsert, 56, nVarRandomHp );
        sqlite3_bind_int( m_stmtSaleItemInsert, 57, nVarRandomMp );
        sqlite3_bind_int( m_stmtSaleItemInsert, 58, nVarRandomSp );
        sqlite3_bind_int( m_stmtSaleItemInsert, 59, nVarRandomHpMpSp );
        sqlite3_bind_int( m_stmtSaleItemInsert, 60, nVarRandomMoveSpeed );

        sqlite3_bind_double( m_stmtSaleItemInsert, 61, fVarAttackSpeed );
        sqlite3_bind_int( m_stmtSaleItemInsert, 62, nVarCritical );
        sqlite3_bind_int( m_stmtSaleItemInsert, 63, nVarCrushingBlow );
        sqlite3_bind_int( m_stmtSaleItemInsert, 64, nBlowType );

        sqlite3_bind_double( m_stmtSaleItemInsert, 65, fCommissionRate );

        sqlite3_bind_int( m_stmtSaleItemInsert, 66, static_cast<int>( emSellType ) );

        sqlite3_bind_text( m_stmtSaleItemInsert, 67, strChaNameUtf8.c_str(), strChaNameUtf8.length(), SQLITE_STATIC );

        sqlite3_bind_int64( m_stmtSaleItemInsert, 68, ExpirationDate );

		sqlite3_bind_int( m_stmtSaleItemInsert, 69, nServerGroup );

        sqlite3_step( m_stmtSaleItemInsert );

        m_SaleItem.CommitTransaction();

        // 컴파일된 m_stmtSaleItemInsert 을 계속 사용하려면 reset 해줘야 한다.
        sqlite3_reset( m_stmtSaleItemInsert );

        // 그냥 ExecuteSQL 를 사용해도 되지만 한번 컴파일한 구문을 계속해서 사용하기 위해서 이렇게 한다.
        // ExecuteSQL 비슷한 과정으로 진행하지만 호출할 때마다. 객체를 생성하고 파괴한다. 
        // sqlite3_step 은 결과값을 받지않는 insert, update, delete 문은 한번만 호출해도 된다. 
        // 결과는 반환하는 구문은 계속해서 sqlite3_step 를 호출해야 한다.

        // 실제 아이템정보 insert
        if ( m_pSaleItem )
        {
            // strGUID 가 중복되지만 않는다면 insert 된다.
			m_pSaleItem->InsertSaleItem( strGUID, sItemcustom, dwChaDbNum, emSellType == eSellType_ConsignmentSale_RanPoint ? true : false, nServerGroup );
        }

        double fElapsed = SaleItemInsertTimer.elapsed();
        if ( fElapsed > 1.0f )
        {
            sc::writeLogWarn( sc::string::format( "InsertSaleItem taken %1% second.", fElapsed ) );
        }

        return eSuccess;
    }

    int GLPrivateMarketSearchSQLite::InsertHoldItem( std::string& strGUID, DWORD dwFieldSlot, DWORD dwChaDbNum, const char* szName, std::string& strQuery, int nSellType, int nServerGroup  )
    {

        SQLite::TablePtr pSaleItemsTable( m_SaleItem.QuerySQL2( strQuery.c_str() ) );
        if ( !pSaleItemsTable.m_pTable )
        {
            return eNullTablePointer;
        }

        SQLite::Table& rSaleItemsTable = pSaleItemsTable();
        int nSaleItemsRowCount = rSaleItemsTable.GetRowCount();
        if ( 0 == nSaleItemsRowCount )
        {
            // 아이템이 없다.
            return eNotExistSaleItem;
        }

        DWORD dwSellerChaDbNum = rSaleItemsTable.GetValue< DWORD >( 0 );
        DWORD dwRemainItemTurnNum = rSaleItemsTable.GetValue< DWORD >( 1 );
        LONGLONG llItemPrice = rSaleItemsTable.GetValue< LONGLONG >( 2 );
        int nSellerChannel = rSaleItemsTable.GetValue< int >( 3 );
        DWORD dwSellerFieldSlot = rSaleItemsTable.GetValue< DWORD >( 4 );
        float fCommissionRate = rSaleItemsTable.GetValue< float >( 5 );
        std::string SellerName = rSaleItemsTable.GetValue< std::string >( 6 );
		int nSellerServerGroup = rSaleItemsTable.GetValue< int >( 7 );

        // Hold 시킨다.
        std::string strBuyerName( "empty" );
        if ( NULL != szName )
        {
            strBuyerName = szName;
        }

		/*
        // ANSI코드->유니코드->UTF8 로 변경 해야 바인드 해줄 때 깨지지 않는다.
        std::string strBuyerNameUtf8("default");
        strBuyerNameUtf8 = sc::string::ansi_to_utf8(strBuyerName);
		*/

        std::string strHoldItemInsertQuery( 
            sc::string::format( 
            "\
            INSERT INTO HoldItems( \
            GUID, SellerChaNum, SellerFieldSlot, SellerChannel, TurnNum, \
            Price, BuyerChaNum, BuyerFieldSlot, BuyerChaName, CommissionRate, \
            SellType, ConsigmentSallerName, SellerServerGroup, BuyerServerGroup ) \
            VALUES( '%1%', %2%, %3%, %4%, %5%, %6%, %7%, %8%, '%9%', %10%, %11%, '%12%', %13%, %14% );", 
            strGUID, dwSellerChaDbNum, dwSellerFieldSlot, nSellerChannel, dwRemainItemTurnNum, 
            llItemPrice, dwChaDbNum, dwFieldSlot, strBuyerName.c_str(), fCommissionRate,
            nSellType, SellerName.c_str(), nSellerServerGroup, nServerGroup ) );

        int nHoldItemInsertResult = execute( strHoldItemInsertQuery );
        if ( SQLITE_OK != nHoldItemInsertResult )
        {
            return eHoldItemInsertFail;
        }

        return eSuccess;
    }

	GLLastLogNumMgr::GLLastLogNumMgr()
	{
	}

	GLLastLogNumMgr::~GLLastLogNumMgr()
	{
		m_theContainer.clear();
	}

	LONGLONG GLLastLogNumMgr::GetLastLogNum( DWORD dwChaDbNum, int ServerGroup )
	{
		CHAINFO chainfo( dwChaDbNum, ServerGroup );
		LAST_LOG_NUMBER_ITER it = m_theContainer.find( chainfo );
		if ( it != m_theContainer.end() )
		{
			return ( *it ).second;
		}
		else
		{
			LONGLONG llLastLogNum = 0;
			std::pair< LAST_LOG_NUMBER_ITER, bool > retPair = m_theContainer.insert( LAST_LOG_NUMBER_VALUE( chainfo, llLastLogNum ) );
			return llLastLogNum;
		}
	}

	void GLLastLogNumMgr::SetLastLogNum( DWORD dwChaDbNum, int ServerGroup, LONGLONG llLogNum )
	{
		CHAINFO chainfo( dwChaDbNum, ServerGroup );
		LAST_LOG_NUMBER_ITER it = m_theContainer.find( chainfo );
		if ( it != m_theContainer.end() )
		{
			( *it ).second = llLogNum;
		}
		else
		{
			std::pair< LAST_LOG_NUMBER_ITER, bool > retPair = m_theContainer.insert( LAST_LOG_NUMBER_VALUE( chainfo, llLogNum ) );
		}
	}

	void GLLastLogNumMgr::DeleteLastLogNum( DWORD dwChaDbNum, int ServerGroup )
	{
		CHAINFO chainfo( dwChaDbNum, ServerGroup );
		LAST_LOG_NUMBER_ITER it = m_theContainer.find( chainfo );
		if ( it != m_theContainer.end() )
		{
			m_theContainer.erase( it );
		}
	}

	// 아이템 검색하기 전에 위탁판매 아이템 정리
	// 판매 기간이 지난 란포인트 거래 타입, 골드 거래 타입 삭제
    int GLPrivateMarketSearchSQLite::CleanUpConsigmentItem()
    {
		{
			// 아이템 삭제하기
			std::string strSaleItemsSelectQuery = 
				sc::string::format( "\
									SELECT GUID, SellerChaNum, ServerGroup \
									FROM SaleItems \
									WHERE ExpirationDate<=%1% AND SaleItems.SellType<>%2%;", 
									CTime::GetCurrentTime().GetTime(), eSellType_PrivateMarket );

			SQLite::TablePtr pSaleItemsTable( m_SaleItem.QuerySQL2( strSaleItemsSelectQuery.c_str() ) );
			if ( pSaleItemsTable.m_pTable )
			{
				SQLite::Table& rSaleItemsTable = pSaleItemsTable();
				int nSaleItemsRowCount = rSaleItemsTable.GetRowCount();

				for ( int loop = 0; loop < nSaleItemsRowCount; ++loop )
				{
					std::string strGUID = rSaleItemsTable.GetValue< std::string >( 0 );
					DWORD dwSellerChaDbNum = rSaleItemsTable.GetValue< DWORD >( 1 );
					DWORD nServerGroup = rSaleItemsTable.GetValue< int >( 2 );
					
					if ( m_pSaleItem )
					{
						m_pSaleItem->DeleteSaleItem( strGUID, dwSellerChaDbNum, nServerGroup );
					}
					
					rSaleItemsTable.GoNext();
				}
			}
		}

        std::string strSaleItemDeleteQuery( 
            sc::string::format( 
            "\
            DELETE FROM SaleItems \
            WHERE ExpirationDate<=%1% AND SaleItems.SellType<>%2%", 
            CTime::GetCurrentTime().GetTime(), eSellType_PrivateMarket ) );

        int nSaleItemDeleteResult = execute( strSaleItemDeleteQuery );
        if ( SQLITE_OK != nSaleItemDeleteResult )
        {
            return eSaleItemDeleteFail;
        }

        return eSuccess;
    }

	int GLPrivateMarketSearchSQLite::BackupDatabase( sqlite3* pDb, const char * szFilename )
	{
		int rc; /* Function return code */ 
		sqlite3 * pFile; /* Database connection opened on zFilename */ 
		sqlite3_backup * pBackup; /* Backup handle used to copy data */ 

		/* Open the database file identified by zFilename. */ 
		rc = sqlite3_open ( szFilename, &pFile); 
		if ( rc == SQLITE_OK )
		{
			/* Open the sqlite3_backup object used to accomplish the transfer */ 
			pBackup = sqlite3_backup_init ( pFile, "main" , pDb, "main" ); 
			if ( pBackup )
			{
				/* Each iteration of this loop copies 5 database pages from database 
				** pDb to the backup database. If the return value of backup_step() 
				** indicates that there are still further pages to copy, sleep for 
				** 250 ms before repeating. */ 
				do
				{ 
					rc = sqlite3_backup_step ( pBackup, 5 ); 
					if ( rc== SQLITE_OK || rc== SQLITE_BUSY || rc== SQLITE_LOCKED )
					{ 
						sqlite3_sleep ( 250 ); 
					}
				} 
				while ( rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED );

				/* Release resources allocated by backup_init(). */ 
				( void ) sqlite3_backup_finish ( pBackup ); 
			} 
			rc = sqlite3_errcode ( pFile); 
		} 

		/* Close the database connection opened on database file zFilename 
		** and return the result of this function. */ 
		( void ) sqlite3_close ( pFile); 
		return rc;
	}
}

