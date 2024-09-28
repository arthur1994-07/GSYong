#pragma once

#include "../InstanceSystemDefine.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

class ServerActor;

namespace InstanceSystem
{
	extern GLGaeaServer* gpGaeaServer;

    enum EM_TRIGGER_TYPE
    {
        EM_TRIGGER_PC           = 0x0001,
        EM_TRIGGER_MOB          = 0x0002,
        EM_TRIGGER_SUMMON       = 0x0004,

        EM_TRIGGER_NON_PC       = EM_TRIGGER_MOB + EM_TRIGGER_SUMMON,
        EM_TRIGGER_NON_MOB      = EM_TRIGGER_PC + EM_TRIGGER_SUMMON,
        EM_TRIGGER_NON_SUMMON   = EM_TRIGGER_PC + EM_TRIGGER_MOB,

        EM_TRIGGER_ALL          = EM_TRIGGER_PC + EM_TRIGGER_MOB + EM_TRIGGER_SUMMON,
    };

    typedef std::vector<Faction::GLActorID> ACTOR_VECTOR;
    typedef ACTOR_VECTOR::iterator          ACTOR_ITER;
    typedef ACTOR_VECTOR::const_iterator    ACTOR_CITER;

    // 액터 트리거;
    class InstanceActorTrigger
    {
        enum
        {
            EM_TRIGGER_SPHERE,
            EM_TRIGGER_CUBE,
        };

        enum
        {
            EM_TRIGGER_EVENT_PC     = 0x0001,
            EM_TRIGGER_EVENT_MOB    = 0x0002,
            EM_TRIGGER_EVENT_NPC    = 0x0004,
            EM_TRIGGER_EVENT_SUMMON = 0x0008,
        };

    public:
        InstanceMapID instanceMapID;
        Faction::GLActorID actorID;
		DWORD   triggerType;
        DWORD   rangeType;
        float   fRadiusSqrt;
        float   fZoneX;
        float   fZoneZ;
        float   fZoneY;

        typedef std::set<Faction::GLActorID>        INNER_ACTOR_SET;
        typedef INNER_ACTOR_SET::iterator           INNER_ACTOR_ITER;
        typedef INNER_ACTOR_SET::const_iterator     INNER_ACTOR_CITER;

		INNER_ACTOR_SET innerActorSet;

        InstanceActorTrigger(const InstanceMapID& _instanceMapID, const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fRadius);
        InstanceActorTrigger(const InstanceMapID& _instanceMapID, const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fZoneX, const float _fZoneZ, const float _fZoneY);

        const bool updateAround(OUT ACTOR_VECTOR& _inActor, OUT ACTOR_VECTOR& _outActor);

    private:
        const bool isInside(const ServerActor* _triggerActor, const ServerActor* _inActor);

    public:
        InstanceActorTrigger& operator= (const InstanceActorTrigger& _rhs)
        {
			instanceMapID = _rhs.instanceMapID;
			actorID = _rhs.actorID;
			triggerType = _rhs.triggerType;
			rangeType = _rhs.rangeType;
			fRadiusSqrt = _rhs.fRadiusSqrt;
			fZoneX = _rhs.fZoneX;
			fZoneZ = _rhs.fZoneZ;
			fZoneY = _rhs.fZoneY;
			innerActorSet = _rhs.innerActorSet;
            return (*this);
        };
    };

	struct InstanceActorTriggerReserveInfo
	{
		enum
		{
			EM_RESERVE_REGIST_NOMAL,
			EM_RESERVE_REGIST_ZONE,
			EM_RESERVE_UNREGIST,
		};

		DWORD emReserveAction;
		
		Faction::GLActorID _actorID;
		DWORD _triggerType;
		float _fRadius;
		float _fZoneX;
		float _fZoneY;
		float _fZoneZ;
	};	

    typedef std::list<InstanceActorTrigger> InstanceActorTriggerVector;
    typedef InstanceActorTriggerVector::const_iterator InstanceActorTriggerVectorCIter;
    typedef InstanceActorTriggerVector::iterator InstanceActorTriggerVectorIter;

	typedef std::queue<InstanceActorTriggerReserveInfo> InstanceActorTriggerReserveQueue;

	class MemberField
	{
	public:
		void frameMove(const float _fTime, const float _fElapsedTime);

		__forceinline const bool isState(const InstanceSystem::EMMEMBER_STATE _emState) const;
		__forceinline void setState(const InstanceSystem::EMMEMBER_STATE _emState, const bool _bOn = true);	

	public:
		MemberField(const DBNUM _dbNum);
		MemberField(const MemberField& _rhs);
		~MemberField(void);

	private:
		const DBNUM dbNum;
		unsignedState nState; // EMMEMBER_STATE;
		DWORD dwFaction; // 플레이어가 속한 진영;
	};

	class IInstanceFieldMode;

	class InstanceField
	{
    private:
        enum
        {
            EM_EVENT_INITIALIZE,                // 인던 최초 생성;

            EM_EVENT_REQUEST_JOIN,              // 플레이어 입장 요청;
            EM_EVENT_REQUEST_JOIN_FORCED,       // 스크립트에 의한 플레이어 입장 요청;
            EM_EVENT_REQUEST_JOIN_GM,           // GM 입장 요청;
            EM_EVENT_REQUEST_JOIN_OBSERVER,     // 관전자 입장 요청;

            EM_EVENT_REQUEST_MOVE_GATE,         // 게이트 이동 요청;
            EM_EVENT_OFFLINE,                   // 플레이어 접속종료;
            EM_EVENT_JOIN,                      // 플레이어 입장;
            EM_EVENT_OUT,                       // 플레이어 퇴장;
            EM_EVENT_MAP_ENTER,                 // 플레이어 맵 이동;
            EM_EVENT_PARTY_CHANGE_MASTER,       // 파티장 변경;
            EM_EVENT_PARTY_JOIN,                // 파티원 추가;
            EM_EVENT_PARTY_OUT,                 // 파티원 탈퇴;
            EM_EVENT_TIMER,                     // 인던 타이머 이벤트 발생;
            EM_EVENT_UPDATE,                    // 인던 업데이트;
			EM_EVENT_RETRY,						// 재도전 처리;
			EM_EVENT_COMPLETE_RETRY,			// 모든 유저의 재도전 처리;
			EM_EVENT_ALL_DIE,					// 플레이어 모두 사망;
            EM_EVENT_DIE,                       // 액터(PC + NPC) 사망;
            EM_EVENT_RECEIVE_DAMAGE,            // 액터(PC + NPC) 피격;
            EM_EVENT_RECEIVE_HEAL,              // 액터(PC + NPC) 회복;
            EM_EVENT_USE_ITEM,                  // 플레이어 아이템 사용;
            EM_EVENT_RESURRECT,                 // 플레이어 부활;
			EM_EVENT_SKILL,						// 플레이어 스킬사용;
            EM_EVENT_MOTION_FINISH,             // 플레이어 모션이 끝났을때 (모션캔슬 포함);
            EM_EVENT_ACTOR_TRIGGER_IN,          // 액터트리거 반경에 액터 들어옴;
            EM_EVENT_ACTOR_TRIGGER_OUT,         // 액터트리거 반경에서 액터 벗어남;
            EM_EVENT_CLICK_TRIGGER,             // 인던커스텀 NPC를 클릭 시;
            EM_EVENT_CUSTOM_MESSAGE,            // 인던에 사용자정의 메시지를 보낼때;
            EM_EVENT_CUSTOM_MESSAGE_OUTER,      // 인던외부에서 인던에 사용자 정의 메시지를 보낼때;
            EM_EVENT_CUSTOM_MESSAGE_FROM_AGENT, // 에이전트 인던스크립트에서 필드서버 인던 스크립트로 사용자 정의 메시지를 보낼때;
			EM_EVENT_UPDATE_VICTORIOUS_COUNTRY,	// 승전국 변경 콜백;

            EM_CALLBACK_FUNCTION_SIZE,
        };

		friend class ManagerField;
	public:
		const bool initiailize(void); // 필드 스크립트 객체를 생성;
		void frameMove(const float _fTime, const float _fElapsedTime);		

		GLLandMan* GetLand();

		__forceinline const InstanceMapID getInstanceMapID(void) const;
		__forceinline const SNATIVEID& getKeyMapID(void) const;
        __forceinline const SNATIVEID getGaeaMapID(const SNATIVEID& _baseMapID) const;
        __forceinline const SNATIVEID getBaseMapID(const SNATIVEID& _landMapID) const;

        __forceinline const bool isState(const DBNUM _dbNum, const InstanceSystem::EMMEMBER_STATE _emState) const;		
        __forceinline const bool isState(const InstanceSystem::EMINSTANCE_STATE _emState) const;
		__forceinline void setState(const DBNUM _dbNum, const InstanceSystem::EMMEMBER_STATE _emState, const bool _bSet = true);
        __forceinline void setState(const InstanceSystem::EMINSTANCE_STATE _emState, const bool _bSet = true);

        __forceinline void addChildInstance(const unsigned int _index, const SNATIVEID& _baseMapID);
		__forceinline void addMember(const DBNUM _dbNum);
        __forceinline void removeMember(const DBNUM _dbNum);

		__forceinline const bool reserveRegistActorTrigger(const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fRadius);
		__forceinline const bool reserveRegistActorTrigger(const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fZoneX, const float _fZoneZ, const float _fZoneY);
		__forceinline const bool reserveRemoveActorTrigger(const Faction::GLActorID& _actorID);

        __forceinline const TimerHandle registTimer(const float _nTimeSec, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop);
		__forceinline const int isRegistTimer(const TimerHandle _hTimer) const;
		__forceinline const int getLeftTime(const TimerHandle _hTimer) const;
		__forceinline const int suspendTimer(const TimerHandle _hTimer);
		__forceinline const int resumeTimer(const TimerHandle _hTimer);
		__forceinline const int removeTimer(const TimerHandle _hTimer);
		__forceinline const bool removeAllTimer(void);

		__forceinline const bool IsReserveDestroy ( void ) const { return m_bDeleteInstance; }

		void destroyLua(void);

	public:
		InstanceField(const InstanceMapID _instanceMapID, const SNATIVEID& _keyMapID, const EMINSTANCE_TYPE _emInstanceType);
		InstanceField(const InstanceField& _rhs);
		~InstanceField(void);  

	private:
		void ReserveDestroy ();
		void CancelReserveDestroy ();

		

		void updateTimer(const float _fElapsedTime);

		__forceinline const bool registActorTrigger(const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fRadius);
		__forceinline const bool registActorTrigger(const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fZoneX, const float _fZoneZ, const float _fZoneY);
		__forceinline const bool removeActorTrigger(const Faction::GLActorID& _actorID);

	public:
		/// Invoke Function ( lua -> c++ );
		void RetryFaction ( SRETRY_INFO sRetryInfo );
		void ShowTimeUI ( const DWORD _dwChaNum, const float _fSec, const bool bShow );
		void ShowEntranceStateUI ( const Faction::FactionID _sFactionID, const DWORD _dwCurCount, const DWORD _dwMaxCount, const float _fSec, const bool _bShow );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////        c++ -> lua       CallBack Event Funtion                ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        const bool CALLBACK EventInitialize();
        const bool CALLBACK EventUpdate(const float _fElapsedTime);
        void CALLBACK EventTimer(const DWORD _timerHandle, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const DWORD _dwParam4);
        void CALLBACK EventActorTriggerIn(const InstanceActorTrigger& _actorTrigger, const ACTOR_VECTOR& _innerActorVector);
        void CALLBACK EventActorTriggerOut(const InstanceActorTrigger& _actorTrigger, const ACTOR_VECTOR& _OuterActorVector);
	public:
        const bool CALLBACK EventRequestJoin(const DBNUM _dbNum, const EMAUTHORITY _emAuthority);
        const bool CALLBACK EventRequestMoveGate(const DBNUM _dbNum, const SNATIVEID& _mapID, const SNATIVEID& _mapTarget, const DWORD _gateID, const DWORD _gateIDTarget);
        void CALLBACK EventOnline(const DBNUM _dbNum);
        void CALLBACK EventOffline(const DBNUM _dbNum, const bool _bReserveMember = false);
        const bool CALLBACK EventJoin(const DBNUM _dbNum, GLLandMan* _pLand, IN OUT D3DXVECTOR3& _vAdjustPos);
        void CALLBACK EventOut(const DBNUM _dbNum);
        void CALLBACK EventMapEnter(const DBNUM _dbNum, const SNATIVEID& _baseMapID);
		void CALLBACK EventPartyChangeMaster(const GLPartyID& _partyID, const bool _bExpedition, const DBNUM _newMasterDbNum, const bool _bInSameInstanceNewMaster, const DBNUM _oldMasterDbNum, const bool _bInSameInstanceOldMaster);
        void CALLBACK EventPartyJoin(const GLPartyID& _partyID, const bool _bExpedition, const DBNUM _joinerDBNum, const bool _bInSameInstance);
        void CALLBACK EventPartyOut(const GLPartyID& _partyID, const bool _bExpedition, const DBNUM _outerDBNum, const bool _bInSameInstance);
        void CALLBACK EventClickTrigger(const DWORD _emNpcType, const DWORD _dwNpcID, const DWORD _emClickActorType, const DWORD _dwClickActorID);
		void CALLBACK EventRetryProc(const DWORD _dwChaNum, bool _bAccept, const DWORD _dwResultFlag );
		const bool CALLBACK EventCompleteRetryProc(const DWORD _emFactionType, const DWORD _dwFactionID, bool _bSuccess, const DWORD _dwResultFlag );
		const bool CALLBACK EventAllDie(const DWORD _emFactionType, const DWORD _dwFactionID);
		const bool CALLBACK EventDie(const DWORD _emDie_ActorType, const DWORD _dwDie_ActorID, const DWORD _emKill_ActorType, const DWORD _dwKill_ActorID);
		const bool CALLBACK EventReceiveDamage(const DWORD _emDamaged_ActorType, const DWORD _dwDamaged_ActorID, const DWORD _emAttack_ActorType, const DWORD _dwAttack_ActorID, DWORD& _dwDamage, const DWORD _dwDamageFlag);
		const bool CALLBACK EventReceiveHeal(const DWORD _emActorType, const DWORD _dwActorID, const DWORD _emReActorType, const DWORD _dwReActorID, DWORD& _dwHeal, const DWORD _dwHealFlag);
		const bool CALLBACK EventUseItem(const DWORD _dwActorID, const DWORD _itemMID, const DWORD _itemSID, const DWORD _nParam = 0, const float _fParam = 0.0f);
        const bool CALLBACK EventResurrect(const DWORD _emResurrectType, const DWORD _emDie_ActorType, const DWORD _dwDie_ActorID, const DWORD _emKill_ActorType, const DWORD _dwKill_ActorID);
        const bool CALLBACK EventSkill(const DWORD _dwActorType, const DWORD _dwActorID);
        void CALLBACK EventMotionFinish(const DWORD _dwActorType, const DWORD _dwActorID, const DWORD _dwMotionMID, const DWORD _dwMotionSID, const float _fRemainSec);
        void CALLBACK EventCustomMessage(const DBNUM _dbNum, const DWORD _Param1, const DWORD _Param2, const DWORD _Param3, const DWORD _Param4);
        void CALLBACK EventCustomMessageOuter(const DBNUM _dbNum, const DWORD _Param1, const DWORD _Param2, const DWORD _Param3, const DWORD _Param4);
        void CALLBACK EventCustomMessageFromAgent(const DWORD _Param1, const DWORD _Param2, const DWORD _Param3, const DWORD _Param4);        
        void CALLBACK EventCustomMessage(const EM_CUSTOM_MESSAGE_TYPE _emSrcType, const DWORD _dwSrcID, const DWORD _Param1, const DWORD _Param2, const DWORD _Param3, const DWORD _Param4);

	public:
		bool CALLBACK EventUpdateVictoriousCountry ( DWORD dwContentID, DWORD dwPreCountryID, DWORD dwCurCountryID );
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	public:
		// Client -> Field;
		const bool CALLBACK OnMessageProc ( GLMSG::NET_INSTANCE_MESSAGE_HEADER* _pMessage, const DWORD _dwGaeaID );

	private:
		const bool CALLBACK MsgRetryFactionCF ( GLMSG::NET_RETRY_FACTION_CF* _pMsg, const DWORD _dwGaeaID );

    private:
        void CheckFunctionExist();
		void CheckInstanceMode ();

	private:        
		// instanceMapID는 field server내에서 유일함;
		const InstanceMapID instanceMapID;
		const SNATIVEID keyMapID; // 인던 종류 ID;
		const EMINSTANCE_TYPE emInstanceType; // 인던 생성 종류;

        bool bExistFunc[EM_CALLBACK_FUNCTION_SIZE]; // 콜백 함수의 존재 여부( lua함수의 호출비용을 줄이기 위함 );

        unsignedState nState; // EMINSTANCE_STATE, 현재 인스턴스의 진행 상태;
        MapIDVector childInstanceIDVector; // 자식 인스턴스들의 MapID;
		sc::LuaInstance luaInstance; // 스크립트;
		MemberFieldMap memberMap; // 인스턴스에 속한 플레이어;

		InstanceTimerMap instanceTimerMap;  // ID로 관리가 가능한 Timer;
		//TimerHandleQueue timerHandleQueue;
        DWORD maxTimer;
		std::vector< InstanceTimer > eventTimerVector;	// 이벤트가 발생한 Timer를 임시로 담는 벡터(이유는 cpp 참조);

        InstanceActorTriggerVector instanceActorTriggerVector; // 주변에 액터가 접근하거나 떠나가는것을 감지하는 액터트리거 리스트;
		InstanceActorTriggerReserveQueue ActorTriggerReserveQueue;  // 액터트리거 등록,등록해제 큐;

        float fUpdateTimer;  // eventUpdate 호출 간격;

		bool m_bDeleteInstance;		// Instance Field 삭제 대기 상태;

		// Faction 재도전 처리;
		std::vector< SRETRY_INFO > m_vecFactionRetry;

		// Instance Field Mode;
		IInstanceFieldMode* m_pMode;
	};




	class ManagerField
	{
		friend class GLGaeaServer;
	public:
        const InstanceSystem::ScriptManager* const getScriptMan() const;
		InstanceField* CALLBACK createInstance(const InstanceMapID _instanceMapID, const SNATIVEID& _keyMapID, const EMINSTANCE_TYPE _emInstanceType, bool& _bAlreadyMake);        

        const bool offline(const DBNUM _dbNum);

		const bool requestJoin(const DBNUM _dbNum, const SNATIVEID& _baseMapID, const DWORD _dwGateID) const;
        const bool requestMoveToPosition(const InstanceMapID _instanceMapID, const InformationPosition& _informationSetPosition) const;
		const bool requestDestroy(const InstanceMapID _instanceMapID);

        const bool reserveRegistActorTrigger(const InstanceMapID _instanceMapID, const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fRadius);
        const bool reserveRegistActorTrigger(const InstanceMapID _instanceMapID, const Faction::GLActorID& _actorID, const DWORD _triggerType, const float _fZoneX, const float _fZoneZ, const float _fZoneY );
        const bool reserveRemoveActorTrigger(const InstanceMapID _instanceMapID, const Faction::GLActorID& _actorID);

		const TimerHandle registTimer(const InstanceMapID _instanceMapID, const float _nTimeSec, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop);
		const int removeTimer(const InstanceMapID _instanceMapID, const TimerHandle _hTimer);
		const int suspendTimer(const InstanceMapID _instanceMapID, const TimerHandle _hTimer);
		const int resumeTimer(const InstanceMapID _instanceMapID, const TimerHandle _hTimer);
		const int isRegistTimer(const InstanceMapID _instanceMapID, const TimerHandle _hTimer);
		const int getLeftTime(const InstanceMapID _instanceMapID, const TimerHandle _hTimer);

	private:
		void       CALLBACK frameMove(const float _fTime, const float _fElapsedTime);		

	private:
		// 전체 Instance Field 에 발생되는 콜백을 처리한다;
		bool CALLBACK EventUpdateVictoriousCountry ( DWORD dwContentID, DWORD dwPreCountryID, DWORD dwCurCountryID );

	private:
		// Client -> Field
		const bool CALLBACK OnMessageProc ( GLMSG::NET_INSTANCE_MESSAGE_HEADER* _pMessage, const DWORD _dwGaeaID );

		// 접속/종료 관련하여 동기화 문제가 발생할 여지가 있다고 가정함;
		// 메세지 구성 후 SENDTOMYSELF()를 이용하여 콜백으로;
		// GaeaServer의 메세지 처리 부분에서의 호출은 예외로 함;	
		const bool CALLBACK _messageProcedure(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageMoveMap(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageCreate(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageDestroy(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageBegin(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageJoin(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);	
		const bool CALLBACK _messageRequestJoinAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageReuqestOut(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageSetPosition(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageDoMoveTo(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageJoinLockAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageJoinLockFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageJoinSuspendAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageJoinSuspendFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageGMCommand(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageReloadAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageFactionsSet(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageClickTrigger(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageNewCustomMessage(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);

		// 실제로 인던을 제거 한다, 직접 이 함수를 호출 하기 보다는 requestDestroy()를 통해서 agent쪽도 같이 제거 되게 해야함;
		const bool CALLBACK _destroy(const InstanceMapID& _instanceMapID);

    private:
		const bool _notifyInstanceInformation(void) const;
        const bool _joinLock(const InstanceMapID _instanceMapID, const bool _bLock);
		const bool _joinLock(const SNATIVEID& _keyMapID, const bool _bLock);
		const bool _joinSuspend(const InstanceMapID _instanceMapID, const bool _bSuspend);
        const bool _addMember(const DBNUM _dbNum, const SNATIVEID& _mapID);
        const bool _removeMember(const DBNUM _dbNum);
		const bool _reload(void);

	public:
		const ManagerField& operator =(const ManagerField&){}
		ManagerField(GLGaeaServer* const _pGaeaServer, const unsigned int _nServerChannel);
		~ManagerField(void);

	private:
		InstanceSystem::ScriptManager* const pScriptManager;
		const unsigned int nServerChannel;

		InstanceFieldMap instanceMap;
		MemberInstanceFieldMap memberInstanceMap;
	};	
}

#include "InstanceSystemField.hpp"
