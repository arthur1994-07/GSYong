#include "pch.h"

#include "../../SigmaCore/Log/LogMan.h"

#include "./IMoneyManager.h"

namespace Money
{

	void CALLBACK IMoneyManager::OnInitialize ( void )
	{
		for ( int i=0; i<GAME_MONEY_CHANGE_MAX; ++i )
		{
			m_arrChangeMoney[ i ] = 0;
			m_arrTempChangeMoney[ i ] = 0;
		}

		m_vecMapChangeMoney.clear();
		m_vecTempMapChangeMoney.clear();

		m_bLock = false;
	}


	void CALLBACK IMoneyManager::OnReset ( void )
	{
		// 기존 저장공간의 내용을 초기화한다;
		for ( int i=0; i<GAME_MONEY_CHANGE_MAX; ++i )
		{
			m_arrChangeMoney[ i ] = 0;
		}

		m_vecMapChangeMoney.clear();

		// 임시 저장공간의 내용을 가져온다;
		for ( int i=0; i<GAME_MONEY_CHANGE_MAX; ++i )
		{
			m_arrChangeMoney[ i ] = m_arrTempChangeMoney[ i ];
		}

		for ( int i=0; i<m_vecTempMapChangeMoney.size(); ++i )
		{
			SMAP_MONEY_LOG sTempMoneyLog = m_vecTempMapChangeMoney.at( i );
			m_vecMapChangeMoney.push_back( sTempMoneyLog );
		}

		// 임시 저장공간을 초기화한다;
		for ( int i=0; i<GAME_MONEY_CHANGE_MAX; ++i )
		{
			m_arrTempChangeMoney[ i ] = 0;
		}

		m_vecTempMapChangeMoney.clear();

		// 락을 해제한다;
		m_bLock = false;
	}


	void CALLBACK IMoneyManager::OnUpdate ( const float _fElapsedTime )
	{
		// Log 이기 때문에 UPDATE_MONEY_TIME 마다 한번씩 동기화해주는 걸로 만족한다;
		// 즉, Field Down 시 UPDATE_MONEY_TIME 만큼의 오차가 발생할 수 있다;
		static float fTime = 0.f;
		fTime += _fElapsedTime;
		if ( fTime >= UPDATE_MONEY_TIME )
		{
			fTime = 0.f;

			m_bLock = true;
		}
		else
			return;

		OnSave();
		OnReset();
	}


	void CALLBACK IMoneyManager::OnStop ( void )
	{
		OnSave();
		OnReset();
	}


	void CALLBACK IMoneyManager::OnSave ( void )
	{
		for ( int i=0; i<GAME_MONEY_CHANGE_MAX; ++i )
		{
			if ( 0 == m_arrChangeMoney[ i ] )
				continue;

			OnSaveChangeMoney(
				static_cast< EMGAME_MONEY_CHANGE >( i ),
				m_arrChangeMoney[ i ] );
		}

		for ( int i=0; i<m_vecMapChangeMoney.size(); ++i )
		{
			SMAP_MONEY_LOG sTempMoneyLog = m_vecMapChangeMoney.at( i );

			for ( int j=0; j<GAME_MONEY_CHANGE_MAX; ++j )
			{
				if ( 0 == sTempMoneyLog.llMoney[ j ] )
					continue;

				OnSaveChangeMapMoney(
					static_cast< EMGAME_MONEY_CHANGE >( j ),
					sTempMoneyLog.nMapID,
					sTempMoneyLog.llMoney[ j ] );
			}
		}
	}


	void CALLBACK IMoneyManager::OnChangeMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const LONGLONG _llChangeMoney )
	{
		if ( _emChangeType < 0 || GAME_MONEY_CHANGE_MAX <= _emChangeType )
		{
			sc::writeLogError( "[ Money ] [ Invalid ChangeMoneyType!! ]" );

			return;
		}

		if ( false == m_bLock )
		{
			if ( INVEN_MONEY_DEFAULT < _emChangeType &&
				_emChangeType <= SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY )
			{
				m_arrChangeMoney[ INVEN_MONEY_DEFAULT ] -= _llChangeMoney;
			}
			else if ( SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY < _emChangeType &&
				_emChangeType < GAME_MONEY_CHANGE_MAX )
			{
				m_arrChangeMoney[ INVEN_MONEY_DEFAULT ] += _llChangeMoney;
			}

			m_arrChangeMoney[ _emChangeType ] += _llChangeMoney;
		}
		else
		{
			if ( INVEN_MONEY_DEFAULT < _emChangeType &&
				_emChangeType <= SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY )
			{
				m_arrTempChangeMoney[ INVEN_MONEY_DEFAULT ] -= _llChangeMoney;
			}
			else if ( SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY < _emChangeType &&
				_emChangeType < GAME_MONEY_CHANGE_MAX )
			{
				m_arrTempChangeMoney[ INVEN_MONEY_DEFAULT ] += _llChangeMoney;
			}

			m_arrTempChangeMoney[ _emChangeType ] += _llChangeMoney;
		}
	}


	void CALLBACK IMoneyManager::OnChangeMapMoney ( const EMGAME_MONEY_CHANGE _emChangeType, const int _nMapID, const LONGLONG _llChangeMoney )
	{
		if ( _emChangeType < 0 || GAME_MONEY_CHANGE_MAX <= _emChangeType )
		{
			sc::writeLogError( "[ Money ] [ Invalid ChangeMoneyType!! ]" );

			return;
		}

		if ( false == m_bLock )
		{
			bool bPush = true;
			for ( int i=0; i<m_vecMapChangeMoney.size(); ++i )
			{
				SMAP_MONEY_LOG& sMoneyLog = m_vecMapChangeMoney.at( i );
				if ( sMoneyLog.nMapID == _nMapID )
				{
					if ( INVEN_MONEY_DEFAULT < _emChangeType &&
						_emChangeType <= SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY )
					{
						sMoneyLog.llMoney[ INVEN_MONEY_DEFAULT ] -= _llChangeMoney;
					}
					else if ( SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY < _emChangeType &&
						_emChangeType < GAME_MONEY_CHANGE_MAX )
					{
						sMoneyLog.llMoney[ INVEN_MONEY_DEFAULT ] += _llChangeMoney;
					}

					sMoneyLog.llMoney[ _emChangeType ] += _llChangeMoney;
					bPush = false;
				}
			}

			if ( bPush )
			{
				SMAP_MONEY_LOG sMoneyLog;

				if ( INVEN_MONEY_DEFAULT < _emChangeType &&
					_emChangeType <= SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY )
				{
					sMoneyLog.llMoney[ INVEN_MONEY_DEFAULT ] -= _llChangeMoney;
				}
				else if ( SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY < _emChangeType &&
					_emChangeType < GAME_MONEY_CHANGE_MAX )
				{
					sMoneyLog.llMoney[ INVEN_MONEY_DEFAULT ] += _llChangeMoney;
				}

				sMoneyLog.llMoney[ _emChangeType ] = _llChangeMoney;
				sMoneyLog.nMapID = _nMapID;

				m_vecMapChangeMoney.push_back( sMoneyLog );
			}
		}
		else
		{
			bool bPush = true;
			for ( int i=0; i<m_vecTempMapChangeMoney.size(); ++i )
			{
				SMAP_MONEY_LOG& sMoneyLog = m_vecTempMapChangeMoney.at( i );
				if ( sMoneyLog.nMapID == _nMapID )
				{
					if ( INVEN_MONEY_DEFAULT < _emChangeType &&
						_emChangeType <= SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY )
					{
						sMoneyLog.llMoney[ INVEN_MONEY_DEFAULT ] -= _llChangeMoney;
					}
					else if ( SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY < _emChangeType &&
						_emChangeType < GAME_MONEY_CHANGE_MAX )
					{
						sMoneyLog.llMoney[ INVEN_MONEY_DEFAULT ] += _llChangeMoney;
					}

					sMoneyLog.llMoney[ _emChangeType ] += _llChangeMoney;
					bPush = false;
				}
			}

			if ( bPush )
			{
				SMAP_MONEY_LOG sMoneyLog;

				if ( INVEN_MONEY_DEFAULT < _emChangeType &&
					_emChangeType <= SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY )
				{
					sMoneyLog.llMoney[ INVEN_MONEY_DEFAULT ] -= _llChangeMoney;
				}
				else if ( SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY < _emChangeType &&
					_emChangeType < GAME_MONEY_CHANGE_MAX )
				{
					sMoneyLog.llMoney[ INVEN_MONEY_DEFAULT ] += _llChangeMoney;
				}

				sMoneyLog.llMoney[ _emChangeType ] = _llChangeMoney;
				sMoneyLog.nMapID = _nMapID;

				m_vecTempMapChangeMoney.push_back( sMoneyLog );
			}
		}
	}

}