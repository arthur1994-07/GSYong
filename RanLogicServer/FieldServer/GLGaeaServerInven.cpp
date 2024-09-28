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

// ��ȯ�� ������ ��� ó��;
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

	/// ������ ������ �����´�;
	// �κ��� ������ Ȯ��;
	SINVENITEM* pINVENITEM = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( NULL == pINVENITEM )
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		SENDTOCLIENT( dwClientID, &sNetMsgFB );

		return FALSE;
	}

	// ������ ���� ������;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_TELEPORT_CARD || pItem->sDrugOp.emDrug != ITEM_DRUG_CALL_TELEPORT )
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		SENDTOCLIENT( dwClientID, &sNetMsgFB );

		return FALSE;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return FALSE;
	}

	/// �����۰� ������ ������ �̿��Ͽ� �̵� ������ �����Ѵ�;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();
	sMoveInfo.sMapID = pItem->sBasicOp.sSubID;

	// Map Position ������ �޾ƿ´�;
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

	/// �̵� ������ �̿��Ͽ� �̵� ������ �����Ѵ�;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_TELEPORT;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap_UseItem( sMoveInfo, pINVENITEM ) )
		return FALSE;

	/// �������� �����Ѵ�;
	// ���� �� �̵��� �����ص� �������� ������ �̷������ �ʱ� ������;
	// �� �̵��� ������ ��츦 ����� �������� ������ ������ �̷������ �Ѵ�;
	pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

    return TRUE;
}

// ��ȯ��, ������ȯī��, ���۱�ȯī�� ���� ������ ��� ó��;
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

	/// ������ ������ �����´�;
	// �κ��� ������ Ȯ��;
	SINVENITEM* pINVENITEM = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
	if ( NULL == pINVENITEM )
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		SENDTOCLIENT( dwClientID, &sNetMsgFB );

		return FALSE;
	}

	// ������ ���� ������;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_RECALL )
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_ITEM;
		SENDTOCLIENT( dwClientID, &sNetMsgFB );

		return FALSE;
	}

	// Note : ���� ���� Ȯ��
	//
	if ( !pChar->SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return FALSE;
	}

	/// �����۰� ������ ������ �̿��Ͽ� �̵� ������ �����Ѵ�;
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

        // ���� ��ȯ ��ġ ����;
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

	// GM �� �ƴҰ�� ���� ������ �˻��Ѵ�;
	if ( pChar->UserLevel() < USER_USER_GM )
	{
		EMREQFAIL emReqFail = EMREQUIRE_COMPLETE;
		const SLEVEL_REQUIRE* pRequire = pMapNode->m_pLevelRequire;
		emReqFail = pRequire->ISCOMPLETE( pChar );
		if ( emReqFail != EMREQUIRE_COMPLETE )
		{
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_JOINCON;
			SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

			// ���۱�ȯī�带 ���� �������� ��쿡 ó���Ѵ�;
			// �� �п��� ���۸����� ���� ��ġ�� �����Ѵ�;
			if ( ITEM_DRUG_CALL_REGEN == pItem->sDrugOp.emDrug )
				pChar->ReSetStartMap();

			return FALSE;
		}

		// MapList �� Move flag �� �����Ǿ� ���� ������ ������ȯī��� �̵� �Ұ����ϴ�;
		if ( false == pMapNode->IsFriendMove() &&
			ITEM_DRUG_CALL_LASTCALL == pItem->sDrugOp.emDrug )
		{
			sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_IMMOVABLE;
			SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

			return FALSE;
		}
	}

	// ���۱�ȯī���̰ų� �п���ȯ���� ������� ��ȯ ��ġ�� �����Ѵ�;
	if ( ITEM_DRUG_CALL_SCHOOL == pItem->sDrugOp.emDrug ||
		ITEM_DRUG_CALL_REGEN == pItem->sDrugOp.emDrug )
	{
		if ( pChar->SaveLastCall() == S_OK )
		{
			// ������ȯ ��ġ DB�� ����;
			AddGameAdoJob(
				db::DbActionPtr(
				new db::CharacterLastCallPosSet(
				pChar->m_CharDbNum,
				pChar->m_sLastCallMapID.dwID,
				pChar->m_vLastCallPos)));
		}
		else
		{
			// Cdm, GuidBattle�� ��� �������� �ʱ� ������ ���� ��ȯ ��ġ�� ���⶧����;
			// �ٽ��ѹ� ���� ��ȯ ��ġ �����Ѵ�;
			pChar->m_sLastCallMapID = SNATIVEID(false);

			GLMSG::SNETPC_UPDATE_LASTCALL sNetMsg;
			sNetMsg.sLastCallMapID = SNATIVEID( false );
			SENDTOCLIENT( pChar->ClientSlot(), &sNetMsg );
		}
	}

	/// PK ����� ������ ��� �̻��� ��� ��ȯ ī���� ����� ���´�;
	DWORD dwPK_LEVEL = pChar->GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX &&									// ������ ���;
		dwPK_LEVEL > GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL	)		// ��ȯ ���� ����;
	{
		sNetMsgFB.emFB = EMREQ_MOVEMAP_RESULT_AFB_PK_LEVEL;
		SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

		return FALSE;
	}
	/// �̵� ������ �̿��Ͽ� �̵� ������ �����Ѵ�;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_RECALL;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap_UseItem( sMoveInfo, pINVENITEM ) )
		return FALSE;

	/// �������� �����Ѵ�;
	// ���� �� �̵��� �����ص� �������� ������ �̷������ �ʱ� ������;
	// �� �̵��� ������ ��츦 ����� �������� ������ ������ �̷������ �Ѵ�;
	pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

    return TRUE;
}

// *****************************************************
// Desc: ��ȯ��û ó��(���� �ʵ弭���� ����)
// *****************************************************
BOOL GLGaeaServer::RequestInvenRecallThisSvr( GLChar* pPC, const MapID& mapID, DWORD dwGATEID, const D3DXVECTOR3& vPOS )
{
    /*if ( !pPC )
		return FALSE;

	const MapID sCurMapID( pPC->GetCurrentMap() );

	// ���� �� ����;
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

    //	Note : �ڽ��� �� �ֺ� ���� ����.
    pPC->ResetViewAround ();

    //	Note : ���� �ʿ� �ִ� ���� ��� ����.
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

    //	Note : ���ο� �� ��ġ�� ������̼� �ʱ�ȭ.
    //
    pPC->SetNavi ( pLandMan->GetNavi(), vStartPos );	
	pPC->SetPosition(vStartPos);

    //
    //mjeon.tbb
    //
    pLandMan->EnterLand(pPC);
    //pPC->m_pLandNode = pLandMan->m_GlobPCList.ADDHEAD ( pPC );



    //	Note : GLLandMan�� ���� ����ϴ� �۾�.
    //			
    //RegistChar ( pPC ); --> ( GLGaeaServer::RequestLandIn ()�� ȣ��� ������ ������. )

    pPC->m_dwCeID = 0;
    pPC->m_pQuadNode = NULL;
    pPC->m_pCellNode = NULL;

	// �δ� �̵� ������ �˸���;
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

    //	�ɸ��� ���� ���°� �ƴ� ���.
    if ( !pChar->IsValidBody() )
    {
        NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    //	��� ������ ���.
    if ( pChar->m_sCONFTING.IsCONFRONTING() ||
		 pChar->IsCheckedSkillFlagSpecial( EMSPECA_TURN_STATE_BATTLE ) )
    {
        NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    //	�ŷ����� ���.
    if ( pChar->m_sTrade.Valid() )
    {
        NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    //	Note : pk ����� ������ ��� �̻��� ��� ���� ����� ���´�.
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

		// NPC ��ȿ üũ
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

    //	�κ��� ������ Ȯ��.
    SINVENITEM *pINVENITEM = pChar->InvenFindPosItem( pNetMsg->wPosX, pNetMsg->wPosY );
    if ( !pINVENITEM )
    {
        NetMsgFB.emFB = EMBUS_TAKE_TICKET;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    if ( pChar->CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) ) return FALSE;

    //	������ ���� ������.
    const SITEM* pItem = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
    if ( !pItem || pItem->sBasicOp.emItemType != ITEM_TICKET )
    {
        NetMsgFB.emFB = EMBUS_TAKE_TICKET;
        SENDTOCLIENT( dwClientID, &NetMsgFB );
        return FALSE;
    }

    SNATIVEID sMAPID( false );
    DWORD dwGATEID( UINT_MAX );

    //	Note : ������ id�� ��Ȯ���� �˻�.
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

    // �������� �˻�
    if ( pChar->UserLevel() < USER_USER_GM )
    {
        EMREQFAIL emReqFail( EMREQUIRE_COMPLETE );
        const SLEVEL_REQUIRE* pRequire = pMapNode->m_pLevelRequire;
        emReqFail = pRequire->ISCOMPLETE( pChar ); 
        if ( emReqFail != EMREQUIRE_COMPLETE )
        {
            //	ž�� ������ ����.
            NetMsgFB.emFB = EMBUS_TAKE_CONDITION;
            SENDTOCLIENT( dwClientID, &NetMsgFB );
            return FALSE;
        }
    }

    //	[�ڽſ���] �ι� ������ �Ҹ��Ŵ.
    pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

    //	Note : ž�� ���� �˸�.
    NetMsgFB.emFB = EMBUS_TAKE_OK;
    SENDTOCLIENT( dwClientID, &NetMsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_BUS ) )		
	{
		pChar->RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		pChar->RestoreActStateByInvisible();
	}

	/// �̵� ����;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_CHECK;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();
	sMoveInfo.sMapID = sMAPID;
	sMoveInfo.dwGateID = dwGATEID;

	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

    /*// PET
    // ���̵��� Pet ����
    DropOutPET ( pChar->m_dwPetGUID,false,true );

    //VEHICLE
    SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false );

    //SUMMON
    DropOutAllSummon ( pChar );

	// ��ũ�ΰ� �����ִٸ� ���̵��� ����;
	if ( pChar->IsActState(EM_ACT_MACROMODE) )
	{
		pChar->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT( pChar->ClientSlot(), &msg );
	}

    //	Note : �ٸ� �ʵ� ������ ���.
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
        
        // ����!: RequestInvenRecallThisSvr �Լ� ȣ�� �Ŀ��� �ɸ����� ���� mapID�� �̵��ϴ� mapID�� ����Ǿ����;
        // �׷��� ���� �� ������ pChar->GetCurrentMapID() �� ����ϸ� �ȵȴ�. const MapID sCurMapID(pChar->GetCurrentMap()); �� ����Ͽ��� �Ѵ�.;
        const MapID sCurMapID(pChar->GetCurrentMap());

        BOOL bOK = RequestInvenRecallThisSvr ( pChar, sMAPID, dwGATEID, D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX) );
        if ( !bOK )
        {
            NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
            SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
            return FALSE;
        }

        // ������ �����̻� ����
        if ( sCurMapID != MapID(sMAPID) )
        {
            for ( int i=0; i<EMBLOW_MULTI; ++i )
                pChar->DISABLEBLOW ( i );
            for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
                pChar->RESETSKEFF ( i );

            // CP �ʱ�ȭ ������ Ȯ��
            if (pMapNode->IsCombatPointReset())
                pChar->ResetCP();
        }

        pChar->ResetAction();
		GLLandMan* const pLand = pChar->GetLandMan();

        //	Note : �� �̵� ������ �˸�.
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
	
	// ������Ʈ�� ���� �޽����̴�. �׷��� �ȿ� ĳ���͹�ȣ �־ �����ų�. 
    GLMSG::SNETPC_REQ_TAXI* pNetMsg = ( GLMSG::SNETPC_REQ_TAXI* )nmg;
    
	GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
    if ( !pChar )
		return FALSE;

    GLMSG::SNETPC_REQ_TAXI_FB NetMsgFB;

    //	�ɸ��� ���� ���°� �ƴ� ���.
    if ( !pChar->IsValidBody() )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    //	�������� ����Ҽ� ���� ��Ȳ�̴�.
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

    //	��� ������ ���.
    if ( pChar->m_sCONFTING.IsCONFRONTING() ||
		 pChar->IsCheckedSkillFlagSpecial( EMSPECA_TURN_STATE_BATTLE ) )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    //	�ŷ����� ���.
    if ( pChar->m_sTrade.Valid() )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    // ������ �˻�
    STAXI_MAP* pTaxiMap = GLTaxiStation::GetInstance().GetTaxiMap ( pNetMsg->dwSelectMap );
    if ( !pTaxiMap )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_MAPFAIL;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    // ������ �˻�
    STAXI_STATION* pStation = pTaxiMap->GetStation( pNetMsg->dwSelectStop );
    if ( !pStation )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_STATIONFAIL;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

    //	�κ��� ������ Ȯ��.
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


    //	������ ���� ������.
    const SITEM* pItem = GLogicData::GetInstance().GetItem( itemID );
    if ( !pItem || pItem->sBasicOp.emItemType != ITEM_TAXI_CARD )
    {
        NetMsgFB.emFB = EMTAXI_TAKE_TICKET;
        SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
        return FALSE;
    }

	// Note : ���� ���� Ȯ��
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
	{ // �ű� �δ��� ��� ��ũ��Ʈ �Լ�(luaFieldUseItem) ȣ��;
		pInstance->EventUseItem( pChar->GetCharID(), itemID.Mid(), itemID.Sid(), pStation->dwMAPID );
	}    

    //	�����ݾ� �˻�
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

    // �ʰ˻�
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
	
    // �������� �˻�
    if ( pChar->UserLevel() < USER_USER_GM )
    {
        EMREQFAIL emReqFail( EMREQUIRE_COMPLETE );
        const SLEVEL_REQUIRE* pRequire = pMapNode->m_pLevelRequire;
        emReqFail = pRequire->ISCOMPLETE ( pChar ); 
        if ( emReqFail != EMREQUIRE_COMPLETE )
        {
            //	ž�� ������ ����.
            NetMsgFB.emFB = EMTAXI_TAKE_CONDITION;
            SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
            return FALSE;
        }
    }

    //	NPC �˻�

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

    //	[�ڽſ���] �ι� ������ �Ҹ��Ŵ.
    pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, true );

    //	[�ڽſ���] ���ݾ��� �Ҹ��Ŵ
    pChar->CheckMoneyUpdate( pChar->GetInvenMoney(), lnCharge, FALSE, "Taxi Charge Money" );
    pChar->SubtractInvenMoneyServer( lnCharge, SUBTRACT_INVEN_MONEY_TAXI );


    //	Note : ž�� ���� �˸�.
    NetMsgFB.emFB = EMTAXI_TAKE_OK;
    SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );

	if ( GLHIDESET::CheckHideSet( EMHIDE_TAXI ) )
	{
		pChar->RemoveSkillFactBySpecial( EMSPECA_INVISIBLE );	
		pChar->RestoreActStateByInvisible();
	}

    pChar->SendInvenMoneyUpdate();

	/// �̵� ����;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_CHECK;
	sMoveInfo.dwChaDBNum = pNetMsg->dwChaNum;
	sMoveInfo.sMapID = sMAPID;
	sMoveInfo.dwGateID = dwGATEID;
	sMoveInfo.vPosition = vPOS;

	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

    /*// PET
    // ���̵��� Pet ����
    DropOutPET ( pChar->m_dwPetGUID,false,true );

    //VEHICLE
    SetActiveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), false );

    //SUMMON
    DropOutAllSummon ( pChar );

	// ��ũ�ΰ� �����ִٸ� ���̵��� ����;
	if ( pChar->IsActState(EM_ACT_MACROMODE) )
	{
		pChar->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT( pChar->ClientSlot(), &msg );
	}

    //	Note : �ٸ� �ʵ� ������ ���.
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
        
        // ����!: RequestInvenRecallThisSvr �Լ� ȣ�� �Ŀ��� �ɸ����� ���� mapID�� �̵��ϴ� mapID�� ����Ǿ����;
        // �׷��� ���� �� ������ pChar->GetCurrentMapID() �� ����ϸ� �ȵȴ�. const MapID sCurMapID(pChar->GetCurrentMap()); �� ����Ͽ��� �Ѵ�.;
        const MapID sCurMapID(pChar->GetCurrentMap());

        BOOL bOK = RequestInvenRecallThisSvr ( pChar, sMAPID, dwGATEID, vPOS );
        if ( !bOK )
        {
            NetMsgFB.emFB = EMREQ_RECALL_FB_FAIL;
            SENDTOCLIENT( pChar->ClientSlot(), &NetMsgFB );
            return FALSE;
        }

        // ������ �����̻� ����
        if ( sCurMapID != MapID(sMAPID) )
        {
            for ( int i=0; i<EMBLOW_MULTI; ++i )
                pChar->DISABLEBLOW ( i );
            for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
                pChar->RESETSKEFF ( i );

            // CP �ʱ�ȭ ������ Ȯ��
            if (pMapNode->IsCombatPointReset())
                pChar->ResetCP();
        }

        pChar->ResetAction();
		GLLandMan* const pLand = pChar->GetLandMan();

        //	Note : �� �̵� ������ �˸�.
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

    // ������ �˻�
    STAXI_MAP* pTaxiMap = GLTaxiStation::GetInstance().GetTaxiMap ( pNetMsg->dwSelectMap );
    if ( !pTaxiMap )
		return FALSE;

    // ������ �˻�
    STAXI_STATION* pStation = pTaxiMap->GetStation( pNetMsg->dwSelectStop );
    if ( !pStation )
		return FALSE;

    SNATIVEID sMAPID( pStation->dwMAPID );

    // �ʰ˻�
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
	MsgReqInvenBoxInfo �̰� ��� ���������� ��� �޾ƿ��� ��û�ϴ°ǵ� ��� ������ ��� ��� ��û�� ��� ���� �� �ֵ��� �Ǿ� �ִ�.
	Ŭ�󿡼��� 1�� ��Ÿ�� üũ�ϰ� �ִ�. Ŭ�󿡼��� Ŭ�󿡼���
	GLSearchDelayMgr ����ؼ� ������ �˻��ϵ��� ����. �ƴϸ� �ʵ忡�� ����ϴ� ������ �������̰� ����ؼ� �Ҽ� ������ �ϰ�
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
 * redmine : #1161 Randombox Ȯ������ �ý��� ó��
 * created : sckim, 2015.12.16, Randombox Open�� Ȯ������ ���� ���� ��û ����
 * modified : sckim, 2016.09.20, Redmine #4544 Ȯ��/������ ����Ʈ ���� ��� ���� 
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
            // ��ü ���� Ȯ�� ���밪 �������� üũ
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
                // ���� �����ۿ� ���� �ִ��� üũ
                if ( m_pRandomChanceCtrl->IsRCItem( sITEM_InBox.Id() ) )
                {
                    bNotify = true;
                }
            }

            if ( bNotify )
            {
	            // ȹ��Ȯ���� 0.5% ������ �������� ȹ��Ǿ��� ��� ä��â�� �ý��� �޼����� �˷��ش�.
                // "�����̸�" �������� "��������"���� "�������̸�"�� ȹ���Ͽ����ϴ�
                // "���󿡸� �������� "�̽��׸��ݻ���"���� "���� �빫�� ��ȯ��"�� ȹ���Ͽ����ϴ�."

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
	// ������Ʈ �������� �� ��Ŷ�� GAEAID�� NULL�̴�;
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

	//  �˻�;
	//	�κ��� ������ Ȯ��;
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

	// ���� ����� �ִ��� Ȯ���Ѵ�
	const int nDecomposeCost( GLCONST_CHAR::nDecomposeCost );
	if ( pChar->GetInvenMoney() < nDecomposeCost )
	{
		msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_NOT_ENOUGH_MONEY;
		SENDTOCLIENT( dwClientID, &msgFB );
		return FALSE;
	}

	// ���� ��� ������ ����;
	float fNowRate = 0.0f;
	std::vector< SGENRESULT > vecResult;
	SGENINFO::VEC_SPGENITEM_CITER iter = pGenInfo->vecGenItem.begin();

	CGENITEM cGenTemp;
	const DWORD dwGenCount( cGenTemp.GenerateNum_RandomOnce( pGenInfo->vecGenNum ) );

	// �κ��丮�� �� ������ Ȯ���Ѵ�;
	if (FALSE == pChar->m_cInventory.ValidCheckInsrt( dwGenCount, FALSE ))
	{
		msgFB.emFeedback = EMREQ_ITEMDECOMPOSE_FB_NOT_ENOUGH_INVEN;
		SENDTOCLIENT( dwClientID, &msgFB );
		return FALSE;
	}
	
	// ���� ���� ������ ���� ������ �����Ѵ�;
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

	// �� �Ҹ�;
	pChar->CheckMoneyUpdate( pChar->GetInvenMoney(), nDecomposeCost, FALSE, "Decompose cost" );
	pChar->SubtractInvenMoneyServer( nDecomposeCost, SUBTRACT_INVEN_MONEY_ITEM_DECOMPOSE );
	pChar->SendInvenMoneyUpdate();

	// �� �α�;
	pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, -nDecomposeCost, ITEM_ROUTE_DELETE, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_DECOMPOSE );
	pChar->LogMoneyExchange( gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, pChar->GetInvenMoney(), MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, ( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_MONEY_CHAR_INVEN_CURRENT );

	// ��� �� �α� ����;
	pChar->LogItemExchange(
		pDECOM_INVENITEM->sItemCustom,
		gamelog::ID_CHAR, pChar->m_CharDbNum,
		gamelog::ID_CHAR, 0,
		ITEM_ROUTE_DELETE, pDECOM_INVENITEM->sItemCustom.wTurnNum, 
		( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_DEL_DECOMPOSE,
		false );

	// ���ص� ������ ó��;
	// ������Ʈ �Ǿ� ��ħO -> ��ħX�� �� ��쵵 ���ؽ� �ϳ��� �����ǰ��ϱ� ����;
	// ī�� �ý������� ���� ���Ǿƴϰ� ����ó���� �þ���. ConsumeInvenItem�� �����Ǿ���;
	if ( pItem->ISPILE() || pDECOM_INVENITEM->sItemCustom.wTurnNum > 1 )
		pChar->ConsumeInvenItem( pNetMsg->wPosX, pNetMsg->wPosY, false, 1, true );
	else
		pChar->InvenDeleteItem( pNetMsg->wPosX, pNetMsg->wPosY, true, FALSE, 1, true );

	// ������ ����;
	int ResultSize = static_cast< int >( vecResult.size() );
	for ( int i = 0; i < ResultSize; ++i )
	{
		if ( vecResult[i].Data.sItemID == SNATIVEID( false ) )
			continue;

		// ������ ����;
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

		// ������ �߻�; ���� üũ;
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

		//	������ ������ Inventory�� ����;
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

		//	[�ڽſ���] �޽��� �߻�;
		pChar->InvenInsertSend( pInsertItem, FALSE, false, true );

		// Ŭ���̾�Ʈ���� ���� ����� ������ֱ� ����;
		msgFB.push_back( pInsertItem->GetNativeID(), pInsertItem->TurnNum() );

		// ��� �� �α� ����: ���ط����� ������ ������ �α�;
		pChar->LogItemExchange( 
			sCustomItemNEW,
			gamelog::ID_NONE, 0,
			gamelog::ID_CHAR, pChar->m_CharDbNum,
			ITEM_ROUTE_DECOMPOSE, sCustomItemNEW.wTurnNum, 
			( pTarChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pTarChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_DECOMPOSE, 
			false );
	}

	// ��� ����;
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