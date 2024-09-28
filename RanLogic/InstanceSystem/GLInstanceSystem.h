#pragma once

#include "./GLInstanceSystemDefine.h"

namespace InstanceSystem
{ // Instance : �ν��Ͻ� ������ �ǹ̷� ���;
	extern HRESULT LoadFile(const std::string& _fileName, const bool _bPastLoad);
    __forceinline void errorMessage(const std::string& _errorMessage);

	/*
	 * levelScript�� �ߺ� �ε��� ���ϱ� ���� InstanceScript�� ScriptManager�� ��ȣ ���� �ϰ� ����;
	 * ��ȣ ������ ���� ������ ���� ������ ����ؾ� �ϱ� ������ �̿� ���� ��;
	 * 
	 * instanceScript :
	 * �δ� ������ŭ ���� �Ǹ� ScriptManager���� ���� �ε� ���� const�ؾ� ��;
	 * �δ� ����/���ῡ ���õ� ��ũ��Ʈ�� ���������� ������ ����;	 
	 *
	 * levelScript :
	 * �δ����� ����ϴ� �������� ��ũ��Ʈ�ν� ���� �ٸ� �δ��� ������ levelScript�� ���� �� ����;
	 * �׷��⿡ �ߺ� �ε��� ���ϱ� ���� instanceScript���� ���� �ϴ� ���� �ƴ�, scriptManager���� ����;
	 * instanceScript������ levelScript�� key�� ������ �ְ�,
	 * scriptMasnager�� ���� ��û�� ���� instanceScript�� ���� key�� �̿��Ͽ�,
	 * levelScript�� ���� �� �Ѱ���;
	*/



	// �ν��Ͻ����� ����ϴ� ������Ʈ ��ũ��Ʈ���� �Ѱ��� ����;
	// ���� ����ü ���¸� ���ϰ� �ִٰ� �δ� ���� ��û�� ����;
	// levelScript���� �籸���Ͽ� ������;
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



	// �ν��Ͻ� �������� �Ѱ��� ����;
	class InstanceScript
	{ // �ε��� ���� �������� ���� �ɶ�����,
		// �� Ŭ������ ���� ����Ǿ �ȵȴ�;
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
		sc::LuaInstance scriptInstance; // InstanceScript���� ����� ��ũ��Ʈ;		
		
		HashVector levelAgentScriptHashVector; // � ���� ��ũ��Ʈ�� ����ϴ���(��Ʈ��->�ؽ�Ű�� ����);
		HashVector levelFieldScriptHashVector; 
		
		unsigned int nInstanceNameKey;

		std::string instanceName; // �δ� �̸�(�ߺ� �Ǿ �ȵ�);
        SNATIVEID keyMapID; // �⺻ mapID(�ٸ� �δ��̶� �ߺ��Ǿ �ȵ�, ������ ������� ���� �� ����);
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
        std::string fileName; // ��� ��� ���� �� ���� �̸�;
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
        const Faction::GLActorID targetActorID; // ������ ������ �ƴ� ���ư� ���� ���� ����;
    };

    class InformationPosition
    { // �δ� �� �ٸ� �������� �̵��� pLand�� ���� �ؾ� �ϴ� ���,
        // ���� �ϴ� ���� ��� ���·� ��ȯ�ϱ� ���� �ʿ��� ����;
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
		const Faction::GLActorID targetActorID; // �̵��� ���� ��ü ID;
		const SNATIVEID targetMapID;        

		const float fPositionX;
        const float fPositionZ;

		// ��Ȱ������ ����ó���� ���� �κ� !�ݵ�� ��Ȱ,���̵� ���� ������ ���� �Ϸ绡�� �� �ڵ带 ���ֵ��� ����!;
		// �Ŀ� �����ϱ� ���ϰ� �ϱ����� ������ �� const ó���� ���� ����;
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

        const __time64_t    tStartTime;  // ������ �ð�;
        float               fLifeTime;  // Ÿ�̸� ���� �ð�;
        float               fElapsedTime;  // ��� �ð�;
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
		const unsigned int timerReset; // �ʱ�ȭ ���� �ֱ�(1000���� 1��);
		unsigned int sizeMessageNotify; // �˸� ����;
		unsigned int countNotify; // �˸� Ƚ��;
		unsigned int timeCumulative;
		unsigned int timeGap;
		InstanceSystem::PacketCounterElementVector vectorPacketCounterElement;
	};

	// Faction �� ��� �׾��� ��� �絵�� ó���� ����;
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
		DWORD dwMoney;						// �絵���ϱ����� �ʿ��� ��;
		DWORD dwCountOK;					// OK ���� ����;
		float fTime;						// �귯�� �ð�;
		float fRetryTime;					// �絵�� �ð�;
		bool bRetry;						// �絵�� ����;
		bool bSuccess;						// �絵�� ���� ����;

		EMRETRY_RESULT emResult;			// �絵�� ���;

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
