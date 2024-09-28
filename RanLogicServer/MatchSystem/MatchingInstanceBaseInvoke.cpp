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

#include "./MatchingGameBase.h"
#include "./MatchingInstanceBase.h"


namespace MatchingSystem
{

	/************************************************************************/
	/* 스크립트 종속 Invoke 함수;                                         */
	/************************************************************************/
	
	/// Script Invoke Function;
	int	MatchingInstanceBase::ScriptInvoke_SendJoin ( lua_State* pState )
	{
		if ( m_bDestroyMatchingInstance )
			return 0;

		int currentStack ( 1 );

		const lua_Integer GroupID(lua_tointeger(pState, currentStack++));
		const lua_Integer _instanceID(lua_tointeger(pState, currentStack++));
		LuaPlus::LuaObject _firstReturnValue(LuaPlus::LuaState::CastState(pState), currentStack++);
		LuaPlus::LuaObject _secondReturnValue(LuaPlus::LuaState::CastState(pState), currentStack++);

		Faction::GLActorID _targetActorID = Faction::GLActorID();

		float _positionX = 0.0f;
		float _positionZ = 0.0f;

		if ( _firstReturnValue.IsNumber() == false )
		{
			// 리턴값이 없다면 0.0,0.0 으로 입장;
		}
		else if ( _secondReturnValue.IsNumber() == false )
		{
			// 리턴 값이 1개일 때는 게이트 번호를 뜻한다.;
			const int _nGateID( _firstReturnValue.GetInteger() );

			_targetActorID = Faction::GLActorID(CROW_GATE, _nGateID);
		}
		else
		{
			// 리턴 값이 2개일 때는 월드 좌표를 뜻한다.;
			const float _fPositionX( _firstReturnValue.GetFloat() );
			const float _fPositionZ( _secondReturnValue.GetFloat() );

			_positionX = _fPositionX;
			_positionZ = _fPositionZ;
		}

		return SendJoin( GroupID, _instanceID, _targetActorID, _positionX, _positionZ );
	}


	/*int MatchingInstanceBase::ScriptInvoke_SendToGroupAskJoinAsyc ( lua_State* L )
	{
		if ( NULL == m_pScript )
			return 0;

		const float fTimer( lua_tonumber(L, -1) );
		lua_pop(L, 1);

		sc::writeLogInfo("SendToGroupAskJoinAsyc : Start coroutine_yield");

		SendToGroupAskJoinAsyc( fTimer );

		return m_pScript->coroutine_yield( L, 0 );
	}*/


	int MatchingInstanceBase::ScriptInvoke_WaitTime ( lua_State* L )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		const float _fDuration( lua_tonumber( L, -1 ) );
		lua_pop( L, 1 );

		WaitTime( _fDuration );
	}


	int MatchingInstanceBase::ScriptInvoke_MapMultiCreate ( lua_State* pState )
	{
		sc::writeLogInfo("MatchingInstanceBase : MapMultyCreate");

		const lua_Integer _MapID = lua_tointeger( pState, -1 );
		lua_pop( pState, 1 );

		const lua_Integer _MapSize = lua_tointeger( pState, -1 );
		lua_pop( pState, 1 );

		if ( _MapSize < 0 )
		{
			lua_pushnil( pState );
			return 1;
		}

		int result = m_pScript->CoroutineYield( pState, 0 );

		MapMultiCreate( _MapID, _MapSize, pState );
		
		return result;
	}





	/************************************************************************/
	/* 일반 Invoke 함수;                                                   */
	/************************************************************************/

	/// Join 관련;
	/*bool MatchingInstanceBase::SetGroupAskJoin ( DWORD GroupID )
	{
		BOOST_FOREACH ( DWORD Node, m_vecJoinSaved )
		{
			if ( Node == GroupID )
				return false;
		}

		m_vecJoinSaved.push_back ( GroupID );

		return true;
	}*/


	/*int MatchingInstanceBase::SendToGroupAskJoinAsyc ( float _fTimer )
	{
		if ( m_vecJoinSaved.size() == 0 )
			return 0;

		m_fAnswerTimer = _fTimer;
		m_dwAnswerCount = 0;

		BOOST_FOREACH( DWORD Node, m_vecJoinSaved )
		{
			SendToGroupAskJoin( Node, _fTimer, true );
		}

		if ( NULL == m_pCurMatchingMode )
			return 0;

		return m_pCurMatchingMode->GetMatchingModeState();
	}*/


	bool MatchingInstanceBase::SendToGroupAskJoin ( float fTime )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		MatchingSystem::GLMatchingGroupManager* pGroupmanager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupmanager )
			return false;

		// 대기 모드 시작;
		m_bWaitingAnswer = true;
		m_fAnswerTimer = fTime;
		m_dwAnswerCharacterCount = 0;

		// Matching 의 유저를 받아온다;
		VEC_CHARACTER_DBNUM vecChaDBNum;
		pGroupmanager->SearchCharacters_MatchingID(
			vecChaDBNum, m_dwMatchingInstanceID );

		// Matching 의 모든 유저에게 질문을 보낸다;
		BOOST_FOREACH ( DWORD dwChaDBNum, vecChaDBNum )
		{
			_SendToClientAskJoin( dwChaDBNum, fTime );
		}

		vecChaDBNum.clear();

		return true;
	}


	int MatchingInstanceBase::SendJoin (
		DWORD _dwGroupID,
		DWORD _dwInstanceID,
		Faction::GLActorID _targetActorID,
		float _fPositionX,
		float _fPositionZ )
	{
		if ( NULL == m_pMatchingBase )
			return 0;

		std::vector< DWORD > clientlist;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( pGroupManager )
			pGroupManager->SearchCharacters_MatchingGroupID( clientlist, _dwGroupID );

		InstanceSystem::ManagerAgent* pManager =
			MatchingCore::getInstanceManager();
		if ( NULL == pManager )
			return 0;

		m_pMatchingBase->getRoomManager()->JoinComplete(
			_dwGroupID, _dwInstanceID);

		BOOST_FOREACH ( DWORD ClientDBnum, clientlist )
		{
			JoinTrigger* pTrigger = new JoinTrigger( m_dwMatchingInstanceID );
			pTrigger->_ClientDBID = ClientDBnum;
			pTrigger->_instanceMapID = _dwInstanceID;
			pTrigger->RegistCallback( *this, &MatchingInstanceBase::OnJoin );

			InstanceSystem::InformationPosition sInformationPosition(
				Faction::GLActorID( CROW_PC, ClientDBnum ),
				SNATIVEID( _dwInstanceID, 0 ),
				_targetActorID,
				_fPositionX,
				_fPositionZ,
				InstanceSystem::EMMOVE_FIELD2INSTANCE );

			bool bJoinSucess = pManager->DoJoin(
				ClientDBnum,
				sInformationPosition,
				InstanceSystem::EMAUTHORITY_NORMAL_FORCED,
				pTrigger );

			m_nJoinCount++;

			sc::writeLogInfo( sc::string::format(
				"MatchingInstanceBase: USER %d, SendJoin %d",
				ClientDBnum,
				_dwInstanceID ) );

			if ( bJoinSucess == false )
			{	
				m_JoinFailTrigger.push_back( pTrigger );

				sc::writeLogInfo( "MatchingInstanceBase : SendJoin FAIL" );
			}
		}

		clientlist.clear();

		return 0;
	}


	/// Join Lock;
	void MatchingInstanceBase::JoinLock ( DWORD _instanceMapID, const bool _bLock )
	{
		InstanceSystem::ManagerAgent* pManager = MatchingCore::getInstanceManager();
		if ( pManager )
			pManager->joinLock( _instanceMapID, _bLock );
	}


	/// Join Suspend
	void MatchingInstanceBase::JoinSuspend ( DWORD _instanceMapID, const bool _bSuspend )
	{
		InstanceSystem::ManagerAgent* pManager = MatchingCore::getInstanceManager();
		if ( pManager )
			pManager->joinSuspend( _instanceMapID, _bSuspend );
	}


	/// 조인 거부 상태자를 세팅한다;
	void MatchingInstanceBase::SetJoinCanselDrop ( bool _JoinCanselDropCondition )
	{
		m_bJoinCancelDropCondition = _JoinCanselDropCondition;
	}





	


	/// 대기;
	int MatchingInstanceBase::WaitTime ( float _fDuration )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		m_fWaitDuration = _fDuration;
		m_bSetWait = true;

		if ( true == m_bUseCoroutine )
		{
			m_fWaitDuration = 0.f;
			m_bSetWait = false;

			sc::writeLogError("Already UseCorutine");

			return 0;
		}

		// 스크립트 모드일 경우;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			return m_pScript->CoroutineYield( m_pCoroutineState, 0 );
		}
		else
		{
			if ( m_pCurMatchingMode )
				return m_pCurMatchingMode->GetMatchingModeState();
		}

		return 0;
	}


	


	/// 로비의 그룹 가지고 오기;
	int MatchingInstanceBase::GetGroupToMatching ( )
	{
		if ( m_bDestroyMatchingInstance )
			return -1;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return -1;

		// 대기열에서 Matching Group 을 꺼내 세팅한다;
		DWORD dwMatchingGroupID =
			pGroupManager->PushMatching(
			GetMatchingInstanceID() );
		if ( INVALID_MATCHING_GROUP_ID == dwMatchingGroupID )
			return -1;

		// 유저 정보를 갱신한다;
		_GroupClientStatusUpdate( dwMatchingGroupID, EMSTATUS_GROUP_PLAYING );

		/// 토너먼트로 인해 임시로 놔둔다;
		// Matching 의 Group 정보를 갱신힌다;
		m_vecGettingGroup.push_back( dwMatchingGroupID );

		return dwMatchingGroupID;
	}


	int MatchingInstanceBase::GetGroupToMatchingNotPlay ( )
    {
		if ( m_bDestroyMatchingInstance )
			return -1;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return -1;

		DWORD dwMatchingGroupID =
			pGroupManager->PushMatching(
			GetMatchingInstanceID() );
		if ( INVALID_MATCHING_GROUP_ID == dwMatchingGroupID )
			return -1;

		/// 토너먼트로 인해 임시로 놔둔다;
		m_vecGettingGroup.push_back( dwMatchingGroupID );

		return dwMatchingGroupID;
    }


    void MatchingInstanceBase::SetGroupToPlay ( DWORD GroupID )
    {
        _GroupClientStatusUpdate( GroupID, EMSTATUS_GROUP_PLAYING );
    }


	/// 그룹 로비로 내보내기;
	void MatchingInstanceBase::SetGroupToLobby ( DWORD dwMatchingGroupID )
	{
		MatchingSystem::GLMatchingGroupManager* pGroupManager = m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		// 첫번째 Matching Group 을 대기열에 넣는다;
		if ( false == pGroupManager->PushWaitingQueue( dwMatchingGroupID ) )
			return;

		// 첫번째 Matching Group 을 로비로 보냈다는 스크립트 실행;
		LobbyManagerbase* pLobbyManager = m_pMatchingBase->getLobbyManager();
		if ( NULL == pLobbyManager )
			return;
		pLobbyManager->OnComebackMatchingGroup( dwMatchingGroupID );

		// Matching Group 상태 갱신;
		_GroupClientStatusUpdate( dwMatchingGroupID, EMSTATUS_GROUP_IN );
	}


	void MatchingInstanceBase::SetGroupToLobbyALL ( )
	{
		if ( NULL == m_pMatchingBase )
			return;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		for ( int i=0; i<GLMatchingGroupManager::MATCHING_GROUP_SIZE; ++i )
		{
			// 첫번째 Matching Group 을 대기열에 넣는다;
			DWORD dwMatchingGroupID = 
				pGroupManager->PushWaitingQueue_MatchingFirst(
				GetMatchingInstanceID() );
			if ( INVALID_MATCHING_GROUP_ID == dwMatchingGroupID )
			{
				// 없으면 리턴;
				return;
			}

			// 첫번째 Matching Group 을 로비로 보냈다는 스크립트 실행;
			LobbyManagerbase* pLobbyManager = m_pMatchingBase->getLobbyManager();
			if ( NULL == pLobbyManager )
				return;
			pLobbyManager->OnComebackMatchingGroup( dwMatchingGroupID );

			// Matching Group 상태 갱신;
			_GroupClientStatusUpdate( dwMatchingGroupID, EMSTATUS_GROUP_IN );
		}
	}


	/// 로비에 몇개의 그룹이 기다리고 있는지 알아본다;
	int MatchingInstanceBase::GetMatchingWaitingGroupCount ( )
	{
		if ( m_bDestroyMatchingInstance )
			return 0;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return 0;

		return pGroupManager->GetSizeOfWaitingQueue();
	}


	/// 로비에서 내보낸다;
	bool MatchingInstanceBase::OutCharInLobby ( DWORD ClientDBnum )
	{
		LobbyManagerbase* pLobbyMan = m_pMatchingBase->getLobbyManager();
		if ( NULL == pLobbyMan )
			return false;

		pLobbyMan->OnDropOutChar( ClientDBnum );

		return true;
	}


	/// 그룹을 부순다;
	void MatchingInstanceBase::SemiFinishGroup ( DWORD GroupID )
	{
		MatchingSystem::GLMatchingGroupManager* pGroupManager = m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		pGroupManager->DeleteMatchingGroup( GroupID );
	}

	void MatchingInstanceBase::FinishGroup ( DWORD dwMatchingGroupID )
	{		
		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		// 그룹 ID 유효성검사;
		if ( false == pGroupManager->IsValidMatchingGroup( dwMatchingGroupID ) )
			return;

		// 그룹에 속하는 캐릭터를 검색한다;
		VEC_CHARACTER_DBNUM vecChaDBNum;
		pGroupManager->SearchCharacters_MatchingGroupID(
			vecChaDBNum,
			dwMatchingGroupID );

		BOOST_FOREACH ( DWORD dwChaDBNum, vecChaDBNum )
		{
			// 매칭시스템에서 퇴장시킨다;
			MatchingCore::getinstance()
				->OutMatchingSystem(
				m_pMatchingBase->GetScriptID(),
				dwChaDBNum );
		}

		// 그룹매니저에서 그룹을 삭제한다;
		pGroupManager->DeleteMatchingGroup( dwMatchingGroupID );
	}

	void MatchingInstanceBase::FinishGroupALL ( )
	{
		/// 토너먼트 로그를 남긴다;
		{
			db::LogTournament* pDbAction = new db::LogTournament( 9005 );
			if ( NULL == pDbAction )
				return;

			RoomManagerBase* pRoomManager = m_pMatchingBase->m_Roominstance;
			if ( NULL == pRoomManager )
				return;

			GLMatchingInfo* pMatchingInfo = pRoomManager->GetGLMatchingInfo();
			if ( NULL == pMatchingInfo )
				return;

			pDbAction->strContentName = pMatchingInfo->m_strContentsName.c_str();
			pDbAction->dwCanObserve = pMatchingInfo->m_bCanObserve;   
			pDbAction->dwCanToto = pMatchingInfo->m_bCanTOTO;   

			if ( gpAgentServer )
				gpAgentServer->AddLogAdoJob( db::DbActionPtr( pDbAction ) );
		}

		/// 그룹 제거;
		{
			MatchingSystem::GLMatchingGroupManager* pGroupManager =
				m_pMatchingBase->getGroupManager();
			if ( NULL == pGroupManager )
				return;

			// 그룹에 속하는 캐릭터를 검색한다;
			VEC_CHARACTER_DBNUM vecChaDBNum;
			pGroupManager->SearchCharacters_MatchingID(
				vecChaDBNum,
				m_dwMatchingInstanceID );

			BOOST_FOREACH ( DWORD dwChaDBNum, vecChaDBNum )
			{
				// 매칭시스템에서 퇴장시킨다;
				MatchingCore::getinstance()
					->OutMatchingSystem(
					m_pMatchingBase->GetScriptID(),
					dwChaDBNum );
			}

			// 그룹매니저에서 그룹을 삭제한다;
			pGroupManager->DeleteMatchingGroupAll_Matching( m_dwMatchingInstanceID );

			/// 토너먼트로 인하여 임시로 놔둔다;
			m_vecGettingGroup.clear();
		}

		/// Matching 을 제거한다;
		{
			MatchingCore* pMatchingCore = MatchingCore::getinstance();
			if ( NULL == pMatchingCore )
				return;

			pMatchingCore->OutMatchingSystem( m_dwMatchingBaseID );
		}
	}


	/// 팩션 설정;
	void MatchingInstanceBase::SendFaction (
		DWORD GroupID,
		DWORD _FactionID,
		DWORD _instanceID )
	{
		if ( m_bDestroyMatchingInstance )
			return;

		std::vector< DWORD > clientlist;

		MatchingSystem::GLMatchingGroupManager* pGroupManager = m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		pGroupManager->SearchCharacters_MatchingGroupID( clientlist, GroupID );

		InstanceSystem::ManagerAgent* pManager = MatchingCore::getInstanceManager();
		if ( NULL == pManager )
			return;

		RoomManagerBase* pRoomManager = m_pMatchingBase->m_Roominstance;
		if ( NULL == pRoomManager )
			return;

		GLMatchingInfo* pMatchingInfo = pRoomManager->GetGLMatchingInfo();
		if ( NULL == pMatchingInfo )
			return;

		BOOST_FOREACH ( DWORD ClientDBnum, clientlist )
		{
			sc::writeLogInfo( sc::string::format(
				"MatchingInstanceBase:SendFaction USER %1%, Faction %2%, MAP %3%",
				ClientDBnum,
				_FactionID,
				_instanceID ) );

			GLCharAG* const _pPlayer( gpAgentServer->GetCharByDbNum( ClientDBnum ) );
			if ( NULL == _pPlayer )
				return;

			bool isSucess = pManager->requestFaction(
				ClientDBnum,
				_FactionID,
				_instanceID,
				Faction::EMFACTION_TEAM );

			if ( false == isSucess )
			{
				SetGroupToLobbyALL();

				sc::writeLogInfo( "MatchingInstanceBase : SendFaction FAIL" );

				return;
			}

			std::string strContentName = pMatchingInfo->m_strContentsName;

			if ( strContentName.compare("TOURNAMENT_PVP_SINGLE_MAPNAME") == 0 ||
				strContentName.compare("TOURNAMENT_PVP_TEAM_MAPNAME") == 0 )
			{
				db::LogTournament* pDbAction = new db::LogTournament( 9002 );
				if ( NULL == pDbAction )
					return;

				pDbAction->dwCharDbNum = ClientDBnum;   
				pDbAction->strChaName = _pPlayer->GetChaName();
				pDbAction->dwFactionID = _FactionID;
				pDbAction->dwMapID = _instanceID;
				pDbAction->dwClass = _pPlayer->GetClass();
				pDbAction->dwSchool =  _pPlayer->GetSchool();      
				pDbAction->dwPartyID = _pPlayer->GetPartyID().GetPartyID();     

				if ( gpAgentServer)
					gpAgentServer->AddLogAdoJob( db::DbActionPtr( pDbAction ) );
			}
		}

		clientlist.clear();
	}


	/// 두개의 그룹을 하나로 합친다;
	/*DWORD MatchingInstanceBase::MergeGroup ( DWORD _lGroupID, DWORD _rGroupID )
	{
		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return MatchingSystem::INVALID_MATCHING_GROUP_ID;

		GROUP_VEC Temp_vecGroupIDList;
		VitrualGroup* RvGroup = NULL;

		// Right Group 삭제;
		{
			std::vector<VitrualGroup*>::iterator biter =
				m_vecVitualGroupList.begin();
			for ( ; biter != m_vecVitualGroupList.end(); ++biter )
			{
				RvGroup = *biter;

				if ( RvGroup->getGroupID() == _rGroupID )
				{
					BOOST_FOREACH ( DWORD node, RvGroup->m_vecGroupIDList )
					{
						Temp_vecGroupIDList.push_back( node );
					}
					biter = m_vecVitualGroupList.erase( biter );
					SAFE_DELETE( RvGroup );
				}			
			}
		}

		// Left Group 삭제;
		{
			std::vector<VitrualGroup*>::iterator biter =
				m_vecVitualGroupList.begin();
			for( ; biter != m_vecVitualGroupList.end(); ++biter )
			{
				RvGroup = *biter;

				if ( RvGroup->getGroupID() == _lGroupID )
				{
					BOOST_FOREACH ( DWORD node, RvGroup->m_vecGroupIDList )
					{
						Temp_vecGroupIDList.push_back( node );
					}
					biter = m_vecVitualGroupList.erase( biter );
					SAFE_DELETE(RvGroup);
				}			
			}
		}

		// 두 개의 Group 병합;
		VitrualGroup* vGroup = NULL;
		DWORD newGroupID = pGroupManager->MergeMatchingGroup( _lGroupID, _rGroupID );

		vGroup = new VitrualGroup( newGroupID );

		BOOST_FOREACH( DWORD node, Temp_vecGroupIDList )
		{
			vGroup->m_vecGroupIDList.push_back( node );
		}

		m_vecVitualGroupList.push_back( vGroup );

		return newGroupID;
	}*/

	/// 최종 승리자를 설정한다;
	void MatchingInstanceBase::SetFinalWinner ( DWORD GroupID )
	{
		GLTournamentBetting* pBetting =
			m_pMatchingBase->getBettingManager();
		if ( pBetting )
			pBetting->ResualtWinner( GroupID );
	}




	/// 매칭 시작;
	bool MatchingInstanceBase::StartMatching ( )
	{
		if ( m_bDestroyMatchingInstance )
		{
			sc::writeLogError( "Already Destroyed Matching!!" );

			return false;
		}

		if ( m_bUseCoroutine == true )
		{
			sc::writeLogError("Already Use Coroutine");
			return false;
		}

		// 스크립트 모드일 경우;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			// 여기서 Coroutine Lua State를 생성한다;
			m_pCoroutineState = m_pScript->CallFunctionCoroutine("OnStartMatching");

			sc::writeLogInfo("MatchingInstanceBase : COROUTINE Start");

			int status = m_pScript->CoroutineResume(m_pCoroutineState,0);

			if( status ==  LUA_YIELD)
				m_bUseCoroutine = true;
			else
				m_bUseCoroutine = false;
		}
		// 스크립트 모드가 아닐 경우;
		else
		{
			if ( m_pCurMatchingMode )
			{
				m_pCurMatchingMode->StartMatching();

				m_pCurMatchingMode->MatchingModeResume();

				if ( m_pCurMatchingMode->GetMatchingModeState() )
					m_bUseCoroutine = true;
				else
					m_bUseCoroutine = false;
			}
		}

		return true;
	}


	/// 시작할 준비가 되었는지 확인;
	bool MatchingInstanceBase::IsReadyToStart ( )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		RoomManagerBase* pRoomManager = m_pMatchingBase->m_Roominstance;
		if ( NULL == pRoomManager )
			return false;

		// 스크립트 모드일 경우;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			if ( m_pScript && m_pScript->PushFunctionEx( "isReadyToStart" ) == true )
			{
				if ( m_pScript->CallFunction( 0, 1 ) == true )
					return m_pScript->PopBoolean();
			}
			else
			{
				if ( pRoomManager->HasMyMap( m_dwMatchingInstanceID ) )
					return false;
				else
					return true;
			}
		}
		// 스크립트 모드가 아닐 경우;
		else
		{
			if ( pRoomManager->HasMyMap( m_dwMatchingInstanceID ) )
				return false;
			else
				return true;
		}

		return false;
	}


	void MatchingInstanceBase::SetMatchingStatus ( DWORD _Status )
	{
		RoomManagerBase* pRoomManager = m_pMatchingBase->m_Roominstance;
		if ( NULL == pRoomManager )
			return;

		pRoomManager->Refresh( _Status );
	}


	void MatchingInstanceBase::SetMatchingPlayTime ( DWORD _TimeH, DWORD _TimeM )
	{

	}


	/// Matching Instance의 파괴를 허가한다;
	bool MatchingInstanceBase::DoDestroy ( )
	{
		m_bDestroyMatchingInstance = true;

		return true;
	}


	/// Matching의 랭킹스코어 로그를 찍는다;
	void MatchingInstanceBase::SetRankScore ( DWORD dbnum, int rating )
	{
		sc::writeLogInfo(
			sc::string::format( "MATCHING: %d Set Ranking Score %d.",
			dbnum,
			rating ) );
	}





	/// 맵 생성;
	/*bool MatchingInstanceBase::RequestMapCreate ( DWORD _MapID, DWORD _Channel ) 
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		RoomManagerBase* pRoomManager =
			m_pMatchingBase->getRoomManager();

		if ( NULL == pRoomManager )
			return false;

		// 트리거를 만든다;
		RoomTrigger* pTrigger = new RoomTrigger( m_dwMatchingInstanceID );
		pTrigger->_instanceID = SNATIVEID( _MapID );
		pTrigger->Channel = _Channel;
		pTrigger->RegistCallback( *this, &MatchingInstanceBase::OnMapCreate );

		// 방을 만든다;
		pRoomManager->CreateNewField( pTrigger );

		return true;
	}*/

	int MatchingInstanceBase::MapMultiCreate ( DWORD _dwMapID, DWORD _dwMapSize, lua_State* _pState )
	{
		RoomManagerBase* pRoomManager =
			m_pMatchingBase->getRoomManager();

		if ( NULL == pRoomManager )
		{
			sc::writeLogError(
				"[ Matching Log ] [ Failed MatchingInstanceBase::MapMultyCreate!! ]" );

			return -1;
		}

		if ( m_pCurMatchingMode )
		{
			m_pCurMatchingMode->MatchingModeYield();
		}

		// 트리거를 만든다;
		RoomMultyTrigger* pTrigger = new RoomMultyTrigger( m_dwMatchingInstanceID );
		pTrigger->_MapNeedSize = _dwMapSize;
		pTrigger->_instanceID = SNATIVEID( _dwMapID );
		pTrigger->pState = _pState;
		pTrigger->RegistCallback( *this, &MatchingInstanceBase::OnMapMultiCreate );

		// 방을 만든다;
		pRoomManager->CreateNewField( pTrigger );

		if ( m_pCurMatchingMode )
		{
			return m_pCurMatchingMode->GetMatchingModeState();
		}

		return 0;
	}


	/// 인던에서 나간다;
	void MatchingInstanceBase::DoOut ( DWORD _dbNum, DWORD _instanceMapID )
	{
		InstanceSystem::ManagerAgent* pManager =
			MatchingCore::getInstanceManager();
		if ( pManager )
			pManager->DoOut( _dbNum, _instanceMapID );
	}

	


	
	/// 매칭 관련 세팅;
	void MatchingInstanceBase::SetMatchingRound ( DWORD _Round )
	{
		m_dwMatchingRound = _Round;
	}


	void MatchingInstanceBase::SetMatchingSize ( DWORD _dwMatchingSize )
	{
		m_dwMatchingSize = _dwMatchingSize;
	}


	void MatchingInstanceBase::SetMatchingPlayersReset ( )
	{
		if ( m_vecMatchingGroupInfo.size() != 0 )
		{
			std::vector< GLMatChingGroupInfo >::iterator _biter =
				m_vecMatchingGroupInfo.begin();
			for ( ; _biter != m_vecMatchingGroupInfo.end(); )
			{
				_biter = m_vecMatchingGroupInfo.erase( _biter );
			}
		}

		m_dwMatchingSize = 0;

		m_vecMatchingGroupInfo.clear();
	}


	int MatchingInstanceBase::SetMatchingPlayers ( lua_State* pState )
	{
		// 매칭 그룹 정보가 존재하지 않을때;
		// 1라운드로 보고 전체 토너먼트 사이즈를 결정한다;
		unsigned int RoundCounting = 2;

		if ( m_vecMatchingGroupInfo.size() == 0 )
		{
			for ( RoundCounting = 2; RoundCounting < m_vecGettingGroup.size(); RoundCounting*=2 )
			{
				m_dwMatchingSize = RoundCounting;
			}

			m_dwMatchingSize  = RoundCounting ;
		}
		else
		{
			RoundCounting = m_dwMatchingSize;
		}

		// 테이블로 보낸 값들을 가져와서 수정하지않고 살펴본다;
		typedef std::vector< KeyPair >	GROUP_Player;		///< 그룹 벡터;
		typedef GROUP_Player::iterator	GROUP_Player_ITOR;	///< 그룹 이터레이터;

		GROUP_Player m_PlayerList;

		// 루아 Plus 로 케스팅!;
		LuaPlus::LuaObject FeaturesList( LuaPlus::LuaState::CastState(pState), -1 );

		// 테이블 인지 체크
		if(!FeaturesList.IsTable())
		{
			// 아니면 오류;
			sc::writeLogInfo("MATCHING: NOT TALBE");

			return false;
		}

		// 테이블 체크후 이터레이터로 테이블 검색 때림		
		for (LuaPlus::LuaTableIterator it(FeaturesList); it; it.Next())
		{
			if(it.GetValue().IsNil() == true)
				continue;
			KeyPair _keypair;
			_keypair.Index = (int)it.GetKey().GetNumber();
			_keypair.GroupID = (int)it.GetValue().GetNumber();
			m_PlayerList.push_back(_keypair);
		}


		// 루아에서 불러온 그룹정보를 담은 Vector를 검색하면서
		// 등록되지않은 새로운 녀석을 찾아서 등록해준다.
		GROUP_Player_ITOR biter = m_PlayerList.begin();
		GROUP_Player_ITOR eiter = m_PlayerList.end();
		for(;biter != eiter; ++biter)
		{
			bool isfind = false;
			KeyPair* item = &*biter;
			std::vector<GLMatChingGroupInfo>::iterator findbiter = m_vecMatchingGroupInfo.begin();
			std::vector<GLMatChingGroupInfo>::iterator findend = m_vecMatchingGroupInfo.end();
			for(;findbiter != findend; ++findbiter)
			{
				//이미 등록한사람들중 존재한다!
				GLMatChingGroupInfo* TempNode= &*findbiter;
				//그럼 찾았으니깐 안넣어둔다.
				if(item->Index == TempNode->Index)
				{
					isfind = true;
					break;
				}
			}

			//찾지못했다면 새로운 녀석이다 다시 넣어주자.
			if(isfind == false)
			{
				int _Round = 1;

				int TotalCount = m_dwMatchingSize;

				for(int i = m_dwMatchingSize/2; TotalCount < item->Index; i/=2 )
				{
					if(i == 0)
						return false;

					TotalCount += i;
					_Round +=1;
				}

				_SetMatchingPlayerTable(*biter,_Round);
			}
		}

		// 클라이언트가 가지고 있는 정보는 모두 옜날자료니 새로 갱신하여라.
		GLMSG::NET_MATCHING_GROUPTABLEINFO_OUTOFDATE _massage(m_pMatchingBase->GetScriptID());
		MatchingCore::getinstance()->_sendMessageToALLClient(&_massage);

		return 0;
	}
}