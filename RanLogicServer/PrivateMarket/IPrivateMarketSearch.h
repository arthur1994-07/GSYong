#pragma once

#include "../../RanLogic/Market/MarketDefine.h"

namespace private_market
{
	class IPrivateMarketSearch
	{
	public:
		IPrivateMarketSearch() {};
		virtual ~IPrivateMarketSearch() {};

	public:
		virtual int InsertSeller( 
			DWORD dwChaDbNum, 
			const char* szChaName, 
			const SNATIVEID& sMapID, 
			WORD wSchool, 
			int nChannel, 
			DWORD dwFieldSlot, 
			int nServerGroup ) = 0;

		virtual int InsertSaleItem( 
			DWORD dwChaDbNum, 
			UUID& sItemGUID, 
			const SITEMCUSTOM& sItemcustom,
			const DWORD dwTurnNum, 
			const LONGLONG& llPrice,
			const float fCommissionRate,
            __time64_t ExpirationDate, 
			int nServerGroup ) = 0;


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
			int nServerGroup ) = 0;

		virtual int DeleteItem( UUID& sItemGUID ) = 0;
		virtual int DeleteItem( DWORD dwChaNum, int nServerGroup ) = 0;

		virtual int GetSaleItems( 
			WORD wPage, 
			int nChannel, 
			WORD wSchool, 
			sSaleItemSearchandSortBasic& sSearch, 
			std::vector< sSaleItem >& vecSaleItems, 
			WORD& wTotalCount, 
			int nServerGroup ) = 0;

		virtual int GetSaleItems( 
			WORD wPage, 
			int nChannel, 
			WORD wSchool, 
			std::string& strItemName, 
			int nSortOption, 
			int nSortMethod, 
			std::vector< sSaleItem >& vecSaleItems, 
			WORD& wTotalCount, 
			int nServerGroup ) = 0;

		// param input
		// sItemGUID 구매하려는 아이템 uuid
		// dwItemTurnNum 구매하려는 아이템 개수
		// dwFieldSlot 구매자 필드서버 슬롯
		// dwChaDbNum 구매자 캐릭터번호
		// param output
		// llPriceTotal 구매자가 지불해야할 총 금액
		virtual int PrepareItemBuy( 
			UUID& sItemGUID, 
			DWORD dwItemTurnNum, 
			DWORD dwFieldSlot, 
			DWORD dwChaDbNum, 
			const char* szName, 
			LONGLONG& llPriceTotal OUT,
            int& nSellType OUT, 
			int nServerGroup ) = 0;

		virtual int DeleteHoldItem( UUID& sItemGUID ) = 0;

		virtual int GetHoldItem( UUID& sItemGUID, sHoldItem& sItem ) = 0;

		virtual int UpdateItem( UUID& sItemGUID, DWORD dwItemTurnNum ) = 0;

		virtual bool IsHoldItem( UUID& sItemGUID ) = 0;

		virtual int InsertHoldItem( UUID& sItemGUID, DWORD dwFieldSlot, DWORD dwChaDbNum, const char* szName, int nServerGroup ) = 0;

		virtual int InsertSaleLog( UUID& sItemGUID ) = 0;

		virtual void FrameMove() = 0;

		virtual int GetSaleLog( DWORD dwChaDbNum, std::vector< sSaleLog >& vecSaleLog, int nServerGroup ) = 0;

		virtual void UpdateBasicOption( 
			bool bChannelCheck, 
			bool bSchoolCheck, 
			bool bAllPage, 
			WORD wSearchItemNumberPerPage, 
			WORD wLogNumberPerPage, 
			WORD wLogNumberMaxPerCharacter, 
			int nSearchOption ) = 0;

		virtual void GetBasicOption( 
			bool& bChannelCheck, 
			bool& bSchoolCheck, 
			bool& bAllPage, 
			WORD& wSearchItemNumberPerPage, 
			WORD& wLogNumberPerPage, 
			WORD& wLogNumberMaxPerCharacter, 
			int& nSearchOption ) = 0;

		virtual void UpdateChannelOption( bool bChannelCheck ) = 0;
		virtual void UpdateSchoolOption( bool bSchoolCheck ) = 0;
		virtual void UpdatePageOption( bool bAllPage ) = 0;
		virtual void UpdateSearchOption( int nValue ) = 0;


		virtual int DeleteSeller( DWORD dwSellerFieldSlot ) = 0;

	};
}

