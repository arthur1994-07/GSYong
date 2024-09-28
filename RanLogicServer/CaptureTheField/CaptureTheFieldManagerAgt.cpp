#include "../pch.h"

#include "./CaptureTheFieldManager.h"
#include "./CaptureTheFieldScriptMan.h"

#include "../../RanLogic/GLUseFeatures.h"

#include "../Land/GLAGLandMan.h"
#include "../AgentServer/GLAgentServer.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../TriggerManager/GLTriggerManagerAG.h"
#include "../PVE/PVESystem.h"

#include "../MatchSystem/MatchingManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



CaptureTheFieldManAgt::CaptureTheFieldManAgt(GLAgentServer* pGLAgentServer)
:m_pGLAgentServer(pGLAgentServer)
,m_pLandManPvE(NULL)
,m_pLandManPvP(NULL)
,m_dwBaseTime(0)
,m_dwCurrentTime(0)
,m_dwPrevElapsedMinutes(0)
,m_nFieldSvr(0)
,m_dwKickCheckTime(0)
{
	
}


CaptureTheFieldManAgt::~CaptureTheFieldManAgt()
{
}


void CaptureTheFieldManAgt::HookAfterInit()
{
	m_pLandManPvE	= m_pGLAgentServer->GetLand(SNATIVEID(m_dwPVEMapID));
	m_pLandManPvP	= m_pGLAgentServer->GetLand(SNATIVEID(m_dwPVPMapID));
}


bool CaptureTheFieldManAgt::FrameMove(float fElapsedAppTime)
{
	if (!GLUseFeatures::GetInstance().IsUsingCaptureTheField())
		return false;

	if (GLCONST_CHAR::bPKLESS)
        return false;

	RunStateMachine();
	

	return true;
}

void CaptureTheFieldManAgt::CTFNoneUserKickOut()
{
	if (m_pLandManPvP && m_pLandManPvP->CharCount() > 0)
	{
		//sc::writeLogInfo(sc::string::format("CTF(NoneUser Out) User Total : %1%", m_pLandManPvP->CharCount() ));

		const GLAGLandMan::SETPC& PcList = m_pLandManPvP->GetChar();
		GLAGLandMan::SETPC_CITER iter    = PcList.begin();
		GLAGLandMan::SETPC_CITER iterEnd = PcList.end();

		GLCharAG *pChaAG = NULL;

		for (; iter != iterEnd; ++iter)
		{
			pChaAG = m_pGLAgentServer->GetCharByDbNum(*iter);	//dbNum

			if (!pChaAG || (pChaAG->UserType() >= USER_GM3) )
				continue;

			if( pChaAG->m_bCTFPlayer == FALSE )
			{
				sc::writeLogInfo(sc::string::format("[CTF] CaptureTheFieldManAgt::CTFNoneUserKickOut ERROR. DbNum : %1%", pChaAG->ChaDbNum()));
				continue;
			}
			
			pChaAG->m_bCTFPlayer = FALSE;

			KickOut(pChaAG);

			sc::writeLogInfo(sc::string::format("CTF(NoneUser Out) DbNum : %1%", pChaAG->ChaDbNum()));
		}

		//sc::writeLogInfo(sc::string::format("CTF(NoneUser Out) CTF State : %1%", m_state ));
	}	
}

ENUM_CTF_STATE CaptureTheFieldManAgt::RunStateMachine()
{
	CTime ctNow = CTime::GetCurrentTime();

	switch(m_state)
	{
	case CTF_STATE_STOP:
		{	
			if (ctNow >= m_ctReadyTime)
			{
				if ( !DoReady() )
					DoStop();
			}
			else
			{
				DWORD dwCurTickCount = GetTickCount();

				// ctf 종료 10초후 부터 체크 or 시작전

				if( dwCurTickCount - m_dwKickCheckTime > 10000 )
				{
					CTFNoneUserKickOut();

					m_dwKickCheckTime = GetTickCount();
				}
			}
		}
		break;

	case CTF_STATE_READY:
		{
			if (ctNow >= m_ctStartTime)
			{
				if ( !DoStart() )
					DoStop();
			}			
			else if ((ctNow < m_ctStartTime) && (ctNow >= m_ctReadyTime))
			{
				DoUpdateReady();
			}
			else
			{
				sc::writeLogError(std::string("CaptureTheFieldManAgt::CheckTime() - MUST NOT come into here! CTF state is wrong."));
			}
		}
		break;

	case CTF_STATE_START:
		{			
			if (ctNow >= m_ctStopTime)
			{				
				DoDone();	//--> msg to Field --> response from Field --> DoReward()
			}
			else
			{
				DoUpdatePlayers();
			}
		}
		break;

	case CTF_STATE_DONE:
		{
			;//The state will be go to REWARD by response of NET_MSG_CTF_DONE_AF_AC msg.
		}
		break;

	case CTF_STATE_REWARD:
		{
			if (ctNow >= m_ctExtraTime)
			{
				DoStop();
			}
		}
		break;

	default:
		{
			//CTFNoneUserKickOut();

			sc::writeLogError(std::string("CaptureTheFieldManAgt::CheckTime() - MUST NOT get to here!"));			
		}		
	}

	return m_state;
}


BOOL CaptureTheFieldManAgt::DoReady()
{
	sc::writeLogInfo(sc::string::format("CTF: Ready for CaptureTheField. %1%min remains.", m_nReadyDurationM));

	if(m_pLandManPvE == NULL || m_pLandManPvP == NULL)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::DoReady() - CTF land does not exist."));
		return FALSE;
	}

	m_state = CTF_STATE_READY;
	m_dwPrevElapsedMinutes = 0;
	m_dwBaseTime = GetTickCount();
	CTime cTime = CTime::GetCurrentTime();
	m_dwBaseTime -= cTime.GetSecond()*1000;

	GLMSG::SNET_CTF_READY_AF msgAF;
	msgAF.bUpdate		= FALSE;
	msgAF.nRemainMinutes= m_nReadyDurationM;
	m_pGLAgentServer->SENDTOCHANNEL(&msgAF, m_nChannel);


	GLMSG::SNET_CTF_READY_AC msgAC;
	msgAC.bUpdate		= FALSE;
	msgAC.nRemainMinutes= m_nReadyDurationM;
	msgAC.bConsecutively= m_bConsecutively;
	m_pGLAgentServer->SENDTOALLCLIENT(&msgAC);

	return TRUE;
}


void CaptureTheFieldManAgt::DoUpdateReady()
{
	m_dwCurrentTime = GetTickCount();

	DWORD dwElapsedMinutes = (m_dwCurrentTime - m_dwBaseTime) / 60000;	//60 sec
	
	if (m_dwPrevElapsedMinutes != dwElapsedMinutes)
	{
		m_dwPrevElapsedMinutes = dwElapsedMinutes;

		sc::writeLogInfo(sc::string::format("CTF: Ready for CaptureTheField. %1%min remains.", m_nReadyDurationM - dwElapsedMinutes));

		GLMSG::SNET_CTF_READY_AF msgAF;
		msgAF.bUpdate		= TRUE;
		msgAF.nRemainMinutes= m_nReadyDurationM - dwElapsedMinutes;
		m_pGLAgentServer->SENDTOALLCHANNEL(&msgAF);
		
		GLMSG::SNET_CTF_READY_AC msgAC;
		msgAC.bUpdate		= TRUE;
		msgAC.nRemainMinutes= m_nReadyDurationM - dwElapsedMinutes;
		msgAC.bConsecutively= m_bConsecutively;
		m_pGLAgentServer->SENDTOALLCLIENT(&msgAC);
	}
}


BOOL CaptureTheFieldManAgt::DoStart()
{
	if (m_state != CTF_STATE_READY)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::Start() - CTF can start only from READY state."));
		return FALSE;
	}

	sc::writeLogInfo(std::string("CTF: CaptureTheField starts."));

	m_state = CTF_STATE_START;
	m_dwPrevElapsedMinutes = 0;			//now tickcount will be used for notifying CTF state while it's in progress.
	m_dwBaseTime = GetTickCount();
	m_wPrevSchoolAuthed = m_wSchoolAuthed;
	m_wSchoolAuthed = SCHOOL_NONE;
	
	

	GLMSG::SNET_CTF_PREPARE_AF	msg;
	GLMSG::SNET_CTF_STAT_LOG_AH	msgLog;
	msgLog.state = CTF_STATE_START;


	//
	//1. Make a players-list from joiners-list.
	//	
	DEQ_CTF_JOINERS_ITER iter;
	UINT i = 0;

	for(int wSchool=0; wSchool<SCHOOL_NUM; wSchool++)
	{
		//
		// note: there are two condition variables, an iterator and an integer.
		//
		for(iter = m_deqCTFJoiners[wSchool].begin(), i = 0; (iter != m_deqCTFJoiners[wSchool].end()) && (i < m_nMaxNumOfPlayers); ++i)
		{
			m_setCTFPlayers[wSchool].insert(*iter);		// list of players who will play the CTF.
			msg.nPlayers[wSchool]++;

			iter = m_deqCTFJoiners[wSchool].erase(iter);
		}

		msgLog.nPlayers[wSchool] = msg.nPlayers[wSchool];
	}	

	//
	//2. Kick out characters who did not registered to play CTF.
	//
	// 이제 점령전 진행중에도 사냥터로 이용할 수 있게 되었다;
	// 즉 KickOut 시키지 않는다;
	//if (KickOutNonPlayers() == FALSE)
	//	return FALSE;


	//
	//3. Let the FieldServer prepare for CTF.
	//
	m_pGLAgentServer->SENDTOALLCHANNEL(&msg);//, CTF_CHANNEL);


	//
	//4. Log for statistics
	//
	m_pGLAgentServer->SENDTOCACHE(&msgLog);
	

	//
	//5. After receiving the response of Prepare msg, CTF will be started by calling Start() method.
	//

	return TRUE;
}


void CaptureTheFieldManAgt::DoDone()
{
	m_state = CTF_STATE_DONE;	

	sc::writeLogInfo(std::string("CTF: Done."));

	GLMSG::SNET_CTF_DONE_AF_AC msg;
	m_pGLAgentServer->SENDTOFIELDSVR(m_nChannel, m_nFieldSvr, &msg);

	//
	//이 메시지를 받는 시점에, PvP맵 상의 모든 클라이언트는 대기중인(부활시간 카운팅 중) 부활처리를 완료시킨다.	
	//
	//m_pLandManPvP->SENDTOALLCLIENT(&msg);
}


void CaptureTheFieldManAgt::DoReward()
{
	m_state = CTF_STATE_REWARD;

	m_ctEndTime		= CTime::GetCurrentTime();
	m_ctExtraTime	= m_ctEndTime + m_ctsRewardTimeM;

	//
	// Notify all clients reward time starts.
	//
	//GLMSG::SNET_CTF_REWARD_AC msgAC;
	//m_pLandManPvP->SENDTOALLCLIENT(&msgAC);

	m_dwPrevElapsedMinutes = 0;
	m_dwBaseTime = GetTickCount();

    m_vecRewardBuffs.clear();
	m_history.vecRewardBuffs.clear();

	sc::writeLogInfo(std::string("CTF: Rewarding..."));

    triggermanager::GLTriggerManagerAG::GetInstance().Get_Trigger_CTF_End().Do( ENUM_SCHOOL(m_wSchoolAuthed) );

	SetConsecutively(m_wSchoolAuthed);

    CTF_REWARD_BUFF arrRewardBuffs[CTF_MAXSIZE_OF_BUFF_ARRAY_4_GLMSG];
    
    UINT nSize = (UINT)m_vecRewardBuffs.size();

    for (UINT i=0; i<nSize; i++)
    {
        arrRewardBuffs[i].dwID  = m_vecRewardBuffs[i].dwID;
        arrRewardBuffs[i].wLevel= m_vecRewardBuffs[i].wLevel;

		m_history.vecRewardBuffs.push_back(arrRewardBuffs[i]);	//save it for the history
    }

    GLMSG::SNET_CTF_REWARD_INFO msg;
    msg.wSchoolAuthed = m_wSchoolAuthed;
    
    if (msg.SetData(nSize, arrRewardBuffs))
    {
        m_pGLAgentServer->SENDTOALLCHANNEL(&msg);

        m_pGLAgentServer->SENDTOCACHE(&msg);	//Log the result    
    }
    else
    {
        sc::writeLogError(std::string("CaptureTheFieldManAgt::DoReward() - msg.SetData() failure."));

        GASSERT(!"CaptureTheFieldManAgt::DoReward() - msg.SetData() failure.");
    }

	//
	//Log for statistics
	//
	GLMSG::SNET_CTF_STAT_LOG_AH	msgLog;
	msgLog.state = CTF_STATE_REWARD;
	
	for (int i=0; i<SCHOOL_NUM; i++)
	{
		msgLog.nPlayers[i] = (UINT)m_setCTFPlayers[i].size();
	}	
	m_pGLAgentServer->SENDTOCACHE(&msgLog);


	m_history.tStartTime	= (__time32_t)m_ctStartTime.GetTime();
	m_history.tEndTime		= (__time32_t)m_ctEndTime.GetTime();
	m_history.wSchoolAuthed = m_wSchoolAuthed;

	//
	//Ranking은 FieldServer의 DoReward이후에 메시지로 전달받는다.
	//
}


void CaptureTheFieldManAgt::DoStop()
{
	m_state = CTF_STATE_STOP;

	m_dwKickCheckTime = GetTickCount();
	//
	//Reset Player-flag
	//
	if (m_pLandManPvP && m_pLandManPvP->CharCount() > 0)
	{
        const GLAGLandMan::SETPC& PcList = m_pLandManPvP->GetChar();
		GLAGLandMan::SETPC_CITER iter    = PcList.begin();
		GLAGLandMan::SETPC_CITER iterEnd = PcList.end();
		
		GLCharAG *pChaAG = NULL;

		for (; iter != iterEnd; ++iter)
		{
			pChaAG = m_pGLAgentServer->GetCharByDbNum(*iter);	//dbNum

			if (!pChaAG)
				continue;

			if( pChaAG->m_bCTFPlayer == FALSE )
			{
				sc::writeLogInfo(sc::string::format("[CTF] CaptureTheFieldManAgt::DoStop ERROR. DbNum : %1%", pChaAG->ChaDbNum()));
				continue;
			}
			
			pChaAG->m_bCTFPlayer = FALSE;

			if (pChaAG->m_wSchool == m_wSchoolAuthed)
			{
				Recall4Winner(pChaAG);
			}
			else
			{
				KickOut(pChaAG);
			}
		}
	}
	
	for (int i=0; i<SCHOOL_NUM; i++)
	{
		m_deqCTFJoiners[i].clear();
		m_setCTFPlayers[i].clear();
	}

	m_ctStartTime	= CTime::GetCurrentTime() + m_ctsStartCycleH;
	m_ctStopTime	= m_ctStartTime + m_ctsPlayTimeM;
	m_ctReadyTime	= m_ctStartTime - m_ctsReadyDurationM;	
	
	
	GLMSG::SNET_CTF_STOP_AF msgAF;
	m_pGLAgentServer->SENDTOALLCHANNEL(&msgAF);

	GLMSG::SNET_CTF_NEXT_READY_TIME msgAF2;
	msgAF2.ctReadyTime = m_ctReadyTime;
	m_pGLAgentServer->SENDTOALLCHANNEL( &msgAF2 );


	GLMSG::SNET_CTF_STOP_AC msgAC;
	msgAC.wSchoolAuthed     = m_wSchoolAuthed;
	msgAC.bConsecutively	= GetConsecutively();
    msgAC.tNextStartTime    = static_cast<__time32_t>(m_ctStartTime.GetTime());

	m_pGLAgentServer->SENDTOALLCLIENT(&msgAC);


	sc::writeLogInfo(std::string("CTF: CaptureTheField has been finished.."));
}


void CaptureTheFieldManAgt::DoUpdatePlayers()
{
	m_dwCurrentTime = GetTickCount();

	DWORD dwElapsedMinutes = (m_dwCurrentTime - m_dwBaseTime) / m_nUpdatePlayersFreq;	//10 sec
	
	if (m_dwPrevElapsedMinutes != dwElapsedMinutes)
	{
		m_dwPrevElapsedMinutes = dwElapsedMinutes;

		UINT nTO		= 0;
		UINT nCandidate = 0;
		UINT nLoop		= 0;
		DWORD dwChaNum	= 0;
		GLMSG::SNET_CTF_SYNC_PLAYERNUM_AF msgSync;

		//DEQ_CTF_JOINERS_ITER iter;
		//DEQ_CTF_JOINERS_ITER iterEnd;
		
		GLCharAG *pChaAG = NULL;


		for (int wSchool=0; wSchool<SCHOOL_NUM; wSchool++)
		{
			nTO			= (UINT)(m_nMaxNumOfPlayers - m_setCTFPlayers[wSchool].size());
			nCandidate	= (UINT)m_deqCTFJoiners[wSchool].size();
			
			if (nTO > 0 && nCandidate > 0)
			{
				nLoop = min(nTO, nCandidate);


				for (UINT j=0; j<nLoop; j++)
				{
					dwChaNum = m_deqCTFJoiners[wSchool].front();

					if (RemoveJoiner(MAXDWORD, wSchool))
					{
						pChaAG = m_pGLAgentServer->GetCharByDbNum(dwChaNum);

						if (pChaAG == NULL)
						{
							sc::writeLogError(sc::string::format("CaptureTheFieldManAgt::DoUpdatePlayers() - Char(%1%) does not exist.", dwChaNum));
							continue;
						}

						if (pChaAG->m_nChannel != m_nChannel)
						{
							sc::writeLogError(sc::string::format("CaptureTheFieldManAgt::DoUpdatePlayers() - Char(%1%) is in another channel(%2%).", dwChaNum, pChaAG->m_nChannel));
							continue;
						}
						
						m_setCTFPlayers[wSchool].insert(dwChaNum);

						RecallOnePlayer(pChaAG);
					}
				}
			}

			msgSync.arrPlayerNum[wSchool] = (UINT)m_setCTFPlayers[wSchool].size();
		}


		//
		// FieldServer쪽에서 별도로 CTF-LandMan의 모든 유저에 대해서 loop를 돌면서
		//  숫자를 카운팅하는 것 보다는, AgentServer에서 처리해서 알려주는 것이 불필요한 load를 방지할 수 있다.
		//

		//
		//Sync the number of players to FieldServer
		//		
		m_pGLAgentServer->SENDTOFIELDSVR(m_nChannel, m_nFieldSvr, &msgSync);
	}
}


int CaptureTheFieldManAgt::Join(DWORD dwClientID, /*DWORD dwChaNum*/GLCharAG *pChaAG, BOOL bJoin, /*WORD wSchool,*/ WORD wLevel, ENUM_CTF_JOIN_RESULT emReciveJoinResult)
{	
	if (m_state != CTF_STATE_READY && m_state != CTF_STATE_START)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::Join() - CTF is not opened yet."));
		return -1;
	}

	int		nQueued	= -1;
	WORD	wSchool = SCHOOL_NONE;

	GLMSG::SNET_CTF_JOIN_RESULT_AC msgFB;
	msgFB.emJoinResult = emReciveJoinResult;

	if (!pChaAG)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::Join() - The character does not exist."));
		
		msgFB.emJoinResult	= CTF_JOIN_NO_CHAR;
		
		goto label_join_end;
	}

	wSchool = pChaAG->m_wSchool;

	GASSERT(wSchool < SCHOOL_NUM);


	if(pChaAG->m_nChannel != m_nChannel)
	{
		sc::writeLogError(sc::string::format("CaptureTheFieldManAgt::Join() - The character(%1%) is in another channel(%2%).", pChaAG->ChaDbNum(), pChaAG->m_nChannel));
					
		msgFB.emJoinResult	= CTF_JOIN_WRONG_CHANNEL;
		goto label_join_end;
	}


	if (wLevel < m_wRequiredLevel)
	{
		sc::writeLogWarn(sc::string::format("CaptureTheFieldManAgt::Join() - The character(%1%)'s level(%2%) is lower than the requirement(%3%).", pChaAG->ChaDbNum(), pChaAG->m_dwUserLvl, m_wRequiredLevel));
		
		msgFB.emJoinResult	= CTF_JOIN_WRONG_LEVEL;
		goto label_join_end;
	}

	if (PVESystem::FindInstantDungeon(pChaAG->ChaDbNum()))
	{
		msgFB.emJoinResult	= CTF_JOIN_WRONG_PVE;
		goto label_join_end;
	}

    if ( GLUseFeatures::GetInstance().IsUsingTournament() )
    {
        if ( MatchingSystem::MatchingCore::getinstance()->IsAreadyJoin(pChaAG->ChaDbNum()) )
        {
            msgFB.emJoinResult	= CTF_JOIN_WRONG_MATCHING_CONTENTS;
            goto label_join_end;
        }
    }

	SMAPNODE* pMapNode = m_pGLAgentServer->FindMapNode( pChaAG->GetCurrentMap() );
	if ( pMapNode && pMapNode->IsInstantMap() )
	{
		msgFB.emJoinResult	= CTF_JOIN_WRONG_PVE;
		goto label_join_end;
	}

	if (bJoin)
	{
		m_deqCTFJoiners[wSchool].push_back(pChaAG->ChaDbNum());

		nQueued = max((int)(m_deqCTFJoiners[wSchool].size() - m_nMaxNumOfPlayers), 0);
	}
	else
	{
		RemoveJoiner(pChaAG->ChaDbNum(), wSchool);
	}

label_join_end:

	msgFB.bJoin			= bJoin;
	msgFB.nQueued		= nQueued;

	m_pGLAgentServer->SENDTOCLIENT(dwClientID, &msgFB);

	
	return nQueued;
}

void CaptureTheFieldManAgt::ForceOut( GLCharAG *pChaAG )
{
	if( pChaAG == NULL ) return;

	switch( m_state )
	{
	case CTF_STATE_READY :
		{
			if( RemoveJoiner( pChaAG->ChaDbNum(), pChaAG->m_wSchool ) )
			{
				GLMSG::SNET_CTF_JOIN_RESULT_AC msgFB;

				msgFB.bJoin		   = FALSE;
				msgFB.emJoinResult = CTF_JOIN_OK;
				msgFB.nQueued      = 0;
				m_pGLAgentServer->SENDTOCLIENT(pChaAG->ClientSlot(), &msgFB);
			}
		}
		break;
	case CTF_STATE_START :
		{
			KickForce( pChaAG );
		}
		break;
	}	
}

BOOL CaptureTheFieldManAgt::RemoveJoiner(DWORD dwChaNum, WORD wSchool)
{
	DEQ_CTF_JOINERS_ITER iterBegin	= m_deqCTFJoiners[wSchool].begin();
	DEQ_CTF_JOINERS_ITER iterEnd	= m_deqCTFJoiners[wSchool].end();
	DEQ_CTF_JOINERS_ITER iter;

	if (dwChaNum == MAXDWORD)
	{
		iter = iterBegin;
	}
	else
	{
		iter = std::find(iterBegin, iterEnd, dwChaNum);
	}

	if (iter == iterEnd)
	{
		//sc::writeLogWarn(sc::string::format("CaptureTheFieldManAgt::Join() - Not joined char(%1%) requested to cancel.", dwChaNum));

		return FALSE;
	}

	iter = m_deqCTFJoiners[wSchool].erase(iter);

	if (iter == m_deqCTFJoiners[wSchool].end())
	{
		return TRUE;
	}

	//
	// In case of non-random-accecss iterators, std::distance() could make some more load.
	//  Therefore, it would be better to avoid this operation if it is not mandatory.
	//
	//nQueued = (UINT)std::distance(iterBegin, iter);
	UINT nQueued = (UINT)(iter - iterBegin);

	GLCharAG *pChaAG = NULL;
	GLMSG::SNET_CTF_UPDATE_QUEUE_AC msgUpdate;

	for (; iter != m_deqCTFJoiners[wSchool].end(); ++iter)
	{
		msgUpdate.nQueued = ++nQueued <= m_nMaxNumOfPlayers ? 0 : nQueued - m_nMaxNumOfPlayers;

		pChaAG = m_pGLAgentServer->GetCharByDbNum(*iter);

		//
		//Just ignore non-exist characters here.
		// All of these characters will be handled at the start-time.
		//
		if(pChaAG == NULL)
			continue;

		if(pChaAG->m_nChannel != m_nChannel)
			continue;

		//
		// The client will calculate new order based on its previous order.
		//
		m_pGLAgentServer->SENDTOCLIENT(pChaAG->ClientSlot(), &msgUpdate);
	}

	return TRUE;
}


void CaptureTheFieldManAgt::Start(UINT nFieldSvr)
{
	m_nFieldSvr = nFieldSvr;

	//
	//1. Recall all joiners into CTF map.
	//
	RecallPlayers();


	//
	//2. Let's start it.
	//
	GLMSG::SNET_CTF_START_AF msgStartAF;
	m_pGLAgentServer->SENDTOCHANNEL(&msgStartAF, m_nChannel);

	GLMSG::SNET_CTF_START_2ALL_AC msgStartAC;
	msgStartAC.tStartTime = (__time32_t)m_ctStartTime.GetTime();
	m_pGLAgentServer->SENDTOALLCLIENT(&msgStartAC);
}

/*
void CaptureTheFieldManAgt::Sync(DWORD dwClientID)
{
	GLMSG::SNET_CTF_SYNC_REQ_CA_FB msg;

	msg.state = (UINT)m_state;

	if (m_state == CTF_STATE_READY)
	{
		DWORD dwElapsedMinutes = (m_dwCurrentTime - m_dwBaseTime) / 60000;	//60 sec
		msg.nRemainMinutes	= m_nReadyDurationM - dwElapsedMinutes;
	}
	else
	{
		msg.nRemainMinutes	= 0;
	}

	m_pGLAgentServer->SENDTOCLIENT(dwClientID, &msg);
}
*/


BOOL CaptureTheFieldManAgt::KickOutNonPlayers()
{
	if(!m_pLandManPvE)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::KickOutNonJoiners() - CTF PvE-land does not exist."));
		return FALSE;
	}

	if(!m_pLandManPvP)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::KickOutNonJoiners() - CTF PvP-land does not exist."));
		return FALSE;
	}

    const GLAGLandMan::SETPC& PcList = m_pLandManPvE->GetChar();
	GLAGLandMan::SETPC_CITER iter	 = PcList.begin();
	GLAGLandMan::SETPC_CITER iterEnd = PcList.end();
	
	GLCharAG *pChaAG = NULL;

	for (; iter != iterEnd; ++iter)
	{
		pChaAG = m_pGLAgentServer->GetCharByDbNum(*iter);	//DbNum

		if (!pChaAG)
			continue;

		if (pChaAG->m_nChannel == m_nChannel)
		{
			if( m_setCTFPlayers[pChaAG->m_wSchool].count(pChaAG->ChaDbNum()) > 0 )	//this character is one of CTF players.
				continue;
		}

		KickOut(pChaAG);
	}

	//
	// LandManPvP에는 어떠한 유저도 존재해서는 안된다.
	//  하지만 한번 체크는 해두자.
	//

	if (m_pLandManPvP->CharCount() > 0)
	{
		sc::writeLogError(std::string("CTF: Some character(s) exist(s) in PvP-land !!! Check it out!!"));

        const GLAGLandMan::SETPC& PcList2 = m_pLandManPvP->GetChar();
		GLAGLandMan::SETPC_CITER iter	 = PcList2.begin();
		GLAGLandMan::SETPC_CITER iterEnd = PcList2.end();
		
		GLCharAG *pChaAG = NULL;

		for (; iter != iterEnd; ++iter)
		{
			pChaAG = m_pGLAgentServer->GetCharByDbNum(*iter);	//DbNum

			if (!pChaAG)
				continue;
					
			if( m_setCTFPlayers[pChaAG->m_wSchool].count(pChaAG->ChaDbNum()) > 0 )	//this character is one of CTF players.
				continue;

			KickOut(pChaAG);
		}
	}

	return TRUE;
}


void CaptureTheFieldManAgt::KickOut(GLCharAG *pChaAG)
{
	if (!pChaAG)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::KickOut() - pChaAG is NULL."));
		return;
	}

	WORD wSchool = pChaAG->GetSchool();
	if ( wSchool >= SCHOOL_NUM )
		wSchool = SCHOOL_SM;
	
	const DWORD dwCurrentGaeaMapID(pChaAG->GetCurrentMap().getGaeaMapID().Id());
	if ( dwCurrentGaeaMapID == m_dwPVEMapID || dwCurrentGaeaMapID == m_dwPVPMapID )	
	{
		GLMSG::SNET_CTF_RECALL_TO_AF msg;
		msg.idMap		= m_dwMapIDKickOut[ wSchool ] ;
		msg.dwGateID	= m_dwGateKickOut[ wSchool ];
		msg.dwChaNum	= pChaAG->ChaDbNum();

		//
		// Request to Recall to the specific map's gate.
		//
		m_pGLAgentServer->SENDTOFIELD(pChaAG->ClientSlot(), &msg);
	}

	if( pChaAG->m_bCTFPlayer == false )
	{
		sc::writeLogInfo(sc::string::format("[CTF] CaptureTheFieldManAgt::KickOut ERROR. DbNum : %1%", pChaAG->ChaDbNum()));
		return;
	}	

    pChaAG->m_bCTFPlayer = false;
}


void CaptureTheFieldManAgt::RecallPlayers()
{
	PGLCHARAG pChaAG = NULL;

	for (int i=0; i<SCHOOL_NUM; i++)
	{
		SET_CTF_PLAYERS_ITER iter = m_setCTFPlayers[i].begin();

		for (; iter != m_setCTFPlayers[i].end();)
		{
			pChaAG = m_pGLAgentServer->GetCharByDbNum(*iter);

			if (pChaAG == NULL)
			{
				sc::writeLogError(sc::string::format("CaptureTheFieldManAgt::RecallPlayers() - Char(%1%) does not exist.", *iter));

				++iter;

				continue;
			}

			if (pChaAG->m_nChannel == m_nChannel)
			{
				//
				//recall
				//
				RecallOnePlayer(pChaAG);
				
				++iter;
			}
			else
			{
				sc::writeLogError(sc::string::format("CaptureTheFieldManAgt::RecallPlayers() - The character(%1%) is in another channel(%2%).", pChaAG->ChaDbNum(), pChaAG->m_nChannel));

				m_setCTFPlayers[i].erase(iter++);
			}
		}
	}
}


BOOL CaptureTheFieldManAgt::RecallOnePlayer(GLCharAG *pChaAG)
{
	if(!pChaAG)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::RecallOnePlayer() - Character does not exist."));
		return FALSE;
	}
	
	if(pChaAG->m_nChannel != m_nChannel)
	{
		sc::writeLogError(sc::string::format("CaptureTheFieldManAgt::RecallOnePlayer() - Char(%1%) is in another channel(%2%).", pChaAG->ChaDbNum(), pChaAG->m_nChannel));
		return FALSE;
	}

	//
	//First, quit the party if the character is in a party.
	//
	QuitParty(pChaAG);

	GLMSG::SNET_CTF_RECALL_TO_AF msg;
	msg.idMap		= m_dwPVPMapID;		//<--------------- to PvP map
	msg.dwGateID	= m_dwGateRevival[pChaAG->m_wSchool];
	msg.dwChaNum	= pChaAG->ChaDbNum();

	if( pChaAG->m_bCTFPlayer == TRUE )
	{
		sc::writeLogInfo(sc::string::format("[CTF] CaptureTheFieldManAgt::RecallOnePlayer ERROR. DbNum : %1%", pChaAG->ChaDbNum()));
		return FALSE;
	}	


	pChaAG->m_bCTFPlayer = TRUE;

	//
	//각 캐릭터가 속한 FieldServer로 Recall 요청
	// 이 때, 모든 캐릭터들은 CTF_CHANNEL(0)으로 접속해서 신청한 상태일 것이므로,
	// 다른 채널에 대해서는 고려하지 않아도 된다.
	//
	m_pGLAgentServer->SENDTOFIELD(pChaAG->ClientSlot(), &msg);

	//NotifyStart(pChaAG);	//Recall 메시지 처리 이후로 타이밍을 수정했다.

	return TRUE;
}


BOOL CaptureTheFieldManAgt::Recall4Winner(GLCharAG *pChaAG)
{
	if(!pChaAG)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::Recall4Winner() - Character does not exist."));
		return FALSE;
	}

	if(pChaAG->m_nChannel != m_nChannel)
	{
		sc::writeLogError(sc::string::format("CaptureTheFieldManAgt::Recall4Winner() - Char(%1%) is in another channel(%2%).", pChaAG->ChaDbNum(), pChaAG->m_nChannel));
		return FALSE;
	}

	const DWORD dwCurrentGaeaMapID(pChaAG->GetCurrentMap().getGaeaMapID().Id());
	if ( dwCurrentGaeaMapID == m_dwPVPMapID )	
	{
		GLMSG::SNET_CTF_RECALL_TO_AF msg;
		msg.idMap		= m_dwPVEMapID;		//<--------------- to PvE map
		msg.dwGateID	= m_dwGateStart;
		msg.dwChaNum	= pChaAG->ChaDbNum();

		//
		//각 캐릭터가 속한 FieldServer로 Recall 요청
		// 이 때, 모든 캐릭터들은 CTF_CHANNEL(0)으로 접속해서 신청한 상태일 것이므로,
		// 다른 채널에 대해서는 고려하지 않아도 된다.
		//
		m_pGLAgentServer->SENDTOFIELD(pChaAG->ClientSlot(), &msg);
	}

	return TRUE;
}


void CaptureTheFieldManAgt::QuitParty(GLCharAG *pChaAG)
{
	if(pChaAG)
	{
		m_pGLAgentServer->QuitParty(pChaAG, false);
	}
}

/*
void CaptureTheFieldManAgt::NotifyStart(GLCharAG *pChaAG)
{
	if (!pChaAG)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::NotifyStart() - pChar is NULL."));
		return;
	}
	
	GLMSG::SNET_CTF_START_2PLAYERS_AC msgAC;
	m_pGLAgentServer->SENDTOCLIENT(pChaAG->ClientSlot(), &msgAC);
}
*/

//
//called by Quit button on the CTF-UI.
//
void CaptureTheFieldManAgt::Quit(DWORD dwChaNum, WORD wSchool)
{	
	RemovePlayer(dwChaNum, wSchool);
	
	GLCharAG *pChaAG = NULL;
	
	pChaAG = m_pGLAgentServer->GetCharByDbNum(dwChaNum);

	if (!pChaAG)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::Quit()#0 - The character does not exist."));
		return;
	}

	if( pChaAG->m_bCTFPlayer == FALSE )
	{
		sc::writeLogInfo(sc::string::format("[CTF] aptureTheFieldManAgt::Quit ERROR. DbNum : %1%", pChaAG->ChaDbNum()));
		return;
	}	

	pChaAG->m_bCTFPlayer = FALSE;

	if (IsRewarding() && (pChaAG->m_wSchool == m_wSchoolAuthed))
	{
		Recall4Winner(pChaAG);
	}
	else
	{
		KickOut(pChaAG);
	}    

    GLMSG::SNET_CTF_QUIT_AF msgAF;
    msgAF.dwChaNum	= dwChaNum;
	msgAF.state		= m_state;
    m_pGLAgentServer->SENDTOFIELDSVR(m_nChannel, m_nFieldSvr, &msgAF);
}

void CaptureTheFieldManAgt::KickForce( GLCharAG *pChaAG )
{
	if( pChaAG == NULL || !pChaAG->m_bCTFPlayer ) return;

	WORD  wSchool  = pChaAG->m_wSchool;
	DWORD dwChaNum = pChaAG->ChaDbNum();

	SET_CTF_PLAYERS_ITER iter = m_setCTFPlayers[wSchool].find(dwChaNum);

	if( iter != m_setCTFPlayers[wSchool].end() )
	{
		RemovePlayer(dwChaNum, wSchool);

		pChaAG->m_bCTFPlayer = FALSE;

		GLMSG::SNET_CTF_QUIT_AF msgAF;
		msgAF.dwChaNum	= dwChaNum;
		msgAF.state		= m_state;
		m_pGLAgentServer->SENDTOFIELDSVR(m_nChannel, m_nFieldSvr, &msgAF);
	}
}

//
//called by destructor of GLCharAG when a player disconnected.
//
void CaptureTheFieldManAgt::RemovePlayer(DWORD dwChaNum, WORD wSchool)
{
	UINT cnt = (UINT)m_setCTFPlayers[wSchool].erase(dwChaNum);

	if (cnt <= 0)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::Quit()#1 - The character is NOT a CTF-Player."));
		return;
	}
}


void CaptureTheFieldManAgt::AuthedBy(WORD wSchool)
{
	m_wSchoolAuthed = wSchool;

	sc::writeLogInfo(sc::string::format("CTF: School#%1% has captured the field!", m_wSchoolAuthed));
}


ENUM_CTF_STATE CaptureTheFieldManAgt::MoveStateByGM()
{
	ENUM_CTF_STATE stateAfter = CTF_STATE_STOP;
	CTimeSpan cTimeSpan;

	switch(GetState())
	{
	case CTF_STATE_READY:	//--> CTF_STATE_START
		m_ctStartTime	= CTime::GetCurrentTime();
		cTimeSpan		= CTimeSpan( 0, 0, 0, m_ctStartTime.GetSecond() );
		m_ctStartTime	-= cTimeSpan;
		m_ctReadyTime	= m_ctStartTime - m_ctsReadyDurationM;
		m_ctStopTime	= m_ctStartTime + m_ctsPlayTimeM;
		m_ctExtraTime	= m_ctStopTime + m_ctsRewardTimeM;

		stateAfter = CTF_STATE_START;
		break;

	case CTF_STATE_START:	//--> CTF_STATE_DONE
		m_ctStopTime	= CTime::GetCurrentTime();
		cTimeSpan		= CTimeSpan( 0, 0, 0, m_ctStopTime.GetSecond() );
		m_ctStopTime	-= cTimeSpan;
		m_ctExtraTime	= m_ctStopTime + m_ctsRewardTimeM;

		stateAfter = CTF_STATE_DONE;
		break;

	case CTF_STATE_DONE:	//--> CTF_STATE_REWARD
		m_ctExtraTime	= m_ctStopTime + m_ctsRewardTimeM;

		stateAfter = CTF_STATE_REWARD;
		break;

	case CTF_STATE_REWARD:	//--> CTF_STATE_STOP
		m_ctExtraTime	= CTime::GetCurrentTime();
		cTimeSpan		= CTimeSpan( 0, 0, 0, m_ctExtraTime.GetSecond() );
		m_ctExtraTime	-= cTimeSpan;

		stateAfter = CTF_STATE_STOP;
		break;

	case CTF_STATE_STOP:	//--> CTF_STATE_READY
		m_ctReadyTime	= CTime::GetCurrentTime();
		cTimeSpan		= CTimeSpan( 0, 0, 0, m_ctReadyTime.GetSecond() );
		m_ctReadyTime	-= cTimeSpan;
		m_ctStartTime	= m_ctReadyTime + m_ctsReadyDurationM;		
		m_ctStopTime	= m_ctStartTime + m_ctsPlayTimeM;
		m_ctExtraTime	= m_ctStopTime + m_ctsRewardTimeM;

		stateAfter = CTF_STATE_READY;
		break;

	default:
		sc::writeLogWarn(std::string("CaptureTheFieldManAgt::MoveStateByGM() - Current CTF state cannot be moved."));
		break;
	}

	GLMSG::SNET_CTF_NEXT_READY_TIME msgAF2;
	msgAF2.ctReadyTime = m_ctReadyTime;
	m_pGLAgentServer->SENDTOALLCHANNEL( &msgAF2 );

	return stateAfter;
}


void CaptureTheFieldManAgt::SetRankers(UINT nHowMany, PVP::CTF_RANKER_BASIC *arrRanking)
{
	m_history.vecRanking.clear();

	if (nHowMany > 0)
	{
		for (UINT i=0; i<nHowMany; i++)
		{
			m_history.vecRanking.push_back(arrRanking[i]);
		}
	}
}

bool CaptureTheFieldManAgt::IsRegisterMember(DWORD dwCharDbNum)
{
	if (m_pGLAgentServer == 0)
	{
		return false;
	}

	GLCharAG* pChar = m_pGLAgentServer->GetCharByDbNum(dwCharDbNum);

	if (pChar == 0)
	{
		return false;
	}

	DEQ_CTF_JOINERS_ITER itBegin = m_deqCTFJoiners[pChar->m_wSchool].begin();
	DEQ_CTF_JOINERS_ITER itEnd = m_deqCTFJoiners[pChar->m_wSchool].end();

	return std::find(itBegin, itEnd, dwCharDbNum) != itEnd;
}

bool CaptureTheFieldManAgt::LoadFromDB(db::IDbManager* pDBMan, db::IDbManager* pAdoMan)
{
	if (!pDBMan)
		return false;

	if (!pAdoMan)
	{
		sc::writeLogError(std::string("CaptureTheFieldManAgt::LoadFromDB pAdoMan==NULL"));
		return false;
	}
	std::vector<DWORD> vAuthSchool;
	int nRET = pAdoMan->GetCTFInfo(GetConsecutivelyNumber(),vAuthSchool);

	for(int i = (vAuthSchool.size()-1); i>-1; i--)
	{
		SetConsecutively(vAuthSchool[i]);
	}

	if (nRET != sc::db::DB_OK)
		return false;

    return true;
}
