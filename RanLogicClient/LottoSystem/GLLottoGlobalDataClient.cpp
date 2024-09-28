#include "pch.h"

#include "./GLLottoGlobalDataClient.h"
#include "./GLLottoSystemManClient.h"

namespace LottoSystem
{

	void GLLottoGlobalDataClient::SyncGlobalValue( GLMSG::NET_LOTTO_MESSAGE_SYNC_GLOBAL_VALUE_AC* pSyncMsg )
	{
		if( pSyncMsg->m_emAC == LottoSystem::EMLOTTO_REQ_SYNC_STATUS_AC_OK )
		{
#ifdef _EVENT_BOX_MESSAGE_LOG
			GLLottoSystemManClient::Instance()->ConsoleLog(
				"SYNC_GLOBAL_VALUE" );
#endif

			m_lottoGlobalValue = pSyncMsg->m_golbalValue;
		}
	}

	void GLLottoGlobalDataClient::SyncStatus( GLMSG::NET_LOTTO_MESSAGE_SYNC_STATUS_AC* pSyncMsg )
	{
		// 올바른 동기화 패킷;
		if( pSyncMsg->m_emAC == LottoSystem::EMLOTTO_REQ_SYNC_STATUS_AC_OK )
		{
			unsigned int id = pSyncMsg->m_lottoSystemId;
			if( id >= 0 && id < MAX_LOTTO_SYSTEM )
			{
#ifdef _EVENT_BOX_MESSAGE_LOG
				GLLottoSystemManClient::Instance()->ConsoleLog(
					sc::string::format( "SYNC_SYSTEM_VALUE %u", id ) );
#endif

				m_lottoSystemValues[ id ] = pSyncMsg->m_systemValue;
				m_systemCount++;
			}
		}
	}

}