#include "pch.h"

#include "../FieldServer/GLGaeaServer.h"

#include "./MoneyManagerField.h"

namespace Money
{

	void CALLBACK CMoneyManagerField::OnSaveChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const LONGLONG _llChangeMoney )
	{
		_SyncCacheChangeMoney( _emChangeType, MAXWORD, _llChangeMoney );
	}


	void CALLBACK CMoneyManagerField::OnSaveChangeMapMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney )
	{
		_SyncCacheChangeMoney( _emChangeType, _nMapID, _llChangeMoney );
	}
	

	void CMoneyManagerField::_SyncCacheChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney )
	{
		if ( NULL == m_pServer )
			return;

		if ( _llChangeMoney <= 0 )
			return;

		const DWORD dwChannelNum = static_cast< const DWORD >( m_pServer->GetServerChannel() );
		const DWORD dwFieldNum = static_cast< const DWORD >( m_pServer->GetFieldSvrID() );

		// INVEN_MONEY_DEFAULT �� '���� - �Һ�'�� ����� ���̴�;
		// �ڵ����� ���Ǿ����Ƿ� ������ �ȵȴ�;
		if ( INVEN_MONEY_DEFAULT == _emChangeType )
			return;

		GLMSG::NET_LOG_MESSAGE_SYNC_CHANGEMONEY_LOG_FH sLogMsg(
			dwChannelNum,
			dwFieldNum,
			_emChangeType,
			_nMapID,
			_llChangeMoney );
		_SendToCache( &sLogMsg );
	}


	void CMoneyManagerField::_SendToCache ( NET_MSG_GENERIC* _pMsg )
	{
		if ( NULL == m_pServer )
			return;

		m_pServer->SENDTOCACHE( _pMsg );
	}

}