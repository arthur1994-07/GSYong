#include "pch.h"
#include "MatchingManager.h"
#include "./MatchingDefine.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../RanLogic/Tournament/GLGroupManager.h"
#include "../InstanceSystem/InstanceSystem.h"
#include "../InstanceSystem/LogicAgent/InvokeFunctionAgent.h"
#include "../InstanceSystem/LogicAgent/InstanceSystemAgent.h"

#include "../Club/GLClubAgent.h"

#include "../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"
#include "../../RanLogic/Land/FactionManager/FactionManagerDefine.h"
#include "../../RanLogic/Tournament/GLGroup.h"

#include "../Database/DBAction/DbActionLogInstantDungeon.h"

#include "./MatchingModeSustained.h"
#include "./MatchingModeImmediately.h"

#include "./MatchingGameBase.h"
#include "./MatchingInstanceBase.h"


namespace MatchingSystem
{

	void MatchingInstanceBase::OnInitialize ( )
	{
		// �δ� �ý��� �Լ� ���;
		InstanceSystem::registFunctionCommon( *m_pScript );
		InstanceSystem::registFunctionAgent( *m_pScript );

		// �׷� ����;
		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
		{
			sc::writeLogError( "[ Matching Log ] [ Callback MatchingInstanceBase::OnInitialize / GroupManager is NULL. ]" );
			return;
		}

		pGroupManager->RegistCallBackFunction_Lua( m_pScript );

		// ���Ӻ��̽� �����Լ� ��� ( �������� �� );
		m_pMatchingBase->RegistFunctionGameBase( m_pScript );

		// Script Invoke Function ���;
		m_pScript->registdirectFunction( "SetRankScore", *this, &MatchingInstanceBase::SetRankScore );
		m_pScript->registdirectFunction( "GetGroupToMatching", *this, &MatchingInstanceBase::GetGroupToMatching );
		m_pScript->registdirectFunction( "GetGroupToMatchingNotPlay", *this, &MatchingInstanceBase::GetGroupToMatchingNotPlay );
		m_pScript->registdirectFunction( "SetGroupToPlay", *this, &MatchingInstanceBase::SetGroupToPlay );
		m_pScript->registdirectFunction( "SendFaction", *this, &MatchingInstanceBase::SendFaction );
		m_pScript->registdirectFunction( "SetGroupToLobby", *this, &MatchingInstanceBase::SetGroupToLobby );
		m_pScript->registdirectFunction( "SetGroupToLobbyALL", *this, &MatchingInstanceBase::SetGroupToLobbyALL );
		m_pScript->registdirectFunction( "GetMatchingWaitingGroupCount", *this, &MatchingInstanceBase::GetMatchingWaitingGroupCount );
		m_pScript->registdirectFunction( "DoDestroy", *this, &MatchingInstanceBase::DoDestroy );
		m_pScript->registdirectFunction( "GroupAskJoinTimeOut", *this, &MatchingInstanceBase::OutCharInLobby );
		m_pScript->registdirectFunction( "JoinLock", *this, &MatchingInstanceBase::JoinLock );
		m_pScript->registdirectFunction( "joinSuspend", *this, &MatchingInstanceBase::JoinSuspend );
		m_pScript->registdirectFunction( "SetTournamentRound", *this, &MatchingInstanceBase::SetMatchingRound );
		m_pScript->registdirectFunction( "SetTournamentStatus", *this, &MatchingInstanceBase::SetMatchingStatus );
		m_pScript->registdirectFunction( "SetTournamentPlayTime", *this, &MatchingInstanceBase::SetMatchingPlayTime );
		m_pScript->registdirectFunction( "SetTournamentPlayersReset", *this, &MatchingInstanceBase::SetMatchingPlayersReset );
		m_pScript->registdirectFunction( "SetTournamentSize", *this, &MatchingInstanceBase::SetMatchingSize );
		m_pScript->registdirectFunction( "SetFinalWinner", *this, &MatchingInstanceBase::SetFinalWinner );
		m_pScript->registdirectFunction( "SetJoinCanselDrop", *this, &MatchingInstanceBase::SetJoinCanselDrop );
		m_pScript->registdirectFunction( "StartMatching", *this, &MatchingInstanceBase::StartMatching );
		m_pScript->registdirectFunction( "FinishGroupALL", *this, &MatchingInstanceBase::FinishGroupALL );
		m_pScript->registdirectFunction( "SamiFinishGroup", *this, &MatchingInstanceBase::SemiFinishGroup );
		m_pScript->registdirectFunction( "FinishGroup", *this, &MatchingInstanceBase::FinishGroup );
		m_pScript->registdirectFunction( "DoOut", *this, &MatchingInstanceBase::DoOut );

		m_pScript->registFunctionex( "WaitTime", *this, &MatchingInstanceBase::ScriptInvoke_WaitTime );
		m_pScript->registFunctionex( "SendJoin", *this, &MatchingInstanceBase::ScriptInvoke_SendJoin );
		//m_pScript->registFunctionex( "SendToGroupAskJoinAsyc", *this, &MatchingInstanceBase::ScriptInvoke_SendToGroupAskJoinAsyc );
		m_pScript->registFunctionex( "MapMultyCreate", *this, &MatchingInstanceBase::ScriptInvoke_MapMultiCreate );
		m_pScript->registFunctionex( "SetTournamentPlayers", *this, &MatchingInstanceBase::SetMatchingPlayers );
	}

	void MatchingInstanceBase::OnSetMatchingMode ( DWORD dwMatchingModeID )
	{
		SAFE_DELETE ( m_pCurMatchingMode );

		m_dwMatchingModeID = dwMatchingModeID;

		switch ( m_dwMatchingModeID )
		{
		case MatchingGameBase::EMMATCHING_MODE_SCRIPT:
			{
				SAFE_DELETE ( m_pCurMatchingMode );
			}
			break;

		case MatchingGameBase::EMMATCHING_MODE_SUSTAINED:
			{
				m_pCurMatchingMode = new CMatchingModeSustained( this, m_pScript );
				if ( m_pCurMatchingMode )
					m_pCurMatchingMode->OnInitialize();
			}
			break;

		case MatchingGameBase::EMMATCHING_MODE_IMMEDIATELY:
			{
				m_pCurMatchingMode = new CMatchingModeImmediately( this, m_pScript );
				if ( m_pCurMatchingMode )
					m_pCurMatchingMode->OnInitialize();
			}
			break;

		default:
			{
				SAFE_DELETE ( m_pCurMatchingMode );

				m_dwMatchingModeID =  MatchingGameBase::EMMATCHING_MODE_SCRIPT;
			}
			break;
		}
	}

	void MatchingInstanceBase::OnFrameMove ( float fElpaseTime )
	{
		if ( m_bDestroyMatchingInstance )
			return;

		static float fTime = 0.f;
		fTime += fElpaseTime;

		if ( fTime > 10.f )
		{
			InstanceSystem::ManagerAgent* pManager =
				MatchingCore::getInstanceManager();
			if ( NULL == pManager )
				return;

			// ���� ���� Trigger üũ;
			std::vector< JoinTrigger* >::iterator bitor =
				m_JoinFailTrigger.begin();
			for ( ; bitor != m_JoinFailTrigger.end(); )
			{
				JoinTrigger* pTrigger = *bitor;

				const InstanceSystem::InformationPosition _informationPosition(
					Faction::GLActorID( CROW_PC, pTrigger->_ClientDBID ),
					SNATIVEID( DWORD( pTrigger->_instanceMapID ) ),
					Faction::GLActorID( CROW_NULL, GAEAID_NULL ),
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE );

				// ���� ��õ�;
				bool isJoinSuccess = pManager->DoJoin(
					pTrigger->_ClientDBID,
					_informationPosition,
					InstanceSystem::EMAUTHORITY_NORMAL_FORCED,
					pTrigger );

				// ������ 10�ʿ� �ѹ��� ��õ��� �ϰԵǹǷ� �ʿ���� �ڵ�;
				/*pTrigger->m_frameMove -= fTime;
				if ( pTrigger->m_frameMove > 0 )
				{
					++bitor;
					continue;
				}*/

				// ��õ� ���н�;
				if ( false == isJoinSuccess )
				{
					// ���� Ƚ���� �ټ����� ������;
					if ( pTrigger->m_faildCounter > 5 )
					{
						CString strTemp;
						strTemp.Format(
							"SendJoin Fail :: Critical Failure (DBID:%d/MAP:%d)",
							pTrigger->_ClientDBID,
							pTrigger->_instanceMapID );
						sc::writeLogError( strTemp.GetString() );

						JoinReceipt failReceipt;
						failReceipt._instanceMapID = pTrigger->_instanceMapID;
						failReceipt.isFail = true;

						OnJoin( &failReceipt );
						bitor = m_JoinFailTrigger.erase(bitor);
						SAFE_DELETE( pTrigger );

						continue;
					}

					// ������ 10�ʿ� �ѹ��� ��õ��� �ϰԵǹǷ� �ʿ���� �ڵ�;
					/*pTrigger->m_frameMove = 1;

					for ( DWORD i=0 ; i<pTrigger->m_faildCounter; ++i )
					{
						pTrigger->m_frameMove *= 2;
					}*/

					pTrigger->m_faildCounter += 1;

					// 10�� �ڿ� ��õ��Ѵٴ� �α׸� �����;
					CString strTemp;
					strTemp.Format(
						"SendJoin Fail :: Retry Join (%d) (retry to %.0fs)",
						pTrigger->m_faildCounter,
						10.f );

					sc::writeLogError( strTemp.GetString() );
				}

				++bitor;
			}

			// ��ũ��Ʈ ����� ��� ��ũ��Ʈ�� FrameMove�� ������;
			if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
				== m_dwMatchingModeID )
			{
				OnLuaFrameMove( fTime );
			}
			// ��ũ��Ʈ ��尡 �ƴ� ���;
			else
			{
				if ( m_pCurMatchingMode )
					m_pCurMatchingMode->OnFrameMove( fTime );
			}

			fTime = 0.f;
		}

		// ��ũ��Ʈ ������ üũ;
		if ( true == m_bSetWait )
		{
			if ( m_fWaitDuration > 0 )
			{
				m_fWaitDuration -= fElpaseTime;
			}
			else 
			{
				m_bSetWait = false;

				int status = m_pScript->CoroutineResume( m_pCoroutineState, 0 );

				if( status ==  LUA_YIELD)
					m_bUseCoroutine = true;
				else
					m_bUseCoroutine = false;
			}
		}

		MatchingSystem::GLMatchingGroupManager* pGroupmanager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupmanager )
			return;

		// Join Ȯ�� ����ð� üũ;
		if ( m_bWaitingAnswer )
		{
			m_fAnswerTimer -= fElpaseTime;
			if ( m_fAnswerTimer < 0.f )
			{
				_SetAnswerEnded();
			}
		}
	}

	bool MatchingInstanceBase::OnEventCustomMsg (
		const InstanceSystem::InstanceMapID _instanceMapID,
		const double _param1,
		const double _param2,
		const double _param3,
		const double _param4 )
    {
        if ( m_bDestroyMatchingInstance )
            return false;

        if ( !m_pMatchingBase->getRoomManager()->IsMyMap(
			_instanceMapID,
			m_dwMatchingInstanceID ) ) 
            return false;

		// ��ũ��Ʈ ����� ���;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript && m_pScript->PushFunctionEx( "EventCustomMsg" ) == true )
			{
				lua_State* pState = m_pScript->GetLuaState();
				lua_pushinteger( pState, _instanceMapID );
				lua_pushnumber( pState, _param1 );
				lua_pushnumber( pState, _param2 );
				lua_pushnumber( pState, _param3 );
			    lua_pushnumber( pState, _param4 );
			    m_pScript->CallFunction( 5, 0 );

			    return true;
			}
		}
		// ��ũ��Ʈ ��尡 �ƴ� ���;
		else
		{
			if ( m_pCurMatchingMode )
			{
				m_pCurMatchingMode->OnEventCustomMsg (
					_instanceMapID,
					_param1,
					_param2,
					_param3,
					_param4 );

				return true;
			}
		}

        return false;
    }


	/*int MatchingInstanceBase::OnMapCreate ( RoomReceipt* _RoomReceipt )
	{
		if ( m_bDestroyMatchingInstance )
			return 0;

		sc::writeLogInfo( sc::string::format(
			"MATCHING: OnMapCreate %d",
			_RoomReceipt->_instanceMapID ) );

		// ��ũ��Ʈ ����� ���;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript && m_pScript->pushFunctionEx( "OnMapCreate" ) == true )
			{
				m_pScript->pushInteger( _RoomReceipt->_instanceMapID );
				m_pScript->pushBoolean( _RoomReceipt->_bCreateComplete );

				if ( m_pScript->callFunction( 2, 1 ) == true )
				{
					// ���� ���� �ƹ��������� ������� �ʰ��ִ�;
					m_pScript->popBoolean();
				}
			}
		}
		// ��ũ��Ʈ ��尡 �ƴ� ���;
		else
		{
			if ( m_pCurMatchingMode )
			{
				m_pCurMatchingMode->OnMapCreate (
					_RoomReceipt->_instanceMapID,
					_RoomReceipt->_bCreateComplete );
			}
		}

		// ���� �ȸ������ ��� �� �ѾƳ���;
		if ( _RoomReceipt->_bCreateComplete == false )
		{
			SetGroupToLobbyALL();

			sc::writeLogInfo("MatchingInstanceBase : OnMapCreate FAIL");
		}
		// �δ��� �ı� Ʈ���Ÿ� �̸� ����Ѵ�;
		else
		{
			DestroyTrigger* pTrigger = new DestroyTrigger( m_dwMatchingInstanceID );
			if ( pTrigger )
			{
				pTrigger->_instanceID = _RoomReceipt->_instanceMapID;
				pTrigger->_MatchingInstancceID = m_dwMatchingInstanceID;
				pTrigger->RegistCallback( *this, &MatchingInstanceBase::OnDestroy );
			}

			MatchingSystem::MatchingCallbackTriggerManager* pMCTM =
				MatchingSystem::MatchingCallbackTriggerManager::GetInstance();
			if ( pMCTM )
				pMCTM->m_DestroyTriggerList.push_back( pTrigger );

			if ( m_pMatchingBase )
			{
				// ���� �ϼ���Ų��;
				m_pMatchingBase->getRoomManager()
					->CreateComplete( _RoomReceipt );
			}
		}

		return 0;
	}*/


	int MatchingInstanceBase::OnMapMultiCreate ( RoomMultyReceipt* _RoomReceipt )
	{
		if ( m_bDestroyMatchingInstance )
			return 0;

		sc::writeLogInfo( "MatchingInstanceBase : OnMapMultyCreate" );

		// ��ũ��Ʈ ����� ���;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			lua_State* pState = _RoomReceipt->pState;
			// �� ���̺� ����;
			lua_newtable( m_pCoroutineState );
			int top = lua_gettop( m_pCoroutineState );

			int i = 0;
			BOOST_FOREACH ( DWORD _MapID, _RoomReceipt->_instanceMapID )
			{
				if ( _RoomReceipt->_bCreateComplete[ i ] == false )
					lua_pushinteger( m_pCoroutineState, -1 );
				else
					lua_pushinteger( m_pCoroutineState, _MapID );

				lua_pushinteger( m_pCoroutineState, i );
				lua_settable( m_pCoroutineState, top );

				DestroyTrigger* pTrigger = new DestroyTrigger( m_dwMatchingInstanceID );
				if ( pTrigger )
				{
					pTrigger->_instanceID = _MapID;
					pTrigger->_MatchingInstancceID = m_dwMatchingInstanceID;
					pTrigger->RegistCallback( *this, &MatchingInstanceBase::OnDestroy );
				}

				MatchingSystem::MatchingCallbackTriggerManager* pMCTM =
					MatchingSystem::MatchingCallbackTriggerManager::GetInstance();
				if ( pMCTM )
					pMCTM->m_DestroyTriggerList.push_back( pTrigger );

				MatchingSystem::RoomReceipt receipt;
				receipt._bCreateComplete = _RoomReceipt->_bCreateComplete[ i ];
				receipt._instanceMapID = _RoomReceipt->_instanceMapID[ i ];
				receipt._MatchingInstanceID = m_dwMatchingInstanceID;
				receipt.dwMatchingBaseID = m_dwMatchingBaseID;

				m_pMatchingBase->getRoomManager()->CreateComplete( &receipt );

				++i;
			}

			int status = m_pScript->CoroutineResume( m_pCoroutineState, 1 );

			if ( status ==  LUA_YIELD )
				m_bUseCoroutine = true;
			else
				m_bUseCoroutine = false;
		}
		// ��ũ��Ʈ ��尡 �ƴ� ���;
		else
		{
			if ( m_pCurMatchingMode )
			{
				int i = 0;
				BOOST_FOREACH ( DWORD _MapID, _RoomReceipt->_instanceMapID )
				{
					DestroyTrigger* pTrigger = new DestroyTrigger( m_dwMatchingInstanceID );
					if ( pTrigger )
					{
						pTrigger->_instanceID = _MapID;
						pTrigger->_MatchingInstancceID = m_dwMatchingInstanceID;
						pTrigger->RegistCallback( *this, &MatchingInstanceBase::OnDestroy );
					}

					MatchingSystem::MatchingCallbackTriggerManager* pMCTM =
						MatchingSystem::MatchingCallbackTriggerManager::GetInstance();
					if ( pMCTM )
						pMCTM->m_DestroyTriggerList.push_back( pTrigger );

					MatchingSystem::RoomReceipt receipt;
					receipt._bCreateComplete = _RoomReceipt->_bCreateComplete[ i ];
					receipt._instanceMapID = _RoomReceipt->_instanceMapID[ i ];
					receipt._MatchingInstanceID = m_dwMatchingInstanceID;
					receipt.dwMatchingBaseID = m_dwMatchingBaseID;

					m_pMatchingBase->getRoomManager()->CreateComplete( &receipt );

					++i;
				}

				m_pCurMatchingMode->OnMapMultiCreate( _RoomReceipt );
			}
		}

		return 0;
	}

	int MatchingInstanceBase::OnJoin ( JoinReceipt* _pJoinReceipt )
	{
		if ( m_bDestroyMatchingInstance )
			return 0;

		// �α׸� �����;
		if ( _pJoinReceipt && _pJoinReceipt->isFail == true )
		{
			sc::writeLogInfo( "[ Matching Log ] [ FIELD OnJoin FAIL ... ]" );

			// ��õ� ������ �� �� �ʿ䰡 �ִ�;
			/*JoinTrigger* pTrigger = new JoinTrigger( m_dwMatchingInstanceID );
			pTrigger->_ClientDBID = _pJoinReceipt->ClientDB;
			pTrigger->_instanceMapID = _pJoinReceipt->_instanceMapID;

			if ( pTrigger )
			{
				sc::writeLogInfo( "[ Matching Log ] [ Retry Join ...... Wait ...  ]" );

				m_JoinFailTrigger.push_back( pTrigger );
			}*/
		}
		else
		{
			sc::writeLogInfo( "[ Matching Log ] [ FIELD OnJoin GOOD ]" );
		}

		return 0;
	}

	int MatchingInstanceBase::OnDestroy ( DestroyReceipt* _RoomReceipt )
	{
		bool NotOUT = true;

		if ( m_bDestroyMatchingInstance )
			return 0;
		
		// ��ũ��Ʈ ����� ���;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript && m_pScript->PushFunctionEx( "OnDestroy" ) == true )
			{
				m_pScript->PushInteger( _RoomReceipt->_instanceMapID );
				m_pScript->PushBoolean( _RoomReceipt->_bDestroyComplete );

				if ( m_pScript->CallFunction(2, 1) == true )
					NotOUT = m_pScript->PopBoolean();
			}
		}
		// ��ũ��Ʈ ��尡 �ƴ� ���;
		else
		{
			if ( m_pCurMatchingMode )
			{
				NotOUT = m_pCurMatchingMode->OnDestroy(
					_RoomReceipt->_instanceMapID,
					_RoomReceipt->_bDestroyComplete );
			}
		}

		if ( m_pMatchingBase )
		{
			m_pMatchingBase->getRoomManager()->DestroyComplete(
				_RoomReceipt, NotOUT );
		}

		return 0;
	}

	bool MatchingInstanceBase::OnReset ( )
	{
		if ( m_bDestroyMatchingInstance )
			return 0;

		// ��ũ��Ʈ ����� ���;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript && m_pScript->PushFunctionEx( "OnReset" ) == true )
			{
				if ( m_pScript->CallFunction( 0, 1 ) == true )
					return m_pScript->PopBoolean();
			}
		}
		// ��ũ��Ʈ ��尡 �ƴ� ���;
		else
		{
			if ( m_pCurMatchingMode )
			{
				return m_pCurMatchingMode->OnReset();
			}
		}

		return false;
	}


	float MatchingInstanceBase::OnLuaFrameMove ( float fElpaseTime )
	{
		if ( m_bDestroyMatchingInstance )
			return 0;

		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript && m_pScript->PushFunctionEx( "OnLuaFrameMove" ) == true )
			{
				m_pScript->PushNumber( fElpaseTime );
				if ( m_pScript->CallFunction( 1, 1 ) == true )
					return m_pScript->PopNumber();
			}
		}

		return fElpaseTime;
	}


	int MatchingInstanceBase::OnReadyAnswer ( ReadyReceipt* _ReadyReceipt )
	{
		// �亯�� ��ٸ��� �ִ� ���°� �ƴ϶��;
		// -> �̹� ������ ���;
		// -> ����;
		if ( false == m_bWaitingAnswer )
		{
			return 0;
		}

		MatchingSystem::GLMatchingGroupManager* pGroupmanager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupmanager )
			return 0;

		// �Ѹ��̶� ���н� ���� ó��;
		if ( false == _ReadyReceipt->isOK )
		{
			_SetAnswerEnded();

			// Waiting ���¸� Ǭ��;
			m_dwAnswerCharacterCount = 0;
			m_bWaitingAnswer = false;
			m_fAnswerTimer = 100.f;

			return 0;
		}

		// ����� ĳ���� ����;
		m_dwAnswerCharacterCount++;

		// ��� ������ ������� ���;
		if ( pGroupmanager->GetCharCountOfMatching( m_dwMatchingInstanceID )
			<= m_dwAnswerCharacterCount )
		{
			// ������ ó��;
			// ��ũ��Ʈ ����� ���;
			if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
				== m_dwMatchingModeID )
			{
				if ( m_pScript->PushFunctionEx( "OnReadyAnswer" ) == true )
				{
					m_pScript->CallFunction( 0, 0 );
				}
			}
			// ��ũ��Ʈ ��尡 �ƴ� ���;
			else
			{
				if ( m_pCurMatchingMode )
				{
					m_pCurMatchingMode->OnReadyAnswer( );
				}
			}

			// Waiting ���¸� Ǭ��;
			m_dwAnswerCharacterCount = 0;
			m_bWaitingAnswer = false;
			m_fAnswerTimer = 100.f;
		}

		/*JoinTable* FindNode = NULL;

		if ( m_JoinTable.size() != 0 &&
			m_vecJoinSaved.size() == 0 )
		{
			JOINTABLE_VEC_ITOR biter = m_JoinTable.begin();

			for ( ; biter != m_JoinTable.end(); )
			{
				JoinTable* nNode = *biter;
				if ( NULL == nNode )
				{
					biter = m_JoinTable.erase( biter );
					continue;
				}

				if ( nNode->isExsist( _ReadyReceipt->ClietDBID ) )
				{
					FindNode = nNode;
					nNode->SetClientAnswer(
						_ReadyReceipt->ClietDBID,
						_ReadyReceipt->isOK );

					if ( nNode->isAllAnswer() )
					{
						// ��ũ��Ʈ ����� ���;
						if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
							== m_dwMatchingModeID )
						{
							if ( m_pScript->pushFunctionEx( "OnReadyGroupAnswer" ) == true )
							{
								m_pScript->pushInteger( nNode->GetGroupID() );
								m_pScript->pushBoolean( _ReadyReceipt->isOK );
								m_pScript->callFunction( 2, 0 );
							}
						}
						// ��ũ��Ʈ ��尡 �ƴ� ���;
						else
						{
							if ( m_pCurMatchingMode )
							{
								m_pCurMatchingMode->OnReadyGroupAnswer(
									nNode->GetGroupID(),
									_ReadyReceipt->isOK );
							}
						}

						SAFE_DELETE( nNode );
						biter = m_JoinTable.erase( biter );

						return 0;
					}

					m_dwAnswerCharacterCount--;
				}

				++biter;
			}


		}
		else if ( m_vecJoinSaved.size() != 0 )
		{
			JOINTABLE_VEC_ITOR biter = m_JoinTable.begin();

			for ( ; biter != m_JoinTable.end(); )
			{
				JoinTable* nNode = *biter;
				if ( NULL == nNode )
				{
					biter = m_JoinTable.erase( biter );
					continue;
				}

				if ( nNode->isExsist( _ReadyReceipt->ClietDBID ) )
				{
					FindNode = nNode;
					nNode->SetClientAnswer(
						_ReadyReceipt->ClietDBID,
						_ReadyReceipt->isOK );

					if ( nNode->isAllAnswer() )
					{
						// ��ũ��Ʈ ����� ���;
						if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
							== m_dwMatchingModeID )
						{
							if ( m_pScript->pushFunctionEx( "OnReadyGroupAnswer" ) == true )
							{
								m_pScript->pushInteger( nNode->GetGroupID() );
								m_pScript->pushBoolean( _ReadyReceipt->isOK );
								m_pScript->callFunction( 2, 0 );
							}
						}
						// ��ũ��Ʈ ��尡 �ƴ� ���;
						else
						{
							if ( m_pCurMatchingMode )
							{
								m_pCurMatchingMode->OnReadyGroupAnswer(
									nNode->GetGroupID(),
									_ReadyReceipt->isOK );
							}
						}
					}

					m_dwAnswerCharacterCount--;

					break;
				}

				++biter;
			}
		}

		// ��ũ��Ʈ ����� ���;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript->pushFunctionEx( "OnReadyAnswer" ) == true )
			{
				m_pScript->pushInteger( _ReadyReceipt->ClietDBID );
				m_pScript->pushBoolean( _ReadyReceipt->isOK );
				m_pScript->callFunction( 2, 0 );

				return 0;
			}
		}
		// ��ũ��Ʈ ��尡 �ƴ� ���;
		else
		{
			if ( m_pCurMatchingMode )
			{
				m_pCurMatchingMode->OnReadyAnswer(
					_ReadyReceipt->ClietDBID,
					_ReadyReceipt->isOK );
			}
		}

		if ( NULL == FindNode )
		{
			return 0;
		}

		if ( m_dwAnswerCharacterCount == 0 )
		{
			_SetAnswerEnded();

			sc::writeLogDebug( "MatchingInstanceBase : SetAnswerEnded" );
		}*/

		return 0;
	}


	bool MatchingInstanceBase::OnReceiveDataStart (
		const InstanceSystem::InstanceMapID _instanceMapID )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		return m_pMatchingBase->getRoomManager()->IsMyMap(
			_instanceMapID,
			m_dwMatchingInstanceID );
	}

	bool MatchingInstanceBase::OnReceiveDataEnd (
		const InstanceSystem::InstanceMapID _instanceMapID )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		if ( !m_pMatchingBase->getRoomManager()->IsMyMap(
			_instanceMapID,
			m_dwMatchingInstanceID ) ) 
			return false;

		m_bReceiveDataError = false;	

		return !m_bReceiveDataError;
	}

	bool MatchingInstanceBase::OnReceiveError (
		const InstanceSystem::InstanceMapID _instanceMapID,
		const DWORD _ErrorCode )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		if ( !m_pMatchingBase->getRoomManager()->IsMyMap(
			_instanceMapID,
			m_dwMatchingInstanceID ) ) 
			return false;

		// ��ũ��Ʈ ����� ���;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript && m_pScript->PushFunctionEx( "ErrorCode" ) == true )
			{
				m_pScript->PushInteger( _ErrorCode );
				if ( m_pScript->CallFunction( 1, 1 ) == true )
					m_bReceiveDataError = m_pScript->PopBoolean();
				else
					m_bReceiveDataError = false;
			}
		}
		// ��ũ��Ʈ ��尡 �ƴ� ���;
		else
		{
			if ( m_pCurMatchingMode )
			{
				m_bReceiveDataError = m_pCurMatchingMode->OnReceiveError(
					_ErrorCode );
			}
		}

		return false;
	}

	bool MatchingInstanceBase::OnReceiveData ( 
		const InstanceSystem::InstanceMapID _instanceMapID,
		const char* varName,
		const double _data )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		if ( !m_pMatchingBase->getRoomManager()->IsMyMap(
			_instanceMapID,
			m_dwMatchingInstanceID ) ) 
			return false;

		m_pScript->SetGlobalValueNumber_Lua( varName, _data );

		return true;
	}

	bool MatchingInstanceBase::OnReceiveData (
		const InstanceSystem::InstanceMapID _instanceMapID,
		const char* varName,
		const char* _data )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		if( !m_pMatchingBase->getRoomManager()->IsMyMap(
			_instanceMapID,
			m_dwMatchingInstanceID ) ) 
			return false;

		m_pScript->SetGlobalValueString_Lua( varName, _data );

		return true;
	}

	bool MatchingInstanceBase::OnReceiveData ( 
		const InstanceSystem::InstanceMapID _instanceMapID,
		const char* varName,
		const bool _data )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		if( !m_pMatchingBase->getRoomManager()->IsMyMap(
			_instanceMapID,
			m_dwMatchingInstanceID ) ) 
			return false;

		m_pScript->SetGlobalValueBoolean_Lua( varName, _data );

		return true;
	}

	bool MatchingInstanceBase::OnNoticeResult (
		const InstanceSystem::InstanceMapID _instanceMapID,
		const DWORD _FactionID,
		const BOOL _IsWinner )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		if ( NULL == m_pMatchingBase )
			return false;

		if ( !m_pMatchingBase->getRoomManager()->IsMyMap(
			_instanceMapID,
			m_dwMatchingInstanceID ) ) 
			return false;

		DWORD dwChaNum = m_pMatchingBase->getRoomManager()->GetChaNumOnGroup(
			_instanceMapID, m_dwMatchingInstanceID, _FactionID );
		if ( dwChaNum == 0 )
			return false;

		const GLCharAG* pChar = gpAgentServer->GetCharByDbNum( dwChaNum );
		if ( NULL == pChar )
			return false;

		// ��Ī �α� �����;
		db::LogTournament* pDbAction = new db::LogTournament( 9004 );

		pDbAction->dwCharDbNum = dwChaNum;
		pDbAction->strChaName = pChar->GetChaName();
		pDbAction->dwFactionID = _FactionID;
		pDbAction->dwForceWin = _IsWinner;

		gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

		// ��ũ��Ʈ ����� ���;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript->PushFunctionEx( "NoticeResualt" ) == true )
			{
				m_pScript->PushInteger( _instanceMapID );
				m_pScript->PushInteger( _FactionID );
				m_pScript->PushBoolean( _IsWinner == TRUE );

				if ( m_pScript->CallFunction( 3, 1 ) == true )
					return m_pScript->PopBoolean();
			}
		}
		// ��ũ��Ʈ ��尡 �ƴ� ���;
		else
		{
			if ( m_pCurMatchingMode )
			{
				return m_pCurMatchingMode->OnNoticeResult(
					_instanceMapID,
					_FactionID,
					_IsWinner == TRUE );
			}
		}

		return false;
	}


	void MatchingInstanceBase::OnUpdateMatchingGroupInfo ( DWORD ClietDBID )
	{
		if ( IsReadyToStart() == true )
			return;

		if ( NULL == gpAgentServer )
			return;

		GLCharAG* const _pPlayer = gpAgentServer->GetCharByDbNum( ClietDBID );
		if ( NULL == _pPlayer )
			return;

		GLMSG::NET_MATCHING_GROUPTABLEINFO_START _MessageMatching;
		_MessageMatching.TourNumentSize = m_dwMatchingSize;
		_MessageMatching.TourNumCurrentRound = m_dwMatchingRound;
		_MessageMatching.ScriptID = m_pMatchingBase->GetScriptID();

		GLMSG::NET_MATCHING_GROUPTABLEINFO_ADD _MessageMatchingADD;
		_MessageMatchingADD.TourNumentSize = m_dwMatchingSize;
		_MessageMatchingADD.ScriptID = m_pMatchingBase->GetScriptID();
		
		std::vector<GLMatChingGroupInfo>::iterator _biter = m_vecMatchingGroupInfo.begin();
		std::vector<GLMatChingGroupInfo>::iterator _fbiter = m_vecMatchingGroupInfo.begin();
		std::vector<GLMatChingGroupInfo>::iterator _ebiter = m_vecMatchingGroupInfo.begin();

		/*  
			16���� ������ ������ �޽��� ���� 1MB�� �Ѿ��ȵȴ�.;
			1MB�� �Ѵ� ������ �̸������̴�. �߰��� �̸��ٲ�� �ٲﵥ�� ��� �Ǳ⶧��.;
		*/
		int counting = 16;
		bool bFirst = true;

		for ( ; _biter!= m_vecMatchingGroupInfo.end(); ++_biter )
		{
			if ( counting < 0 )
			{
				_ebiter = _biter;

				if ( bFirst == true )
				{
					std::copy( _fbiter, _ebiter, std::back_inserter( _MessageMatching.matchingInfo ) );
					msgpack::sbuffer SendBuffer;
					msgpack::pack( SendBuffer, _MessageMatching );

					gpAgentServer->SENDTOCLIENT(
						_pPlayer->ClientSlot(),
						NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM,
						SendBuffer );

					_MessageMatching.matchingInfo.clear();

					bFirst = false;
				}
				else
				{
					std::copy( _fbiter, _ebiter, std::back_inserter( _MessageMatchingADD.matchingInfo ) );
					msgpack::sbuffer SendBuffer;
					msgpack::pack( SendBuffer, _MessageMatchingADD );

					gpAgentServer->SENDTOCLIENT(
						_pPlayer->ClientSlot(),
						NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM,
						SendBuffer );

					_MessageMatchingADD.matchingInfo.clear();
				}

				// 16���� ������;
				counting = 16;
				_fbiter = _biter;
				
			}
			else
			{
				counting--;
			}
		}

		// �޽��� �����ߵɲ� ���� �� ���Ҵٸ�;
		if ( _ebiter != m_vecMatchingGroupInfo.end() )
		{
			_ebiter = m_vecMatchingGroupInfo.end();

			if ( bFirst == true )
			{
				std::copy( _fbiter, _ebiter, std::back_inserter( _MessageMatching.matchingInfo ) );
				msgpack::sbuffer SendBuffer;
				msgpack::pack( SendBuffer, _MessageMatching );

				gpAgentServer->SENDTOCLIENT(
					_pPlayer->ClientSlot(),
					NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM,
					SendBuffer );

				_MessageMatching.matchingInfo.clear();

				bFirst = false;
			}
			else
			{
				std::copy( _fbiter, _ebiter, std::back_inserter( _MessageMatchingADD.matchingInfo ) );
				msgpack::sbuffer SendBuffer;
				msgpack::pack( SendBuffer, _MessageMatchingADD );

				gpAgentServer->SENDTOCLIENT(
					_pPlayer->ClientSlot(),
					NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM,
					SendBuffer );

				_MessageMatchingADD.matchingInfo.clear();
			}
		}
	}

}