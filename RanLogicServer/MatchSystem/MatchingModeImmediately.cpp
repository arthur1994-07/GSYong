#include "pch.h"

#include "./MatchingManager.h"

#include "./MatchingGameBase.h"
#include "./MatchingInstanceBase.h"

#include "./MatchingModeImmediately.h"

namespace MatchingSystem
{
	const std::string MATCHING_MODE_MAP_ID			= "BattleMapID";

	CMatchingModeImmediately::CMatchingModeImmediately ( MatchingInstanceBase* _pMatchingInstance
		, sc::LuaInstance* pScript)
		: IMatchingMode ()
		, m_pMatchingInstance ( _pMatchingInstance )
		, m_pScript ( pScript )
		, m_bCreateInstanceMap ( false )
	{

	}


	CMatchingModeImmediately::~CMatchingModeImmediately ()
	{

	}


	void CMatchingModeImmediately::OnInitialize ()
	{
		// Contents Map ID 세팅;
		{
			LuaPlus::LuaObject objMapIDTable =
				m_pScript->GetGlobalValue_Lua( MATCHING_MODE_MAP_ID.c_str() );
			if ( objMapIDTable.IsTable() )
			{
				int scriptid = 0;

				int nMainMapID = 0;
				int nSubMapID = 0;

				LuaPlus::LuaTableIterator iter( objMapIDTable );
				m_pScript->GetIntegerGlobalValueObject_Lua( iter.GetValue(), nMainMapID );
				iter.Next();
				m_pScript->GetIntegerGlobalValueObject_Lua( iter.GetValue(), nSubMapID );
				iter.Next();
				/*wMainMapID = iter.GetValue().GetNumber();
				iter.Next();
				wSubMapID = iter.GetValue().GetNumber();
				iter.Next();*/

				m_sMapID = SNATIVEID( nMainMapID, nSubMapID );

				sc::writeLogInfo( sc::string::format(
					"[ Matching Log ] [ MATCHING_MODE_MAP_ID = ( %1%, %2% ) ]",
					nMainMapID, nSubMapID ) );
			}
			else
			{
				m_sMapID = SNATIVEID( 180, 0 );

				sc::writeLogInfo( "[ Matching Log ] [ MATCHING_MODE_MAP_ID dose not Exist. ]" );
				sc::writeLogInfo( "[ Matching Log ] -> [ MATCHING_MODE_MAP_ID default = ( 180, 0 ) ]" );
			}
		}
	}


	void CMatchingModeImmediately::OnFrameMove ( float _fElapsedTime )
	{

	}


	bool CMatchingModeImmediately::OnEventCustomMsg (
		const InstanceSystem::InstanceMapID _nInstanceMapID,
		const double _fParam1,
		const double _fParam2,
		const double _fParam3,
		const double _fParam4 )
	{
		return true;
	}


	void CMatchingModeImmediately::OnMapCreate (
		DWORD _dwInstanceMapID,
		bool _bDestroyComplete )
	{

	}


	void CMatchingModeImmediately::OnMapMultiCreate (
		RoomMultyReceipt* _RoomReceipt )
	{
		if ( NULL == m_pMatchingInstance )
			return;

		MatchingGameBase* pMatchingBase =
			m_pMatchingInstance->GetMatchingGameBase();
		if ( NULL == pMatchingBase )
			return;

		MatchingSystem::GLMatchingGroupManager* pGroupManager =
			pMatchingBase->getGroupManager();
		if ( NULL == pGroupManager )
			return;

		bool bMapCreate = false;
		DWORD dwInstanceMapID = 0;

		BOOST_FOREACH ( DWORD dwTempInstanceMApID, _RoomReceipt->_instanceMapID )
		{
			sc::writeLogInfo(
				sc::string::format(
				"[ Matching Log ] [ Completed Create Map / Instance Map ID = %1% ]",
				dwTempInstanceMApID ).c_str() );

			bMapCreate = true;
			dwInstanceMapID = dwTempInstanceMApID;
		}

		if ( false == bMapCreate )
		{
			sc::writeLogInfo( "[ Matching Log ] [ Failed Create Map ]" );

			m_bCreateInstanceMap = false;

			return;
		}

		m_pMatchingInstance->SetMatchingStatus( EMMATCHING_CONTENTS_IN_PROGRESS );

		// 플레이하지 않는 그룹의 레이팅 계산;
		int nWaitingQueueSize = m_pMatchingInstance->GetMatchingWaitingGroupCount();
		for ( int i=0; i<nWaitingQueueSize; ++i )
		{
			DWORD dwTempGroupID = m_pMatchingInstance->GetGroupToMatching();
			if ( !pGroupManager->IsValidMatchingGroup( dwTempGroupID ) )
				continue;

			m_pMatchingInstance->SendJoin(
				dwTempGroupID,
				dwInstanceMapID,
				Faction::GLActorID(),
				0.f, 0.f );

			m_pMatchingInstance->FinishGroup( dwTempGroupID );
		}
	}


	const bool CMatchingModeImmediately::OnDestroy (
		DWORD dwInstanceMapID,
		bool bDestroyComplete )
	{
		m_bCreateInstanceMap = false;

		m_pMatchingInstance->SetMatchingStatus( EMMATCHING_CONTENTS_CLOSED );

		// 이 맵에 있는 모든 녀석들을 나가게 한다;
		return false;
	}


	const bool CMatchingModeImmediately::OnReset ()
	{
		return true;
	}


	void CMatchingModeImmediately::OnReadyGroupAnswer (
		DWORD dwGroupID,
		bool bOK )
	{

	}


	void CMatchingModeImmediately::OnReadyAnswer ( )
	{

	}


	const bool CMatchingModeImmediately::OnReceiveError (
		DWORD dwErrorCode )
	{
		return true;
	}


	const bool CMatchingModeImmediately::OnNoticeResult (
		const InstanceSystem::InstanceMapID _instanceID,
		const DWORD _FactionID,
		const BOOL _IsWinner )
	{
		return true;
	}


	void CMatchingModeImmediately::StartMatching ()
	{
		if ( NULL == m_pMatchingInstance )
			return;

		if ( false == m_bCreateInstanceMap )
		{
			m_bCreateInstanceMap = true;

			m_pMatchingInstance->MapMultiCreate( m_sMapID.dwID, 1 );
		}
	}


	void CMatchingModeImmediately::SetInfoMatchingImmediately (
		SNATIVEID sMapID )
	{
		m_sMapID = sMapID;
	}

}