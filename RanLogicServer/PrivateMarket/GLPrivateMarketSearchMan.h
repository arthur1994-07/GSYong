#pragma once

#include "../../RanLogic/Market/MarketDefine.h"

namespace private_market
{
	class IPrivateMarketSearch;

	class GLPrivateMarketSearchMan
	{
	public:
		GLPrivateMarketSearchMan();
		~GLPrivateMarketSearchMan();

	public:
		int InsertSeller( 
			DWORD dwChaDbNum, 
			const char* szChaName, 
			const SNATIVEID& sMapID, 
			WORD wSchool, 
			int nChannel, 
			DWORD dwFieldSlot, 
			int nServerGroup );

		int InsertSaleItem( 
			DWORD dwChaDbNum, 
			UUID& sItemGUID, 
			const SITEMCUSTOM& sItemcustom,
			const DWORD dwTurnNum, 
			const LONGLONG& llPrice,
			const float fCommissionRate, 
			int nServerGroup );

        int InsertConsigmentSaleItem( 
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

		int DeleteItem( UUID& sItemGUID );
		int DeleteItem( DWORD dwChaNum, int nServerGroup );

		int GetSaleItems( 
			WORD wPage, 
			int nChannel, 
			WORD wSchool, 
			sSaleItemSearchandSortBasic& sSearch, 
			std::vector< sSaleItem >& vecSaleItems, 
			WORD& wTotalCount, 
			int nServerGroup );

		int GetSaleItems( 
			WORD wPage, 
			int nChannel, 
			WORD wSchool, 
			std::string& strItemName, 
			int nSortOption, 
			int nSortMethod, 
			std::vector< sSaleItem >& vecSaleItems, 
			WORD& wTotalCount, 
			int nServerGroup );

		int PrepareItemBuy( 
			UUID& sItemGUID, 
			DWORD dwItemTurnNum, 
			DWORD dwFieldSlot, 
			DWORD dwChaDbNum, 
			const char* szName, 
			LONGLONG& llPriceTotal OUT,
            int& nSellType OUT, 
			int nServerGroup );

		int DeleteHoldItem( UUID& sItemGUID );

		int GetHoldItem( UUID& sItemGUID, sHoldItem& sItem );

		int UpdateItem( UUID& sItemGUID, DWORD dwItemTurnNum );

		bool IsHoldItem( UUID& sItemGUID );

		int InsertHoldItem( UUID& sItemGUID, DWORD dwFieldSlot, DWORD dwChaDbNum, const char* szName, int nServerGroup );

		int InsertSaleLog( UUID& sItemGUID );

		void FrameMove();

		int GetSaleLog( DWORD dwChaDbNum, std::vector< sSaleLog >& vecSaleLog, int nServerGroup );

		void UpdateBasicOption( 
			bool bChannelCheck, 
			bool bSchoolCheck, 
			bool bAllPage, 
			WORD wSearchItemNumberPerPage, 
			WORD wLogNumberPerPage, 
			WORD wLogNumberMaxPerCharacter, 
			int nSearchOption );

		void GetBasicOption( 
			bool& bChannelCheck, 
			bool& bSchoolCheck, 
			bool& bAllPage, 
			WORD& wSearchItemNumberPerPage, 
			WORD& wLogNumberPerPage, 
			WORD& wLogNumberMaxPerCharacter, 
			int& nSearchOption );

		void UpdateChannelOption( bool bChannelCheck );
		void UpdateSchoolOption( bool bSchoolCheck );
		void UpdatePageOption( bool bAllPage );
		void UpdateSearchOption( int nValue );

		int DeleteSeller( DWORD dwSellerFieldSlot );

	protected:
		IPrivateMarketSearch* m_pImpl;

	};
}

