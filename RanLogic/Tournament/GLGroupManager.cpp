#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"

#include "./GLGroupManager.h"


using namespace sc;

namespace MatchingSystem
{

	const DWORD GLMatchingGroupManager::MATCHING_GROUP_SIZE = 1000;

	GLMatchingGroupManager::GLMatchingGroupManager ( )
	{
		// 그룹 아이디를 미리 지정한다;
		for ( DWORD i = 1; i < MATCHING_GROUP_SIZE ; ++i )
		{
			m_queueUnassignedMatchingGroupID.push_back(
				static_cast< MatchingGroupID >( i ) );
		}
	}


	GLMatchingGroupManager::~GLMatchingGroupManager ( )
	{
		// Matching Group 정보 초기화;
		MAP_MATCHING_GROUP_ITER biter = m_mapMatchingGroup.begin();
		for ( ; biter != m_mapMatchingGroup.end(); )
		{
			GLMatchingGroup* pGroup = biter->second;
			biter = m_mapMatchingGroup.erase( biter );
			SAFE_DELETE( pGroup );
		}
	}


	/************************************************************************/
	/* Script;                                                              */
	/************************************************************************/

	void GLMatchingGroupManager::RegistCallBackFunction_Lua ( sc::LuaInstance* pScript )
	{
		pScript->registdirectFunction( "GetGroupSize", *this, &GLMatchingGroupManager::GetCharCountOfMatchingGroup );
		pScript->registdirectFunction( "IsValidGroup", *this, &GLMatchingGroupManager::IsValidMatchingGroup );
		pScript->registdirectFunction( "GroupPushClient", *this, &GLMatchingGroupManager::JoinCharacter );
		pScript->registdirectFunction( "GroupDeleteClient", *this, &GLMatchingGroupManager::DropOutCharacter );
		pScript->registdirectFunction( "CreateNewIDGroup", *this, &GLMatchingGroupManager::CreateNewMatchingGroup );
		
		pScript->registFunctionex( "GetGroup", *this, &GLMatchingGroupManager::SearchCharacters_MatchingGroupID_Lua );
	}


	int GLMatchingGroupManager::SearchCharacters_MatchingGroupID_Lua ( lua_State* pState )
	{
		// 매개변수 MatchingGroupID 얻어온다;
		const unsigned int nGroupID = lua_tointeger( pState, -1 );
		lua_pop( pState, 1 );
		MatchingGroupID dwMatchingGroupID = static_cast< MatchingGroupID >( nGroupID );

		// ChaDBNum 을 넣을 테이블을 생성한다;
		lua_newtable( pState );

		// Table 을 세팅하기 위해 스택위치를 얻어온다;
		int top = lua_gettop( pState );

		// ChaDBNum 을 검색한다;
		VEC_CHARACTER_DBNUM vecCharacterDBNum;
		SearchCharacters_MatchingGroupID( vecCharacterDBNum, dwMatchingGroupID );

		// Table 에 세팅한다;
		for ( unsigned int i=0; i<vecCharacterDBNum.size(); ++i )
		{
			DWORD dwChaDBNum = vecCharacterDBNum.at( i );

			lua_pushinteger( pState, dwChaDBNum );
			lua_pushinteger( pState, i );

			lua_settable( pState, top );
		}

		return 1;
	}


	/************************************************************************/
	/* Invoke Function                                                      */
	/************************************************************************/

	MatchingGroupID GLMatchingGroupManager::CreateNewMatchingGroup ( DWORD dwMatchingID )
	{
		// 사용하지 않은 ID가 없으면 실패한다;
		if ( 0 == m_queueUnassignedMatchingGroupID.size() )
		{
			// 다음 로그가 남을 경우 GroupManager가 매칭그룹을;
			// 제대로 통제하고 있지 못하다는 의미이다;
			// 개선이 필요하다;
			sc::writeLogError ( sc::string::format(
				"[ Matching Log ] [ Overflow MatchingGroup!!! The Matching Group is not bigger than %1%. ]",
				MATCHING_GROUP_SIZE ) );
			sc::writeLogError ( 
				"[ Matching Log ] -> [ This error is important. ]" );

			return INVALID_MATCHING_GROUP_ID;
		}

		// 새로운 ID를 얻어온다;
		DWORD dwMatchingGroupID = m_queueUnassignedMatchingGroupID.front();

		// 그룹을 생성한다;
		GLMatchingGroup* pGroup = new GLMatchingGroup( dwMatchingGroupID );
		if ( NULL == pGroup )
		{
			sc::writeLogError ( 
				"[ Matching Log ] [ Failed Create MatchingGroup. ]" );

			return INVALID_MATCHING_GROUP_ID;
		}

		// 그룹의 Matching ID를 등록한다;
		pGroup->SetMatchingID( dwMatchingID );

		// ID를 사용중으로 변경하고 새로운 Matching Group을 등록한다;
		m_queueUnassignedMatchingGroupID.pop_front();
		m_mapMatchingGroup.insert(
			MAP_MATCHING_GROUP_VALUE(
			pGroup->GetMatchingGroupID(),
			pGroup ) );

		return dwMatchingGroupID;
	}


	void GLMatchingGroupManager::DeleteMatchingGroup ( MatchingGroupID dwMatchingGroupID )
	{
		if ( 0 == m_mapMatchingGroup.size() )
			return ;

		// Matching Group 컨테이너에서 제거한다;
		// 제거하기 전에 Matching Group 의 Matching ID 를 획득한다;
		MatchingID dwMatchingID = INVALID_MATCHING_ID;
		MAP_MATCHING_GROUP_ITER iter = m_mapMatchingGroup.begin();
		MAP_MATCHING_GROUP_ITER eiter = m_mapMatchingGroup.end();
		for( ; iter != eiter; )
		{
			GLMatchingGroup* pGroup = iter->second;
			if ( pGroup && pGroup->GetMatchingGroupID() == dwMatchingGroupID )
			{
				dwMatchingID = pGroup->GetMatchingID();

				pGroup->OnDestroy();
				m_mapMatchingGroup.erase( iter++ );
				SAFE_DELETE( pGroup );
			}
			else
			{
				iter++;
			}
		}

		/// Matching 컨테이너 또는 대기열에서 제거한다;
		// 대기열에 있을 경우 대기열에서 제거;
		if ( dwMatchingID == INVALID_MATCHING_ID )
		{
			QUEUE_MATCHING_GROUP_ID_ITER biter = m_queueWaitingMatchingGroupID.begin();
			for ( ; biter != m_queueWaitingMatchingGroupID.end(); )
			{
				MatchingGroupID dwTempMatchingGroupID = *biter;
				if ( dwTempMatchingGroupID == dwMatchingGroupID )
				{
					biter = m_queueWaitingMatchingGroupID.erase( biter );
				}
				else
				{
					++biter;
				}
			}
		}
		// Matching 컨테이너에 있을 경우 Matching 컨테이너에서 제거;
		else
		{
			MAP_MATCHING_ITER iter = m_mapMatching.find( dwMatchingID );
			if ( iter != m_mapMatching.end() )
			{
				VEC_MATCHING_GROUP_ID& vecMatchingGroupID = iter->second;
				VEC_MATCHING_GROUP_ID_ITER GroupIDIter = vecMatchingGroupID.begin();
				for ( ; GroupIDIter != vecMatchingGroupID.end(); )
				{
					MatchingGroupID dwTempMatchingGroupID = *GroupIDIter;
					if ( dwTempMatchingGroupID == dwMatchingGroupID )
					{
						GroupIDIter = vecMatchingGroupID.erase( GroupIDIter );
					}
					else
					{
						++GroupIDIter;
					}
				}
			}
		}

		// 할당되지 않은 Matching Group ID 에 넣는다;
		m_queueUnassignedMatchingGroupID.push_back( dwMatchingGroupID );
	}


	void GLMatchingGroupManager::DeleteMatchingGroupAll ( )
	{
		MAP_MATCHING_GROUP_ITER iter = m_mapMatchingGroup.begin();
		MAP_MATCHING_GROUP_ITER eiter = m_mapMatchingGroup.end();
		for ( ; iter != eiter; )
		{
			GLMatchingGroup* pGroup = iter->second;
			if ( NULL == pGroup )
			{
				iter++;

				sc::writeLogError( sc::string::format(
					"[ Matching Log ] [ This Matching Group ID %1% is invalid. ]",
					iter->first ) );

				continue;
			}

			MatchingGroupID dwMatchingGroupID = pGroup->GetMatchingGroupID();

			pGroup->OnDestroy();
			m_mapMatchingGroup.erase( iter++ );
			SAFE_DELETE( pGroup );

			m_queueUnassignedMatchingGroupID.push_back( dwMatchingGroupID );
		}

		m_queueWaitingMatchingGroupID.clear();
		m_mapMatchingGroup.clear();
	}


	void GLMatchingGroupManager::DeleteMatchingGroupAll_Matching ( MatchingID dwMatchingID )
	{
		MatchingGroupID dwMatchingGroupID = INVALID_MATCHING_GROUP_ID;

		MAP_MATCHING_ITER iter = m_mapMatching.find( dwMatchingID );
		if ( iter != m_mapMatching.end() )
		{
			// 참조만 할 것이기 때문에 레퍼런스로 사용하면 안된다;
			const VEC_MATCHING_GROUP_ID vecMatchingGroupID = iter->second;

			// 현재 Matching 이 비어있다면;
			if ( vecMatchingGroupID.empty() )
				return;

			VEC_MATCHING_GROUP_ID_CITER beginGroupIDIter = vecMatchingGroupID.begin();
			VEC_MATCHING_GROUP_ID_CITER endGroupIDIter = vecMatchingGroupID.end();
			for ( ; beginGroupIDIter != endGroupIDIter; ++beginGroupIDIter )
			{
				MatchingGroupID dwMatchingGroupID = *beginGroupIDIter;

				DeleteMatchingGroup( dwMatchingGroupID );
			}
		}
	}


	bool GLMatchingGroupManager::SetMatching ( MatchingGroupID dwMatchingGroupID, DWORD dwMatchingID )
	{
		GLMatchingGroup* pGroup = GetMatchingGroup( dwMatchingGroupID );
		if ( NULL == pGroup )
		{
			sc::writeLogError( sc::string::format(
				"[ Matching Log ] [ Invalid Matching Group ID ( %1% ). ]",
				dwMatchingGroupID ) );

			return false;
		}

		/// 기존에 Matching 에 속해 있던 Matching Group 은 안된다;
		/// 먼저 대기열로 보내야 한다;
		if ( INVALID_MATCHING_ID != pGroup->GetMatchingID() )
		{
			sc::writeLogError( sc::string::format(
				"[ Matching Log ] [ First, The Matching Group ( %1% ) have to go to Waiting Queue. ]",
				dwMatchingGroupID ) );

			return false;
		}

		/// Matching 에 Matching Group 을 넣는다;
		// 같은 MatchingGroupID 가 들어갈 수 있는 경우는 배제한다;
		MAP_MATCHING_ITER iter = m_mapMatching.find( dwMatchingID );
		if ( iter != m_mapMatching.end() )
		{
			VEC_MATCHING_GROUP_ID& vecMatchingGroupID = iter->second;
			vecMatchingGroupID.push_back( dwMatchingGroupID );
		}
		else
		{
			// Matching Group 을 추가한다;
			VEC_MATCHING_GROUP_ID vecMatchingGroupID;
			vecMatchingGroupID.push_back( dwMatchingGroupID );

			m_mapMatching.insert(
				MAP_MATCHING_VALUE(
				dwMatchingID,
				vecMatchingGroupID ) );
		}

		// Matching Group 의 Matching 을 세팅한다;
		pGroup->SetMatchingID( dwMatchingID );

		return true;
	}


	MatchingGroupID GLMatchingGroupManager::PushMatching ( MatchingID dwMatchingID )
	{
		// 대기열이 비어있으면 세팅할 수 없다;
		if ( m_queueWaitingMatchingGroupID.empty() )
		{
			sc::writeLogWarn( "[ Matching Log ] [ Waiting Group is empty. ]" );

			return INVALID_MATCHING_GROUP_ID;
		}

		// 대기열의 다음 Matching Group 을 얻어온다;
		MatchingGroupID dwMatchingGroupID = m_queueWaitingMatchingGroupID.front();

		// Matching 에 세팅한다;
		if ( false == SetMatching( dwMatchingGroupID, dwMatchingID ) )
		{
			// SetMatcing Function 안에서 로그를 출력하고 있다;
			return INVALID_MATCHING_GROUP_ID;
		}

		// 세팅한 Matching Group 을 대기열에서 제거한다;
		m_queueWaitingMatchingGroupID.pop_front();

		return dwMatchingGroupID;
	}


	bool GLMatchingGroupManager::PushWaitingQueue ( MatchingGroupID dwMatchingGroupID )
	{
		GLMatchingGroup* pGroup = GetMatchingGroup( dwMatchingGroupID );
		if ( NULL == pGroup )
		{
			sc::writeLogError( sc::string::format(
				"[ Matching Log ] [ Invalid Matching Group ID ( %1% ). ]",
				dwMatchingGroupID ) );

			return false;
		}

		// Matching Group 이 이미 대기열에 있는 상태라면 취소한다;
		if ( INVALID_MATCHING_ID == pGroup->GetMatchingID() )
		{
			return false;
		}

		// 기존의 Matching 에서 삭제한다;
		MAP_MATCHING_ITER iter = m_mapMatching.find( pGroup->GetMatchingID() );
		if ( iter != m_mapMatching.end() )
		{
			VEC_MATCHING_GROUP_ID& vecMatchingGroupID = iter->second;
			VEC_MATCHING_GROUP_ID_ITER GroupIDIter = vecMatchingGroupID.begin();
			for ( ; GroupIDIter != vecMatchingGroupID.end(); )
			{
				MatchingGroupID dwTempMatchingGroupID = *GroupIDIter;
				if ( dwTempMatchingGroupID == dwMatchingGroupID )
				{
					GroupIDIter = vecMatchingGroupID.erase( GroupIDIter );
				}
				else
				{
					++GroupIDIter;
				}
			}
		}

		// Matching Group 의 Matching 을 대기상태로 바꾼다;
		pGroup->SetMatchingID( INVALID_MATCHING_ID );

		// 대기열에 추가한다;
		m_queueWaitingMatchingGroupID.push_back( dwMatchingGroupID );

		return true;
	}


	MatchingGroupID GLMatchingGroupManager::PushWaitingQueue_MatchingFirst (
		MatchingID dwMatchingID )
	{
		MatchingGroupID dwMatchingGroupID = INVALID_MATCHING_GROUP_ID;

		MAP_MATCHING_ITER iter = m_mapMatching.find( dwMatchingID );
		if ( iter != m_mapMatching.end() )
		{
			VEC_MATCHING_GROUP_ID& vecMatchingGroupID = iter->second;

			// 현재 Matching 이 비어있다면;
			if ( vecMatchingGroupID.empty() )
				return INVALID_MATCHING_GROUP_ID;

			// 비어있지 않다면 첫번째 Matching Group 을 가져온다;
			VEC_MATCHING_GROUP_ID_ITER beginGroupIDIter = vecMatchingGroupID.begin();
			dwMatchingGroupID = *beginGroupIDIter;

			GLMatchingGroup* pGroup = GetMatchingGroup( dwMatchingGroupID );
			if ( NULL == pGroup )
			{
				sc::writeLogError( sc::string::format(
					"[ Matching Log ] [ Invalid Matching Group ID ( %1% ). ]",
					dwMatchingGroupID ) );

				return false;
			}

			// Matching Group 이 이미 대기열에 있는 상태라면 취소한다;
			if ( INVALID_MATCHING_ID == pGroup->GetMatchingID() )
			{
				return false;
			}

			// Matching Group 의 Matching 을 대기상태로 바꾼다;
			pGroup->SetMatchingID( INVALID_MATCHING_ID );

			// 대기열에 추가한다;
			m_queueWaitingMatchingGroupID.push_back( dwMatchingGroupID );

			vecMatchingGroupID.erase( beginGroupIDIter );
		}

		return dwMatchingGroupID;
	}


	bool GLMatchingGroupManager::JoinCharacter (
		MatchingGroupID dwMatchingGroupID,
		DWORD dwChaDBNum )
	{
		GLMatchingGroup* pGroup = GetMatchingGroup( dwMatchingGroupID );
		if ( NULL == pGroup )
		{
			sc::writeLogError( sc::string::format(
				"[ Matching Log ] [ Invalid Matching Group ID ( %1% ). ]",
				dwMatchingGroupID ) );

			return false;
		}

		return pGroup->JoinCharacter( dwChaDBNum );
	}


	bool GLMatchingGroupManager::DropOutCharacter_MatchingGroupID (
		MatchingGroupID dwMatchingGroupID,
		DWORD dwChaDBNum )
	{
		GLMatchingGroup* pGroup = GetMatchingGroup( dwMatchingGroupID );
		if ( NULL == pGroup )
		{
			sc::writeLogError( sc::string::format(
				"[ Matching Log ] [ Invalid Matching Group ID ( %1% ). ]",
				dwMatchingGroupID ) );

			return false;
		}

		return pGroup->DropOutCharacter( dwChaDBNum );
	}


	bool GLMatchingGroupManager::DropOutCharacter ( DWORD dwChaDBNum )
	{
		DWORD dwMatchingGroupID = SearchMatchingGroupID_ChaDBNum( dwChaDBNum );
		if ( INVALID_MATCHING_GROUP_ID == dwMatchingGroupID )
			return false;

		return DropOutCharacter_MatchingGroupID( dwMatchingGroupID, dwChaDBNum );
	}


	/*DWORD GLMatchingGroupManager::MergeMatchingGroup (
		DWORD dwMatchingGroupID1,
		DWORD dwMatchingGroupID2 )
	{
		// 같은 그룹을 넣었을 경우 리턴한다;
		if ( dwMatchingGroupID1 == dwMatchingGroupID2 )
			return dwMatchingGroupID1;

		// 1번 그룹을 찾는다;
		GLMatchingGroup* pGroup1 = GetMatchingGroup( dwMatchingGroupID1 );
		if ( NULL == pGroup1 )
			return INVALID_MATCHING_GROUP_ID;

		// 2번 그룹을 찾는다;
		GLMatchingGroup* pGroup2 = GetMatchingGroup( dwMatchingGroupID2 );
		if ( NULL == pGroup2 )
			return INVALID_MATCHING_GROUP_ID;

		// 병합한다 ( 1번으로 몰아넣는다 );
		BOOST_FOREACH ( DWORD dwChaDBNum, pGroup2->GetCharacterList() )
		{
			pGroup1->JoinCharacter( dwChaDBNum );
		}

		// 병합 후에도 삭제하지 않는다;
		// 다시 병합을 풀 수도 있어야 한다;
		//DeleteMatchingGroup( pGroup2->GetGroupID() );

		return pGroup1->GetMatchingGroupID();
	}*/


	/************************************************************************/
	/* Getting                                                              */
	/************************************************************************/

	DWORD GLMatchingGroupManager::GetSizeOfMatchingGroupContainer ( )
	{
		return m_mapMatchingGroup.size();
	}


	DWORD GLMatchingGroupManager::GetSizeOfMatchingContainer ( )
	{
		return m_mapMatching.size();
	}


	DWORD GLMatchingGroupManager::GetSizeOfWaitingQueue ( )
	{
		return m_queueWaitingMatchingGroupID.size();
	}


	GLMatchingGroup* GLMatchingGroupManager::GetMatchingGroup ( MatchingGroupID dwMatchingGroupID )
	{
		MAP_MATCHING_GROUP_ITER iter = m_mapMatchingGroup.find( dwMatchingGroupID );
		if ( iter != m_mapMatchingGroup.end() )
			return iter->second;

		return NULL;
	}


	bool GLMatchingGroupManager::IsValidMatchingGroup ( MatchingGroupID dwMatchingGroupID )
	{
		GLMatchingGroup* pGroup = GetMatchingGroup( dwMatchingGroupID );
		if ( NULL == pGroup )
			return false;

		return true;
	}


	bool GLMatchingGroupManager::IsRegistCharacter ( DWORD dwChaDBNum )
	{
		// 캐릭터가 Matching Group에 속하는지 확인한다;
		if ( INVALID_MATCHING_GROUP_ID ==
			SearchMatchingGroupID_ChaDBNum( dwChaDBNum ) )
		{
			return false;
		}
		else
		{
			return true;
		}
	}


	DWORD GLMatchingGroupManager::GetCharCountOfMatching (
		MatchingID dwMatchingID )
	{
		if ( 0 == m_mapMatchingGroup.size() )
			return 0;

		DWORD dwChaCount = 0;

		MAP_MATCHING_GROUP_ITER iter = m_mapMatchingGroup.begin();
		MAP_MATCHING_GROUP_ITER eiter = m_mapMatchingGroup.end();
		for ( ;iter != eiter; ++iter )
		{
			GLMatchingGroup* pGroup = iter->second;
			if ( pGroup && pGroup->GetMatchingID() == dwMatchingID )
				dwChaCount += pGroup->GetCountCharacter();
		}

		return dwChaCount;
	}

	DWORD GLMatchingGroupManager::GetMatchingGroupCountOfMatching ( MatchingID dwMatchingID )
	{
		if ( 0 == m_mapMatchingGroup.size() )
			return 0;

		DWORD dwGroupCount = 0;

		MAP_MATCHING_GROUP_ITER iter = m_mapMatchingGroup.begin();
		MAP_MATCHING_GROUP_ITER eiter = m_mapMatchingGroup.end();
		for ( ;iter != eiter; ++iter )
		{
			GLMatchingGroup* pGroup = iter->second;
			if ( pGroup && pGroup->GetMatchingID() == dwMatchingID )
				dwGroupCount++;
		}

		return dwGroupCount;
	}

	DWORD GLMatchingGroupManager::GetCharCountOfMatchingGroup ( MatchingGroupID dwMatchingGroupID )
	{
		GLMatchingGroup* pGroup = GetMatchingGroup( dwMatchingGroupID );
		if ( NULL == pGroup )
			return 0;

		return pGroup->GetCountCharacter();
	}


	/************************************************************************/
	/* Searching;                                                           */
	/************************************************************************/

	MatchingGroupID GLMatchingGroupManager::SearchMatchingGroupID_ChaDBNum ( DWORD dwChaDBNum )
	{
		if ( 0 == m_mapMatchingGroup.size() )
			return INVALID_MATCHING_GROUP_ID;

		MAP_MATCHING_GROUP_ITER iter = m_mapMatchingGroup.begin();
		MAP_MATCHING_GROUP_ITER eiter = m_mapMatchingGroup.end();
		for ( ; iter != eiter; ++iter )
		{
			GLMatchingGroup* pGroup = iter->second;
			if ( pGroup && pGroup->IsExistCharacter( dwChaDBNum ) )
				return pGroup->GetMatchingGroupID();
		}

		return INVALID_MATCHING_GROUP_ID;
	}


	bool GLMatchingGroupManager::SearchMatchingGroup_MatchingID (
		VEC_MATCHING_GROUP_ID& refVecMatchingGroupID,
		MatchingID dwMatchingID )
	{
		if ( 0 == m_mapMatchingGroup.size() )
			return false;

		MAP_MATCHING_GROUP_ITER iter = m_mapMatchingGroup.begin();
		MAP_MATCHING_GROUP_ITER eiter = m_mapMatchingGroup.end();
		for ( ; iter != eiter; ++iter )
		{
			GLMatchingGroup* pGroup = iter->second;
			if ( pGroup && pGroup->GetMatchingID() == dwMatchingID )
			{
				refVecMatchingGroupID.push_back(
					pGroup->GetMatchingGroupID() );
			}
		}

		return true;
	}


	

	bool GLMatchingGroupManager::SearchCharacters_MatchingID (
		VEC_CHARACTER_DBNUM& refVecCharacter,
		MatchingID dwMatchingID )
	{
		// Matching 에 속하는 Matching Group 을 받아온다;
		std::vector< MatchingGroupID > vecMatchingGroupID;
		SearchMatchingGroup_MatchingID( vecMatchingGroupID, dwMatchingID );

		// 각 Matching Group 에 속하는 캐릭터를 추가한다;
		for ( unsigned int i=0; i<vecMatchingGroupID.size(); ++i )
		{
			MatchingGroupID dwMatchingGroupID = vecMatchingGroupID.at( i );
			GLMatchingGroup* pGroup = GetMatchingGroup( dwMatchingGroupID );
			if ( NULL == pGroup )
				continue;

			VEC_CHARACTER_DBNUM vecChatacterInGroup = pGroup->GetCharacterList();
			for ( unsigned int j=0; j<vecChatacterInGroup.size(); ++j )
			{
				DWORD dwChaDBNum = vecChatacterInGroup.at( j );
				refVecCharacter.push_back( dwChaDBNum );
			}
		}

		return true;
	}


	bool GLMatchingGroupManager::SearchCharacters_MatchingGroupID (
		VEC_CHARACTER_DBNUM& refVecCharacter,
		DWORD dwMatchingGroupID )
	{
		MAP_MATCHING_GROUP_ITER iter = m_mapMatchingGroup.find( dwMatchingGroupID );
		if ( iter != m_mapMatchingGroup.end() )
		{
			GLMatchingGroup* pGroup = iter->second;
			if ( pGroup && pGroup->GetMatchingGroupID() == dwMatchingGroupID )
			{
				VEC_CHARACTER_DBNUM vecChatacterInGroup = pGroup->GetCharacterList();
				BOOST_FOREACH ( DWORD dwChaDBNum, vecChatacterInGroup )
				{
					refVecCharacter.push_back( dwChaDBNum );
				}
			}
		}

		return true;
	}
	
}