#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Msg/PointShopMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Character/GLCharCache.h"
#include "../Database/DBAction/DbActionLog.h"
#include "../Database/DBAction/DbActionShop.h"
#include "../Database/DbManager.h"
#include "../PointShop/PointShopCache.h"
#include "./CacheServerConfigXml.h"
#include "./CacheServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CacheServer::PointShopDirectLoad()
{
	CTime CurrentTime( CTime::GetCurrentTime() );
	__time64_t ShopUpdateTime = CurrentTime.GetTime();
	std::vector< pointshop::SellItem > SellList;
	bool bReload = false;
	m_pDbManager->PointShopList( ShopUpdateTime, SellList, m_pConfig->IsStandAlonePointShop() );

	m_pPointShop->Clear();
	m_pPointShop->SetUpdateDate( ShopUpdateTime );

	for ( size_t i = 0; i < SellList.size(); ++i )
	{
		const pointshop::SellItem& SellItem = SellList[i];
		const SITEM* pItem = GLogicData::GetInstance().GetItem( SellItem.ItemId() );
		if ( pItem )
		{
			m_pPointShop->Insert( SellItem );
		}
		else
		{
			sc::writeLogError( sc::string::format( 
				"MsgPsDataDS no item data. check item data (%1%/%2%)", SellItem.ItemId().wMainID, SellItem.ItemId().wSubID ) );
		}
	}

	if ( bReload )
	{
		PointShopDataSend( 0, true );
	}
}

//! Session->Cache Server:Point Shop reload
void CacheServer::MsgPsReloadSS(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    //pointshop::NET_RELOAD_SS* pPacket = (pointshop::NET_RELOAD_SS*) pMsg;
    // PointShop reload
    m_pDbManager->AddShopJob(
        db::DbActionPtr(
            new db::GetPointShop(true,m_pConfig->IsStandAlonePointShop())));
}

void CacheServer::MsgPsDataStartDS(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    if (dwClient != 0)
        return;
    pointshop::NET_DATA_START_DS* pPacket = (pointshop::NET_DATA_START_DS*) pMsg;

    m_pPointShop->Clear();
	m_pPointShop->SetUpdateDate(pPacket->ShopUpdateDate());

// #ifndef _RELEASE
//     sc::writeLogInfo(
//         sc::string::format(
//             "MsgPsDataStartDS %1%", pPacket->ShopUpdateDate()));
// #endif
}

void CacheServer::MsgPsDataEndDS(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    if (dwClient != 0)
        return;
    pointshop::NET_DATA_END_DS* pPacket = (pointshop::NET_DATA_END_DS*) pMsg;
    if (pPacket->IsReload())
    {
        // 리로드 되었다. 각 서버들로 전송.
        PointShopDataSend(0, true);
    }

// #ifndef _RELEASE
//     sc::writeLogInfo(std::string("MsgPsDataEndDS"));        
// #endif
}

void CacheServer::MsgPsDataDS(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    if (dwClient != 0)
        return;
    pointshop::NET_DATA_DS* pPacket = (pointshop::NET_DATA_DS*) pMsg;
    const pointshop::SellItem& SellItem = pPacket->Data();

    const SITEM* pItem = GLogicData::GetInstance().GetItem(SellItem.ItemId());
    if (pItem)
    {
        m_pPointShop->Insert(SellItem);
    }
    else
    {
        sc::writeLogError(
            sc::string::format(
                "MsgPsDataDS no item data. check item data (%1%/%2%)",
                SellItem.ItemId().wMainID,
                SellItem.ItemId().wSubID));
    }

// #ifndef _RELEASE
//     sc::writeLogInfo(
//         sc::string::format(
//             "MsgPsDataDS %1% %2% %3%",
//             SellItem.ItemId().wMainID,
//             SellItem.ItemId().wSubID,
//             SellItem.Price()));
// #endif
}

void CacheServer::MsgPsReqDataSH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    pointshop::NET_REQ_DATA_SH* pPacket = (pointshop::NET_REQ_DATA_SH*) pMsg;
    PointShopDataSend(dwClient, false);    
}

void CacheServer::PointShopDataSend(DWORD dwClient, bool bReload)
{
    pointshop::NET_DATA_START_HS MsgStart(m_pPointShop->UpdateDate());
    if (bReload)
        SendAllClientWithoutSession(&MsgStart);
    else
        SendClient(dwClient, &MsgStart);

    std::vector<pointshop::SellItem> ItemList;
    m_pPointShop->SellList(ItemList);
    for (size_t i=0; i<ItemList.size(); ++i)
    {
        pointshop::NET_DATA_HS MsgData(ItemList[i]);
        if (bReload)
            SendAllClientWithoutSession(&MsgData);
        else
            SendClient(dwClient, &MsgData);
    }
    
    if (bReload)
    {
        pointshop::NET_DATA_END_HS MsgEnd(true);
        SendAllClientWithoutSession(&MsgEnd);
    }
    else
    {
        pointshop::NET_DATA_END_HS MsgEnd(false);
        SendClient(dwClient, &MsgEnd);
    }

// #ifndef _RELEASE
//     sc::writeLogInfo(
//         sc::string::format(
//             "MsgPsReqDataSH %1%",
//             dwClient));
// #endif
}

//! Field->Cache:Character Point 변경됨
void CacheServer::MsgCharPsPointChangeFH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_CHAR_PS_POINT_CHANGE_FH* pPacket = (NET_CHAR_PS_POINT_CHANGE_FH*) pMsg;
    const CHARACTER_POINT& OldPoint = pPacket->OldPoint();
    const CHARACTER_POINT& NewPoint = pPacket->NewPoint();
    DWORD ChaDbNum = pPacket->ChaDbNum();
    
    // Character point 세팅
    spCharCache spChar = GetChar(ChaDbNum);
    if (spChar)
	{
        spChar->PointSet(NewPoint);
	}
	else
	{
		std::string ErrMsg(
		   sc::string::format(
			    "MsgCharPsPointChangeFH Character not exist(%1%) Old Point(%2%/%3%) New Point(%4%/%5%)",
				ChaDbNum,
				OldPoint.Refundable(),
				OldPoint.NotRefundable(),
				NewPoint.Refundable(),
				NewPoint.NotRefundable()));
		sc::writeLogError(ErrMsg);
	}

	SNATIVEID ItemID = NATIVEID_NULL();

    if ( pPacket->ChangeType() == pointshop::BUY_ITEM )
    {
        ItemID = pPacket->ItemId();
        __int64 Price = pPacket->Price();

        // Shop DB 에 아이템 구매 로그를 남긴다
        SaleLog(
            ItemID,
            Price,
            db::SELL_POINT_SHOP,
            pPacket->UserDbNum(),
            pPacket->ServerNum(),
            pPacket->ChaDbNum(),
            pPacket->ChaLevel());

        // 통계 업데이트
        m_pPointShop->AddStatistics(ItemID, Price);
    }

	// DB 에 Log 를 남긴다
    m_pDbManager->AddLogJob(
        db::DbActionPtr(
            new db::LogPointShopInsertRenew(ChaDbNum,  pPacket->ChangeType(), OldPoint, NewPoint, ItemID )));

    m_pDbManager->AddLogJob(
        db::DbActionPtr(
		new db::LogPointShopExchange(gamelog::ID_CHAR, ChaDbNum,  ITEM_ROUTE_SYSTEM, ItemID.wMainID, ItemID.wSubID, pPacket->Price() , LOG_RANPOINT_MINUS_BY_POINTSHOP) ));
}

void CacheServer::MsgCharChangePointFH( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_CHAR_POINT_CHANGE_CACHE_FH* pPacket =
		static_cast< NET_CHAR_POINT_CHANGE_CACHE_FH* >( pMsg );

	const DWORD dwChaDbNum = pPacket->GetChaDbNum();
	const CHARACTER_POINT& sUpdatePoint = pPacket->GetUpdatePoint();

	// Character point 세팅;
	spCharCache spChar = GetChar( dwChaDbNum );
	if ( spChar )
	{
		spChar->PointSet( sUpdatePoint );
	}
	else
	{
		std::string strErrMsg(
			sc::string::format(
			"MsgCharChangePointFH Character not exist(%1%) UpdatePoint(%4%/%5%)",
			dwChaDbNum,
			sUpdatePoint.Refundable(),
			sUpdatePoint.NotRefundable()));
		sc::writeLogError( strErrMsg );
	}
}

void CacheServer::MsgCharWebShopLogFH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_CHAR_WEB_SHOP_LOG* pPacket = (NET_CHAR_WEB_SHOP_LOG*) pMsg;
    SaleLog(
        pPacket->m_ItemId,
        pPacket->m_Price,
        db::SELL_WEB_SHOP,
        pPacket->m_UserDbNum,
        pPacket->m_ServerGroup,
        pPacket->m_ChaDbNum,
        pPacket->m_ChaLevel);
}

void CacheServer::SaleLog(
    const SNATIVEID& ItemId,
    __int64 Price,
    db::EM_BILLING_SELL_TYPE BillingType,
    DWORD UserDbNum,
    int ServerNum,
    DWORD ChaDbNum,
    int ChaLevel)
{
    if (m_pDbManager)
    {
        m_pDbManager->AddShopJob(
            db::DbActionPtr(
                new db::SaleLogInsert(
                    ItemId,
                    Price,
                    BillingType,
                    UserDbNum,
                    ServerNum,
                    ChaDbNum,
                    ChaLevel)));
    }
}