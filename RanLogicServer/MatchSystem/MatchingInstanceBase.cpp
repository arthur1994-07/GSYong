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

	MatchingInstanceBase::MatchingInstanceBase ( MatchingGameBase* pMatchingBase
		, sc::LuaInstance* pScript
		, DWORD dwScriptID
		, DWORD dwMatchingInstanceID )
		: m_pScript ( pScript )								// 스크립트;
		, m_dwMatchingBaseID ( dwScriptID )						// 스크립트 ID;
		, m_dwMatchingInstanceID ( dwMatchingInstanceID )	// Matching Instance ID;
		, m_pMatchingBase ( pMatchingBase )					// Matching Game Base;
		, m_nJoinCount ( 0 )								// Matching Join Count;
		, m_bReceiveDataError ( false )
		, m_bDestroyMatchingInstance ( false )
		, m_bJoinCancelDropCondition ( true )
		, m_pCoroutineState ( NULL )
		, m_bSetWait ( false )
		, m_fWaitDuration ( 0 )
		, m_dwMatchingRound ( 0 )
		, m_dwMatchingSize ( 0 )
		, m_bUseCoroutine ( false )
		, m_dwMatchingModeID ( 0 )
		, m_pCurMatchingMode ( NULL )
		, m_bWaitingAnswer ( false )
	{
	}
	

	MatchingInstanceBase::~MatchingInstanceBase()
	{
		// Callback Trigger 제거;
		MatchingSystem::MatchingCallbackTriggerManager* pMCTM =
			MatchingSystem::MatchingCallbackTriggerManager::GetInstance();
		if ( pMCTM )
			pMCTM->RemoveCallback( m_dwMatchingInstanceID );

		// 스크립트 제거;
		if ( m_pScript )
		{
			m_pScript->OnDestroy();
			SAFE_DELETE( m_pScript );
		}

		// Join Table 제거;
		/*if ( m_JoinTable.size() != 0 )
		{
			JOINTABLE_VEC_ITOR biter = m_JoinTable.begin();
			for ( ; biter != m_JoinTable.end(); )
			{
				JoinTable* nNode = *biter;
				SAFE_DELETE( nNode );
				biter = m_JoinTable.erase( biter );
			}
		}*/	

		// Join 실패 Trigger 제거;
		if ( m_JoinFailTrigger.size() != 0 )
		{
			JOINTRIGGER_VEC_ITOR biter = m_JoinFailTrigger.begin();
			for ( ; biter != m_JoinFailTrigger.end(); )
			{
				JoinTrigger* nNode = *biter;
				SAFE_DELETE( nNode );
				biter = m_JoinFailTrigger.erase( biter );
			}
		}		

		// Mode 제거;
		SAFE_DELETE( m_pCurMatchingMode );
	}


	bool MatchingInstanceBase::_SendToClientAskJoin (
		DWORD dwChaDBNum,
		float fTime ,
		bool isUseCounting )
	{
		GLMSG::NET_MATCHING_NOTIFY_RUOK_AC _MessageBox(
			dwChaDBNum,
			m_dwMatchingInstanceID,
			fTime );

		MatchingCore::getinstance()->_sendMessageToClient(
			dwChaDBNum,
			&_MessageBox );

		// Ready Trigger 등록;
		// 준비완료 될 경우 OnReadyAnswer 콜백을 실행한다;
		ReadyTrigger* pTrigger = new ReadyTrigger( m_dwMatchingInstanceID );
		if ( pTrigger )
		{
			pTrigger->_MatchingInstanceid = m_dwMatchingInstanceID;
			pTrigger->RegistCallback( *this, &MatchingInstanceBase::OnReadyAnswer );

			MatchingSystem::MatchingCallbackTriggerManager* pMCTM =
				MatchingSystem::MatchingCallbackTriggerManager::GetInstance();
			if ( pMCTM )
				pMCTM->m_ReadyTriggerVector.push_back( pTrigger );
		}

		return true;
	}	


	void MatchingInstanceBase::_SetAnswerEnded()
	{
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
				// 이미 취소되었음을 알린다;
				GLMSG::NET_MATCHING_NOTIFY_RUOK_AC sMessageBox(
					dwChaDBNum,
					m_dwMatchingInstanceID,
					0.f );
				sMessageBox.bComplete = true;

				MatchingCore::getinstance()->_sendMessageToClient(
					dwChaDBNum,
					&sMessageBox );

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

		// 모두 대기열로 보낸다;
		//SetGroupToLobbyALL();

		// Join 확인 답변이 모두 왔을 경우나 시간이 오버되었을 경우;
		/*if ( m_JoinTable.size() == 0 )
			return;

		if ( m_pCoroutineState == NULL )
			return;

		//lua_State* pState = _RoomReceipt->pState;
		// 새 테이블 생성
		lua_newtable(m_pCoroutineState);
		int top = lua_gettop(m_pCoroutineState);

		int i = 0;
		JOINTABLE_VEC_ITOR __biter = m_JoinTable.begin();
		for(;__biter != m_JoinTable.end();)
		{
			JoinTable* nNode = *__biter;

			if(nNode == NULL)
			{
				__biter = m_JoinTable.erase(__biter);
				continue;
			}

			int isAllAnser = ANSWER_OK;

			//그리고 후처리! 해줘야지
			if(nNode->isAllAnswer() ==false)
			{
				isAllAnser = ANSWER_NO_RESPONS;
				//나머지 벙어리들 강퇴시킨다.
				nNode->SetAllAnser(false);
			}
			
			if(nNode->isAllOkAnswer() == false)
			{
				// 그룹 파괴한다!!!!!
				if(m_bJoinCancelDropCondition == true)
				{
					//this->m_parent->getLobbyManager()->DeleteGroup(nNode->GetGroupID());
					if(isAllAnser != ANSWER_NO_RESPONS)
						isAllAnser = ANSWER_CANSEL;
				}
				else
				{
					//그냥 그룹 유지하길 원한다면 이렇게 함.
					while(nNode->isAllOkAnswer() == false)
					{
						DWORD ClientID = nNode->PopNotAnserClient();
						// 그룹에서 팅군다.
						m_pMatchingBase->getGroupManager()->DropOutCharacter_MatchingGroupID(nNode->GetGroupID(),ClientID);
					}
					if(m_pMatchingBase->getGroupManager()->GetCharCountOfMatchingGroup(nNode->GetGroupID()) == 0)
					{
						// 다나가면 머 그룹 파괴된거자나!
						this->m_pMatchingBase->getLobbyManager()->DeleteMatchingGroup(nNode->GetGroupID());
					}
				}
			}

			lua_pushinteger(m_pCoroutineState, nNode->GetGroupID());
			lua_pushinteger(m_pCoroutineState, isAllAnser);

			
			//맵 ID 집어놓고
			//lua_pushinteger(m_CoroutineState, i);
			// 만들어서 던져준다.
			lua_settable(m_pCoroutineState, top);
			i++;
			

			SAFE_DELETE(nNode);
			__biter = m_JoinTable.erase(__biter);
		}


		m_vecJoinSaved.clear();
		
		int status = m_pScript->coroutine_resume(m_pCoroutineState,1);
		if( status ==  LUA_YIELD)
			m_bUseCoroutine = true;
		else
			m_bUseCoroutine = false;*/
	}


	int MatchingInstanceBase::_GroupClientStatusUpdate (
		DWORD GroupID,
		EMCUSTOMER_STATUS_TYPE status )
	{
		if ( NULL == m_pMatchingBase )
			return 0;

		std::vector< DWORD > clientlist;

		MatchingSystem::GLMatchingGroupManager* pGroupmanager =
			m_pMatchingBase->getGroupManager();
		if ( NULL == pGroupmanager )
			return 0;

		pGroupmanager->SearchCharacters_MatchingGroupID( clientlist, GroupID );

		MatchingCore* pMatchingCore = MatchingCore::getinstance();
		if ( NULL == pMatchingCore )
			return 0;

		BOOST_FOREACH ( DWORD ClientDBnum, clientlist )
		{
			pMatchingCore->UserMatchingStatusUpdate(
				ClientDBnum,
				m_pMatchingBase->GetScriptID(),
				status );
		}

		return 0;
	}


	void MatchingInstanceBase::_SetMatchingPlayerTable ( KeyPair _Groupid, DWORD num )
	{
		GLMatChingGroupInfo NewGorupInfo;
		NewGorupInfo.dwGROUPID = INVALID_MATCHING_GROUP_ID;
		NewGorupInfo.GroupName = "*****";
		NewGorupInfo.Index = _Groupid.Index;
		NewGorupInfo.Position = num;
		NewGorupInfo.ScriptID = m_pMatchingBase->GetScriptID();
		NewGorupInfo.MatchingInstanceID = m_dwMatchingBaseID;
		NewGorupInfo.GroupSize = 0;
		NewGorupInfo.GroupMaxSize = 0;
		NewGorupInfo.CharLevel = 0;

		if ( INVALID_MATCHING_GROUP_ID == _Groupid.GroupID )
		{
			m_vecMatchingGroupInfo.push_back( NewGorupInfo );

			return;
		}

		MatchingSystem::GLMatchingGroup* pNode =
			m_pMatchingBase->getGroupManager()->GetMatchingGroup(_Groupid.GroupID);
		if ( pNode == NULL )
		{
			m_vecMatchingGroupInfo.push_back( NewGorupInfo );

			return;
		}

		const GLCharAG* pChar(gpAgentServer->GetCharByDbNum(pNode->GetMatchingGroupChairman()));

		if( pChar == NULL )
		{
			m_vecMatchingGroupInfo.push_back(NewGorupInfo);
			return;
		}

		WORD CharLevel = pChar->GetChaLevel();


		if( pNode->GetCountCharacter() <= 1 )
		{
			std::string _name(pChar->GetChaName());
			NewGorupInfo.GroupName = _name;
			NewGorupInfo.dwGROUPID = _Groupid.GroupID;
			NewGorupInfo.GroupSize = 1;
			NewGorupInfo.GroupMaxSize = 1;
			NewGorupInfo.CharLevel = CharLevel;
			NewGorupInfo.GroupClass = pChar->GetClass();

			m_vecMatchingGroupInfo.push_back(NewGorupInfo);
			return;

		}
		else
		{

			NewGorupInfo.dwGROUPID = _Groupid.GroupID;
			NewGorupInfo.GroupSize = 1;
			NewGorupInfo.GroupMaxSize = 1;
			NewGorupInfo.GroupClass = pChar->GetClass();

			std::string _name(pChar->GetChaName());
			NewGorupInfo.GroupName = _name;

			GLPartyID _id =pChar->GetPartyID();
			const GLPartyAgent* agent = gpAgentServer->GetParty(_id);
			std::string _clubname = "";

			DWORD MaxCount = 8;


			std::tr1::shared_ptr<GLClubAgent> pMyClub = gpAgentServer->GetClub(pChar->GetClubDbNum());

			if(pMyClub != NULL)
			{
				NewGorupInfo.ClubName = pMyClub->Name();
			}

			if (agent != NULL)
			{
				if(_id.isValidExpedition() == true)
				{
					NewGorupInfo.GroupMaxSize = 40;
				}

				const GLCharAG* pChar(gpAgentServer->GetCharByGaeaID(agent->getMaster()));
				if(pChar != NULL)
				{
					NewGorupInfo.GroupName = pChar->GetChaName();
					NewGorupInfo.CharLevel = pChar->GetChaLevel();
				}
				else
				{
					NewGorupInfo.GroupName = "*****";
					NewGorupInfo.CharLevel = 0;
				}

				NewGorupInfo.GroupSize = pNode->GetCountCharacter();
				if( NewGorupInfo.GroupSize < 8 || NewGorupInfo.GroupMaxSize >= 8)
					NewGorupInfo.GroupMaxSize = 40;
				else
					NewGorupInfo.GroupMaxSize = 8;
			}
			m_vecMatchingGroupInfo.push_back(NewGorupInfo);
			return;
		}
	}

}