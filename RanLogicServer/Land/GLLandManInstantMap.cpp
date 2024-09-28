#include "pch.h"
#include "./GLLandMan.h"
#include "../Character/GLChar.h"
#include "../Move/MoveManagerField.h"

#include "../FieldServer/GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLLandMan::CanBeDeleted(float fElapsedTime)
{
	if ( Empty() == FALSE )	
		return FALSE;

	const EMINSTANCE_TYPE emInstanceType(GetInstantType());
	if ( emInstanceType == EMINSTANCE_OLD_INSTANCE )
	{	
		m_fInstantMapDeleteTime += fElapsedTime;
		if(m_fInstantMapDeleteTime >= INSTANTMAP_DELETE_TIME)
			return TRUE;
	}	

	return FALSE;
}

void GLLandMan::ExtraFrameMoveInstantMap( float fElapsedTime )
{
	if( GetInstantType() != SMAPNODE::EMINSTANCE_OLD_INSTANCE )
		return; 

	if(m_GlobPCList.m_dwAmount == 0)
		return;

	typedef std::set<DWORD>		MOVEOUTID;
	typedef MOVEOUTID::iterator MOVEOUTID_ITER;

	MOVEOUTID setReserveMoveOutID;
	
	if (m_sPVEIndunID.IsVaild() )
	{
		// PVE 용 인던이라면
		// 아웃 처리를 하지 않는다.
	}
	// 파티 인던인 경우 파티가 다르거나 아닌 사람들은 쫓아 낸다.
	else if( IsPartyInstantMap() )
	{
		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;

		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar* pChar = pCharNode->Data;

			if( !pChar )
				continue;

			if( pChar->GetPartyID() != GetInstantMapHostID() && pChar->UserLevel() < USER_USER_GM )
			{
				setReserveMoveOutID.insert(pChar->GetGaeaID());
			}
		}
		// 파티 인던이 아닌 경우엔 gaeaID와 HOST ID가 다르면 쫓아 낸다.
	}
	else
	{
		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;

		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar* pChar = pCharNode->Data;

			if( !pChar )
				continue;

			if( pChar->GetGaeaID() != GetInstantMapHostID() && pChar->UserLevel() < USER_USER_GM )
			{
				setReserveMoveOutID.insert(pChar->GetGaeaID());
			}
		}
	}
	
	

	if( GetUseLevelEtcFunc(EMETCFUNC_LIMITTIME) )
	{
		const SLEVEL_ETC_FUNC *pLevelEtcFunc = GetLevelEtcFunc();
		m_fElapsedLimitTime += fElapsedTime;

		if( !m_bExcessiveLimitTime )
		{
			// 시간 초과를 알려줌
			int nLimitMinute = (int)(m_fElapsedLimitTime / 60.0f);

			if( nLimitMinute >= pLevelEtcFunc->m_sLimitTime.nLimitMinute )
			{
				GLMSG::SNETLIMITTIME_OVER NetMsg;
				NetMsg.nRemainTime = 0;

				GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;

				for ( ; pCharNode; pCharNode = pCharNode->pNext )
				{
					GLChar* pChar = pCharNode->Data;

					if( !pChar )
						continue;

					m_pGLGaeaServer->SENDTOCLIENT ( pChar->ClientSlot(), &NetMsg );
				}

				m_bExcessiveLimitTime = TRUE;
				m_fElapsedLimitTime = 0.0f;				
			}
			// 일정 시간마다 메시지를 보냄
			else if( nLimitMinute % pLevelEtcFunc->m_sLimitTime.nTellTermMinute == 0 && m_nLastSendRemainTime != nLimitMinute )
			{				
				GLMSG::SNETLIMITTIME_OVER NetMsg;
				NetMsg.nRemainTime = pLevelEtcFunc->m_sLimitTime.nLimitMinute - nLimitMinute;

				GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;

				for ( ; pCharNode; pCharNode = pCharNode->pNext )
				{
					GLChar* pChar = pCharNode->Data;

					if( !pChar )
						continue;

					m_pGLGaeaServer->SENDTOCLIENT ( pChar->ClientSlot(), &NetMsg );
				}

				m_nLastSendRemainTime = nLimitMinute;
			}			
		}
		// 시간 초과후에 남아있는 유저들 처리
		else if( m_fElapsedLimitTime >= 8.0f )
		{
			GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;

			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				GLChar* pChar = pCharNode->Data;

				if( !pChar )
					continue;

				setReserveMoveOutID.insert( pChar->GetGaeaID() );
			}
		}
	}

	MOVEOUTID_ITER moveOutIDIter = setReserveMoveOutID.begin();

	for( ; moveOutIDIter != setReserveMoveOutID.end(); ++moveOutIDIter )
	{
		GLChar* pCHAR = m_pGLGaeaServer->GetChar ( *moveOutIDIter );

		if( !pCHAR )
			continue;

		MoveOutInstantMap( pCHAR );
	}
	setReserveMoveOutID.clear();
}


BOOL GLLandMan::MoveOutInstantMap(GLChar *pChar)
{
	if (!pChar)
        return E_FAIL;

	//  [5/31/2014 gbgim];
	// 이 시점에(인던 시간카운트 끝) 이미 다른 맵으로 이동했을 수 있다;
	GLLandMan* _pLand(pChar->GetLandMan());
	if ( _pLand != NULL )
	{		
		if ( GetGaeaMapID() != _pLand->GetGaeaMapID() )
			return E_FAIL;
	}
	
	GLLandMan* pInLandMan = NULL;
	DxLandGateMan* pInGateMan = NULL;
	PDXLANDGATE pInGate = NULL;
	D3DXVECTOR3 vPos(0,0,0);

	DxLandGateMan* pOutGateMan = NULL;
	PDXLANDGATE pOutGate = NULL;	


	pOutGateMan = GetLandGateMan();
	
	if( !pOutGateMan )
		return FALSE;

	DWORD dwGateID = 0;
	DWORD dwOutGateID = 0;
	SNATIVEID sMoveMapId;
	SNATIVEID sCurMapID  = GetGaeaMapID();
	SMAPNODE *pMapNode = NULL;

	// 인던이 아니면 리턴
	pMapNode = m_pGLGaeaServer->FindMapNode ( GetBaseMapID() );
	if( pMapNode && !pMapNode->IsInstantMap() )
	{
		sc::writeLogError(sc::string::format("GLLandMan::MoveOutInstantMap() - This is not an instant map(%1%, %2%).", sCurMapID.wMainID, sCurMapID.wSubID));
		return TRUE;
	}

	for( dwGateID = 0; dwGateID < pOutGateMan->GetNumLandGate(); dwGateID++ )
	{
		pOutGate = pOutGateMan->FindLandGate ( dwGateID );
		if( pOutGate ) 
		{
			pInLandMan = m_pGLGaeaServer->GetLand ( pOutGate->GetToMapID() );
			if ( !pInLandMan )		
			{
				pOutGate = NULL;
				continue;
			}

			break;
		}
	}

	// Gate를 못 찾았을경우
	if( !pOutGate )
	{
		sc::writeLogDebug(sc::string::format( "Instant Out Error. Not Find Gate. MapID[%1%][%2%]", sCurMapID.wMainID, sCurMapID.wSubID ) );

		dwOutGateID	= GLCONST_CHAR::dwSTARTGATE[pChar->m_wSchool];
		sMoveMapId  = GLCONST_CHAR::nidSTARTMAP[pChar->m_wSchool];

		pMapNode = m_pGLGaeaServer->FindMapNode ( sMoveMapId );
		
		if ( !pMapNode )
			return FALSE;
	}
	else
	{
		GLLandMan* pInLandMan = NULL;
		DxLandGateMan* pInGateMan = NULL;
		pInLandMan = m_pGLGaeaServer->GetLand ( pOutGate->GetToMapID() );
		
		if(!pInLandMan)
			return FALSE;		

		dwOutGateID = pOutGate->GetToGateID();		

		// 목표 게이트 가져오기.					//

		pInGateMan = pInLandMan->GetLandGateMan ();
		
		if(!pInGateMan)
			return FALSE;
		
		pInGate = pInGateMan->FindLandGate ( dwOutGateID );
		
		if(!pInGate)
			return FALSE;

		sMoveMapId = pInLandMan->GetBaseMapID();

		pMapNode = m_pGLGaeaServer->FindMapNode ( sMoveMapId );
		
		if(!pMapNode)
			return FALSE;

		vPos = pInGate->GetGenPos ( DxLandGate::GEN_RENDUM );	
	}

	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();
	sMoveInfo.sMapID = sMoveMapId;
	sMoveInfo.dwGateID = dwOutGateID;
	sMoveInfo.vPosition = vPos;

	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

    /*//PET
	m_pGLGaeaServer->DropOutPET ( pChar->m_dwPetGUID, true, true );

	//VEHICLE
	m_pGLGaeaServer->SaveVehicle( pChar->ClientSlot(), pChar->GetGaeaID(), true );

	//SUMMON
	m_pGLGaeaServer->DropOutAllSummon( pChar );

    //DIE
    if ( pChar->IsActState(EM_ACT_DIE) )
    {
        // 케릭터 되살림;
        pChar->SetAlive();
    }

	if(pMapNode->GetFieldServerNum() != m_pGLGaeaServer->GetFieldSvrID())
	{
		GLMSG::SNETPC_REQ_MUST_LEAVE_MAP_AG NetMsgAg;
		NetMsgAg.dwChaNum = pChar->CharDbNum();
		NetMsgAg.sMAPID	  = sMoveMapId;
		NetMsgAg.dwGATEID = dwOutGateID;
		NetMsgAg.vPOS	  = vPos;
		m_pGLGaeaServer->SENDTOAGENT(&NetMsgAg);	//pChar->ClientSlot()
	}
	else
	{
        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다. const MapID sCurMapID(pChar->GetCurrentMap()); 를 사용하여야 한다.;
        const MapID sCurMapID(pChar->GetCurrentMap());

		BOOL bOK = m_pGLGaeaServer->RequestInvenRecallThisSvr ( pChar, sMoveMapId, dwOutGateID, vPos );
		
		if( !bOK )
		{
			sc::writeLogDebug(sc::string::format( "Instant Map Must Leave Failed. Name[%1%], MapID [%2%][%3%]",
								   pChar->m_szName, sCurMapID.getGaeaMapID().Mid(), sCurMapID.getGaeaMapID().Sid()) );
			return FALSE;	
		}

		// 버프와 상태이상 제거
		if ( sCurMapID != MapID(sMoveMapId) )
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

		// 멥 이동 성공을 알림.
		//		
		GLMSG::SNETPC_REQ_RECALL_FB NetRecallFB;
		NetRecallFB.dwChaNum = pChar->CharDbNum();
		NetRecallFB.emFB   = EMREQ_RECALL_FB_OK;
		NetRecallFB.bCullByObjectMap = pLand->IsCullByObjectMap();
		NetRecallFB.sMAPID = sMoveMapId;
		NetRecallFB.vPOS   = vPos;
		m_pGLGaeaServer->SENDTOAGENT(&NetRecallFB);	//pChar->ClientSlot()
	}*/

	return TRUE;
}
