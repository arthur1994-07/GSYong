#include "../pch.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaServer::LoadClubDeathMatch(const std::vector<std::string>& vecFile, bool bToolMode)
{
    m_pDeathMatchMan = new GLClubDeathMatchFieldMan(this);
    m_pDeathMatchMan->Load(vecFile, bToolMode);
	m_pDeathMatchMan->SetMapState();
}

void GLGaeaServer::FrameMoveClubDeathMatch(float fElapsedAppTime)
{
    // [2011.04.08] shhan
    // Tool에서 이 함수까지 들어오는데, m_pDeathMatchMan 포인터가 NULL이다.
    if ( !m_pDeathMatchMan )
        return;

    m_pDeathMatchMan->FrameMove(fElapsedAppTime);
}

bool GLGaeaServer::IsClubDeathMatch(DWORD MapId)
{
    GLClubDeathMatch* pCDM = m_pDeathMatchMan->Find(MapId);
    if (!pCDM)
        return false;
    else
        return pCDM->IsBattle();
}

DWORD GLGaeaServer::ClubDeathMatchAccrueCount(DWORD MapId, DWORD CharId)
{
    return m_pDeathMatchMan->GetAccrueCount(MapId, CharId);
}

GLClubDeathMatch* GLGaeaServer::ClubDeathMatchFind(DWORD MapId)
{
    return m_pDeathMatchMan->Find(MapId);
}

void GLGaeaServer::ClubDeathMatchAddCdmScore(GLClubDeathMatch* pCDM, DWORD dwKillClub, DWORD dwDeathClub, DWORD dwKillChar, DWORD dwDeathChar)
{
    m_pDeathMatchMan->AddCDMScore(pCDM, dwKillClub, dwDeathClub, dwKillChar, dwDeathChar);
}

BOOL GLGaeaServer::ServerClubDeathMatchReady(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_DEATHMATCH_READY_FLD *pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_READY_FLD *)nmg;
    m_pDeathMatchMan->ReadyBattle(pNetMsg->dwID);
    return TRUE;
}

BOOL GLGaeaServer::ServerClubDeathMatchStart(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_DEATHMATCH_START_FLD *pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_START_FLD *)nmg;
    m_bClubDMStarted = true;
    m_pDeathMatchMan->BeginBattle(pNetMsg->dwID);
    return TRUE;
}

BOOL GLGaeaServer::ServerClubDeathMatchEnd(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_DEATHMATCH_END_FLD *pNetMsg = (GLMSG::SNET_CLUB_DEATHMATCH_END_FLD *)nmg;
    m_bClubDMStarted = false;
    m_pDeathMatchMan->EndBattle(pNetMsg->dwID);
    return TRUE;
}