#include "pch.h"

#include "./GLGaeaServer.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/ItemRepairMsg.h"
#include "../../RanLogic/Item/ItemDurability.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLGaeaServer::MsgItemRepairReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_ITEM_REPAIR_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if( !pChar )
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on MsgItemRepairReq ", dwGaeaID));
        return FALSE;
	}

    if( pChar->CharDbNum() != RecvData.m_dwChaDbNum )
        return FALSE;

    if( !pChar->IsValidBody() )
        return FALSE;

    //! �������� ������
    GLMSG::NET_ITEM_REPAIR_ACK SendData;
    {
        SendData.m_dwChaDbNum = pChar->CharDbNum();

        LONGLONG llRepairPrice = 0;

		LONGLONG llTempPrice = 0;

        BOOST_FOREACH( const BYTE& bySlot, RecvData.m_vecSlot )
        {
            const EMSLOT emSlot = (EMSLOT)bySlot;

            const SITEMCUSTOM& sItemCustom = pChar->GET_SLOT_ITEM( emSlot );
            const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );

            if (!pItem)
                continue;

			llTempPrice = ItemDurability::GetInstance().GetRepairPriceAllPoint(sItemCustom);

			if( llTempPrice > 0 )
				pChar->LogItemExchange( sItemCustom, gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, MONEY_USE_ITEM_REPAIR, 1, pChar->GetCurrentMap(), LOG_MONEY_ITEM_REPAIR, true );
            // ������ �ջ�
            llRepairPrice += llTempPrice;

            SendData.m_vecSlot.push_back( emSlot );
        }

        // ������ ����
        //if( pChar->CheckMoneyUpdate( pChar->m_lnMoney, llRepairPrice, FALSE, "ITEM_REPAIR" ) )
        if( llRepairPrice <= pChar->GetInvenMoney() )
        {
			pChar->SubtractInvenMoneyServer( llRepairPrice, SUBTRACT_INVEN_MONEY_ITEM_REPAIR );

			// ��� �� �α� ����
			pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, -llRepairPrice, MONEY_USE_ITEM_REPAIR, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_ITEM_REPAIR );
			// ��� �� �α� ����
            pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, pChar->GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

            pChar->SendInvenMoneyUpdate();

            // ������ ����
            BOOST_FOREACH( const BYTE& bySlot, SendData.m_vecSlot )
                pChar->ItemRepairing( (EMSLOT)bySlot );

        }
        else
        {
            SendData.m_vecSlot.clear();
        }

    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_ITEM_REPAIR_ACK, PackBuffer);

    return TRUE;
}

BOOL GLGaeaServer::MsgItemRepairCardUseReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_ITEM_REPAIR_CARD_USE_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if( !pChar )
        return FALSE;

    if( pChar->CharDbNum() != RecvData.m_dwChaDbNum )
        return FALSE;

    if( !pChar->IsValidBody() )
        return FALSE;


    SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( RecvData.m_nInvecPosX, RecvData.m_nInvecPosY );
    if( !pInvenItem )
        return FALSE;

    if( pChar->CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
        return FALSE;

    const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
    if( !pItem || pItem->sBasicOp.emItemType != ITEM_REPAIR_CARD )
        return FALSE;

	// Note : ���� ���� Ȯ��
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return FALSE;
	}

    //! �������� ������
    GLMSG::NET_ITEM_REPAIR_CARD_USE_ACK SendData;
    {
        SendData.m_dwChaDbNum = RecvData.m_dwChaDbNum;
    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_ITEM_REPAIR_CARD_ACK, PackBuffer);

    return TRUE;

}
