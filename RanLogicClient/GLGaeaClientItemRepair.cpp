#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"

#include "../enginelib/GUInterface/UITextControl.h"
#include "../enginelib/GUInterface/GameTextControl.h"

#include "../RanLogic/Msg/ItemRepairMsg.h"
#include "../RanLogic/Item/GLItem.h"

#include "./Char/GLCharacter.h"
#include "./Widget/GLWidgetScript.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::MsgItemRepairAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_ITEM_REPAIR_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;


    GLCharacter* pMyChar = GetCharacter();
    if( !pMyChar )
        return;

	if ( pMyChar->CharDbNum() != RecvData.m_dwChaDbNum )
		return;
    
    if( RecvData.m_vecSlot.size() == 0 )
    {
        // 수리 실패 수리비 부족
        PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_REPAIR_SYS_2") );
        return;

    }
    else if(  RecvData.m_vecSlot.size() == 1 )
    {
        // 1개의 아이템을 수리
        pMyChar->ItemRepairing( (EMSLOT)RecvData.m_vecSlot[0] );
        SITEM* pItem = pMyChar->GET_SLOT_ITEMDATA( (EMSLOT)RecvData.m_vecSlot[0] );

        if( pItem )
            PrintMsgText( NS_UITEXTCOLOR::GREENYELLOW, sc::string::format( ID2GAMEINTEXT("ITEM_REPAIR_SYS_0"), pItem->GetName() ).c_str() );
    }
    else
    {
        // 전체 아이템 수리
        BOOST_FOREACH( const BYTE& bySlot, RecvData.m_vecSlot )
            pMyChar->ItemRepairing( (EMSLOT)bySlot );

        PrintMsgText( NS_UITEXTCOLOR::GREENYELLOW, ID2GAMEINTEXT("ITEM_REPAIR_SYS_1") );
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFUnc_ItemRepair_Update );
}

void GLGaeaClient::MsgItemRepairCardUse( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_ITEM_REPAIR_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    GLCharacter* pMyChar = GetCharacter();
    if( !pMyChar )
        return;

	if (pMyChar->CharDbNum() != RecvData.m_dwChaDbNum)
		return;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ItemRepair_Open );
}
