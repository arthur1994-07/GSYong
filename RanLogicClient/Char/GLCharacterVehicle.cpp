#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxEffect/DxPostEffect.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Stage/DxGlobalStage.h"
#include "../Effect/GLFactEffect.h"
#include "../Land/GLLandManClient.h"
#include "../GLGaeaClient.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

LuaTable GLCharacter::GetPutOnVehicleInfo()
{
	LuaTable tbVehicle( GLWidgetScript::GetInstance().GetLuaState() );

	const SITEMCUSTOM& sItem = GET_SLOT_ITEM( SLOT_VEHICLE );
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sItem.Id() );
	if( NULL == pItem )
		return tbVehicle;

	// 탈것 기본 정보
	VEHICLE_TYPE emType = pItem->VehicleType();
	if( VEHICLE_TYPE_NONE == emType )
	{
		tbVehicle.set( 1, (int) VEHICLE_TYPE_NONE );
		tbVehicle.set( 2, ID2GAMEWORD( "VEHICLE_TEXT_STATIC", 3 ) );
		tbVehicle.set( 3, ID2GAMEWORD( "VEHICLE_TEXT_STATIC", 3 ) );
	}
	else
	{
		tbVehicle.set( 1, (int) emType );
		tbVehicle.set( 2, COMMENT::VEHICLE_TYPE[ emType ] );
		tbVehicle.set( 3, pItem->GetName() );
	}

	// 이동 속도 증가율
	float fVolume = 0.0f;
	float fAddOnSpeed = 0.0f;

// 아래 else부분 말이 안되는거같은데...;
// 	EMITEM_VAR emItemVar = pItem->sSuitOp.sVOLUME.emTYPE;
// 	if( EMVAR_NONE != emItemVar )
// 	{
// 		if( EMVAR_MOVE_SPEED == emItemVar )
// 		{
// 			fVolume = sItem.GETMOVESPEED();
// 			fAddOnSpeed = VehiclePutOnItemMoveSpeedTotal();
// 		}
// 		else
// 		{
// 			fVolume = pItem->sSuitOp.sVOLUME.fVolume;
// 		}
// 	}
	INT nIdx = -1;
	if ( pItem->sSuitOp.IsHaveAddon(EMADDEX_INC_MOVE_SPEED, nIdx) )
	{
		fVolume = sItem.GETMOVESPEED();
		fAddOnSpeed = VehiclePutOnItemMoveSpeedTotal();
	}

	tbVehicle.set( 4, (int) EMADDEX_INC_MOVE_SPEED );
	tbVehicle.set( 5, fVolume );
	tbVehicle.set( 6, fAddOnSpeed );

	// 부스터 ID
	tbVehicle.set( 7, VehicleBoosterId() );

	// 탈것 장착 아이템
	LuaTable tbVehicleItem( GLWidgetScript::GetInstance().GetLuaState() );
	for( WORD i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; ++i )
	{
		const SITEMCUSTOM& sItemCustom = VehiclePutOnItemRef( VEHICLE_ACCESSORYTYPE( i ) );
		
		LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, sItemCustom.GetNativeID().Mid() );
		tbItem.set( 2, sItemCustom.GetNativeID().Sid() );
		tbItem.set( 3, sItemCustom.tBORNTIME );
		tbVehicleItem.set( i, tbItem );
	}

	tbVehicle.set( 8, tbVehicleItem );

	return tbVehicle;
}

float GLCharacter::GetVehicleBoosterGauge()
{
	float fGauge = 1.0f;

	if( TRUE == VehicleUseBoost() )
		fGauge = 1.0f - VehicleBoostTime() / 6.0f;

	if( TRUE == VehicleBoostUseDelay() )
		fGauge = 1.0f - VehicleBoostDelay() / 20.0f;

	return fGauge;
}

void GLCharacter::MsgVehicleBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_VEHICLE_BRD *pNetMsg = (GLMSG::SNETPC_VEHICLE_BRD *) pMsg;

    m_emVehicle = pNetMsg->emVehicle;
    m_sVehicle.m_DbNum = pNetMsg->sVehicle.m_DbNum;
    m_sVehicle.m_emTYPE = pNetMsg->sVehicle.m_emTYPE;
    m_sVehicle.m_sVehicleID = pNetMsg->sVehicle.m_sVehicleID;

    for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
    {
        m_sVehicle.m_PutOnItems[i].Assign( pNetMsg->sVehicle.m_PutOnItems[i] );
    }
    m_sVehicle.ITEM_UPDATE();

    UpdateSuit();
}

void GLCharacter::MsgWithVehicleMoveMap(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_MOVE_MAP_WITH_VEHICLE *pNetMsg = (GLMSG::SNETPC_MOVE_MAP_WITH_VEHICLE *) pMsg;

    if( pNetMsg->bMove )
    {
        m_pGaeaClient->GetCharacter()->ReqGateOut(pNetMsg->dwGateToIndex);
    }
}


void GLCharacter::MsgActiveWithVehicleFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ACTIVE_WITH_VEHICLE_FB *pNetMsg = (GLMSG::SNET_ACTIVE_WITH_VEHICLE_FB *) pMsg;

    if ( pNetMsg->bActive )
    {
        // AgentServer 갱신을 위해서 다시 메세지를 보내준다
        // Field는 이미 갱신된 상태임...
        if ( !IsActState(EM_ACT_PEACEMODE) )
            ReqTogglePeaceMode();

        for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
        {
            m_sVehicle.PassengerIdSet(i, pNetMsg->dwPassengerID[i]);
        }
        m_emVehicle = EMVEHICLE_PASSENGER;

        PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
        std::tr1::shared_ptr<GLCharClient> pChar;

        for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
        {
            if (m_sVehicle.PassengerId(i) == GAEAID_NULL || m_sVehicle.PassengerId(i) == m_dwGaeaID)
                continue;

            pChar = pLand->GetChar(m_sVehicle.PassengerId(i));
            if (!pChar)	
                continue;

            for (INT j=0; j<MAX_PASSENGER_COUNT; ++j)
            {
                pChar->VehiclePassengerIdSet(j, m_sVehicle.PassengerId(j));
            }
        }

        DxViewPort::GetInstance().SetVehicleCamera();

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_RideVehicle,
			"-b, -b", false, false );
    }
    else
    {
        m_emVehicle = EMVEHICLE_OFF;

        PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
        std::tr1::shared_ptr<GLCharClient> pChar;

        for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
        {
            pChar = pLand->GetChar(m_sVehicle.PassengerId(i));
            if (pChar)
            {
                for (INT j=0; j<MAX_PASSENGER_COUNT; ++j)
                {
                    if (pChar->VehiclePassengerId(j) == m_dwGaeaID)
                    {
                        pChar->VehiclePassengerIdSet(j, GAEAID_NULL);
                    }
                }
            }
        }

        for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
        {
            m_sVehicle.PassengerIdSet(i, GAEAID_NULL);
        }

        DxViewPort::GetInstance().SetGameCamera();

        DxPostEffect::GetInstance().RadialBlurActiveOff();

		GLWidgetScript::GetInstance().LuaCallFunc<void>(
			NSWIDGET_SCRIPT::g_strFunc_GetoffVehicle, "-b", false);
    }

    EMSLOT emRHand = GetCurRHand();
    EMSLOT emLHand = GetCurLHand();
    m_emANISUBTYPE = CHECK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );

    GLCHARLOGIC::INIT_DATA ( FALSE, FALSE );
    UpdateSuit( TRUE );
    ReSelectAnimation();

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
}


void GLCharacter::MsgWithVehicleFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_WITH_VEHICLE_FB *pNetMsg = (GLMSG::SNET_WITH_VEHICLE_FB *) pMsg;

    switch ( pNetMsg->emAns )
    {
    case EMWITH_VEHICLE_FB_OK:
        break;
    case EMWITH_VEHICLE_CANCEL:
        {
            PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
            std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar ( pNetMsg->dwTargetID );
            if ( !pChar )
                return;

			GLWidgetScript::GetInstance().LuaCallFunc<void>(
				NSWIDGET_SCRIPT::g_strFunc_VehicleReqTargetAnswer, "-b, -dw", false, pNetMsg->dwTargetID);

            std::string strText = sc::string::format( ID2GAMEINTEXT("EMBIKE_CANCEL"), pChar->GetName() ).c_str();
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
        }
        break;
    case EMWITH_VEHICLE_CANCEL_DIST:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_CANCEL_DIST") );
        }
        break;

    case EMWITH_VEHICLE_CANCEL_MY_DIE:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_CANCEL_MY_DIE") );
        }
        break;
    case EMWITH_VEHICLE_CANCEL_TAR_DIE:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_CANCEL_TAR_DIE") );
        }
        break;

    case EMWITH_VEHICLE_CANCEL_MY_DIE_ANS:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_CANCEL_MY_DIE_ANS") );
        }
        break;

    case EMWITH_VEHICLE_CANCEL_TAR_DIE_ANS:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_CANCEL_TAR_DIE_ANS") );
        }
        break;
    case EMWITH_VEHICLE_CANCEL_CONFT:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_CANCEL_CONFT") );
        }
        break;

    case EMWITH_VEHICLE_CANCEL_PMARKET:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_CANCEL_PMARKET") );
        }
        break;

    }
}

void GLCharacter::MsgVehicleReqItemInfoFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_REQ_ITEM_INFO_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_ITEM_INFO_FB* ) pMsg;

	// DB에 없으면 그냥 초기값으로 넘어오는 경우가 있으므로 체크해준다.
	if ( pNetMsg->emTYPE == VEHICLE_TYPE_NONE )
		return;

	switch( pNetMsg->eStatus )
	{
	case EMVEHICLE_BROAD_ITEM_INFO :
		{
			BYTE iRef = pNetMsg->iIndex;

			if( iRef >= ACCE_TYPE_SIZE ) return;

			if( !pNetMsg->bTrade )
			{				
				VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( pNetMsg->m_VehicleDbNum );

				if( iter != m_mapVEHICLEItemInfo.end() )
				{
					iter->second.m_PutOnItems[iRef] = pNetMsg->PutOnItems;
				}
				else
				{
					SVEHICLEITEMINFO sVehicleItemInfo;
					sVehicleItemInfo.m_PutOnItems[iRef] = pNetMsg->PutOnItems;

					m_mapVEHICLEItemInfo.insert ( std::make_pair(pNetMsg->m_VehicleDbNum, sVehicleItemInfo) );
				}				
			}
			else
			{
				VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfoTemp.find ( pNetMsg->m_VehicleDbNum );

				if( iter != m_mapVEHICLEItemInfoTemp.end() )
				{
					iter->second.m_PutOnItems[iRef] = pNetMsg->PutOnItems;
				}
				else
				{
					SVEHICLEITEMINFO sVehicleItemInfo;
					sVehicleItemInfo.m_PutOnItems[iRef] = pNetMsg->PutOnItems;

					m_mapVEHICLEItemInfoTemp.insert ( std::make_pair(pNetMsg->m_VehicleDbNum, sVehicleItemInfo) );
				}
			}

		}
		break;
	case EMVEHICLE_BROAD_OK        :
		{
			if( !pNetMsg->bTrade )
			{
				VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( pNetMsg->m_VehicleDbNum );

				if( iter != m_mapVEHICLEItemInfo.end() )
				{
					iter->second.m_emTYPE = pNetMsg->emTYPE;
					iter->second.m_nFull  = pNetMsg->nFull;
					iter->second.m_dwBoosterID = pNetMsg->dwBoosterID;
				}
				else
				{
					std::string ErrMsg = "Vehicle Receive1 failed";
					sc::writeLogError(ErrMsg);
					AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
				}
			}
			else
			{
				VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfoTemp.find ( pNetMsg->m_VehicleDbNum );

				if( iter != m_mapVEHICLEItemInfoTemp.end() )
				{
					iter->second.m_emTYPE = pNetMsg->emTYPE;
					iter->second.m_nFull  = pNetMsg->nFull;
					iter->second.m_dwBoosterID = pNetMsg->dwBoosterID;
				}
				else
				{
					std::string ErrMsg = " Vehicle Receive2 failed";
					sc::writeLogError(ErrMsg);
					AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONSTOP);
				}
			}
		}
		break;
	}

	/*

    // DB에 없으면 그냥 초기값으로 넘어오는 경우가 있으므로 체크해준다.
    if ( pNetMsg->emTYPE == VEHICLE_TYPE_NONE )
        return;

    SVEHICLEITEMINFO sVehicleItemInfo;
    sVehicleItemInfo.m_emTYPE = pNetMsg->emTYPE;
    sVehicleItemInfo.m_nFull  = pNetMsg->nFull;
    sVehicleItemInfo.m_dwBoosterID = pNetMsg->dwBoosterID;

    for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
    {
        sVehicleItemInfo.m_PutOnItems[i] = pNetMsg->PutOnItems[i];
    }

    if ( !pNetMsg->bTrade )
    {
        VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( pNetMsg->m_VehicleDbNum );
        if ( iter != m_mapVEHICLEItemInfo.end() )
            m_mapVEHICLEItemInfo.erase ( iter );

        m_mapVEHICLEItemInfo.insert ( std::make_pair(pNetMsg->m_VehicleDbNum, sVehicleItemInfo) );
    }
    else
    {
        m_mapVEHICLEItemInfoTemp.insert ( std::make_pair(pNetMsg->m_VehicleDbNum, sVehicleItemInfo) );
    }
	*/

	// 탈것 장착 업데이트
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Vehicle_Equip );
}

void GLCharacter::MsgVehicleReqGiveBatteryFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY_FB* ) pMsg;

    switch( pNetMsg->emFB )
    {
    case EMVEHICLE_REQ_GIVE_BATTERY_FB_OK:
        {
            SITEM* pItem = GLogicData::GetInstance().GetItem ( pNetMsg->sBatteryID );
            SITEM* pVehicleItem	= GLogicData::GetInstance().GetItem ( pNetMsg->sItemID );
            if ( pItem && pVehicleItem )
            {

                std::string strText;
                
                if (m_sVehicle.m_DbNum == pNetMsg->dwVehicleID)
                    m_sVehicle.IncreaseFull(pItem->sDrugOp.dwCureVolume, pItem->sDrugOp.bRatio);

                DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
                if (pGlobalStage->IsEmulator() )
                {							
                    m_pGaeaClient->PrintMsgText(
                        NS_UITEXTCOLOR::PALEGREEN,
                        sc::string::format(
                        ID2GAMEINTEXT("EMVEHICLE_REQ_GIVE_BATTERY_FB_OK"),
                        pVehicleItem->GetName(),
                        pItem->sDrugOp.dwCureVolume));
                    break;
                }


                VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( pNetMsg->dwVehicleID );
                if ( iter!=m_mapVEHICLEItemInfo.end() )
                {
                    SVEHICLEITEMINFO& sVehicle = (*iter).second;
                    // 팻카드의 정보 갱신
                    sVehicle.m_nFull = pNetMsg->nFull;

                    strText = sc::string::format( ID2GAMEINTEXT("EMVEHICLE_REQ_GIVE_BATTERY_FB_OK"), pVehicleItem->GetName(), pItem->sDrugOp.dwCureVolume ).c_str();
                    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
                    break;
                }

            }
        }
        break;
    case EMVEHICLE_REQ_GIVE_BATTERY_FB_FAIL:
        break;
    case EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDBATTERY:
        break;
    case EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDITEM:
        break;
    case EMVEHICLE_REQ_GET_BATTERY_FROMDB_OK:
        break;
    case EMVEHICLE_REQ_GET_BATTERY_FROMDB_FAIL:
        break;
    }
}


void GLCharacter::MsgVehicleUpdateClientBattery(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_UPDATE_CLIENT_BATTERY* pNetMsg = ( GLMSG::SNET_VEHICLE_UPDATE_CLIENT_BATTERY* ) pMsg;
    m_sVehicle.m_nFull = pNetMsg->nFull;

    if( m_sVehicle.IsNotEnoughFull() )
    {
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_BATTERY_NOT_ENOUGH") );
    }

    VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_DbNum );
    if ( iter!=m_mapVEHICLEItemInfo.end() )
    {
        SVEHICLEITEMINFO& sVehicle = (*iter).second;
        // 팻카드의 정보 갱신
        sVehicle.m_nFull = pNetMsg->nFull;
    }

	// 탈것 UI 연료 업데이트
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Vehicle_UpdateFuel,
		"-n", m_sVehicle.m_nFull );
}


void GLCharacter::MsgVehicleAccessoryDelete(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_ACCESSORY_DELETE* pNetMsg = (GLMSG::SNET_VEHICLE_ACCESSORY_DELETE*) pMsg;

    if ( m_sVehicle.IsActiveValue() && m_sVehicle.m_DbNum == pNetMsg->m_VehicleDbNum )
    {
        m_sVehicle.m_PutOnItems[pNetMsg->accetype] = SITEMCUSTOM();			
        m_sVehicle.ITEM_UPDATE();
        m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
        m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();
        UpdateSuit ( FALSE );
        INIT_DATA( FALSE, FALSE );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
    }

    VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( pNetMsg->m_VehicleDbNum  );
    if ( iter!=m_mapVEHICLEItemInfo.end() )
    {
        SVEHICLEITEMINFO& sVehicle = (*iter).second;
        // 팻카드의 정보 갱신
        sVehicle.m_PutOnItems[pNetMsg->accetype] = SITEMCUSTOM();	
    }

	// 탈것 UI 업데이트
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Vehicle_Update );
}


void GLCharacter::MsgVehicleRemoveSlotItemFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB* ) pMsg;

    WORD wPosX(0), wPosY(0);

    SITEMCUSTOM	sSlotItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );

    SITEM* pSlotItem = GLogicData::GetInstance().GetItem ( sSlotItemCustom.GetNativeID() );
    if ( !pSlotItem ) 
    {
        // 일반 오류
        return;
    }

    // 슬롯아이템 제거
    m_sVehicle.ReSetSlotItem( pNetMsg->emSuit );

    m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
    m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();


    // 탈것의 정보를 갱신해준다.
    VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find( m_sVehicle.m_DbNum );
    if ( iter!= m_mapVEHICLEItemInfo.end() )
    {
        SVEHICLEITEMINFO& sVehicle = (*iter).second;
        WORD i = (WORD)pNetMsg->emSuit - (WORD ) SUIT_VEHICLE_SKIN;
        if ( i < ACCE_TYPE_SIZE )
		{
			const SNATIVEID sID_NULL = NATIVEID_NULL();
            sVehicle.m_PutOnItems[i] = SITEMCUSTOM( sID_NULL );

			// 탈것 UI 업데이트
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Vehicle_Update );
		}
    }

    UpdateSuit( FALSE );
    INIT_DATA( FALSE, FALSE );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
}


void GLCharacter::MsgVehicleReqSlotToHoldFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB* ) pMsg;

    // TODO : Hold Item 없애기.

    //switch ( pNetMsg->emFB )
    //{
    //case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL:
    //    m_pGaeaClient->PrintMsgText ( 
    //        NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL") );
    //    break;

    //case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK:
    //    {
    //        // 장착된 아이템 BackUp
    //        SITEMCUSTOM sSlotItemCustom = m_sVehicle.GetSlotitembySuittype ( pNetMsg->emSuit );


    //        // 장착된 아이템 제거
    //        m_sVehicle.ReSetSlotItem ( pNetMsg->emSuit );
    //        m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
    //        m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();

    //        // 탈것의 정보를 갱신해준다.
    //        VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_DbNum );
    //        if ( iter!= m_mapVEHICLEItemInfo.end() )
    //        {
    //            SVEHICLEITEMINFO& sVehicle = (*iter).second;
    //            WORD i = (WORD)pNetMsg->emSuit - (WORD ) SUIT_VEHICLE_SKIN;
    //            if ( i < ACCE_TYPE_SIZE )
    //                sVehicle.m_PutOnItems[i] = SITEMCUSTOM ( NATIVEID_NULL() );
    //        }

    //        // 장착됐던 아이템을 손에 든다
    //        HOLD_ITEM ( sSlotItemCustom );
    //        UpdateSuit ( FALSE );
    //        INIT_DATA( FALSE, FALSE );
    //    }
    //    break;

    //case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH:
    //    m_pGaeaClient->PrintMsgText ( 
    //        NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
    //    break;

    //case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
    //    m_pGaeaClient->PrintMsgText ( 
    //        NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
    //    break;
    //}
}


void GLCharacter::MsgVehicleReqHoldToSlotFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB* ) pMsg;

    switch (pNetMsg->emFB)
    {
    case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL") );
        break;

    case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK:
        {
            // 손에든 아이템을 팻에게 장착시키고
            m_sVehicle.SetSlotItem( pNetMsg->emSuit, pNetMsg->sItemCustom );
            m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
            m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();					

            // 탈것의 정보를 갱신해준다.
            VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_DbNum );
            if ( iter!= m_mapVEHICLEItemInfo.end() )
            {
                SVEHICLEITEMINFO& sVehicle = (*iter).second;

				WORD wIndex = (WORD) pNetMsg->emSuit - (WORD) SUIT_VEHICLE_SKIN;
				if( ACCE_TYPE_SIZE > wIndex )
				{
					sVehicle.m_PutOnItems[ wIndex ] = pNetMsg->sItemCustom;

					// 탈것 UI 업데이트
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_Vehicle_Update );
				}
			}

            UpdateSuit( FALSE );
            INIT_DATA( FALSE, FALSE );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
        }
        break;

    case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH:
        m_pGaeaClient->PrintMsgText ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
        break;

    case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
        m_pGaeaClient->PrintMsgText ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
        break;

    default:
        m_pGaeaClient->PrintMsgText( 
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
                "MsgVehicleReqHoldToSlotFb Unknown type %1%",
                pNetMsg->emFB));
        break;
    };
}


void GLCharacter::MsgVehicleReqSlotExHoldFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB* ) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL") );

        break;

    case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK:
        {
            // 손에든 아이템을 팻에게 장착시키고
            m_sVehicle.SetSlotItem( pNetMsg->emSuit, pNetMsg->sHoldtoSlotItemCustom );
            m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
            m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();


            // 탈것의 정보를 갱신해준다.
            VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_DbNum );
            if ( iter!= m_mapVEHICLEItemInfo.end() )
            {
                SVEHICLEITEMINFO& sVehicle = (*iter).second;

				WORD wIndex = (WORD) pNetMsg->emSuit - (WORD) SUIT_VEHICLE_SKIN;
				if( ACCE_TYPE_SIZE > wIndex )
				{
					sVehicle.m_PutOnItems[ wIndex ] = pNetMsg->sHoldtoSlotItemCustom;

					// 탈것 UI 업데이트
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_Vehicle_Update );
				}
            }

            UpdateSuit ( FALSE );
            INIT_DATA( FALSE, FALSE );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
        }
        break;

    case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH:
        m_pGaeaClient->PrintMsgText ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH") );
        break;

    case EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM:
        m_pGaeaClient->PrintMsgText ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM") );
        break;
    };
}


void GLCharacter::MsgUngetVehicleFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UNGET_VEHICLE_FB* pNetMsg = (GLMSG::SNETPC_UNGET_VEHICLE_FB*) pMsg;
    m_sVehicle.SetActiveValue( false );
    m_sVehicle.RESET();
}


void GLCharacter::MsgGetVehicleFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_GET_VEHICLE_FB* pNetMsg = (GLMSG::SNETPC_GET_VEHICLE_FB*) pMsg;

    switch( pNetMsg->emFB ) 
    {
	case EMVEHICLE_GET_FB_ITEM_INFO :
		{
			BYTE iRefPt = pNetMsg->iIndex;

			if( iRefPt < ACCE_TYPE_SIZE )
			{				
				m_sVehicle.m_PutOnItems[iRefPt] = pNetMsg->PutOnItems;
				m_sVehicle.m_sColor[iRefPt]     = pNetMsg->sVehicleColor;
			}
		}
		break;
    case EMVEHICLE_GET_FB_OK:
        {
			m_PutOnItems[SLOT_VEHICLE].m_VehicleDbNum = pNetMsg->basicinfo.m_DbNum;
            m_sVehicle.m_DbNum						  = pNetMsg->basicinfo.m_DbNum;
            m_sVehicle.m_emTYPE						  = pNetMsg->basicinfo.emTYPE;
            m_sVehicle.m_sVehicleID					  = pNetMsg->basicinfo.sVehicleID;
            m_sVehicle.m_nFull						  = pNetMsg->basicinfo.nFull;
            m_sVehicle.m_dwBoosterID				  = pNetMsg->basicinfo.dwBoosterID;

			m_sVehicle.OwnerDbNumSet(pNetMsg->basicinfo.m_OwnerDbNum);
            m_sVehicle.SetActiveValue( true );
            m_sVehicle.ITEM_UPDATE();

            m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
            m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();

            UpdateSuit ( FALSE );
            INIT_DATA( FALSE, FALSE );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_EquipVehicle,
				"-b, -n, -n, -n", 
				true, 
				m_PutOnItems[ SLOT_VEHICLE ].Mid(), 
				m_PutOnItems[ SLOT_VEHICLE ].Sid(),
				m_sVehicle.m_nFull );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

            if ( m_bIsVehicleActive ) 
            {
                ReqSetVehicle( true );
                m_bIsVehicleActive = FALSE;
            }
			/*
            m_PutOnItems[SLOT_VEHICLE].m_VehicleDbNum = pNetMsg->m_DbNum;
            m_sVehicle.m_DbNum = pNetMsg->m_DbNum;
            m_sVehicle.m_emTYPE = pNetMsg->emTYPE;
            m_sVehicle.OwnerDbNumSet(pNetMsg->m_OwnerDbNum);
            m_sVehicle.m_sVehicleID = pNetMsg->sVehicleID;
            m_sVehicle.m_nFull	= pNetMsg->nFull;
            m_sVehicle.m_dwBoosterID = pNetMsg->dwBoosterID;

            for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
            {
                m_sVehicle.m_PutOnItems[i] = pNetMsg->PutOnItems[i];
            }

            for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
            {
                m_sVehicle.m_sColor[i] = pNetMsg->sVehicleColor[i];
            }

            m_sVehicle.SetActiveValue( true );

            m_sVehicle.ITEM_UPDATE();

            m_fVehicleSpeedRate = m_sVehicle.GetSpeedRate();
            m_fVehicleSpeedVol = m_sVehicle.GetSpeedVol();

            UpdateSuit ( FALSE );
            INIT_DATA( FALSE, FALSE );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Vehicle_Equip );

            if ( m_bIsVehicleActive ) 
            {
                ReqSetVehicle( true );
                m_bIsVehicleActive = FALSE;
            }
			*/
        }
        break;
    case EMVEHICLE_GET_FB_FAIL:
        {
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "VEHICLE_GET_FB_FAIL" ));
        }
        break;
    case EMVEHICLE_GET_FB_INVALIDITEM:
        {
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "VEHICLE_GET_FB_INVALIDITEM" ));					
        }
        break;
    case EMVEHICLE_GET_FB_NOITEM:
        {
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "VEHICLE_GET_FB_NOITEM" ));					
        }
        break;			
    case EMVEHICLE_GET_FB_NODATA:
        {
            if ( m_PutOnItems[SLOT_VEHICLE].GetNativeID() != NATIVEID_NULL() ) 
                m_PutOnItems[SLOT_VEHICLE].m_VehicleDbNum = 0;

            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "VEHICLE_GET_FB_NODATA" ));
        }
        break;

    }
}



void GLCharacter::MsgActiveVehicleFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CHAR_ACTIVE_VEHICLE_FC* pPacket = (GLMSG::NET_CHAR_ACTIVE_VEHICLE_FC*) pMsg;

    switch (pPacket->emFB)
    {
    case EMVEHICLE_SET_FB_RESET:
//#ifndef NDEBUG
//        m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("MsgActiveVehicleFb Reset"));
//#endif
        break;

    case EMVEHICLE_SET_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_FAIL") );
        break;

    case EMVEHICLE_SET_FB_OK:
        if (pPacket->bActive)
        {
            m_sVehicle.PassengerIdSet(0, pPacket->dwClientID);
            SetVehicle(true);
            m_sVehicle.BoostReset();

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_RideVehicle,
				"-b, -b", 
				true, 0 != m_sVehicle.BoosterId() ? true : false );
        }
        else
        {
            for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
            {
                m_sVehicle.PassengerIdSet(i, GAEAID_NULL);
            }
            if (pPacket->bLeaveFieldServer)
                m_bIsVehicleActive = true;
            SetVehicle(false);

            m_sVehicle.BoostReset();

            DxPostEffect::GetInstance().RadialBlurActiveOff();

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_VehicleColor_Close );
			GLWidgetScript::GetInstance().LuaCallFunc<void>(
				NSWIDGET_SCRIPT::g_strFunc_GetoffVehicle, "-b", true);
        }
        break;

    case EMVEHICLE_SET_FB_NOTENOUGH_OIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_NOTENOUGH_OIL") );
        break;

    case EMVEHICLE_SET_FB_MAP_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_MAP_FAIL") );
        break;

    case EMVEHICLE_SET_FB_NO_ITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_NO_ITEM") );
        break;

    default:
//#ifndef NDEBUG
//        m_pGaeaClient->PrintMsgText(
//            NS_UITEXTCOLOR::NEGATIVE,
//            sc::string::format(
//                "MsgActiveVehicleFb unknown error %1%", pPacket->emFB));
//#endif
        break;
    }

	// 예약행위에 해당 행위가 완료되었음을 알림
	GLREACTION_TOGGLE_VEHICLE sReaction;
	sReaction.SetACTIVE(pPacket->bActive);
	m_ReservationAction.FeedbackReaction(sReaction);
    
}

void GLCharacter::MsgVehicleColorChangeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE_FB* pPacket = (GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE_FB*)pMsg;

    switch (pPacket->emFB)
    {
    case EMINVEN_VEHICLE_CHANGE_FB_FAIL:
        UpdateSuit();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_BIKECOLOR_CHANGE_FB_FAIL") );
        break;
    case EMINVEN_VEHICLE_CHANGE_FB_OK:
        {
            for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
            {
                m_sVehicle.m_sColor[ i ].dwMainColor = pPacket->dwMainColor[ i ];
                m_sVehicle.m_sColor[ i ].dwSubColor = pPacket->dwSubColor[ i ];
            }
            UpdateSuit();

            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMINVEN_BIKECOLOR_CHANGE_FB_OK") );
        }
        break;
    case EMINVEN_VEHICLE_CHANGE_FB_NOITEM:
        UpdateSuit();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_BIKECOLOR_CHANGE_FB_NOITEM") );
        break;
    case EMINVEN_VEHICLE_CHANGE_FB_BADITEM:
        UpdateSuit();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_BIKECOLOR_CHANGE_FB_BADITEM") );
        break;
    case EMINVEN_VEHICLE_CHANGE_FB_BADCLASS:
        UpdateSuit();
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_BIKECOLOR_CHANGE_FB_BADCLASS") );
        break;
    case EMINVEN_VEHICLE_CHANGE_FB_COOLTIME:
        UpdateSuit();
        break;
    };
}

void GLCharacter::MsgLearnBikeBoostFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_BIKE_BOOST_FB *pNetMsg = (GLMSG::SNETPC_REQ_BIKE_BOOST_FB*)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_LEARN_BIKE_BOOST_FB_FAIL: // different enum types are compared
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_BIKE_BOOST_FB_FAIL") );
        break;
    case EMINVEN_LEARN_BIKE_BOOST_FB_OK: // different enum types are compared
        {
            m_sVehicle.m_dwBoosterID = pNetMsg->dwBoosterID;

            VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find ( m_sVehicle.m_DbNum );
            if ( iter!=m_mapVEHICLEItemInfo.end() )
            {
                SVEHICLEITEMINFO& sVehicle = (*iter).second;
                // 탈것 부스터 배움 정보 갱신
                sVehicle.m_dwBoosterID = pNetMsg->dwBoosterID;

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_BoosterGauge_Learn, "-b", IsRideAVehicleNow() );
            }

            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMINVEN_BIKE_BOOST_FB_OK") );

			// 탈것 UI 업데이트
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Vehicle_Update );
        }
        break;
    case EMINVEN_LEARN_BIKE_BOOST_FB_NOITEM: // different enum types are compared
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_BIKE_BOOST_FB_NOITEM") );
        break;
    case EMINVEN_LEARN_BIKE_BOOST_FB_BADITEM: // different enum types are compared
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_BIKE_BOOST_FB_BADITEM") );
        break;
    case EMINVEN_LEARN_BIKE_BOOST_FB_BADCLASS: // different enum types are compared
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_BIKECOLOR_CHANGE_FB_BADCLASS") );
        break;
    case EMINVEN_LEARN_BIKE_BOOST_FB_COOLTIME: // different enum types are compared
        break;
    };
}


void GLCharacter::MsgUseBikeBoostFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_USE_BIKE_BOOST_FB *pNetMsg = (GLMSG::SNETPC_USE_BIKE_BOOST_FB*)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_USE_BIKE_BOOST_FB_FAIL:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("BIKE_BOOSTER_FB_FAIL") );
        }				
        break;
    case EMINVEN_USE_BIKE_BOOST_FB_OK:
        {
            m_sVehicle.m_bOnceBoost = TRUE;
            m_sVehicle.m_bUseBoost = TRUE;
            m_sVehicle.m_fBoostTime = 0.0f;	
            DxPostEffect::GetInstance().RadialBlurActiveOn();

            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("BIKE_BOOSTER_FB_OK") );

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_BoosterGauge_State,
				"-n", 2 );
        }
        break;
    case EMINVEN_USE_BIKE_BOOST_FB_NOITEM:
        {

        }
        break;
    case EMINVEN_USE_BIKE_BOOST_FB_NOTFULL:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_BATTERY_NOT_ENOUGH") );
        }
        break;
    }
}

void GLCharacter::MsgUseBikeBoostCancelFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_USE_BIKE_BOOST_CANCEL_FB *pNetMsg = (GLMSG::SNETPC_USE_BIKE_BOOST_CANCEL_FB*)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_USE_BIKE_BOOST_CANCEL:
        {
            m_sVehicle.BoostReset();					
            DxPostEffect::GetInstance().RadialBlurActiveOff();

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_BoosterGauge_State,
				"-n", 0 );
        }
        break;

    case EMINVEN_USE_BIKE_BOOST_DELAY_CANCEL:
        {
            m_sVehicle.m_fBoostDelay = 0.0f;
            m_sVehicle.m_bUseBoostDelay = FALSE;

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_BoosterGauge_State,
				"-n", 1 );
        }
        break;
    }
}

void GLCharacter::VehiclePassengerSet(size_t Index, DWORD GaeaId) 
{
	const bool bBeforePasCheck(m_sVehicle.GetPassengerNum() > 1);
	// GaeaId가 GAEAID_NULL로 들어올 수 있음;
	// 탑승해제시 GAEAID_NULL로 보냄;
	m_sVehicle.PassengerIdSet(Index, GaeaId); 

	// 탑승 수락을 따로 받는 곳이없음, 그리하여 GAEAID가 존재하면 탑승 수락이라고 본다;
	// 탑승 거절은 따로 받는다;
	if ( GaeaId != GAEAID_NULL )
	{
		GLWidgetScript::GetInstance().LuaCallFunc<void>(
			NSWIDGET_SCRIPT::g_strFunc_VehicleReqTargetAnswer, "-b, -dw", true, GaeaId);
	}

	const bool bAfterPasCheck(m_sVehicle.GetPassengerNum() > 1);
	if (bBeforePasCheck != bAfterPasCheck)
	{
		GLWidgetScript::GetInstance().LuaCallFunc<void>(
			NSWIDGET_SCRIPT::g_strFunc_QuickMenuUpdateRideWithState, "-b", bAfterPasCheck);
	}
}
