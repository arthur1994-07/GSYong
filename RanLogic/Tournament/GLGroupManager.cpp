#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"

#include "./GLGroupManager.h"


using namespace sc;

namespace MatchingSystem
{

	const DWORD GLMatchingGroupManager::MATCHING_GROUP_SIZE = 1000;

	GLMatchingGroupManager::GLMatchingGroupManager ( )
	{
		// �׷� ���̵� �̸� �����Ѵ�;
		for ( DWORD i = 1; i < MATCHING_GROUP_SIZE ; ++i )
		{
			m_queueUnassignedMatchingGroupID.push_back(
				static_cast< MatchingGroupID >( i ) );
		}
	}


	GLMatchingGroupManager::~GLMatchingGroupManager ( )
	{
		// Matching Group ���� �ʱ�ȭ;
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
		// �Ű����� MatchingGroupID ���´�;
		const unsigned int nGroupID = lua_tointeger( pState, -1 );
		lua_pop( pState, 1 );
		MatchingGroupID dwMatchingGroupID = static_cast< MatchingGroupID >( nGroupID );

		// ChaDBNum �� ���� ���̺��� �����Ѵ�;
		lua_newtable( pState );

		// Table �� �����ϱ� ���� ������ġ�� ���´�;
		int top = lua_gettop( pState );

		// ChaDBNum �� �˻��Ѵ�;
		VEC_CHARACTER_DBNUM vecCharacterDBNum;
		SearchCharacters_MatchingGroupID( vecCharacterDBNum, dwMatchingGroupID );

		// Table �� �����Ѵ�;
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
		// ������� ���� ID�� ������ �����Ѵ�;
		if ( 0 == m_queueUnassignedMatchingGroupID.size() )
		{
			// ���� �αװ� ���� ��� GroupManager�� ��Ī�׷���;
			// ����� �����ϰ� ���� ���ϴٴ� �ǹ��̴�;
			// ������ �ʿ��ϴ�;
			sc::writeLogError ( sc::string::format(
				"[ Matching Log ] [ Overflow MatchingGroup!!! The Matching Group is not bigger than %1%. ]",
				MATCHING_GROUP_SIZE ) );
			sc::writeLogError ( 
				"[ Matching Log ] -> [ This error is important. ]" );

			return INVALID_MATCHING_GROUP_ID;
		}

		// ���ο� ID�� ���´�;
		DWORD dwMatchingGroupID = m_queueUnassignedMatchingGroupID.front();

		// �׷��� �����Ѵ�;
		GLMatchingGroup* pGroup = new GLMatchingGroup( dwMatchingGroupID );
		if ( NULL == pGroup )
		{
			sc::writeLogError ( 
				"[ Matching Log ] [ Failed Create MatchingGroup. ]" );

			return INVALID_MATCHING_GROUP_ID;
		}

		// �׷��� Matching ID�� ����Ѵ�;
		pGroup->SetMatchingID( dwMatchingID );

		// ID�� ��������� �����ϰ� ���ο� Matching Group�� ����Ѵ�;
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

		// Matching Group �����̳ʿ��� �����Ѵ�;
		// �����ϱ� ���� Matching Group �� Matching ID �� ȹ���Ѵ�;
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

		/// Matching �����̳� �Ǵ� ��⿭���� �����Ѵ�;
		// ��⿭�� ���� ��� ��⿭���� ����;
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
		// Matching �����̳ʿ� ���� ��� Matching �����̳ʿ��� ����;
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

		// �Ҵ���� ���� Matching Group ID �� �ִ´�;
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
			// ������ �� ���̱� ������ ���۷����� ����ϸ� �ȵȴ�;
			const VEC_MATCHING_GROUP_ID vecMatchingGroupID = iter->second;

			// ���� Matching �� ����ִٸ�;
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

		/// ������ Matching �� ���� �ִ� Matching Group �� �ȵȴ�;
		/// ���� ��⿭�� ������ �Ѵ�;
		if ( INVALID_MATCHING_ID != pGroup->GetMatchingID() )
		{
			sc::writeLogError( sc::string::format(
				"[ Matching Log ] [ First, The Matching Group ( %1% ) have to go to Waiting Queue. ]",
				dwMatchingGroupID ) );

			return false;
		}

		/// Matching �� Matching Group �� �ִ´�;
		// ���� MatchingGroupID �� �� �� �ִ� ���� �����Ѵ�;
		MAP_MATCHING_ITER iter = m_mapMatching.find( dwMatchingID );
		if ( iter != m_mapMatching.end() )
		{
			VEC_MATCHING_GROUP_ID& vecMatchingGroupID = iter->second;
			vecMatchingGroupID.push_back( dwMatchingGroupID );
		}
		else
		{
			// Matching Group �� �߰��Ѵ�;
			VEC_MATCHING_GROUP_ID vecMatchingGroupID;
			vecMatchingGroupID.push_back( dwMatchingGroupID );

			m_mapMatching.insert(
				MAP_MATCHING_VALUE(
				dwMatchingID,
				vecMatchingGroupID ) );
		}

		// Matching Group �� Matching �� �����Ѵ�;
		pGroup->SetMatchingID( dwMatchingID );

		return true;
	}


	MatchingGroupID GLMatchingGroupManager::PushMatching ( MatchingID dwMatchingID )
	{
		// ��⿭�� ��������� ������ �� ����;
		if ( m_queueWaitingMatchingGroupID.empty() )
		{
			sc::writeLogWarn( "[ Matching Log ] [ Waiting Group is empty. ]" );

			return INVALID_MATCHING_GROUP_ID;
		}

		// ��⿭�� ���� Matching Group �� ���´�;
		MatchingGroupID dwMatchingGroupID = m_queueWaitingMatchingGroupID.front();

		// Matching �� �����Ѵ�;
		if ( false == SetMatching( dwMatchingGroupID, dwMatchingID ) )
		{
			// SetMatcing Function �ȿ��� �α׸� ����ϰ� �ִ�;
			return INVALID_MATCHING_GROUP_ID;
		}

		// ������ Matching Group �� ��⿭���� �����Ѵ�;
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

		// Matching Group �� �̹� ��⿭�� �ִ� ���¶�� ����Ѵ�;
		if ( INVALID_MATCHING_ID == pGroup->GetMatchingID() )
		{
			return false;
		}

		// ������ Matching ���� �����Ѵ�;
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

		// Matching Group �� Matching �� �����·� �ٲ۴�;
		pGroup->SetMatchingID( INVALID_MATCHING_ID );

		// ��⿭�� �߰��Ѵ�;
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

			// ���� Matching �� ����ִٸ�;
			if ( vecMatchingGroupID.empty() )
				return INVALID_MATCHING_GROUP_ID;

			// ������� �ʴٸ� ù��° Matching Group �� �����´�;
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

			// Matching Group �� �̹� ��⿭�� �ִ� ���¶�� ����Ѵ�;
			if ( INVALID_MATCHING_ID == pGroup->GetMatchingID() )
			{
				return false;
			}

			// Matching Group �� Matching �� �����·� �ٲ۴�;
			pGroup->SetMatchingID( INVALID_MATCHING_ID );

			// ��⿭�� �߰��Ѵ�;
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
		// ���� �׷��� �־��� ��� �����Ѵ�;
		if ( dwMatchingGroupID1 == dwMatchingGroupID2 )
			return dwMatchingGroupID1;

		// 1�� �׷��� ã�´�;
		GLMatchingGroup* pGroup1 = GetMatchingGroup( dwMatchingGroupID1 );
		if ( NULL == pGroup1 )
			return INVALID_MATCHING_GROUP_ID;

		// 2�� �׷��� ã�´�;
		GLMatchingGroup* pGroup2 = GetMatchingGroup( dwMatchingGroupID2 );
		if ( NULL == pGroup2 )
			return INVALID_MATCHING_GROUP_ID;

		// �����Ѵ� ( 1������ ���Ƴִ´� );
		BOOST_FOREACH ( DWORD dwChaDBNum, pGroup2->GetCharacterList() )
		{
			pGroup1->JoinCharacter( dwChaDBNum );
		}

		// ���� �Ŀ��� �������� �ʴ´�;
		// �ٽ� ������ Ǯ ���� �־�� �Ѵ�;
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
		// ĳ���Ͱ� Matching Group�� ���ϴ��� Ȯ���Ѵ�;
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
		// Matching �� ���ϴ� Matching Group �� �޾ƿ´�;
		std::vector< MatchingGroupID > vecMatchingGroupID;
		SearchMatchingGroup_MatchingID( vecMatchingGroupID, dwMatchingID );

		// �� Matching Group �� ���ϴ� ĳ���͸� �߰��Ѵ�;
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