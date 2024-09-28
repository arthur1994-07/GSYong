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
		typedef std::vector< JoinTable* >		JOINTABLE_VEC;			///< �׷� ����;
		typedef JOINTABLE_VEC::iterator			JOINTABLE_VEC_ITOR;		///< �׷� ���ͷ�����;

		typedef std::vector< JoinTrigger* >		JOINTRIGGER_VEC;		///< �׷� ����;
		typedef JOINTRIGGER_VEC::iterator		JOINTRIGGER_VEC_ITOR;	///< �׷� ���ͷ�����;

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

		// Lua ���� Callback;
		float	OnLuaFrameMove ( float fElpaseTime );

		// Join ���� Callback;
		int		OnReadyAnswer ( ReadyReceipt* pReadyReceipt );

		/// �ʿ����;
		// �δ� ��ũ��Ʈ���� ����� ���� �ݹ� - ��ʸ�Ʈ������ ���� ��� ��;
		bool	OnReceiveDataStart ( const InstanceSystem::InstanceMapID _instanceMapID );
		bool	OnReceiveDataEnd ( const InstanceSystem::InstanceMapID _instanceMapID );
		bool	OnReceiveError ( const InstanceSystem::InstanceMapID _instanceMapID,const DWORD _ErrorCode );

		bool	OnReceiveData ( const InstanceSystem::InstanceMapID _instanceMapID, const char* varName, const double _data );
		bool	OnReceiveData ( const InstanceSystem::InstanceMapID _instanceMapID, const char* varName, const char* _data );
		bool	OnReceiveData ( const InstanceSystem::InstanceMapID _instanceMapID, const char* varName, const bool _data );

		bool	OnNoticeResult ( const InstanceSystem::InstanceMapID _instanceMapID, const DWORD _FactionID, const BOOL _IsWinner );

		// Matching Group ���� ������Ʈ;
		void	OnUpdateMatchingGroupInfo ( DWORD ClietDBID );

	public:
		/************************************************************************/
		/* Invoke Function;                                                     */
		/************************************************************************/

		/// Lua �� �������� Invoke;
		int		ScriptInvoke_SendJoin ( lua_State* pState );
		int		ScriptInvoke_WaitTime ( lua_State* L );

		//int		ScriptInvoke_SendToGroupAskJoinAsyc ( lua_State* L );	// ���� ����� �׷���� ���� ���;

		int		ScriptInvoke_MapMultiCreate ( lua_State* pState );

		/// �Ϲ� Invoke;

		/// ���� ����;

		// �׷� ���� Ȯ�� �� ���ν�Ŵ ( ����ȭ ���� ) - ���� ����غ� ���� ���� ����;
		//bool	SetGroupAskJoin ( DWORD GroupID );			// ���� ����� �׷�� ����;
		//int		SendToGroupAskJoinAsyc ( float _fTimer );	// ���� ����� �׷���� ���� ���;

		// ���� �ϱ����� Ȯ���Ѵ�;
		bool	SendToGroupAskJoin ( float fTimer );

		// �׷� Join;
		int		SendJoin ( DWORD _dwGroupID, DWORD _dwInstanceID, Faction::GLActorID _targetActorID, float _fPositionX, float _fPositionZ );

		// Join Lock�� �Ǵ�;
		void	JoinLock ( DWORD _instanceMapID, const bool _bLock );

		// Join Suspend�� �Ǵ� ( ���� ��� );
		void	JoinSuspend ( DWORD _instanceMapID, const bool _bSuspend );

		// ���� �ź� �����ڸ� �����Ѵ�;
		void	SetJoinCanselDrop ( bool _JoinCanselDropCondition );


		/// Utility;

		// ���;
		int		WaitTime ( float _fDuration );


		/// �׷� ����;
		// �κ��� �׷� ��������;
		int		GetGroupToMatching ( );
		int		GetGroupToMatchingNotPlay ( );
		void	SetGroupToPlay ( DWORD GroupID );

		// �׷� �������� -> ���� ��ܿ� �־��ش�;
		void	SetGroupToLobby ( DWORD GroupID );

		// �ƿ� ������ �����ȵ� -> �ٽ� ��� ���·� ��������;
		void	SetGroupToLobbyALL ( );

		// ��Ī �׷쿡 ����� ��ٸ��� �ִ��� �˾ƺ���;
		int		GetMatchingWaitingGroupCount ( );

		// �ð��� ������ Ÿ�Ӿƿ�;
		bool	OutCharInLobby ( DWORD ClientDBnum );

		// ���� �׷��� �μ���. (���԰��� ���̶�� �ٷ� ���� **�Ұ���!!!);
		void	SemiFinishGroup ( DWORD GroupID );

		// �׷��� ���������� �μŹ�����;
		void	FinishGroup ( DWORD GroupID );

		// ��ϵ� ��� �׷��� ���������� �μ� ������ (���԰��� ���̶�� �ٷ� �ٽ� ���԰���);
		void	FinishGroupALL ( );

		// �׷��� �����Ѵ�. ���߿� �ٽ� Ǯ���ֵ��� ������ �Ǿ��ִ�;
		//DWORD	MergeGroup ( DWORD _lGroupID, DWORD _rGroupID);

		// ���� ��� �׷��� �����Ѵ�;
		void	SetFinalWinner ( DWORD GroupID );

		// ���� �����ϱ�;
		void	SendFaction ( DWORD GroupID, DWORD _FactionID, DWORD _instanceID );


		/// ��Ī ����;
		// ��Ī ����;
		bool	StartMatching ( );

		// ���� �غ� �Ǿ����� Ȯ���Ѵ�;
		bool	IsReadyToStart ( );

		// ��Ī ���¸� �Է��Ѵ�. ( ���� ���� �Ұ�.. ���);
		void	SetMatchingStatus ( DWORD _Status );

		// ��Ī �÷��� �ð��� �����Ѵ� ( ���� ��� ���� );
		void	SetMatchingPlayTime ( DWORD _TimeH, DWORD _TimeM );

		// ��Ī�� �ı��Ѵ�;
		bool	DoDestroy ( );

		// ��ũ ���ھ� ���� ( �α� ��� �Լ� );
		void	SetRankScore ( DWORD dbnum, int rating );


		/// ���� ��ʸ�Ʈ������ ���Ǵ� ��Ī ����;
		// ��Ī�� �ش� ���带 �������ش�;
		void	SetMatchingRound ( DWORD _Round );

		// ��Ī ����� �����Ѵ�;
		void	SetMatchingSize ( DWORD _dwMatchingSize );

		// ��Ī �÷��̾���� ��� ���� ���ѹ�����;
		void	SetMatchingPlayersReset ( );

		// ��Ī �÷��̾ �����Ѵ�;
		// ���� ����� Stack ������� �׾Ƽ�;
		// �˾Ƽ� Ŭ���̾�Ʈ�� �����Ͽ� �����ִ¹��;
		// ( ��ũ��Ʈ ���� );
		int		SetMatchingPlayers ( lua_State* pState );


		/// �δ� ����;
		// �� ����;
		//bool	RequestMapCreate ( DWORD _MapID, DWORD _Channel );
		int		MapMultiCreate ( DWORD _dwMapID, DWORD _dwMapSize, lua_State* _pState = NULL );

		// �δ����� ������;
		void	DoOut ( DWORD _dbNum, DWORD _instanceMapID );

	private:
		/************************************************************************/
		/* Inner Function;                                                      */
		/************************************************************************/
		// ���� �� ���ν�Ű�� �׷����ΰ��� Inner Function - ���� ����� ���� ���� ����;
		bool	_SendToClientAskJoin ( DWORD ClientDBnum, float Time = 5, bool isUseCounting = false );
		void	_SetAnswerEnded ();

		// ��Ī �÷��̾� ���� - ��ʸ�Ʈ������ ���;
		// ��ʸ�Ʈ ����� ���� �߰� ���� �ʿ� ( ��ũ��Ʈ ��� ���� �� );
		void	_SetMatchingPlayerTable ( KeyPair player, DWORD num );

		// �׷� Ŭ���̾�Ʈ ���� ����;
		int		_GroupClientStatusUpdate ( DWORD GroupID, EMCUSTOMER_STATUS_TYPE status );

	public:
		/************************************************************************/
		/* Getter;                                                              */
		/************************************************************************/
		// Matching�� Base�� �����´�;
		inline MatchingGameBase*	GetMatchingGameBase() { return m_pMatchingBase; }

		// Matching�� �ı� ���� ���θ� �����´�;
		inline const bool			CanDestroy() { return m_bDestroyMatchingInstance; }

		// Matching Instance�� ID�� �����´�;
		inline DWORD				GetMatchingInstanceID() { return m_dwMatchingInstanceID; }

	public:
		// ��Ī �׷� ����;
		// �ܺο� ����Ǿ��ִ� �����̴�;
		std::vector< GLMatChingGroupInfo >	m_vecMatchingGroupInfo;

	private:
		// Lua Script ����� ����;
		sc::LuaInstance*			m_pScript;
		lua_State*					m_pCoroutineState;

		// �ڷ�ƾ ����;
		// Script ��� ���� ���� ����Ѵ�;
		bool						m_bUseCoroutine;			// �ڷ�ƾ ��� ����;
		float						m_fWaitDuration;			// ��� �ð�;
		bool						m_bSetWait;					// ��� ���� ����;

		// Matching Game�� Base;
		// Base�� �������� �Ļ��� ���� Matching Instance�̴�;
		// ���� Matching�� Instance;
		MatchingGameBase*			m_pMatchingBase;

		// Matching Instance�� ID;
		DWORD						m_dwMatchingBaseID;
		DWORD						m_dwMatchingInstanceID;

		// �� Matching Instance�� Join Ƚ��;
		int							m_nJoinCount;

		// �δ� ��ũ��Ʈ���� ��� ���;
		bool						m_bReceiveDataError;

		// ���� ��� ����;
		bool						m_bJoinCancelDropCondition;

		/// �κ񿡼� ������ �׷� ����Ʈ ( ��ʸ�Ʈ �������� ��� �д� );
		GROUP_VEC					m_vecGettingGroup;

		// Matching Info;
		DWORD						m_dwMatchingRound;
		DWORD						m_dwMatchingSize;

		// Join Fail Trigger -> ���� ���� ��ϵ��� �ʴ´�;
		JOINTRIGGER_VEC				m_JoinFailTrigger;

		// ���� ���� -> ���� Ȯ�� �� ���ν�Ű�� ��Ŀ��� ����ϴ� ����;
		//JOINTABLE_VEC				m_JoinTable;
		//GROUP_VEC					m_vecJoinSaved;

		bool						m_bWaitingAnswer;			// ��ٸ��� ��;
		float						m_fAnswerTimer;				// �ð�;
		DWORD						m_dwAnswerCharacterCount;	// ����� ĳ���� ��;

		// Matching Instance�� �ı� �㰡 ����;
		bool						m_bDestroyMatchingInstance;

		// Matching Mode;
		// ��ũ��Ʈ�� ����ϴ� ��� NULL / Mode ID : 0;
		// ��ũ��Ʈ�� ������� �ʴ� ��� State Pattern���� �ٲ� �� �ִ�;
		DWORD						m_dwMatchingModeID;
		IMatchingMode*				m_pCurMatchingMode;
	};

}