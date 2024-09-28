#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../GLGaeaClient.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

LuaTable GLCharacter::GetStorageTimeForWidget( DWORD dwChannel )
{
	LuaTable tbTime( GLWidgetScript::GetInstance().GetLuaState() );
	
	bool bUsable = IsKEEP_STORAGE( dwChannel );
	if( true == bUsable )
	{
		CTime time = GetStorageTime( dwChannel );
		m_pGaeaClient->GetConvertServerTime( time );

		tbTime.set( "nYear", time.GetYear() );
		tbTime.set( "nMonth", time.GetMonth() );
		tbTime.set( "nDay", time.GetDay() );
		tbTime.set( "nHour", time.GetHour() );
		tbTime.set( "nMinute", time.GetMinute() );
	}

	tbTime.set( "bIsUsable", bUsable );

	return tbTime;
}

void GLCharacter::MsgReqGetStorageFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_GETSTORAGE_FB *pNetMsg = (GLMSG::SNETPC_REQ_GETSTORAGE_FB *)pMsg;

    //	Note : 창고 정보 새로 받기 시도.
    //
    const DWORD dwChannel = pNetMsg->dwChannel;

    SetStorageMoney( pNetMsg->lnMoney );

    m_bStorage[dwChannel] = false;
    m_dwNumStorageItem[dwChannel] = pNetMsg->dwNumStorageItem;
    m_cStorage[dwChannel].DeleteItemAll ();

    if ( m_cStorage[dwChannel].GetNumItems() == m_dwNumStorageItem[dwChannel] )
    {
        m_bStorage[dwChannel] = true;
        m_dwNumStorageItem[dwChannel] = UINT_MAX;
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Locker_UpdateMoney,
		"-l", pNetMsg->lnMoney );
}

void GLCharacter::MsgStorageDelInsert(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_STORAGE_DEL_AND_INSERT *pNetMsg = (GLMSG::SNETPC_STORAGE_DEL_AND_INSERT *)pMsg;

    DWORD dwChannel = pNetMsg->dwChannel;
    /*
    // PET
    // 팻카드 정보를 제거해준다.
    SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem ( pNetMsg->wDelX, pNetMsg->wDelX );
    if ( pInvenItem )
    {
    PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
    if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
    }
    */
    m_cStorage[dwChannel].DeleteItem ( pNetMsg->wDelX, pNetMsg->wDelY );
    m_cStorage[dwChannel].InsertItem ( pNetMsg->sInsert.sItemCustom, pNetMsg->sInsert.wPosX, pNetMsg->sInsert.wPosY );

	// 락커 UI 업데이트
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Locker_UpdateItem,
		"-n", dwChannel );

    // PET
    // 팻카드 정보를 요청한다.
    SITEM* pItem = GLogicData::GetInstance().GetItem ( pNetMsg->sInsert.sItemCustom.GetNativeID() );
    if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pNetMsg->sInsert.sItemCustom.PetDbNum() != 0  )
    {
        GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(pNetMsg->sInsert.sItemCustom.PetDbNum());
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
}

void GLCharacter::MsgStorageItemUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_STORAGE_ITEM_UPDATE *pNetMsg = (GLMSG::SNETPC_STORAGE_ITEM_UPDATE *)pMsg;
    const DWORD dwChannel = pNetMsg->dwChannel;

    SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( !pInvenItem )
        return;
    if ( pInvenItem->sItemCustom.GetNativeID() != pNetMsg->sItemCustom.GetNativeID() )
        return;

    pInvenItem->sItemCustom = pNetMsg->sItemCustom;
}

void GLCharacter::MsgStorageDelete(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_STORAGE_DELETE *pNetMsg = (GLMSG::SNETPC_STORAGE_DELETE *)pMsg;

    const DWORD dwChannel = pNetMsg->dwChannel;
    /*
    // PET
    // 팻카드 정보를 제거해준다.
    SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( pInvenItem )
    {
    PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
    if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
    }
    */
    m_cStorage[dwChannel].DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );

	// 락커 UI 업데이트
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Locker_UpdateItem,
		"-n", dwChannel );
}

void GLCharacter::MsgStorageInsert(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_STORAGE_INSERT *pNetMsg = (GLMSG::SNETPC_STORAGE_INSERT *)pMsg;

    const SINVENITEM &sInvenItem = pNetMsg->Data;
    const DWORD dwChannel = pNetMsg->dwChannel;

    m_cStorage[dwChannel].InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY );

	// 락커 UI 업데이트
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Locker_UpdateItem,
		"-n", dwChannel );

    // PET
    // 팻카드 정보를 요청한다.
    SITEM* pItem = GLogicData::GetInstance().GetItem ( sInvenItem.sItemCustom.GetNativeID() );
    if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && sInvenItem.sItemCustom.PetDbNum() != 0 )
    {
        GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(sInvenItem.sItemCustom.PetDbNum());
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
}

void GLCharacter::MsgReqGetStorageItem(NET_MSG_GENERIC* pMsg)
{
    // NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM
    // GLMSG::SNETPC_REQ_GETSTORAGE_ITEM *pNetMsg = (GLMSG::SNETPC_REQ_GETSTORAGE_ITEM *)pMsg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::SNETPC_REQ_GETSTORAGE_ITEM RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    const SINVENITEM &sInvenItem = RecvData.Data;
    const DWORD dwChannel = RecvData.dwChannel;

    m_cStorage[dwChannel].InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY );

    if ( m_cStorage[dwChannel].GetNumItems() == m_dwNumStorageItem[dwChannel] )
    {
        m_bStorage[dwChannel] = true;
        m_dwNumStorageItem[dwChannel] = UINT_MAX;
    }

	// 락커 UI 업데이트
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Locker_UpdateItem,
		"-n", dwChannel );

    // PET
    // 팻카드 정보를 요청한다.
    SITEM* pItem = GLogicData::GetInstance().GetItem ( sInvenItem.sItemCustom.GetNativeID() );
    if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && sInvenItem.sItemCustom.PetDbNum() != 0 )
    {
        GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(sInvenItem.sItemCustom.PetDbNum());
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
}

void GLCharacter::MsgStorageDrugUpdate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_STORAGE_DRUG_UPDATE *pNetMsg = (GLMSG::SNETPC_STORAGE_DRUG_UPDATE *)pMsg;

    DWORD dwChannel = pNetMsg->dwChannel;

    SINVENITEM* pInvenItem = m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( pInvenItem )
    {
        pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
    }

	// 락커 UI 업데이트
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Locker_UpdateItem,
		"-n", dwChannel );
}

void GLCharacter::MsgStorageUpdateMoney(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY *pNetMsg = (GLMSG::SNETPC_REQ_STORAGE_UPDATE_MONEY *)pMsg;
    SetStorageMoney( pNetMsg->lnMoney );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Locker_UpdateMoney,
		"-l", pNetMsg->lnMoney );
}

void GLCharacter::MsgStorageState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_STORAGE_STATE *pNetMsg = (GLMSG::SNETPC_STORAGE_STATE *) pMsg;

    std::string strText;

    for ( int i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i  )
    {
        if ( m_bSTORAGE[i]==true && pNetMsg->bVALID[i]==false )
        {
            strText = sc::string::format( ID2GAMEINTEXT("EMREQ_STORAGE_END"), i+1 ).c_str();
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
        }

        m_bSTORAGE[i] = pNetMsg->bVALID[i];
    }
}


void GLCharacter::MsgStorageSpecificItemCount(NET_MSG_GENERIC* pMsg)
{
	m_StorageSpecificItemCacheVec.clear();

	GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB *pNetMsg = (GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB*)pMsg;	
	m_StorageSpecificItemCacheCount = pNetMsg->dwItemCount;

	if( m_StorageSpecificItemCacheCount == 0 )
	{
		//!! SF_TODO
		//m_pInterface->SetStorageItemToVehicleTapWindow(m_StorageSpecificItemCacheVec);
	}
}

void GLCharacter::MsgStorageSpecificItem(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_FB *pNetMsg = (GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_FB*)pMsg;	
	m_StorageSpecificItemCacheVec.push_back(pNetMsg->Data);

	if( m_StorageSpecificItemCacheCount == (DWORD)m_StorageSpecificItemCacheVec.size() )
	{
		//!! SF_TODO
		//m_pInterface->SetStorageItemToVehicleTapWindow(m_StorageSpecificItemCacheVec);

		m_StorageSpecificItemCacheCount = 0;
		m_StorageSpecificItemCacheVec.clear();
	}
}


