#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"

#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Msg/GLContrlPrivateMarket.h"
#include "../../RanLogic/Msg/ConsignmentSaleMsg.h"

#include "../../RanLogicServer/PrivateMarket/GLPrivateMarketSearchMan.h"

#include "./CacheServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



void CacheServer::MsgConsignmentSaleRegisterReq( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )pMsg;
    GLMSG::NET_CONSIGNMENT_ITEM_REGIST_REQ RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return;

    if ( m_pPrivateMarketSearch )
    {
        // 아이템 등록
        m_pPrivateMarketSearch->InsertConsigmentSaleItem( 
            RecvData.SaleItem.strChaName,
            RecvData.SaleItem.dwChaDbNum, 
            RecvData.SaleItem.GetGUID(), 
            RecvData.SaleItem.sItemcustom, 
            RecvData.SaleItem.wTurnNum, 
            RecvData.SaleItem.llPrice,
            RecvData.fCommissionRate, 
            static_cast<private_market::eSellType>( RecvData.SaleItem.SellType ),
            RecvData.SaleItem.ExpirationDate, 
			RecvData.SaleItem.ServerGroup );
    }
}

void CacheServer::MsgConsignmentSaleSendbackReq( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )pMsg;
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return;

    sc::MGUID Guid;
    char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
    StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, RecvData.strGUID.c_str() );
    sc::string::stringToUUID( szGuid, Guid );

	bool bDelete = false;
	if ( m_pPrivateMarketSearch == NULL )
		return;

	int nCacheResult = m_pPrivateMarketSearch->InsertHoldItem( Guid, dwClient, RecvData.dwChaDbNum, "SendBack", RecvData.nServerGroup );
	
	// 홀드 중이 아니라면 삭제한다.
	if ( nCacheResult==private_market::eSuccess )
		nCacheResult = m_pPrivateMarketSearch->DeleteItem( Guid );

	m_pPrivateMarketSearch->DeleteHoldItem( Guid );

    // 필드서버에게 응답해준다.
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK SendData;
    {
        SendData.strGUID = RecvData.strGUID;
        SendData.dwChaDbNum = RecvData.dwChaDbNum;
		SendData.nResult = nCacheResult;
    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    SendClient( dwClient, NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK, PackBuffer );
}

void CacheServer::MsgConsignmentSalePurchaseFail( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )pMsg;
	GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK RecvData;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
		return;

	sc::MGUID Guid;
	char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
	StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, RecvData.strGUID.c_str() );
	sc::string::stringToUUID( szGuid, Guid );

	m_pPrivateMarketSearch->DeleteHoldItem( Guid );
}

void CacheServer::MsgConsignmentSalePayMoneyAck( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	/*
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )pMsg;
	GLMSG::NET_CONSIGNMENT_SALE_PAY_MONEY_ACK RecvData;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
		return;

	// 이제 예전에 구매자 필드서버에서 하던 위탁판매 아이템 처리한다.
	sc::MGUID Guid;
	char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
	StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, RecvData.strGUID.c_str() );
	sc::string::stringToUUID( szGuid, Guid );

	AddGameAdoJob( 
		db::DbActionPtr( new db::ConsignmentSalePurchaseForCacheServer( Guid, RecvData.nPurchaseCount, RecvData ) ) );
	*/
}

void CacheServer::MsgConsignmentSaleUpdateItemAck( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
	/*
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )pMsg;
	GLMSG::NET_CONSIGNMENT_SALE_UPDATE_ITEM_ACK RecvData;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
		return;
	*/

	/*
	sc::MGUID Guid;
	char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
	StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, RecvData.strGUID.c_str() );
	sc::string::stringToUUID( szGuid, Guid );

	private_market::sHoldItem sItem;
	int nResult = m_pPrivateMarketSearch->GetHoldItem( Guid, sItem );
	if ( private_market::eSuccess == nResult )
	{
		// 아이템 처리 결과
		if ( 0 == RecvData.Result )
		{
			// 성공, 이제 구매자 필드로 가서 우편 보내고, 로그 남기고 캐시서버에 기존 방식대로 결과 처리
			GLMSG::NET_CONSIGNMENT_SALE_SEND_POST_REQ SendData;
			{
				SendData.strBuyerName = RecvData.strBuyerName;
				SendData.dwBuyerDbNum = RecvData.dwBuyerDbNum;
				SendData.strSellerName = RecvData.strSellerName;
				SendData.dwSellerDbNum = RecvData.dwSellerDbNum;
				SendData.strGUID = RecvData.strGUID;
				SendData.nPurchaseCount = RecvData.nPurchaseCount;
				SendData.llItemPriceTotal = RecvData.llItemPriceTotal;
				SendData.nSellType = RecvData.nSellType;
				SendData.Attach = RecvData.Attach;
			}
			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );

			SendClient( sItem.dwBuyerFieldSlot, NET_MSG_CONSIGNMENT_SALE_SEND_POST_AND_LOG_REQ, PackBuffer);
		}
		else if ( -1 == RecvData.Result )
		{
			// 실패, 구매자 필드로 가서 구매 대금 돌려준다. 캐릭터 있다면 바로 돌려주고, 없다면 우편으로
		}
		else if ( -2 == RecvData.Result )
		{
			// 실패, 구매자 필드로 가서 구매 대금 돌려준다. 캐릭터 있다면 바로 돌려주고, 없다면 우편으로
			// 아이템도 돌려줘야 하는 케이스지만, 현재는 이런 경우는 없다.
		}
	}
	else
	{
		sc::writeLogError( sc::string::format( 
			"MsgConsignmentSaleUpdateItemAck GetHoldItem error. %1% %2% %3% %4% %5% %6%", 
			RecvData.dwBuyerDbNum, RecvData.llItemPriceTotal, RecvData.nSellType, RecvData.dwSellerDbNum, RecvData.strGUID.c_str(), RecvData.nPurchaseCount ) );
	}
	*/

	/*
	// 아이템 처리 결과
	if ( 0 == RecvData.Result )
	{
		// 성공, 이제 구매자 필드로 가서 우편 보내고, 로그 남기고 캐시서버에 기존 방식대로 결과 처리
		GLMSG::NET_CONSIGNMENT_SALE_SEND_POST_REQ SendData;
		{
			SendData.strBuyerName = RecvData.strBuyerName;
			SendData.dwBuyerDbNum = RecvData.dwBuyerDbNum;
			SendData.strSellerName = RecvData.strSellerName;
			SendData.dwSellerDbNum = RecvData.dwSellerDbNum;
			SendData.strGUID = RecvData.strGUID;
			SendData.nPurchaseCount = RecvData.nPurchaseCount;
			SendData.llItemPriceTotal = RecvData.llItemPriceTotal;
			SendData.nSellType = RecvData.nSellType;
			SendData.Attach = RecvData.Attach;
		}
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		SendClient( RecvData.dwBuyerFieldSlot, NET_MSG_CONSIGNMENT_SALE_SEND_POST_AND_LOG_REQ, PackBuffer);
	}
	else if ( -1 == RecvData.Result )
	{
		// 실패, 구매자 필드로 가서 구매 대금 돌려준다. 캐릭터 있다면 바로 돌려주고, 없다면 우편으로
	}
	else if ( -2 == RecvData.Result )
	{
		// 실패, 구매자 필드로 가서 구매 대금 돌려준다. 캐릭터 있다면 바로 돌려주고, 없다면 우편으로
		// 아이템도 돌려줘야 하는 케이스지만, 현재는 이런 경우는 없다.
	}
	*/
}
