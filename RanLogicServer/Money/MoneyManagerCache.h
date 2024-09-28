#pragma once

#include "../../RanLogic/Msg/GLContrlLogMsg.h"

#include "../../RanLogic/Money/IMoneyManager.h"

class CacheServer;

namespace Money
{

	class CMoneyManagerCache : public IMoneyManager
	{
	public:
		void CALLBACK					OnMessage ( DWORD _dwClientID, GLMSG::NET_LOG_MESSAGE_HEADER* _pMsg );


	public:
		virtual void CALLBACK			OnSaveChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const LONGLONG _llChangeMoney );
		virtual void CALLBACK			OnSaveChangeMapMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney );


	public:
		/// Invoke Function;
		// 오로지 Callback 만 일어난다;

	private:
		/// Inner Function;
		void							_DBActionChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney );


	private:
		// Field Server;
		CacheServer*					m_pServer;


	public:
		CMoneyManagerCache ( CacheServer* _pServer, const float _fUpdateMoneyTime )
			: IMoneyManager ( _fUpdateMoneyTime )
			, m_pServer ( _pServer ) { }
		~CMoneyManagerCache ( ) { }
	};

}