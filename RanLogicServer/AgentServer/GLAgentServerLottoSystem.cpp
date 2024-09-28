#include "pch.h"

#include "./GLAgentServer.h"
#include "../LottoSystem/GLLottoSystemManAgent.h"

#include "../../SigmaCore/Log/LogMan.h"

#include "../../RanLogic/GLUseFeatures.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Lotto System 초기화;
void GLAgentServer::InitializeLottoSystem()
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return;

	LottoSystem::GLLottoSystemManAgent::Instance()->Initialize();
}

// Lotto System 파괴;
void GLAgentServer::DestroyLottoSystem()
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return;

	LottoSystem::GLLottoSystemManAgent::Instance()->Destroy();
}

// Setting Lotto System Client Data ( 동기화 );
/*void GLAgentServer::LottoSystemSyncState( DWORD dwClientID, DWORD dwChaNum )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return;

	LottoSystem::GLLottoSystemManAgent::Instance()->SyncState( dwClientID, dwChaNum );
}*/

// Frame Move;
void GLAgentServer::FrameMoveLottoSystem( float fElapsedTime, const CTime& currentTime )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return;

	LottoSystem::GLLottoSystemManAgent::Instance()->Update( fElapsedTime, currentTime );
}

// Lotto System Message Procedure;
BOOL GLAgentServer::LottoSystemMsgProc( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return TRUE;

	return LottoSystem::GLLottoSystemManAgent::Instance()->OnMessage( static_cast<GLMSG::NET_LOTTO_MESSAGE_HEADER*>( nmg ), dwClientID, dwGaeaID ) ? TRUE : FALSE;
}