#pragma once

#include "./GLInstanceSystemDefine.h"

namespace InstanceSystem
{ // Instance : 인스턴스 던전의 의미로 사용;
	extern HRESULT LoadFile(const std::string& _fileName, const bool _bPastLoad);
    __forceinline void errorMessage(const std::string& _errorMessage);

	/*
	 * levelScript의 중복 로딩을 피하기 위해 InstanceScript와 ScriptManager는 상호 참조 하고 있음;
	 * 상호 참조가 좋지 않지만 일정 문제도 고려해야 하기 때문에 이와 같이 함;
	 * 
	 * instanceScript :
	 * 인던 종류만큼 생성 되며 ScriptManager에서 관리 로딩 이후 const해야 함;
	 * 인던 입장/종료에 관련된 스크립트를 내부적으로 가지고 있음;	 
	 *
	 * levelScript :
	 * 인던에서 사용하는 실질적인 스크립트로써 서로 다른 인던이 동일한 levelScript를 가질 수 있음;
	 * 그렇기에 중복 로딩을 피하기 위해 instanceScript에서 관리 하는 것이 아닌, scriptManager에서 관리;
	 * instanceScript에서는 levelScript의 key만 가지고 있고,
	 * scriptMasnager로 생성 요청이 오면 instanceScript가 가진 key를 이용하여,
	 * levelScript를 구성 후 넘겨줌;
	*/



	// 인스턴스에서 사용하는 컴포넌트 스크립트마다 한개씩 생성;
	// 파일 구조체 형태를 취하고 있다가 인던 생성 요청이 오면;
	// levelScript들을 재구성하여 돌려줌;
	class LevelScript
	{
		friend class ScriptManager;
	public:		
		const int doFile(sc::LuaInstance& _refDefaultLuaInstance);

	public:
		~LevelScript(void);
		LevelScript(const LevelScript& _rhs);

	private:
		const LevelScript& operator =(const LevelScript& _rhs);
		LevelScript(const std::string& _fileName);		

	private:
		std::string fileName;
		unsigned int nSize;
		char* pBuffer;		
	};



	// 인스턴스 종류마다 한개씩 생성;
	class InstanceScript
	{ // 로딩이 끝난 시점부터 삭제 될때까지,
		// 이 클래스의 값은 변경되어선 안된다;
		friend class ScriptManager;
		friend class InstanceInformationClient;
	public:
		const int doFile(const std::string& _fileName);
		
        __forceinline void setState(const unsigned int _nState, const bool _bOn = true);
        __forceinline const bool isState(const unsigned int _nState) const;

		__forceinline const unsigned int getNLevelAgentScript(void) const;
		__forceinline const unsigned int getNLevelFieldScript(void) const;		
		__forceinline const unsigned int getLevelAgentKey(const unsigned int _nIndex) const;
		__forceinline const unsigned int getLevelFieldKey(const unsigned int _nIndex) const;
		__forceinline const unsigned int getMaxPlayer(void) const;
		__forceinline const unsigned int getMaxCreate(void) const;
		__forceinline const SNATIVEID& getKeyMapID(void) const;
		__forceinline sc::LuaInstance& getScript(void);
        __forceinline const std::string& getIsntanceName(void) const;

	private:
		void _destroy(void);

	public:		
		~InstanceScript(void);

	private:
		InstanceScript(void);

	private:		
		sc::LuaInstance scriptInstance; // InstanceScript에서 사용할 스크립트;		
		
		HashVector levelAgentScriptHashVector; // 어떤 레벨 스크립트를 사용하는지(스트링->해쉬키로 저장);
		HashVector levelFieldScriptHashVector; 
		
		unsigned int nInstanceNameKey;

		std::string instanceName; // 인던 이름(중복 되어선 안됨);
        SNATIVEID keyMapID; // 기본 mapID(다른 인던이랑 중복되어선 안됨, 실제로 사용하지 않을 수 있음);
		unsigned int nMaxPlayer;
		unsigned int nMaxCreate;

        unsignedState nState;
	};

	class InstanceInformationClient
	{
	public:
		InstanceInformationClient(void);
		__forceinline const InstanceInformationClient& operator =(const InstanceScript& _instanceScript);
		__forceinline const InstanceInformationClient& operator =(const InstanceInformationClient& _rhs);
	
		SNATIVEID keyMapID;

        bool bOpen;
        bool bWaiting;

		char instanceName[InstanceSystem::EMINSTANCE_NAME_LENGTH];
	};
	
	class ScriptManager
	{
	public:
		typedef void ( *FUNC_SCRIPT_BASE ) ( sc::LuaInstance& _refScript );


	private:
		friend class InstanceScript;
		friend class LevelScript;


	public:        
		__forceinline void setState(const unsigned int _nState, const bool _bOn = true);
		__forceinline const bool setState(const SNATIVEID& _keyMapID, const unsigned int _nState, const bool _bOn = true);
        __forceinline const bool isState(const SNATIVEID& _keyMapID, const unsigned int _nState) const;
		__forceinline const bool isState(const unsigned int _nState) const;
		__forceinline const bool isExist(const SNATIVEID& _keyMapID) const;

		__forceinline const unsigned int getNInstance(void) const;
		__forceinline sc::LuaInstance* const getScript(const SNATIVEID& _keyMapID);

		__forceinline const unsigned int getMaxPlayer(const SNATIVEID& _keyMapID) const;
		__forceinline const unsigned int getMaxCreate(const SNATIVEID& _keyMapID) const;
		__forceinline const int getInstanceInformation(const SNATIVEID& _keyMapID, InstanceInformationClient& _instanceInformation) const;
		__forceinline const std::string* getInstanceName(const SNATIVEID& _keyMapID) const;
		__forceinline const int getLevelAgentScript(const InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, sc::LuaInstance& _scriptReference) const;
        __forceinline const int getLevelFieldScript(const InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, sc::LuaInstance& _scriptReference) const;
		__forceinline const MapIDVector& getKeyMapIDVector(void) const;


		__forceinline void registFunction(const char* const _name, lua_CFunction _function);
        __forceinline void pushCallFunction(const char* const _name);

		__forceinline void registFunctionCommon ( FUNC_SCRIPT_BASE _function );


		const int doFile(const std::string& _fileName);
        const int reload(void);

	private:
		const int _pushScriptLevel(const std::string& _fileName, const unsigned int _nHashKey, sc::LuaInstance& _refDefaultLuaInstance);
		void _reset(void);

	public:
		static ScriptManager* const getInstance(void);
		~ScriptManager(void);

	private:
		ScriptManager(void);

	private:
        std::string fileName; // 모든 경로 포함 된 파일 이름;
		InstanceScriptMap instanceScriptMap;
		LevelScriptMap levelScriptMap;
		MapIDVector keyMapIDVector;
		unsignedState nState;
	};	

    class InstanceIDSet
    {
    public:
        const InstanceIDSet& operator =(const InstanceIDSet&){}
        InstanceIDSet(const SNATIVEID& _keyMapID, const InstanceMapID& _instanceMapID, const unsigned int _indexStage);
		InstanceIDSet(const SNATIVEID& _keyMapID, const SNATIVEID& _gaeaMapID);
        InstanceIDSet(const InstanceIDSet& _rhs);

        const SNATIVEID keyMapID;
        const InstanceMapID instanceMapID;
        unsigned int nStage;
    };	

    class MapInformation
    {
    public:
        const MapInformation& operator =(const MapInformation& _rhs);
        MapInformation(void);
        MapInformation(const SNATIVEID& _mapID, const D3DXVECTOR3& _vPosition, const Faction::GLActorID& _targetActorID);
        MapInformation(const MapInformation& _rhs);		

        SNATIVEID targetMapID;
        D3DXVECTOR3 targetPosition;
        const Faction::GLActorID targetActorID; // 주인의 정보가 아닌 돌아갈 곳에 대한 정보;
    };

    class InformationPosition
    { // 인던 내 다른 지역으로 이동시 pLand를 생성 해야 하는 경우,
        // 생성 하는 동안 대기 상태로 전환하기 위해 필요한 정보;
    public:
        const InformationPosition& operator =(const InformationPosition& _rhs);
        InformationPosition(
			const Faction::GLActorID& _actorID,
			const SNATIVEID& _targetMapID, const Faction::GLActorID& _targetActorID,
            const float _fPositionX, const float _fPositionZ, 
			const InstanceSystem::EMMOVE_TYPE _emMoveType);
        InformationPosition(const InformationPosition& _rhs);        
        InformationPosition(void);

    public:
		const InstanceSystem::EMMOVE_TYPE emMoveType;
		const Faction::GLActorID actorID;
		const Faction::GLActorID targetActorID; // 이동할 곳의 객체 ID;
		const SNATIVEID targetMapID;        

		const float fPositionX;
        const float fPositionZ;

		// 부활때문에 예외처리로 생긴 부분 !반드시 부활,맵이동 관련 개편을 통해 하루빨리 이 코드를 없애도록 하자!;
		// 후에 삭제하기 편하게 하기위해 생성자 및 const 처리를 하지 않음;
		bool bResurrect;
    };

    class InstanceLog
    {
    public:
        __forceinline const InstanceLog& operator =(const InstanceLog& _rhs);
        InstanceLog(void);
        InstanceLog(const SNATIVEID& _keyMapID, const unsigned int _nUsedCount, const unsigned int _nRunningCount, const unsigned int _nMaxCreate);

        SNATIVEID keyMapID;
        unsigned int nUsedCount;
        unsigned int nRunningCount;
		unsigned int nMaxCreate;
    };

    class InstanceList
    {
    public:
        __forceinline const InstanceList& operator =(const InstanceList& _rhs);
        InstanceList(void);
        InstanceList(const SNATIVEID& _keyMapID, const InstanceMapID _instanceMapID);

        SNATIVEID keyMapID;
        InstanceMapID instanceMapID;
    };

    class InstanceTimer
    {
    public:
        TimerHandle			hTimerHandle;

        bool                bLoop;

        const __time64_t    tStartTime;  // 생성된 시간;
        float               fLifeTime;  // 타이머 설정 시간;
        float               fElapsedTime;  // 경과 시간;
        bool                bPause;

        DWORD               m_dwParam0;
        DWORD               m_dwParam1;
        DWORD               m_dwParam2;
        DWORD               m_dwParam3;

    public:
        __forceinline const InstanceTimer& operator =(const InstanceTimer& _rhs);

        InstanceTimer(TimerHandle _hTimerHandle, const float _fLifeTime, const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop);
        ~InstanceTimer(void);

        __forceinline const bool updateTimer(const float _fElapsedTime);
        __forceinline const bool isPause(void) const;
        __forceinline void suspend(void);
        __forceinline void resume(void);
    };

    class InstanceAbsTimer
    {
    public:
        TimerHandle			hTimerHandle;

        bool                bLoop;
        bool                bUsed;

        WORD                wWeek;
        WORD                wDay;

        WORD                wHour;
        WORD                wMinute;
        WORD                wSecond;

        float               m_dwParam0;
        float               m_dwParam1;
        float               m_dwParam2;
        float               m_dwParam3;

    public:
        __forceinline const InstanceAbsTimer& operator =(const InstanceAbsTimer& _rhs);

        InstanceAbsTimer(TimerHandle _hTimerHandle, WORD _wWeek, WORD _wDay, WORD _wHour, WORD _wMinute, WORD _wSecond,
                         const DWORD _dwParam0, const DWORD _dwParam1, const DWORD _dwParam2, const DWORD _dwParam3, const bool _bLoop);
        ~InstanceAbsTimer(void);

        __forceinline const bool checkTimer(const CTime& _cCurrentTime);
    };

    struct InstanceXmlString
    {
        enum EMXML_TYPE
        {
            EMXMLGAME_IN_TEXT,
            EMXMLGAME_WORD,
            EMXMLGAME_EX_TEXT,
        };
        std::string strXmlKey;
        DWORD       emXmlType;
        DWORD       index;
        std::vector<std::string> XmlParamVector;

        MSGPACK_DEFINE(strXmlKey, emXmlType, index, XmlParamVector);
    };

	struct SystemMail
	{
		DWORD			CharID;
		std::string		szName;
		bool			IsUseItem;
		SNATIVEID		NewItem;
		LONGLONG		AttachedMoney;
		std::string		Sender;
		std::string		PostTitle;
		std::string		PostContent;
		DWORD			ItemNum;
		DWORD			dwContentTextIndex;
		MSGPACK_DEFINE(CharID, szName,IsUseItem,NewItem, AttachedMoney, Sender,PostTitle,PostContent,ItemNum,dwContentTextIndex);
	};





	class PacketCounter;
	class PacketCounterElement
	{
	public:	
		__forceinline const unsigned int increase(void);
		__forceinline void clear(void);

	public:
		__forceinline const InstanceSystem::PacketCounterElement& operator =(const InstanceSystem::PacketCounterElement& _rhs);
	
		PacketCounterElement(const unsigned int _indexPacket);

	private:
		friend class InstanceSystem::PacketCounter;
		const unsigned int indexPacket;
		unsigned int countPacket;
		unsigned int countPacketPrevious;
	};
	typedef std::vector<InstanceSystem::PacketCounterElement> PacketCounterElementVector;

	class PacketCounter
	{
	public:		
		const unsigned int increase(const unsigned int _indexPacket);
		__forceinline const unsigned int size(void) const;

	private:
		void _clear(void);

	public:		
		__forceinline const InstanceSystem::PacketCounter& operator =(const InstanceSystem::PacketCounter& _rhs);
		__forceinline const unsigned int operator [](const unsigned int _indexPacket) const;

		PacketCounter(const unsigned int _sizePacket, const unsigned int _timerReset = 1000);

	private:
		const unsigned int timerReset; // 초기화 갱신 주기(1000분의 1초);
		unsigned int sizeMessageNotify; // 알림 수량;
		unsigned int countNotify; // 알림 횟수;
		unsigned int timeCumulative;
		unsigned int timeGap;
		InstanceSystem::PacketCounterElementVector vectorPacketCounterElement;
	};

	// Faction 이 모두 죽었을 경우 재도전 처리를 위함;
	struct SRETRY_INFO
	{
		enum EMRETRY_RESULT
		{
			EMRETRY_RESULT_OK,
			EMRETRY_RESULT_FAIL,
			EMRETRY_RESULT_TIMEOVER,
			EMRETRY_RESULT_INVALID_MONEY,
		};

		Faction::FactionID sFactionID;		// Factio ID;
		DWORD dwMoney;						// 재도전하기위해 필요한 돈;
		DWORD dwCountOK;					// OK 누른 숫자;
		float fTime;						// 흘러간 시간;
		float fRetryTime;					// 재도전 시간;
		bool bRetry;						// 재도전 상태;
		bool bSuccess;						// 재도전 성공 여부;

		EMRETRY_RESULT emResult;			// 재도전 결과;

		SRETRY_INFO()
			: dwCountOK ( 0 )
			, dwMoney ( 0 )
			, fTime ( 0.f )
			, fRetryTime ( 10.f )
			, bRetry ( false )
			, bSuccess ( false )
			, emResult ( EMRETRY_RESULT_OK )
		{

		}
	};
};

#include "./GLInstanceSystem.hpp"
