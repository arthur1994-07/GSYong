#include "../pch.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLCompetition.h"

#include "../Database/DBAction/DbActionLog.h"
#include "../Land/GLLandMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"
#include "../Crow/GLCrow.h"

#include "../TriggerManager/GLTriggerManagerFD.h"

#include "./CaptureTheFieldManager.h"
#include "./CaptureTheFieldScriptMan.h"


#include "../../SigmaCore/DebugInclude.h"


CaptureTheFieldManFld::CaptureTheFieldManFld(GLGaeaServer *pGLGaeaServer)
    : m_pGLGaeaServer(pGLGaeaServer)
    , m_pLandManPvE(NULL)
    , m_pLandManPvP(NULL)
    , m_nRemainMinutes(m_nReadyDurationM)
    , m_bCTFBattleGround(FALSE)
    , m_pCertification1(NULL)
    , m_pCertification2(NULL)
    , m_pCertification3(NULL)
    , m_fUpdatePlayersPositionTimer(0.0)	//1sec
{
	ZeroMemory(m_arrPlayerNum, sizeof(m_arrPlayerNum));
	ZeroMemory(m_arrRebirthTimeSec, sizeof(m_arrRebirthTimeSec));	
}

void CaptureTheFieldManFld::HookAfterInit()
{
	m_pLandManPvE		= m_pGLGaeaServer->GetLand(SNATIVEID(m_dwPVEMapID));
	m_pLandManPvP		= m_pGLGaeaServer->GetLand(SNATIVEID(m_dwPVPMapID));   

    // 채널과 필드서버 번호 둘다 체크하도록 수정;
    bool bCTFBattleFieldSrv(false);
    SMAPNODE* pMapNode =  m_pGLGaeaServer->FindMapNode(SNATIVEID(m_dwPVEMapID));
    if ( pMapNode )
    {
        bCTFBattleFieldSrv = (pMapNode->GetFieldServerNum() == m_pGLGaeaServer->GetFieldSvrID());
    }
	m_bCTFBattleGround	= ( (m_pLandManPvE != NULL) && (m_pLandManPvP != NULL) &&
							(m_pGLGaeaServer->GetServerChannel() == m_nChannel) &&
                            bCTFBattleFieldSrv );

	//이 시점에는 아직 LandMan들에 대해 FrameMove가 돌기 전이므로, 각 Crow들이 DropCrow되기 전이다.
	//즉, 인증기가 아직 생성되지 않았다.
}


CaptureTheFieldManFld::~CaptureTheFieldManFld()
{
}


bool CaptureTheFieldManFld::FrameMove(float fElapsedAppTime)
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
		return false;

	if (GLCONST_CHAR::bPKLESS)
        return false;

	if (m_bCTFBattleGround && m_state == CTF_STATE_START)
	{
		UpdatePlayersPosition(fElapsedAppTime);	//update players' position vector for minimap update

		if (IsAuthed())
		{
			Captured();
		}
	}

	return true;
}


void CaptureTheFieldManFld::Ready()
{
	sc::writeLogInfo(sc::string::format("CTF: Ready for CaptureTheField. %1%min remains.", m_nReadyDurationM));

	m_state = CTF_STATE_READY;
	
	/*
	m_bCTFBattleGround	= ( (m_pLandManPvE != NULL) && (m_pLandManPvP != NULL) &&
							(m_pGLGaeaServer->GetServerChannel() == m_nChannel) );
	*/

	if(m_bCTFBattleGround)
	{
		//This FieldServer has the map for CTF.

        m_pCertification1 = m_pLandManPvP->GetCrowByID(m_dwCertificationID1);

        if (!m_pCertification1)
        {
            sc::writeLogError(std::string("CTF: Certification#1 does not exist."));
            return;
        }

        m_pCertification2 = m_pLandManPvP->GetCrowByID(m_dwCertificationID2);

        if (!m_pCertification2)
        {
            sc::writeLogError(std::string("CTF: Certification#2 does not exist."));
            return;
        }

		m_pCertification3 = m_pLandManPvP->GetCrowByID(m_dwCertificationID3);

        if (!m_pCertification3)
        {
            sc::writeLogError(std::string("CTF: Certification#3 does not exist."));
            return;
        }

		//
		//All characters in the CTF map will be warned that they will be dropped if they are not registered for CTF.
		//
		// 이제 점령전 진행중에도 사냥터로 이용할 수 있게 되었다;
		// 경고를 하지 않는다;
		/*GLMSG::SNET_CTF_WARNING4DROP_FC msgWarning;
		msgWarning.nRemainMinutes = m_nReadyDurationM;

		m_pLandManPvE->SendMsgPC(&msgWarning);*/
	}
	else
	{
		//This FieldServer is not responsible for CTF.

		//Do Nothing.
	}
}


void CaptureTheFieldManFld::UpdateReady()
{
	if(!m_pLandManPvE)
		return;

	//if (m_nRemainMinutes == 1)
	{
		//
		//All characters in the CTF map will be warned that they will be dropped if they are not registered for CTF.
		//
		// 이제 점령전 진행중에도 사냥터로 이용할 수 있게 되었다;
		// 경고를 하지 않는다;
		/*GLMSG::SNET_CTF_WARNING4DROP_FC msgWarning;
		msgWarning.nRemainMinutes = m_nRemainMinutes;

		m_pLandManPvE->SendMsgPC(&msgWarning);*/

		sc::writeLogInfo(sc::string::format("CTF: Ready for CaptureTheField. %1%min remains.", m_nRemainMinutes));
	}
}


BOOL CaptureTheFieldManFld::Prepare()
{
	if (!m_pLandManPvP)
	{
		sc::writeLogWarn(std::string("CaptureTheFieldManFld::Prepare() - This FieldServer does not have CTF map."));
		return FALSE;
	}

	if (m_bCTFBattleGround)
	{
		sc::writeLogInfo(std::string("CTF: Preparing for CTF."));
	}

	//
	//Initialize Authenticators' HP and value of authed-school.
	//
	m_pLandManPvP->ResetCrow4CTF();
	m_wPrevSchoolAuthed = m_wSchoolAuthed;
	m_wSchoolAuthed = SCHOOL_NONE;

    triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_CTF_Begin().Do();

	//
	//1. Disable all gates to CTF-map.
	//
	// -> this case will be handled in MsgReqFieldSvrCharChkFb() and MsgReqGateOut() each for moving field and moving through a gate.
	//

	//
	// Do something before start here.
	//

	GLMSG::SNET_CTF_PREPARE_AF_FB msg;
	msg.bHere		= m_bCTFBattleGround ? TRUE : FALSE;
	msg.nFieldSvr	= m_pGLGaeaServer->GetFieldSvrID();		//and Channel is CTF_CHANNEL(0).

	m_pGLGaeaServer->SENDTOAGENT(&msg);


	return TRUE;
}


void CaptureTheFieldManFld::Start()
{
	sc::writeLogInfo(std::string("CTF: CaptureTheField starts."));

	m_state = CTF_STATE_START;

	triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_CTF_Begin().Do();

    //m_tStartedTime = CTime::GetCurrentTime().GetTime();	
}


void CaptureTheFieldManFld::Done()
{
	m_state = CTF_STATE_DONE;
	
	sc::writeLogInfo(std::string("CTF: Done."));
	
	m_wSchoolAuthed = SCHOOL_NONE;

	m_ctEndTime = CTime::GetCurrentTime();	

	GASSERT(m_pLandManPvP);
	GASSERT(m_pCertification1 && m_pCertification2 && m_pCertification3);

	WORD arrSchoolAuthed[3];
	arrSchoolAuthed[0] = m_pCertification1->IsAuthedBy();
	arrSchoolAuthed[1] = m_pCertification2->IsAuthedBy();
	arrSchoolAuthed[2] = m_pCertification3->IsAuthedBy();

	
	UINT arrAuthCount[SCHOOL_NUM];
	ZeroMemory(arrAuthCount, sizeof(arrAuthCount));

	WORD wSchool = 0;	
	for (int i=0; i<3/*인증기 갯수*/; i++)
	{
		wSchool = arrSchoolAuthed[i];

		if (wSchool != SCHOOL_NONE)
		{
			if ( ++arrAuthCount[wSchool] >= 2)
			{
				m_wSchoolAuthed = wSchool;	//wSchool has authed 2 certifications.
				break;
			}
		}
	}

	GLMSG::SNET_CTF_DONE_AF_FB msg;
	msg.wSchoolAuthed = m_wSchoolAuthed;

	m_pGLGaeaServer->SENDTOAGENT(&msg);
}


void CaptureTheFieldManFld::Stop()
{
	m_state = CTF_STATE_STOP;
	
	sc::writeLogInfo(std::string("CTF: CaptureTheField has been finished."));
}


void CaptureTheFieldManFld::Quit(DWORD dwChaNum, ENUM_CTF_STATE state)
{
    GLChar* pChar = m_pGLGaeaServer->GetCharByDbNum(dwChaNum);

    if (!pChar)
    {
        //! 필드에서 이미 지운후 agent에서 올수도 있다.
        sc::writeLogError(std::string("CaptureTheFieldManFld::Quit() - pChar is NULL."));
        return;
    }

	//
	// Unset flag to prevent using of potions.
	//
	//pChar->m_dwActionLimit ~= EMACTION_LIMIT_ITEM;

    triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_CTF_Quit_Character().Do(pChar);
}

void CaptureTheFieldManFld::Rebirth(GLChar *pChar)
{
	if(!pChar)
	{
		sc::writeLogError(std::string("CaptureTheFieldManFld::Rebirth() - pChar is NULL"));
		return;
	}
	triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_CTF_Rebirth_Character().Do(pChar);
	
}

void CaptureTheFieldManFld::NewPlayerHasCome(DWORD dwChaNum)
{
    GLChar* pChar = m_pGLGaeaServer->GetCharByDbNum(dwChaNum);

    if (!pChar)
    {
        sc::writeLogError(std::string("CaptureTheFieldManFld::NewPlayerHasCome(dwChaNum) - pChar is NULL."));
        return;
    }

    triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_CTF_New_Character().Do(pChar);
}


void CaptureTheFieldManFld::NewPlayerHasCome(GLChar *pChar)
{
    if (!pChar)
    {
        sc::writeLogError(std::string("CaptureTheFieldManFld::NewPlayerHasCome(pChar) - pChar is NULL."));
        return;
    }

    triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_CTF_New_Character().Do(pChar);
}


void CaptureTheFieldManFld::Captured()
{
    m_state = CTF_STATE_DONE;

	GLMSG::SNET_CTF_END_FA msg;    
	msg.wSchoolAuthed = m_wSchoolAuthed;

	m_pGLGaeaServer->SENDTOAGENT(&msg);

	sc::writeLogInfo(sc::string::format("CTF: School#%1% has captured the field!", m_wSchoolAuthed));
}


void CaptureTheFieldManFld::Reward(WORD wSchoolAuthed, UINT nHowMany, CTF_REWARD_BUFF *arrRewardBuff)
{
	m_state = CTF_STATE_REWARD;
	
	m_wSchoolAuthed = wSchoolAuthed;

	sc::writeLogInfo(std::string("CTF: Rewarding..."));	

	if (IsBattleGround())
	{
		//
		// Notify all clients reward time starts.
		//
		GLMSG::SNET_CTF_REWARD_FC msgFC;
		m_pLandManPvP->SendMsgPC(&msgFC);

		std::vector<CTF_STATISTICS_4_ONEPLAYER>				vecResult;
		std::vector<CTF_STATISTICS_4_ONEPLAYER>::iterator	iter;


		//
		//Update players' contribution points
		//
		m_setRanking.clear();

        // for club log
        std::set<PVP::CTF_RANKER_CLUB_LOG> SetClubLog;

		GLChar *pChar			= NULL;
		GLCHARNODE* pCharNode	= m_pLandManPvP->m_GlobPCList.m_pHead;
			
		for(; pCharNode; pCharNode = pCharNode->pNext)
		{
			pChar = pCharNode->Data;

			if (pChar)
			{
				//
				// 참가자들만 기여도를 받게된다.
				UpdateContributionPoint(pChar);
				
				CTF_STATISTICS_4_ONEPLAYER one((DWORD)pChar->m_emClass, pChar->GetSchool(), pChar->m_szName, pChar->m_PvPPerformance);				

				vecResult.push_back(one);

				//
				// for ranking
				//
				PVP::CTF_RANKER ranker(pChar->m_PvPPerformance);
				ranker.wClass	= (DWORD)pChar->m_emClass;
				ranker.wSchool	= pChar->GetSchool();
				ranker.SetName(pChar->m_szName);				
				
				m_setRanking.insert(ranker);

                PVP::CTF_RANKER_CLUB_LOG ClubRanker(pChar->m_PvPPerformance, pChar->GetClubDbNum(), pChar->GetName());
                SetClubLog.insert(ClubRanker);
			}
		}
        
        // Club log
        // 전령전 종료시 3위 이내 진입한 클럽에 소속된 캐릭터의 로그
        size_t RankCount = 1;
        for (std::set<PVP::CTF_RANKER_CLUB_LOG>::const_iterator iter=SetClubLog.begin(); iter!=SetClubLog.end(); ++iter)
        {
            if (RankCount > 3)
                break;

            const PVP::CTF_RANKER_CLUB_LOG& Data = *iter;
            if (Data.m_ClubDbNum != CLUB_NULL)
            {
                // [%1%]님이 점령전에서 랭킹 %2%위를 기록했습니다.
                std::string ClubLog(
                    sc::string::format(
                        ID2GAMEINTEXT("CLUB_LOG_28"),
                        Data.m_szName,
                        RankCount));
                m_pGLGaeaServer->SENDCLUBLOG(Data.m_ClubDbNum, club::LOG_CTF, ClubLog);
            }
            ++RankCount;
        }
		
		//
		//Send CTF statistics to all players
		//
		UINT cnt = (UINT)vecResult.size();

		WORD wLastSeq = cnt / HOW_MANY_CTFSTATISTICS_PER_ONE_MSG + ((cnt % HOW_MANY_CTFSTATISTICS_PER_ONE_MSG) > 0);

		
		GLMSG::SNET_CTF_STATISTICS_FC msg;


		if (wLastSeq == 1)
		{
			msg.SetData(vecResult, 1, 1);			
			
			m_pLandManPvP->SendMsgPC(&msg);
		}
		else
		{
			std::vector<CTF_STATISTICS_4_ONEPLAYER>::iterator iterCurrent = vecResult.begin();
			UINT i = 1;

			//
			// 1 ~ (n-1)
			//
			for (WORD wSeq = 1; wSeq < wLastSeq; wSeq++)
			{	
				msg.SetData(iterCurrent, iterCurrent + HOW_MANY_CTFSTATISTICS_PER_ONE_MSG, wSeq, wLastSeq);
				
				m_pLandManPvP->SendMsgPC(&msg);

				iterCurrent = iterCurrent + HOW_MANY_CTFSTATISTICS_PER_ONE_MSG;

				GASSERT(iterCurrent < vecResult.end());
			}
			//
			// n (last one)
			//
			msg.SetData(iterCurrent, vecResult.end(), wLastSeq, wLastSeq);			
			
			m_pLandManPvP->SendMsgPC(&msg);
		}        

		GLMSG::SNET_CTF_RANKING_FAC msgRanking;
		msgRanking.SetData(m_setRanking);

		m_pGLGaeaServer->SENDTOAGENT(&msgRanking);
	}
    
    ClearRewardBuff();

    for (UINT i=0; i<nHowMany; i++)
    {
        AddRewardBuff(SNATIVEID(arrRewardBuff[i].dwID), arrRewardBuff[i].wLevel);        
    }

	//
	//Do jobs for Reward
    //
    triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_CTF_End().Do(ENUM_SCHOOL(m_wSchoolAuthed), nHowMany);
}


void CaptureTheFieldManFld::PlayerNumHasUpdated()
{
    triggermanager::GLTriggerManagerFD::GetInstance().Get_Trigger_CTF_Changed_UserCount().Do( m_arrPlayerNum );
}


//
//Update players' position vector for minimap update
//
void CaptureTheFieldManFld::UpdatePlayersPosition ( float fElapsedAppTime )
{
	m_fUpdatePlayersPositionTimer += fElapsedAppTime;

	if ( m_fUpdatePlayersPositionTimer > m_fUpdatePlayerPositionFreq )
	{
		m_fUpdatePlayersPositionTimer = 0.0;

		// 학원별 Temp 메시지;
		GLMSG::SNET_CTF_UPDATE_PLAYERS_VECTOR_FC msgPos[SCHOOL_NUM];

		// 실제로 보내는 메시지 컨테이너;
		//  - 최대인원 50명에 대해 두당 12-bytes 짜리 data를 보내야하므로;
		//  - 모든 학원 정보를 하나의 메시지에 실어서 보내는것은 무리가 있다;
		std::vector<GLMSG::SNET_CTF_UPDATE_PLAYERS_VECTOR_FC> vecMsgPos[SCHOOL_NUM];

		GLChar* pChar			= NULL;
		GLCHARNODE* pCharNode	= m_pLandManPvP->m_GlobPCList.m_pHead;
		WORD wSchool			= SCHOOL_NONE;

		// 1. 모든 캐릭터를 조사하여 학원별로 Temp 메시지를 세팅한다;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			pChar = pCharNode->Data;

			if ( pChar )
			{
				wSchool = pChar->GetSchool();

				if ( wSchool >= SCHOOL_NUM )
				{
					sc::writeLogError( stringformat(
						"CaptureTheFieldManFld::UpdatePlayersPosition#1 - Char(%1%) has wrong school(%1%)",
						pChar->CharDbNum(),
						wSchool ) );

					continue;
				}

				// msgPos[wSchool].AddData(pChar->GetPosition());

				/// 캐릭터의 추가를 실패하는경우, ;
				/// 기존의 Temp 메시지를 컨테이너에 등록하고 Temp 메시지를 하나 더 만든다;
				if ( FALSE == msgPos[wSchool].AddData( pChar->GetPosition() ) /*|| (pCharNode->pNext == NULL)*/ )
				{
					if ( msgPos[wSchool].nHowMany == 0 )
						continue;

					const UINT nNextPos( msgPos[wSchool].nRelativeUserIndex + msgPos[wSchool].nHowMany );

					/// 여기서 발생하는 복사비용이 아까우면;
					/// msgPos[SCHOOL_NUM]를 벡터 이터레이터로 바꾸던가하면됨;

					// 기존의 Temp 메시지를 컨테이너에 등록한다;
					vecMsgPos[wSchool].push_back(msgPos[wSchool]);

					// 새로운 Temp 메시지를 만든다;
					msgPos[wSchool] = GLMSG::SNET_CTF_UPDATE_PLAYERS_VECTOR_FC();
					msgPos[wSchool].nRelativeUserIndex = nNextPos;
				}
			}
		}

		// 2. 각 학원별 Temp 메시지를 컨테이너에 등록한다;
		for ( int i=0; i<SCHOOL_NUM; ++i )
		{
			vecMsgPos[i].push_back( msgPos[i] );
		}


		pCharNode	= m_pLandManPvP->m_GlobPCList.m_pHead;	

		// 3. 모든 캐릭터를 조사하여 학원별로 메시지 컨테이너에 등록된 메시지를 보낸다;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			pChar = pCharNode->Data;

			if ( pChar )
			{
				wSchool = pChar->GetSchool();

				if ( wSchool >= SCHOOL_NUM )
				{
					sc::writeLogError( stringformat(
						"CaptureTheFieldManFld::UpdatePlayersPosition#2 - Char(%1%) has wrong school(%1%)",
						pChar->CharDbNum(),
						wSchool ) );

					continue;
				}

				// msgPos[wSchool].dvMyPos = pChar->GetPosition();	//미니맵에서 본인의 좌표는 표시되지 않도록 하기 위함.
				// pChar->SendToClient(&msgPos[wSchool]);
				const INT nVecSize( (INT)vecMsgPos[wSchool].size() );
				std::vector<GLMSG::SNET_CTF_UPDATE_PLAYERS_VECTOR_FC>& vecPlayerPos = vecMsgPos[wSchool];
				for ( INT i = 0; i < nVecSize; ++i )
				{
					vecPlayerPos[i].dvMyPos = pChar->GetPosition();
					pChar->SendToClient(&vecPlayerPos[i]);
				}
			}
		}
	}
}


void CaptureTheFieldManFld::SetNumberOfPlayers(UINT idx, UINT num)
{
	GASSERT((idx >= 0) && (idx < SCHOOL_NUM));
	//GASSERT(num >= 0); // always true

	m_arrPlayerNum[idx] = num;
}


UINT CaptureTheFieldManFld::GetNumberOfPlayers(UINT idx)
{
	GASSERT((idx >= 0) && (idx < SCHOOL_NUM));	

	return m_arrPlayerNum[idx];
}


DWORD CaptureTheFieldManFld::GetRebirthTimeSec(WORD wSchool)
{
	int nSecs = (int)(m_nMaxRebirthTime - (m_nMaxNumOfPlayers - m_arrPlayerNum[wSchool]) / 2);

	return (DWORD)max(nSecs, 1);
}


BOOL CaptureTheFieldManFld::IsAuthed()
{
	GASSERT(m_pLandManPvP);
	GASSERT(m_pCertification1 && m_pCertification2 && m_pCertification3);

	if ( !m_pCertification1 || !m_pCertification2 || !m_pCertification3 )
	{
		sc::writeLogError( "CaptureTheFieldManFld Certification is NULL." );

		return FALSE;
	}

	WORD wSchoolAuthed1 = m_pCertification1->IsAuthedBy();
	WORD wSchoolAuthed2 = m_pCertification2->IsAuthedBy();
	WORD wSchoolAuthed3 = m_pCertification3->IsAuthedBy();

	if ( (wSchoolAuthed1 != SCHOOL_NONE) && (wSchoolAuthed2 != SCHOOL_NONE) && (wSchoolAuthed3 != SCHOOL_NONE) 
		&& (wSchoolAuthed1 == wSchoolAuthed2) && (wSchoolAuthed2 == wSchoolAuthed3) )
	{
		m_wSchoolAuthed = wSchoolAuthed1;
		return TRUE;
	}
	
	return FALSE;
}


BOOL CaptureTheFieldManFld::UpdateContributionPoint(GLChar *pChar)
{
	if (!pChar)
    {
        sc::writeLogError(std::string("CaptureTheFieldManFld::UpdateContributionPoint() - pChar is NULL."));
        return FALSE;
    }

	if (pChar->GetSchool() == m_wSchoolAuthed)
	{
		if( pChar->m_PvPPerformance.nDamageTo >= m_pGLGaeaServer->GetCTFDamage() ||
			pChar->m_PvPPerformance.nHealTo >= m_pGLGaeaServer->GetCTFRecovery() )
			pChar->UpdateContributionPoint(m_nContributionPoint4Winner, m_nContributionPointMax);
	}
	else
	{
		pChar->UpdateContributionPoint(m_nContributionPoint4Loser, m_nContributionPointMax);
	}


	//
	//CTF PlayLog
	//
	//현재 기여도가 줄어드는 문제를 해결하기 위해 로그기능을 추가함. 더불어 CacheServer 사이의 연관관계가 있는지 확실하지 않기 때문에 이 로그는 Field에서 direct로 DB처리를 하도록 함.
	//
	pChar->AddGameAdoJob(
			db::DbActionPtr(
			    new db::CCTFPlayLog(
                    pChar->m_CharDbNum,
                    pChar->GetSchool(),
                    pChar->m_nContributionPoint,
                    pChar->m_nRewardContribution)));
	
	pChar->SaveContributionPoint();

	return TRUE;
}


const D3DXVECTOR3& CaptureTheFieldManFld::GetCertPos1()
{
	return m_pCertification1->GetPosition();
}


const D3DXVECTOR3& CaptureTheFieldManFld::GetCertPos2()
{
	return m_pCertification2->GetPosition();
}


const D3DXVECTOR3& CaptureTheFieldManFld::GetCertPos3()
{
	return m_pCertification3->GetPosition();
}


WORD CaptureTheFieldManFld::GetSchoolAuthedCert1()
{
	return m_pCertification1->IsAuthedBy();
}


WORD CaptureTheFieldManFld::GetSchoolAuthedCert2()
{
	return m_pCertification2->IsAuthedBy();
}


WORD CaptureTheFieldManFld::GetSchoolAuthedCert3()
{
	return m_pCertification3->IsAuthedBy();
}
