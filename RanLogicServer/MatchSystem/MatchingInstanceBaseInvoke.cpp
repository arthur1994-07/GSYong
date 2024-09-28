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
	/* ��ũ��Ʈ ���� Invoke �Լ�;                                         */
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
			// ���ϰ��� ���ٸ� 0.0,0.0 ���� ����;
		}
		else if ( _secondReturnValue.IsNumber() == false )
		{
			// ���� ���� 1���� ���� ����Ʈ ��ȣ�� ���Ѵ�.;
			const int _nGateID( _firstReturnValue.GetInteger() );

			_targetActorID = Faction::GLActorID(CROW_GATE, _nGateID);
		}
		else
		{
			// ���� ���� 2���� ���� ���� ��ǥ�� ���Ѵ�.;
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
	/* �Ϲ� Invoke �Լ�;                                                   */
	/************************************************************************/

	/// Join ����;
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

		// ��� ��� ����;
		m_bWaitingAnswer = true;
		m_fAnswerTimer = fTime;
		m_dwAnswerCharacterCount = 0;

		// Matching �� ������ �޾ƿ´�;
		VEC_CHARACTER_DBNUM vecChaDBNum;
		pGroupmanager->SearchCharacters_MatchingID(
			vecChaDBNum, m_dwMatchingInstanceID );

		// Matching �� ��� �������� ������ ������;
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


	/// ���� �ź� �����ڸ� �����Ѵ�;
	void MatchingInstanceBase::SetJoinCanselDrop ( bool _JoinCanselDropCondition )
	{
		m_bJoinCancelDropCondition = _JoinCanselDropCondition;
	}





	


	/// ���;
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

		// ��ũ��Ʈ ����� ���;
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


	


	/// �κ��� �׷� ������ ����;
	int MatchingInstanceBase::GetGroupToMatching ( )
	{
		if ( m_bDestroyMatchingInstance )
			return -1;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return -1;

		// ��⿭���� Matching Group �� ���� �����Ѵ�;
		DWORD dwMatchingGroupID =
			pGroupManager->PushMatching(
			GetMatchingInstanceID() );
		if ( INVALID_MATCHING_GROUP_ID == dwMatchingGroupID )
			return -1;

		// ���� ������ �����Ѵ�;
		_GroupClientStatusUpdate( dwMatchingGroupID, EMSTATUS_GROUP_PLAYING );

		/// ��ʸ�Ʈ�� ���� �ӽ÷� ���д�;
		// Matching �� Group ������ ��������;
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

		/// ��ʸ�Ʈ�� ���� �ӽ÷� ���д�;
		m_vecGettingGroup.push_back( dwMatchingGroupID );

		return dwMatchingGroupID;
    }


    void MatchingInstanceBase::SetGroupToPlay ( DWORD GroupID )
    {
        _GroupClientStatusUpdate( GroupID, EMSTATUS_GROUP_PLAYING );
    }


	/// �׷� �κ�� ��������;
	void MatchingInstanceBase::SetGroupToLobby ( DWORD dwMatchingGroupID )
	{
		MatchingSystem::GLMatchingGroupManager* pGroupManager = m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		// ù��° Matching Group �� ��⿭�� �ִ´�;
		if ( false == pGroupManager->PushWaitingQueue( dwMatchingGroupID ) )
			return;

		// ù��° Matching Group �� �κ�� ���´ٴ� ��ũ��Ʈ ����;
		LobbyManagerbase* pLobbyManager = m_pMatchingBase->getLobbyManager();
		if ( NULL == pLobbyManager )
			return;
		pLobbyManager->OnComebackMatchingGroup( dwMatchingGroupID );

		// Matching Group ���� ����;
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
			// ù��° Matching Group �� ��⿭�� �ִ´�;
			DWORD dwMatchingGroupID = 
				pGroupManager->PushWaitingQueue_MatchingFirst(
				GetMatchingInstanceID() );
			if ( INVALID_MATCHING_GROUP_ID == dwMatchingGroupID )
			{
				// ������ ����;
				return;
			}

			// ù��° Matching Group �� �κ�� ���´ٴ� ��ũ��Ʈ ����;
			LobbyManagerbase* pLobbyManager = m_pMatchingBase->getLobbyManager();
			if ( NULL == pLobbyManager )
				return;
			pLobbyManager->OnComebackMatchingGroup( dwMatchingGroupID );

			// Matching Group ���� ����;
			_GroupClientStatusUpdate( dwMatchingGroupID, EMSTATUS_GROUP_IN );
		}
	}


	/// �κ� ��� �׷��� ��ٸ��� �ִ��� �˾ƺ���;
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


	/// �κ񿡼� ��������;
	bool MatchingInstanceBase::OutCharInLobby ( DWORD ClientDBnum )
	{
		LobbyManagerbase* pLobbyMan = m_pMatchingBase->getLobbyManager();
		if ( NULL == pLobbyMan )
			return false;

		pLobbyMan->OnDropOutChar( ClientDBnum );

		return true;
	}


	/// �׷��� �μ���;
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

		// �׷� ID ��ȿ���˻�;
		if ( false == pGroupManager->IsValidMatchingGroup( dwMatchingGroupID ) )
			return;

		// �׷쿡 ���ϴ� ĳ���͸� �˻��Ѵ�;
		VEC_CHARACTER_DBNUM vecChaDBNum;
		pGroupManager->SearchCharacters_MatchingGroupID(
			vecChaDBNum,
			dwMatchingGroupID );

		BOOST_FOREACH ( DWORD dwChaDBNum, vecChaDBNum )
		{
			// ��Ī�ý��ۿ��� �����Ų��;
			MatchingCore::getinstance()
				->OutMatchingSystem(
				m_pMatchingBase->GetScriptID(),
				dwChaDBNum );
		}

		// �׷�Ŵ������� �׷��� �����Ѵ�;
		pGroupManager->DeleteMatchingGroup( dwMatchingGroupID );
	}

	void MatchingInstanceBase::FinishGroupALL ( )
	{
		/// ��ʸ�Ʈ �α׸� �����;
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

		/// �׷� ����;
		{
			MatchingSystem::GLMatchingGroupManager* pGroupManager =
				m_pMatchingBase->getGroupManager();
			if ( NULL == pGroupManager )
				return;

			// �׷쿡 ���ϴ� ĳ���͸� �˻��Ѵ�;
			VEC_CHARACTER_DBNUM vecChaDBNum;
			pGroupManager->SearchCharacters_MatchingID(
				vecChaDBNum,
				m_dwMatchingInstanceID );

			BOOST_FOREACH ( DWORD dwChaDBNum, vecChaDBNum )
			{
				// ��Ī�ý��ۿ��� �����Ų��;
				MatchingCore::getinstance()
					->OutMatchingSystem(
					m_pMatchingBase->GetScriptID(),
					dwChaDBNum );
			}

			// �׷�Ŵ������� �׷��� �����Ѵ�;
			pGroupManager->DeleteMatchingGroupAll_Matching( m_dwMatchingInstanceID );

			/// ��ʸ�Ʈ�� ���Ͽ� �ӽ÷� ���д�;
			m_vecGettingGroup.clear();
		}

		/// Matching �� �����Ѵ�;
		{
			MatchingCore* pMatchingCore = MatchingCore::getinstance();
			if ( NULL == pMatchingCore )
				return;

			pMatchingCore->OutMatchingSystem( m_dwMatchingBaseID );
		}
	}


	/// �Ѽ� ����;
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


	/// �ΰ��� �׷��� �ϳ��� ��ģ��;
	/*DWORD MatchingInstanceBase::MergeGroup ( DWORD _lGroupID, DWORD _rGroupID )
	{
		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return MatchingSystem::INVALID_MATCHING_GROUP_ID;

		GROUP_VEC Temp_vecGroupIDList;
		VitrualGroup* RvGroup = NULL;

		// Right Group ����;
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

		// Left Group ����;
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

		// �� ���� Group ����;
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

	/// ���� �¸��ڸ� �����Ѵ�;
	void MatchingInstanceBase::SetFinalWinner ( DWORD GroupID )
	{
		GLTournamentBetting* pBetting =
			m_pMatchingBase->getBettingManager();
		if ( pBetting )
			pBetting->ResualtWinner( GroupID );
	}




	/// ��Ī ����;
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

		// ��ũ��Ʈ ����� ���;
		if ( static_cast< DWORD >( MatchingGameBase::EMMATCHING_MODE_SCRIPT )
			== m_dwMatchingModeID )
		{
			// ���⼭ Coroutine Lua State�� �����Ѵ�;
			m_pCoroutineState = m_pScript->CallFunctionCoroutine("OnStartMatching");

			sc::writeLogInfo("MatchingInstanceBase : COROUTINE Start");

			int status = m_pScript->CoroutineResume(m_pCoroutineState,0);

			if( status ==  LUA_YIELD)
				m_bUseCoroutine = true;
			else
				m_bUseCoroutine = false;
		}
		// ��ũ��Ʈ ��尡 �ƴ� ���;
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


	/// ������ �غ� �Ǿ����� Ȯ��;
	bool MatchingInstanceBase::IsReadyToStart ( )
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		RoomManagerBase* pRoomManager = m_pMatchingBase->m_Roominstance;
		if ( NULL == pRoomManager )
			return false;

		// ��ũ��Ʈ ����� ���;
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
		// ��ũ��Ʈ ��尡 �ƴ� ���;
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


	/// Matching Instance�� �ı��� �㰡�Ѵ�;
	bool MatchingInstanceBase::DoDestroy ( )
	{
		m_bDestroyMatchingInstance = true;

		return true;
	}


	/// Matching�� ��ŷ���ھ� �α׸� ��´�;
	void MatchingInstanceBase::SetRankScore ( DWORD dbnum, int rating )
	{
		sc::writeLogInfo(
			sc::string::format( "MATCHING: %d Set Ranking Score %d.",
			dbnum,
			rating ) );
	}





	/// �� ����;
	/*bool MatchingInstanceBase::RequestMapCreate ( DWORD _MapID, DWORD _Channel ) 
	{
		if ( m_bDestroyMatchingInstance )
			return false;

		RoomManagerBase* pRoomManager =
			m_pMatchingBase->getRoomManager();

		if ( NULL == pRoomManager )
			return false;

		// Ʈ���Ÿ� �����;
		RoomTrigger* pTrigger = new RoomTrigger( m_dwMatchingInstanceID );
		pTrigger->_instanceID = SNATIVEID( _MapID );
		pTrigger->Channel = _Channel;
		pTrigger->RegistCallback( *this, &MatchingInstanceBase::OnMapCreate );

		// ���� �����;
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

		// Ʈ���Ÿ� �����;
		RoomMultyTrigger* pTrigger = new RoomMultyTrigger( m_dwMatchingInstanceID );
		pTrigger->_MapNeedSize = _dwMapSize;
		pTrigger->_instanceID = SNATIVEID( _dwMapID );
		pTrigger->pState = _pState;
		pTrigger->RegistCallback( *this, &MatchingInstanceBase::OnMapMultiCreate );

		// ���� �����;
		pRoomManager->CreateNewField( pTrigger );

		if ( m_pCurMatchingMode )
		{
			return m_pCurMatchingMode->GetMatchingModeState();
		}

		return 0;
	}


	/// �δ����� ������;
	void MatchingInstanceBase::DoOut ( DWORD _dbNum, DWORD _instanceMapID )
	{
		InstanceSystem::ManagerAgent* pManager =
			MatchingCore::getInstanceManager();
		if ( pManager )
			pManager->DoOut( _dbNum, _instanceMapID );
	}

	


	
	/// ��Ī ���� ����;
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
		// ��Ī �׷� ������ �������� ������;
		// 1����� ���� ��ü ��ʸ�Ʈ ����� �����Ѵ�;
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

		// ���̺�� ���� ������ �����ͼ� ���������ʰ� ���캻��;
		typedef std::vector< KeyPair >	GROUP_Player;		///< �׷� ����;
		typedef GROUP_Player::iterator	GROUP_Player_ITOR;	///< �׷� ���ͷ�����;

		GROUP_Player m_PlayerList;

		// ��� Plus �� �ɽ���!;
		LuaPlus::LuaObject FeaturesList( LuaPlus::LuaState::CastState(pState), -1 );

		// ���̺� ���� üũ
		if(!FeaturesList.IsTable())
		{
			// �ƴϸ� ����;
			sc::writeLogInfo("MATCHING: NOT TALBE");

			return false;
		}

		// ���̺� üũ�� ���ͷ����ͷ� ���̺� �˻� ����		
		for (LuaPlus::LuaTableIterator it(FeaturesList); it; it.Next())
		{
			if(it.GetValue().IsNil() == true)
				continue;
			KeyPair _keypair;
			_keypair.Index = (int)it.GetKey().GetNumber();
			_keypair.GroupID = (int)it.GetValue().GetNumber();
			m_PlayerList.push_back(_keypair);
		}


		// ��ƿ��� �ҷ��� �׷������� ���� Vector�� �˻��ϸ鼭
		// ��ϵ������� ���ο� �༮�� ã�Ƽ� ������ش�.
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
				//�̹� ����ѻ������ �����Ѵ�!
				GLMatChingGroupInfo* TempNode= &*findbiter;
				//�׷� ã�����ϱ� �ȳ־�д�.
				if(item->Index == TempNode->Index)
				{
					isfind = true;
					break;
				}
			}

			//ã�����ߴٸ� ���ο� �༮�̴� �ٽ� �־�����.
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

		// Ŭ���̾�Ʈ�� ������ �ִ� ������ ��� �����ڷ�� ���� �����Ͽ���.
		GLMSG::NET_MATCHING_GROUPTABLEINFO_OUTOFDATE _massage(m_pMatchingBase->GetScriptID());
		MatchingCore::getinstance()->_sendMessageToALLClient(&_massage);

		return 0;
	}
}