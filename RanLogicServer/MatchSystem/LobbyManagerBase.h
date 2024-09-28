#pragma once

#include "../../SigmaCore/Lua/MinLua.h"

#include "../../RanLogic/Tournament/GLTournamentTreeDefine.h"

namespace MatchingSystem
{
	class MatchingGameBase;

	/************************************************************************/
	/* 유저들의 신청을 받아 관리하여 Matching에 보내주는 역할을 한다;  */
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

		/// 다시 Lobby 로 복귀한 경우의 Callback;
		bool	OnComebackMatchingGroup ( DWORD dwMatchingGroupID );

		/// 캐릭터 Drop Out Callback;
		void	OnDropOutChar ( DWORD dwChaDBNum );

		/// 파티 관련 처리 Callback;
		void	OnChangeParty ( DWORD dwChaDBNum );
		void	OnChangePartyMember ( DWORD dwChaDBNum, DWORD dwParyID );
		void	OnDropOutPartyMember ( DWORD dwChaDBNum, DWORD dwParyID );
		void	OnChangePartyMaster ( DWORD dwOldChaDBNum, DWORD dwNewChaDBNum, DWORD dwParyID );
		void	OnDestoryParty ( DWORD dwParyID );

	public:
		/************************************************************************/
		/* Invoke Function;                                                     */
		/************************************************************************/
		/// Matching System 관리;
		bool	IsJoinGame ( DWORD dwChaDBNum );					// 유저가 이미 입장한 상태인지 확인한다;
		DWORD	GetJoinGame ( const DWORD dwChaDBNum );				// 유저가 입장한 게임이 뭔지 확인한다;

		const bool	CanRejoin ( DWORD dwChaDBNum );

		// Matching System 에서 내보낸다;
		void	OutMatchingSystem ( DWORD dwChaDBNum );

		/// Matching 관리;
		// Matching 을 시작한다;
		bool	StartMatching ( );

		// 캐릭터가 임시 로비로 입장;
		// 이는 Matching Group 을 만드는데 사용된다;
		bool	JoinCharacter_TempLobby ( DWORD dwChaDBNum );

		// 입장한 캐릭터들을 이용해 Matching Group 을 만든다;
		void	MakeMatchingGroup ( );

		// Matching 입장 취소;
		void	CancelJoinMatching ( const DWORD dwChaDBNum );

		/// Matching Group 관리;
		bool	IsValidMatchingGroup ( DWORD dwChaDBNum );
		DWORD	SearchMatchingGroup ( DWORD dwChaDBNum );
		void	DeleteMatchingGroup ( DWORD dwChaDBNum );

	public:
		/// 의미없는 Function ( 제대로 동작하지 않는다, 수정필요 );
		DWORD	GetCountTournamentPeople ( );				// 토너먼트 현제 인원;
		int		GetSchoolParticipation ( DWORD dwSchool );	// 학원별 인원 정보 요청;

		/// Tournament 관련;
		bool	CheckMoney ( DWORD dwChaDBNum, DWORD dwMoney );
		bool	SpendMoney ( DWORD dwChaDBNum, DWORD dwMoney );

		void	ClientMoneySpendCallBack ( DWORD dwChaDBNum, BOOL bOk );
		void	ClientMoneyCheckCallBack ( DWORD dwChaDBNum, BOOL bOk );

	public:
		inline sc::LuaInstance*		GetScript() { return m_pScript; };

	private:
		// Matching Group 이 생성되기전에 임시로 캐릭터를 저장하는 컨테이너;
		VEC_CHARACTER_DBNUM			m_tempVecCharacterDBNum;

		// FrameMove Timer;
		float						m_fTime;

	private:
		sc::LuaInstance*			m_pScript;
		MatchingGameBase*			m_pMatchingBase;
	};

}