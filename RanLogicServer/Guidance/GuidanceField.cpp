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

		// ���� Ŭ���� �����ϴ� ��.
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

		// ���� Ŭ�� ���� �Ա���.
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

		// ���� Ŭ�� ������.
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

	// �� ���� ����.

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

	// üũ����
	pGuid->m_bBattleEndCheck = true;
	pGuid->m_fCHECK_TIME = 10.0f;
	pGuid->m_fHoldCertifyTime = 0.0f;
	pGuid->m_nMapEffectIndex = -1;

	//	������ �α� ���
	//	5�и��� ����ε� 2�� 30�� �̻� �����ٸ� ������ ����� 
	//	��ŵ�Ȱ����� ���� ����Ѵ�.
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

		// ���� �� ��ȯ.
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

	// ���� Ŭ���� �����ϴ� ��.
	for (std::vector<DWORD>::size_type m=0; m<pGuid->m_vecMaps.size(); ++m)
	{
		SNATIVEID nidMAP(pGuid->m_vecMaps[m]);
		GLLandMan* pLandMan = m_pServer->GetLand(nidMAP);
		if (!pLandMan)
            continue;

		pLandMan->m_dwGuidClubID = pGuid->m_dwGuidanceClub;
		pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );
	}

	// ���� Ŭ�� ���� �Ա���.
	SNATIVEID nidHallMAP(pGuid->m_dwClubHallMap);
	GLLandMan* pLandMan = m_pServer->GetLand(nidHallMAP);
	if (pLandMan)
	{
		pLandMan->m_dwGuidClubID = pGuid->m_dwGuidanceClub;
		pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );
	}

	// ���� Ŭ�� ������.
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

	// ���� Ŭ���� �����ϴ� ��.
	for ( std::vector<DWORD>::size_type m=0; m<pGuid->m_vecMaps.size(); ++m )
	{
		SNATIVEID nidMAP ( pGuid->m_vecMaps[m] );
		GLLandMan *pLandMan = m_pServer->GetLand(nidMAP);
		if (!pLandMan)
            continue;
		
		 pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );
	}

	// ���� Ŭ�� ���� �Ա���.
	SNATIVEID nidHallMAP ( pGuid->m_dwClubHallMap );
	GLLandMan *pLandMan = m_pServer->GetLand(nidHallMAP);
	if ( pLandMan )
		pLandMan->SetCommissionRate( pGuid->m_fCommissionPercent );

	// ���� Ŭ�� ������.
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
				//	Log���
				DoLogPrint( sGuidance.m_dwID );
				sGuidance.m_fLog_TIMER -= fLOG_TIME;
			}

			sGuidance.CheckMapEffect(m_pServer, fElaps);
		}

		if ( sGuidance.IsCertify() )
		{
			// �������� ���� ����
			GLChar* pCdChar = m_pServer->GetCharByDbNum(sGuidance.m_dwCERTIFY_CHARID);
			if (!pCdChar)
			{
				sGuidance.DoCertifyEnd();
				continue;
			}

			// �Ϲ� ����..
			GLLandMan* const pLand(pCdChar->GetLandMan());
			if ( pLand == NULL )
			{
				sGuidance.DoCertifyEnd();
				continue;
			}

			// �Ϲ� ���� ( �������� Ŭ���� Ż���� ��� )
			if (pCdChar->m_ClubDbNum == CLUB_NULL)
			{
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_FAIL;
				m_pServer->SENDTOCLIENT(pCdChar->ClientSlot(), &NetMsgFb);
				sGuidance.DoCertifyEnd();
				continue;
			}

			// ����̵� ī�带 �̿��� ���(ģī, ��ȯī���)
			if (pCdChar->GetCurrentMap().getGaeaMapID().Id() != sGuidance.m_dwClubMap)
			{
				// �������� ��� ��Ż.
				// �ڱ��ڽſ���
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_ING_DISTANCE;
				m_pServer->SENDTOCLIENT ( pCdChar->ClientSlot(), &NetMsgFb );

				// ��ο���
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
			
			// �������� ���
			if ( !pCdChar->IsValidBody() )
			{
				// �ڱ��ڽſ���
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_ING_DIE;
				m_pServer->SENDTOCLIENT ( pCdChar->ClientSlot(), &NetMsgFb );

				// ��ο���

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
				// �������� ��� ��Ż.
				// �ڱ��ڽſ���
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_ING_DISTANCE;
				m_pServer->SENDTOCLIENT ( pCdChar->ClientSlot(), &NetMsgFb );

				// ��ο���
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
				// �������� �ð� �ʰ�.
				// �ڱ��ڽſ���
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFb;
				NetMsgFb.emFB = EMCDCERTIFY_ING_TIMEOUT;
				m_pServer->SENDTOCLIENT(pCdChar->ClientSlot(), &NetMsgFb);

				// ��ο���
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
				// ���� �Ϸ�.
				GLMSG::SNET_CLUB_CD_CERTIFY_FB NetMsgFB;
				NetMsgFB.emFB = EMCDCERTIFY_COMPLETE;
				m_pServer->SENDTOCLIENT(pCdChar->ClientSlot(), &NetMsgFB);

				// ������Ʈ�� �˸�.
				GLMSG::NET_CLUB_CERTIFIED_FA NetMsg;
				NetMsg.m_GuidanceId = sGuidance.GetId();
				NetMsg.m_ClubDbNum  = pCdChar->GetClubDbNum();
				m_pServer->SENDTOAGENT(&NetMsg);

                // Club log
                // [%1%]���� [%2%]������ ����Ŭ�� ���������� ������ �����Ͽ����ϴ�.
                std::string ClubLog(
                    sc::string::format(
                        ID2GAMEINTEXT("CLUB_LOG_24"),
                        pCdChar->GetName(),
                        GetName(sGuidance.GetId())));
                m_pServer->SENDCLUBLOG(pCdChar->GetClubDbNum(), club::LOG_GUIDANCE, ClubLog);

				// �� ����
                // �������� Ŭ���� �Ҽ�ĳ���Ͱ� �ƴϸ� ��� �� ������ ƨ�ܳ���
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

	// ������ 3�� �̻����� �����ؾ� �Ѵ�. ������ 3���� �س��� ���� ���� �� ��� �۾��ϱ⵵ �ָ��ϴ�. 
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

		//	���� ó�������ؼ� FieldServer������ ������ Ŭ���� ����Ʈ�� ������ �ִ�....		
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
	// ���� �ʵ��� ���⸦ �����ش�.
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

	// ���� ������ �ִ� ������ ���⸦ �����ش�.
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
					sFlagNetMsgBrd.dwGuidanceID = -1;			// GLogicData::GetInstance().wbGetGuidanceID() �� ���� ���Ѵ�.
																// -1 �̶�� ���� ���� ���� ���� ������ ������ Ŭ���� ���⸦ ǥ���ϵ��� �Ѵ�.
																// ������� �����ϰ� ���� Effect�� wbCountry.lua -> wbEffectGuidanceID ���� ������ �� 
																//  { -1, L"flag.egp" }, ������� ������ �Ѵ�.
					sFlagNetMsgBrd.dwCountryCode = spGuidClub->GetCountry();
					m_pServer->SENDTOCLIENT ( pChar->ClientSlot(), &sFlagNetMsgBrd );
				}
				return;
			}
		}
	}
}
