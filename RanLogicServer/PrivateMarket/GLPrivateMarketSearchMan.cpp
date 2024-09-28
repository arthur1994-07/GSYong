#include "pch.h"

#include "./GLPrivateMarketSearchSQLite.h"
#include "./GLPrivateMarketSearchMan.h"

#include "../../SigmaCore/DebugInclude.h"

namespace private_market
{
	GLPrivateMarketSearchMan::GLPrivateMarketSearchMan()
		: m_pImpl( NULL )
	{
		m_pImpl = new GLPrivateMarketSearchSQLite();
	}

	GLPrivateMarketSearchMan::~GLPrivateMarketSearchMan()
	{
		SAFE_DELETE( m_pImpl );
	}

	int GLPrivateMarketSearchMan::InsertSeller( 
		DWORD dwChaDbNum, 
		const char* szChaName, 
		const SNATIVEID& sMapID, 
		WORD wSchool, 
		int nChannel, 
		DWORD dwFieldSlot, 
		int nServerGroup )
	{
		return m_pImpl->InsertSeller( dwChaDbNum, szChaName, sMapID, wSchool, nChannel, dwFieldSlot, nServerGroup );
	}

	int GLPrivateMarketSearchMan::InsertSaleItem( 
		DWORD dwChaDbNum, 
		UUID& sItemGUID, 
		const SITEMCUSTOM& sItemcustom,
		const DWORD dwTurnNum, 
		const LONGLONG& llPrice,
		const float fCommissionRate, 
		int nServerGroup )
	{
		return m_pImpl->InsertSaleItem( dwChaDbNum, sItemGUID, sItemcustom, dwTurnNum, llPrice, fCommissionRate, 0, nServerGroup );
	}

    int GLPrivateMarketSearchMan::InsertConsigmentSaleItem( 
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
        return m_pImpl->InsertConsigmentSaleItem( strChaName, dwChaDbNum, sItemGUID, sItemcustom, dwTurnNum, llPrice, fCommissionRate, emSellType, ExpirationDate, nServerGroup );
    }

	int GLPrivateMarketSearchMan::DeleteItem( UUID& sItemGUID )
	{
		return m_pImpl->DeleteItem( sItemGUID );
	}

	int GLPrivateMarketSearchMan::DeleteItem( DWORD dwChaNum, int nServerGroup )
	{
		return m_pImpl->DeleteItem( dwChaNum, nServerGroup );
	}

	int GLPrivateMarketSearchMan::GetSaleItems( 
		WORD wPage, 
		int nChannel, 
		WORD wSchool, 
		sSaleItemSearchandSortBasic& sSearch, 
		std::vector< sSaleItem >& vecSaleItems, 
		WORD& wTotalCount, 
		int nServerGroup )
	{
		return m_pImpl->GetSaleItems( wPage, nChannel, wSchool, sSearch, vecSaleItems, wTotalCount, nServerGroup );
	}

	int GLPrivateMarketSearchMan::GetSaleItems( 
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
		return m_pImpl->GetSaleItems( wPage, nChannel, wSchool, strItemName, nSortOption, nSortMethod, vecSaleItems, wTotalCount, nServerGroup );
	}

	int GLPrivateMarketSearchMan::PrepareItemBuy( 
		UUID& sItemGUID, 
		DWORD dwItemTurnNum, 
		DWORD dwFieldSlot, 
		DWORD dwChaDbNum, 
		const char* szName, 
		LONGLONG& llPriceTotal OUT,
        int& nSellType OUT, 
		int nServerGroup )
	{
		return m_pImpl->PrepareItemBuy( sItemGUID, dwItemTurnNum, dwFieldSlot, dwChaDbNum, szName, llPriceTotal, nSellType, nServerGroup );
	}

	int GLPrivateMarketSearchMan::DeleteHoldItem( UUID& sItemGUID )
	{
		return m_pImpl->DeleteHoldItem( sItemGUID );
	}

	int GLPrivateMarketSearchMan::GetHoldItem( UUID& sItemGUID, sHoldItem& sItem )
	{
		return m_pImpl->GetHoldItem( sItemGUID, sItem );
	}

	int GLPrivateMarketSearchMan::UpdateItem( UUID& sItemGUID, DWORD dwItemTurnNum )
	{
		return m_pImpl->UpdateItem( sItemGUID, dwItemTurnNum );
	}

	bool GLPrivateMarketSearchMan::IsHoldItem( UUID& sItemGUID )
	{
		return m_pImpl->IsHoldItem( sItemGUID );
	}

	int GLPrivateMarketSearchMan::InsertHoldItem( UUID& sItemGUID, DWORD dwFieldSlot, DWORD dwChaDbNum, const char* szName, int nServerGroup )
	{
		return m_pImpl->InsertHoldItem( sItemGUID, dwFieldSlot, dwChaDbNum, szName, nServerGroup );
	}

	int GLPrivateMarketSearchMan::InsertSaleLog( UUID& sItemGUID )
	{
		return m_pImpl->InsertSaleLog( sItemGUID );
	}

	void GLPrivateMarketSearchMan::FrameMove()
	{
		m_pImpl->FrameMove();
	}

	int GLPrivateMarketSearchMan::GetSaleLog( DWORD dwChaDbNum, std::vector< sSaleLog >& vecSaleLog, int nServerGroup )
	{
		return m_pImpl->GetSaleLog( dwChaDbNum, vecSaleLog, nServerGroup );
	}

	void GLPrivateMarketSearchMan::UpdateBasicOption( 
		bool bChannelCheck, 
		bool bSchoolCheck, 
		bool bAllPage, 
		WORD wSearchItemNumberPerPage, 
		WORD wLogNumberPerPage, 
		WORD wLogNumberMaxPerCharacter, 
		int nSearchOption )
	{
		m_pImpl->UpdateBasicOption( bChannelCheck, bSchoolCheck, bAllPage, wSearchItemNumberPerPage, wLogNumberPerPage, wLogNumberMaxPerCharacter, nSearchOption );
	}

	void GLPrivateMarketSearchMan::GetBasicOption( 
		bool& bChannelCheck, 
		bool& bSchoolCheck, 
		bool& bAllPage, 
		WORD& wSearchItemNumberPerPage, 
		WORD& wLogNumberPerPage, 
		WORD& wLogNumberMaxPerCharacter, 
		int& nSearchOption )
	{
		m_pImpl->GetBasicOption( 
			bChannelCheck, 
			bSchoolCheck, 
			bAllPage, 
			wSearchItemNumberPerPage, 
			wLogNumberPerPage, 
			wLogNumberMaxPerCharacter, 
			nSearchOption );
	}

	void GLPrivateMarketSearchMan::UpdateChannelOption( bool bChannelCheck )
	{
		m_pImpl->UpdateChannelOption( bChannelCheck );
	}

	void GLPrivateMarketSearchMan::UpdateSchoolOption( bool bSchoolCheck )
	{
		m_pImpl->UpdateSchoolOption( bSchoolCheck );
	}

	void GLPrivateMarketSearchMan::UpdatePageOption( bool bAllPage )
	{
		m_pImpl->UpdatePageOption( bAllPage );
	}

	void GLPrivateMarketSearchMan::UpdateSearchOption( int nValue )
	{
		m_pImpl->UpdateSearchOption( nValue );
	}

	int GLPrivateMarketSearchMan::DeleteSeller( DWORD dwSellerFieldSlot )
	{
		return m_pImpl->DeleteSeller( dwSellerFieldSlot );
	}

}

