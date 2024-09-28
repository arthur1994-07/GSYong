#include "../pch.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Club/GLClubMan.h"
#include "../Database/DBAction/DbActionGameVehicle.h"
#include "../PrivateMarket/PrivateMarketField.h"
#include "../Util/GLItemLimit.h"
#include "../Vehicle/GLVehicleField.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


BOOL GLGaeaServer::MsgVehicleInvenExSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgVehicleInvenExSlot( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgVehicleInvenToSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgVehicleInvenToSlot( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgVehicleSlotToInven(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgVehicleSlotToInven( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgVehicleRemoveSlot(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgVehicleRemoveSlot( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgVehicleGiveBattery(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgVehicleGiveBattery( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgGetVehicleFullFromDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgGetVehicleFullFromDB( nmg );
    return TRUE;
}

BOOL GLGaeaServer::MsgReqInvenVehicleColorChange(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenVehicleColorChange( nmg );
    return TRUE;    
}

//CAF
BOOL GLGaeaServer::MsgReqVehicleGetOff(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_GET_OFF_WITH_VEHICLE *msg = (GLMSG::SNET_GET_OFF_WITH_VEHICLE *)nmg;
    
	//GLChar* pChar = GetChar ( dwGaeaID );
	GLChar* pChar = GetCharByDbNum(msg->dwChaNum);
    if ( !pChar )
        return FALSE;
    pChar->MsgReqVehicleGetOff( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqLearnBikeBoost(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqLearnBikeBoost( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqUseBikeBoost(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqUseBikeBoost( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqVietnamGainType(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqVietnamGainType( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::ReqActiveVehicle(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (m_bEmptyMsg)
    {
        sc::writeLogError(std::string("GLGaeaServer::ReqActiveVehicle m_bEmptyMsg"));
        return FALSE;
    }

    if (IsReserveServerStop())
        return FALSE;

    GLMSG::NET_CHAR_ACTIVE_VEHICLE_CF* pPacket = (GLMSG::NET_CHAR_ACTIVE_VEHICLE_CF*) nmg;

    GLChar* pChar = GetChar(dwGaeaID);
    if (!pChar)
    {
		GLMSG::NET_CHAR_ACTIVE_VEHICLE_FC NetMsgFB;
        SENDTOCLIENT(dwClientID, &NetMsgFB);
        return FALSE;
    }

	if( pPacket->bActive && pChar->IsProducting() )
	{
		return FALSE;
	}

	if( pPacket->bActive && pChar->IsCheckedSkillFlagSpecial(EMSPECA_DISGUISE)) return FALSE;


	return pChar->ToggleVehicle(pPacket->bActive);
}


//	탑승한 전 캐릭터 다른맵으로 이동 처리한다.
void GLGaeaServer::SetMapMoveVehicle ( DWORD dwClientID, DWORD dwGaeaID, DWORD dwGateToIndex )
{
    if (IsReserveServerStop())
        return;

    GLMSG::SNETPC_MOVE_MAP_WITH_VEHICLE NetMsg;

    GLChar* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return;

    NetMsg.bMove = TRUE;
	NetMsg.dwGateToIndex = dwGateToIndex;

    if (pChar->VehicleState() != EMVEHICLE_DRIVER)
        return;

    for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
    {
        DWORD PassengerId = pChar->VehiclePassengerId(i);
        if (PassengerId == GAEAID_NULL || PassengerId == pChar->GetGaeaID())
            continue;

        GLChar* pPassenger = GetChar(PassengerId);
        if (pPassenger)
            SENDTOCLIENT(pPassenger->ClientSlot(), &NetMsg);
    }
}

void GLGaeaServer::SetActiveVehicle ( DWORD dwClientID, DWORD dwGaeaID, bool bActive, bool bAutoRide )
{
    if (IsReserveServerStop())
        return;

    GLMSG::NET_CHAR_ACTIVE_VEHICLE_FC NetMsgFB;

    GLChar* pChar = GetChar ( dwGaeaID );
    if (!pChar)
    {
        SENDTOCLIENT(dwClientID, &NetMsgFB);
        return;
    }

    if (!pChar->VehicleActiveValue())
    {
        //sc::writeLogError(std::string("SetActiveVehicle VehicleActiveValue false"));
        return;
    }

	if( bActive && pChar->IsProducting() )
	{
		return;
	}

    // 맵소환가능여부 체크
    GLLandMan* pLandMan(pChar->GetLandMan());
    if (!pLandMan)
    {
        sc::writeLogError(std::string("SetActiveVehicle !pLandMan"));
        return;
    }

    if (!pLandMan->IsVehicleActivity () && bActive)
    {
        NetMsgFB.emFB = EMVEHICLE_SET_FB_MAP_FAIL;
        SENDTOCLIENT(dwClientID, &NetMsgFB);
        return;
    }

    SITEMCUSTOM& sItemCostom = pChar->m_PutOnItems[SLOT_VEHICLE];
    if ( sItemCostom.GetNativeID() == NATIVEID_NULL() )
    {
        NetMsgFB.emFB = EMVEHICLE_SET_FB_NO_ITEM;
        SENDTOCLIENT(dwClientID, &NetMsgFB);
        return;
    }

    const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCostom.GetNativeID() );
    if ( !pItem || pItem->sBasicOp.emItemType != ITEM_VEHICLE  )
    {
        NetMsgFB.emFB = EMVEHICLE_SET_FB_NO_ITEM;
        SENDTOCLIENT ( dwClientID, &NetMsgFB );
        return;		
    }	

    if (pChar->VehicleIsNotEnoughFull())
    {
        //NetMsgFB.emFB = EMVEHICLE_SET_FB_NOTENOUGH_OIL;
        //SENDTOCLIENT ( dwClientID, &NetMsgFB );
        return;	
    }

    if ( pChar->m_sCONFTING.IsCONFRONTING() && bActive )
    {
        NetMsgFB.emFB = EMVEHICLE_SET_FB_FAIL;
        SENDTOCLIENT ( dwClientID, &NetMsgFB );
        return;
    }


    //	탑승 해제 요청이고 운전자라면...
    if ( !bActive && pChar->VehicleState() == EMVEHICLE_DRIVER ) 
    {
        for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
        {
            DWORD PassengerId = pChar->VehiclePassengerId(i);
            if (PassengerId == GAEAID_NULL || PassengerId == pChar->GetGaeaID())
                continue;

            GLChar* pPassenger = GetChar(PassengerId);
            if (pPassenger)
                pPassenger->ActiveWithVehicle(bActive, true);
        }
    }
    pChar->ActiveVehicle(bActive, bAutoRide);
    return;
}

BOOL GLGaeaServer::ReqGetVehicle( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if (IsReserveServerStop())
		return FALSE;

    GLMSG::SNETPC_GET_VEHICLE* pNetMsg = (GLMSG::SNETPC_GET_VEHICLE*)nmg;
    GLMSG::SNETPC_GET_VEHICLE_FB NetMsgFB;

    GLChar* pOwner = GetCharByDbNum(pNetMsg->dwChaNum);
    if (!pOwner)
    {
        SENDTOCLIENT(dwClientID, &NetMsgFB);
        return FALSE;
    }

    DWORD ClientSlot = pOwner->ClientSlot();

    // 탈것 활성화 ( 불가능한 경우 ) 
    if (pOwner->VehicleState())
        return FALSE;

    // 탈것 미 장착
    SITEMCUSTOM& sItemCustom = pOwner->m_PutOnItems[SLOT_VEHICLE];
    if (sItemCustom.GetNativeID() == NATIVEID_NULL())
    {
        NetMsgFB.emFB = EMVEHICLE_GET_FB_NOITEM;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return E_FAIL;
    }

    // 장착한 아이템과 요청한 아이템이 다를때
    if (sItemCustom.GetNativeID() != pNetMsg->m_ItemID)
    {
        NetMsgFB.emFB = EMVEHICLE_GET_FB_NOITEM;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return E_FAIL;
    }

    // 요청한 아이템 미존재
    const SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID());
    if (!pItem)
    {
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return E_FAIL;
    }

    if (pItem->sBasicOp.emItemType != ITEM_VEHICLE)
    {
        NetMsgFB.emFB = EMVEHICLE_GET_FB_INVALIDITEM;
        SENDTOCLIENT(ClientSlot, &NetMsgFB);
        return E_FAIL;
    }

    // 에뮬레이터 모드
    if (m_bEmulator)
    {
        GLVEHICLE* pVehicle = GLCONST_VEHICLE::GetVehicleData(pItem->sVehicle.emVehicleType);
        if (!pVehicle)
        {
            SENDTOCLIENT(ClientSlot, &NetMsgFB);
            return E_FAIL;
        }

        pOwner->VehicleAssign(*pVehicle);
        pOwner->VehicleOwnerDbNumSet(pOwner->CharDbNum());
        pOwner->VehicleIdSet(sItemCustom.GetNativeID());

        CreateVehicle(ClientSlot, pOwner->GetGaeaID(), sItemCustom.m_VehicleDbNum);

        return S_OK;
    }

    // 신규생성
    if (sItemCustom.m_VehicleDbNum == 0)
    {
        const GLVEHICLE* pVehicle = GLCONST_VEHICLE::GetVehicleData(pItem->sVehicle.emVehicleType);
        if (!pVehicle)
        {
            SENDTOCLIENT(ClientSlot, &NetMsgFB);
            return E_FAIL;
        }

        AddGameAdoJob(
            db::DbActionPtr(
                new CCreateVehicle(
                    ClientSlot,
                    pOwner->m_CharDbNum, 
                    sItemCustom.GETNAME().c_str(),
                    (int) pItem->sVehicle.emVehicleType,
                    (int) sItemCustom.GetNativeID().wMainID,
                    (int) sItemCustom.GetNativeID().wSubID)));		

        // 일단 생성중인 번호임을 알리기 위해
        sItemCustom.m_VehicleDbNum = UINT_MAX;
        pOwner->VehicleFromDbSet(true);
    }    
    else 
    {
        // DB에서 탈것 정보 가져옵니다.
        // 이미 탈것 부르고 있으면 캔슬
        if (pOwner->VehicleFromDb())
            return TRUE;

        // 로직에 탈것의 정보가 같다면
        if (pOwner->VehicleDbNum() == sItemCustom.VehicleDbNum())
        {
            CreateVehicle(ClientSlot, pOwner->GetGaeaID(), sItemCustom.VehicleDbNum());
            return TRUE;
        }

        const GLVEHICLE* pVehicle = GLCONST_VEHICLE::GetVehicleData(pItem->sVehicle.emVehicleType);
        if (!pVehicle)
        {
            SENDTOCLIENT(ClientSlot, &NetMsgFB);
            return E_FAIL;
        }

        GLVEHICLE* pNewVehicle = new GLVEHICLE; //GLVehicleField(this);
        pNewVehicle->ASSIGN(*pVehicle);

        // DB작업 실패시 혹은 생성후 메모리 해제해줘라
        AddGameAdoJob(
            db::DbActionPtr(
                new CGetVehicle(
                    pNewVehicle,
                    sItemCustom.m_VehicleDbNum,
                    ClientSlot,
                    pOwner->m_CharDbNum)));
        pOwner->VehicleFromDbSet(true);
    }
    return TRUE;
}

BOOL GLGaeaServer::CreateVehicle( DWORD dwClientID, DWORD dwGaeaID, DWORD dwVehicleID )
{
    GLMSG::SNETPC_GET_VEHICLE_FB NetMsg;

    // 요청 케릭터가 유효성 체크
    GLChar* pOwner = GetChar ( dwGaeaID );
    if (!pOwner)
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on CreateVehicle ", dwGaeaID ) );
        return FALSE;
	}

    const CTime cTIME_CUR = CTime::GetCurrentTime();

    // 탈것 아이템 시효성 체크 
    SITEMCUSTOM sCUSTOM = pOwner->m_PutOnItems[SLOT_VEHICLE];
    if ( sCUSTOM.GetNativeID() == NATIVEID_NULL() )
        return FALSE;
    const SITEM* pITEM = GLogicData::GetInstance().GetItem( sCUSTOM.GetNativeID() );
    if ( !pITEM )
        return FALSE;

	if ( sCUSTOM.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
    {
        CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
        CTime cTIME_LMT( sCUSTOM.tBORNTIME );
        cTIME_LMT += cSPAN + sCUSTOM.GetPeriodExBasic();

        if ( cTIME_CUR > cTIME_LMT )
        {
            //	시간 제한으로 아이템 삭제 로그 남김.
			// 통계 및 로그 수정
			m_pItemLimit->LogItemExchange ( sCUSTOM, gamelog::ID_CHAR, pOwner->m_CharDbNum, gamelog::ID_CHAR, 0, ITEM_ROUTE_DELETE, sCUSTOM.wTurnNum, (pOwner == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pOwner->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT, false );

            //	시간 제한으로 아이템 삭제 알림.
            GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
            NetMsgInvenDelTimeLmt.nidITEM = sCUSTOM.GetNativeID();
            SENDTOCLIENT( dwClientID,&NetMsgInvenDelTimeLmt );

            if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && sCUSTOM.m_VehicleDbNum != 0 )
            {
                SaveVehicle( dwClientID, dwGaeaID, false );

                // 활동중이면 사라지게 해준다.
                pOwner->VehicleUpdateTimeLmtItem( pOwner );

                // 넣어줄꺼 넣어준다.
                for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
                {
                    CItemDrop cDropItem;
                    cDropItem.sItemCustom = pOwner->VehiclePutOnItemRef( VEHICLE_ACCESSORYTYPE( i ) );
                    if ( pOwner->IsInsertToInven( &cDropItem ) )
                    {
                        pOwner->InsertToInven( &cDropItem, false, false, false );
                    }
                    else
                    {
						pOwner->DropItem( cDropItem.sItemCustom, pOwner->GetPosition() );
                    }
                }

                AddGameAdoJob(
                    db::DbActionPtr(
                        new CDeleteVehicle(
                            pOwner->ClientSlot(),
                            pOwner->m_CharDbNum,
                            sCUSTOM.m_VehicleDbNum ) ) );
            }

            //	아이템 제거.
            pOwner->RELEASE_SLOT_ITEM_SERVER( SLOT_VEHICLE, true );

            //	[자신에게]  아이탬 제거.
            GLMSG::SNETPC_PUTON_RELEASE NetMsg_Release( SLOT_VEHICLE );
            SENDTOCLIENT( dwClientID,&NetMsg_Release );

            //	Note : 주변 사람들에게 아이탬 제거.
            GLMSG::SNETPC_PUTON_RELEASE_BRD NetMsgReleaseBrd;
            NetMsgReleaseBrd.dwGaeaID = dwGaeaID;
            NetMsgReleaseBrd.emSlot = SLOT_VEHICLE;
            pOwner->SendMsgViewAround( &NetMsgReleaseBrd );	

            return FALSE;
        }
    }


    pOwner->VehicleActiveValueSet( true );

	/*
    NetMsg.emFB         = EMVEHICLE_GET_FB_OK;
    NetMsg.m_DbNum      = pOwner->VehicleDbNum();
    NetMsg.m_OwnerDbNum = pOwner->VehicleOwnerDbNum();
    NetMsg.emTYPE       = pOwner->VehicleType();
    NetMsg.nFull        = pOwner->VehicleFull();
    NetMsg.sVehicleID   = pOwner->VehicleId();
    NetMsg.dwBoosterID  = pOwner->VehicleBoosterId(); 
	*/

	SITEMCUSTOM sCustomTemp[ACCE_TYPE_SIZE];

    for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
    {
        const SITEMCUSTOM& sVehicleItem = pOwner->VehiclePutOnItemRef( VEHICLE_ACCESSORYTYPE( i ) );
        if ( sVehicleItem.GetNativeID() == NATIVEID_NULL() )
            continue;

        const SITEM* pItemData = GLogicData::GetInstance().GetItem( sVehicleItem.GetNativeID() );
        if ( !pItemData )
            continue;

        // 시한부 아이템
		if ( sVehicleItem.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
        {
            CTimeSpan cSPAN( pItemData->sDrugOp.tTIME_LMT );
            CTime cTIME_LMT( sVehicleItem.tBORNTIME );
            cTIME_LMT += cSPAN + sVehicleItem.GetPeriodExBasic();

            if ( cTIME_CUR > cTIME_LMT )
            {
                //	시간 제한으로 아이템 삭제 로그 남김.
				// 통계 및 로그 수정
				m_pItemLimit->LogItemExchange ( sVehicleItem, gamelog::ID_CHAR, pOwner->m_CharDbNum, gamelog::ID_CHAR, 0, ITEM_ROUTE_DELETE, 0, (pOwner == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pOwner->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT,false );

                //	아이템 삭제.
                pOwner->VehiclePutOnItemReset( VEHICLE_ACCESSORYTYPE( i ), true );

                //	시간 제한으로 아이템 삭제 알림.
                GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
                NetMsgInvenDelTimeLmt.nidITEM = sVehicleItem.GetNativeID();
                SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgInvenDelTimeLmt );
            }
        }
		sCustomTemp[i] = pOwner->VehiclePutOnItem( VEHICLE_ACCESSORYTYPE( i ) );
    }

	SVEHICLE_COLOR sVehicleTemp[ACCE_TYPE_SIZE];

    for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
    {
		sVehicleTemp[i] = pOwner->VehicleColor( VEHICLE_ACCESSORYTYPE( i ) );
    }

    pOwner->VehicleItemUpdate();
    pOwner->VehicleSpeedRateSet( pOwner->VehicleSpeedRate() );
    pOwner->VehicleSpeedVolSet( pOwner->VehicleSpeedVol() );
    pOwner->INIT_DATA( FALSE, FALSE );

	NetMsg.emFB = EMVEHICLE_GET_FB_ITEM_INFO;

	for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		NetMsg.PutOnItems    = sCustomTemp[i];
		NetMsg.sVehicleColor = sVehicleTemp[i];
		NetMsg.iIndex        = i;
		
		SENDTOCLIENT( pOwner->ClientSlot(), &NetMsg );
	}

	NetMsg.emFB                   = EMVEHICLE_GET_FB_OK;
	NetMsg.basicinfo.m_DbNum      = pOwner->VehicleDbNum();
	NetMsg.basicinfo.m_OwnerDbNum = pOwner->VehicleOwnerDbNum();
	NetMsg.basicinfo.emTYPE       = pOwner->VehicleType();
	NetMsg.basicinfo.nFull        = pOwner->VehicleFull();
	NetMsg.basicinfo.sVehicleID   = pOwner->VehicleId();
	NetMsg.basicinfo.dwBoosterID  = pOwner->VehicleBoosterId(); 
	NetMsg.PutOnItems             = SITEMCUSTOM( NATIVEID_NULL() );
	NetMsg.sVehicleColor          = SVEHICLE_COLOR();

    SENDTOCLIENT(pOwner->ClientSlot(), &NetMsg);

    // 주변에 알림
    GLMSG::SNETPC_GET_VEHICLE_BRD NetMsgBrd;
    
	/*
    for (int i=0; i<ACCE_TYPE_SIZE; ++i)
        NetMsgBrd.PutOnItems[i] = pOwner->VehiclePutOnItem(VEHICLE_ACCESSORYTYPE(i));

    for (WORD i=0; i<ACCE_TYPE_SIZE; ++i)
        NetMsgBrd.sVehicleColor[i] = pOwner->VehicleColor(VEHICLE_ACCESSORYTYPE(i));
	*/
	NetMsgBrd.dwGaeaID   = pOwner->GetGaeaID();
	NetMsgBrd.eInfoState = EMVEHICLE_BROAD_ITEM_INFO;

	for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
	{
		NetMsgBrd.PutOnItems    = pOwner->VehiclePutOnItem( VEHICLE_ACCESSORYTYPE( i ) );
		NetMsgBrd.sVehicleColor = pOwner->VehicleColor( VEHICLE_ACCESSORYTYPE( i ) );
		NetMsgBrd.iIndex        = i;

		pOwner->SendMsgViewAround( &NetMsgBrd );
	}

	NetMsgBrd.iIndex				   = 0;
	NetMsgBrd.eInfoState			   = EMVEHICLE_BROAD_OK;
	NetMsgBrd.basisInfo.emTYPE         = pOwner->VehicleType();
	NetMsgBrd.basisInfo.m_VehicleDbNum = pOwner->VehicleDbNum();
	NetMsgBrd.basisInfo.sVehicleID     = pOwner->VehicleId();
	
    pOwner->SendMsgViewAround( &NetMsgBrd );

    return TRUE;
}

BOOL GLGaeaServer::CreateVehicleOnDB(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_VEHICLE_CREATE_FROMDB_FB* pPacket = (GLMSG::SNET_VEHICLE_CREATE_FROMDB_FB*) nmg;
    DWORD ChaDbNum = pPacket->m_ChaDbNum;    
    switch (pPacket->emFB) 
    {
    case EMVEHICLE_CREATE_FROMDB_FB_FAIL:
        {
            // 실패하면 보드카드의 팻번호를 다시 0으로 되돌려 놓는다. (재생성 가능하게 하기 위해)
            GLChar* pOwner = GetCharByDbNum(ChaDbNum);
            if (pOwner)
            {
                SITEMCUSTOM& sItemCustom = pOwner->m_PutOnItems[SLOT_VEHICLE];
                if (sItemCustom.GetNativeID() != NATIVEID_NULL())
                    sItemCustom.m_VehicleDbNum = 0;
                pOwner->VehicleFromDbSet(false);
            }
        }
        break;
    case EMVEHICLE_CREATE_FROMDB_FB_OK:
        {
            GLMSG::SNETPC_GET_VEHICLE_FB NetMsgFB;

            GLChar* pOwner = GetCharByDbNum(ChaDbNum);
            if (!pOwner)
			{
				// 통계 및 로그 수정
				// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
				sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on CreateVehicleOnDB ", ChaDbNum));
                return FALSE;
			}

            DWORD GaeaId = pOwner->GetGaeaID();
            DWORD ClientSlot = pOwner->ClientSlot();
            pOwner->VehicleFromDbSet(false);

            SITEMCUSTOM& sItemCustom = pOwner->m_PutOnItems[SLOT_VEHICLE];
            if (sItemCustom.GetNativeID() == NATIVEID_NULL() || sItemCustom.GetNativeID() != pPacket->sVehicleID)
            {
                NetMsgFB.emFB = EMVEHICLE_GET_FB_NOITEM;
                SENDTOCLIENT ( ClientSlot, &NetMsgFB );
                return FALSE;
            }

            const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
            if (!pItem)
            {
                NetMsgFB.emFB = EMVEHICLE_GET_FB_FAIL;
                SENDTOCLIENT(ClientSlot, &NetMsgFB);
                return FALSE;
            }

            if (pItem->BasicType() != ITEM_VEHICLE)
            {
                NetMsgFB.emFB = EMVEHICLE_GET_FB_INVALIDITEM;
                SENDTOCLIENT(ClientSlot, &NetMsgFB);
                return FALSE;
            }

            // 탈것 아이디 설정
            sItemCustom.m_VehicleDbNum = pPacket->m_VehicleDbNum;

            const GLVEHICLE* pVehicle = GLCONST_VEHICLE::GetVehicleData(pItem->sVehicle.emVehicleType);
            if (!pVehicle)
                return FALSE;

            pOwner->VehicleAssign(*pVehicle);
            pOwner->VehicleTypeSet(pItem->VehicleType());
            pOwner->VehicleDbNumSet(pPacket->m_VehicleDbNum);
            pOwner->VehicleOwnerDbNumSet(ChaDbNum);
            pOwner->VehicleIdSet(pPacket->sVehicleID);
            pOwner->VehicleActiveValueSet(true);

            CreateVehicle(ClientSlot, GaeaId, pPacket->m_VehicleDbNum);

            // 탈것 아이템의 정보도 수정해준다.
            GLMSG::SNET_VEHICLE_REQ_ITEM_INFO_FB NetMsg;
			
			/*
            NetMsg.emTYPE		  = pOwner->VehicleType();
            NetMsg.nFull		  = pOwner->VehicleFull();
            NetMsg.m_VehicleDbNum = pOwner->VehicleDbNum();
            NetMsg.dwBoosterID	  = pOwner->VehicleBoosterId();
            NetMsg.bTrade		  = false;

            for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
            {
                NetMsg.PutOnItems[i] = pOwner->VehiclePutOnItem(VEHICLE_ACCESSORYTYPE(i));
            }

            for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
            {
                NetMsg.sVehicleColor[i] = pOwner->VehicleColor(VEHICLE_ACCESSORYTYPE(i));
            }

			SENDTOCLIENT ( ClientSlot, &NetMsg );
			*/
			NetMsg.eStatus        = EMVEHICLE_BROAD_ITEM_INFO;
			NetMsg.emTYPE         = pOwner->VehicleType();
			NetMsg.m_VehicleDbNum = pOwner->VehicleDbNum();
			NetMsg.bTrade		  = false;

			for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
			{
				NetMsg.PutOnItems    = pOwner->VehiclePutOnItem(VEHICLE_ACCESSORYTYPE(i));
				NetMsg.sVehicleColor = pOwner->VehicleColor(VEHICLE_ACCESSORYTYPE(i));
				NetMsg.iIndex        = i;

				SENDTOCLIENT ( ClientSlot, &NetMsg );
			}
			
			NetMsg.eStatus	    	= EMVEHICLE_BROAD_OK;			
			NetMsg.nFull		    = pOwner->VehicleFull();			
			NetMsg.dwBoosterID	    = pOwner->VehicleBoosterId();			
			NetMsg.iIndex           = 0;

            SENDTOCLIENT ( ClientSlot, &NetMsg );

            // 로그에 최초생성을 남김
			// 통계 및 로그 수정
            m_pItemLimit->LogItemExchange(
                sItemCustom,
                gamelog::ID_CHAR,
                pOwner->CharDbNum(),
                gamelog::ID_CHAR,
                0,
                ITEM_ROUTE_VEHICLE,
                1,
				(pOwner == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pOwner->GetCurrentMap(), LOG_ITEM_VEHICLE_CREATE_VEHICLE, 
                false);
        }
        break;
    default:
        GLChar* pOwner = GetCharByDbNum(ChaDbNum);
        if (pOwner)
        {            
            pOwner->VehicleFromDbSet(false);
            sc::writeLogError(
                sc::string::format(
                    "CreateVehicleOnDB %1%",
                    pPacket->emFB));
        }
        return FALSE;
        break;
    };
    return TRUE;
}

BOOL GLGaeaServer::GetVehicleInfoFromDB ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_VEHICLE_GET_FROMDB_FB* pPacket = ( GLMSG::SNET_VEHICLE_GET_FROMDB_FB* ) nmg;
    GLVEHICLE* pVehicle = pPacket->pVehicle;
    if (!pVehicle)
        return FALSE;

    // DB에 없으면 그냥 초기값으로 넘어오는 경우가 있으므로 체크해준다.
    if ( pVehicle->m_emTYPE == VEHICLE_TYPE_NONE )
    {
        SAFE_DELETE( pVehicle );
        return FALSE;
    }

    // 보드 아이템 정보 발신
    if ( pPacket->bCardInfo )
    {
        GLMSG::SNET_VEHICLE_REQ_ITEM_INFO_FB NetMsgFB;
		/*
        NetMsgFB.emTYPE				= pVehicle->m_emTYPE;
        NetMsgFB.nFull				= pVehicle->m_nFull;
        NetMsgFB.m_VehicleDbNum		= pPacket->m_VehicleDbNum;
        NetMsgFB.dwBoosterID		= pVehicle->m_dwBoosterID;
        NetMsgFB.bTrade				= pPacket->bTrade;
        for (WORD i = 0; i < ACCE_TYPE_SIZE; ++i)
            NetMsgFB.PutOnItems[i] = pVehicle->m_PutOnItems[i];

        SENDTOCLIENT(dwClientID, &NetMsgFB); */

		NetMsgFB.eStatus          = EMVEHICLE_BROAD_ITEM_INFO;
		NetMsgFB.emTYPE           = pVehicle->m_emTYPE;
		NetMsgFB.m_VehicleDbNum   = pPacket->m_VehicleDbNum;
		NetMsgFB.bTrade		      = pPacket->bTrade;
		for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
		{
			NetMsgFB.PutOnItems    = pVehicle->m_PutOnItems[i];
			NetMsgFB.iIndex        = i;

			SENDTOCLIENT ( dwClientID, &NetMsgFB );
		}

		NetMsgFB.eStatus		= EMVEHICLE_BROAD_OK;
		NetMsgFB.nFull		    = pVehicle->m_nFull;		
		NetMsgFB.dwBoosterID	= pVehicle->m_dwBoosterID;		
		NetMsgFB.iIndex         = 0;

		SENDTOCLIENT ( dwClientID, &NetMsgFB );
    }    
    else if ( pPacket->bLMTItemCheck )
    {
        // 보드 악세서리의 시효성 검사
        GLChar* pChar = GetChar ( dwGaeaID );
        if ( pChar )
        {
            pVehicle->m_DbNum = pPacket->m_VehicleDbNum;
            
            // 없앨꺼 없애고
            GLVEHICLE_FIELD::UpdateTimeLmtItem( this, pChar, pVehicle );

            // 넣어줄꺼 넣어준다.
            for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
            {
				if ( pVehicle->m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() )
				{
					continue;
				}

                CItemDrop cDropItem;
                cDropItem.sItemCustom = pVehicle->m_PutOnItems[i];
                if ( pChar->IsInsertToInven ( &cDropItem ) ) 
                {
                    pChar->InsertToInven ( &cDropItem, false, false, false );
                }
                else
                {
					pChar->DropItem( cDropItem.sItemCustom, pChar->GetPosition() );
                }
            }

            AddGameAdoJob(
                db::DbActionPtr(
                    new CDeleteVehicle( dwClientID, pChar->m_CharDbNum, pPacket->m_VehicleDbNum ) ) );
        }
    }
    else
    {
        // 탈것을 생성해준다.
        GLChar* pOwner = GetChar ( dwGaeaID );
        if (!pOwner)
		{
			SAFE_DELETE( pVehicle );
            return FALSE;
		}

        pOwner->VehicleAssign( *pVehicle );
        pOwner->VehicleDbNumSet( pPacket->m_VehicleDbNum );
        CreateVehicle( dwClientID, dwGaeaID, pPacket->m_VehicleDbNum );
        pOwner->VehicleFromDbSet( false );
    }

    SAFE_DELETE( pVehicle );
    return TRUE;
}

BOOL GLGaeaServer::GetVehicleInfoFromDBError(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_VEHICLE_GET_FROMDB_ERROR* pPacket = (GLMSG::SNET_VEHICLE_GET_FROMDB_ERROR*) nmg;
    GLChar* pOwner = GetChar(dwGaeaID);
    switch (pPacket->emFB) 
    {
    case EMVEHICLE_GET_FROMDB_ERROR_FB_DBFAIL:
        {
            if (pOwner)
                pOwner->VehicleFromDbSet(false);
        }
        break;

    case EMVEHICLE_GET_FROMDB_ERROR_FB_NODATA:
        {
            GLMSG::SNETPC_GET_VEHICLE_FB NetMsgFB;
            if (!pOwner)
                return FALSE;

            pOwner->VehicleFromDbSet(false);

            if (pPacket->bTrade || pPacket->bCardInfo || pPacket->bLMTItemCheck)
                return FALSE;

            SITEMCUSTOM& sItemCustom = pOwner->m_PutOnItems[SLOT_VEHICLE];
            if (sItemCustom.GetNativeID() != NATIVEID_NULL())
                sItemCustom.m_VehicleDbNum = 0;            

            NetMsgFB.emFB = EMVEHICLE_GET_FB_NODATA;
            SENDTOCLIENT ( dwClientID, &NetMsgFB );	
        }
        break;

    default:
        {            
            if (pOwner)
            {                
                pOwner->VehicleFromDbSet(false);
                sc::writeLogError(
                    sc::string::format(
                        "GetVehicleInfoFromDBError %1%",
                        pPacket->emFB));
            }
            return FALSE;
        }
        break;
    }
    return TRUE;
}


void GLGaeaServer::SaveVehicle( DWORD dwClientID, DWORD dwGaeaID, bool bLeaveFieldServer )
{
    GLChar* pChar = GetChar( dwGaeaID );

	SaveVehicle( dwClientID, pChar, bLeaveFieldServer );
}

void GLGaeaServer::SaveVehicle( DWORD dwClientID, GLChar *pChar, bool bLeaveFieldServer )
{
	if (!pChar)
    {
        sc::writeLogError(sc::string::format("pOwner is Invailed %d", dwClientID ));

        return;
    }

	EMVEHICLE_WHETHER emVehicle = pChar->VehicleState();
    DWORD VehicleDbNum = pChar->VehicleDbNum();
    DWORD dwOwnerID = pChar->VehicleOwnerDbNum();

    if (!pChar->VehicleActiveValue())
        return;

    if ( ( VehicleDbNum <= 0 || dwOwnerID <= 0 )  && emVehicle == EMVEHICLE_DRIVER )
    {
        if ( IsEmulatorMode() )
        {
            pChar->ActiveVehicle( false, bLeaveFieldServer );
        }
        else
        {
            sc::writeLogWarn(
                sc::string::format(
                    "Vehicle ID NULL m_putOnItem %1%/%2% VehicleID %3% dwOwnerID %4% dwCharID %5% emVehicle %6%",
                    pChar->m_PutOnItems[SLOT_VEHICLE].GetNativeID().wMainID,
                    pChar->m_PutOnItems[SLOT_VEHICLE].GetNativeID().wSubID,
                    VehicleDbNum,
                    dwOwnerID,
                    pChar->m_CharDbNum,
                    emVehicle));
            pChar->VehicleActiveValueSet(false);
            return;
        }
    }


    if( pChar->VehicleState() == EMVEHICLE_DRIVER )
    {
        for (INT i=1; i<MAX_PASSENGER_COUNT; ++i)
        {
            if (pChar->VehiclePassengerId(i) == GAEAID_NULL)
                continue;

            GLChar* pPassenger = GetChar(pChar->VehiclePassengerId(i));
            if (pPassenger)
                pPassenger->ActiveWithVehicle(false, false);
        }

        pChar->ActiveVehicle( false, bLeaveFieldServer );	

    }
    else if( pChar->VehicleState() == EMVEHICLE_PASSENGER )
    {
        INT nIndex = -1;
        for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
        {
            if (pChar->VehiclePassengerId(i) == pChar->GetGaeaID())
            {
                nIndex = i;
                break;
            }
        }

        if ( nIndex > 0 && nIndex < MAX_PASSENGER_COUNT ) 
        {
            for (INT j=0; j<MAX_PASSENGER_COUNT; ++j)
            {
                if (pChar->VehiclePassengerId(j) == pChar->GetGaeaID() || pChar->VehiclePassengerId(j) == GAEAID_NULL)
                    continue;

                GLChar* pPassenger = GetChar(pChar->VehiclePassengerId(j));
                if (pPassenger)
                    pPassenger->VehiclePassengerIdReset(nIndex);
            }
        }

        pChar->ActiveWithVehicle( false, false );
    }


    // 보드 데이터 DB저장
    if( VehicleDbNum > 0 && dwOwnerID > 0 )
    {
        AddGameAdoJob(
            db::DbActionPtr(
                new CSetVehicleBattery(
                    dwClientID, pChar->CharDbNum(), VehicleDbNum, pChar->VehicleFull())));

        AddGameAdoJob(
            db::DbActionPtr(
                new CSetVehicleInven(
                    dwClientID, pChar->CharDbNum(), VehicleDbNum, pChar->VehicleConst())));

        AddGameAdoJob(
            db::DbActionPtr(
                new CSetVehicleColor(
                    dwClientID, pChar->CharDbNum(), VehicleDbNum, pChar->VehicleConst())));
    }

    pChar->VehicleActiveValueSet(false);

    GLMSG::SNETPC_UNGET_VEHICLE_FB NetFB;
    SENDTOCLIENT ( dwClientID, &NetFB );

    GLMSG::SNETPC_UNGET_VEHICLE_BRD NetBRD;
    NetBRD.dwGaeaID = pChar->GetGaeaID();
    pChar->SendMsgViewAround(&NetBRD);
}


BOOL GLGaeaServer::GetVehicleItemInfo ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_VEHICLE_REQ_ITEM_INFO* pNetMsg = (GLMSG::SNET_VEHICLE_REQ_ITEM_INFO*)nmg;
    GLChar* pChar = GetChar ( dwGaeaID );
    if (!pChar)
        return FALSE;

    // 팻 생성여부 확인
    if ( pNetMsg->dwVehicleID == 0 ) 
        return FALSE;

    // 거래시 사용될 팻카드 정보이면
    if ( pNetMsg->bTrade )
    {
        GLVEHICLE* pVehicleInfo = new GLVEHICLE; //GLVehicleField(this);

        // DB작업 실패시 혹은 생성후 메모리 해제해줘라
        AddGameAdoJob(
            db::DbActionPtr(
                new CGetVehicle(pVehicleInfo, pNetMsg->dwVehicleID, dwClientID, pChar->m_CharDbNum, false, false, true)));
    }    
    else
    {
        // 인벤토리,창고에 사용될 팻카드 정보이면
//         std::tr1::shared_ptr<GLVEHICLE> spVehicle = m_pVehicleCache->Get(pNetMsg->dwVehicleID);
//         if (spVehicle)
//         {
//             GLMSG::SNET_VEHICLE_REQ_ITEM_INFO_FB NetMsgFB;
//             NetMsgFB.emTYPE				= spVehicle->m_emTYPE;
//             NetMsgFB.nFull				= spVehicle->m_nFull;
//             NetMsgFB.m_VehicleDbNum		= pNetMsg->dwVehicleID;
//             NetMsgFB.dwBoosterID		= spVehicle->m_dwBoosterID;
//             NetMsgFB.bTrade				= pNetMsg->bTrade;
//             for (WORD i = 0; i < ACCE_TYPE_SIZE; ++i)
//                 NetMsgFB.PutOnItems[i] = spVehicle->m_PutOnItems[i];
// 
//             SENDTOCLIENT(dwClientID, &NetMsgFB);
//         }
//         else
//         {
            GLVEHICLE* pVehicleInfo = new GLVEHICLE; //GLVehicleField(this);
            // DB작업 실패시 혹은 생성후 메모리 해제해줘라
            AddGameAdoJob(
                db::DbActionPtr(
                    new CGetVehicle(pVehicleInfo, pNetMsg->dwVehicleID, dwClientID, pChar->m_CharDbNum,  false, true)));
//        }
    }
    return TRUE;
}

//CAF
BOOL GLGaeaServer::WithVehicle ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_WITH_VEHICLE* pPacket = (GLMSG::SNET_WITH_VEHICLE*)nmg;
    GLChar* pMyChar = GetCharByDbNum(pPacket->dwChaNum);
    if (!pMyChar)
        return FALSE;

    INT nIndex = 0;
    for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
    {
        if( pMyChar->VehiclePassengerId(i) == -1 ||
            pMyChar->VehiclePassengerId(i) == GAEAID_NULL)
        {
            continue;
        }
        nIndex++;	
    }

    if( nIndex >= PASSENGER_COUNT  )
    {
        return FALSE;
    }

	if( pMyChar->IsProducting() )
	{
		return FALSE;
	}

    const SITEM* pVehicleItemData = GLogicData::GetInstance().GetItem(pMyChar->VehicleId());
    if (!pVehicleItemData)
    {
        return FALSE;
    }

	if ( pVehicleItemData->sVehicle.bNotUseParts == TRUE )
	{
		if ( pVehicleItemData->sVehicle.emVehicleType == VEHICLE_TYPE_BOARD )
		{
			return FALSE;
		}
	}
	else
    {
        if ( pMyChar->VehiclePutOnItemId(ACCE_TYPE_PARTS_E) == NATIVEID_NULL() )
        {
            return FALSE;
        }
    }

    GLChar* pTarChar = GetChar ( pPacket->dwTargetID );
    if ( !pTarChar )
        return FALSE;

	if( pTarChar->IsProducting() )
	{
		return FALSE;
	}

    //! CTF 진행중에는 다른 학원과 할 수 없음
    if ( pMyChar->IsCTFPlayer() )
    {
        if ( pMyChar->GetSchool() != pTarChar->GetSchool() )
        {
            GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
			NetMsgFB.dwChaNum = pMyChar->CharDbNum();
            NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
            NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_CONFT;

            SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()
            return FALSE;
        }
    }

    if ( pTarChar->VehicleState() )
        return FALSE;

	GLLandMan* const pLand = pMyChar->GetLandMan();
	if ( !pLand )
		return FALSE;

	WORD wTakeRange = pMyChar->GetBodyRadius() + pMyChar->GETBODYRADIUS() + 30;
	WORD wTakeAbleDis = wTakeRange + 2;

	if ( !pLand->IsAbleAttack_Range_LayerSimpleCollision( wTakeAbleDis, pMyChar->GetPosition(), pTarChar->GetPosition(), 5.f ) )
	{
		GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
		NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
		NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_DIST;

		SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

		return FALSE;        
	}

    if( pMyChar->IsDie() )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_MY_DIE;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;
    }

    if( pTarChar->IsDie() )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_TAR_DIE;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;
    }

    if( pMyChar->m_sCONFTING.IsCONFRONTING() )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_CONFT;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;
    }
    if( pTarChar->m_sCONFTING.IsCONFRONTING() )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_CONFT;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;
    }

    if (pMyChar->IsPrivateMarketOpen())
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_PMARKET;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;

    }

    if (pTarChar->IsPrivateMarketOpen())
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_PMARKET;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;
    }

    GLMSG::SNET_WITH_VEHICLE_TAR NetMsg;
	NetMsg.dwChaNum = pTarChar->CharDbNum();
    NetMsg.dwMasterID = pMyChar->GetGaeaID();//dwGaeaID;
    SENDTOAGENT(&NetMsg);	//pTarChar->ClientSlot()

    return TRUE;
}

//CAF
BOOL GLGaeaServer::WithVehicleTarAns ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_WITH_VEHICLE_TAR_ANS* pNetMsg = (GLMSG::SNET_WITH_VEHICLE_TAR_ANS*)nmg;

	GLChar* pMyChar = GetCharByDbNum(pNetMsg->dwChaNum);
    //GLChar* pMyChar = GetChar ( dwGaeaID );
    if ( !pMyChar )
		return FALSE;

    GLChar* pMasterChar = GetChar ( pNetMsg->dwMasterID );
    if ( !pMasterChar )
		return FALSE;

    if ( pMyChar->VehicleState() )
		return FALSE;

	if ( !pMasterChar->VehicleState() )
		return FALSE;

	if( pMyChar->IsProducting() ||
		pMasterChar->IsProducting() )
	{
		GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMasterChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL;

        SENDTOAGENT(&NetMsgFB);	//pMasterChar->ClientSlot()

        return FALSE;
	}

    if ( pNetMsg->emAns == EMWITH_VEHICLE_CANCEL )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMasterChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL;

        SENDTOAGENT(&NetMsgFB);	//pMasterChar->ClientSlot()

        return FALSE;
    } 

	GLLandMan* const pLand = pMyChar->GetLandMan();
	if ( !pLand )
		return FALSE;

    WORD wTakeRange = pMyChar->GetBodyRadius() + pMyChar->GETBODYRADIUS() + 30;
    WORD wTakeAbleDis = wTakeRange + 2;

    if( !pLand->IsAbleAttack_Range_LayerSimpleCollision( wTakeAbleDis, pMyChar->GetPosition(), pMasterChar->GetPosition(), 5.f ) )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_DIST;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;        
    }

    if( pMyChar->IsDie() )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_MY_DIE_ANS;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;
    }

    if( pMasterChar->IsDie() )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_TAR_DIE_ANS;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;
    }

    if( pMyChar->m_sCONFTING.IsCONFRONTING() )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_CONFT;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;
    }
    if( pMasterChar->m_sCONFTING.IsCONFRONTING() )
    {
        GLMSG::SNET_WITH_VEHICLE_FB NetMsgFB;
		NetMsgFB.dwChaNum = pMyChar->CharDbNum();
        NetMsgFB.dwTargetID = pMyChar->GetGaeaID();//dwGaeaID;
        NetMsgFB.emAns = EMWITH_VEHICLE_CANCEL_CONFT;

        SENDTOAGENT(&NetMsgFB);	//pMyChar->ClientSlot()

        return FALSE;
    }

    INT nIndex = 0;
    int nInsert = UINT_MAX;
    for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
    {
        if (pMasterChar->VehiclePassengerId(i) == GAEAID_NULL)
        {
            if (nInsert == UINT_MAX)
                nInsert = i;
            continue;
        }
        nIndex++;	
    }

    if( nIndex >= PASSENGER_COUNT  )
    {
        return FALSE;
    }

    const SITEM* pVehicleItemData = GLogicData::GetInstance().GetItem(pMasterChar->VehicleId());
    if (!pVehicleItemData)
    {
        return FALSE;
    }

    if (!pVehicleItemData->sVehicle.bNotUseParts)
    {

        if (pMasterChar->VehiclePutOnItemId(ACCE_TYPE_PARTS_E) == NATIVEID_NULL())
        {
            return FALSE;
        }
    }

    if (nInsert < 0 || nInsert >= MAX_PASSENGER_COUNT)
        return FALSE;

    for (int i=0; i<MAX_PASSENGER_COUNT; ++i) 
    {
		DWORD dwID = pMasterChar->VehiclePassengerId(i);
		
		if (dwID == GAEAID_NULL)
			continue;

        GLChar* pPassenger = GetChar(dwID);
        if (!pPassenger)
            continue;

        if (pPassenger->VehiclePassengerId(nInsert) != GAEAID_NULL)
            continue;

        pPassenger->VehiclePassengerIdSet(nInsert, pMyChar->GetGaeaID());//dwGaeaID);
    }


    for (int i=0; i<MAX_PASSENGER_COUNT; ++i)
    {
        pMyChar->VehiclePassengerIdSet(i, pMasterChar->VehiclePassengerId(i));
    }

    pMyChar->ActiveWithVehicle(true, false);

    return TRUE;
}
