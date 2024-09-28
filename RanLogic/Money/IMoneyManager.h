#pragma once

#include "./MoneyLogDefine.h"

namespace Money
{

	class IMoneyManager
	{
	public:
		/// Callback Function;
		virtual void CALLBACK			OnInitialize ( void );
		virtual void CALLBACK			OnReset ( void );
		virtual void CALLBACK			OnUpdate ( const float _fElapsedTime );
		virtual void CALLBACK			OnStop ( void );
		virtual void CALLBACK			OnSave ( void );

		virtual void CALLBACK			OnChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const LONGLONG _llChangeMoney );
		virtual void CALLBACK			OnChangeMapMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney );


	public:
		virtual void CALLBACK			OnSaveChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const LONGLONG _llChangeMoney ) = 0;
		virtual void CALLBACK			OnSaveChangeMapMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney ) = 0;


	protected:
		// 인벤토리 이동 금액을 저장한다;
		LONGLONG						m_arrChangeMoney[ GAME_MONEY_CHANGE_MAX ];
		LONGLONG						m_arrTempChangeMoney[ GAME_MONEY_CHANGE_MAX ];

		// 맵 별로 생산되는 금액을 저장한다;
		VEC_MAP_MONEY_LOG				m_vecMapChangeMoney;
		VEC_MAP_MONEY_LOG				m_vecTempMapChangeMoney;

		// 동기화 시점에 락을 걸고 임시저장공간에 저장한다;
		bool							m_bLock;


	public:
		IMoneyManager ( const float _fUpdateMoneyTime ) : m_bLock ( false ), UPDATE_MONEY_TIME ( _fUpdateMoneyTime )
		{
			OnInitialize();
		}
		~IMoneyManager ( ) { }


	protected:
		const float				UPDATE_MONEY_TIME;
	};

}