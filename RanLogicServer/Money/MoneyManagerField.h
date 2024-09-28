#pragma once

#include "../../RanLogic/Msg/GLContrlLogMsg.h"

#include "../../RanLogic/Money/IMoneyManager.h"

class GLGaeaServer;

namespace Money
{

	class CMoneyManagerField : public IMoneyManager
	{
	public:
		virtual void CALLBACK			OnSaveChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const LONGLONG _llChangeMoney );
		virtual void CALLBACK			OnSaveChangeMapMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney );

	public:
		/// Invoke Function;
		// ������ Callback �� �Ͼ��;

	private:
		/// Inner Function;
		void							_SyncCacheChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney );

		// ���� Cache Server �ϰ� ����Ѵ�;
		void							_SendToCache ( NET_MSG_GENERIC* _pMsg );


	private:
		// Field Server;
		GLGaeaServer*					m_pServer;


	public:
		CMoneyManagerField ( GLGaeaServer* _pServer, const float _fUpdateMoneyTime )
			: IMoneyManager ( _fUpdateMoneyTime )
			, m_pServer ( _pServer ) { }
		~CMoneyManagerField ( ) { }
	};

}