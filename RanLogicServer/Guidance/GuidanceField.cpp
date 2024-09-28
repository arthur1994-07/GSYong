#include "pch.h"
#include <algorithm>

#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Msg/WorldBattleMsg.h"

#include "../Club/GLClubField.h"
#include "../Database/DbManager.h"
#include "../Database/DBAction/DbActionLog.h"
#include "../Database/DBAction/DbActionGameClub.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./GuidanceField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const float fLOG_TIME = 300.0f;
const int MAX_LAND_EFFECT = 10;

GLGuidanceFieldMan::GLGuidanceFieldMan(GLGaeaServer* pServer)
    : m_pServer(pServer)
{
}

GLGuidanceFieldMan::~GLGuidanceFieldMan()
{
}

bool GLGuidanceFieldMan::SetMapState ()
{
//	GLLANDMANLIST& cLandList = m_pServer->GetLandManList();

	/*GLLANDMANNODE* pLandManNode = cLandList.m_pHead;
	for ( ; pLandManNode; pLandManNode = pLandManNode->pNext )
	{
		PGLLANDMAN pLand = pLandManNode->Data;

		pLand->m_bGuidBattleMap = false;
		pLand->m_bGuidBattleMapHall = false;
		
		SMAPNODE *pMAPNODE = m_pServer->FindMapNode ( pLand->GetMapID() );
		if ( pMAPNODE && pMAPNODE->bCommission )
		{
			pLand->m_fCommissionRate = GLCONST_CHAR::fDEFAULT_COMMISSION;
		}
		else
		{
			pLand->m_fCommissionRate = 0.0f;
		}
	}*/

	for ( GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i )
	{
		GLGuidance &sGuidance = m_vecGuidance[i];

		// 선도 클럽이 관리하는 멥.
		for ( std::vector<DWORD>::size_type m=0; m<sGuidance.m_vecMaps.size(); ++m )
		{
			SNATIVEID nidMAP ( sGuidance.m_vecMaps[m] );

			GLLandMan *pLandMan = m_pServer->GetLand ( nidMAP );
			if ( !pLandMan )
                continue;

			pLandMan->m_dwClubMapID = sGuidance.m_dwID;
			pLandMan->m_pGuidance = Find ( sGuidance.m_dwID );

			pLandMan->m_dwGuidClubID = sGuidance.m_dwGuidanceClub;
			pLandMan->SetCommissionRate( sGuidance.m_fCommissionPercent );
		}

		// 선도 클럽 전투 입구존.
		SNATIVEID nidHallMAP ( sGuidance.m_dwClubHallMap );
		GLLandMan *pLandMan = m_pServer->GetLand ( nidHallMAP );
		if ( pLandMan )
		{
			pLandMan->m_dwClubMapID = sGuidance.m_dwID;
			pLandMan->m_pGuidance = Find(sGuidance.m_dwID);

			pLandMan->m_bGuidBattleMapHall = true;
			pLandMan->m_dwGuidClubID = sGuidance.m_dwGuidanceClub;
			pLandMan->SetCommissionRate( sGuidance.m_fCommissionPercent );
		}

		// 선도 클럽 전투존.
		SNATIVEID nidMAP ( sGuidance.m_dwClubMap );
		SMAPNODE *pMAPNODE = m_pServer->FindMapNode ( nidMAP );
		if ( pMAPNODE )
		{
			pMAPNODE->SetLeaderClubZone(true);
		}

		pLandMan = m_pServer->GetLand ( nidMAP );
		if ( pLandMan )
		{
			pLandMan->m_dwClubMapID = sGuidance.m_dwID;
			pLandMan->m_pGuidance = Find(sGuidance.m_dwID);

			pLandMan->m_bGuidBattleMap = true;
			pLandMan->m_dwGuidClubID = sGuidance.m_dwGuidanceClub;
			pLandMan->SetCommissionRate( sGuidance.m_fCommissionPercent );
		}
	}

	return true;
}

bool GLGuidanceFieldMan::BeginBattle ( DWORD dwID )
{
	GLGuidance *pGuid = Find ( dwID );
	if ( !pGuid )	return false;

	pGuid->m_bBattle = true;

	// 멥 설정 변경.

	pGuid->m_fLog_TIMER = 0.0f;
	pGuid->m_mapKillLog.clear();

	pGuid->m_fHoldCertifyTime = 0.0f;
	pGuid->m_nMapEffectIndex = -1;

	return true;
}

bool GLGuidanceFieldMan::EndBattle(DWORD dwID)
{
	GLGuidance *pGuid = Find(dwID);
	if (!pGuid)
        return false;

	pGuid->m_bBattle = false;

	// 체크시작
	pGuid->m_bBattleEndCheck = true;
	pGuid->m_fCHECK_TIME = 10.0f;
	pGuid->m_fHoldCertifyTime = 0.0f;
	pGuid->m_nMapEffectIndex = -1;

	//	마지막 로그 출력
	//	5분마다 출력인데 2분 30초 이상 지났다면 마지막 출력이 
	//	스킵된것으로 보고 출력한다.
	if (pGuid->m_fLog_TIMER > (fLOG_TIME / 2.0f)) 
	{
		DoLogPrint(dwID);
		pGuid->m_fLog_TIMER = 0.0f;
	}

	pGuid->m_mapKillLog.clear();

	GLLandMan *pLandMan = m_pServer->GetLand(pGuid->m_dwClubMap);
	if (pLandMan)
	{
		pLandMan->DoGateOutPCAll(pGuid->m_dwGuidanceClub, pGuid->m_dwCLubMapGate);

		// 보상 몹 소환.
		for (size_t n=0; n < pGuid->m_vecAwardMob.size(); ++n)
		{
			pLandMan->DropCrow(
                pGuid->m_vecAwardMob[n].nidGenMob, 
                pGuid->m_vecAwardMob[n].wGenPosX, 
                pGuid->m_vecAwardMob[n].wGenPosY);
		}

        const char* MapName = m_pServer->GetMapName(pGuid->m_dwClubMap);
        if (MapName)
        {            
            std::string LogMsg(
                sc::string::format(
                    ID2GAMEINTEXT("CLUB_LOG_GUIDANCE"),
                    MapName));
            m_pServer->SENDCLUBLOG(pGuid->m_dwGuidanceClub, club::LOG_GUIDANCE, LogMsg);
        }
	}
	return true;
}

bool GLGuidanceFieldMan::ChangeGuidClub(DWORD dwID, DWORD dwClubID)
{
	GLGuidance *pGuid = Find(dwID);
	if (!pGuid)
        return false;

	pGuid->m_dwGuidanceClub = dwClubID;
	pGuid->m_fCommissionPercent = pGuid->m_fDefaultCommissionPer;

	// 선도 클럽이 관리하는 멥.
	for (std::vector<DWORD>::size_type m=0; m<pGuid->m_vecMaps.size(); ++m)
	{
		SNATIVEID nidMAP(pGuid->m_vecMaps[m]);
		GLLandMan* pLandMan = m_pServer->GetLand(nidMAP);
		if (!pLandMan)
            continue;

		pLandMan->m_dwGuidClubID = pGuid->m_dwGuidanceClub;
		pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );
	}

	// 선도 클럽 전투 입구존.
	SNATIVEID nidHallMAP(pGuid->m_dwClubHallMap);
	GLLandMan* pLandMan = m_pServer->GetLand(nidHallMAP);
	if (pLandMan)
	{
		pLandMan->m_dwGuidClubID = pGuid->m_dwGuidanceClub;
		pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );
	}

	// 선도 클럽 전투존.
	SNATIVEID nidMAP(pGuid->m_dwClubMap);
	pLandMan = m_pServer->GetLand(nidMAP);
	if (pLandMan)
	{
		pLandMan->m_dwGuidClubID = pGuid->m_dwGuidanceClub;
		pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );
	}

	return true;
}

bool GLGuidanceFieldMan::ChangeCommission ( DWORD dwID, float fRate )
{
	GLGuidance *pGuid = Find(dwID);
	if (!pGuid)
        return false;

	if (0 > fRate)
        fRate = 0.0f;
	if (GLCONST_CHAR::fMAX_COMMISSION < fRate)
        fRate = GLCONST_CHAR::fMAX_COMMISSION;

	pGuid->m_fCommissionPercent = fRate;

	// 선도 클럽이 관리하는 멥.
	for ( std::vector<DWORD>::size_type m=0; m<pGuid->m_vecMaps.size(); ++m )
	{
		SNATIVEID nidMAP ( pGuid->m_vecMaps[m] );
		GLLandMan *pLandMan = m_pServer->GetLand(nidMAP);
		if (!pLandMan)
            continue;
		
		 pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );
	}

	// 선도 클럽 전투 입구존.
	SNATIVEID nidHallMAP ( pGuid->m_dwClubHallMap );
	GLLandMan *pLandMan = m_pServer->GetLand(nidHallMAP);
	if ( pLandMan )
		pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );

	// 선도 클럽 전투존.
	SNATIVEID nidMAP ( pGuid->m_dwClubMap );
	pLandMan = m_pServer->GetLand(nidMAP);
	if ( pLandMan )
		pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );

	return true;
}

bool GLGuidanceFieldMan::DoCertify ( DWORD dwID, DWORD dwCHARID, DWORD dwNpcGlobID, const D3DXVECTOR3 &vPOS )
{
	GLGuidance *pGuid = Find ( dwID );
	if ( !pGuid )	return false;

	pGuid->m_dwCERTIFY_CHARID = dwCHARID;
	pGuid->m_dwCERTIFY_NPCID = dwNpcGlobID;
	pGuid->m_fCERTIFY_TIMER = 0.0f;
	pGuid->m_vCERTIFY_POS = vPOS;

	return true;
}

EMCHECKCERTIFY GLGuidanceFieldMan::CheckCertify ( DWORD dwID, DWORD dwCHARID )
{
	GLGuidance *pGuid = Find ( dwID );
	if ( !pGuid )				return EMCHECKCERTIFY_FAIL;

	if ( pGuid->IsCertify() )
	{
		if ( pGuid->m_dwCERTIFY_CHARID==dwCHARID )	return EMCHECKCERTIFY_DOING;
		else										return EMCHECKCERTIFY_OTHERDOING;
	}

	if ( !pGuid->m_bBattle )	return EMCHECKCERTIFY_NOTBATTLE;

	return EMCHECKCERTIFY_OK;
}

std::string GLGuidanceFieldMan::GetName(DWORD dwID)
{
	GLGuidance* pGuid = Find(dwID);
	if (pGuid)
        return pGuid->GetName();        
    else
        return std::string("");
}

DWORD GLGuidanceFieldMan::GetGuidID(DWORD dwClubID) const
{
	for (GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i)
	{
		const GLGuidance& sGuidance = m_vecGuidance[i];
		if (sGuidance.m_dwGuidanceClub == dwClubID)
            return sGuidance.m_dwID;
	}
	return UINT_MAX;
}

bool GLGuidanceFieldMan::FrameMove(float fElaps)
{
	if (m_pServer->IsInstance())
		return false;

	for ( GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i )
	{
		GLGuidance &sGuidance = m_vecGuidance[i];

		sGuidance.m_fCERTIFY_TIMER += fElaps;
		
		if ( sGuidance.IsBattle() ) 
		{
			sGuidance.m_fLog_TIMER += fElaps;	

			if ( sGuidance.m_fLog_TIMER > fLOG_TIME ) 
			{
				//	Log출력
				DoLogPrint( sGuidance.m_dwID );
				sGuidance.m_fLog_TIMER -= fLOG_TIME;
			}

			sGuidance.CheckMapEffect(m_pServer, fElaps);
		}

		if ( sGuidance.IsCertify() )
		{
			// 인증도중 게임 종료
			GLChar* pCdChar = m_pServer->GetCharByDbNum(sGuidance.m_dwCERTIFY_CHARID);
			if (!pCdChar)
			{
				sGuidance.DoCertifyEnd();
				continue;
			}

			// 일반 오류..
			GLLandMan* const pLand(pCdChar->GetLandMan());
			if ( pLand == NULL )
			{
				sGuidance.DoCertifyEnd();
				continue;
			}

			// 일반 오류 ( 인증도중 클럽을 탈퇴할 경우 )
			if (pCdChar->m_ClubDbNum == CLUB_NULL)
			{
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_FAIL;
				m_pServer->SENDTOCLIENT(pCdChar->ClientSlot(), &NetMsgFb);
				sGuidance.DoCertifyEnd();
				continue;
			}

			// 장소이동 카드를 이용한 경우(친카, 귀환카드류)
			if (pCdChar->GetCurrentMap().getGaeaMapID().Id() != sGuidance.m_dwClubMap)
			{
				// 인증도중 장소 이탈.
				// 자기자신에게
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_ING_DISTANCE;
				m_pServer->SENDTOCLIENT ( pCdChar->ClientSlot(), &NetMsgFb );

				// 모두에게
				GLMSG::NET_CLUB_CD_CERTIFY_ING_BRD NetMsgBrd;
				NetMsgBrd.emFB = EMCDCERTIFY_ING_DISTANCE;
				std::string strZone = GetName ( pLand->m_dwClubMapID );
				std::tr1::shared_ptr<GLClubField> spClub = m_pServer->GetClub ( pCdChar->m_ClubDbNum );
				if ( spClub->IsAlliance() )
				{
					std::tr1::shared_ptr<GLClubField> pCHIEFCLUB = m_pServer->GetClub ( spClub->m_dwAlliance );
					NetMsgBrd.SetAlliance(pCHIEFCLUB->Name());
				}
					
				NetMsgBrd.SetZone(strZone.c_str());
				NetMsgBrd.SetClubName(spClub->Name());
				NetMsgBrd.SetName(pCdChar->m_szName);

                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, NetMsgBrd);

				m_pServer->SENDTOAGENT(NET_MSG_CLUB_CD_CERTIFY_ING_BRD, SendBuffer);

				sGuidance.DoCertifyEnd();
				continue;
			}
			
			// 인증도중 사망
			if ( !pCdChar->IsValidBody() )
			{
				// 자기자신에게
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_ING_DIE;
				m_pServer->SENDTOCLIENT ( pCdChar->ClientSlot(), &NetMsgFb );

				// 모두에게

				GLMSG::NET_CLUB_CD_CERTIFY_ING_BRD NetMsgBrd;
				NetMsgBrd.emFB = EMCDCERTIFY_ING_DIE;
				std::string strZone = GetName ( pLand->m_dwClubMapID );
				std::tr1::shared_ptr<GLClubField> pCLUB = m_pServer->GetClub ( pCdChar->m_ClubDbNum );
				if ( pCLUB->IsAlliance() )
				{
					std::tr1::shared_ptr<GLClubField> pCHIEFCLUB = m_pServer->GetClub ( pCLUB->m_dwAlliance );
					NetMsgBrd.SetAlliance(pCHIEFCLUB->Name());
				}
					
				NetMsgBrd.SetZone(strZone.c_str());
				NetMsgBrd.SetClubName(pCLUB->Name());
				NetMsgBrd.SetName(pCdChar->m_szName);

                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, NetMsgBrd);

				m_pServer->SENDTOAGENT(NET_MSG_CLUB_CD_CERTIFY_ING_BRD, SendBuffer);

				sGuidance.DoCertifyEnd();
				continue;
			}

			PGLCROW pCROW = pLand->GetCrow( sGuidance.m_dwCERTIFY_NPCID );

			D3DXVECTOR3 vDist1 = pCROW->GetPosition() - pCdChar->GetPosition();
			float fDist1 = D3DXVec3Length ( &vDist1 );

			D3DXVECTOR3 vDist2 = pCdChar->GetPosition() - sGuidance.m_vCERTIFY_POS;
			float fDist2 = D3DXVec3Length ( &vDist2 );

			if ( fDist1 > GLCONST_CHAR::fCDCERTIFY_DIST || fDist2 > GLCONST_CHAR::fCDCERTIFY_DIST2 )
			{
				// 인증도중 장소 이탈.
				// 자기자신에게
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_ING_DISTANCE;
				m_pServer->SENDTOCLIENT ( pCdChar->ClientSlot(), &NetMsgFb );

				// 모두에게
				GLMSG::NET_CLUB_CD_CERTIFY_ING_BRD NetMsgBrd;
				NetMsgBrd.emFB = EMCDCERTIFY_ING_DISTANCE;
				std::string strZone = GetName(pLand->m_dwClubMapID);
				std::tr1::shared_ptr<GLClubField> pCLUB = m_pServer->GetClub(pCdChar->m_ClubDbNum);
				if (pCLUB->IsAlliance())
				{
					std::tr1::shared_ptr<GLClubField> pCHIEFCLUB = m_pServer->GetClub ( pCLUB->m_dwAlliance );
					NetMsgBrd.SetAlliance(pCHIEFCLUB->Name());
				}
					
				NetMsgBrd.SetZone(strZone.c_str());
				NetMsgBrd.SetClubName(pCLUB->Name());
				NetMsgBrd.SetName(pCdChar->m_szName);

                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, NetMsgBrd);

				m_pServer->SENDTOAGENT(NET_MSG_CLUB_CD_CERTIFY_ING_BRD, SendBuffer);

				sGuidance.DoCertifyEnd();
				continue;
			}

			if (!sGuidance.m_bBattle)
			{
				// 인증도중 시간 초과.
				// 자기자신에게
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_ING_TIMEOUT;
				m_pServer->SENDTOCLIENT(pCdChar->ClientSlot(), &NetMsgFb);

				// 모두에게
				GLMSG::NET_CLUB_CD_CERTIFY_ING_BRD NetMsgBrd;
				NetMsgBrd.emFB = EMCDCERTIFY_ING_TIMEOUT;
				std::string strZone = GetName(pLand->m_dwClubMapID);
				std::tr1::shared_ptr<GLClubField> pCLUB = m_pServer->GetClub(pCdChar->m_ClubDbNum);
				if (pCLUB->IsAlliance())
				{
					std::tr1::shared_ptr<GLClubField> pCHIEFCLUB = m_pServer->GetClub ( pCLUB->m_dwAlliance );
					NetMsgBrd.SetAlliance(pCHIEFCLUB->Name());
				}
					
				NetMsgBrd.SetZone(strZone.c_str());
				NetMsgBrd.SetClubName(pCLUB->Name());
				NetMsgBrd.SetName(pCdChar->m_szName);

                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, NetMsgBrd);

				m_pServer->SENDTOAGENT(NET_MSG_CLUB_CD_CERTIFY_ING_BRD, SendBuffer);

				sGuidance.DoCertifyEnd();
				continue;
			}

			if (sGuidance.m_fCERTIFY_TIMER >= GLCONST_CHAR::fCDCERTIFY_TIME)
			{
				// 인증 완료.
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFB;
				NetMsgFB.emFB = EMCDCERTIFY_COMPLETE;
				m_pServer->SENDTOCLIENT(pCdChar->ClientSlot(), &NetMsgFB);

				// 에이젼트에 알림.
				GLMSG::NET_CLUB_CERTIFIED_FA NetMsg;
				NetMsg.m_GuidanceId = sGuidance.GetId();
				NetMsg.m_ClubDbNum  = pCdChar->GetClubDbNum();
				m_pServer->SENDTOAGENT(&NetMsg);

                // Club log
                // [%1%]님이 [%2%]지역의 선도클럽 결정전에서 인증에 성공하였습니다.
                std::string ClubLog(
                    sc::string::format(
                        ID2GAMEINTEXT("CLUB_LOG_24"),
                        pCdChar->GetName(),
                        GetName(sGuidance.GetId())));
                m_pServer->SENDCLUBLOG(pCdChar->GetClubDbNum(), club::LOG_GUIDANCE, ClubLog);

				// 맵 정리
                // 인증을한 클럽의 소속캐릭터가 아니면 모두 맵 밖으로 튕겨낸다
				GLLandMan* pLandMan = m_pServer->GetLand(sGuidance.m_dwClubMap);
				if (pLandMan)                    
					pLandMan->DoGateOutPCAll(pCdChar->m_ClubDbNum, sGuidance.m_dwCLubMapGate);

				sGuidance.DoCertifyEnd();
				sGuidance.m_fHoldCertifyTime = 0.0f;
				sGuidance.m_nMapEffectIndex = -1;
			}
		}
	}

	return true;
}

void GLGuidanceFieldMan::CheckExtraGuild(float fElaps)
{
	for (GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i)
	{
		GLGuidance &sGuidance = m_vecGuidance[i];
		if (sGuidance.m_bBattleEndCheck)
			sGuidance.CheckExtraGuild(m_pServer, fElaps);
	}
}

void GLGuidanceFieldMan::DoLogPrint(DWORD dwID)
{
	GLGuidance* pGuid = Find(dwID);
	if (!pGuid)
        return;
	if (!pGuid->IsBattle())
        return;
	if (!pGuid->m_bLog)
        return;
	
	GLLandMan *pLandMan = m_pServer->GetLand(pGuid->m_dwClubMap);
	if (!pLandMan)
        return;

//////////
	PROFILE_BEGIN( "GUID_LOG_PRINT" );
//////////

	SNATIVEID sMapID = pGuid->m_dwClubMap;

	// 동맹은 3개 이상설정시 수정해야 한다. 문서에 3개만 해놔서 지금 당장 머 어떻게 작업하기도 애매하다. 
	CTime cCurTime = CTime::GetCurrentTime();
    CString strTime = cCurTime.Format("%Y-%m-%d %H:%M:%S");	

	GLCHARNODE* pCharNode = pLandMan->m_GlobPCList.m_pHead;
	for ( ; pCharNode; pCharNode = pCharNode->pNext)
	{
		GLChar* pChar = pCharNode->Data;
		if (!pChar)
            continue;

		LOG_GUIDANCE sLog;
		
		sLog.nCertify = ( pGuid->m_dwGuidanceClub == pChar->m_ClubDbNum ) ? 1 : 0;

		//	동맹 처리관련해서 FieldServer에서는 마스터 클럽만 리스트를 가지고 있다....		
        std::tr1::shared_ptr<GLClubField> pClub = m_pServer->GetClub( pChar->m_ClubDbNum );
		if (pClub && pClub->IsAlliance()) 
		{			
			int nCount = 0;

			std::tr1::shared_ptr<GLClubField> pClub_Chief = m_pServer->GetClub( pClub->m_dwAlliance );
			if ( pClub_Chief ) 
			{
				CLUB_ALLIANCE_ITER pos = pClub_Chief->m_setAlliance.begin();
				CLUB_ALLIANCE_ITER end = pClub_Chief->m_setAlliance.end();
				for ( ; pos!=end; ++pos)
				{
					const GLCLUBALLIANCE& sALLIANCE = pos->second;
					if (nCount == MAX_ALLIY)
                        break;
					if (sALLIANCE.m_dwID == pClub->m_DbNum)
                        continue;
					
					sLog.dwAlliance[nCount] = sALLIANCE.m_dwID;
					++nCount;
				}
			}
		}

		
		GUID_KILL_LOG_MAP_ITER pos = pGuid->m_mapKillLog.find( pChar->m_CharDbNum );
		if ( pos != pGuid->m_mapKillLog.end() ) 
		{
			const SGUID_KILL_LOG& sKillLog = pos->second;
			sLog.wKill = sKillLog.wKill;
			sLog.wDeath = sKillLog.wDeath;
		}	

		sLog.wMapMID = sMapID.wMainID;
		sLog.wMapSID = sMapID.wSubID;
		sLog.dwClubID = pChar->m_ClubDbNum;
		sLog.dwAllianceID = pClub->m_dwAlliance;
		sLog.dwCharID = pChar->m_CharDbNum;
		sLog.dwClass = (DWORD)CharClassToClassIndex( pChar->m_emClass );
		sLog.wLevel = pChar->m_wLevel;
		sLog.strTime = strTime;

        m_pServer->AddLogAdoJob(
            db::DbActionPtr(
                new db::LogGuidance(sLog)));
	}

	PROFILE_END( "GUID_LOG_PRINT" );
}

void GLGuidanceFieldMan::SendWorldBattleNationFlag( GLChar* pChar )
{
	// 관리 맵들의 국기를 보내준다.
	for ( GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i )
	{
		GLGuidance &sGuidance = m_vecGuidance[i];
		std::tr1::shared_ptr<GLClubField> spGuidClub = m_pServer->GetClub ( sGuidance.m_dwGuidanceClub );
		if (spGuidClub)
		{
			wb::SNET_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD sFlagNetMsgBrd;
			sFlagNetMsgBrd.dwGuidanceID = sGuidance.m_dwID;
			sFlagNetMsgBrd.dwCountryCode = spGuidClub->GetCountry();
			m_pServer->SENDTOCLIENT(pChar->ClientSlot(), &sFlagNetMsgBrd);
		}
	}

	// 현재 유저가 있는 지역의 국기를 보내준다.
	for ( GUIDANCE_VEC::size_type i=0; i<m_vecGuidance.size(); ++i )
	{
		GLGuidance &sGuidance = m_vecGuidance[i];

		std::vector<DWORD>::size_type nSize = sGuidance.m_vecMaps.size();
		for ( std::vector<DWORD>::size_type m=0; m<nSize; ++m )
		{
			SNATIVEID nidMAP = (sGuidance.m_vecMaps[m]);
			if ( pChar->GetCurrentMap().getGaeaMapID() == nidMAP )
			{
				std::tr1::shared_ptr<GLClubField> spGuidClub = m_pServer->GetClub ( sGuidance.m_dwGuidanceClub );
				if (spGuidClub)
				{
					wb::SNET_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD sFlagNetMsgBrd;
					sFlagNetMsgBrd.dwGuidanceID = -1;			// GLogicData::GetInstance().wbGetGuidanceID() 과 값을 비교한다.
																// -1 이라는 것은 현재 맵의 선도 구역을 점령한 클럽의 국기를 표시하도록 한다.
																// 예를들어 적용하고 싶은 Effect가 wbCountry.lua -> wbEffectGuidanceID 에서 셋팅할 때 
																//  { -1, L"flag.egp" }, 방법으로 셋팅을 한다.
					sFlagNetMsgBrd.dwCountryCode = spGuidClub->GetCountry();
					m_pServer->SENDTOCLIENT ( pChar->ClientSlot(), &sFlagNetMsgBrd );
				}
				return;
			}
		}
	}
}
