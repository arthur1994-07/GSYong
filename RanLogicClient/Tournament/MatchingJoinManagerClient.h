#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Tournament/GLTournamentTreeDefine.h"

namespace MatchingSystem
{

	// Matching 입장 횟수 관리자;
	class CMatchingJoinManagerClient : public CSingletone< CMatchingJoinManagerClient >
	{
	protected:
		friend CSingletone;

	public:
		/// Callback Function;
		void CALLBACK				OnUpdateJoinCount ( const GLMSG::NET_MATCHING_UPDATE_JOIN_COUNT_AC* _pMsg );

	public:
		/// 유저의 입장 횟수를 얻어온다;
		const DWORD					GetJoinCount ( const MatchingBaseID _dwMatchingGameID ) const;

	private:
		// 각 Matching의 캐릭터 별 입장 횟수를 저장하는 컨테이너;
		VEC_JOIN_COUNT				m_vecJoinCount;

	private:
		CMatchingJoinManagerClient() { }
		~CMatchingJoinManagerClient() { }
	};
	
}