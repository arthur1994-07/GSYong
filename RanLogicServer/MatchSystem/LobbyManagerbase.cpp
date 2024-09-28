#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"

#include "../../enginelib/GUInterface/UITextControl.h"

#include "../../RanLogic/Tournament/GLGroupManager.h"
#include "../../RanLogic/InstanceSystem/GLInstanceSystem.h"
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

#include "../TournamentManager/MatchingJoinManager.h"

#include "../InstanceSystem/InstanceSystem.h"
#include "../InstanceSystem/LogicAgent/InvokeFunctionAgent.h"

#include "../Party/GLPartyField.h"
#include "../Party/GLPartyManagerField.h"

#include "./MatchingManager.h"
#include "./MatchingGameBase.h"

#include "./LobbyManagerBase.h"


namespace MatchingSystem
{
	LobbyManagerbase::LobbyManagerbase (
		MatchingGameBase* pMatchingBase,
		sc::LuaInstance* pScript )
		: m_pScript ( pScript )
		, m_pMatchingBase ( pMatchingBase )
		, m_fTime ( 0.f )
	{
	}


	LobbyManagerbase::~LobbyManagerbase ( )
	{
		if ( m_pScript )
			m_pScript->OnDestroy();

		SAFE_DELETE( m_pScript );
	}


	void LobbyManagerbase::RegistCallbackFunction_Lua ( sc::LuaInstance* pScript )
	{
		pScript->registdirectFunction( "IsValidGroup", *this, &LobbyManagerbase::IsValidMatchingGroup );							// 그룹이 올바른지 체크;
		pScript->registdirectFunction( "DeleteGroup", *this, &LobbyManagerbase::DeleteMatchingGroup );							// 그룹 삭제;
		pScript->registdirectFunction( "FindGroup", *this, &LobbyManagerbase::SearchMatchingGroup );								// 그룹 찾기;
		pScript->registdirectFunction( "GetCountTournamentPeople", *this, &LobbyManagerbase::GetCountTournamentPeople ); // 토너먼트 참여자 갯수 가져오기;
		pScript->registdirectFunction( "IsJoinGame", *this, &LobbyManagerbase::IsJoinGame );								// 게임이 진입을 한적이 있는지 물어봄 (해당 매칭만);
	}


	/************************************************************************/
	/* Callback Function;                                                   */
	/************************************************************************/

	void LobbyManagerbase::OnInitialize ( )
	{
		if ( NULL == m_pMatchingBase )
			return;

		MatchingSystem::GLMatchingGroupManager* pGroupmanager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupmanager )
			return;

		pGroupmanager->RegistCallBackFunction_Lua( m_pScript );

		// 인던 시스템 함수 등록;
		InstanceSystem::registFunctionCommon( *m_pScript );
		InstanceSystem::registFunctionAgent( *m_pScript );

		// 게임베이스 공용함수 등록 ( 전장점수 등 );
		m_pMatchingBase->RegistFunctionGameBase( m_pScript );

		m_pScript->registdirectFunction( "TournamentStart", *this, &LobbyManagerbase::StartMatching );				// 토너먼트 시작하기;
		m_pScript->registdirectFunction( "GetSchoolParticipation", *this, &LobbyManagerbase::GetSchoolParticipation );	// 학교별 참여자 갯수 가져오기;
		m_pScript->registdirectFunction( "SendMatchingQueue", *this, &LobbyManagerbase::JoinCharacter_TempLobby );			// 매칭 큐를 시작하기;
		m_pScript->registdirectFunction( "EndTournamentQueue", *this, &LobbyManagerbase::MakeMatchingGroup );			// 매칭 큐를 끝내기;
		m_pScript->registdirectFunction( "SpendMoney", *this, &LobbyManagerbase::SpendMoney );							// 돈 써버리기;
		m_pScript->registdirectFunction( "CheckMoney", *this, &LobbyManagerbase::CheckMoney );							// 돈 체크하기;
	}


	void LobbyManagerbase::OnFrameMove ( float fElpaseTime )
	{
		m_fTime += fElpaseTime;

		// 3초에 한번씩 매칭할 준비가 되었는지 확인한다;
		if ( m_fTime > 3.f )
		{
			OnReqReadyMatching( m_fTime );
			m_fTime = 0.f;
		}
	}


	bool LobbyManagerbase::OnReqReadyMatching ( float fElpaseTime )
	{
		if ( m_pScript->PushFunctionEx( "reqTournamentReady" ) == true )
		{
			m_pScript->PushNumber( fElpaseTime );

			if ( m_pScript->CallFunction( 1, 1 ) == true )
			{
				return m_pScript->PopBoolean();
			}
		}

		return false;
	}


	bool LobbyManagerbase::OnReqJoinMatching ( DWORD dwChaDBNum )
	{
		if ( NULL == m_pMatchingBase || NULL == m_pScript )
		{
			CancelJoinMatching( dwChaDBNum );
			return false;
		}

		RoomManagerBase* pRoom = m_pMatchingBase->getRoomManager();
		if ( NULL == pRoom )
		{
			CancelJoinMatching( dwChaDBNum );
			return false;
		}

		// Join Count 체크;
		// Max Join Count 가 0 일 경우 체크하지 않는다;
		/*sc::writeLogInfo ( "[ Matching Log ] [ Check Join Count!! ]" );
		DWORD dwJoinCount = CMatchingJoinManager::Instance()->
			GetJoinCount_DBNum( m_pMatchingBase->GetScriptID(), dwChaDBNum );
		if ( pRoom->GetJoinMaxCount() > 0 && pRoom->GetJoinMaxCount() <= dwJoinCount )
		{
			sc::writeLogInfo ( "[ Matching Log ] [ Failed Join!! ]" );
			CancelJoinMatching( dwChaDBNum );
			return false;
		}*/

		// 인던에 들어가 있지 않은 상태라면 처리한다;
		if ( false == IsJoinGame( dwChaDBNum ) )
		{
			// 신청이 열려있지 않으면 신청할 수 없다;
			if ( pRoom->GetStatus() != EMMATCHING_CONTENTS_OPENED )
			{
				CancelJoinMatching( dwChaDBNum );
				return false;
			}

			GLCharAG* pChar = gpAgentServer->GetCharByDbNum( dwChaDBNum );
			if ( NULL == pChar )
			{
				CancelJoinMatching( dwChaDBNum );
				return false;
			}

			// 파티원 전체를 체크해야 한다;
			const GLPartyID& sPartyID = pChar->GetPartyID();
			if ( sPartyID.isValidParty() )
			{
				const GLPartyAgent* pParty = gpAgentServer->GetParty( sPartyID );
				if ( pParty )
				{
					if ( pParty->m_Member.size() == 0 )
						return false;

					GLPartyAgent::MEMBER_CITER biter = pParty->m_Member.begin();
					GLPartyAgent::MEMBER_CITER eiter = pParty->m_Member.end();
					for ( ; biter != eiter; biter++ ) 
					{
						GLCharAG* const pPartyMember = gpAgentServer->GetCharByGaeaID( *biter );
						if ( pPartyMember )
						{
							// 캐릭터가 인던 맵에 있는지 검사한다;
							SMAPNODE* pMapNode = gpAgentServer->FindMapNode( pPartyMember->GetCurrentMap() );
							if ( ( pMapNode && pMapNode->IsInstantMap() ) ||		// 신, 구인던;
								pPartyMember->IsCtfPlayer() ||						// 점령전
								MatchingSystem::MatchingCore::getinstance()->IsAreadyJoin( pPartyMember->ChaDbNum() ) )	// Matching List;
							{
								CancelJoinMatching( pPartyMember->ChaDbNum() );

								sc::writeLogInfo ( sc::string::format(
									"[ Matching Log ] [ Already Join Other Instance Map : %1% ]",
									pPartyMember->ChaDbNum() ) );

								GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
								sNetMsg.xmlString.emXmlType = InstanceSystem::InstanceXmlString::EMXMLGAME_IN_TEXT;
								sNetMsg.xmlString.strXmlKey = "PVE_ENTRANCE_ACK_CONFIRM_TEXT";
								sNetMsg.xmlString.index = 1;
								sNetMsg.xmlString.XmlParamVector.push_back( pPartyMember->GetChaName() );
								sNetMsg.dwColor = NS_UITEXTCOLOR::RED;

								msgpack::sbuffer sSendBuffer;
								msgpack::pack( sSendBuffer, sNetMsg );
								gpAgentServer->SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, sSendBuffer );

								return false;
							}

							// 파티원 중 한명이라도 Join Count 에 충족되지 않으면 입장시키지 않는다;
							DWORD dwJoinCount = CMatchingJoinManager::Instance()->
								GetJoinCount_DBNum( m_pMatchingBase->GetScriptID(), pPartyMember->ChaDbNum() );
							if ( pRoom->GetJoinMaxCount() > 0 && pRoom->GetJoinMaxCount() <= dwJoinCount )
							{
								CancelJoinMatching( pPartyMember->ChaDbNum() );

								sc::writeLogInfo ( sc::string::format(
									"[ Matching Log ] [ Join Count is Zero : %1% ]",
									pPartyMember->ChaDbNum() ) );

								GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
								sNetMsg.xmlString.emXmlType = InstanceSystem::InstanceXmlString::EMXMLGAME_IN_TEXT;
								sNetMsg.xmlString.strXmlKey = "COMPETITION_PVE_FAILED_ENTRANCE";
								sNetMsg.xmlString.index = 0;
								sNetMsg.dwColor = NS_UITEXTCOLOR::RED;

								msgpack::sbuffer sSendBuffer;
								msgpack::pack( sSendBuffer, sNetMsg );
								gpAgentServer->SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, sSendBuffer );

								return false;
							}
						}
					}
				}
			}
			// 파티가 아니라면 신청자만 검사한다;
			else
			{
				// 캐릭터가 인던 맵에 있는지 검사한다;
				SMAPNODE* pMapNode = gpAgentServer->FindMapNode( pChar->GetCurrentMap() );
				if ( ( pMapNode && pMapNode->IsInstantMap() ) ||	// 신, 구인던;
					pChar->IsCtfPlayer() ||							// 점령전;
					MatchingSystem::MatchingCore::getinstance()->IsAreadyJoin( pChar->ChaDbNum() ) )	// Matching List;
				{
					CancelJoinMatching( dwChaDBNum );

					sc::writeLogInfo ( sc::string::format(
						"[ Matching Log ] [ Already Join Other Instance Map : %1% ]",
						dwChaDBNum ) );

					GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
					sNetMsg.xmlString.emXmlType = InstanceSystem::InstanceXmlString::EMXMLGAME_IN_TEXT;
					sNetMsg.xmlString.strXmlKey = "PVE_ENTRANCE_ACK_CONFIRM_TEXT";
					sNetMsg.xmlString.index = 1;
					sNetMsg.dwColor = NS_UITEXTCOLOR::RED;
					sNetMsg.xmlString.XmlParamVector.push_back( pChar->GetChaName() );

					msgpack::sbuffer sSendBuffer;
					msgpack::pack( sSendBuffer, sNetMsg );
					gpAgentServer->SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, sSendBuffer );

					return false;
				}

				// 파티원 중 한명이라도 Join Count 에 충족되지 않으면 입장시키지 않는다;
				DWORD dwJoinCount = CMatchingJoinManager::Instance()->
					GetJoinCount_DBNum( m_pMatchingBase->GetScriptID(), dwChaDBNum );
				if ( pRoom->GetJoinMaxCount() > 0 && pRoom->GetJoinMaxCount() <= dwJoinCount )
				{
					CancelJoinMatching( dwChaDBNum );

					sc::writeLogInfo ( sc::string::format(
						"[ Matching Log ] [ Join Count is Zero : %1% ]",
						dwChaDBNum ) );

					GLMSG::NET_INSTANCE_UI_CHATXMLMSG sNetMsg;
					sNetMsg.xmlString.emXmlType = InstanceSystem::InstanceXmlString::EMXMLGAME_IN_TEXT;
					sNetMsg.xmlString.strXmlKey = "COMPETITION_PVE_FAILED_ENTRANCE";
					sNetMsg.xmlString.index = 0;
					sNetMsg.dwColor = NS_UITEXTCOLOR::RED;

					msgpack::sbuffer sSendBuffer;
					msgpack::pack( sSendBuffer, sNetMsg );
					gpAgentServer->SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, sSendBuffer );

					return false;
				}
			}

			
			// 신청자를 중심으로 체크해야할 사항을 체크한다 ( Script );
			if ( m_pScript->PushFunctionEx( "TournamentRequestJoin" ) == true )
			{
				m_pScript->PushInteger( dwChaDBNum );

				if ( m_pScript->CallFunction( 1, 1 ) == true )
				{
					bool bJoin = m_pScript->PopBoolean();
					if ( bJoin )
						sc::writeLogInfo( "[ Matching Log ] [ Succeed Request Join Matching!! ]" );

					MatchingCore::getinstance()->SendClientUserMatchingInfo(
						dwChaDBNum,
						m_pMatchingBase->GetScriptID() );

					return bJoin;
				}
			}
		}
		// 인던에 들어가 있는 상태라면 처리한다;
		// 재입장을 위해서 로직이 존재한다;
		else
		{
			// 인던에 들어가 있지 않다면 취소한다;
			const DWORD dwInstanceMapID = GetJoinGame( dwChaDBNum );
			if ( -1 == dwInstanceMapID )
			{
				CancelJoinMatching( dwChaDBNum );
				return false;
			}

			// 재입장 가능한 방인지 확인;
			RoomManagerBase* pRoom = m_pMatchingBase->getRoomManager();
			if ( pRoom && pRoom->GetMatchingInfo().m_ContentsType !=
				EMMATCHING_CONTENTS_NOMAL_REJOINALBE )
			{
				CancelJoinMatching( dwChaDBNum );
				return false;
			}
			
			// 재입장 가능한 매칭인지 확인한다;
			if ( false == CanRejoin( dwChaDBNum ) )
			{
				CancelJoinMatching( dwChaDBNum );
				return false;
			}
			
			GLCharAG* pChar = gpAgentServer->GetCharByDbNum( dwChaDBNum );
			if ( NULL == pChar )
			{
				CancelJoinMatching( dwChaDBNum );
				return false;
			}

			// 캐릭터가 인던에 들어가 있는 상태가 아닌지 검사한다;
			if ( pChar->GetCurrentMap().getGaeaMapID() == dwInstanceMapID )
			{
				CancelJoinMatching( dwChaDBNum );
				return false;
			}
			
			// 인던에 입장 예약한다;
			InstanceSystem::InformationPosition sInformationPosition(
				Faction::GLActorID( CROW_PC, dwChaDBNum ),
				SNATIVEID( dwInstanceMapID ),
				Faction::GLActorID(),
				0, 0, InstanceSystem::EMMOVE_FIELD2INSTANCE );

			InstanceSystem::ManagerAgent* pManager =
				MatchingCore::getInstanceManager();
			if ( pManager )
			{
				pManager->reserveMember( dwChaDBNum, sInformationPosition );

				return true;
			}
		}

		/// 이곳에는 로직상 들어올 수 없다;
		/// 만약, 들어온다면 신청을 취소시킨다;
		CancelJoinMatching( dwChaDBNum );

		return false;
	}


	bool LobbyManagerbase::OnComebackMatchingGroup ( DWORD dwMatchingGroupID )
	{
		// 스크립트에서 확인 후 재도전이 true로 설정되어 있을 때만 처리한다;
		if ( m_pScript->PushFunctionEx( "TournamentRequestREJoinGroup" ) == true )
		{
			m_pScript->PushInteger( dwMatchingGroupID );

			if ( true == m_pScript->CallFunction( 1, 1 ) )
			{
				bool bJoin = m_pScript->PopBoolean();
				if ( true == bJoin )
				{
					sc::writeLogInfo( "[ Matching Log ] [ Succeed Add RejoinGroup. ]" );
				}
				else
				{
					sc::writeLogInfo( "[ Matching Log ] [ Failed Add RejoinGroup. ]" );
				}

				return bJoin;
			}
		}

		return false;
	}


	void LobbyManagerbase::OnDropOutChar ( DWORD dwChaDBNum )
	{
		if ( m_pScript->PushFunctionEx( "OnDropOutChar" ) == true )
		{
			m_pScript->PushInteger( dwChaDBNum );
			m_pScript->CallFunction( 1, 0 );
		}
		else
		{
			GLCharAG* const pChar = gpAgentServer->GetCharByDbNum( dwChaDBNum );
			if ( NULL == pChar )
				return;
			else
			{
				DWORD dwMatchingGroupID = SearchMatchingGroup( dwChaDBNum );
				DeleteMatchingGroup( dwMatchingGroupID );

				return;
			}

			GLPartyID dwPartyID = pChar->GetPartyID();
			const GLPartyField* pParty = gpGaeaServer->GetParty( dwPartyID );
			if ( pParty )
			{
				GLPartyField::MEMBER_CITER bIter = pParty->m_cMEMBER.begin();
				GLPartyField::MEMBER_CITER eIter = pParty->m_cMEMBER.end();
				for ( ; bIter != eIter; bIter++ ) 
				{
					DWORD dwMatchingGroupID = SearchMatchingGroup( bIter->first );
					DeleteMatchingGroup( dwMatchingGroupID );
				}
			}
			else
			{
				DWORD dwMatchingGroupID = SearchMatchingGroup( dwChaDBNum );
				DeleteMatchingGroup( dwMatchingGroupID );
			}			
		}
	}


	void LobbyManagerbase::OnChangeParty ( DWORD dwChaDBNum )
	{
		// 파티에 관련된 내용이 변경되었는데 아무것도 설정하고 있지 않은경우;
		if ( m_pScript->PushFunctionEx( "PlayerPartyChanged" ) == true )
		{
			m_pScript->PushInteger( dwChaDBNum );
			m_pScript->CallFunction( 1, 0 );

			return;
		}

		// 정말 아무런 처리도 없다면 나가게 한다;
		this->OnDropOutChar( dwChaDBNum );
	}


	void LobbyManagerbase::OnChangePartyMember ( DWORD dwChaDBNum, DWORD dwParyID )
	{
		if ( m_pScript->PushFunctionEx( "PartyMemberChaged" ) == true )
		{
			m_pScript->PushInteger( dwChaDBNum );
			m_pScript->PushInteger( dwParyID );
			m_pScript->CallFunction( 2, 0 );

			return;
		}
		else
		{
			OnChangeParty( dwChaDBNum );
		}
	}


	void LobbyManagerbase::OnDropOutPartyMember ( DWORD dwChaDBNum, DWORD dwParyID )
	{
		if ( m_pScript->PushFunctionEx( "PartyMemberRemoved" ) == true )
		{
			m_pScript->PushInteger( dwChaDBNum );
			m_pScript->PushInteger( dwParyID );
			m_pScript->CallFunction( 2, 0 );

			return;
		}
		else
		{
			OnChangeParty( dwChaDBNum );
		}
	}


	void LobbyManagerbase::OnChangePartyMaster (
		DWORD dwOldChaDBNum,
		DWORD dwNewChaDBNum,
		DWORD dwParyID )
	{
		if ( m_pScript->PushFunctionEx( "PartyMasterChaged" ) == true )
		{
			m_pScript->PushInteger( dwOldChaDBNum );
			m_pScript->PushInteger( dwNewChaDBNum );
			m_pScript->PushInteger( dwParyID );
			m_pScript->CallFunction( 3, 0 );

			return;
		}
		else
		{
			OnChangeParty( dwOldChaDBNum );
			OnChangeParty( dwNewChaDBNum );
		}
	}


	void LobbyManagerbase::OnDestoryParty ( DWORD dwParyID )
	{
		if ( m_pScript->PushFunctionEx( "PartyDestroyed" ) == true )
		{
			m_pScript->PushInteger( dwParyID );
			m_pScript->CallFunction( 1, 0 );

			return;
		}
	}


	/************************************************************************/
	/* Invoke Function;                                                     */
	/************************************************************************/

	bool LobbyManagerbase::IsJoinGame ( DWORD dwChaDBNum )
	{
		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return false;

		// Matching 에 들어가있는가?;
		if ( pGroupManager->IsRegistCharacter( dwChaDBNum ) )
			return true;

		// 게임에 참가하고 있는가?;
		if ( -1 == GetJoinGame( dwChaDBNum ) )
			return false;

		return true;
	}


	DWORD LobbyManagerbase::GetJoinGame ( const DWORD dwChaDBNum )
	{
		if ( NULL == m_pMatchingBase )
			return -1;

		RoomManagerBase* pRoom = m_pMatchingBase->m_Roominstance;
		if ( NULL == pRoom)
			return -1;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return -1;

		std::vector< JoinData* >::iterator beginJoinIter =
			pRoom->m_vecInstanceMapids.begin();
		for ( ; beginJoinIter != pRoom->m_vecInstanceMapids.end(); ++beginJoinIter )
		{
			JoinData* pJoinData = *beginJoinIter;
			if ( NULL == pJoinData )
				continue;

			std::vector<DWORD>::iterator beginClientIter =
				pJoinData->m_VecClientID.begin();
			for ( ; beginClientIter != pJoinData->m_VecClientID.end(); ++beginClientIter )
			{
				if ( *beginClientIter == dwChaDBNum )
				{
					return pJoinData->InstanceMapID;
				}
			}
		}

		return -1;
	}


	const bool LobbyManagerbase::CanRejoin ( DWORD _dwChaDBNum )
	{
		// 어떤 인던에 들어가 있는지 확인한다;
		const DWORD dwInstanceMapID = GetJoinGame( _dwChaDBNum );
		if ( -1 == dwInstanceMapID )
		{
			return false;
		}

		// Script 에서 재입장 설정되었는지 확인한다;
		if ( m_pScript->PushFunctionEx( "TournamentRequestReJoin" ) == true )
		{
			m_pScript->PushInteger( _dwChaDBNum );
			m_pScript->PushInteger( dwInstanceMapID );

			if ( m_pScript->CallFunction( 2, 1 ) == true )
			{
				return m_pScript->PopBoolean();
			}
		}

		return false;
	}


	void LobbyManagerbase::OutMatchingSystem ( DWORD dwChaDBNum )
	{
		// 매칭시스템에서 퇴장시킨다;
		MatchingCore::getinstance()->OutMatchingSystem(
			m_pMatchingBase->GetScriptID(),
			dwChaDBNum );
	}


	bool LobbyManagerbase::StartMatching ( )
	{
		if ( NULL == m_pMatchingBase )
			return false;

		MatchingInstanceBase* pMatchingInstance =
			m_pMatchingBase->getMatchingManager();

		// 매칭이 꽉 찼거나, 만들 수가 없다;
		if ( NULL == pMatchingInstance )
		{
			sc::writeLogError( "[ Matching Log ] [ Matching is Failed!!! ]" );

			return false;
		}

		sc::writeLogInfo( "[ Matching Log ] [ Start Matching!! ( Lobby ) ]" );

		// 매칭 시작;
		pMatchingInstance->StartMatching();

		return true;
	}

	
	bool LobbyManagerbase::JoinCharacter_TempLobby ( DWORD dwChaDBNum )
	{
		m_tempVecCharacterDBNum.push_back( dwChaDBNum );

		return true;
	}


	void LobbyManagerbase::MakeMatchingGroup ( )
	{
		if ( NULL == m_pMatchingBase )
			return;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		// Matching Base ID;
		DWORD dwMatchingBaseID = m_pMatchingBase->GetScriptID();

		// Matching을 위한 GroupID를 생성한다;
		DWORD dwMatchingGroupID = pGroupManager->CreateNewMatchingGroup( dwMatchingBaseID );
		if ( INVALID_MATCHING_GROUP_ID == dwMatchingGroupID )
		{
			sc::writeLogError ( "[ Matching Log ] [ Failed New Create Matching Group ID. ]" );
			sc::writeLogError ( "[ Matching Log ] -> [ Execute GM Command '/mt re'. ]" );

			return;
		}

		// 대기 그룹에 넣는다;
		pGroupManager->PushWaitingQueue( dwMatchingGroupID );

		// 그룹에 대기중인 캐릭터를 입장시킨다;
		BOOST_FOREACH ( DWORD dwChaDBNum, m_tempVecCharacterDBNum )
		{
			if ( pGroupManager->JoinCharacter( dwMatchingGroupID, dwChaDBNum ) )
			{
				// 캐릭터를 매칭시스템에도 입장시킨다;
				MatchingCore::getinstance()
					->JoinMatchingSystem( dwMatchingBaseID, dwChaDBNum );
			}

		}

		m_tempVecCharacterDBNum.clear();

		sc::writeLogInfo( "[ Matching Log ] [ Succeed MakeMatchingGroup. ]");
	}


	void LobbyManagerbase::CancelJoinMatching ( const DWORD dwChaDBNum ) 
	{
		if ( m_pScript->PushFunctionEx( "MatchingJoinCansel" ) == true )
		{
			m_pScript->PushInteger( dwChaDBNum );
			m_pScript->CallFunction( 1, 0 );

			return;
		}

		this->OnDropOutChar( dwChaDBNum );
	}


	bool LobbyManagerbase::IsValidMatchingGroup ( DWORD dwMatchingGroupID )
	{
		if ( NULL == m_pMatchingBase )
			return false;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return false;

		return pGroupManager->IsValidMatchingGroup( dwMatchingGroupID );
	}


	DWORD LobbyManagerbase::SearchMatchingGroup ( DWORD dwChaDBNum )
	{
		if ( NULL == m_pMatchingBase )
			return 0;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return 0;

		return pGroupManager->SearchMatchingGroupID_ChaDBNum( dwChaDBNum );
	}


	void LobbyManagerbase::DeleteMatchingGroup ( DWORD dwMatchingGroupID )
	{
		if ( NULL == m_pMatchingBase )
			return;

		// 그룹 ID 유효성검사;
		if ( false == IsValidMatchingGroup( dwMatchingGroupID ) )
			return;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
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
	
///////////////////////////////////////////////////////////////////////////////////////////////////////////
	DWORD LobbyManagerbase::GetCountTournamentPeople()
	{
		MatchingSystem::GLMatchingGroupManager* groupmanager = m_pMatchingBase->getGroupManager();
		return groupmanager->GetCharCountOfMatching(m_pMatchingBase->GetScriptID());
	}

	int LobbyManagerbase::GetSchoolParticipation(DWORD school)
	{
		int returnvalue = 0;
		std::vector<DWORD> clientlist;
		MatchingSystem::GLMatchingGroupManager* groupmanager = m_pMatchingBase->getGroupManager();
		groupmanager->SearchMatchingGroup_MatchingID(clientlist,m_pMatchingBase->GetScriptID());

		BOOST_FOREACH(DWORD ClientDBnum,clientlist)
		{
            const GLCharAG* pChar = gpAgentServer->GetCharByDbNum(ClientDBnum);
            if ( pChar )
            {
                if(school == pChar->GetSchool() )
                {
                    returnvalue ++;
                }
            }
		}
		clientlist.clear();
		return returnvalue;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	void LobbyManagerbase::ClientMoneySpendCallBack ( DWORD dwClientID, BOOL bOk )
	{
		if ( m_pScript->PushFunctionEx( "SpendMoneyResualt" ) == true )
		{
			m_pScript->PushInteger( dwClientID );
			m_pScript->CallFunction( 1, 0 );
/*
			bool isJoin = m_script->popBoolean();
			if(isJoin == true)
				sc::writeLogInfo("SpendMoneyResualt : SpendMoneyResualt Success");
*/
			return;
		}
	}


	void LobbyManagerbase::ClientMoneyCheckCallBack ( DWORD dwClientID, BOOL bOk )
	{
		if ( m_pScript->PushFunctionEx( "CheckMoneyResualt" ) == true )
		{
			m_pScript->PushInteger( dwClientID );
			m_pScript->CallFunction( 1, 0 );
/*
			bool isJoin = m_script->popBoolean();
			if(isJoin == true)
				sc::writeLogInfo("CheckMoneyResualt : CheckMoneyResualt Success");
*/
			return;
		}
	}


	bool LobbyManagerbase::CheckMoney ( DWORD dwClientID, DWORD dwMoney )
	{
		GLMSG::NET_MATCHING_MONEY_FEE_OK_AF _MessageBox( dwClientID, dwMoney );
		MatchingCore::getinstance()->_sendMessageToClient( dwClientID, &_MessageBox );

		return true;
	}


	bool LobbyManagerbase::SpendMoney ( DWORD dwClientID, DWORD dwMoney )
	{
		GLMSG::NET_MATCHING_MONEY_FEE_SPEND_AF _MessageBox( dwClientID, dwMoney );
		MatchingCore::getinstance()->_sendMessageToClient( dwClientID, &_MessageBox );

		return true;
	}
	
}
