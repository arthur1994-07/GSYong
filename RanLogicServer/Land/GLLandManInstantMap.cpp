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
		// PVE �� �δ��̶��
		// �ƿ� ó���� ���� �ʴ´�.
	}
	// ��Ƽ �δ��� ��� ��Ƽ�� �ٸ��ų� �ƴ� ������� �Ѿ� ����.
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
		// ��Ƽ �δ��� �ƴ� ��쿣 gaeaID�� HOST ID�� �ٸ��� �Ѿ� ����.
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
			// �ð� �ʰ��� �˷���
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
			// ���� �ð����� �޽����� ����
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
		// �ð� �ʰ��Ŀ� �����ִ� ������ ó��
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
	// �� ������(�δ� �ð�ī��Ʈ ��) �̹� �ٸ� ������ �̵����� �� �ִ�;
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

	// �δ��� �ƴϸ� ����
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

	// Gate�� �� ã�������
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

		// ��ǥ ����Ʈ ��������.					//

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
        // �ɸ��� �ǻ츲;
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
        // ����!: RequestInvenRecallThisSvr �Լ� ȣ�� �Ŀ��� �ɸ����� ���� mapID�� �̵��ϴ� mapID�� ����Ǿ����;
        // �׷��� ���� �� ������ pChar->GetCurrentMapID() �� ����ϸ� �ȵȴ�. const MapID sCurMapID(pChar->GetCurrentMap()); �� ����Ͽ��� �Ѵ�.;
        const MapID sCurMapID(pChar->GetCurrentMap());

		BOOL bOK = m_pGLGaeaServer->RequestInvenRecallThisSvr ( pChar, sMoveMapId, dwOutGateID, vPos );
		
		if( !bOK )
		{
			sc::writeLogDebug(sc::string::format( "Instant Map Must Leave Failed. Name[%1%], MapID [%2%][%3%]",
								   pChar->m_szName, sCurMapID.getGaeaMapID().Mid(), sCurMapID.getGaeaMapID().Sid()) );
			return FALSE;	
		}

		// ������ �����̻� ����
		if ( sCurMapID != MapID(sMoveMapId) )
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

		// �� �̵� ������ �˸�.
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
