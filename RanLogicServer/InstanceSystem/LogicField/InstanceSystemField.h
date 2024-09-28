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

    // ���� Ʈ����;
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
		DWORD dwFaction; // �÷��̾ ���� ����;
	};

	class IInstanceFieldMode;

	class InstanceField
	{
    private:
        enum
        {
            EM_EVENT_INITIALIZE,                // �δ� ���� ����;

            EM_EVENT_REQUEST_JOIN,              // �÷��̾� ���� ��û;
            EM_EVENT_REQUEST_JOIN_FORCED,       // ��ũ��Ʈ�� ���� �÷��̾� ���� ��û;
            EM_EVENT_REQUEST_JOIN_GM,           // GM ���� ��û;
            EM_EVENT_REQUEST_JOIN_OBSERVER,     // ������ ���� ��û;

            EM_EVENT_REQUEST_MOVE_GATE,         // ����Ʈ �̵� ��û;
            EM_EVENT_OFFLINE,                   // �÷��̾� ��������;
            EM_EVENT_JOIN,                      // �÷��̾� ����;
            EM_EVENT_OUT,                       // �÷��̾� ����;
            EM_EVENT_MAP_ENTER,                 // �÷��̾� �� �̵�;
            EM_EVENT_PARTY_CHANGE_MASTER,       // ��Ƽ�� ����;
            EM_EVENT_PARTY_JOIN,                // ��Ƽ�� �߰�;
            EM_EVENT_PARTY_OUT,                 // ��Ƽ�� Ż��;
            EM_EVENT_TIMER,                     // �δ� Ÿ�̸� �̺�Ʈ �߻�;
            EM_EVENT_UPDATE,                    // �δ� ������Ʈ;
			EM_EVENT_RETRY,						// �絵�� ó��;
			EM_EVENT_COMPLETE_RETRY,			// ��� ������ �絵�� ó��;
			EM_EVENT_ALL_DIE,					// �÷��̾� ��� ���;
            EM_EVENT_DIE,                       // ����(PC + NPC) ���;
            EM_EVENT_RECEIVE_DAMAGE,            // ����(PC + NPC) �ǰ�;
            EM_EVENT_RECEIVE_HEAL,              // ����(PC + NPC) ȸ��;
            EM_EVENT_USE_ITEM,                  // �÷��̾� ������ ���;
            EM_EVENT_RESURRECT,                 // �÷��̾� ��Ȱ;
			EM_EVENT_SKILL,						// �÷��̾� ��ų���;
            EM_EVENT_MOTION_FINISH,             // �÷��̾� ����� �������� (���ĵ�� ����);
            EM_EVENT_ACTOR_TRIGGER_IN,          // ����Ʈ���� �ݰ濡 ���� ����;
            EM_EVENT_ACTOR_TRIGGER_OUT,         // ����Ʈ���� �ݰ濡�� ���� ���;
            EM_EVENT_CLICK_TRIGGER,             // �δ�Ŀ���� NPC�� Ŭ�� ��;
            EM_EVENT_CUSTOM_MESSAGE,            // �δ��� ��������� �޽����� ������;
            EM_EVENT_CUSTOM_MESSAGE_OUTER,      // �δ��ܺο��� �δ��� ����� ���� �޽����� ������;
            EM_EVENT_CUSTOM_MESSAGE_FROM_AGENT, // ������Ʈ �δ���ũ��Ʈ���� �ʵ弭�� �δ� ��ũ��Ʈ�� ����� ���� �޽����� ������;
			EM_EVENT_UPDATE_VICTORIOUS_COUNTRY,	// ������ ���� �ݹ�;

            EM_CALLBACK_FUNCTION_SIZE,
        };

		friend class ManagerField;
	public:
		const bool initiailize(void); // �ʵ� ��ũ��Ʈ ��ü�� ����;
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
		// instanceMapID�� field server������ ������;
		const InstanceMapID instanceMapID;
		const SNATIVEID keyMapID; // �δ� ���� ID;
		const EMINSTANCE_TYPE emInstanceType; // �δ� ���� ����;

        bool bExistFunc[EM_CALLBACK_FUNCTION_SIZE]; // �ݹ� �Լ��� ���� ����( lua�Լ��� ȣ������ ���̱� ���� );

        unsignedState nState; // EMINSTANCE_STATE, ���� �ν��Ͻ��� ���� ����;
        MapIDVector childInstanceIDVector; // �ڽ� �ν��Ͻ����� MapID;
		sc::LuaInstance luaInstance; // ��ũ��Ʈ;
		MemberFieldMap memberMap; // �ν��Ͻ��� ���� �÷��̾�;

		InstanceTimerMap instanceTimerMap;  // ID�� ������ ������ Timer;
		//TimerHandleQueue timerHandleQueue;
        DWORD maxTimer;
		std::vector< InstanceTimer > eventTimerVector;	// �̺�Ʈ�� �߻��� Timer�� �ӽ÷� ��� ����(������ cpp ����);

        InstanceActorTriggerVector instanceActorTriggerVector; // �ֺ��� ���Ͱ� �����ϰų� �������°��� �����ϴ� ����Ʈ���� ����Ʈ;
		InstanceActorTriggerReserveQueue ActorTriggerReserveQueue;  // ����Ʈ���� ���,������� ť;

        float fUpdateTimer;  // eventUpdate ȣ�� ����;

		bool m_bDeleteInstance;		// Instance Field ���� ��� ����;

		// Faction �絵�� ó��;
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
		// ��ü Instance Field �� �߻��Ǵ� �ݹ��� ó���Ѵ�;
		bool CALLBACK EventUpdateVictoriousCountry ( DWORD dwContentID, DWORD dwPreCountryID, DWORD dwCurCountryID );

	private:
		// Client -> Field
		const bool CALLBACK OnMessageProc ( GLMSG::NET_INSTANCE_MESSAGE_HEADER* _pMessage, const DWORD _dwGaeaID );

		// ����/���� �����Ͽ� ����ȭ ������ �߻��� ������ �ִٰ� ������;
		// �޼��� ���� �� SENDTOMYSELF()�� �̿��Ͽ� �ݹ�����;
		// GaeaServer�� �޼��� ó�� �κп����� ȣ���� ���ܷ� ��;	
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

		// ������ �δ��� ���� �Ѵ�, ���� �� �Լ��� ȣ�� �ϱ� ���ٴ� requestDestroy()�� ���ؼ� agent�ʵ� ���� ���� �ǰ� �ؾ���;
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
