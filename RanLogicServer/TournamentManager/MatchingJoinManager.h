#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/Tournament/GLTournamentTreeDefine.h"

class GLAgentServer;

namespace MatchingSystem
{

	// Matching 입장 횟수 관리자;
	class CMatchingJoinManager : public CSingletone< CMatchingJoinManager >
	{
	protected:
		friend CSingletone;

	public:
		/// Callback Function;
		void CALLBACK				OnLoad ();
		void CALLBACK				OnFrameMove ( const float _fElapsedTime, const CTime& _sCurrentTime );

	public:
		/// Invoke Function;
		// Server <-> Client 동기화;
		void						SyncState ( const DWORD _dwClientID, const DWORD _dwChaDBNum );

		// 유저의 입장 횟수를 얻어온다;
		// 매칭에 입장하기 전에 횟수를 체크해야 한다;
		const DWORD					GetJoinCount_DBNum ( const MatchingBaseID _dwMatchingGameID, const DWORD _dwChaDBNum ) const;

		// Matching 에 입장할 때 횟수를 체크한다;
		// 이 함수는 인던 입장이 완료되었을 때 수행해야 한다;
		void						JoinMatching_DBNum ( const MatchingBaseID _dwMatchingGameID, const DWORD _dwChaDBNum );

		// Matching 의 입장 횟수를 1 충전한다;
		const bool					ChargeJoinCount_DBNum ( const MatchingBaseID _dwMatchingGameID, const DWORD _dwChaDBNum );

		// Join Count List 를 초기화한다;
		void						ResetJoinCountMap ();

	private:
		/// DB Action;
		// Join Count List 를 불러온다;
		void						DBActionLoadJoinCountList ( MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList );

		// Join Count List 를 초기화한다;
		void						DBActionResetJoinCountList ();

		// Join Count 를 갱신한다;
		void						DBActionUpdateJoinCount ( const DWORD _dwChaDBNum, const SJOIN_COUNT _sJoinCount );

	public:
		inline void					SetServer ( GLAgentServer* _pServer ) { m_pServer = _pServer; }

	private:
		// 각 Matching의 캐릭터 별 입장 횟수를 저장하는 컨테이너;
		MAP_CHAR_JOIN_COUNT_LIST	m_mapCharJoinCount;

		// 하루에 한번 리셋되는 타이머 실행이 완료되었는가?;
		bool						m_bUsedTimer;

		GLAgentServer*				m_pServer;

	private:
		CMatchingJoinManager() : m_pServer ( NULL ), m_bUsedTimer ( false ) { }
		~CMatchingJoinManager() { }
	};

}