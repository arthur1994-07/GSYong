#pragma once

#include "../../SigmaCore/Lua/MinLua.h"

#include "../../RanLogic/Tournament/GLTournamentTreeDefine.h"

namespace MatchingSystem
{
	class MatchingGameBase;

	/************************************************************************/
	/* �������� ��û�� �޾� �����Ͽ� Matching�� �����ִ� ������ �Ѵ�;  */
	/************************************************************************/
	class LobbyManagerbase
	{
	public:
		LobbyManagerbase (
			MatchingGameBase* pMatchingBase,
			sc::LuaInstance* pScript );
		~LobbyManagerbase ( );

	public:
		void	RegistCallbackFunction_Lua ( sc::LuaInstance* pScript );

	public:
		/************************************************************************/
		/* Callback Function;                                                   */
		/************************************************************************/
		void	OnInitialize ( );
		void	OnFrameMove ( float fElpaseTime );
		bool	OnReqReadyMatching ( float fElpaseTime );
		bool	OnReqJoinMatching ( DWORD dwChaDBNum );

		/// �ٽ� Lobby �� ������ ����� Callback;
		bool	OnComebackMatchingGroup ( DWORD dwMatchingGroupID );

		/// ĳ���� Drop Out Callback;
		void	OnDropOutChar ( DWORD dwChaDBNum );

		/// ��Ƽ ���� ó�� Callback;
		void	OnChangeParty ( DWORD dwChaDBNum );
		void	OnChangePartyMember ( DWORD dwChaDBNum, DWORD dwParyID );
		void	OnDropOutPartyMember ( DWORD dwChaDBNum, DWORD dwParyID );
		void	OnChangePartyMaster ( DWORD dwOldChaDBNum, DWORD dwNewChaDBNum, DWORD dwParyID );
		void	OnDestoryParty ( DWORD dwParyID );

	public:
		/************************************************************************/
		/* Invoke Function;                                                     */
		/************************************************************************/
		/// Matching System ����;
		bool	IsJoinGame ( DWORD dwChaDBNum );					// ������ �̹� ������ �������� Ȯ���Ѵ�;
		DWORD	GetJoinGame ( const DWORD dwChaDBNum );				// ������ ������ ������ ���� Ȯ���Ѵ�;

		const bool	CanRejoin ( DWORD dwChaDBNum );

		// Matching System ���� ��������;
		void	OutMatchingSystem ( DWORD dwChaDBNum );

		/// Matching ����;
		// Matching �� �����Ѵ�;
		bool	StartMatching ( );

		// ĳ���Ͱ� �ӽ� �κ�� ����;
		// �̴� Matching Group �� ����µ� ���ȴ�;
		bool	JoinCharacter_TempLobby ( DWORD dwChaDBNum );

		// ������ ĳ���͵��� �̿��� Matching Group �� �����;
		void	MakeMatchingGroup ( );

		// Matching ���� ���;
		void	CancelJoinMatching ( const DWORD dwChaDBNum );

		/// Matching Group ����;
		bool	IsValidMatchingGroup ( DWORD dwChaDBNum );
		DWORD	SearchMatchingGroup ( DWORD dwChaDBNum );
		void	DeleteMatchingGroup ( DWORD dwChaDBNum );

	public:
		/// �ǹ̾��� Function ( ����� �������� �ʴ´�, �����ʿ� );
		DWORD	GetCountTournamentPeople ( );				// ��ʸ�Ʈ ���� �ο�;
		int		GetSchoolParticipation ( DWORD dwSchool );	// �п��� �ο� ���� ��û;

		/// Tournament ����;
		bool	CheckMoney ( DWORD dwChaDBNum, DWORD dwMoney );
		bool	SpendMoney ( DWORD dwChaDBNum, DWORD dwMoney );

		void	ClientMoneySpendCallBack ( DWORD dwChaDBNum, BOOL bOk );
		void	ClientMoneyCheckCallBack ( DWORD dwChaDBNum, BOOL bOk );

	public:
		inline sc::LuaInstance*		GetScript() { return m_pScript; };

	private:
		// Matching Group �� �����Ǳ����� �ӽ÷� ĳ���͸� �����ϴ� �����̳�;
		VEC_CHARACTER_DBNUM			m_tempVecCharacterDBNum;

		// FrameMove Timer;
		float						m_fTime;

	private:
		sc::LuaInstance*			m_pScript;
		MatchingGameBase*			m_pMatchingBase;
	};

}