#include "../pch.h"
#include "../CaptureTheField/CaptureTheFieldManager.h"
#include "./GLGaeaServer.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void GLGaeaServer::FrameMoveCTF(float fElapsedTime)
{
	m_pCTFMan->FrameMove(fElapsedTime); 
}


BOOL GLGaeaServer::RecallTo ( GLChar *pChar, SNATIVEID &idMap, DWORD dwGateID )
{
	/// 이동정보를 세팅한다;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = pChar->CharDbNum();
	sMoveInfo.sMapID = idMap;
	sMoveInfo.dwGateID = dwGateID;

	/// 이동한다;
	if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo ) )
		return FALSE;

	return TRUE;

	/*//
	//2.Does the map exist?
	//
	if (idMap == SNATIVEID(false))
	{
		sc::writeLogError(std::string("GLGaeaServer::RecallTo - Map does not exist"));
		return FALSE;
	}

	//
	//3.Does MapNode exist?
	//
	const SMAPNODE *pMapNode = FindMapNode(idMap.dwID);

	if( pMapNode == NULL )
	{
		sc::writeLogError(std::string("GLGaeaServer::RecallTo - MapNode does not exist"));
		return FALSE;
	}

	//
	//4.Handle pet, vehicle and summons.
	//

	DropOutPET(pChar->m_dwPetGUID, true, true);
		
	SaveVehicle(pChar->ClientSlot(), pChar->GetGaeaID(), true);
	
	DropOutAllSummon(pChar);	


	//
	//5.Handle Position
	//
	//GLMapAxisInfo sMapAxisInfo;
	//sMapAxisInfo.LoadFile(pMapNode->strFile.c_str());

	//D3DXVECTOR3 vPOS(0,0,0); 
	//sMapAxisInfo.MapPos2MiniPos(pmsg->dwPosX, pmsg->dwPosX, vPOS.x, vPOS.z);


	// 매크로가 켜져있다면 맵이동시 끈다;
	if ( pChar->IsActState(EM_ACT_MACROMODE) )
	{
		pChar->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT ( pChar->ClientSlot(), &msg );
	}
	//
	//6.Request to Recall to AgentServer
	//
		
	if(pMapNode->GetFieldServerNum() != m_dwFieldSvrID)	// move to other FieldServer.
	{
		GLMSG::SNETPC_REQ_RECALL_AG msgRecall;
		msgRecall.dwChaNum	= pChar->CharDbNum();
		msgRecall.dwFieldServerIDTo = pMapNode->GetFieldServerNum();
		msgRecall.mapIDTo	= idMap;
		msgRecall.dwGATEID	= dwGateID;	//UINT_MAX;
		msgRecall.vPosition	= D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX); //vPOS;
		SENDTOAGENT(&msgRecall);
	}	
	else										// move to same FieldServer
	{
		GLMSG::SNETPC_REQ_RECALL_FB msgRecall;

        // 주의!: RequestInvenRecallThisSvr 함수 호출 후에는 케릭터의 현재 mapID가 이동하는 mapID로 변경되어버림;
        // 그래서 현재 맵 정보는 pChar->GetCurrentMapID() 를 사용하면 안된다. const MapID sCurMapID(pChar->GetCurrentMap()); 를 사용하여야 한다.;
        const MapID sCurMapID(pChar->GetCurrentMap());

		BOOL bOK = RequestInvenRecallThisSvr(pChar, idMap, dwGateID, D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX));

		if ( bOK == FALSE )
		{
			sc::writeLogError(sc::string::format("GLGaeaServer::RecallTo() - Char(%1%) failed to recall to same fieldsvr.", pChar->CharDbNum()));
			return FALSE;
		}

		const GLLandMan* const pLand(pChar->GetLandMan());
		if ( pLand == NULL )
		{
            sc::writeLogError(sc::string::format("GLGaeaServer::RecallTo() - pLand is NULL, Char(%1%) %2%/%3%, %4%/%5%.",
                pChar->CharDbNum(), sCurMapID.getBaseMapID().Mid(), sCurMapID.getBaseMapID().Sid(), sCurMapID.getGaeaMapID().Mid(), sCurMapID.getGaeaMapID().Sid()));
			return FALSE;
		}
		//
		//Reset all Buffs and DeBuffs.
		//
		if ( sCurMapID != MapID(idMap) )
		{
			for(int i=0; i<EMBLOW_MULTI; ++i)
				pChar->DISABLEBLOW(i);

			for(int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
				pChar->RESETSKEFF(i);

			// CP 초기화 맵인지 확인
			if (pMapNode->IsCombatPointReset())
				pChar->ResetCP();
		}

		pChar->ResetAction();	

		//
		// Notify that moved to Map successfully.
		//
		msgRecall.dwChaNum = pChar->CharDbNum();
		msgRecall.emFB	= EMREQ_RECALL_FB_OK;
		msgRecall.bCullByObjectMap = pLand->IsCullByObjectMap();
		msgRecall.sMAPID= idMap;
		msgRecall.vPOS	= pChar->GetPosition();
		SENDTOAGENT(&msgRecall);
	}

	return TRUE;*/
}

BOOL GLGaeaServer::IsCTFPvPMap(const SNATIVEID& mapID)
{
	if ( m_pCTFMan == NULL )
	{
		sc::writeLogError(std::string("GLGaeaServer::IsCTFPvPMap() - m_pCTFMan is NULL."));
		
		return FALSE;
	}

	return (m_pCTFMan->GetPvPMapID() == mapID.Id());
}


BOOL GLGaeaServer::IsCTFPvEMap(const SNATIVEID& mapID)
{
	if ( m_pCTFMan == NULL )
	{
		sc::writeLogError(std::string("GLGaeaServer::IsCTFPvEMap() - m_pCTFMan is NULL."));
		
		return FALSE;
	}

	return (m_pCTFMan->GetPvEMapID() == mapID.Id());
}


BOOL GLGaeaServer::IsCTFPlaying()
{
	return (GetCTFState() == CTF_STATE_START);
}

BOOL GLGaeaServer::IsCTFRewarding()
{
	return (GetCTFState() == CTF_STATE_DONE || GetCTFState() == CTF_STATE_REWARD);
}

BOOL GLGaeaServer::IsCTFStop()
{
	return ( GetCTFState() == CTF_STATE_STOP );
}

CTime GLGaeaServer::GetCTFReadyTime()
{
	CTime ctReturn( CTime::GetCurrentTime() );
	if ( m_pCTFMan )
	{
		ctReturn = m_pCTFMan->GetReadyTime();
	}
	return ctReturn;
}

/*
BOOL GLGaeaServer::NotifyCTFStart(GLChar *pChar)
{
	if (!m_pCTFMan)
	{
		sc::writeLogError(std::string("GLGaeaServer::NotifyCTFStart() - m_pCTFMan is NULL."));
		
		return FALSE;
	}
	
	m_pCTFMan->NotifyStart(pChar);	
}
*/


ENUM_CTF_STATE GLGaeaServer::GetCTFState()
{
	if (!m_pCTFMan)
	{
		sc::writeLogError(std::string("GLGaeaServer::GetCTFState() - m_pCTFMan is NULL."));

		return CTF_STATE_STOP;
	}

	return m_pCTFMan->GetState();
}


DWORD GLGaeaServer::GetCTFRebirthTime(WORD wSchool)
{
	if (!m_pCTFMan)
	{
		sc::writeLogError(std::string("GLGaeaServer::GetCTFRebirthTime() - m_pCTFMan is NULL."));
		
		return 0;
	}

	return (m_pCTFMan->GetRebirthTimeSec(wSchool));
}


void GLGaeaServer::AddCTFRewardBuff(SNATIVEID id, WORD wLevel)
{
    if (!m_pCTFMan)
    {
        sc::writeLogError(std::string("GLGaeaServer::AddCTFRewardBuff() - m_pCTFMan is NULL."));

        return;
    }

    m_pCTFMan->AddRewardBuff(id, wLevel);
}

void GLGaeaServer::GetCTFRewardBuff(UINT idx, OUT CTF_REWARD_BUFF &rewardBuff)
{
    if (!m_pCTFMan)
    {
        sc::writeLogError(std::string("GLGaeaServer::GetCTFRewardBuff() - m_pCTFMan is NULL."));

        return;
    }

    m_pCTFMan->GetRewardBuff(idx, rewardBuff);
}


UINT GLGaeaServer::GetCTFRewardBuffSize()
{
    if (!m_pCTFMan)
    {
        sc::writeLogError(std::string("GLGaeaServer::GetCTFRewardBuffSize() - m_pCTFMan is NULL."));

        return 0;
    }

    return m_pCTFMan->GetRewardBuffSize();
}


/*
//
// Replace the level-file of the idMap to strLevelFile.
//
BOOL GLGaeaServer::ReplaceMapLevel(DWORD dwMapID, std::string &strLevelFile)
{
	SNATIVEID idMap(dwMapID);

	if (idMap == SNATIVEID(false) )
	{
		sc::writeLogError(std::string("GLGaeaServer::ReplaceLevel() - idMap is NULL."));
		return FALSE;
	}

	GLLandMan *pLandMan = GetLand(idMap);
	if(!pLandMan)
	{
		sc::writeLogError(sc::string::format("GLGaeaServer::ReplaceLevel() - LandMan does not exist for the idMap(%1%, %2%).", idMap.wMainID, idMap.wSubID));
		return FALSE;
	}
	

	//
	//prevent to regen mobs.
	//
	pLandMan->LoadMobSchManEx(strLevelFile.c_str());
	//sc::writeLogInfo(sc::string::format("GLGaeaServer::ReplaceLevel(%1%) - DONE for the idMap(%2%, %3%).", strLevelFile, idMap.wMainID, idMap.wSubID));


	//
	//delete existing mobs in this map.
	//	
	for (int i=0; i<MAXCROW; i++ )
		pLandMan->DropOutCrow(i);	

	return TRUE;
}
*/

