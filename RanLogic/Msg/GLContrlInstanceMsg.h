#pragma once

#include "GLContrlBaseMsg.h"
#include "../InstanceSystem/GLInstanceSystem.h"
#include "../Party/GLPartyDefine.h"

namespace InstanceSystem
{
    enum EMMESSAGE_TYPE
    {
        EMMESSAGE_ERROR,					// error!;
        EMMESSAGE_GM_COMMAND,			    // GM 명령어;
		EMMESSAGE_RELOAD_AF,				// do script Reload agent -> field;
		EMMESSAGE_RELOAD_FA,				// result script Reload field -> agent;
        EMMESSAGE_NOTIFY_PLAYER_POSITION,	// player의 위치 알림;
        EMMESSAGE_NOTIFY_INSTANCE_LOG,		// instance 현황;
        EMMESSAGE_NOTIFY_INSTANCE_LIST,		// instance 목록;
        EMMESSAGE_NOTIFY_CHILD_LAND,		// instance 내 포함된 land ID;
        EMMESSAGE_NOTIFY_PLAYER_LIST,		// instance 내 player 목록;
		EMMESSAGE_NOTIFY_GLOBAL_FLOAT,		// field instance script 내 float global 변수 값;
		EMMESSAGE_NOTIFY_GLOBAL_INTEGER,	// field instance script 내 integer global 변수 값;
		EMMESSAGE_NOTIFY_GLOBAL_BOOLEAN,	// field instance script 내 boolean global 변수 값;
		EMMESSAGE_NOTIFY_GLOBAL_STRING,		// field instance script 내 string global 변수 값;
        EMMESSAGE_MOVE_MAP,				    // player 지역 이동;
        EMMESSAGE_CREATE,					// instance 생성;
        EMMESSAGE_CREATE_COMPLETE,	        // instance 생성 완료;
        EMMESSAGE_DESTROY,					// instance 삭제;
        EMMESSAGE_JOIN,						// (재)입장;
        EMMESSAGE_REQUEST_JOIN_AF,		    // field server instance에 입장 가능 여부 물어봄;
        EMMESSAGE_ANSWER_JOIN_FA,		    // EMMESSAGE_REQUEST_JOIN_AF에 대한 답장;
        EMMESSAGE_UI_CHATMSG,			    // 채팅창에 메시지출력(디버깅용);
		EMMESSAGE_UI_CHATXMLMSG,		    // 채팅창에 XML 메세지 출력;
        EMMESSAGE_UI_SIMPLE_MSG,		    // 화면 중앙에 메시지출력(디버깅용);
        EMMESSAGE_UI_SIMPLE_XMLMSG,		    // 화면 중앙에 XML 메세지 출력;
        EMMESSAGE_UI_TEXT_DLG_MSG,		    // 화면 중상단에 텍스트 다이얼로그 메세지 출력(디버깅용);
        EMMESSAGE_UI_TEXT_DLG_XMLMSG,		    // 화면 중상단에 텍스트 다이얼로그 XML 메세지 출력;
        EMMESSAGE_UI_TIMER_MSGBOX,          // 화면 중앙에 타이머가 있는 Ok,Cancel 메시지 박스 출력;
		EMMESSAGE_EFFECT,				    // 화면에 이펙트를 출력;
		EMMESSAGE_MATCHING_MAIL_AF,		    // 메일 보내기
        EMMESSAGE_SET_POSITION,			    // 특정 위치로 이동(즉시 이동);
        EMMESSAGE_SET_POSITION_FA,		    // 결과;
        EMMESSAGE_DO_MOVE_TO,			    // 특정 위치로 이동(걸어서 이동);
        EMMESSAGE_JOIN_LOCK_AF,			    // 접속 잠금(agent -> field);
        EMMESSAGE_JOIN_LOCK_FA,			    // 접속 잠금(field -> agent);
		EMMESSAGE_JOIN_SUSPEND_AF,		    // 접속 지연(agent -> field);
		EMMESSAGE_JOIN_SUSPEND_FA,		    // 접속 지연(field -> agent);
		EMMESSAGE_REQ_FACTOIN_SET_FACTION,	// Faction 설정.				
        EMMESSAGE_INFORMATION_CHANGE,	    // 인던에 관한 정보 변경사항;
		EMMESSAGE_REQUEST_JOIN,			    // 입장 신청 등록;		
		EMMESSAGE_REQUEST_OUT,			    // 인던에서 쫓아냄;
        EMMESSAGE_CLICK_TRIGGER,		    // 인던 스크립트용 커스텀 NPC 클릭이벤트 발생시 필드에 요청;
        EMMESSAGE_SET_MOTION_BRD,		    // 캐릭터 상태를 특정 모션상태로 변경 (field -> 해당 pc및 pc주변으로 BroadCast);
        EMMESSAGE_SET_ACT_STATE_BRD,	    // 케릭터 ActState를 특성 상태로 변경 (field -> Client);
        EMMESSAGE_NEW_CUSTOM_MESSAGE,	    // 인던간 및 인던과 유저간의 통신을 위한 커스템 메시지;
		EMMESSAGE_MOVE_DONE,			    // 이동이 완료 된 직후에 field -> agent로 보내는 메세지;
        EMMESSAGE_JOIN_PARTY_FA,            // 파티 생성 및 파티원 추가;
        EMMESSAGE_OUT_PARTY_FA,             // 파티 탈퇴;
        EMMESSAGE_CLEAR_PARTY_FA,           // 파티 해산;
		EMMESSAGE_REQUEST_OBSERVE,		// 관전 요청;
		EMMESSAGE_SET_AUTHORITY,			// 권한 설정(field -> agent);
		EMMESSAGE_NOTIFY_COUNT_PACKET,	// subType에 대한 수신된 패킷 수량 알림;
        EMMESSAGE_NSIZE,
    };

    enum EMCOMMAND_TYPE
    { // GM 명령어 목록;
        EMCOMMAND_WHO,					// player 목록 보기;
        EMCOMMAND_WHERE,				// player 위치 보기;
        EMCOMMAND_LAND,					// instance에 속한 land 목록 보기;
		EMCOMMAND_DEBUG,				// agent/field 에서의 mapID 값 확인;
//        EMCOMMAND_FACTION,				// player의 진영 ID 보기;
        EMCOMMAND_LIST,					// instance 목록 보기;
        EMCOMMAND_LOG,					// instance log 보기;
        EMCOMMAND_DESTROY,			// 특정 instance 삭제;
        EMCOMMAND_DESTROY_TYPE,	// 해당 type의 instance 모두 삭제;
        EMCOMMAND_CREATE,				// instance 생성;
		EMCOMMAND_CREATE_TEST,		// instance 생성 x n(부하 테스트);
		EMCOMMAND_SET_GLOBAL,		// script 변수 값 조정;
		EMCOMMAND_GET_GLOBAL,		// script 변수 값 얻어 옮;		
		EMCOMMAND_MOVE_TO_INSTANCE,	// instance -> instance 내 특정 장소로 이동;
		EMCOMMAND_MOVE_TO_GATE,			// instance -> instance 내 특정 Gate로 이동;
		EMCOMMAND_MOVE_TO_FIELD,			// instance -> field 내 특정 장소로 이동;		
		EMCOMMAND_ENTER_TO_INSTANCE,	// field -> instance 내 특정 장소로 이동;
		EMCOMMAND_MOVE_TO,					// instance 내부에서 움직여서 특정 위치로 이동;
		EMCOMMAND_DO_JOIN,				// 특정 player를 인던으로 집어 넣음;
		EMCOMMAND_DO_OUT,				// 특정 player를 인던에서 쫓아냄;
        EMCOMMAND_OFF,					// instance 끔;
        EMCOMMAND_ON,						// instance 켬;
		EMCOMMAND_JOIN_SUSPEND,				// instance 접속 지연;
		EMCOMMAND_JOIN_SUSPEND_TYPE,	// instance 접속 지연;
        EMCOMMAND_JOIN_LOCK,			// instance 접속 잠금;
		EMCOMMAND_JOIN_LOCK_TYPE,	// 해당 종류 instnace 접속 잠금;
        EMCOMMAND_RELOAD,				// script 다시 불러오기;
		EMCOMMAND_COUNT_PACKET,	// 수신된 패킷량 보고;
        EMCOMMAND_NSIZE,
    };

	// 인스턴스 시스템에 관련 된 모든 에러 코드는 이곳에;
	enum EMERROR_TYPE
	{
		EMERROR_CANNOT_JOIN_INSTANCE,
		EMERROR_CANNOT_JOIN_INSTANCE_IN_INSTANCE,
		EMERROR_MISMATCH_INFORMATION,
		EMERROR_ACCESS_DENIED_INSTANCE,
		EMERROR_CANNOT_CREATE_INSTANCE,
		EMERROR_CANNOT_CREATE_INSTANCE_IN_INSTANCE,
		EMERROR_RELOAD_SCRIPT,
		EMERROR_FAILED_TO_CREATE_INSTANCE,
		EMERROR_NOT_EXIST_INSTANCE,
		EMERROR_ALREADY_IN_INSTANCE,
		EMERROR_NOT_EXIST_TYPE_OF_INSTANCE,
		EMERROR_REJECT_JOIN,
        EMERROR_DESTROY_INSTANCE,
		EMERROR_FIELD_SERVER_OUT,
		EMERROR_INVALID_REQUEST_INFORMATION,
		EMERROR_NSIZE,
	};
}


namespace GLMSG
{
	// 인스턴스 베이스 메세지;
	struct NET_INSTANCE_MESSAGE : public NET_MSG_GENERIC
	{
		const InstanceSystem::EMMESSAGE_TYPE emType;

		NET_INSTANCE_MESSAGE(const DWORD nSize, const InstanceSystem::EMMESSAGE_TYPE _emType)
			: NET_MSG_GENERIC(NET_MSG_GCTRL_INSTANCE_SYSTEM, nSize)
			, emType(_emType)
		{
		}
	};

    //! 주의 : NET_INSTANCE_MSGPACK_MESSAGE를 상속받은 모든 구조체의 경우
    // MSGPACK_DEFINE 선언시 emType을 맨 첫번째로 넣은 이후에 자신의 자료형을 넣어줘야한다.
    struct NET_INSTANCE_MSGPACK_MESSAGE // : public NET_MSG_GENERIC
    {
        DWORD emType;

        MSGPACK_DEFINE(emType);

        NET_INSTANCE_MSGPACK_MESSAGE()
            : emType(0)
        {
        }

        NET_INSTANCE_MSGPACK_MESSAGE(const InstanceSystem::EMMESSAGE_TYPE _emType)
            : emType(_emType)
        {
        }
    };

	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
    struct NET_INSTANCE_ERROR : public NET_INSTANCE_MESSAGE
    {
        enum
        {
            NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_MESSAGE) - sizeof(InstanceSystem::InstanceMapID) - sizeof(SNATIVEID) - sizeof(InstanceSystem::EMERROR_TYPE) - sizeof(DWORD)) / sizeof(DWORD),
            HEADER_NSIZE = sizeof(NET_INSTANCE_MESSAGE) + sizeof(InstanceSystem::InstanceMapID) + sizeof(SNATIVEID) + sizeof(InstanceSystem::EMERROR_TYPE) + sizeof(DWORD),
        };

        const InstanceSystem::InstanceMapID instanceMapID;
        const SNATIVEID keyMapID;
        InstanceSystem::EMERROR_TYPE emError;
        DWORD nPlayer;
        InstanceSystem::DBNUM nDbNum[NSIZE];

        const bool addPlayer(const InstanceSystem::DBNUM _nDbNum)
        {
            if ( this->nPlayer == NSIZE )
                return false;

            this->nDbNum[this->nPlayer] = _nDbNum;
            this->dwSize += sizeof(InstanceSystem::DBNUM);
            ++this->nPlayer;

            if ( this->nPlayer == NSIZE )
                return false;

            return true;
        }

        void reset(void)
        {
            this->nPlayer = 0;
            this->dwSize = HEADER_NSIZE;
        }

        const bool isValid(void) const
        {
            return this->nPlayer != 0;
        }

        NET_INSTANCE_ERROR(const SNATIVEID& _keyMapID, const InstanceSystem::InstanceMapID _instanceMapID = GAEAID_NULL)
            : NET_INSTANCE_MESSAGE(HEADER_NSIZE, InstanceSystem::EMMESSAGE_ERROR)
            , instanceMapID(_instanceMapID)
            , keyMapID(_keyMapID)
            , nPlayer(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_ERROR) <= NET_DATA_BUFSIZE);
        }
		NET_INSTANCE_ERROR(const InstanceSystem::EMERROR_TYPE _emError, const SNATIVEID& _keyMapID = SNATIVEID(false), const InstanceSystem::InstanceMapID _instanceMapID = GAEAID_NULL)
            : NET_INSTANCE_MESSAGE(HEADER_NSIZE, InstanceSystem::EMMESSAGE_ERROR)
            , instanceMapID(_instanceMapID)
            , keyMapID(_keyMapID)
            , emError(_emError)
            , nPlayer(0)			
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_ERROR) <= NET_DATA_BUFSIZE);
        }		
    };

    struct NET_INSTANCE_INFORMATION_CHANGE : public NET_INSTANCE_MESSAGE
    {
        InstanceSystem::InstanceInformationClient instanceInformation;

        NET_INSTANCE_INFORMATION_CHANGE(const InstanceSystem::InstanceInformationClient& _instanceInformation)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_INFORMATION_CHANGE), InstanceSystem::EMMESSAGE_INFORMATION_CHANGE)
            , instanceInformation(_instanceInformation)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_INFORMATION_CHANGE) <= NET_DATA_BUFSIZE);
        }
    };

	struct NET_INSTANCE_RELOAD_AF : public NET_INSTANCE_MESSAGE
	{
		const DWORD dwGaeaID;

		NET_INSTANCE_RELOAD_AF(const DWORD _dwGaeaID)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_RELOAD_AF), InstanceSystem::EMMESSAGE_RELOAD_AF)
			, dwGaeaID(_dwGaeaID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RELOAD_AF) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_INSTANCE_RELOAD_FA : public NET_INSTANCE_MESSAGE
	{
		const unsigned int nFieldServer;
		const unsigned int nFieldChannel;
		const bool bSuccess;
		const DWORD dwGaeaID; // 알림용 gaeaID, 없어도 동작하는데 지장 없다;

		NET_INSTANCE_RELOAD_FA(const unsigned int _nFieldServer, const unsigned int _nFieldChannel, const bool _bSuccess, const DWORD _dwGaeaID = GAEAID_NULL)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_RELOAD_FA), InstanceSystem::EMMESSAGE_RELOAD_FA)
			, nFieldServer(_nFieldServer)
			, nFieldChannel(_nFieldChannel)
			, bSuccess(_bSuccess)
			, dwGaeaID(_dwGaeaID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_RELOAD_FA) <= NET_DATA_BUFSIZE);
		}
	};

    struct NET_INSTANCE_GM_COMMAND : public NET_INSTANCE_MESSAGE
    {
		enum
		{
			EMPARAMETER_LENGTH = 128,
		};
        InstanceSystem::EMCOMMAND_TYPE emCommandType;
        union
        {
            struct
            {
                unsigned int nParameter0;
                unsigned int nParameter1;
				unsigned int nParameter2;
				unsigned int nParameter3;
            };	
            struct
            {
                float fParameter0;
                float fParameter1;
				float fParameter2;
				float fParameter3;
            };
        };
        char szParameter0[EMPARAMETER_LENGTH];
        char szParameter1[EMPARAMETER_LENGTH];

        NET_INSTANCE_GM_COMMAND(void)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_GM_COMMAND), InstanceSystem::EMMESSAGE_GM_COMMAND)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_GM_COMMAND) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_NOTIFY_PLAYER_POSITION : public NET_INSTANCE_MESSAGE
    {
        const InstanceSystem::DBNUM dbNum;
        const SNATIVEID baseMapID; // mainMapID/subMapID;		
        const SNATIVEID instanceMapID; // memoryIndex/stage;
        const SNATIVEID keyMapID; // -1 = not in Instance Dungeon;
        const float fPositionX;
        const float fPositionZ;
        char targetPlayerName[CHAR_SZNAME];

        NET_INSTANCE_NOTIFY_PLAYER_POSITION(
            const InstanceSystem::DBNUM _dbNum, const SNATIVEID& _baseMapID, const D3DXVECTOR3& _position,
            const SNATIVEID& _instanceMapID = SNATIVEID(false), const SNATIVEID _keyMapID = SNATIVEID(false))
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_NOTIFY_PLAYER_POSITION), InstanceSystem::EMMESSAGE_NOTIFY_PLAYER_POSITION)
            , dbNum(_dbNum)
            , baseMapID(_baseMapID)
            , instanceMapID(_instanceMapID)
            , keyMapID(_keyMapID)
            , fPositionX(_position.x)
            , fPositionZ(_position.z)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_PLAYER_POSITION) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_NOTIFY_INSTANCE_LOG : public NET_INSTANCE_MESSAGE
    {		
        typedef InstanceSystem::InstanceLog CustomArray;

        enum
        {
            NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_MESSAGE) - sizeof(DWORD)) / sizeof(CustomArray),
            HEADER_NSIZE = sizeof(NET_INSTANCE_MESSAGE) + sizeof(DWORD),
        };
        DWORD nInstance;
        CustomArray instanceLog[NSIZE];

        const bool addInstance(const CustomArray& _instanceLog)
        {
            if ( this->nInstance == NSIZE )
                return false;

            this->instanceLog[this->nInstance] = _instanceLog;
            this->dwSize += sizeof(CustomArray);
            ++this->nInstance;

            if ( this->nInstance == NSIZE )
                return false;

            return true;
        }

        void reset(void)
        {
            this->nInstance = 0;
            this->dwSize = HEADER_NSIZE;
        }

        const bool isValid(void) const
        {
            return this->nInstance != 0;
        }

        NET_INSTANCE_NOTIFY_INSTANCE_LOG(void)
            : NET_INSTANCE_MESSAGE(HEADER_NSIZE, InstanceSystem::EMMESSAGE_NOTIFY_INSTANCE_LOG)
            , nInstance(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_INSTANCE_LOG) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_NOTIFY_INSTANCE_LIST : public NET_INSTANCE_MESSAGE
    {
        typedef InstanceSystem::InstanceList CustomArray;

        enum
        {
            NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_MESSAGE) - sizeof(DWORD) - sizeof(DWORD) - sizeof(DWORD) - sizeof(DWORD)) / sizeof(CustomArray),
            HEADER_NSIZE = sizeof(NET_INSTANCE_MESSAGE) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD),
        };
        const DWORD nViewPage;
        const DWORD nMaxPage;
        const DWORD nMaxInstance;
        DWORD nInstance;
        CustomArray instanceList[NSIZE];

        const bool addInstance(const CustomArray& _instanceLog)
        {
            if ( this->nInstance == NSIZE )
                return false;

            this->instanceList[this->nInstance] = _instanceLog;
            this->dwSize += sizeof(CustomArray);
            ++this->nInstance;

            if ( this->nInstance == NSIZE )
                return false;

            return true;
        }

        void reset(void)
        {
            this->nInstance = 0;
            this->dwSize = HEADER_NSIZE;
        }

        const bool isValid(void) const
        {
            return this->nInstance != 0;
        }

        NET_INSTANCE_NOTIFY_INSTANCE_LIST(const DWORD _nViewPage, const DWORD _nMaxPage, const DWORD _nMaxInstance)
            : NET_INSTANCE_MESSAGE(HEADER_NSIZE, InstanceSystem::EMMESSAGE_NOTIFY_INSTANCE_LIST)
            , nViewPage(_nViewPage)
            , nMaxPage(_nMaxPage)
            , nMaxInstance(_nMaxInstance)
            , nInstance(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_INSTANCE_LIST) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_NOTIFY_CHILD_LAND : public NET_INSTANCE_MESSAGE
    {
        const SNATIVEID keyMapID;
        const InstanceSystem::InstanceMapID instanceMapID;		
        SNATIVEID childMapID[MAX_LAND_SID];	
        DWORD nChildMap;

        void addChildMapID(const SNATIVEID& _childMapID)
        {
            if ( this->nChildMap == MAX_LAND_SID )
                return;

            this->childMapID[this->nChildMap] = _childMapID;
            ++this->nChildMap;
        }

        NET_INSTANCE_NOTIFY_CHILD_LAND(const SNATIVEID& _keyMapID, const InstanceSystem::InstanceMapID& _instanceMapID)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_NOTIFY_CHILD_LAND), InstanceSystem::EMMESSAGE_NOTIFY_CHILD_LAND)
            , keyMapID(_keyMapID)
            , instanceMapID(_instanceMapID)
            , nChildMap(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_CHILD_LAND) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_NOTIFY_PLAYER_LIST : public NET_INSTANCE_MESSAGE
    {
        struct PlayerElement
        {
            InstanceSystem::DBNUM dbNum;
            char playerName[CHAR_SZNAME];
			InstanceSystem::EMAUTHORITY typeAuthority;
        };
        typedef PlayerElement CustomArray;

        enum
        {
            NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_MESSAGE) - sizeof(SNATIVEID) - sizeof(InstanceSystem::InstanceMapID) - sizeof(DWORD)) / sizeof(CustomArray),
            HEADER_NSIZE = sizeof(NET_INSTANCE_MESSAGE) + sizeof(SNATIVEID) + sizeof(InstanceSystem::InstanceMapID) + sizeof(DWORD),
        };

        const bool addPlayer(const InstanceSystem::DBNUM _dbNum, const char* const _playerName, InstanceSystem::EMAUTHORITY _typeAuthority)
        {
            if ( this->nPlayer == NSIZE )
                return false;

            const unsigned int _nIndex(this->nPlayer);
            this->playerList[_nIndex].dbNum = _dbNum;
			this->playerList[_nIndex].typeAuthority = _typeAuthority;

            if ( _playerName != 0 )
                ::StringCchCopy(this->playerList[_nIndex].playerName, CHAR_SZNAME, _playerName);
            this->dwSize += sizeof(CustomArray);
            ++this->nPlayer;

            if ( this->nPlayer == NSIZE )
                return false;

            return true;
        }

        void reset(void)
        {
            this->nPlayer = 0;
            this->dwSize = HEADER_NSIZE;
        }

        const bool isValid(void) const
        {
            return this->nPlayer != 0;
        }

        const SNATIVEID keyMapID;
        const InstanceSystem::InstanceMapID instanceMapID;
        DWORD nPlayer;
        CustomArray playerList[NSIZE];

        NET_INSTANCE_NOTIFY_PLAYER_LIST(const SNATIVEID& _keyMapID, const InstanceSystem::InstanceMapID& _instanceMapID)
            : NET_INSTANCE_MESSAGE(HEADER_NSIZE, InstanceSystem::EMMESSAGE_NOTIFY_PLAYER_LIST)
            , keyMapID(_keyMapID)
            , instanceMapID(_instanceMapID)
            , nPlayer(0)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_PLAYER_LIST) <= NET_DATA_BUFSIZE);
        }
    };
		
	struct NET_INSTANCE_NOTIFY_GLOBAL_FLOAT : public NET_INSTANCE_MESSAGE
	{
		struct CustomArray
		{
			DWORD nKey;
			float value;
		};
		enum
		{
			NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_MESSAGE) - sizeof(bool) - sizeof(InstanceSystem::InstanceMapID) - sizeof(SNATIVEID) - (sizeof(char) * 64) - sizeof(DWORD)) / sizeof(CustomArray),
			HEADER_NSIZE = sizeof(NET_INSTANCE_MESSAGE) + sizeof(bool) + sizeof(InstanceSystem::InstanceMapID) + sizeof(SNATIVEID) + (sizeof(char) * 64) + sizeof(DWORD),
		};
		const bool bTableValue;
		const InstanceSystem::InstanceMapID instanceMapID;
		const SNATIVEID keyMapID;

		char globalName[64];
		DWORD nVariable;
		CustomArray globalVariable[NSIZE];

		const bool addGlobal(const DWORD _nKey, const float _value)
		{
			if ( this->nVariable == NSIZE )
				return false;

			const unsigned int _nIndex(this->nVariable);
			this->globalVariable[_nIndex].nKey = _nKey;
			this->globalVariable[_nIndex].value = _value;
			
			this->dwSize += sizeof(CustomArray);
			++this->nVariable;

			if ( this->nVariable == NSIZE )
				return false;

			return true;
		}

		void reset(void)
		{
			this->nVariable = 0;
			this->dwSize = HEADER_NSIZE;
		}

		const bool isValid(void) const
		{
			return this->nVariable != 0;
		}

		NET_INSTANCE_NOTIFY_GLOBAL_FLOAT(const char* const _globalName, const InstanceSystem::InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const bool _bTableValue = false)
			: NET_INSTANCE_MESSAGE(HEADER_NSIZE, InstanceSystem::EMMESSAGE_NOTIFY_GLOBAL_FLOAT)
			, nVariable(0)
			, bTableValue(_bTableValue)
			, instanceMapID(_instanceMapID)
			, keyMapID(_keyMapID)
		{
			::StringCchCopy(this->globalName, 63, _globalName);
			this->globalName[63] = 0;
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_GLOBAL_FLOAT) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_INSTANCE_NOTIFY_GLOBAL_INTEGER : public NET_INSTANCE_MESSAGE
	{
		struct CustomArray
		{
			DWORD nKey;
			int value;
		};
		enum
		{
			NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_MESSAGE) - sizeof(InstanceSystem::InstanceMapID) - sizeof(SNATIVEID) - sizeof(DWORD) - (sizeof(char) * 64) - sizeof(DWORD)) / sizeof(CustomArray),
			HEADER_NSIZE = sizeof(NET_INSTANCE_MESSAGE) + sizeof(InstanceSystem::InstanceMapID) + sizeof(SNATIVEID) + sizeof(DWORD) + (sizeof(char) * 64) + sizeof(DWORD),
		};
		const bool bTableValue;
		const InstanceSystem::InstanceMapID instanceMapID;
		const SNATIVEID keyMapID;

		char globalName[64];
		DWORD nVariable;
		CustomArray globalVariable[NSIZE];

		const bool addGlobal(const DWORD _nKey, const int _value)
		{
			if ( this->nVariable == NSIZE )
				return false;

			const unsigned int _nIndex(this->nVariable);
			this->globalVariable[_nIndex].nKey = _nKey;
			this->globalVariable[_nIndex].value = _value;

			this->dwSize += sizeof(CustomArray);
			++this->nVariable;

			if ( this->nVariable == NSIZE )
				return false;

			return true;
		}

		void reset(void)
		{
			this->nVariable = 0;
			this->dwSize = HEADER_NSIZE;
		}

		const bool isValid(void) const
		{
			return this->nVariable != 0;
		}

		NET_INSTANCE_NOTIFY_GLOBAL_INTEGER(const char* const _globalName, const InstanceSystem::InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const bool _bTableValue = false)
			: NET_INSTANCE_MESSAGE(HEADER_NSIZE, InstanceSystem::EMMESSAGE_NOTIFY_GLOBAL_INTEGER)
			, nVariable(0)
			, bTableValue(_bTableValue)
			, instanceMapID(_instanceMapID)
			, keyMapID(_keyMapID)
		{
			::StringCchCopy(this->globalName, 63, _globalName);
			this->globalName[63] = 0;
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_GLOBAL_INTEGER) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_INSTANCE_WINNER : public NET_INSTANCE_MESSAGE
	{		
		const SNATIVEID instanceMapID;

		NET_INSTANCE_WINNER(const SNATIVEID& _instanceMapID)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_WINNER), InstanceSystem::EMMESSAGE_DESTROY)
			, instanceMapID(_instanceMapID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_WINNER) <= NET_DATA_BUFSIZE);
		}
	};


	struct NET_FACTION_RELATION_SET_FACTOIN: public NET_INSTANCE_MESSAGE
	{
		const DWORD dwDbNum;
		const DWORD dwFaction;
		const DWORD instanceMapID;
		Faction::EMFACTION_TYPE factiontype;

		NET_FACTION_RELATION_SET_FACTOIN(const DWORD _dwDbNum,
			const DWORD _dwFaction,const DWORD _mapID,
			Faction::EMFACTION_TYPE _factionType)
			: NET_INSTANCE_MESSAGE(sizeof(NET_FACTION_RELATION_SET_FACTOIN), InstanceSystem::EMMESSAGE_REQ_FACTOIN_SET_FACTION)
			, dwDbNum(_dwDbNum) , dwFaction(_dwFaction) , instanceMapID(_mapID) , factiontype(_factionType)
		{
			MIN_STATIC_ASSERT(sizeof(NET_FACTION_RELATION_SET_FACTOIN) <= NET_DATA_BUFSIZE);
		}
	};


	struct NET_INSTANCE_NOTIFY_GLOBAL_BOOLEAN : public NET_INSTANCE_MESSAGE
	{
		struct CustomArray
		{
			DWORD nKey;
			bool value;
		};
		enum
		{
			NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_MESSAGE) - sizeof(InstanceSystem::InstanceMapID) - sizeof(SNATIVEID) - sizeof(bool) - (sizeof(char) * 64) - sizeof(DWORD)) / sizeof(CustomArray),
			HEADER_NSIZE = sizeof(NET_INSTANCE_MESSAGE) + sizeof(InstanceSystem::InstanceMapID) + sizeof(SNATIVEID) + sizeof(bool) + (sizeof(char) * 64) + sizeof(DWORD),
		};

		const bool bTableValue;
		const InstanceSystem::InstanceMapID instanceMapID;
		const SNATIVEID keyMapID;

		char globalName[64];
		DWORD nVariable;
		CustomArray globalVariable[NSIZE];

		const bool addGlobal(const DWORD _nKey, const bool _value)
		{
			if ( this->nVariable == NSIZE )
				return false;

			const unsigned int _nIndex(this->nVariable);
			this->globalVariable[_nIndex].nKey = _nKey;
			this->globalVariable[_nIndex].value = _value;

			this->dwSize += sizeof(CustomArray);
			++this->nVariable;

			if ( this->nVariable == NSIZE )
				return false;

			return true;
		}

		void reset(void)
		{
			this->nVariable = 0;
			this->dwSize = HEADER_NSIZE;
		}

		const bool isValid(void) const
		{
			return this->nVariable != 0;
		}

		NET_INSTANCE_NOTIFY_GLOBAL_BOOLEAN(const char* const _globalName, const InstanceSystem::InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const bool _bTableValue = false)
			: NET_INSTANCE_MESSAGE(HEADER_NSIZE, InstanceSystem::EMMESSAGE_NOTIFY_GLOBAL_BOOLEAN)
			, nVariable(0)
			, bTableValue(_bTableValue)
			, instanceMapID(_instanceMapID)
			, keyMapID(_keyMapID)
		{
			::StringCchCopy(this->globalName, 63, _globalName);
			this->globalName[63] = 0;
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_GLOBAL_BOOLEAN) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_INSTANCE_NOTIFY_GLOBAL_STRING : public NET_INSTANCE_MESSAGE
	{
		struct CustomArray
		{
			DWORD nKey;
			char value[64];
		};
		enum
		{
			NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_MESSAGE) - sizeof(InstanceSystem::InstanceMapID) - sizeof(SNATIVEID) - sizeof(bool) - (sizeof(char) * 64) - sizeof(DWORD)) / sizeof(CustomArray),
			HEADER_NSIZE = sizeof(NET_INSTANCE_MESSAGE) + sizeof(InstanceSystem::InstanceMapID) + sizeof(SNATIVEID) + sizeof(bool) + (sizeof(char) * 64) + sizeof(DWORD),
		};
		const bool bTableValue;
		const InstanceSystem::InstanceMapID instanceMapID;
		const SNATIVEID keyMapID;

		char globalName[64];
		DWORD nVariable;
		CustomArray globalVariable[NSIZE];

		const bool addGlobal(const DWORD _nKey, const char* const _value)
		{
			if ( this->nVariable == NSIZE )
				return false;

			const unsigned int _nIndex(this->nVariable);
			this->globalVariable[_nIndex].nKey = _nKey;
			::StringCchCopy(this->globalVariable[_nIndex].value, 63, _value);
			this->globalVariable[_nIndex].value[63] = 0;

			this->dwSize += sizeof(CustomArray);
			++this->nVariable;

			if ( this->nVariable == NSIZE )
				return false;

			return true;
		}

		void reset(void)
		{
			this->nVariable = 0;
			this->dwSize = HEADER_NSIZE;
		}

		const bool isValid(void) const
		{
			return this->nVariable != 0;
		}

		NET_INSTANCE_NOTIFY_GLOBAL_STRING(const char* const _globalName, const InstanceSystem::InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const bool _bTableValue = false)
			: NET_INSTANCE_MESSAGE(HEADER_NSIZE, InstanceSystem::EMMESSAGE_NOTIFY_GLOBAL_BOOLEAN)
			, nVariable(0)
			, bTableValue(_bTableValue)
			, instanceMapID(_instanceMapID)
			, keyMapID(_keyMapID)
		{
			::StringCchCopy(this->globalName, 63, _globalName);
			this->globalName[63] = 0;
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_GLOBAL_STRING) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_INSTANCE_MOVE_MAP : public NET_INSTANCE_MESSAGE
	{ // 이동 통보 메세지;		
		const InstanceSystem::DBNUM dbNum;
		const MapID fromMapID;
		const D3DXVECTOR3 fromMapPosition;
		//const DWORD fromFieldServer;
		const MapID toMapID;

		// 
		const bool bAfterMove;

		NET_INSTANCE_MOVE_MAP(const InstanceSystem::DBNUM _dbNum,
			const MapID& _fromMapID, const D3DXVECTOR3& _fromMapPosition,// const DWORD _fromFieldServer,
			const MapID& _toMapID, const bool _bAfterMove = true )
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_MOVE_MAP), InstanceSystem::EMMESSAGE_MOVE_MAP)
			, dbNum(_dbNum)
			, fromMapID(_fromMapID)
			, fromMapPosition(_fromMapPosition)
			//, fromFieldServer(_fromFieldServer)
			, toMapID(_toMapID)
			, bAfterMove(_bAfterMove)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_MOVE_MAP) <= NET_DATA_BUFSIZE);
		}
		NET_INSTANCE_MOVE_MAP(const NET_INSTANCE_MOVE_MAP& _rhs)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_MOVE_MAP), InstanceSystem::EMMESSAGE_MOVE_MAP)
			, dbNum(_rhs.dbNum)
			, fromMapID(_rhs.fromMapID)
			, fromMapPosition(_rhs.fromMapPosition)
			//, fromFieldServer(_rhs.fromFieldServer)
			, toMapID(_rhs.toMapID)
			, bAfterMove(_rhs.bAfterMove)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_MOVE_MAP) <= NET_DATA_BUFSIZE);
		}
	};

    struct NET_INSTANCE_CREATE : public NET_INSTANCE_MESSAGE
    {        
        const InstanceSystem::InstanceMapID instanceMapID;
		const SNATIVEID keyMapID;
        const SNATIVEID baseMapID;
		const InstanceSystem::EMINSTANCE_TYPE emInstanceType;


		NET_INSTANCE_CREATE(const SNATIVEID& _keyMapID, const InstanceSystem::InstanceMapID _instanceMapID, const SNATIVEID& _baseMapID, const InstanceSystem::EMINSTANCE_TYPE _emInstanceType)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_CREATE), InstanceSystem::EMMESSAGE_CREATE)
            , keyMapID(_keyMapID)
            , instanceMapID(_instanceMapID)
            , baseMapID(_baseMapID)
			, emInstanceType(_emInstanceType)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_CREATE) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_CREATE_COMPLETE : public NET_INSTANCE_MESSAGE
    {
        const InstanceSystem::InstanceMapID instanceMapID;
		const bool bFirstMake;
        const bool bSuccess;

        NET_INSTANCE_CREATE_COMPLETE(const InstanceSystem::InstanceMapID _instanceMapID, const bool _bFirstMake, const bool _bSuccess = true)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_CREATE_COMPLETE), InstanceSystem::EMMESSAGE_CREATE_COMPLETE)
            , instanceMapID(_instanceMapID)
			, bFirstMake(_bFirstMake)
            , bSuccess(_bSuccess)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_CREATE_COMPLETE) <= NET_DATA_BUFSIZE);
        }
    };	

    struct NET_INSTANCE_DESTROY : public NET_INSTANCE_MESSAGE
    {		
        const InstanceSystem::InstanceMapID instanceMapID;
		const DWORD dwFieldChannel;
		const DWORD dwFieldID;

        NET_INSTANCE_DESTROY(const InstanceSystem::InstanceMapID _instanceMapID, const DWORD _dwFieldChannel = net::INVALID_SLOT, const DWORD _dwFieldID = net::INVALID_SLOT)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_DESTROY), InstanceSystem::EMMESSAGE_DESTROY)
            , instanceMapID(_instanceMapID)
			, dwFieldChannel(_dwFieldChannel)
			, dwFieldID(_dwFieldID)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_DESTROY) <= NET_DATA_BUFSIZE);
        }
    };
    
    // agent에 입장 가능 등록이 됐다고 해도,
    // 실제 인던측 사정에 따라 접속 불가 혹은 대기 상태여야 할 수 있으므로,
    // 인던 생성후 혹은 진행중이라 하더라도 바로 입장 시키지 않고 field server님께 여쭈어 본다;
    struct NET_INSTANCE_REQUEST_JOIN_AF : public NET_INSTANCE_MESSAGE
    {	
        const InstanceSystem::InstanceMapID instanceMapID;
        const SNATIVEID keyMapID;
		const InstanceSystem::DBNUM dbNum;
		const InstanceSystem::EMAUTHORITY emAuthority;
		

		NET_INSTANCE_REQUEST_JOIN_AF(const InstanceSystem::InstanceMapID _instanceMapID, const SNATIVEID& _keyMapID, const InstanceSystem::DBNUM _dbNum, const InstanceSystem::EMAUTHORITY _emAuthority)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_REQUEST_JOIN_AF), InstanceSystem::EMMESSAGE_REQUEST_JOIN_AF)
            , instanceMapID(_instanceMapID)
            , keyMapID(_keyMapID)
			, dbNum(_dbNum)
			, emAuthority(_emAuthority)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_REQUEST_JOIN_AF) <= NET_DATA_BUFSIZE);
        }
    };
    struct NET_INSTANCE_ANSWER_JOIN_FA : public NET_INSTANCE_MESSAGE
    {
        const InstanceSystem::InstanceMapID instanceMapID;
        const SNATIVEID keyMapID;
		const InstanceSystem::DBNUM dbNum;
		const bool bReject;

		NET_INSTANCE_ANSWER_JOIN_FA(const InstanceSystem::InstanceMapID _instanceMapID, const SNATIVEID& _keyMapID, const InstanceSystem::DBNUM _dbNum, const bool _bReject)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_ANSWER_JOIN_FA), InstanceSystem::EMMESSAGE_ANSWER_JOIN_FA)
            , instanceMapID(_instanceMapID)
            , keyMapID(_keyMapID)
            , dbNum(_dbNum)
			, bReject(_bReject)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_ANSWER_JOIN_FA) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_UI_CHATMSG : public NET_INSTANCE_MSGPACK_MESSAGE
    {
        std::string strText;
        DWORD dwColor;

        MSGPACK_DEFINE(emType, strText, dwColor);

        NET_INSTANCE_UI_CHATMSG()
            : NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_UI_CHATMSG)
        {
        }
        NET_INSTANCE_UI_CHATMSG(const char* _pText, const DWORD _dwColor)
            : NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_UI_CHATMSG)
            , strText(_pText)
            , dwColor(_dwColor)
        {
        }
    };

    struct NET_INSTANCE_UI_CHATXMLMSG : public NET_INSTANCE_MSGPACK_MESSAGE
    {
        DWORD dwColor;
        InstanceSystem::InstanceXmlString xmlString;

        MSGPACK_DEFINE(emType, dwColor, xmlString);

        // EMMESSAGE_UI_CHATXMLMSG;

        NET_INSTANCE_UI_CHATXMLMSG()
            : NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_UI_CHATXMLMSG)
        {
        }
    };

    struct NET_INSTANCE_UI_SIMPLE_MSG : public NET_INSTANCE_MSGPACK_MESSAGE
    {
        float fShowTime;
        std::string strText;
        DWORD dwColor;
        DWORD dwFontSize;

        MSGPACK_DEFINE(emType, fShowTime, strText, dwColor);

        NET_INSTANCE_UI_SIMPLE_MSG()
            : NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_UI_SIMPLE_MSG)
        {
        }
        NET_INSTANCE_UI_SIMPLE_MSG(const float _fShowTime, const char* const _pText, const DWORD _dwColor, const DWORD _dwFontSize)
            : NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_UI_SIMPLE_MSG)
            , fShowTime(_fShowTime)
            , strText(_pText)
            , dwColor(_dwColor)
            , dwFontSize(_dwFontSize)
        {
        }
    };

    struct NET_INSTANCE_UI_SIMPLE_XMLMSG : public NET_INSTANCE_MSGPACK_MESSAGE
    {
        InstanceSystem::InstanceXmlString xmlString;
        float fShowTime;
        DWORD dwColor;
        DWORD dwFontSize;

        MSGPACK_DEFINE(emType, xmlString, fShowTime, dwColor, dwFontSize);

        NET_INSTANCE_UI_SIMPLE_XMLMSG()
            : NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_UI_SIMPLE_XMLMSG)
        {
        }
    };

    struct NET_INSTANCE_UI_TEXT_DLG_MSG : public NET_INSTANCE_MSGPACK_MESSAGE
    {
        std::string strText;
        DWORD dwColor;

        MSGPACK_DEFINE(emType, strText, dwColor);

        NET_INSTANCE_UI_TEXT_DLG_MSG()
            : NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_UI_TEXT_DLG_MSG)
        {
        }
    };

    struct NET_INSTANCE_UI_TEXT_DLG_XMLMSG : public NET_INSTANCE_MSGPACK_MESSAGE
    {
        InstanceSystem::InstanceXmlString xmlString;
        DWORD dwColor;

        MSGPACK_DEFINE(emType, xmlString, dwColor);

        NET_INSTANCE_UI_TEXT_DLG_XMLMSG()
            : NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_UI_TEXT_DLG_XMLMSG)
        {
        }
    };

    struct NET_INSTANCE_UI_TIMER_MSGBOX : public NET_INSTANCE_MSGPACK_MESSAGE
    {
        DWORD dwDbNum;  // UI를 출력할 User의 CharDbnum;

        std::string _strContentText;
        std::string _strOkText;
        std::string _strCancelText;

        DWORD emResponseType;
        DWORD dwInstanceID;
        DWORD dwResponseMsgParam1;
        DWORD dwResponseMsgParam2;

        float fTimer;  // 타이머 시간 ( 0일경우 타이머 사용안함 );

        MSGPACK_DEFINE(emType, dwDbNum, _strContentText, _strOkText, _strCancelText, emResponseType, dwInstanceID, dwResponseMsgParam1, dwResponseMsgParam2, fTimer);

        NET_INSTANCE_UI_TIMER_MSGBOX()
            : NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_UI_TIMER_MSGBOX)
            , dwDbNum(0)
            , emResponseType(InstanceSystem::EM_TO_MY_INSTANCE)
            , dwInstanceID(0)
            , dwResponseMsgParam1(0)
            , dwResponseMsgParam2(0)
            , fTimer(0.0f)
        {
        }
    };

	// 이펙트 출력 메시지
	struct NET_INSTANCE_EFFECT: public NET_INSTANCE_MSGPACK_MESSAGE
	{
		std::string _strEffectName;  // 이펙트 파일 이름;
        float _fEffectZonePosX;     // 이펙트 지역;
        float _fEffectZonePosY;
        float _fEffectZonePosZ;

        DWORD _targetActorType;     // 이펙트 대상;
        DWORD _targetActorID;
        //DWORD _targetPosX;          // 이펙트 대상 위치;
        //DWORD _targetPosY;
        //DWORD _targetPosZ;

		MSGPACK_DEFINE(emType, _strEffectName, _fEffectZonePosX, _fEffectZonePosY, _fEffectZonePosZ, _targetActorType, _targetActorID);

		NET_INSTANCE_EFFECT()
			:NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_EFFECT)
            , _fEffectZonePosX(0.0f)
            , _fEffectZonePosY(0.0f)
            , _fEffectZonePosZ(0.0f)
            , _targetActorType(0)
            , _targetActorID(0)
		{
		}
	};

	// 메칭 우편 메시지
	struct NET_INSTANCE_MAIL: public NET_INSTANCE_MSGPACK_MESSAGE
	{
		InstanceSystem::SystemMail Mail;

		MSGPACK_DEFINE(emType,Mail);

		NET_INSTANCE_MAIL()
			:NET_INSTANCE_MSGPACK_MESSAGE(InstanceSystem::EMMESSAGE_MATCHING_MAIL_AF)
		{
		}

	};


    struct NET_INSTANCE_SET_POSITION : public NET_INSTANCE_MESSAGE
    {	
        const InstanceSystem::InstanceMapID instanceMapID;
        const InstanceSystem::InformationPosition informationSetPosition;	

        NET_INSTANCE_SET_POSITION(const InstanceSystem::InstanceMapID& _instanceMapID, const InstanceSystem::InformationPosition& _reserveSetPosition)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_SET_POSITION), InstanceSystem::EMMESSAGE_SET_POSITION)
            , instanceMapID(_instanceMapID)
            , informationSetPosition(_reserveSetPosition)
        {
            // targetActorType/targetActorID 가 GAEAID_NULL이냐 아니냐에 따라 이동 형태가 달라짐;
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_SET_POSITION) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_SET_POSITION_FA : public NET_INSTANCE_MESSAGE
    {
        const Faction::GLActorID actorID;
		const MapID targetMapID;
        const bool bFail;

        NET_INSTANCE_SET_POSITION_FA(const Faction::GLActorID& _actorID, const MapID& _targetMapID = MapID(), const bool _bFail = true)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_SET_POSITION_FA), InstanceSystem::EMMESSAGE_SET_POSITION_FA)
            , actorID(_actorID)
            , targetMapID(_targetMapID)
            , bFail(_bFail)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_SET_POSITION_FA) <= NET_DATA_BUFSIZE);
        }
    };	

    struct NET_INSTANCE_DO_MOVE_TO : public NET_INSTANCE_MESSAGE
    {
        const InstanceSystem::InstanceMapID instanceMapID;
        const Faction::GLActorID actorID;
        const Faction::GLActorID targetActorID;
        const D3DXVECTOR3 targetPosition;

        NET_INSTANCE_DO_MOVE_TO(const InstanceSystem::InstanceMapID& _instanceMapID, const Faction::GLActorID& _actorID, const Faction::GLActorID& _targetID, const D3DXVECTOR3& _targetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f))
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_DO_MOVE_TO), InstanceSystem::EMMESSAGE_DO_MOVE_TO)
            , instanceMapID(_instanceMapID)
            , actorID(_actorID)
            , targetActorID(_targetID)
            , targetPosition(_targetPosition)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_DO_MOVE_TO) <= NET_DATA_BUFSIZE);

        }
    };

    struct NET_INSTANCE_JOIN_LOCK_AF : public NET_INSTANCE_MESSAGE
    {
        const InstanceSystem::InstanceMapID instanceMapID;
		const SNATIVEID keyMapID;
        const bool bLock;

		NET_INSTANCE_JOIN_LOCK_AF(const InstanceSystem::InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const bool _bLock)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_JOIN_LOCK_AF), InstanceSystem::EMMESSAGE_JOIN_LOCK_AF)
            , instanceMapID(_instanceMapID)
			, keyMapID(_keyMapID)
            , bLock(_bLock)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_JOIN_LOCK_AF) <= NET_DATA_BUFSIZE);
        }
		NET_INSTANCE_JOIN_LOCK_AF(const NET_INSTANCE_JOIN_LOCK_AF& _rhs)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_JOIN_LOCK_AF), InstanceSystem::EMMESSAGE_JOIN_LOCK_AF)
			, instanceMapID(_rhs.instanceMapID)
			, keyMapID(_rhs.keyMapID)
			, bLock(_rhs.bLock)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_JOIN_LOCK_AF) <= NET_DATA_BUFSIZE);
		}
    };

    struct NET_INSTANCE_JOIN_LOCK_FA : public NET_INSTANCE_MESSAGE
    {
        const InstanceSystem::InstanceMapID instanceMapID;
		const SNATIVEID keyMapID;
        const bool bLock;

        NET_INSTANCE_JOIN_LOCK_FA(const InstanceSystem::InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const bool _bLock)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_JOIN_LOCK_FA), InstanceSystem::EMMESSAGE_JOIN_LOCK_FA)
            , instanceMapID(_instanceMapID)
			, keyMapID(_keyMapID)
            , bLock(_bLock)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_JOIN_LOCK_FA) <= NET_DATA_BUFSIZE);
        }
		NET_INSTANCE_JOIN_LOCK_FA(const NET_INSTANCE_JOIN_LOCK_FA& _rhs)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_JOIN_LOCK_FA), InstanceSystem::EMMESSAGE_JOIN_LOCK_FA)
			, instanceMapID(_rhs.instanceMapID)
			, keyMapID(_rhs.keyMapID)
			, bLock(_rhs.bLock)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_JOIN_LOCK_FA) <= NET_DATA_BUFSIZE);
		}
    };

	struct NET_INSTANCE_JOIN_SUSPEND_AF : public NET_INSTANCE_MESSAGE
	{
		const InstanceSystem::InstanceMapID instanceMapID;
		const SNATIVEID keyMapID; 
		const bool bSuspend;

		NET_INSTANCE_JOIN_SUSPEND_AF(const InstanceSystem::InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const bool _bSuspend)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_JOIN_SUSPEND_AF), InstanceSystem::EMMESSAGE_JOIN_SUSPEND_AF)
			, instanceMapID(_instanceMapID)
			, keyMapID(_keyMapID)
			, bSuspend(_bSuspend)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_JOIN_SUSPEND_AF) <= NET_DATA_BUFSIZE);
		}
		NET_INSTANCE_JOIN_SUSPEND_AF(const NET_INSTANCE_JOIN_SUSPEND_AF& _rhs)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_JOIN_SUSPEND_AF), InstanceSystem::EMMESSAGE_JOIN_SUSPEND_AF)
			, instanceMapID(_rhs.instanceMapID)
			, keyMapID(_rhs.keyMapID)
			, bSuspend(_rhs.bSuspend)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_JOIN_SUSPEND_AF) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_INSTANCE_JOIN_SUSPEND_FA : public NET_INSTANCE_MESSAGE
	{
		const InstanceSystem::InstanceMapID instanceMapID;
		const SNATIVEID keyMapID; 
		const bool bSuspend;

		NET_INSTANCE_JOIN_SUSPEND_FA(const InstanceSystem::InstanceMapID& _instanceMapID, const SNATIVEID& _keyMapID, const bool _bSuspend)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_JOIN_SUSPEND_FA), InstanceSystem::EMMESSAGE_JOIN_SUSPEND_FA)
			, instanceMapID(_instanceMapID)
			, keyMapID(_keyMapID)
			, bSuspend(_bSuspend)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_JOIN_SUSPEND_FA) <= NET_DATA_BUFSIZE);
		}
		NET_INSTANCE_JOIN_SUSPEND_FA(const NET_INSTANCE_JOIN_SUSPEND_FA& _rhs)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_JOIN_LOCK_FA), InstanceSystem::EMMESSAGE_JOIN_SUSPEND_FA)
			, instanceMapID(_rhs.instanceMapID)
			, keyMapID(_rhs.keyMapID)
			, bSuspend(_rhs.bSuspend)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_JOIN_SUSPEND_FA) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_INSTANCE_REQUEST_JOIN : public NET_INSTANCE_MESSAGE
	{
		const InstanceSystem::InformationPosition informationSetPosition;	
		const InstanceSystem::EMAUTHORITY emAuthority;

		NET_INSTANCE_REQUEST_JOIN(const InstanceSystem::InformationPosition& _informationPosition, InstanceSystem::EMAUTHORITY _emAuthority)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_REQUEST_JOIN), InstanceSystem::EMMESSAGE_REQUEST_JOIN)
			, informationSetPosition(_informationPosition)
			, emAuthority(_emAuthority)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_REQUEST_JOIN) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_INSTANCE_REQUEST_OUT : public NET_INSTANCE_MESSAGE
	{
		const InstanceSystem::InstanceMapID instanceMapID;
		const InstanceSystem::DBNUM dbNum;

		NET_INSTANCE_REQUEST_OUT(const InstanceSystem::InstanceMapID _instanceMapID, const InstanceSystem::DBNUM _dbNum)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_REQUEST_OUT), InstanceSystem::EMMESSAGE_REQUEST_OUT)
			, instanceMapID(_instanceMapID)
			, dbNum(_dbNum)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_REQUEST_OUT) <= NET_DATA_BUFSIZE);
		}
	};


    // 인던 스크립트용 커스텀 NPC 클릭시 Client -> Field로 이벤트 메시지 전송;
    struct NET_INSTANCE_CLICK_TRIGGER : public NET_INSTANCE_MESSAGE
    {
        STARGETID sTarget;

        NET_INSTANCE_CLICK_TRIGGER( const STARGETID& _target )
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_CLICK_TRIGGER), InstanceSystem::EMMESSAGE_CLICK_TRIGGER)
            , sTarget(_target)
        {
            MIN_STATIC_ASSERT( sizeof(NET_INSTANCE_CLICK_TRIGGER)<= NET_DATA_BUFSIZE );
        }
    };

    // 인던<->인던 혹은 인던<->클라이언트간 의 커스텀 메시지를 주고 받음;
    struct NET_INSTANCE_NEW_CUSTOM_MESSAGE : public NET_INSTANCE_MESSAGE
    {
        // ID값의 의미는 Type에 따라 다르며 아래와 같음;
        // EM_USER : CharDbNum, EM_TO_MY_INSTANCE : CharDbNum,  Base급 : 인던 컨텐츠 KeyMapID,  Level급 : 인던 객체의 InstanceMapID;

        DWORD emDestType;   // 보낼 목적지 타입;
        DWORD dwDestID;     // 보낼 목적지 정보;
        DWORD emSrcType;    // 보낸 출발지 타입;
        DWORD dwSrcID;      // 보낸 출발지 정보;

        // 커스텀 메시지의 값;
        // 추후 String이나 float등의 값 추가 필요시 여기에 추가;
        DWORD param1;
        DWORD param2;
        DWORD param3;
        DWORD param4;

        NET_INSTANCE_NEW_CUSTOM_MESSAGE(DWORD _param1 , DWORD _param2 = 0, DWORD _param3 = 0, DWORD _param4 = 0 )
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_NEW_CUSTOM_MESSAGE), InstanceSystem::EMMESSAGE_NEW_CUSTOM_MESSAGE)
            , emDestType(InstanceSystem::EM_MESSAGE_USER)
            , dwDestID(0)
            , emSrcType(InstanceSystem::EM_MESSAGE_USER)
            , dwSrcID(0)
            , param1(_param1)
            , param2(_param2)
            , param3(_param3)
            , param4(_param4)
        {
            MIN_STATIC_ASSERT( sizeof(NET_INSTANCE_NEW_CUSTOM_MESSAGE)<= NET_DATA_BUFSIZE );
        }
    };

    // 캐릭터 상태를 특정 모션상태로 변경 (field -> 해당 pc및 pc주변으로 BroadCast);
    struct NET_INSTANCE_SET_MOTION_BRD : public NET_INSTANCE_MESSAGE
    {
        DWORD dwGaeaID;
        DWORD emMainType;
        DWORD emSubType;
        float fAniSec;

        NET_INSTANCE_SET_MOTION_BRD(const DWORD GaeaId)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_SET_MOTION_BRD), InstanceSystem::EMMESSAGE_SET_MOTION_BRD)
            , dwGaeaID(GaeaId)
            , emMainType(UINT_MAX)
            , emSubType(UINT_MAX)
            , fAniSec(0.0f)
        {
            MIN_STATIC_ASSERT( sizeof(NET_INSTANCE_SET_MOTION_BRD)<= NET_DATA_BUFSIZE );
        }
        NET_INSTANCE_SET_MOTION_BRD(const DWORD GaeaId, const DWORD _mainID, const DWORD _subID, const float _aniSec)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_SET_MOTION_BRD), InstanceSystem::EMMESSAGE_SET_MOTION_BRD)
            , dwGaeaID(GaeaId)
            , emMainType(_mainID)
            , emSubType(_subID)
            , fAniSec(_aniSec)
        {
            MIN_STATIC_ASSERT( sizeof(NET_INSTANCE_SET_MOTION_BRD)<= NET_DATA_BUFSIZE );
        }
    };

    struct NET_INSTANCE_SET_ACT_STATE_BRD : public NET_INSTANCE_MESSAGE
    {
        DWORD dwGaeaID;
        DWORD dwActStateType;
        bool bState;

        NET_INSTANCE_SET_ACT_STATE_BRD(const DWORD GaeaID, const DWORD _actState, const bool _state)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_SET_ACT_STATE_BRD), InstanceSystem::EMMESSAGE_SET_ACT_STATE_BRD)
            , dwGaeaID(GaeaID)
            , dwActStateType(_actState)
            , bState(_state)
        {
            MIN_STATIC_ASSERT( sizeof(NET_INSTANCE_SET_ACT_STATE_BRD)<= NET_DATA_BUFSIZE );
        }
    };

	struct NET_INSTANCE_MOVE_DONE : public NET_INSTANCE_MESSAGE
	{
		const MapID mapIDCurrent;
		const DWORD dbNum;

		NET_INSTANCE_MOVE_DONE(const MapID& _mapIDCurrent, const DWORD _dbNum)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_MOVE_DONE), InstanceSystem::EMMESSAGE_MOVE_DONE)
			, mapIDCurrent(_mapIDCurrent)
			, dbNum(_dbNum)
		{
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_MOVE_DONE) <= NET_DATA_BUFSIZE);
		}
	};

    struct NET_INSTANCE_JOIN_PARTY_FA : public NET_INSTANCE_MESSAGE
    {
        const DWORD dwMasterGaeaID;
        const DWORD dwMemberGaeaID;
        const SPartyOption partyOption;

        NET_INSTANCE_JOIN_PARTY_FA(const DWORD _dwMasterDbNum, const DWORD _dwMemberDbNum, const SPartyOption& _partyOption)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_JOIN_PARTY_FA), InstanceSystem::EMMESSAGE_JOIN_PARTY_FA)
            , dwMasterGaeaID(_dwMasterDbNum)
            , dwMemberGaeaID(_dwMemberDbNum)
            , partyOption(_partyOption)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_JOIN_PARTY_FA) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_OUT_PARTY_FA : public NET_INSTANCE_MESSAGE
    {
        const DWORD dwGaeaID;

        NET_INSTANCE_OUT_PARTY_FA(const DWORD _dbNum)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_OUT_PARTY_FA), InstanceSystem::EMMESSAGE_OUT_PARTY_FA)
            , dwGaeaID(_dbNum)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_OUT_PARTY_FA) <= NET_DATA_BUFSIZE);
        }
    };

    struct NET_INSTANCE_CLEAR_PARTY_FA : public NET_INSTANCE_MESSAGE
    {
        const DWORD dwPartyID;

        NET_INSTANCE_CLEAR_PARTY_FA(const DWORD _dwPartyID)
            : NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_CLEAR_PARTY_FA), InstanceSystem::EMMESSAGE_CLEAR_PARTY_FA)
            , dwPartyID(_dwPartyID)
        {
            MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_CLEAR_PARTY_FA) <= NET_DATA_BUFSIZE);
        }
    };

	struct NET_INSTANCE_REQUEST_OBSERVE : public NET_INSTANCE_MESSAGE
	{
		NET_INSTANCE_REQUEST_OBSERVE(void)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_REQUEST_OBSERVE), InstanceSystem::EMMESSAGE_REQUEST_OBSERVE)
			, instanceMapIDTargetObserve(InstanceSystem::InstanceMapID(-1))
			, dbNumTargetObserve(InstanceSystem::DBNUM(-1))
		{			
		}

		// 아래 셋 중에 하나만 설정하면 해당 인던에 관전 요청을 보낸다;
		// 처리 우선 순위는 : instanceMapID > dbNum > name 이다;
		InstanceSystem::InstanceMapID instanceMapIDTargetObserve;
		InstanceSystem::DBNUM dbNumTargetObserve;
		char namePlayerTargetObserve[CHAR_SZNAME];
	};		

	struct NET_INSTANCE_NOTIFY_COUNT_PACKET : public NET_INSTANCE_MESSAGE
	{		
		struct PACKET_COUNT
		{
			union
			{
				struct
				{
					unsigned short indexPacket;
					unsigned short countPacket;
				};
				unsigned int informationPacket;
			};
		};

		enum
		{
			ARRAY_NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_INSTANCE_MESSAGE) - sizeof(unsigned int)) / sizeof(NET_INSTANCE_NOTIFY_COUNT_PACKET::PACKET_COUNT),
		};


		const bool isValid(void) const { return this->sizePacket == 0 ? false : true; }
		const bool push_back(const unsigned int _indexPacket, const unsigned int _countPacket)
		{
			if ( this->sizePacket >= NET_INSTANCE_NOTIFY_COUNT_PACKET::ARRAY_NSIZE )
				return false;

			this->countPacket[this->sizePacket].indexPacket = unsigned short(_indexPacket);
			this->countPacket[this->sizePacket].countPacket = unsigned short(_countPacket);

			this->sizePacket += 1;
			dwSize += sizeof(NET_INSTANCE_NOTIFY_COUNT_PACKET::PACKET_COUNT);
			if ( this->sizePacket >= NET_INSTANCE_NOTIFY_COUNT_PACKET::ARRAY_NSIZE )
				return false;

			return true;
		}
		void clear(void)
		{
			this->sizePacket = 0;
			dwSize = sizeof(NET_INSTANCE_NOTIFY_COUNT_PACKET) - sizeof(NET_INSTANCE_NOTIFY_COUNT_PACKET::PACKET_COUNT) * NET_INSTANCE_NOTIFY_COUNT_PACKET::ARRAY_NSIZE;
		}


		NET_INSTANCE_NOTIFY_COUNT_PACKET(void)
			: NET_INSTANCE_MESSAGE(sizeof(NET_INSTANCE_NOTIFY_COUNT_PACKET), InstanceSystem::EMMESSAGE_NOTIFY_COUNT_PACKET)
			, sizePacket(0)
		{
			dwSize = sizeof(NET_INSTANCE_NOTIFY_COUNT_PACKET) - sizeof(NET_INSTANCE_NOTIFY_COUNT_PACKET::PACKET_COUNT) * NET_INSTANCE_NOTIFY_COUNT_PACKET::ARRAY_NSIZE;
			MIN_STATIC_ASSERT(sizeof(NET_INSTANCE_NOTIFY_COUNT_PACKET) <= NET_DATA_BUFSIZE);
		}

		unsigned int sizePacket;
		PACKET_COUNT countPacket[NET_INSTANCE_NOTIFY_COUNT_PACKET::ARRAY_NSIZE];
	};

	/// 원래의 란 시스템에 맞춰서 패킷모니터링에 노출시키기 위해서;
	/// 새로운 헤더를 정의한다;

	// Instance System 의 모든 패킷의 헤더;
	// 사용할 수 있는 패킷이 아니다;
	struct NET_INSTANCE_MESSAGE_HEADER : public NET_MSG_GENERIC
	{
		InstanceSystem::InstanceMapID nInstanceID;
		SNATIVEID sKeyMapID;

		NET_INSTANCE_MESSAGE_HEADER ( const DWORD _nSize
			, const EMNET_MSG _msg
			, InstanceSystem::InstanceMapID _nInstanceID
			, SNATIVEID _sKeyMapID )
			: NET_MSG_GENERIC( _msg, _nSize )
			, nInstanceID ( _nInstanceID )
			, sKeyMapID ( _sKeyMapID )
		{

		}
	};

	struct NET_RETRY_FACTION_FB : public NET_INSTANCE_MESSAGE_HEADER
	{
		float fTime;				// 나가기 시간;
		DWORD dwMoney;				// 사용되는 돈;
		bool bShow;

		NET_RETRY_FACTION_FB ( InstanceSystem::InstanceMapID _nInstanceID
			, SNATIVEID _sKeyMapID )
			: NET_INSTANCE_MESSAGE_HEADER ( sizeof( NET_RETRY_FACTION_FB )
			, NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_FB
			, _nInstanceID
			, _sKeyMapID )
			, fTime ( 0.f )
			, dwMoney ( 0 )
			, bShow ( true )
		{

		}
	};

	struct NET_RETRY_FACTION_CF : public NET_INSTANCE_MESSAGE_HEADER
	{
		bool bOK;

		NET_RETRY_FACTION_CF ( InstanceSystem::InstanceMapID _nInstanceID
			, SNATIVEID _sKeyMapID )
			: NET_INSTANCE_MESSAGE_HEADER ( sizeof( NET_RETRY_FACTION_CF )
			, NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_CF
			, _nInstanceID
			, _sKeyMapID )
			, bOK ( false )
		{

		}
	};

	struct NET_SHOW_TIME_UI_FB : public NET_INSTANCE_MESSAGE_HEADER
	{
		bool bShow;
		float fSec;					// 나가기 시간;

		NET_SHOW_TIME_UI_FB ( InstanceSystem::InstanceMapID _nInstanceID
			, SNATIVEID _sKeyMapID )
			: NET_INSTANCE_MESSAGE_HEADER ( sizeof( NET_SHOW_TIME_UI_FB )
			, NET_MSG_GCTRL_INSTANCE_SHOW_TIME_UI_FB
			, _nInstanceID
			, _sKeyMapID )
			, bShow ( false )
			, fSec ( 0.f )
		{

		}
	};

	struct NET_SHOW_ENTRANCE_STATE_UI_FB : public NET_INSTANCE_MESSAGE_HEADER
	{
		DWORD dwCurCount;
		DWORD dwMaxCount;
		bool bShow;
		float fSec;					// 나가기 시간;

		NET_SHOW_ENTRANCE_STATE_UI_FB ( InstanceSystem::InstanceMapID _nInstanceID
			, SNATIVEID _sKeyMapID )
			: NET_INSTANCE_MESSAGE_HEADER ( sizeof( NET_SHOW_ENTRANCE_STATE_UI_FB )
			, NET_MSG_GCTRL_INSTANCE_SHOW_ENTRANCE_STATE_UI_FB
			, _nInstanceID
			, _sKeyMapID )
			, dwCurCount ( 0 )
			, dwMaxCount ( 0 )
			, bShow ( false )
			, fSec ( 0.f )
		{

		}
	};

	struct NET_SET_RETURNMAP_INFORMATION_FA : public NET_INSTANCE_MESSAGE_HEADER
	{
		DWORD dwChaDBNum;
		InstanceSystem::MapInformation sReturnMapInformation;

		NET_SET_RETURNMAP_INFORMATION_FA ( InstanceSystem::InstanceMapID _nInstanceID
			, SNATIVEID _sKeyMapID )
			: NET_INSTANCE_MESSAGE_HEADER ( sizeof( NET_SET_RETURNMAP_INFORMATION_FA )
			, NET_MSG_GCTRL_INSTANCE_SET_RETURNMAP_INFORMATION_FA
			, _nInstanceID
			, _sKeyMapID )
			, dwChaDBNum ( 0 )
		{

		}
	};
}
