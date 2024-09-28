#include "../pch.h"

#include "./GLAgentServer.h"
#include "../CaptureTheField/CaptureTheFieldManager.h"

#include "../Party/GLPartyAgent.h"
#include "../Party/GLPartyManagerAgent.h"

#include "../../SigmaCore/Log/LogMan.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void GLAgentServer::FrameMoveCTF(float fElapsedTime)
{
    m_pCTFMan->FrameMove(fElapsedTime);
}


void GLAgentServer::CTFQuit(DWORD dwChaNum, WORD wSchool)
{
	m_pCTFMan->Quit(dwChaNum, wSchool);
}


void GLAgentServer::CTFSyncState(DWORD dwClientID)
{
	//
	// 1.현재 CTF의 상태 정보와 이전CTF에 대한 History 동기화
	//
	GLMSG::SNET_CTF_SYNC_STATUS_AC msgSync;

	msgSync.state						= (DWORD)m_pCTFMan->GetState();
	msgSync.wRequiredLevel				= m_pCTFMan->GetRequiredLevel();
	msgSync.wSchoolAuthed				= m_pCTFMan->GetAuthedSchool();			//현재 CTF의 승리학원
	msgSync.wPrevSchoolAuthed			= m_pCTFMan->GetHistory().wSchoolAuthed;//이전 CTF의 승리학원
    msgSync.tStartTime					= (__time32_t)m_pCTFMan->GetStartTime().GetTime();
	msgSync.tPrevStartTime				= (__time32_t)m_pCTFMan->GetHistory().tStartTime;
	msgSync.tPrevEndTime				= (__time32_t)m_pCTFMan->GetHistory().tEndTime;
    msgSync.nPlayTimeM					= m_pCTFMan->GetPlayTimeM();
	msgSync.nRewardTimeM				= m_pCTFMan->GetRewardTimeM();
	msgSync.nPlayCycleH					= m_pCTFMan->GetStartCycleS();
	msgSync.dwPvPMapID					= m_pCTFMan->GetPvPMapID();
	msgSync.dwPvEMapID					= m_pCTFMan->GetPvEMapID();
	msgSync.dwCertificationID1			= m_pCTFMan->GetCert1ID();
	msgSync.dwCertificationID2			= m_pCTFMan->GetCert2ID();
	msgSync.dwCertificationID3			= m_pCTFMan->GetCert3ID();
	msgSync.dwMaxPlayerNumPerSchool		= m_pCTFMan->GetMaxNumOfPlayers();

	// 연속 점령 확장.
	msgSync.dwConsecutivelyNumber		= m_pCTFMan->GetConsecutivelyNumber();
	msgSync.bConsecutivelyCapturel		= m_pCTFMan->GetConsecutivelyCapturel();
	msgSync.bisConsecutively			= m_pCTFMan->GetConsecutively();
	msgSync.bPrevConsecutively			= m_pCTFMan->GetPrevConsecutively();

	SENDTOCLIENT(dwClientID, &msgSync);


	//
	// 2.이전 CTF의 보상 정보 (History)
	//
	GLMSG::SNET_CTF_REWARD_INFO msgReward;	
	msgReward.wSchoolAuthed = m_pCTFMan->GetHistory().wSchoolAuthed;
    
    if (msgReward.SetData(m_pCTFMan->GetHistory().vecRewardBuffs))
    {
		SENDTOCLIENT(dwClientID, &msgReward);
    }

	
	//
	// 3.이전 CTF의 랭킹 정보 (History)
	//
	GLMSG::SNET_CTF_RANKING_FAC msgRanking;
	if (msgRanking.SetData(m_pCTFMan->GetRankingVector()))
	{
		SENDTOCLIENT(dwClientID, &msgRanking);
	}
}


/*
BOOL GLAgentServer::CTFCheckPlayerRecall(GLCharAG *pChaAG, DWORD dwMapID)
{
	if (!m_pCTFMan)
	{
		sc::writeLogError(std::string("GLAgentServer::CTFCheckPlayerRecall() - CTFMan is NULL."));
		return FALSE;
	}

	if (m_pCTFMan->IsStarted() && m_pCTFMan->GetPvPMapID() == dwMapID)
	{
		m_pCTFMan->NotifyStart(pChaAG);
	}

	return TRUE;
}
*/


void GLAgentServer::AddCTFRewardBuff(SNATIVEID id, WORD wLevel)
{
    if (!m_pCTFMan)
    {
        sc::writeLogError(std::string("GLAgentServer::AddCTFRewardBuff() - CTFMan is NULL."));
        return;
    }

    m_pCTFMan->AddRewardBuff(id, wLevel);
}

void GLAgentServer::ClearCTFRewardBuff()
{ 
    if (!m_pCTFMan)
    {
        sc::writeLogError(std::string("GLAgentServer::ClearCTFRewardBuff() - CTFMan is NULL."));
        return;
    }

    m_pCTFMan->ClearRewardBuff(); 
}

bool GLAgentServer::IsRegisterCTFMember(DWORD dwCharDbNum)
{ 
	if (!m_pCTFMan)
	{
		return false;
	}

	return m_pCTFMan->IsRegisterMember(dwCharDbNum);
}

UINT GLAgentServer::CTFGetChannel()
{
	if (!m_pCTFMan)
	{
		sc::writeLogError(std::string("GLAgentServer::CTFGetChannel() - CTFMan is NULL."));
		return FALSE;
	}

	return m_pCTFMan->GetChannel();
}

CTime GLAgentServer::GetCTFReadyTime()
{
	CTime ctReady( CTime::GetCurrentTime() );
	if ( m_pCTFMan )
	{
		ctReady = m_pCTFMan->GetReadyTime();
	}
	return ctReady;
}
