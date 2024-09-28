#include "pch.h"

#include "../Server/CacheServer.h"
#include "../Database/DBAction/DbActionLog.h"

#include "./MoneyManagerCache.h"

namespace Money
{

	void CALLBACK CMoneyManagerCache::OnMessage ( DWORD _dwClientID, GLMSG::NET_LOG_MESSAGE_HEADER* _pMsg )
	{
		if ( NULL == _pMsg )
			return;

		// 현재는 채널번호와 필드번호를 사용하지 않는다;
		// 나중에 따로 구분해서 표시할 필요가 있다면 사용하자;
		const DWORD dwChannelNum = _pMsg->m_dwChannelNum;
		const DWORD dwFieldNum = _pMsg->m_dwFieldNum;

		switch ( _pMsg->nType )
		{
		case NET_MSG_GCTRL_LOG_CHANGEMONEY_FH:
			{
				GLMSG::NET_LOG_MESSAGE_SYNC_CHANGEMONEY_LOG_FH* pLogMsg = 
					static_cast< GLMSG::NET_LOG_MESSAGE_SYNC_CHANGEMONEY_LOG_FH* >( _pMsg );
				if ( pLogMsg )
				{
					const EMGAME_MONEY_CHANGE emMoneyType = pLogMsg->m_emMoneyType;
					const int nMapID = pLogMsg->m_nMapID;
					const LONGLONG llMoney = pLogMsg->m_llChangeMoney;

					if ( MAXWORD == nMapID )
						OnChangeMoney( emMoneyType, llMoney );
					else
						OnChangeMapMoney( emMoneyType, nMapID, llMoney );
				}
			}
			break;
		}
	}


	void CALLBACK CMoneyManagerCache::OnSaveChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const LONGLONG _llChangeMoney )
	{
		_DBActionChangeMoney( _emChangeType, MAXWORD, _llChangeMoney );
	}


	void CALLBACK CMoneyManagerCache::OnSaveChangeMapMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney )
	{
		_DBActionChangeMoney( _emChangeType, _nMapID, _llChangeMoney );
	}


	void CMoneyManagerCache::_DBActionChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney )
	{
		if ( NULL == m_pServer )
			return;

		sc::writeLogInfo ( sc::string::format(
			"[ Money ] [ Change Money Log ( Type : %1%, MapID : %2%, Money %3% ) ]",
			_emChangeType,
			_nMapID,
			_llChangeMoney ) );

		m_pServer->AddLogAdoJob(
			db::DbActionPtr( new db::CMoneyLogInsertAction(
			static_cast< int >( _emChangeType ),
			_nMapID,
			_llChangeMoney ) ) );
	}

}