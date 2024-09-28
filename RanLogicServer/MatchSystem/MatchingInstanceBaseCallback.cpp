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
		// 인던 시스템 함수 등록;
		InstanceSystem::registFunctionCommon( *m_pScript );
		InstanceSystem::registFunctionAgent( *m_pScript );

		// 그룹 설정;
		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
		{
			sc::writeLogError( "[ Matching Log ] [ Callback MatchingInstanceBase::OnInitialize / GroupManager is NULL. ]" );
			return;
		}

		pGroupManager->RegistCallBackFunction_Lua( m_pScript );

		// 게임베이스 공용함수 등록 ( 전장점수 등 );
		m_pMatchingBase->RegistFunctionGameBase( m_pScript );

		// Script Invoke Function 등록;
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

			// 입장 실패 Trigger 체크;
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

				// 입장 재시도;
				bool isJoinSuccess = pManager->DoJoin(
					pTrigger->_ClientDBID,
					_informationPosition,
					InstanceSystem::EMAUTHORITY_NORMAL_FORCED,
					pTrigger );

				// 무조건 10초에 한번씩 재시도를 하게되므로 필요없는 코드;
				/*pTrigger->m_frameMove -= fTime;
				if ( pTrigger->m_frameMove > 0 )
				{
					++bitor;
					continue;
				}*/

				// 재시도 실패시;
				if ( false == isJoinSuccess )
				{
					// 실패 횟수가 다섯번을 넘으면;
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

					// 무조건 10초에 한번씩 재시도를 하게되므로 필요없는 코드;
					/*pTrigger->m_frameMove = 1;

					for ( DWORD i=0 ; i<pTrigger->m_faildCounter; ++i )
					{
						pTrigger->m_frameMove *= 2;
					}*/

					pTrigger->m_faildCounter += 1;

					// 10초 뒤에 재시도한다는 로그를 남긴다;
					CString strTemp;
					strTemp.Format(
						"SendJoin Fail :: Retry Join (%d) (retry to %.0fs)",
						pTrigger->m_faildCounter,
						10.f );

					sc::writeLogError( strTemp.GetString() );
				}

				++bitor;
			}

			// 스크립트 모드일 경우 스크립트의 FrameMove를 돌린다;
			if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
				== m_dwMatchingModeID )
			{
				OnLuaFrameMove( fTime );
			}
			// 스크립트 모드가 아닐 경우;
			else
			{
				if ( m_pCurMatchingMode )
					m_pCurMatchingMode->OnFrameMove( fTime );
			}

			fTime = 0.f;
		}

		// 스크립트 대기상태 체크;
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

		// Join 확인 응답시간 체크;
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

		// 스크립트 모드일 경우;
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
		// 스크립트 모드가 아닐 경우;
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

		// 스크립트 모드일 경우;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript && m_pScript->pushFunctionEx( "OnMapCreate" ) == true )
			{
				m_pScript->pushInteger( _RoomReceipt->_instanceMapID );
				m_pScript->pushBoolean( _RoomReceipt->_bCreateComplete );

				if ( m_pScript->callFunction( 2, 1 ) == true )
				{
					// 리턴 값을 아무곳에서도 사용하지 않고있다;
					m_pScript->popBoolean();
				}
			}
		}
		// 스크립트 모드가 아닐 경우;
		else
		{
			if ( m_pCurMatchingMode )
			{
				m_pCurMatchingMode->OnMapCreate (
					_RoomReceipt->_instanceMapID,
					_RoomReceipt->_bCreateComplete );
			}
		}

		// 방이 안만들어질 경우 다 쫓아낸다;
		if ( _RoomReceipt->_bCreateComplete == false )
		{
			SetGroupToLobbyALL();

			sc::writeLogInfo("MatchingInstanceBase : OnMapCreate FAIL");
		}
		// 인던의 파괴 트리거를 미리 등록한다;
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
				// 방을 완성시킨다;
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

		// 스크립트 모드일 경우;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			lua_State* pState = _RoomReceipt->pState;
			// 새 테이블 생성;
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
		// 스크립트 모드가 아닐 경우;
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

		// 로그를 남긴다;
		if ( _pJoinReceipt && _pJoinReceipt->isFail == true )
		{
			sc::writeLogInfo( "[ Matching Log ] [ FIELD OnJoin FAIL ... ]" );

			// 재시도 로직을 손 볼 필요가 있다;
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
		
		// 스크립트 모드일 경우;
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
		// 스크립트 모드가 아닐 경우;
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

		// 스크립트 모드일 경우;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript && m_pScript->PushFunctionEx( "OnReset" ) == true )
			{
				if ( m_pScript->CallFunction( 0, 1 ) == true )
					return m_pScript->PopBoolean();
			}
		}
		// 스크립트 모드가 아닐 경우;
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
		// 답변을 기다리고 있는 상태가 아니라면;
		// -> 이미 실패한 경우;
		// -> 버그;
		if ( false == m_bWaitingAnswer )
		{
			return 0;
		}

		MatchingSystem::GLMatchingGroupManager* pGroupmanager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupmanager )
			return 0;

		// 한명이라도 실패시 실패 처리;
		if ( false == _ReadyReceipt->isOK )
		{
			_SetAnswerEnded();

			// Waiting 상태를 푼다;
			m_dwAnswerCharacterCount = 0;
			m_bWaitingAnswer = false;
			m_fAnswerTimer = 100.f;

			return 0;
		}

		// 대답한 캐릭터 증가;
		m_dwAnswerCharacterCount++;

		// 모든 유저가 대답했을 경우;
		if ( pGroupmanager->GetCharCountOfMatching( m_dwMatchingInstanceID )
			<= m_dwAnswerCharacterCount )
		{
			// 대답관련 처리;
			// 스크립트 모드일 경우;
			if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
				== m_dwMatchingModeID )
			{
				if ( m_pScript->PushFunctionEx( "OnReadyAnswer" ) == true )
				{
					m_pScript->CallFunction( 0, 0 );
				}
			}
			// 스크립트 모드가 아닐 경우;
			else
			{
				if ( m_pCurMatchingMode )
				{
					m_pCurMatchingMode->OnReadyAnswer( );
				}
			}

			// Waiting 상태를 푼다;
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
						// 스크립트 모드일 경우;
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
						// 스크립트 모드가 아닐 경우;
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
						// 스크립트 모드일 경우;
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
						// 스크립트 모드가 아닐 경우;
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

		// 스크립트 모드일 경우;
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
		// 스크립트 모드가 아닐 경우;
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

		// 스크립트 모드일 경우;
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
		// 스크립트 모드가 아닐 경우;
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

		// 매칭 로그 남기기;
		db::LogTournament* pDbAction = new db::LogTournament( 9004 );

		pDbAction->dwCharDbNum = dwChaNum;
		pDbAction->strChaName = pChar->GetChaName();
		pDbAction->dwFactionID = _FactionID;
		pDbAction->dwForceWin = _IsWinner;

		gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );

		// 스크립트 모드일 경우;
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
		// 스크립트 모드가 아닐 경우;
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
			16개씩 나누는 이유는 메시지 팩이 1MB를 넘어서면안된다.;
			1MB가 넘는 이유는 이름때문이다. 중간에 이름바뀌면 바뀐데로 써야 되기때문.;
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

				// 16개씩 나눈다;
				counting = 16;
				_fbiter = _biter;
				
			}
			else
			{
				counting--;
			}
		}

		// 메시지 보내야될께 조금 더 남았다면;
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