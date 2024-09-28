#include "../pch.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Transport/GLBusStation.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"
#include "../Database/DBAction/DbActionLog.h"
#include "../Club/GLClubMan.h"
#include "./GLGaeaServer.h"
#include "./CharManField.h"

// Move System;
#include "../Move/MoveManagerField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 소환서 아이템 사용 처리;
BOOL GLGaeaServer::RequestInvenTeleport( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
    GLMSG::SNETPC_REQ_INVEN_TELEPORT* pNetMsg =
		static_cast< GLMSG::SNETPC_REQ_INVEN_TELEPORT* >( pMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( NULL == pChar)
		return FALSE;

	GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
	sNetMsgFB.emMoveType = MoveSystem::EMMOVE_TYPE_TELEPORT;

	/// 아이템 정보를 가져온다;
	// 인벤의 아이템 확인;
	SINVENITEM* pINVENITEM = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( NULL == pINVENITEM )
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		SENDTOCLIENT( dwClientID, &sNetMsgFB );

		return FALSE;
	}

	// 아이템 정보 가져옴;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_TELEPORT_CARD || pItem->sDrugOp.emDrug != ITEM_DRUG_CALL_TELEPORT )
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		SENDTOCLIENT( dwClientID, &sNetMsgFB );

		return FALSE;
	}

	// Note : 레벨 조건 확인
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return FALSE;
	}

	/// 아이템과 도착맵 정보를 이용하여 이동 정보를 세팅한다;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();
	sMoveInfo.sMapID = pItem->sBasicOp.sSubID;

	// Map Position 정보를 받아온다;
	const SMAPNODE* pMapNode = FindMapNode( sMoveInfo.sMapID );
	if ( NULL == pMapNode )
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
		SENDTOCLIENT( dwClientID, &sNetMsgFB );

		return FALSE;
	}

	std::tr1::shared_ptr< GLMapAxisInfo > spAxisInfo = GetAxisInfo( pMapNode->m_LevelFile );
	if ( spAxisInfo )
	{
		spAxisInfo->MapPos2MiniPos(
			pItem->sBasicOp.wPosX,
			pItem->sBasicOp.wPosY,
			sMoveInfo.vPosition.x,
			sMoveInfo.vPosition.z );
	}

	/// 이동 정보를 이용하여 이동 로직을 수행한다;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_TELEPORT;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap_UseItem( sMoveInfo, pINVENITEM ) )
		return FALSE;

	/// 아이템을 제거한다;
	// 현재 맵 이동이 실패해도 아이템의 복구가 이루어지지 않기 때문에;
	// 맵 이동이 실패할 경우를 대비한 로직에서 아이템 복구가 이루어져야 한다;
	pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

    return TRUE;
}

// 귀환서, 직전귀환카드, 시작귀환카드 등의 아이템 사용 처리;
BOOL GLGaeaServer::RequestInvenRecall( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
    GLMSG::SNETPC_REQ_INVEN_RECALL* pNetMsg =
		static_cast< GLMSG::SNETPC_REQ_INVEN_RECALL* >( pMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( NULL == pChar)
		return FALSE;

	GLMSG::SNETPC_NOTICE_MOVEMAP_RESULT_AFB	sNetMsgFB;
	sNetMsgFB.emMoveType = MoveSystem::EMMOVE_TYPE_RECALL;

	/// 아이템 정보를 가져온다;
	// 인벤의 아이템 확인;
	SINVENITEM* pINVENITEM = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( NULL == pINVENITEM )
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		SENDTOCLIENT( dwClientID, &sNetMsgFB );

		return FALSE;
	}

	// 아이템 정보 가져옴;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_RECALL )
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		SENDTOCLIENT( dwClientID, &sNetMsgFB );

		return FALSE;
	}

	// Note : 레벨 조건 확인
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return FALSE;
	}

	/// 아이템과 도착맵 정보를 이용하여 이동 정보를 세팅한다;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();

    WORD wSchool = pChar->m_wSchool;
    switch ( pItem->sDrugOp.emDrug )
    {
    default:
        sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_ITEM;
        SENDTOCLIENT( dwClientID, &sNetMsgFB );

        return FALSE;

    case ITEM_DRUG_CALL_SCHOOL:
        sMoveInfo.sMapID = GLCONST_CHAR::nidSTARTMAP[ wSchool ];
        sMoveInfo.dwGateID = GLCONST_CHAR::dwSTARTGATE[ wSchool ];
        break;

    case ITEM_DRUG_CALL_REGEN:
        sMoveInfo.sMapID = pChar->m_sStartMapID;
        sMoveInfo.dwGateID = pChar->m_dwStartGate;
        break;

    case ITEM_DRUG_CALL_LASTCALL:
        if ( SNATIVEID( false ) == pChar->m_sLastCallMapID )
        {
            sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_NOTLASTCALL;
            SENDTOCLIENT( dwClientID, &sNetMsgFB );

            return FALSE;
        }

		const SMAPNODE* pMapNode = FindMapNode( pChar->m_sLastCallMapID );
		if ( NULL == pMapNode )
        {
            pChar->m_sLastCallMapID = SNATIVEID( false );

            sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_INSTANTMAP;
            SENDTOCLIENT( dwClientID, &sNetMsgFB );

            GLMSG::SNETPC_UPDATE_LASTCALL NetMsg;
            NetMsg.sLastCallMapID = SNATIVEID( false );
            SENDTOCLIENT( dwClientID, &NetMsg );

            return FALSE;
        }

        sMoveInfo.sMapID = pChar->m_sLastCallMapID;
        sMoveInfo.dwGateID = UINT_MAX;
        sMoveInfo.vPosition = pChar->m_vLastCallPos;

        // 직전 귀환 위치 리셋;
        pChar->m_sLastCallMapID = SNATIVEID( false );

        GLMSG::SNETPC_UPDATE_LASTCALL NetMsg;
        NetMsg.sLastCallMapID = SNATIVEID( false );
        SENDTOCLIENT( dwClientID, &NetMsg );

        break;
    };

	const SMAPNODE* pMapNode = FindMapNode( sMoveInfo.sMapID );
	if ( NULL == pMapNode)
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_FAIL;
		SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

		return FALSE;
	}

	// GM 이 아닐경우 진입 조건을 검사한다;
	if ( pChar->UserLevel() < USER_USER_GM )
	{
		EMREQFAIL emReqFail = EMREQUIRE_COMPLETE;
		const SLEVEL_REQUIRE* pRequire = pMapNode->m_pLevelRequire;
		emReqFail = pRequire->ISCOMPLETE( pChar );
		if ( emReqFail != EMREQUIRE_COMPLETE )
		{
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_JOINCON;
			SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

			// 시작귀환카드를 사용시 실패했을 경우에 처리한다;
			// 각 학원별 시작맵으로 시작 위치를 리셋한다;
			if ( ITEM_DRUG_CALL_REGEN == pItem->sDrugOp.emDrug )
				pChar->ReSetStartMap();

			return FALSE;
		}

		// MapList 에 Move flag 가 설정되어 있지 않으면 직전귀환카드로 이동 불가능하다;
		if ( false == pMapNode->IsFriendMove() &&
			ITEM_DRUG_CALL_LASTCALL == pItem->sDrugOp.emDrug )
		{
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_IMMOVABLE;
			SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

			return FALSE;
		}
	}

	// 시작귀환카드이거나 학원귀환서일 경우직전 귀환 위치를 저장한다;
	if ( ITEM_DRUG_CALL_SCHOOL == pItem->sDrugOp.emDrug ||
		ITEM_DRUG_CALL_REGEN == pItem->sDrugOp.emDrug )
	{
		if ( pChar->SaveLastCall() == S_OK )
		{
			// 직전귀환 위치 DB에 저장;
			AddGameAdoJob(
				db::DbActionPtr(
				new db::CharacterLastCallPosSet(
				pChar->m_CharDbNum,
				pChar->m_sLastCallMapID.dwID,
				pChar->m_vLastCallPos)));
		}
		else
		{
			// Cdm, GuidBattle일 경우 저장하지 않기 때문에 이전 귀환 위치가 남기때문에;
			// 다시한번 직전 귀환 위치 리셋한다;
			pChar->m_sLastCallMapID = SNATIVEID(false);

			GLMSG::SNETPC_UPDATE_LASTCALL sNetMsg;
			sNetMsg.sLastCallMapID = SNATIVEID( false );
			SENDTOCLIENT( pChar->ClientSlot(), &sNetMsg );
		}
	}

	/// PK 등급이 살인자 등급 이상일 경우 귀환 카드의 사용을 막는다;
	DWORD dwPK_LEVEL = pChar->GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX &&									// 살인자 등급;
		dwPK_LEVEL > GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL	)		// 귀환 가능 레벨;
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_PK_LEVEL;
		SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

		return FALSE;
	}
	/// 이동 정보를 이용하여 이동 로직을 수행한다;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_RECALL;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap_UseItem( sMoveInfo, pINVENITEM ) )
		return FALSE;

	/// 아이템을 제거한다;
	// 현재 맵 이동이 실패해도 아이템의 복구가 이루어지지 않기 때문에;
	// 맵 이동이 실패할 경우를 대비한 로직에서 아이템 복구가 이루어져야 한다;
	pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

    return TRUE;
}

// *****************************************************
// Desc: 소환요청 처리(같은 필드서버에 진입)
// *****************************************************
BOOL GLGaeaServer::RequestInvenRecallThisSvr( GLChar* pPC, const MapID& mapID, DWORD dwGATEID, const D3DXVECTOR3& vPOS )
{
    /*if ( !pPC )
		return FALSE;

	const MapID sCurMapID( pPC->GetCurrentMap() );

	// 이전 맵 정보;
	const SNATIVEID& previousMapID = sCurMapID.getGaeaMapID();

    GLLandMan* pLandMan = NULL;
    DxLandGateMan* pGateMan = NULL;
    PDXLANDGATE pGate = NULL;
    D3DXVECTOR3 vStartPos;

    pLandMan = GetLand (mapID.getGaeaMapID());

    if ( !pLandMan )
		return FALSE;	

	if ( pLandMan->GetGaeaMapID() == SNATIVEID(false) )
		return FALSE;

    if ( dwGATEID!=UINT_MAX )
    {
        pGateMan = pLandMan->GetLandGateMan ();
        pGate = pGateMan->FindLandGate ( dwGATEID );
        
		if ( !pGate )
			return FALSE;

        vStartPos = pGate->GetGenPos ( DxLandGate::GEN_RENDUM );
    }
    else
    {
        vStartPos = vPOS;
    }

    //	Note : 자신이 본 주변 셀을 정리.
    pPC->ResetViewAround ();

    //	Note : 종전 맵에 있던 관리 노드 삭제.
    //
	GLLandMan* pCurrentLandMan(pPC->GetLandMan());
	const MapID& _landMapID(pCurrentLandMan == NULL ? MapID() : pCurrentLandMan->GetMapID());
	if ( pCurrentLandMan != NULL )
	{
		if ( pCurrentLandMan->AddGlobPcDelList(pPC->m_pCharNodeInLand) == false )
		{
			sc::writeLogError(sc::string::format("RequestInvenRecallThisSvr() : pPC->m_pCharNodeInLand is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}


	if (pPC->m_pQuadNode && pPC->m_pQuadNode->pData)
	{
		if ( pPC->m_pQuadNode->pData->m_PCList.DELNODE(pPC->m_pCellNode) == false )
		{
			sc::writeLogError(sc::string::format("RequestInvenRecallThisSvr() : pPC->m_pCellNode is invalid, LandMapID(%1%/%2%, %3%/%4%)",
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}
	else
	{
		sc::writeLogError(sc::string::format("RequestInvenRecallThisSvr() : pPC->m_pQuadNode is invalid, LandMapID(%1%/%2%, %3%/%4%)", 
			_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));		
	}

    //	Note : 새로운 맵 위치로 내비게이션 초기화.
    //
    pPC->SetNavi ( pLandMan->GetNavi(), vStartPos );	
	pPC->SetPosition(vStartPos);

    //
    //mjeon.tbb
    //
    pLandMan->EnterLand(pPC);
    //pPC->m_pLandNode = pLandMan->m_GlobPCList.ADDHEAD ( pPC );



    //	Note : GLLandMan의 셀에 등록하는 작업.
    //			
    //RegistChar ( pPC ); --> ( GLGaeaServer::RequestLandIn ()이 호출될 때까지 유보됨. )

    pPC->m_dwCeID = 0;
    pPC->m_pQuadNode = NULL;
    pPC->m_pCellNode = NULL;

	// 인던 이동 로직에 알린다;
	GLMSG::NET_INSTANCE_MOVE_MAP _moveMapMessage(
		pPC->CharDbNum(),
		previousMapID,
		pPC->GetPosition(),
		GetFieldSvrID(),
		mapID );
	SENDTOMYSELF( &_moveMapMessage );*/

    return TRUE;
}

BOOL GLGaeaServer::RequestBus( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_REQ_BUS* pNetMsg = ( GLMSG::SNETPC_REQ_BUS* )nmg;
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
		return FALSE;

    GLMSG::SNETPC_REQ_BUS_FB NetMsgFB;

    //	케릭이 정상 상태가 아닐 경우.
    if ( !pChar->IsValidBody() )
    {
        NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    //	대련 도중일 경우.
    if ( pChar->m_sCONFTING.IsCONFRONTING() ||
		 pChar->IsCheckedSkillFlagSpecial( EMSPECA_TURN_STATE_BATTLE ) )
    {
        NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    //	거래중일 경우.
    if ( pChar->m_sTrade.Valid() )
    {
        NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    //	Note : pk 등급이 살인자 등급 이상일 경우 버스 사용을 막는다.
    //
    //DWORD dwPK_LEVEL = pChar->GET_PK_LEVEL();
    //if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
    //{
    //	NetMsgFB.emFB = EMBUS_TAKE_PK_LEVEL;
    //	SENDTOCLIENT( dwClientID, &NetMsgFB );
    //	return FALSE;
    //}
    
	GLLandMan* const pLand( pChar->GetLandMan() );

	if( pNetMsg->emCrow == CROW_SUMMON )
	{
		GLSummonField* pCrow = pLand->GetSummon( pNetMsg->dwNpcGlobID );
		if (!pCrow) return E_FAIL;

		if ( !pCrow->IsSummonNpcInteractionAble(pChar->GetPosition(), pChar->GETBODYRADIUS(), SNpcTalk::EM_MARKET ) )		return FALSE;

		if( pCrow->GetSummonType() != SUMMON_TYPE_NPC ) return FALSE;
	}
	else
	{
		PGLCROW pCROW( NULL );

		if ( pLand != NULL )
			pCROW = pLand->GetCrow( pNetMsg->dwNpcGlobID );

		if ( !pCROW )
		{
			NetMsgFB.emFB = EMBUS_TAKE_FAIL;
			SENDTOCLIENT( dwClientID, &NetMsgFB );
			return FALSE;
		}

		// NPC 유효 체크
		if ( !pCROW->IsNpcInteractionAble( pChar->GetPosition(), pChar->GETBODYRADIUS(), SNpcTalk::EM_BUSSTATION ) )
		{
			NetMsgFB.emFB = EMBUS_TAKE_FAIL;
			SENDTOCLIENT( dwClientID, &NetMsgFB );
			return FALSE;
		}

		if ( !pCROW->IsBasicTalk( SNpcTalk::EM_BUSSTATION ) )
		{
			NetMsgFB.emFB = EMBUS_TAKE_FAIL;
			SENDTOCLIENT( dwClientID, &NetMsgFB );
			return FALSE;
		}
	}

    //	인벤의 아이템 확인.
    SINVENITEM *pINVENITEM = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( !pINVENITEM )
    {
        NetMsgFB.emFB = EMBUS_TAKE_TICKET;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    if ( pChar->CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) ) return FALSE;

    //	아이템 정보 가져옴.
    const SITEM* pItem = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
    if ( !pItem || pItem->sBasicOp.emItemType != ITEM_TICKET )
    {
        NetMsgFB.emFB = EMBUS_TAKE_TICKET;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    SNATIVEID sMAPID( false );
    DWORD dwGATEID( UINT_MAX );

    //	Note : 정류장 id가 정확한지 검사.
    //
    const SSTATION* pSTATION = GLBusStation::GetInstance().GetStation ( pNetMsg->dwSTATION_ID );
    if ( !pSTATION )
    {
        NetMsgFB.emFB = EMBUS_TAKE_FAIL;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    //	map id.
    sMAPID.dwID = pSTATION->dwMAPID;

    //	gate id.
    dwGATEID = pSTATION->dwGATEID;


    if ( sMAPID==SNATIVEID( false ) )
    {
        NetMsgFB.emFB = EMBUS_TAKE_FAIL;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    const SMAPNODE *pMapNode = FindMapNode( sMAPID.dwID );
    if ( !pMapNode )
    {
        NetMsgFB.emFB = EMBUS_TAKE_FAIL;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    // 진입조건 검사
    if ( pChar->UserLevel() < USER_USER_GM )
    {
        EMREQFAIL emReqFail( EMREQUIRE_COMPLETE );
        const SLEVEL_REQUIRE* pRequire = pMapNode->m_pLevelRequire;
        emReqFail = pRequire->ISCOMPLETE( pChar ); 
        if ( emReqFail != EMREQUIRE_COMPLETE )
        {
            //	탑승 권한이 없음.
            NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
            SENDTOCLIENT( dwClientID, &NetMsgFB );
            return FALSE;
        }
    }

    //	[자신에게] 인밴 아이탬 소모시킴.
    pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

    //	Note : 탑승 성공 알림.
    NetMsgFB.emFB = EMBUS_TAKE_OK;
    SENDTOCLIENT( dwClientID, &NetMsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_BUS ) )		
	{
		pChar->RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		pChar->RestoreActStateByInvisible();
	}

	/// 이동 진행;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_CHECK;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();
	sMoveInfo.sMapID = sMAPID;
	sMoveInfo.dwGateID = dwGATEID;

	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

    /*// PET
    // 맵이동시 Pet 삭제
    DropOutPET ( pChar->m_dwPetGUID,false,true );

    //VEHICLE
    SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false );

    //SUMMON
    DropOutAllSummon ( pChar );

	// 매크로가 켜져있다면 맵이동시 끈다;
	if ( pChar->IsActState(EM_ACT_MACROMODE) )
	{
		pChar->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT( pChar->ClientSlot(), &msg );
	}

    //	Note : 다른 필드 서버일 경우.
    if ( pMapNode->GetFieldServerNum() != m_dwFieldSvrID )
    {
        GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.dwChaNum = pChar->CharDbNum();
		NetMsgAg.dwFieldServerIDTo = pMapNode->GetFieldServerNum();
        NetMsgAg.mapIDTo = sMAPID;
        NetMsgAg.dwGATEID = dwGATEID;
        NetMsgAg.vPosition = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
        SENDTOAGENT(&NetMsgAg);	//pChar->ClientSlot()
    }
    else
    {
        GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
        
        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다. const MapID sCurMapID(pChar->GetCurrentMap()); 를 사용하여야 한다.;
        const MapID sCurMapID(pChar->GetCurrentMap());

        BOOL bOK = RequestInvenRecallThisSvr ( pChar, sMAPID, dwGATEID, D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX) );
        if ( !bOK )
        {
            NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
            SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
            return FALSE;
        }

        // 버프와 상태이상 제거
        if ( sCurMapID != MapID(sMAPID) )
        {
            for ( int i=0; i<EMBLOW_MULTI; ++i )
                pChar->DISABLEBLOW ( i );
            for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
                pChar->RESETSKEFF ( i );

            // CP 초기화 맵인지 확인
            if (pMapNode->IsCombatPointReset())
                pChar->ResetCP();
        }

        pChar->ResetAction();
		GLLandMan* const pLand = pChar->GetLandMan();

        //	Note : 멥 이동 성공을 알림.
        //
		NetMsgFB.dwChaNum = pChar->CharDbNum();
        NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
        NetMsgFB.sMAPID = sMAPID;
        NetMsgFB.vPOS = pChar->GetPosition();
        SENDTOAGENT(&NetMsgFB);	//pChar->ClientSlot()
    }*/

    return TRUE;
}

BOOL GLGaeaServer::RequestTaxi( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GASSERT( IsField() );
	
	// 에이전트가 보낸 메시지이다. 그래서 안에 캐릭터번호 넣어서 보낸거네. 
    GLMSG::SNETPC_REQ_TAXI* pNetMsg = ( GLMSG::SNETPC_REQ_TAXI* )nmg;
    
	GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
    if ( !pChar )
		return FALSE;

    GLMSG::SNETPC_REQ_TAXI_FB NetMsgFB;

    //	케릭이 정상 상태가 아닐 경우.
    if ( !pChar->IsValidBody() )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    //	아이템을 사용할수 없는 상황이다.
    if ( pChar->IsActionLimit( EMACTION_LIMIT_ITEM | EMACTION_LIMIT_CARD ) )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;	
    }

    SITEM* pVehicleItem = pChar->GET_SLOT_ITEMDATA( SLOT_VEHICLE );	
    if ( pVehicleItem )
    {
        if ( pChar->VehicleState() == EMVEHICLE_DRIVER && !vehicle::CanUseTeleportItem( pVehicleItem->VehicleType() ) )
        {
            return FALSE;                        
        }
        else if ( pChar->VehicleState() == EMVEHICLE_PASSENGER )
        {
            return FALSE;
        }
    }

    //	대련 도중일 경우.
    if ( pChar->m_sCONFTING.IsCONFRONTING() ||
		 pChar->IsCheckedSkillFlagSpecial( EMSPECA_TURN_STATE_BATTLE ) )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    //	거래중일 경우.
    if ( pChar->m_sTrade.Valid() )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    // 정류장 검사
    STAXI_MAP* pTaxiMap = GLTaxiStation::GetInstance().GetTaxiMap ( pNetMsg->dwSelectMap );
    if ( !pTaxiMap )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    // 정류장 검사
    STAXI_STATION* pStation = pTaxiMap->GetStation( pNetMsg->dwSelectStop );
    if ( !pStation )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_STATIONFAIL;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    //	인벤의 아이템 확인.
    SINVENITEM *pINVENITEM = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( !pINVENITEM )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_TICKET;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

	const SNATIVEID itemID( pINVENITEM->sItemCustom.GetNativeID() );
    if ( pChar->CheckCoolTime( itemID ) )
		return FALSE;


    //	아이템 정보 가져옴.
    const SITEM* pItem = GLogicData::GetInstance().GetItem( itemID );
    if ( !pItem || pItem->sBasicOp.emItemType != ITEM_TAXI_CARD )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_TICKET;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

	// Note : 레벨 조건 확인
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return FALSE;
	}

	GLLandMan* pLand( pChar->GetLandMan() );
	if ( pLand == NULL )
	{
		NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
		SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
		return FALSE;			
	}

	InstanceSystem::InstanceField* const pInstance( pLand->GetScriptInstance() );	
	if ( pInstance == NULL )
	{
		const SMAPNODE* pCurMapNode( FindMapNode( pLand->GetBaseMapID() ) );
		if ( !pCurMapNode || pCurMapNode->IsBlockDrugItem( pItem->BasicType() ) )
		{
			NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
			SENDTOCLIENT( dwClientID, &NetMsgFB );
			return FALSE;
		}
	}
	else
	{ // 신규 인던인 경우 스크립트 함수(luaFieldUseItem) 호출;
		pInstance->EventUseItem( pChar->GetCharID(), itemID.Mid(), itemID.Sid(), pStation->dwMAPID );
	}    

    //	소지금액 검사
    const DWORD dwCurMapID( pLand->GetGaeaMapID().Id() );
    LONGLONG lnCharge = GLTaxiStation::GetInstance().GetBasicCharge();

    if ( pStation->dwMAPID != dwCurMapID )
		lnCharge += pStation->dwMapCharge;

    volatile float fSHOP_RATE = pChar->GET_PK_SHOP2BUY();
    volatile float fSHOP_2RATE = fSHOP_RATE + pLand->GetCommissionRate();
    volatile float fSHOP_2RATE_C = fSHOP_2RATE * 0.01f;
    lnCharge = ( LONGLONG )( lnCharge * fSHOP_2RATE_C );

    if ( pChar->GetInvenMoney() < lnCharge )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_MONEY;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return S_FALSE;
    }

    SNATIVEID sMAPID( false );
    DWORD dwNPCID( UINT_MAX );

    //	map id.
    sMAPID.dwID = pStation->dwMAPID;

    //	gate id.
    dwNPCID = pStation->dwNPCID;

    // 맵검사
    if ( sMAPID == SNATIVEID( false ) )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    const SMAPNODE *pMapNode = FindMapNode( sMAPID.dwID );
    if ( !pMapNode )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }
	
	if ( pMapNode->IsBlockDrugItem( pItem->BasicType() ) )
		return S_FALSE;
	
    // 진입조건 검사
    if ( pChar->UserLevel() < USER_USER_GM )
    {
        EMREQFAIL emReqFail( EMREQUIRE_COMPLETE );
        const SLEVEL_REQUIRE* pRequire = pMapNode->m_pLevelRequire;
        emReqFail = pRequire->ISCOMPLETE ( pChar ); 
        if ( emReqFail != EMREQUIRE_COMPLETE )
        {
            //	탑승 권한이 없음.
            NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
            SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
            return FALSE;
        }
    }

    //	NPC 검사

    PGLCROW pCROW( NULL );
    D3DXVECTOR3 vPOS( FLT_MAX,FLT_MAX,FLT_MAX );
    DWORD dwGATEID( UINT_MAX );

    if ( pMapNode->GetFieldServerNum() == m_dwFieldSvrID )
    {
        GLLandMan* pToLandMan = GetLand( sMAPID );

        if ( pToLandMan )
        {
            SNATIVEID sNpcID( dwNPCID );
            if ( pStation->wPosX != 0 && pStation->wPosY != 0 ) 
                pCROW = pToLandMan->GetCrowByID( sNpcID, pStation->wPosX, pStation->wPosY );
            else 
                pCROW = pToLandMan->GetCrowByID( sNpcID );

            if ( !pCROW )
            {
                NetMsgFB.emFB = EMTAXI_TAKE_NPCFAIL;
                SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
                return FALSE;
            }

            D3DXVECTOR3 vDIR = pCROW->GetDirect();
            vDIR = vDIR * 20.0f;

            vPOS = pCROW->GetPosition();
            vPOS += vDIR;
        }	
    }
    else
    {
        vPOS = pNetMsg->vPos;
    }

    //	[자신에게] 인밴 아이탬 소모시킴.
    pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

    //	[자신에게] 사용금액을 소모시킴
    pChar->CheckMoneyUpdate( pChar->GetInvenMoney(), lnCharge, FALSE, "Taxi Charge Money" );
    pChar->SubtractInvenMoneyServer( lnCharge, SUBTRACT_INVEN_MONEY_TAXI );


    //	Note : 탑승 성공 알림.
    NetMsgFB.emFB = EMTAXI_TAKE_OK;
    SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_TAXI ) )
	{
		pChar->RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		pChar->RestoreActStateByInvisible();
	}

    pChar->SendInvenMoneyUpdate();

	/// 이동 진행;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_CHECK;
	sMoveInfo.dwChaDBNum = pNetMsg->dwChaNum;
	sMoveInfo.sMapID = sMAPID;
	sMoveInfo.dwGateID = dwGATEID;
	sMoveInfo.vPosition = vPOS;

	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

    /*// PET
    // 맵이동시 Pet 삭제
    DropOutPET ( pChar->m_dwPetGUID,false,true );

    //VEHICLE
    SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false );

    //SUMMON
    DropOutAllSummon ( pChar );

	// 매크로가 켜져있다면 맵이동시 끈다;
	if ( pChar->IsActState(EM_ACT_MACROMODE) )
	{
		pChar->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT( pChar->ClientSlot(), &msg );
	}

    //	Note : 다른 필드 서버일 경우.
    if ( pMapNode->GetFieldServerNum()!=m_dwFieldSvrID )
    {
        GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
		NetMsgAg.dwChaNum = pChar->CharDbNum();
		NetMsgAg.dwFieldServerIDTo = pMapNode->GetFieldServerNum();
        NetMsgAg.mapIDTo = sMAPID;
        NetMsgAg.dwGATEID = dwGATEID;
        NetMsgAg.vPosition = vPOS;
        SENDTOAGENT(&NetMsgAg);	//pChar->ClientSlot()
    }
    else
    {
        GLMSG::SNETPC_REQ_RECALL_FB	NetMsgFB;
        
        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다. const MapID sCurMapID(pChar->GetCurrentMap()); 를 사용하여야 한다.;
        const MapID sCurMapID(pChar->GetCurrentMap());

        BOOL bOK = RequestInvenRecallThisSvr ( pChar, sMAPID, dwGATEID, vPOS );
        if ( !bOK )
        {
            NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
            SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
            return FALSE;
        }

        // 버프와 상태이상 제거
        if ( sCurMapID != MapID(sMAPID) )
        {
            for ( int i=0; i<EMBLOW_MULTI; ++i )
                pChar->DISABLEBLOW ( i );
            for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
                pChar->RESETSKEFF ( i );

            // CP 초기화 맵인지 확인
            if (pMapNode->IsCombatPointReset())
                pChar->ResetCP();
        }

        pChar->ResetAction();
		GLLandMan* const pLand = pChar->GetLandMan();

        //	Note : 멥 이동 성공을 알림.
        //
		NetMsgFB.dwChaNum = pChar->CharDbNum();
        NetMsgFB.emFB = EMREQ_RECALL_FB_OK;
		NetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
        NetMsgFB.sMAPID = sMAPID;
        NetMsgFB.vPOS = pChar->GetPosition();
        SENDTOAGENT(&NetMsgFB);	//pChar->ClientSlot()
    }*/

    return TRUE;
}

BOOL GLGaeaServer::RequestTaxiNpcPos( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLMSG::SNETPC_REQ_TAXI_NPCPOS* pNetMsg = ( GLMSG::SNETPC_REQ_TAXI_NPCPOS* )nmg;
    GLMSG::SNETPC_REQ_TAXI_NPCPOS_FB NetMsgFB;

    // 정류장 검사
    STAXI_MAP* pTaxiMap = GLTaxiStation::GetInstance().GetTaxiMap ( pNetMsg->dwSelectMap );
    if ( !pTaxiMap )
		return FALSE;

    // 정류장 검사
    STAXI_STATION* pStation = pTaxiMap->GetStation( pNetMsg->dwSelectStop );
    if ( !pStation )
		return FALSE;

    SNATIVEID sMAPID( pStation->dwMAPID );

    // 맵검사
    if ( sMAPID == SNATIVEID( false ) )
		return FALSE;

    GLLandMan* pToLandMan = GetLand( sMAPID );

    if ( !pToLandMan )
		return FALSE;

    SNATIVEID sNpcID( pStation->dwNPCID );
    PGLCROW pCROW;

    if ( pStation->wPosX != 0 && pStation->wPosY != 0 ) 
        pCROW = pToLandMan->GetCrowByID ( sNpcID, pStation->wPosX, pStation->wPosY );
    else 
        pCROW = pToLandMan->GetCrowByID ( sNpcID );

    if ( !pCROW )
    {
        NetMsgFB.vPos = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
        NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
        SENDTOAGENT( &NetMsgFB );
        return FALSE;
    }

    D3DXVECTOR3 vPos = pCROW->GetPosition();
    D3DXVECTOR3 vDir = pCROW->GetDirect();
    vDir = vDir * 20.0f;
    vPos += vDir;

    NetMsgFB.dwSelectMap = pNetMsg->dwSelectMap;
    NetMsgFB.dwSelectStop = pNetMsg->dwSelectStop;
    NetMsgFB.wPosX = pNetMsg->wPosX;
    NetMsgFB.wPosY = pNetMsg->wPosY;
    NetMsgFB.dwGaeaID = pNetMsg->dwGaeaID;
    NetMsgFB.vPos = vPos;

    SENDTOAGENT( &NetMsgFB );	//dwClient

    return TRUE;
}


BOOL GLGaeaServer::MsgReqFieldToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqFieldToInven( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqFieldToHold( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqFieldToHold( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenToHold( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenToHold( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenExInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenExInven( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqStorageExInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqStorageExInven( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenToSlot( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenToSlot( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqSlotToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqSlotToInven( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqVNGainToHold( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqVNGainToHold( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqVNGainExHold( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqVNGainExHold( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqHoldToVNGain( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqHoldToVNGain( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqVNInvenToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqVNInvenToInven( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqVNGainInvenReset( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqVNGainInvenReset( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqSlotToHold( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqSlotToHold( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenSplit( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenSplit( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenToField( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenToField( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqHoldToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqHoldToInven( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenToInven( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqStorageToInven( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqStorageToInven( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqHoldToSlot( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqHoldToSlot( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqSlotChange( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqSlotChange( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqFireCracker( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqFireCracker( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenDrug( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLMSG::SNETPC_REQ_INVENDRUG* pPacket = ( GLMSG::SNETPC_REQ_INVENDRUG* ) nmg;
    unsigned int Crc32 = sc::string::getCrc32( pPacket, sizeof( GLMSG::SNETPC_REQ_INVENDRUG ) - sizeof( unsigned int ) );
    GLChar* pChar = GetChar( dwGaeaID );
    if ( pChar )
    {
        if ( pPacket->Crc32 == Crc32 )
        {
            pChar->MsgReqInvenDrug( pPacket->wPosX, pPacket->wPosY, pPacket->bPetSkill );
            return TRUE;
        }
        else
        {
            sc::writeLogError( sc::string::format( "SNETPC_REQ_INVENDRUG ChaDbNum %1% CRC %2%/%3%", pChar->CharDbNum(), pPacket->Crc32, Crc32 ) );
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqInvenLunchBox( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenLunchBox( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenBoxOpen( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenBoxOpen( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenBoxInfo( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	/*
	#item
	MsgReqInvenBoxInfo 이거 디비에 빌링아이템 목록 받아오는 요청하는건데 계속 보내면 계속 디비에 요청을 계속 보낼 수 있도록 되어 있다.
	클라에서만 1분 쿨타임 체크하고 있다. 클라에서만 클라에서만
	GLSearchDelayMgr 사용해서 딜레이 검사하도록 하자. 아니면 필드에서 사용하는 아이템 딜레이이거 사용해서 할수 있으면 하고
	*/
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( pChar )
    {
        pChar->MsgReqInvenBoxInfo( nmg );
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL GLGaeaServer::MsgReqInvenDisguise( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenDisguise( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenCleanser( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenCleanser( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenChargedItem( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenChargedItem( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenRandomBoxOpen( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
    {
        return FALSE;
    }
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
    {
        return FALSE;
    }

    pChar->MsgReqInvenRandomBoxOpen(dwGaeaID, nmg);

    return TRUE;    
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.16, Randombox Open시 확률제어 적용 여부 요청 응답
 * modified : sckim, 2016.09.20, Redmine #4544 확률/아이템 리스트 제어 기능 적용 
 */
BOOL GLGaeaServer::MsgReqInvenRandomBoxOpenFB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    bool bNotify = false;

    GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF* pPacket = ( GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF* ) nmg;

    GLChar* pChar = GetChar( pPacket->dwGaeaID );
    if ( !pChar )
    {
 #ifndef _RELEASE
        sc::writeLogInfo( "GetChar(pPacket->dwGaeaID):Error" );
 #endif
        return FALSE;
    }

    SITEMCUSTOM sITEM_RandomBox;
    SITEMCUSTOM sITEM_InBox;

    float fItemRate = 0.0f;
    if ( pChar->MsgReqInvenRandomBoxOpenFB( nmg, fItemRate, sITEM_RandomBox, sITEM_InBox ) == S_OK )
    {
 #ifndef _RELEASE
        sc::writeLogInfo( "MsgReqInvenRandomBoxOpenFB : S_OK" );
        sc::writeLogInfo(sc::string::format("RandomBox : [%1%][%2%], InBox : [%3%][%4%]", sITEM_RandomBox.Mid(), sITEM_RandomBox.Sid(), sITEM_InBox.Mid(), sITEM_InBox.Sid()));
 #endif
        if ( GLUseFeatures::GetInstance().IsUsingRandomboxChanceWin_notify() == TRUE )
        {
            // 전체 공지 확률 적용값 이하인지 체크
            //if ( ( fItemRate > 0.0f ) && ( fItemRate <= 5.0f ) )
            if ( ( fItemRate > 0.0f ) && ( m_pRandomChanceCtrl->IsGlobalRC(fItemRate) ) )
            {
 #ifndef _RELEASE
                sc::writeLogInfo(sc::string::format("fItemRate : [%1%]", fItemRate));
 #endif
                bNotify = true;
            }

            if ( !bNotify )
            {
                // 공지 아이템에 속해 있는지 체크
                if ( m_pRandomChanceCtrl->IsRCItem( sITEM_InBox.Id() ) )
                {
                    bNotify = true;
                }
            }

            if ( bNotify )
            {
	            // 획득확률이 0.5% 이하의 아이템이 획득되었을 경우 채팅창에 시스템 메세지로 알려준다.
                // "유저이름" 유저님이 "렌덤상자"에서 "아이템이름"을 획득하였습니다
                // "도라에몽 유저님이 "미스테리금상자"에서 "베노 용무기 교환권"을 획득하였습니다."

                /*
                std::string szChaName = m_pCharMan->GetChaNameByDbNum(pPacket->dwChaNum);
                std::string strText = sc::string::format(ID2SERVERTEXT("RANDOMBOX_CHANCE_WINNER"), szChaName, szRandomboxName);
                std::string strText = sc::string::format(_T("ID2SERVERTEXT:RANDOMBOX_CHANCE_WINNER:[%1%],[%2%]"), szChaName, szRandomboxName);
                */
#ifndef _RELEASE
                sc::writeLogInfo( "IsUsingRandomboxChanceWin_notify() = TRUE" );
 #endif
                GLMSG::SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA NetNotify( pPacket->dwChaNum, sITEM_RandomBox, sITEM_InBox );
                SENDTOAGENT( &NetNotify );

                /*
 		        GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
		        if (NetMsg.SETTEXT(strText))
                {
                    msgpack::sbuffer SendBuffer;
                    msgpack::pack(SendBuffer, NetMsg);
		            SENDTOALLCLIENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
                }
                */
	        }
        }
    }

    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenSelformBoxSelectOpen( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqInvenSelformBoxSelectOpen( nmg );
	return TRUE;   
}

BOOL GLGaeaServer::MsgReqInvenDisJunction( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenDisJunction( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqInvenTLGrinding( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqInvenTLGrinding( nmg );
	return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenGrinding( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenGrinding( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenPeriodExtend( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;

	pChar->MsgReqInvenPeriodExtend( nmg );
	return TRUE;    
}


BOOL GLGaeaServer::MsgReqInvenLockBoxUnLock( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;

	pChar->MsgReqInvenLockBoxUnLock( nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgReqInvenLockBoxOpen( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;

	pChar->MsgReqInvenLockBoxOpen( nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgReqInvenResetSkSt( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenResetSkSt( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqInvenLine( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenLine( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenStorageOpen( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenStorageOpen( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenRemodelOpen( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenRemodelOpen( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenGarbageOpen( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenGarbageOpen( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenStorageClose( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenStorageClose( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenPMarketSearchOpenCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenPMarketSearchOpen( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenPMarketSearchOpenAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	/*
	// 에이전트 서버에서 온 패킷은 GAEAID가 NULL이다;
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    const GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_AF* pNetMsg = ( GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_AF* )nmg;

    GLChar* pChar = GetChar ( pNetMsg->dwGaeaID );
    if ( !pChar )
        return FALSE;

    pChar->MsgReqInvenPMarketSearchOpenFB( nmg );
	*/
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenPremiumSet( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenPremiumSet( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenHairChange( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenHairChange( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenFaceChange( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenFaceChange( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenHairStyleChange( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenHairStyleChange( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenHairColorChange( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenHairColorChange( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenFaceStyleChange( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenFaceStyleChange( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenGenderChange( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenGenderChange( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenSchoolChange( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenSchoolChange( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenRename( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenRename( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgInvenRename( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_INVEN_RENAME_FROM_DB* msg = ( GLMSG::SNETPC_INVEN_RENAME_FROM_DB* )nmg;

    GLChar* pChar = GetCharByDbNum( msg->dwCharID );
    if ( !pChar )
        return FALSE;
    pChar->MsgInvenRename( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenCostumColorChange( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;

    pChar->MsgReqInvenCostumColorChange( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqItemSort( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqItemSort( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqItemSortStart( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqItemSortStart( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqItemSortData( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqItemSortData( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqItemSortEnd( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqItemSortEnd( nmg );
    return TRUE;    
}


BOOL GLGaeaServer::MsgReqInvenBoxWrapping( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenBoxWrapping( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenBoxUnwrapping( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;

    GLChar* pChar = GetChar( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqInvenBoxUnwrapping( nmg );
    return TRUE;   
}

BOOL GLGaeaServer::MsgInvenInitializeCountry (
	DWORD dwClientID,
	DWORD dwGaeaID,
	NET_MSG_GENERIC* pMsg )
{
	if ( GAEAID_NULL == dwGaeaID )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;

	pChar->MsgInvenInitializeCountry( pMsg, dwClientID );

	return TRUE;
}

BOOL GLGaeaServer::MsgItemDecomposeReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar || !pChar->IsValidBody() || pChar->m_sTrade.Valid() )
		return FALSE;

	GLChar* pTarChar = GetCharByDbNum( pChar->m_CharDbNum );
	if ( !pTarChar )
		return FALSE;

	if ( GLUseFeatures::GetInstance().IsUsingItemDecompose() == FALSE )
	{
		sc::writeLogInfo ( "[ Item Decompose ] [Usefeatures is false, but decomposition was requested.]" );
		return FALSE;
	}

	GLMSG::SNETPC_REQ_INVEN_ITEMDECOMPOSE_CF* pNetMsg = ( GLMSG::SNETPC_REQ_INVEN_ITEMDECOMPOSE_CF* )nmg;
	GLMSG::SNETPC_REQ_INVEN_ITEMDECOMPOSE_FB msgFB;
	msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_FAILED;

	//  검사;
	//	인벤의 아이템 확인;
	SINVENITEM *pDECOM_INVENITEM = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( !pDECOM_INVENITEM )
	{
		SENDTOCLIENT( dwClientID, &msgFB );
		return FALSE;
	}

	SNATIVEID& sNid = pDECOM_INVENITEM->GetNativeID();
	msgFB.sDecomposeItem = sNid;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sNid );
	if ( !pItem )
	{
		SENDTOCLIENT( dwClientID, &msgFB );
		return FALSE;
	}

	if ( !pDECOM_INVENITEM->sItemCustom.IsDecompose() )
	{
		msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_NOT_SPLIT;
		SENDTOCLIENT( dwClientID, &msgFB );
		return FALSE;
	}

	CItemNode* pItemNode = GLItemMan::GetInstance().GetItemNode( sNid.Mid(), sNid.Sid() );
	if (NULL == pItemNode)
		return FALSE;

	SGENINFO* const pGenInfo = pItemNode->m_spDecomposeGenInfo.get();
	if (NULL == pGenInfo)
		return FALSE;

	if ( pGenInfo->fProbability < sc::Random::getInstance().GetFloatPercent() )
		return FALSE;

	// 분해 비용이 있는지 확인한다
	const int nDecomposeCost( GLCONST_CHAR::nDecomposeCost );
	if ( pChar->GetInvenMoney() < nDecomposeCost )
	{
		msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_NOT_ENOUGH_MONEY;
		SENDTOCLIENT( dwClientID, &msgFB );
		return FALSE;
	}

	// 분해 결과 아이템 산출;
	float fNowRate = 0.0f;
	std::vector< SGENRESULT > vecResult;
	SGENINFO::VEC_SPGENITEM_CITER iter = pGenInfo->vecGenItem.begin();

	CGENITEM cGenTemp;
	const DWORD dwGenCount( cGenTemp.GenerateNum_RandomOnce( pGenInfo->vecGenNum ) );

	// 인벤토리의 빈 공간을 확인한다;
	if (FALSE == pChar->m_cInventory.ValidCheckInsrt( dwGenCount, FALSE ))
	{
		msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_NOT_ENOUGH_INVEN;
		SENDTOCLIENT( dwClientID, &msgFB );
		return FALSE;
	}
	
	// 실제 분해 아이템 생성 정보를 저장한다;
	for ( DWORD dwIdx = 0; dwIdx < dwGenCount; ++dwIdx)
	{
		SGENRESULT sResult;
		pChar->Decompose_GenerateItem( pGenInfo->vecGenItem, sResult );
		if ( sResult.IsNull() )
		{
			continue;
		}
		vecResult.push_back( sResult );
	}

	// 돈 소모;
	pChar->CheckMoneyUpdate( pChar->GetInvenMoney(), nDecomposeCost, FALSE, "Decompose cost" );
	pChar->SubtractInvenMoneyServer( nDecomposeCost, SUBTRACT_INVEN_MONEY_ITEM_DECOMPOSE );
	pChar->SendInvenMoneyUpdate();

	// 돈 로그;
	pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, -nDecomposeCost, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_DECOMPOSE );
	pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, pChar->GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

	// 통계 및 로그 수정;
	pChar->LogItemExchange(
		pDECOM_INVENITEM->sItemCustom,
		gamelog::ID_CHAR, pChar->m_CharDbNum,
		gamelog::ID_CHAR, 0,
		ITEM_ROUTE_DELETE, pDECOM_INVENITEM->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_DECOMPOSE,
		false );

	// 분해된 아이템 처리;
	// 업데이트 되어 겹침O -> 겹침X로 된 경우도 분해시 하나씩 차감되게하기 위함;
	// 카드 시스템으로 인해 본의아니게 예외처리가 늘었다. ConsumeInvenItem도 수정되었음;
	if ( pItem->ISPILE() || pDECOM_INVENITEM->sItemCustom.wTurnNum > 1 )
		pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, false, 1, true );
	else
		pChar->InvenDeleteItem( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, 1, true );

	// 아이템 지급;
	int ResultSize = static_cast< int >( vecResult.size() );
	for ( int i = 0; i < ResultSize; ++i )
	{
		if ( vecResult[i].Data.sItemID == SNATIVEID( false ) )
			continue;

		// 아이템 생성;
		SITEMCUSTOM sCustomItemNEW( vecResult[i].Data.sItemID );
		CTime cTIME = CTime::GetCurrentTime();
		sCustomItemNEW.tBORNTIME = cTIME.GetTime();
		sCustomItemNEW.cGenType = EMGEN_DECOMPOSE;
		sc::SeqUniqueGUID::Instance()->getGUID( sCustomItemNEW.guid );
		sCustomItemNEW.GenerateBasicStat( false );
		sCustomItemNEW.GenerateLinkSkill();
		sCustomItemNEW.GenerateAddOption();

		SITEM* pITEM = GLogicData::GetInstance().GetItem( sCustomItemNEW.GetNativeID() );
		if ( !pITEM )
		{
			sc::writeLogError(sc::string::format(
				"[ Decompose Log ] [ Failed Create Item ( %1%/%2% ), Target Character : %3% ] ]",
				sCustomItemNEW.GetNativeID().Mid(),
				sCustomItemNEW.GetNativeID().Sid(),
				pChar->CharDbNum()));

			msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_FAILED;
			SENDTOCLIENT( dwClientID, &msgFB );

			return E_FAIL;
		}

		// 아이템 발생; 공간 체크;
		WORD wINSERTX( 0 ), wINSERTY( 0 );
		BOOL bOK = pChar->InvenFindInsertable( wINSERTX, wINSERTY );
		if ( !bOK )
		{
			sc::writeLogError(sc::string::format(
				"[ Decompose Log ] [ Failed Insert Item ( %1%/%2% ), Target Character : %3% ] ]",
				sCustomItemNEW.GetNativeID().Mid(),
				sCustomItemNEW.GetNativeID().Sid(),
				pChar->CharDbNum()));

			msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_FAILED;
			SENDTOCLIENT( dwClientID, &msgFB );

			return FALSE;
		}

		//	생성한 아이템 Inventory에 넣음;
		pChar->InvenInsertItem( sCustomItemNEW, wINSERTX, wINSERTY, false, 1, true, false, true );
		SINVENITEM *pInsertItem = pChar->InvenGetItem( wINSERTX, wINSERTY );

		if ( !pInsertItem )
		{
			sc::writeLogError(sc::string::format(
				"[ Decompose Log ] [ Failed Insert Item ( %1%/%2% ), Target Character : %3% ] ]",
				sCustomItemNEW.GetNativeID().Mid(),
				sCustomItemNEW.GetNativeID().Sid(),
				pChar->CharDbNum()));

			msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_FAILED;
			SENDTOCLIENT( dwClientID, &msgFB );

			return E_FAIL;
		}

		//	[자신에게] 메시지 발생;
		pChar->InvenInsertSend( pInsertItem, FALSE, false, true );

		// 클라이언트에서 분해 결과를 출력해주기 위함;
		msgFB.push_back( pInsertItem->GetNativeID(), pInsertItem->TurnNum() );

		// 통계 및 로그 수정: 분해로인해 생성된 아이템 로그;
		pChar->LogItemExchange( 
			sCustomItemNEW,
			gamelog::ID_NONE, 0,
			gamelog::ID_CHAR, pChar->m_CharDbNum,
			ITEM_ROUTE_DECOMPOSE, sCustomItemNEW.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_DECOMPOSE, 
			false );
	}

	// 결과 전송;
	msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_SUCCESS;
	SENDTOCLIENT( dwClientID, &msgFB );
	return S_OK;
}

BOOL GLGaeaServer::MsgReqInvenCostumeStats( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;

	pChar->MsgReqInvenCostumeStats( nmg );

	return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenCostumeStats_Release( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_INVEN_COSTUME_RELEASE* pPacket = ( GLMSG::SNET_INVEN_COSTUME_RELEASE* ) nmg;
	unsigned int Crc32 = sc::string::getCrc32( pPacket, sizeof( GLMSG::SNET_INVEN_COSTUME_RELEASE ) - sizeof( unsigned int ) );
	GLChar* pChar = GetChar( dwGaeaID );
	if ( pChar )
	{
		if ( pPacket->Crc32 == Crc32 )
		{
			pChar->MsgReqInvenCostumeStats_Release( nmg );
			return TRUE;
		}
		else
		{
			sc::writeLogError( sc::string::format( "SNET_INVEN_COSTUME_RELEASE ChaDbNum %1% CRC %2%/%3%", pChar->CharDbNum(), pPacket->Crc32, Crc32));
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;    
}

BOOL GLGaeaServer::MsgReqChangeCharSlotUseItemCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNETPC_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF* pNetMsg =
		static_cast< GLMSG::SNETPC_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetChar( _dwGaeaID );
	if ( NULL == pChar )
		return FALSE;

	pChar->MsgReqInvenUnlockCharacterSlot( _pNetMsg );

	return TRUE;
}

BOOL GLGaeaServer::MsgReqInvenChangeItemSkillCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_UIOPEN* pNetMsg =
		static_cast< GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_UIOPEN* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetChar( _dwGaeaID );
	if ( NULL == pChar )
		return FALSE;

	pChar->MsgReqInvenChangeItemSkill( _pNetMsg );

	return TRUE;
}

BOOL GLGaeaServer::MsgReqInvenChangeBasicStatsCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNET_INVEN_CHANGE_BASICSTATS_UIOPEN* pNetMsg =
		static_cast< GLMSG::SNET_INVEN_CHANGE_BASICSTATS_UIOPEN* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetChar( _dwGaeaID );
	if ( NULL == pChar )
		return FALSE;

	pChar->MsgReqInvenChangeBasicStats( _pNetMsg );

	return TRUE;
}

BOOL GLGaeaServer::MsgReqInvenChangeAddStatsCF ( DWORD _dwClientID, DWORD _dwGaeaID, NET_MSG_GENERIC* _pNetMsg )
{
	GLMSG::SNET_INVEN_CHANGE_ADDSTATS_UIOPEN* pNetMsg =
		static_cast< GLMSG::SNET_INVEN_CHANGE_ADDSTATS_UIOPEN* >( _pNetMsg );
	if ( NULL == pNetMsg )
		return FALSE;

	GLChar* pChar = GetChar( _dwGaeaID );
	if ( NULL == pChar )
		return FALSE;

	pChar->MsgReqInvenChangeAddStats( _pNetMsg );

	return TRUE;
}