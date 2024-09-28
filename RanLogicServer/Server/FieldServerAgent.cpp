#include "../pch.h"
#include "../../Enginelib/Common/profile.h"
#include "../../RanLogic/Msg/PointShopMsg.h"

#include "../Client/NetFieldClientMan.h"
#include "../FieldServer/GLGaeaServer.h"
#include "./FieldServerConfigXml.h"
#include "./NetServerHelper.h"
#include "./s_CFieldServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CFieldServer::AgentMsgProcess(MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

    DWORD dwClient = pMsg->dwClient;
    DWORD dwGaeaID = GAEAID_NULL;

    if ( m_pClientManager->IsClientSlot(dwClient) )
        return;

    NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*)(pMsg->Buffer);
    EMNET_MSG emType = nmg->Type();

    switch (emType)
    {
//     case NET_MSG_GCTRL_TRADE_TAR_ANS:
//         m_pGaeaServer->RequestTradeTarAns(dwClient, dwGaeaID, nmg);
//         break;
	case NET_MSG_REFERENCE_COUNT_CAF:
		MsgReferenceCountAF( dwClient, nmg );
		break;

	case NET_MSG_GCTRL_COUNTRY_CHANGE_AFC:
		MsgUpdateUserCountryAF( dwClient, nmg );
		break;
    }
}