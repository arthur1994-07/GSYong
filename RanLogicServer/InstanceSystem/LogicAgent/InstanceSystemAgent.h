#pragma once

#include "../../MatchSystem/MatchingCallbackTriggerManager.h"

#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

#include "../InstanceSystemDefine.h"

namespace InstanceSystem
{
	extern GLAgentServer* gpAgentServer;
	extern volatile long nSimpleUniqueKey;
	__forceinline const unsigned int generateSimpleUniqueKey(void);

	//void doMessageFieldServerOut(GLCharAG* _pPlayer, const SNATIVEID& _baseMapID, const SNATIVEID& _targetMapID, const DWORD _dwTargetGateID, const D3DXVECTOR3& _vTargetPosition, const unsigned int _nTargetFieldServer, const unsigned int _nTargetFieldChannel);	

	class ReloadState
	{			
	public:
		__forceinline const ReloadState& operator =(const ReloadState& _rhs);
		ReloadState(const unsigned int _nFieldServer, const unsigned int _nFieldChannel, const bool _bSuccess);
		~ReloadState(void);

	public:
		const unsigned int nFieldServer;
		const unsigned int nFieldChannel;
		bool bSuccess;
	};

	class MemberAgent  // 에이전트서버 인던 플레이어;
	{
	public:
		const bool frameMove(const float _fElapsedTime, InstanceAgent& _instanceAgent);

		__forceinline void setPosition(const InformationPosition& _informationPosition);
		__forceinline void moveDone(void);
		__forceinline void offline(void);
		__forceinline const DBNUM getDbNum(void) const;
		__forceinline const EMAUTHORITY getAuthority(void) const;
		__forceinline const MapInformation& getPreviousMapInformation(void) const;
		__forceinline MapInformation& getPreviousMapInformation(void);
		__forceinline const bool isState(const InstanceSystem::EMMEMBER_STATE _emState) const;
		__forceinline const bool isValidToDestroy(void) const;

		// 비트플래그로 쓸 필요가 없는 내용이다;
		__forceinline void setState(const InstanceSystem::EMMEMBER_STATE _emState, const bool _bOn = true);		

		__forceinline void setPreviousMapInformation(const MapInformation& _previousMapInformation);

	private:
		void _frameMove_field2instance(const float _fElapsedTime, InstanceAgent& _instanceAgent);
		void _frameMove_instance2instance(const float _fElapsedTime, InstanceAgent& _instanceAgent);
		void _frameMove_instance2field(const float _fElapsedTime, InstanceAgent& _instanceAgent);

	public:
		__forceinline const MemberAgent& operator =(const MemberAgent& _rhs);
		MemberAgent(const DBNUM _dbNum, const EMAUTHORITY _emAuthority);
		MemberAgent(const MemberAgent& _rhs);
		~MemberAgent(void);

	private:
		DBNUM dbNum;
		EMAUTHORITY emAuthority;

		// 비트 플래그로 쓸 필요가 없다;
		unsignedState nState; // EMMEMBER_STATE;

		MapInformation previousMapInformation;
		InformationPositionQueue queueInformationPosition; 
	};


	class InstanceAgent  // 에이전트서버 인던 객체;
	{ 
	public:
		__forceinline const MapInformation* const getPreviousMapPosition(const DBNUM _dbNum);
		__forceinline const bool setPreviousMapPosition(const DBNUM _dbNum, const MapInformation& _previousMapInformation);
		__forceinline const bool setPositionMember(const DBNUM _dbNum, const InformationPosition& _informationPosition);
		__forceinline const bool reserveMember(const DBNUM _dbNum, const InformationPosition& _informationPosition, const EMAUTHORITY _emAuthority);
		__forceinline const bool removeMember(const DBNUM _dbNum, const InformationPosition& _informationPosition);
		__forceinline const bool removeMember(const DBNUM _dbNum);
		__forceinline const bool moveDone(const DBNUM _dbNum);

		__forceinline void setStateMember(const DBNUM _dbNum, const InstanceSystem::EMMEMBER_STATE _emState, const bool _bSet = true);		
		__forceinline void setState(const InstanceSystem::EMINSTANCE_STATE _emState, const bool _bSet = true);
        __forceinline const bool addChildInstance(const SNATIVEID& _baseMapID);
		__forceinline const bool isMember(const DBNUM _dbNum) const;
		__forceinline const bool isState(const DBNUM _dbNum, const InstanceSystem::EMMEMBER_STATE _emState) const;
		__forceinline const bool isState(const InstanceSystem::EMINSTANCE_STATE _emState) const;

		// 삭제가 가능 인던인가? = 인던이 깨끗이 비워져있는가?
		__forceinline const bool isValidToDestroy(const float _fTimeElapsed = 0.0f);

		// 인던 내부에서 맵이 동이 가능한 상태인가?
		__forceinline const bool isValidToMove(void) const;
        
        void frameMove(const float _fTime, const float _fElapsedTime, const CTime& _currentTime);
        void CALLBACK createComplete(void); // 자식 인던을 포함하여 생성 완료 메세지가 온 경우 호출;

		// baseMapID를 가지는 자식 인던의 instanceMapID를 반환;
        __forceinline const SNATIVEID getLandMapID(const SNATIVEID& _baseMapID) const;

		// indexStage에 해당하는 자식 인던의 baseMapID를 반환;
        __forceinline const SNATIVEID getChildMapID(const unsigned int _indexStage) const;

		__forceinline const SNATIVEID& getKeyMapID(void) const;

		__forceinline const InstanceSystem::InstanceMapID getInstanceMapID(void) const;		
		__forceinline const InstanceSystem::EMINSTANCE_TYPE getInstanceType(void) const;
		__forceinline const InstanceSystem::EMAUTHORITY getAuthority(const InstanceSystem::DBNUM _dbNum) const;
        
        __forceinline const unsigned int getNChildInstance(void) const;		
		__forceinline const unsigned int getFieldServer(void) const;
		__forceinline const unsigned int getFieldChannel(void) const;		

		__forceinline const MemberAgentMap& getMemberMap(void) const;
		
		const bool initialize(void);

		void offline(const DBNUM _dbNum);
		void destroy(void);
		void reserveDestroy(void);

		// Lua call by C++Logic ( Callback Event Function );
		const bool requestJoin(const DBNUM _dbNum, const EMAUTHORITY _emAuthority);
		const bool requestEntry(const DBNUM _dbNum, const bool _bRequestByGate);
		void eventCreate(const InstanceMapID _instanceMapID, const DWORD _dwParam1 = 0, const DWORD _dwParam2 = 0, const DWORD _dwParam3 = 0, const DWORD _dwParam4 = 0);
		const InformationPosition eventJoin(const DBNUM _dbNum, const InformationPosition& _informationPosition, const EMAUTHORITY _emAuthority);
		const bool eventPartyJoin(const DBNUM _masterDbNum, const DBNUM _memberDbNum);
		const bool eventPartyOut(const DBNUM _dbNum);
        const bool eventPartyDissolution(const DBNUM _dbNum);
		//void eventClubJoin(const DBNUM _dbNum) const;
		void eventClubOut(const DWORD _clubDbNum, const DWORD _dbNum, const DWORD _ActionActor); // 클럽 번호, 탈퇴자, 탈퇴행위자;
        void CALLBACK eventCustomMessage(const EM_CUSTOM_MESSAGE_TYPE _emSrcType, const DWORD _dwSrcID, const DWORD _Param1, const DWORD _Param2, const DWORD _Param3, const DWORD _Param4);

		// MatchingServer Addon
		void addonTrigger(MatchingSystem::RoomTrigger* _roomTrigger);
		void addonTrigger(MatchingSystem::RoomMultyTrigger* _roomTrigger);


    private:
		void _frameMoveMember(const float _fElapsedTime);

        const bool _clearRequestMember(void); // 정리가 완전히 끝난 경우 true 반환;

	public:
		const bool CALLBACK OnMessageProc ( GLMSG::NET_INSTANCE_MESSAGE_HEADER* _pMessage, const DWORD _dwGaeaID );

	private:
		const bool CALLBACK MsgSetReturnMapInformationFA ( GLMSG::NET_SET_RETURNMAP_INFORMATION_FA* _pMsg, const DWORD _dwGaeaID );

	public:
		InstanceAgent(const InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const EMINSTANCE_TYPE _emInstanceType, const unsigned int _nFieldServer, const unsigned int _nChannel);
		InstanceAgent(const InstanceAgent& _rhs);
		~InstanceAgent(void);

	private:
		// instanceMapID는 agent server내에서 유일함;
        const InstanceMapID instanceMapID;		
        const SNATIVEID keyMapID;
		const EMINSTANCE_TYPE emInstanceType;
		const unsigned int instanceUniqueKey;
		const unsigned int nFieldServer; // field Server ID;
		const unsigned int nFieldChannel; // field channel ID;

		unsignedState nState; // EMINSTANCE_STATE, 현재 인스턴스의 진행 상태;
		sc::LuaInstance luaInstance; // 스크립트;
		MemberAgentMap memberMap; // 인스턴스에 속한 플레이어;		

        MapIDVector childInstanceIDVector; // 자식 Land들의 baseMapID;

        unsigned int countCreatingChildLand;
		unsigned int nMaxPlayer;
		unsigned int nCurrentPlayer;

		float fDestroyTimer;
	};	


    // 현재 인던 스크립트의 구조는 pScriptManager에서 스크립트의 데이터를 관리하고;
    // ManagerAgent에서 생성된 인던을 관리하는 형태라서 생성되지 않은 인던의 스크립트;
    // 즉 PublicAgent급의 스크립트는 따로 관리가 되고 있지 않고 pScriptManager에 있는 스크립트를 활용도록 되어있다.;
    // 현재 작업하기엔 양이 많지만 추후에는 인던의 종류마다 매니저형태로 해당종류의 생성된 인던을 관리하는 방향으로 수정하는 게 좋음;
    class InstanceBaseAgent
    {
    public:
        InstanceBaseAgent(const SNATIVEID& _keyMapID);

        ~InstanceBaseAgent();

    public:
        HRESULT frameMove(const float fTime, const float fElapsedTime, const CTime& CurrentTime);
        const TimerHandle registAbsTimer(const WORD wWeek, const WORD wDay, const WORD wHour, const WORD wMinute, const WORD wSecond,
                                         const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop);
        const TimerHandle registTimer(const float _fTime, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop);

        const bool isRegistTimer(const TimerHandle _hTimer) const;
        const bool removeTimer(const TimerHandle _hTimer);

    private:
        const SNATIVEID keyMapID;

        InstanceTimerMap instanceTimerMap;  // ID로 관리가 가능한 Timer;
        InstanceAbsTimerMap instanceAbsTimerMap;  // ID로 관리가 가능한 Timer;
        TimerHandleQueue timerHandleQueue;
        DWORD maxTimer;
        std::vector< InstanceTimer > eventTimerVector;	// 이벤트가 발생한 Timer를 임시로 담는 벡터(이유는 cpp 참조);
        std::vector< InstanceAbsTimer > eventAbsTimerVector;	// 이벤트가 발생한 Timer를 임시로 담는 벡터(이유는 cpp 참조);
    };

	// messageProcedure에 대한 필드와의 규칙이 다르다;
	// (pLandMan 생성 방식이 다름);
	// 다른 사람이 만든건지 의도적인건지는 모르겠으나,
	// 일정 문제로 기존 방식을 따라감;
	class ManagerAgent  // 에이전트 서버 인던 매니저;
	{
	public:
        void initialize();
		// 현재 매칭 시스템이 없어 임시 구현;		
		HRESULT frameMove(const float fTime, const float fElapsedTime, const CTime& CurrentTime);		
		const bool messageProcedure(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID);		
        const bool messageProcedureMsgPack(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID);		

		const InstanceMapID createInstance(const SNATIVEID& _keyMapID, const DWORD _dwParam1 = 0, const DWORD _dwParam2 = 0, const DWORD _dwParam3 = 0, const DWORD _dwParam4 = 0);
		const bool createInstanceByTrigger(MatchingSystem::RoomTrigger* _trigger);
		const bool createInstanceByTrigger(MatchingSystem::RoomMultyTrigger* _trigger);

		const bool doScript(const DBNUM _dbNum, const SNATIVEID& _keyMapID, const unsigned int _nGateID, const unsigned int _nToGateID);		

		const bool requestEntry(const DBNUM _dbNum, const SNATIVEID& _keyMapID, const unsigned int _nGateID, const unsigned int _nToGateID);		
		const bool requestCreate(const DBNUM _dbNum, const SNATIVEID& _keyMapID, const bool _bRequestByGate, OUT DWORD& _param1, OUT DWORD& _param2, OUT DWORD& _param3, OUT DWORD& _param4);
		const bool requestFaction(const DBNUM _dbNum,const DWORD _dwFaction, const InstanceMapID _instanceMapID, Faction::EMFACTION_TYPE factionstype);

		const bool reserveMember(const DBNUM _dbNum, const InformationPosition& _informationPosition, const EMAUTHORITY _emAuthority = EMAUTHORITY_NORMAL);
        const bool offline(const DBNUM _dbNum);

		const InstanceSystem::EMAUTHORITY getAuthority(const DBNUM _dbNum) const;

		const bool reload(const DWORD _dwGaeaID = GAEAID_NULL);

		//토너먼트에서 사용할 함수들
		const bool DoJoin(const DBNUM _dbNum, const InformationPosition& _informationPosition, const EMAUTHORITY _emAuthority,MatchingSystem::JoinTrigger* _trigger);
		const bool DoOut(const DBNUM _dbNum, const DWORD _instanceMapID );

		const bool joinLock(const InstanceMapID& _instanceMapID, const bool _bLock){return _joinLock(_instanceMapID,_bLock);}
		const bool joinSuspend(const InstanceMapID& _instanceMapID, const bool _bSuspend){return _joinSuspend(_instanceMapID,_bSuspend);}	

        // 구인던에서 사용하는 함수;
        __forceinline const InstanceMapID getInstanceMapID(void) const;
        __forceinline void popInstanceMapID(void);
        __forceinline void pushInstanceMapID(const InstanceMapID& _instanceMapID);

		// Lua call by C++Logic ( Callback Event Function );
		const SNATIVEID requestMapID(const SNATIVEID& _keyMapID); // ret : mapID, fail to -1;
        const TimerHandle registTimer(const SNATIVEID& _keyMapID, const float _fTime, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop);
        const TimerHandle registAbsTimer(const SNATIVEID& _keyMapID, const WORD wWeek, const WORD wDay, const WORD wHour, const WORD wMinute, const WORD wSecond, const DWORD _nParam0, const DWORD _nParam1, const DWORD _nParam2, const DWORD _nParam3, const bool _bLoop);
        const bool isRegistTimer(const SNATIVEID& _keyMapID, const TimerHandle _hTimer);
        const bool removeTimer(const SNATIVEID& _keyMapID, const TimerHandle _hTimer);

	private:
		const bool CALLBACK _messageError(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageReloadFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageGMCommand(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageMoveMap(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageCreateComplete(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageDestroy(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageAnswerJoinFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageSetPositionInstance(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageSetPositionInstanceFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageJoinLockAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageJoinLockFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageJoinSuspendAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageJoinSuspendFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageRequestOut(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageRequestJoin(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageMoveDone(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageJoinParty(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageOutParty(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageClearParty(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
		const bool CALLBACK _messageRequestObserve(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageNewCustomMessage(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID);
        const bool CALLBACK _messageUITimerMsgBox(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID);

	public:
		const bool CALLBACK OnMessageProc ( GLMSG::NET_INSTANCE_MESSAGE_HEADER* _pMessage, const DWORD _dwGaeaID );

    private:
        const bool _joinLock(const InstanceMapID& _instanceMapID, const bool _bLock);
		const bool _joinLock(const SNATIVEID& _keyMapID, const bool _bLock);
		const bool _joinSuspend(const InstanceMapID& _instanceMapID, const bool _bSuspend);		
		const bool CALLBACK _removeMember(const DBNUM _dbNum);

		void _sendMessageToClient(const DBNUM _dbNum, NET_MSG_GENERIC* _pMessage);

	private: // 메칭 관련 콜벡
		void CallTrigger(const SNATIVEID& _MapID,const InstanceMapID& instanceMapID,	const bool bSuccess);
	public:

		
	public:
		const ManagerAgent& operator =(const ManagerAgent&){}
		ManagerAgent(GLAgentServer* const _pAgentServer, const bool* const _bUsed);
		~ManagerAgent(void);

	private:
		InstanceSystem::ScriptManager* const pScriptManager;
		MatchingSystem::MatchingCallbackTriggerManager* m_pMatchingCallBackTriggerManager;

        InstanceBaseAgentMap instanceBaseMap;
		InstanceAgentMap instanceMap;
		InstanceLogMap instanceLogMap;

        MemberInstanceAgentMap memberInstanceMap;

        InstanceMapIDQueue instanceMapIDQueue;		
		
		ReloadStateVector reloadStateVector;
		unsigned int nReloadServerGroup;
        float fUpdateTimer;

		InstanceSystem::PacketCounter packetCounter;
	};	
}



#include "./InstanceSystemAgent.hpp"
