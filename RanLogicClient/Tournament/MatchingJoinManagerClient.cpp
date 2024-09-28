#include "pch.h"

#include "./MatchingJoinManagerClient.h"

namespace MatchingSystem
{

	void CMatchingJoinManagerClient::OnUpdateJoinCount ( const GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC* _pMsg )
	{
		if ( NULL == _pMsg )
			return;

		// Default Message 가 올 경우 초기화한다;
		if ( _pMsg->dwMatchingBaseID == MAXDWORD )
			m_vecJoinCount.clear();

		// 횟수를 갱신한다;
		for ( unsigned int i=0; i<m_vecJoinCount.size(); ++i )
		{
			SJOIN_COUNT& sJoinCount = m_vecJoinCount.at( i );
			if ( _pMsg->dwMatchingBaseID == sJoinCount.m_dwMatchingBaseID )
			{
				sJoinCount.m_dwJoinCount = _pMsg->dwJoinCount;
				return;
			}
		}

		// 없을 경우 추가한다;
		SJOIN_COUNT sTempJoinCount;
		sTempJoinCount.m_dwMatchingBaseID = _pMsg->dwMatchingBaseID;
		sTempJoinCount.m_dwJoinCount = _pMsg->dwJoinCount;

		m_vecJoinCount.push_back( sTempJoinCount );
	}

	const DWORD CMatchingJoinManagerClient::GetJoinCount ( const MatchingBaseID _dwMatchingGameID ) const
	{
		for ( unsigned int i=0; i<m_vecJoinCount.size(); ++i )
		{
			const SJOIN_COUNT sJoinCount = m_vecJoinCount.at( i );
			if ( _dwMatchingGameID == sJoinCount.m_dwMatchingBaseID )
				return sJoinCount.m_dwJoinCount;
		}

		// 없으면 0;
		return 0;
	}

}