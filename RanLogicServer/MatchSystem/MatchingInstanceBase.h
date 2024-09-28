#pragma once

#include "MatchingCallbackTriggerDefine.h"

namespace MatchingSystem
{
	class JoinTable;
	struct KeyPair;
	class MatchingGameBase;

	class IMatchingMode;

	class MatchingInstanceBase
	{
	protected:
		typedef std::vector< JoinTable* >		JOINTABLE_VEC;			///< 그룹 벡터;
		typedef JOINTABLE_VEC::iterator			JOINTABLE_VEC_ITOR;		///< 그룹 이터레이터;

		typedef std::vector< JoinTrigger* >		JOINTRIGGER_VEC;		///< 그룹 벡터;
		typedef JOINTRIGGER_VEC::iterator		JOINTRIGGER_VEC_ITOR;	///< 그룹 이터레이터;

	public:
		MatchingInstanceBase (
			MatchingGameBase* pMatchingBase,
			sc::LuaInstance* pScript,
			DWORD dwScriptID,
			DWORD dwMatchingInstanceID );
		~MatchingInstanceBase ();

	public:
		/************************************************************************/
		/* Callback Function;                                                   */
		/************************************************************************/
		void	OnInitialize ();
		void	OnSetMatchingMode ( DWORD dwMatchingModeID );
		void	OnFrameMove ( float fElpaseTime );
		bool	OnEventCustomMsg ( const InstanceSystem::InstanceMapID nInstanceMapID, const double fParam1, const double fParam2, const double fParam3, const double fParam4 );
		//int		OnMapCreate ( RoomReceipt* pRoomReceipt );
		int		OnMapMultiCreate ( RoomMultyReceipt* pRoomReceipt );
		int		OnJoin ( JoinReceipt* pRoomReceipt );
		int		OnDestroy ( DestroyReceipt* pRoomReceipt );
		bool	OnReset ();

		// Lua 고유 Callback;
		float	OnLuaFrameMove ( float fElpaseTime );

		// Join 응답 Callback;
		int		OnReadyAnswer ( ReadyReceipt* pReadyReceipt );

		/// 필요없음;
		// 인던 스크립트와의 통신을 위한 콜백 - 토너먼트에서만 조금 사용 중;
		bool	OnReceiveDataStart ( const InstanceSystem::InstanceMapID _instanceMapID );
		bool	OnReceiveDataEnd ( const InstanceSystem::InstanceMapID _instanceMapID );
		bool	OnReceiveError ( const InstanceSystem::InstanceMapID _instanceMapID,const DWORD _ErrorCode );

		bool	OnReceiveData ( const InstanceSystem::InstanceMapID _instanceMapID, const char* varName, const double _data );
		bool	OnReceiveData ( const InstanceSystem::InstanceMapID _instanceMapID, const char* varName, const char* _data );
		bool	OnReceiveData ( const InstanceSystem::InstanceMapID _instanceMapID, const char* varName, const bool _data );

		bool	OnNoticeResult ( const InstanceSystem::InstanceMapID _instanceMapID, const DWORD _FactionID, const BOOL _IsWinner );

		// Matching Group 정보 업데이트;
		void	OnUpdateMatchingGroupInfo ( DWORD ClietDBID );

	public:
		/************************************************************************/
		/* Invoke Function;                                                     */
		/************************************************************************/

		/// Lua 에 종속적인 Invoke;
		int		ScriptInvoke_SendJoin ( lua_State* pState );
		int		ScriptInvoke_WaitTime ( lua_State* L );

		//int		ScriptInvoke_SendToGroupAskJoinAsyc ( lua_State* L );	// 조인 등록한 그룹들의 조인 물어봄;

		int		ScriptInvoke_MapMultiCreate ( lua_State* pState );

		/// 일반 Invoke;

		/// 조인 관리;

		// 그룹 조인 확인 후 조인시킴 ( 동기화 버전 ) - 아직 사용해본 적이 없는 로직;
		//bool	SetGroupAskJoin ( DWORD GroupID );			// 조인 등록한 그룹들 저장;
		//int		SendToGroupAskJoinAsyc ( float _fTimer );	// 조인 등록한 그룹들의 조인 물어봄;

		// 입장 하기전에 확인한다;
		bool	SendToGroupAskJoin ( float fTimer );

		// 그룹 Join;
		int		SendJoin ( DWORD _dwGroupID, DWORD _dwInstanceID, Faction::GLActorID _targetActorID, float _fPositionX, float _fPositionZ );

		// Join Lock을 건다;
		void	JoinLock ( DWORD _instanceMapID, const bool _bLock );

		// Join Suspend를 건다 ( 조인 대기 );
		void	JoinSuspend ( DWORD _instanceMapID, const bool _bSuspend );

		// 조인 거부 상태자를 세팅한다;
		void	SetJoinCanselDrop ( bool _JoinCanselDropCondition );


		/// Utility;

		// 대기;
		int		WaitTime ( float _fDuration );


		/// 그룹 관리;
		// 로비의 그룹 가져오기;
		int		GetGroupToMatching ( );
		int		GetGroupToMatchingNotPlay ( );
		void	SetGroupToPlay ( DWORD GroupID );

		// 그룹 내보내기 -> 제일 상단에 넣어준다;
		void	SetGroupToLobby ( DWORD GroupID );

		// 아예 게임이 성립안됨 -> 다시 대기 상태로 보내주자;
		void	SetGroupToLobbyALL ( );

		// 메칭 그룹에 몇명이 기다리고 있는지 알아본다;
		int		GetMatchingWaitingGroupCount ( );

		// 시간이 지났음 타임아웃;
		bool	OutCharInLobby ( DWORD ClientDBnum );

		// 반쯤 그룹을 부순다. (진입가능 중이라면 바로 진입 **불가능!!!);
		void	SemiFinishGroup ( DWORD GroupID );

		// 그룹을 최종적으로 부셔버린다;
		void	FinishGroup ( DWORD GroupID );

		// 등록된 모든 그룹을 최종적으로 부셔 버린다 (진입가능 중이라면 바로 다시 진입가능);
		void	FinishGroupALL ( );

		// 그룹을 통합한다. 나중에 다시 풀수있도록 세팅이 되어있다;
		//DWORD	MergeGroup ( DWORD _lGroupID, DWORD _rGroupID);

		// 최종 우승 그룹을 설정한다;
		void	SetFinalWinner ( DWORD GroupID );

		// 진영 선택하기;
		void	SendFaction ( DWORD GroupID, DWORD _FactionID, DWORD _instanceID );


		/// 매칭 관리;
		// 메칭 시작;
		bool	StartMatching ( );

		// 시작 준비가 되었는지 확인한다;
		bool	IsReadyToStart ( );

		// 매칭 상태를 입력한다. ( 입장 가능 불가.. 등등);
		void	SetMatchingStatus ( DWORD _Status );

		// 매칭 플레이 시간을 설정한다 ( 현재 기능 없음 );
		void	SetMatchingPlayTime ( DWORD _TimeH, DWORD _TimeM );

		// 매칭을 파괴한다;
		bool	DoDestroy ( );

		// 랭크 스코어 세팅 ( 로그 찍는 함수 );
		void	SetRankScore ( DWORD dbnum, int rating );


		/// 현재 토너먼트에서만 사용되는 매칭 관리;
		// 매칭의 해당 라운드를 설정해준다;
		void	SetMatchingRound ( DWORD _Round );

		// 매칭 사이즈를 세팅한다;
		void	SetMatchingSize ( DWORD _dwMatchingSize );

		// 매칭 플레이어들을 모두 리셋 시켜버린다;
		void	SetMatchingPlayersReset ( );

		// 매칭 플레이어를 설정한다;
		// 라운드 순대로 Stack 방식으로 쌓아서;
		// 알아서 클라이언트가 정리하여 보여주는방식;
		// ( 스크립트 전용 );
		int		SetMatchingPlayers ( lua_State* pState );


		/// 인던 관리;
		// 맵 생성;
		//bool	RequestMapCreate ( DWORD _MapID, DWORD _Channel );
		int		MapMultiCreate ( DWORD _dwMapID, DWORD _dwMapSize, lua_State* _pState = NULL );

		// 인던에서 나가기;
		void	DoOut ( DWORD _dbNum, DWORD _instanceMapID );

	private:
		/************************************************************************/
		/* Inner Function;                                                      */
		/************************************************************************/
		// 응답 후 조인시키는 그룹조인관련 Inner Function - 아직 사용한 적이 없는 로직;
		bool	_SendToClientAskJoin ( DWORD ClientDBnum, float Time = 5, bool isUseCounting = false );
		void	_SetAnswerEnded ();

		// 매칭 플레이어 세팅 - 토너먼트에서만 사용;
		// 토너먼트 사용을 위해 추가 수정 필요 ( 스크립트 사용 안할 시 );
		void	_SetMatchingPlayerTable ( KeyPair player, DWORD num );

		// 그룹 클라이언트 상태 갱신;
		int		_GroupClientStatusUpdate ( DWORD GroupID, EMCUSTOMER_STATUS_TYPE status );

	public:
		/************************************************************************/
		/* Getter;                                                              */
		/************************************************************************/
		// Matching의 Base를 가져온다;
		inline MatchingGameBase*	GetMatchingGameBase() { return m_pMatchingBase; }

		// Matching의 파괴 가능 여부를 가져온다;
		inline const bool			CanDestroy() { return m_bDestroyMatchingInstance; }

		// Matching Instance의 ID를 가져온다;
		inline DWORD				GetMatchingInstanceID() { return m_dwMatchingInstanceID; }

	public:
		// 매칭 그룹 정보;
		// 외부에 노출되어있는 상태이다;
		std::vector< GLMatChingGroupInfo >	m_vecMatchingGroupInfo;

	private:
		// Lua Script 사용을 위함;
		sc::LuaInstance*			m_pScript;
		lua_State*					m_pCoroutineState;

		// 코루틴 관련;
		// Script 사용 안할 때도 사용한다;
		bool						m_bUseCoroutine;			// 코루틴 사용 여부;
		float						m_fWaitDuration;			// 대기 시간;
		bool						m_bSetWait;					// 대기 상태 여부;

		// Matching Game의 Base;
		// Base를 바탕으로 파생된 것이 Matching Instance이다;
		// 실제 Matching은 Instance;
		MatchingGameBase*			m_pMatchingBase;

		// Matching Instance의 ID;
		DWORD						m_dwMatchingBaseID;
		DWORD						m_dwMatchingInstanceID;

		// 현 Matching Instance의 Join 횟수;
		int							m_nJoinCount;

		// 인던 스크립트와의 통신 결과;
		bool						m_bReceiveDataError;

		// 입장 취소 상태;
		bool						m_bJoinCancelDropCondition;

		/// 로비에서 가져온 그룹 리스트 ( 토너먼트 전용으로 잠시 둔다 );
		GROUP_VEC					m_vecGettingGroup;

		// Matching Info;
		DWORD						m_dwMatchingRound;
		DWORD						m_dwMatchingSize;

		// Join Fail Trigger -> 현재 절대 등록되지 않는다;
		JOINTRIGGER_VEC				m_JoinFailTrigger;

		// 조인 질문 -> 응답 확인 후 조인시키는 방식에서 사용하는 변수;
		//JOINTABLE_VEC				m_JoinTable;
		//GROUP_VEC					m_vecJoinSaved;

		bool						m_bWaitingAnswer;			// 기다리는 중;
		float						m_fAnswerTimer;				// 시간;
		DWORD						m_dwAnswerCharacterCount;	// 대답한 캐릭터 수;

		// Matching Instance의 파괴 허가 여부;
		bool						m_bDestroyMatchingInstance;

		// Matching Mode;
		// 스크립트를 사용하는 경우 NULL / Mode ID : 0;
		// 스크립트를 사용하지 않는 경우 State Pattern으로 바뀔 수 있다;
		DWORD						m_dwMatchingModeID;
		IMatchingMode*				m_pCurMatchingMode;
	};

}