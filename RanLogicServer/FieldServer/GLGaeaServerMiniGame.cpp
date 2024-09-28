#include "../pch.h"

#include "../../SigmaCore/gassert.h"

#include "../MiniGame/GLMiniGameManager.h"
#include "GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaServer::FrameMoveMiniGame(float fTime, float fElapsedTime)
{
	// LevelEdit ���� m_pMiniGameManager�� NULL �̴�.
	if ( m_pMiniGameManager )
	{
		m_pMiniGameManager->FrameMove(fTime, fElapsedTime);
	}
}

BOOL GLGaeaServer::MsgMiniGame(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	return m_pMiniGameManager->MessageProcedure(nmg, dwGaeaID) ? TRUE : FALSE;
}