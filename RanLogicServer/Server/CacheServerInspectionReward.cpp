#include "pch.h"
#include "./CacheServer.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/DateTime.h"

#include "../../RanLogic/Msg/ServerManagerMsg.h"

#include "../../RanLogicServer/Server/NetCacheClientMan.h"
#include "../../RanLogicServer/Database/DBAction/DbActionLog.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CacheServer::MsgInspectionRewardSet( DWORD dwClient, NET_MSG_GENERIC* pMsg )
{
    int nSlotType = m_pClientManager->GetSlotType( dwClient );

	if ( nSlotType !=  net::SLOT_SESSION )
	{
		// 해킹.
		sc::writeLogError( sc::string::format( "Hack Message Detect! IP:[%s], SlotNum:[%d]" , m_pClientManager->GetClientIP(dwClient), dwClient) );
		return;
	}


	GLMSG::NET_INSPECTION_REWARD_SET_REQ* pNetMsg = (GLMSG::NET_INSPECTION_REWARD_SET_REQ*)pMsg;
	if ( pNetMsg->nServerGroup != ServerGroup() )
		return;
	
	// 로그
	sc::writeLogInfo( sc::string::format("Inspection Reward Cache -> DB [ServerGroup:%d, RewardMinute:%d]" , pNetMsg->nServerGroup, pNetMsg->nRewardTime ) );
	sc::writeLogInfo( sc::string::format("Inspection Reward Server Group:[%d] ", pNetMsg->nServerGroup ) );
	sc::writeLogInfo( sc::string::format("Inspection Reward Start Time:[%s] ", sc::time::DateTimeFormat(pNetMsg->tStart, true, true) ));
	sc::writeLogInfo( sc::string::format("Inspection Reward End   Time:[%s] ", sc::time::DateTimeFormat(pNetMsg->tEnd, true, true) ));
	sc::writeLogInfo( sc::string::format("Inspection Reward Minte:[%d] ", pNetMsg->nRewardTime ) );

	AddLogAdoJob( 
		db::DbActionPtr( 
			new db::LogInspectionRewardPolicyInsert( pNetMsg->tStart, pNetMsg->tEnd, pNetMsg->nRewardTime )
		));

	

		

}
