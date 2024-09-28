#pragma once

#include <string>
#include <vector>

#include "../../SigmaCore/Database/Sqlite/SQLite.h"

#include "../../RanLogic/Market/GLSaleItem.h"

#include "./IPrivateMarketSearch.h"

namespace private_market
{
	class GLSaleItem;

	class GLLastLogNumMgr
	{
	public:
		GLLastLogNumMgr();
		~GLLastLogNumMgr();

	public:
		typedef std::map< CHAINFO, LONGLONG > LAST_LOG_NUMBER;
		typedef LAST_LOG_NUMBER::iterator LAST_LOG_NUMBER_ITER;
		typedef LAST_LOG_NUMBER::const_iterator LAST_LOG_NUMBER_CITER;
		typedef LAST_LOG_NUMBER::value_type LAST_LOG_NUMBER_VALUE;

	public:
		LONGLONG GetLastLogNum( DWORD dwChaDbNum, int ServerGroup );
		void SetLastLogNum( DWORD dwChaDbNum, int ServerGroup, LONGLONG llLogNum );
		void DeleteLastLogNum( DWORD dwChaDbNum, int ServerGroup );

	protected:
		LAST_LOG_NUMBER m_theContainer;

	};

	class GLPrivateMarketSearchSQLite : public IPrivateMarketSearch
	{
	public:
		GLPrivateMarketSearchSQLite();
		virtual ~GLPrivateMarketSearchSQLite();

	public:
		virtual int InsertSeller( 
			DWORD dwChaDbNum, 
			const char* szChaName, 
			const SNATIVEID& sMapID, 
			WORD wSchool, 
			int nChannel, 
			DWORD dwFieldSlot, 
			int nServerGroup );

		virtual int InsertSaleItem( 
			DWORD dwChaDbNum, 
			UUID& sItemGUID, 
			const SITEMCUSTOM& sItemcustom,
			const DWORD dwTurnNum, 
			const LONGLONG& llPrice,
			const float fCommissionRate,
            __time64_t ExpirationDate, 
			int nServerGroup );

        virtual int InsertConsigmentSaleItem( 
            std::string strChaName, 
            DWORD dwChaDbNum, 
            UUID& sItemGUID, 
            const SITEMCUSTOM& sItemcustom,
            const DWORD dwTurnNum, 
            const LONGLONG& llPrice,
            const float fCommissionRate,
            eSellType emSellType,
            __time64_t ExpirationDate, 
			int nServerGroup );

		virtual int DeleteItem( UUID& sItemGUID );
		virtual int DeleteItem( DWORD dwChaDbNum, int nServerGroup );

		virtual int GetSaleItems( 
			WORD wPage, 
			int nChannel, 
			WORD wSchool, 
			sSaleItemSearchandSortBasic& sSearch, 
			std::vector< sSaleItem >& vecSaleItems, 
			WORD& wTotalCount, 
			int nServerGroup );

		virtual int GetSaleItems( 
			WORD wPage, 
			int nChannel, 
			WORD wSchool, 
			std::string& strItemName, 
			int nSortOption, 
			int nSortMethod, 
			std::vector< sSaleItem >& vecSaleItems, 
			WORD& wTotalCount, 
			int nServerGroup );

        // 위탁판매가 나중에 끼어듬에따라 쿼리의 변동이 생김.
        virtual int PrepareItemBuy( 
            UUID& sItemGUID,
            DWORD dwItemTurnNum, 
            DWORD dwFieldSlot, 
            DWORD dwChaDbNum, 
            const char* szName, 
            LONGLONG& llPriceTotal OUT,
            int& nSellType OUT, 
			int nServerGroup );

		virtual int DeleteHoldItem( UUID& sItemGUID );

		virtual int GetHoldItem( UUID& sItemGUID, sHoldItem& sItem );

		virtual int UpdateItem( UUID& sItemGUID, DWORD dwItemTurnNum );

		virtual bool IsHoldItem( UUID& sItemGUID );

		virtual int InsertHoldItem( UUID& sItemGUID, DWORD dwFieldSlot, DWORD dwChaDbNum, const char* szName, int nServerGroup );

		virtual int InsertSaleLog( UUID& sItemGUID );

		virtual void FrameMove();

		virtual int GetSaleLog( DWORD dwChaDbNum, std::vector< sSaleLog >& vecSaleLog, int nServerGroup );

		virtual void UpdateBasicOption( 
			bool bChannelCheck, 
			bool bSchoolCheck, 
			bool bAllPage, 
			WORD wSearchItemNumberPerPage, 
			WORD wLogNumberPerPage, 
			WORD wLogNumberMaxPerCharacter, 
			int nSearchOption );

		virtual void GetBasicOption( 
			bool& bChannelCheck, 
			bool& bSchoolCheck, 
			bool& bAllPage, 
			WORD& wSearchItemNumberPerPage, 
			WORD& wLogNumberPerPage, 
			WORD& wLogNumberMaxPerCharacter, 
			int& nSearchOption );

		virtual void UpdateChannelOption( bool bChannelCheck );
		virtual void UpdateSchoolOption( bool bSchoolCheck );
		virtual void UpdatePageOption( bool bAllPage );
		virtual void UpdateSearchOption( int nValue );

		int DeleteSeller( DWORD dwSellerFieldSlot );

		void FrameMoveSaleLog();
		int BackupDatabase( sqlite3* pDb, const char * szFilename );	// 느림 사용X

	protected:
		int openDatabase();
		void closeDatabase();
		int createTable();
		void prepareSQLStatement();
		void finalizeSQLStatement();
		int execute( const std::string& strQuery, bool bDisplayError = true );
		int executeForSaleLog( const std::string& strQuery );

		void validateSearchOption( sSaleItemSearchandSortBasic& sSearch );
		std::string makeSearchQuery( WORD wPage, int nChannel, WORD wSchool, sSaleItemSearchandSortBasic& sSearch, std::string& strCountQuery, int nServerGroup, bool bConsignmentSale = false );
        //std::string makeSearchQueryConsignmentSale( WORD wPage, int nChannel, WORD wSchool, sSaleItemSearchandSortBasic& sSearch, std::string& strCountQuery );
		bool getWhereChannel( std::string& strQuery, int nChannel );
		bool getWhereSchool( std::string& strQuery, WORD wSchool );
		bool getWhereTurnNum( std::string& strQuery, DWORD dwItemTurnNum );
		bool getWhereClassAndSex( std::string& strQuery, int nClass, int nSex );
		bool getWhereSearchable( std::string& strQuery, int nBuyerServerGroup );
		bool getWhereType( std::string& strQuery, int nType );
		bool getWhereWeaponType( std::string& strQuery, int nWeaponType );
		bool getWhereArmorType( std::string& strQuery, int nArmorType );
		bool getWhereTimelimit( std::string& strQuery, bool bTimelimit );
		bool getWhereItemName( std::string& strQuery, const std::string& strItemName );
		bool getWhereRequire( std::string& strQuery, int nRequire, int nRequireValue );
		bool getWhereRank( std::string& strQuery, int nRank );
		bool getWhereBasic( std::string& strQuery, int nBasic, float fBasicValue );
		bool getWhereAddonVarRandom( std::string& strQuery, int nAddonVarRandom, float fAddonVarRandomValue );
		bool getWhereSerachSellType( std::string& strQuery, int nSerachSellType );

		std::string getOrderBy( int nOption, int nMethod );
		std::string getLimit( WORD wPage, WORD wSelRowNum );

		//bool select( std::string& strQuery, std::vector< sSaleItem >& vecSaleItems );
        bool selectConsignmentSale( std::string& strQuery, std::vector< sSaleItem >& vecSaleItems );

		void deleteHoldItems();

        // 위탁판매가 생김으로써 추가된 함수
        // GUID로 아이템의 판매 타임을 구한다.
        int GetSellType( UUID& sItemGUID, int& nSellType OUT );

        int InsertSaleItem(
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
			int nServerGroup );

        int InsertHoldItem( std::string& strGUID, DWORD dwFieldSlot, DWORD dwChaDbNum, const char* szName, std::string& strQuery, int nSellType, int nServerGroup );

        int CleanUpConsigmentItem();

	protected:
		SQLite::Database m_SaleItem;
		SQLite::Database m_SaleLog;
		GLSaleItem* m_pSaleItem;
		GLLastLogNumMgr* m_pLastLogNum;
		sqlite3_stmt* m_stmtSaleItemInsert;
		std::string m_strSaleItemInsertQuery;

		bool m_bChannelCheck;
		bool m_bSchoolCheck;
		bool m_bAllPage;
		int m_nSearchOption;

		WORD m_wSearchItemNumberPerPage;
		WORD m_wLogNumberPerPage;
		WORD m_wLogNumberMaxPerCharacter;

		DWORD m_dwFrameMoveTimer;
		DWORD m_dwSaleLogFrameMoveTimer;

		BOOL m_bTriggerSaleLogBackup;

	};
}

