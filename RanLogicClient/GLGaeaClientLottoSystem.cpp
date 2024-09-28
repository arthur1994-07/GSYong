#include "pch.h"

#include "./LottoSystem/GLLottoSystemManClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::MsgLottoSystem( NET_MSG_GENERIC* pMsg )
{
	if( m_pLottoSystemMan )
		m_pLottoSystemMan->OnMessage( static_cast<GLMSG::NET_LOTTO_MESSAGE_HEADER*>( pMsg ), 0, 0 );
}