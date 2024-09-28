#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Tournament/GLTournamentTreeDefine.h"

namespace MatchingSystem
{

	// Matching ���� Ƚ�� ������;
	class CMatchingJoinManagerClient : public CSingletone< CMatchingJoinManagerClient >
	{
	protected:
		friend CSingletone;

	public:
		/// Callback Function;
		void CALLBACK				OnUpdateJoinCount ( const GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC* _pMsg );

	public:
		/// ������ ���� Ƚ���� ���´�;
		const DWORD					GetJoinCount ( const MatchingBaseID _dwMatchingGameID ) const;

	private:
		// �� Matching�� ĳ���� �� ���� Ƚ���� �����ϴ� �����̳�;
		VEC_JOIN_COUNT				m_vecJoinCount;

	private:
		CMatchingJoinManagerClient() { }
		~CMatchingJoinManagerClient() { }
	};
	
}