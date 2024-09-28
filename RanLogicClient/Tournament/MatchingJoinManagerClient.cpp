#include "pch.h"

#include "./MatchingJoinManagerClient.h"

namespace MatchingSystem
{

	void CMatchingJoinManagerClient::OnUpdateJoinCount ( const GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC* _pMsg )
	{
		if ( NULL == _pMsg )
			return;

		// Default Message �� �� ��� �ʱ�ȭ�Ѵ�;
		if ( _pMsg->dwMatchingBaseID == MAXDWORD )
			m_vecJoinCount.clear();

		// Ƚ���� �����Ѵ�;
		for ( unsigned int i=0; i<m_vecJoinCount.size(); ++i )
		{
			SJOIN_COUNT& sJoinCount = m_vecJoinCount.at( i );
			if ( _pMsg->dwMatchingBaseID == sJoinCount.m_dwMatchingBaseID )
			{
				sJoinCount.m_dwJoinCount = _pMsg->dwJoinCount;
				return;
			}
		}

		// ���� ��� �߰��Ѵ�;
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

		// ������ 0;
		return 0;
	}

}