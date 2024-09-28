#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/Tournament/GLTournamentTreeDefine.h"

class GLAgentServer;

namespace MatchingSystem
{

	// Matching ���� Ƚ�� ������;
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
		// Server <-> Client ����ȭ;
		void						SyncState ( const DWORD _dwClientID, const DWORD _dwChaDBNum );

		// ������ ���� Ƚ���� ���´�;
		// ��Ī�� �����ϱ� ���� Ƚ���� üũ�ؾ� �Ѵ�;
		const DWORD					GetJoinCount_DBNum ( const MatchingBaseID _dwMatchingGameID, const DWORD _dwChaDBNum ) const;

		// Matching �� ������ �� Ƚ���� üũ�Ѵ�;
		// �� �Լ��� �δ� ������ �Ϸ�Ǿ��� �� �����ؾ� �Ѵ�;
		void						JoinMatching_DBNum ( const MatchingBaseID _dwMatchingGameID, const DWORD _dwChaDBNum );

		// Matching �� ���� Ƚ���� 1 �����Ѵ�;
		const bool					ChargeJoinCount_DBNum ( const MatchingBaseID _dwMatchingGameID, const DWORD _dwChaDBNum );

		// Join Count List �� �ʱ�ȭ�Ѵ�;
		void						ResetJoinCountMap ();

	private:
		/// DB Action;
		// Join Count List �� �ҷ��´�;
		void						DBActionLoadJoinCountList ( MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList );

		// Join Count List �� �ʱ�ȭ�Ѵ�;
		void						DBActionResetJoinCountList ();

		// Join Count �� �����Ѵ�;
		void						DBActionUpdateJoinCount ( const DWORD _dwChaDBNum, const SJOIN_COUNT _sJoinCount );

	public:
		inline void					SetServer ( GLAgentServer* _pServer ) { m_pServer = _pServer; }

	private:
		// �� Matching�� ĳ���� �� ���� Ƚ���� �����ϴ� �����̳�;
		MAP_CHAR_JOIN_COUNT_LIST	m_mapCharJoinCount;

		// �Ϸ翡 �ѹ� ���µǴ� Ÿ�̸� ������ �Ϸ�Ǿ��°�?;
		bool						m_bUsedTimer;

		GLAgentServer*				m_pServer;

	private:
		CMatchingJoinManager() : m_pServer ( NULL ), m_bUsedTimer ( false ) { }
		~CMatchingJoinManager() { }
	};

}