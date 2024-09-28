#include "pch.h"

#include "./GLGaeaClient.h"
#include "../RanLogicClient/Product/GLProductClient.h"

void GLGaeaClient::MsgProductProcess( NET_MSG_GENERIC* pMsg )
{
    m_pProductClient->ProductMsgProcess( pMsg );
}