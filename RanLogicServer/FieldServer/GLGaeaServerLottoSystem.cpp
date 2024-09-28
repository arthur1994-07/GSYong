#include "pch.h"

#include "./GLGaeaServer.h"
#include "../LottoSystem/GLLottoSystemManField.h"

#include "../../SigmaCore/Log/LogMan.h"

#include "../../RanLogic/GLUseFeatures.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLGaeaServer::MsgLottoSystemProc( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return TRUE;

	return LottoSystem::GLLottoSystemManField::Instance()->OnMessage( static_cast<GLMSG::NET_LOTTO_MESSAGE_HEADER*>( nmg ), dwClientID, dwGaeaID ) ? TRUE : FALSE;
}