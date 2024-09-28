#include "../pch.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "./GLAgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLAgentServer::FrameMoveClubDeathMatch(float fElapsedTime)
{
    m_pClubDeathMatchMan->FrameMove(fElapsedTime);
}

bool GLAgentServer::ClubDeathMatchIsBattle() const
{
    if ( m_pClubDeathMatchMan->IsBattle() || m_pClubDeathMatchMan->IsBattleReady() )
    {
        return true;
    }
    return false;
}

const bool GLAgentServer::IsClubDeathMatchMap(const SNATIVEID& _baseMapID)
{
	return m_pClubDeathMatchMan->IsClubDeathMatchMap(_baseMapID);
}
