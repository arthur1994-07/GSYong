#include "../pch.h"
#include "../Guidance/GuidanceField.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaServer::FrameMoveGuidance(float fElapsedTime)
{
    m_pGuidance->FrameMove(fElapsedTime);
}

GLGuidanceFieldMan* GLGaeaServer::GuidanceMan()
{
    return m_pGuidance;
}

GLGuidance* GLGaeaServer::GuidanceFind(DWORD MapId)
{
    return m_pGuidance->Find(MapId);
}

DWORD GLGaeaServer::GuidanceId(DWORD ClubId)
{
    return m_pGuidance->GetGuidID(ClubId);
}

EMCHECKCERTIFY GLGaeaServer::GuidanceCheckCertify(DWORD dwID, DWORD dwCHARID)
{
    return m_pGuidance->CheckCertify(dwID, dwCHARID);
}
 
void GLGaeaServer::GuidanceDoCertify(DWORD dwID, DWORD dwCHARID, DWORD dwNpcGlobID, const D3DXVECTOR3 &vPOS)
{
    m_pGuidance->DoCertify(dwID, dwCHARID, dwNpcGlobID, vPOS);
}

const std::string GLGaeaServer::GuidanceGetName(DWORD Id)
{
    return m_pGuidance->GetName(Id);
}

void GLGaeaServer::GuidanceSetState(VECGUID_DB& vecGUID)
{
    m_pGuidance->SetState(vecGUID);
}

void GLGaeaServer::SendWorldBattleNationFlag( GLChar* pChar )
{
	m_pGuidance->SendWorldBattleNationFlag( pChar );
}

bool GLGaeaServer::IsGuidanceBattle() const
{
    return m_pGuidance->IsBattle();
}