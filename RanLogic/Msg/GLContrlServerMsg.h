#pragma once

#include "../AutoLevel/GLAutoLevelDefine.h"
#include "./GLContrlBaseMsg.h"

#define GM_NONCONFRONT_MODE_VIEW_MAX 20
#define GM_EVENT_EXP_MAX_RATE 4.0f
#define GM_EVENT_EXP_MIN_RATE 1.0f

enum GameDataType
{
	ItemList, 
	QuestList, 
	SkillList, 
};

namespace GLMSG
{
#pragma pack(1)

	struct SNET_PERIOD : public NET_MSG_GENERIC
	{
		SPRERIODTIME sPRERIODTIME;
		SNET_PERIOD()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PERIOD;
			MIN_STATIC_ASSERT(sizeof(SNET_PERIOD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_FIELDINFO_RESET : public NET_MSG_GENERIC
	{
		SNET_FIELDINFO_RESET()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_FIELDINFO_RESET;
			MIN_STATIC_ASSERT(sizeof(SNET_FIELDINFO_RESET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_WEATHER : public NET_MSG_GENERIC
	{
		DWORD				dwWeather;
		SNETPC_WEATHER () :
		dwWeather(NULL)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_WEATHER;
			MIN_STATIC_ASSERT(sizeof(SNETPC_WEATHER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_WHIMSICALWEATHER : public NET_MSG_GENERIC
	{
		DWORD				dwWhimsical;

		SNETPC_WHIMSICALWEATHER () :
		dwWhimsical(NULL)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_WHIMSICAL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_WHIMSICALWEATHER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_MAPWEATHER : public NET_MSG_GENERIC
	{
		enum { MAX_WEATHER = 32 };
		SONEMAPWEATHER				MapWeather[MAX_WEATHER];
		DWORD						dwMapWeatherSize;
		
        SNETPC_MAPWEATHER ()  :
		dwMapWeatherSize(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_MAPWEATHER;
			MIN_STATIC_ASSERT(sizeof(SNETPC_MAPWEATHER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_MAPWHIMSICALWEATHER : public NET_MSG_GENERIC
	{
		SONEMAPWEATHER				MapWeather;
		SNETPC_MAPWHIMSICALWEATHER () 
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_MAPWHIMSICAL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_MAPWHIMSICALWEATHER)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_WEATHER : public NET_MSG_GENERIC
	{
		bool				bActive;
		DWORD				dwWeather;

		SNETSERVER_CTRL_WEATHER () :
		bActive(true),
			dwWeather(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_SERVER_CTRL_WEATHER;
			MIN_STATIC_ASSERT(sizeof(SNETSERVER_CTRL_WEATHER)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_WEATHER2 : public NET_MSG_GENERIC
	{
		DWORD				dwWeather;
		WORD				map_mID;
		WORD				map_sID;
		DWORD				dwApplyTime;

		SNETSERVER_CTRL_WEATHER2 () :
		dwWeather(0),
			map_mID(0),
			map_sID(0),
			dwApplyTime(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_SERVER_CTRL_WEATHER2;
			MIN_STATIC_ASSERT(sizeof(SNETSERVER_CTRL_WEATHER2)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_TIME : public NET_MSG_GENERIC
	{
        DWORD				dwTime;

		SNETSERVER_CTRL_TIME () :
		dwTime(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_SERVER_CTRL_TIME;
			MIN_STATIC_ASSERT(sizeof(SNETSERVER_CTRL_TIME)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_MONTH : public NET_MSG_GENERIC
	{
		DWORD				dwMonth;

		SNETSERVER_CTRL_MONTH () :
		dwMonth(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_SERVER_CTRL_MONTH;
			MIN_STATIC_ASSERT(sizeof(SNETSERVER_CTRL_MONTH)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETSERVER_CTRL_GENITEMHOLD : public NET_MSG_GENERIC
	{
		bool				bHold;

		SNETSERVER_CTRL_GENITEMHOLD () :
		bHold(true)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_SERVER_CTRL_GENITEMHOLD;
			MIN_STATIC_ASSERT(sizeof(SNETSERVER_CTRL_GENITEMHOLD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_GM_MOVE2GATE : public NET_MSG_GENERIC
	{
		DWORD				dwGATE;

		SNET_GM_MOVE2GATE () :
		dwGATE(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOVE2GATE;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOVE2GATE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOVE2GATE_FLD : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwGATE;

		SNET_GM_MOVE2GATE_FLD () :
		dwGATE(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOVE2GATE_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOVE2GATE_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOVE2GATE_FB : public NET_MSG_GENERIC
	{
		D3DXVECTOR3			vPOS;
		DWORD				dwFlags;

		SNET_GM_MOVE2GATE_FB () :
			vPOS		( 0, 0, 0 ),
			dwFlags		( 0 )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOVE2GATE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOVE2GATE_FB)<=NET_DATA_BUFSIZE);
		}
	};	

	struct SNET_GM_MOVE2MAPPOS : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNATIVEID			nidMAP;

		DWORD				dwPOSX;
		DWORD				dwPOSY;

		SNET_GM_MOVE2MAPPOS()
            : nidMAP(false)
            , dwPOSX(0)
            , dwPOSY(0)
		{
			dwSize = sizeof(SNET_GM_MOVE2MAPPOS);
			nType = NET_MSG_GM_MOVE2MAPPOS;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOVE2MAPPOS)<=NET_DATA_BUFSIZE);
		}
		SNET_GM_MOVE2MAPPOS(const SNATIVEID& _mapID, const DWORD _posX, const DWORD _posY)
			: nidMAP(_mapID)
			, dwPOSX(_posX)
			, dwPOSY(_posY)
		{
			dwSize = sizeof(SNET_GM_MOVE2MAPPOS);
			nType = NET_MSG_GM_MOVE2MAPPOS;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOVE2MAPPOS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOVE2MAPPOS_FLD : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNATIVEID			nidMAP;
		DWORD				dwGaeaID;
		DWORD				dwPOSX;
		DWORD				dwPOSY;

		SNET_GM_MOVE2MAPPOS_FLD () :
		nidMAP(false),
			dwGaeaID(0),
			dwPOSX(0),
			dwPOSY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOVE2MAPPOS_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOVE2MAPPOS_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_BLOCK_DETECTED : public NET_MSG_GENERIC
	{
		enum { MAX_INFO = 512 };
		DWORD dwDETECTED;
		DWORD dwCHARID;
		char szINFO[MAX_INFO];

		SNET_BLOCK_DETECTED(DWORD Detect, DWORD CharId, const char* Info)
            : dwDETECTED(Detect)
            , dwCHARID(CharId)
		{
			dwSize = sizeof(SNET_BLOCK_DETECTED);
			nType = NET_MSG_BLOCK_DETECTED;
			MIN_STATIC_ASSERT(sizeof(SNET_BLOCK_DETECTED)<=NET_DATA_BUFSIZE);

            memset(szINFO, 0, sizeof(char)*MAX_INFO);
            if (Info)
                StringCchCopy(szINFO, MAX_INFO, Info);
		}
	};

	struct SNET_GM_SHOWMETHEMONEY : public NET_MSG_GENERIC
	{
		LONGLONG llMoney;

		SNET_GM_SHOWMETHEMONEY(LONGLONG AddMoney)
            : llMoney(AddMoney)
		{
			dwSize = sizeof(SNET_GM_SHOWMETHEMONEY);
			nType = NET_MSG_GM_SHOWMETHEMONEY;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_SHOWMETHEMONEY)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_SHOWMETHEMONEY_FLD : public NET_MSG_GENERIC
	{
		DWORD	 dwGaeaID;
		LONGLONG llMoney;

		SNET_GM_SHOWMETHEMONEY_FLD()
            : dwGaeaID(0)
            , llMoney(0)
		{
			dwSize = sizeof(SNET_GM_SHOWMETHEMONEY_FLD);
			nType = NET_MSG_GM_SHOWMETHEMONEY_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_SHOWMETHEMONEY_FLD)<=NET_DATA_BUFSIZE);
		}
	};

    struct NET_GM_SHOWMETHEMONEY_EMULATOR_CF : public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 9273, };
        int m_MagicNum;
        LONGLONG llMoney;
        
        NET_GM_SHOWMETHEMONEY_EMULATOR_CF(LONGLONG AddMoney)
            : llMoney(AddMoney)
            , m_MagicNum(MAGIC_NUM)
        {
            dwSize = sizeof(NET_GM_SHOWMETHEMONEY_EMULATOR_CF);
            nType = NET_MSG_GM_SHOWMETHEMONEY_EMULATOR_CF;
            MIN_STATIC_ASSERT(sizeof(NET_GM_SHOWMETHEMONEY_EMULATOR_CF)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_GM_WHYSOSERIOUS : public NET_MSG_GENERIC
    {
		char szTargetName[CHAR_SZNAME];
		DWORD       dwNums;
        SNATIVEID   sItemID;        

        SNET_GM_WHYSOSERIOUS(void)
        {
            dwSize = sizeof(SNET_GM_WHYSOSERIOUS);
            nType = NET_MSG_GM_WHYSOSERIOUS;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_WHYSOSERIOUS)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_GM_WHYSOSERIOUS_EMULATOR_CF : public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM = 486, };
        int         m_MagicNum;
        SNATIVEID   sItemID;
        DWORD       dwNums;
        
        NET_GM_WHYSOSERIOUS_EMULATOR_CF(const SNATIVEID& ItemId, DWORD ItemNum)
            : sItemID(ItemId)
            , dwNums(ItemNum)
            , m_MagicNum(MAGIC_NUM)
        {
            dwSize = sizeof(NET_GM_WHYSOSERIOUS_EMULATOR_CF);
            nType = NET_MSG_GM_WHYSOSERIOUS_EMULATOR_CF;
            MIN_STATIC_ASSERT(sizeof(NET_GM_WHYSOSERIOUS_EMULATOR_CF)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_GM_WHYSOSERIOUS_FLD : public NET_MSG_GENERIC
    {		
		DWORD		dwMasterUserNum;
		DWORD		dwMasterGaeaID;
		DWORD		dwGaeaID;
        SNATIVEID   sItemID;
        DWORD       dwNums;		
		TCHAR		szMasterIP[MAX_IP_LENGTH + 1];
		TCHAR		szMasterID[USR_ID_LENGTH + 1];

        SNET_GM_WHYSOSERIOUS_FLD ()
			: dwGaeaID(GAEAID_NULL)
			, sItemID(false)
			, dwNums(0)
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_GM_WHYSOSERIOUS_FLD;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_WHYSOSERIOUS_FLD)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNET_GM_WHYSOSERIOUS_FB_FAC : public NET_MSG_GENERIC
	{
		DWORD dwGaeaIDGM;
		TCHAR szTargetName[CHAR_SZNAME + 1];
		SNATIVEID   sItemID;
		DWORD       dwNums;
		bool			bFailed;

		SNET_GM_WHYSOSERIOUS_FB_FAC(const DWORD _dwGaeaIDGM, const TCHAR* const _szTargetName, const SNATIVEID& _itemID, const DWORD _dwNums)
			: dwGaeaIDGM(_dwGaeaIDGM)
			, sItemID(_itemID)
			, dwNums(_dwNums)
		{
			dwSize = sizeof(SNET_GM_WHYSOSERIOUS_FB_FAC);
			nType = NET_MSG_GM_WHYSOSERIOUS_FB_FAC;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHYSOSERIOUS_FB_FAC) <= NET_DATA_BUFSIZE);

			::StringCchCopy(szTargetName, CHAR_SZNAME, _szTargetName);
		}
	};

    struct SNET_GM_LOG_TO_DB_CAF : public NET_MSG_GENERIC
    {
        SNET_GM_LOG_TO_DB_CAF ()
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_GM_LOG_TO_DB_CAF;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_LOG_TO_DB_CAF)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNET_GM_LAND : public NET_MSG_GENERIC
	{
		enum
		{
			EMLAND_INFORMATION, // Land에 대한 정보;
		};

		unsigned int nSubType;
		unsigned int nParameter0;
		unsigned int nParameter1;

		SNET_GM_LAND(void)
		{
			dwSize = sizeof(SNET_GM_LAND);
			nType = NET_MSG_GM_LAND;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LAND) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LAND_INFORMATION : public NET_MSG_GENERIC
	{
		SNATIVEID mapID;
		unsigned int nStage;
		unsigned int nUsedMemory;

		SNATIVEID mapIDLand[MAX_LAND_SID][2];

		void addInformationLand(const SNATIVEID& _baseMapID, const SNATIVEID& _instanceMapID)
		{
			if ( nStage >= MAX_LAND_SID )
				return;

			mapIDLand[nStage][0] = _baseMapID;
			mapIDLand[nStage][1] = _instanceMapID;

			++nStage;
		}

		SNET_GM_LAND_INFORMATION(void)
			: nStage(0)
		{
			dwSize = sizeof(SNET_GM_LAND_INFORMATION);
			nType = NET_MSG_GM_LAND_INFORMATION;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LAND_INFORMATION) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MAP_LAYER : public NET_MSG_GENERIC
	{
		enum
		{
			EMMAP_LAYER_NEW, // 새로운 Layer로 전환;
			EMMAP_LAYER_DEL, // 원래 Layer로 전환;
			EMMAP_LAYER_LIST, // 설정 되어 있는 Layer 목록 출력;
		};

		unsigned int nSubType;
		unsigned int nParameter0;
		unsigned int nParameter1;
		unsigned int nParameter2;
		unsigned int nParameter3;

		SNET_GM_MAP_LAYER(void)
		{
			dwSize = sizeof(SNET_GM_MAP_LAYER);
			nType = NET_MSG_GM_MAP_LAYER;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MAP_LAYER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MAP_LAYER_NEW_NOTIFY : public NET_MSG_GENERIC
	{
		const SNATIVEID mapID;
		const DWORD nLayerTo;

		SNET_GM_MAP_LAYER_NEW_NOTIFY(const SNATIVEID& _mapID, const DWORD _nLayerTo)
			: mapID(_mapID)
			, nLayerTo(_nLayerTo)
		{
			dwSize = sizeof(SNET_GM_MAP_LAYER_NEW_NOTIFY);
			nType = NET_MSG_GM_MAP_LAYER_NEW_NOTIFY;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MAP_LAYER_NEW_NOTIFY)<=NET_DATA_BUFSIZE);
		}
	};
	struct SNET_GM_MAP_LAYER_DEL_NOTIFY : public NET_MSG_GENERIC
	{
		const SNATIVEID mapID;

		SNET_GM_MAP_LAYER_DEL_NOTIFY(const SNATIVEID& _mapID)
			: mapID(_mapID)
		{
			dwSize = sizeof(SNET_GM_MAP_LAYER_DEL_NOTIFY);
			nType = NET_MSG_GM_MAP_LAYER_DEL_NOTIFY;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MAP_LAYER_DEL_NOTIFY)<=NET_DATA_BUFSIZE);
		}
	};
	struct SNET_GM_MAP_LAYER_ERROR_NOTIFY : public NET_MSG_GENERIC
	{
		enum
		{
			EMERROR_WRONG_MAP_ID,
			EMERROR_DIFFERENT_FIELD_SERVER,
		};

		const SNATIVEID mapID;
		const DWORD emErrorCode;

		SNET_GM_MAP_LAYER_ERROR_NOTIFY(const SNATIVEID& _mapID, const DWORD _emErrorCode)
			: mapID(_mapID)
			, emErrorCode(_emErrorCode)
		{
			dwSize = sizeof(SNET_GM_MAP_LAYER_ERROR_NOTIFY);
			nType = NET_MSG_GM_MAP_LAYER_ERROR_NOTIFY;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MAP_LAYER_ERROR_NOTIFY) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_AUTO_LEVEL : public NET_MSG_GENERIC
	{
		enum
		{
			EMAUTO_LEVEL_LIST, // auto-level 목록;
			EMAUTO_LEVEL_TODAY, // 오늘 예약 되어 있는 auto-level 목록;
			EMAUTO_LEVEL_INFORMATION, // auto-level의 세부 정보;			
			EMAUTO_LEVEL_BEGIN, // auto-level 시작;
			EMAUTO_LEVEL_END, // auto-level 종료;
			EMAUTO_LEVEL_PROCESS, // auto-level 진행;
			EMAUTO_LEVEL_RESERVE, // auto-level 예약;
			EMAUTO_LEVEL_ON, // auto-level 켬;
			EMAUTO_LEVEL_OFF, // auto-level 끔;
		};

		unsigned int nSubType;
		unsigned int nParameter0;
		unsigned int nParameter1;

		SNET_GM_AUTO_LEVEL(void)
		{
			dwSize = sizeof(SNET_GM_AUTO_LEVEL);
			nType = NET_MSG_GM_AUTO_LEVEL;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_AUTO_LEVEL) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_AUTO_LEVEL_INFORMATION : public NET_MSG_GENERIC
	{
		const DWORD indexLevel;		
		const SNATIVEID hallMapID;
		const SNATIVEID callMapID;
		const GLLEV_TIME beginTime;
		const GLLEV_TIME endTime;
		bool eventDay[7]; // 월 ~ 일;
		char levelName[64];

		SNET_GM_AUTO_LEVEL_INFORMATION(const DWORD _indexLevel, const DWORD _eventDay[7], const std::string& _levelName, const SNATIVEID& _hallMapID, const SNATIVEID& _callMapID, const GLLEV_TIME& _beginTime, const GLLEV_TIME& _endTime)
			: indexLevel(_indexLevel)
			, hallMapID(_hallMapID)
			, callMapID(_callMapID)
			, beginTime(_beginTime)
			, endTime(_endTime)
		{
			StringCchCopy(levelName, 63, _levelName.c_str());
			for ( unsigned int _index(7); _index--; )
				eventDay[_index] = _eventDay[_index] ? true : false;

			dwSize = sizeof(SNET_GM_AUTO_LEVEL_INFORMATION);
			nType = NET_MSG_GM_AUTO_LEVEL_INFORMATION;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_AUTO_LEVEL_INFORMATION) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_AUTO_LEVEL_LIST : public NET_MSG_GENERIC
	{
		struct LevelName
		{
			unsigned int nIndex;
			bool bRunning;
			bool bUse;
			char levelName[64];
		};

		enum
		{
			NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_MSG_GENERIC) - sizeof(unsigned int)) / sizeof(LevelName),
			HEADER_NSIZE = sizeof(NET_MSG_GENERIC) + sizeof(DWORD),
		};

		DWORD nLevelSize;
		LevelName levelName[NSIZE];

		const bool addLevel(const std::string& _levelName, const unsigned int _nIndex, const bool _bRunning, const bool _bUse)
		{
			if ( nLevelSize == NSIZE )
				return false;

			levelName[nLevelSize].nIndex = _nIndex;
			levelName[nLevelSize].bUse = _bUse;
			levelName[nLevelSize].bRunning = _bRunning;
			StringCchCopy(levelName[nLevelSize].levelName, 63, _levelName.c_str());

			dwSize += sizeof(LevelName);
			++nLevelSize;

			if ( nLevelSize == NSIZE )
				return false;

			return true;
		}

		const bool isValid(void)
		{
			return nLevelSize != 0;
		}
		void reset(void)
		{
			dwSize = HEADER_NSIZE;
			nLevelSize = 0;
		}		

		SNET_GM_AUTO_LEVEL_LIST(void)
			: nLevelSize(0)
		{
			dwSize = HEADER_NSIZE;
			nType = NET_MSG_GM_AUTO_LEVEL_LIST;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_AUTO_LEVEL_LIST) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_AUTO_LEVEL_RESULT : NET_MSG_GENERIC
	{
		enum
		{
			EMRESULT_SUCCESS_BEGIN,
			EMRESULT_SUCCESS_END,
			EMRESULT_SUCCESS_PROCESS,
			EMRESULT_WRONG_INDEX,
			EMRESULT_NOT_USING,
			EMRESULT_NOT_RUNNING,
			EMRESULT_ALREADY_RUNNING,
			EMRESULT_NSIZE,
		};

		unsigned int nResult;		

		SNET_GM_AUTO_LEVEL_RESULT(const unsigned int _nResult = EMRESULT_NSIZE)
			: nResult(_nResult)
		{
			dwSize = sizeof(SNET_GM_AUTO_LEVEL_RESULT);
			nType = NET_MSG_GM_AUTO_LEVEL_RESULT;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_AUTO_LEVEL_RESULT) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_KICK_USER : public NET_MSG_GENERIC
	{
		DWORD				dwID;
		char				szCHARNAME[CHAR_SZNAME];

		SNET_GM_KICK_USER () :
		dwID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_KICK_USER;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			MIN_STATIC_ASSERT(sizeof(SNET_GM_KICK_USER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_KICK_USER_PROC : public NET_MSG_GENERIC
	{
		SNET_GM_KICK_USER_PROC ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_KICK_USER_PROC;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_KICK_USER_PROC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_KICK_USER_PROC_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwID;

		SNET_GM_KICK_USER_PROC_FLD () :
		dwID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_KICK_USER_PROC_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_KICK_USER_PROC_FLD)<=NET_DATA_BUFSIZE);
		}
	};

    //! 경험치 상승 명령 ServerManager->Session
    struct NET_EVENT_EXP_MS : public NET_MSG_GENERIC
    {
        int m_ServerGroup;
        float m_Scale;

        NET_EVENT_EXP_MS(int ServerGroup, float Scale)
            : m_ServerGroup(ServerGroup)
            , m_Scale(Scale)
        {
            MIN_STATIC_ASSERT(sizeof(NET_EVENT_EXP_MS)<=NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_EVENT_EXP_MS);
            nType = NET_MSG_EVENT_EXP_MS;
        }
    };
	
    //! 경험치 상승 명령 Session->Agent
    struct NET_EVENT_EXP_SA : public NET_MSG_GENERIC
    {        
        float m_Scale;

        NET_EVENT_EXP_SA(float Scale)
            :m_Scale(Scale)
        {
            MIN_STATIC_ASSERT(sizeof(NET_EVENT_EXP_SA)<=NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_EVENT_EXP_MS);
            nType = NET_MSG_EVENT_EXP_SA;
        }
    };

	struct NET_GM_EVENT_EXP_CAF : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		float m_Scale;

		NET_GM_EVENT_EXP_CAF(DWORD ChaDbNum=0, float Scale=0)
            : m_Scale(Scale)            
		{
            dwChaNum = ChaDbNum;

			dwSize = sizeof(NET_GM_EVENT_EXP_CAF);
			nType = NET_MSG_GM_EVENT_EXP_CAF;
			MIN_STATIC_ASSERT(sizeof(NET_GM_EVENT_EXP_CAF)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_BEGIN : public NET_MSG_GENERIC
	{
		int			start_Lv;
		int			end_Lv;
		int			play_Time;
		int			buster_Time;
		float		expGain_Rate;
		float		itemGain_Rate;
		DWORD		dwEventMinute;


		SNET_GM_LIMIT_EVENT_BEGIN () :
		start_Lv(0),
			end_Lv(0),
			play_Time(0),
			buster_Time(0),
			expGain_Rate(0.0f),
			itemGain_Rate(0.0f),
			dwEventMinute(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_BEGIN;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_BEGIN)<=NET_DATA_BUFSIZE);
		}
	};

    //! 경험치 상승 종료 ServerManager->Session
    struct NET_EVENT_EXP_END_MS : public NET_MSG_GENERIC
    {
        int m_ServerGroup;

        NET_EVENT_EXP_END_MS(int ServerGroup)
            : m_ServerGroup(ServerGroup)
        {
            MIN_STATIC_ASSERT(sizeof(NET_EVENT_EXP_END_MS)<=NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_EVENT_EXP_END_MS);
            nType = NET_MSG_EVENT_EXP_END_MS;
        }
    };

    struct NET_EVENT_EXP_END_SA : public NET_MSG_GENERIC
    {
        NET_EVENT_EXP_END_SA()
        {
            MIN_STATIC_ASSERT(sizeof(NET_EVENT_EXP_END_SA)<=NET_DATA_BUFSIZE);

            dwSize = sizeof(NET_EVENT_EXP_END_SA);
            nType = NET_MSG_EVENT_EXP_END_SA;
        }
    };

	struct SNET_GM_LIMIT_EVENT_END : public NET_MSG_GENERIC
	{
		SNET_GM_LIMIT_EVENT_END () 
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_END;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_END)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CLASS_EVENT : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		BYTE				dwEventType; // 0 : EXP 1 : ITEM 2 : MONEY
		DWORD				dwClassIndex;
		float				fEventRate;
		bool				bStart;
		int					start_Lv;
		int					end_Lv;

		SNET_GM_CLASS_EVENT () :
		dwEventType(0),
			dwClassIndex(GLCC_NONE),
			fEventRate(0.0f),
			bStart(TRUE),
			start_Lv(0),
			end_Lv(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_CLASS_EVENT;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CLASS_EVENT)<=NET_DATA_BUFSIZE);
		}
	};

	struct  SNET_GM_LIMIT_EVENT_RESTART : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		__time64_t restartTime;

		SNET_GM_LIMIT_EVENT_RESTART() :
		restartTime(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_RESTART;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_RESTART)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_BEGIN_FB : public NET_MSG_GENERIC
	{
		int start_Lv;
		int end_Lv;
		int play_Time;
		int buster_Time;
		float expGain_Rate;
		float itemGain_Rate;


		SNET_GM_LIMIT_EVENT_BEGIN_FB () :
		start_Lv(0),
			end_Lv(0),
			play_Time(0),
			buster_Time(0),
			expGain_Rate(0.0f),
			itemGain_Rate(0.0f)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_BEGIN_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_BEGIN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_END_FB : public NET_MSG_GENERIC
	{
		SNET_GM_LIMIT_EVENT_END_FB ()		
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_END_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_END_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct  SNET_GM_LIMIT_EVENT_APPLY_START : public NET_MSG_GENERIC
	{
		__time64_t loginTime;   // 로그인 시간이나 이벤트 시작시 시간

		SNET_GM_LIMIT_EVENT_APPLY_START() :
		loginTime( 0 )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_APPLY_START;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_APPLY_START)<=NET_DATA_BUFSIZE);
		}
	};

	struct  SNET_GM_LIMIT_EVENT_APPLY_END : public NET_MSG_GENERIC
	{
		__time64_t loginTime;   // 로그인 시간이나 이벤트 시작시 시간

		SNET_GM_LIMIT_EVENT_APPLY_END() :
		loginTime( 0 )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_APPLY_END;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_APPLY_END)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNET_GM_LIMIT_EVENT_TIME_REQ : public NET_MSG_GENERIC
	{
		DWORD dwGaeaID;

		SNET_GM_LIMIT_EVENT_TIME_REQ()
            : dwGaeaID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_TIME_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_TIME_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_TIME_RESET : public NET_MSG_GENERIC
	{
		__time64_t		loginTime;
		DWORD			dwGaeaID;

		SNET_GM_LIMIT_EVENT_TIME_RESET() :
		dwGaeaID(0),
			loginTime(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_TIME_RESET;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_TIME_RESET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_LIMIT_EVENT_TIME_REQ_FB : public NET_MSG_GENERIC
	{
		bool bEventStart;       // 이벤트 시작 여부나 적용 대상이 아니면 FALSE 리턴
		__time64_t loginTime;   // 로그인 시간이나 이벤트 시작시 시간
		int play_Time;			// 이벤트 적용 시작 시간
		int buster_Time;		// 이벤트 적용 시간

		int start_Lv;
		int end_Lv;

		float expGain_Rate;
		float itemGain_Rate;


		SNET_GM_LIMIT_EVENT_TIME_REQ_FB() :
		bEventStart(FALSE),
			loginTime(0),
			play_Time(0),
			buster_Time(0),
			start_Lv(0),
			end_Lv(0),
			expGain_Rate(0.0f),
			itemGain_Rate(0.0f)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_LIMIT_EVENT_TIME_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_LIMIT_EVENT_TIME_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_GM_EVENT_EXP_END_CAF : public NET_MSG_CHARACTER
	{
		NET_GM_EVENT_EXP_END_CAF()
		{
			dwSize = sizeof(NET_GM_EVENT_EXP_END_CAF);
			nType = NET_MSG_GM_EVENT_EXP_END_CAF;
			MIN_STATIC_ASSERT(sizeof(NET_GM_EVENT_EXP_END_CAF)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_EXP_END_FB : public NET_MSG_GENERIC
	{
		WORD				wFIELDSVR;

		SNET_GM_EVENT_EXP_END_FB () :
		wFIELDSVR(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_EXP_END_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVENT_EXP_END_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_EX : public NET_MSG_GENERIC
	{
		EMGM_EVENT_TYPE		emType;
		WORD				wValue;

		SNET_GM_EVENT_EX () 
			: emType(EMGM_EVENT_NONE)
			, wValue(0)
		{
			dwSize = sizeof(SNET_GM_EVENT_EX);
			nType = NET_MSG_GM_EVENT_EX;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVENT_EX)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_EX_END : public NET_MSG_GENERIC
	{
		EMGM_EVENT_TYPE		emType;

		SNET_GM_EVENT_EX_END ()
			: emType(EMGM_EVENT_NONE)
		{
			dwSize = sizeof(SNET_GM_EVENT_EX_END);
			nType = NET_MSG_GM_EVENT_EX_END;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVENT_EX_END)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_ITEM_GEN : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		float				fRATE;

		SNET_GM_EVEN_ITEM_GEN () :
		fRATE(1)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_ITEM_GEN;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVEN_ITEM_GEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_ITEM_GEN_FB : public NET_MSG_GENERIC
	{
		float				fRATE;

		SNET_GM_EVEN_ITEM_GEN_FB () :
		fRATE(1)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_ITEM_GEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVEN_ITEM_GEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_ITEM_GEN_END : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{

		SNET_GM_EVEN_ITEM_GEN_END ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_ITEM_GEN_END;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVEN_ITEM_GEN_END)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_ITEM_GEN_END_FB : public NET_MSG_GENERIC
	{
		SNET_GM_EVEN_ITEM_GEN_END_FB ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_ITEM_GEN_END_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVEN_ITEM_GEN_END_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_GRADE : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		float				fRATE;

		SNET_GM_EVENT_GRADE () :
		fRATE(1)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_GRADE;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVENT_GRADE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_GRADE_FB : public NET_MSG_GENERIC
	{
		float				fRATE;

		SNET_GM_EVENT_GRADE_FB () :
		fRATE(1)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_GRADE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVENT_GRADE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_GRADE_END : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNET_GM_EVENT_GRADE_END ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_GRADE_END;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVENT_GRADE_END)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVENT_GRADE_END_FB : public NET_MSG_GENERIC
	{
		SNET_GM_EVENT_GRADE_END_FB ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_GRADE_END_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVENT_GRADE_END_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_GM_EVEN_MONEY_GEN : public NET_MSG_GENERIC
	{
		float				fRATE;

		SNET_GM_EVEN_MONEY_GEN () :
		fRATE(1)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_MONEY_GEN;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVEN_MONEY_GEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_EVEN_MONEY_GEN_END : public NET_MSG_GENERIC
	{
		SNET_GM_EVEN_MONEY_GEN_END ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_EVENT_MONEY_GEN_END;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_EVEN_MONEY_GEN_END)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SERVER_CLOSE_FIELD : NET_MSG_GENERIC
	{
		DWORD dwFieldChannel;
		DWORD dwFieldID;

		SNET_SERVER_CLOSE_FIELD(const DWORD _dwFieldChannel, const DWORD _dwFieldID)
			: dwFieldChannel(_dwFieldChannel)
			, dwFieldID(_dwFieldID)
		{
			dwSize = sizeof(SNET_SERVER_CLOSE_FIELD);
			nType = NET_MSG_SERVER_CLOSE_FIELD;
		}
	};

	struct SNET_SERVER_CLOSE_AGENT : NET_MSG_GENERIC
	{
		DWORD dwAgentID;

		SNET_SERVER_CLOSE_AGENT ( const DWORD _dwAgentID )
			: dwAgentID( _dwAgentID )
		{
			dwSize = sizeof( SNET_SERVER_CLOSE_AGENT );
			nType = NET_MSG_SERVER_CLOSE_AGENT;
		}
	};

	struct SNET_SERVER_GENERALCHAT //: public NET_MSG_GENERIC
	{
		//DWORD dwLENGTH;
		//enum { EMMAX_TEXT = NET_DATA_BUFSIZE-sizeof(NET_MSG_GENERIC)-sizeof(DWORD)-1 };
        enum { EMMAX_TEXT = 512, };
        std::string m_ChatMsg; //[EMMAX_TEXT];

        MSGPACK_DEFINE(m_ChatMsg);

		SNET_SERVER_GENERALCHAT()
            //: dwLENGTH(0)
		{
			//dwSize = sizeof(SNET_SERVER_GENERALCHAT);
			//nType = NET_MSG_SERVER_GENERALCHAT;
			//MIN_STATIC_ASSERT(sizeof(SNET_SERVER_GENERALCHAT)<=NET_DATA_BUFSIZE);
			//memset(szTEXT, 0, sizeof(char) * EMMAX_TEXT);
		}

		bool SETTEXT(const char* szTemp)
		{
			if (szTemp)
				return false;
            else
                return SETTEXT(std::string(szTemp));
		}

        bool SETTEXT(const std::string& SendMsg)
        {
            if (SendMsg.empty())
                return false;

            if (SendMsg.length() < EMMAX_TEXT)
                m_ChatMsg = SendMsg;
            return true;
        }
	};

	struct SNET_CYBERCAFECLASS_UPDATE : public NET_MSG_GENERIC
	{
        EMIP_BONUS_CLASS	emIPBonus;

		SNET_CYBERCAFECLASS_UPDATE()
			: emIPBonus(EMIP_BONUS_NONE)
		{
			dwSize = sizeof(SNET_CYBERCAFECLASS_UPDATE);
			nType = NET_MSG_CYBERCAFECLASS_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNET_CYBERCAFECLASS_UPDATE)<=NET_DATA_BUFSIZE);
		}

	};


	struct SNET_SERVER_PLAYERKILLING_MODE : public NET_MSG_GENERIC
	{
		BOOL				bMODE;

		SNET_SERVER_PLAYERKILLING_MODE () :
		bMODE(FALSE)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_SERVER_PLAYERKILLING_MODE;
			MIN_STATIC_ASSERT(sizeof(SNET_SERVER_PLAYERKILLING_MODE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_NONCONFRONT_MODE : public NET_MSG_GENERIC
	{
		BOOL				bMode;
		SNATIVEID			sMapID;
		INT32				nChannel;
		DWORD				dwPeriod;
		BOOL				bThisChannel;

		SNET_GM_NONCONFRONT_MODE () 
			: bMode		   ( FALSE )
			, sMapID	   ( NATIVEID_NULL() )
			, dwPeriod	   ( 0 )
			, nChannel	   ( 0 )
			, bThisChannel ( FALSE )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_NONCONFRONT_MODE;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_NONCONFRONT_MODE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_NONCONFRONT_MODE_VIEW_REQ : public NET_MSG_GENERIC
	{
		SNET_GM_NONCONFRONT_MODE_VIEW_REQ ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_NONCONFRONT_MODE_VIEW_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_NONCONFRONT_MODE_VIEW_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_NONCONFRONT_MODE_VIEW_FB : public NET_MSG_GENERIC
	{
		struct SVIEWINFO
		{
			SNATIVEID sMAPID;
			INT32	  nCHANNEL;
			DWORD	  dwAPPLYTIME;
			DWORD	  dwREMAINEDTIME;

			SVIEWINFO() 
			{
				sMAPID		   = NATIVEID_NULL();
				nCHANNEL	   = 0;
				dwAPPLYTIME	   = 0;
				dwREMAINEDTIME = 0;
			}

		} sInfo[GM_NONCONFRONT_MODE_VIEW_MAX];

		WORD wSize;

		SNET_GM_NONCONFRONT_MODE_VIEW_FB ()
			: wSize(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_NONCONFRONT_MODE_VIEW_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_NONCONFRONT_MODE_VIEW_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_PRINT_CROWLIST : public NET_MSG_GENERIC
	{
        BYTE				dwFindMob;

		SNET_GM_PRINT_CROWLIST () :
		dwFindMob( 0 )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_PRINT_CROWLIST;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_PRINT_CROWLIST)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_PRINT_CROWLIST_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		BYTE				dwFindMob;

		SNET_GM_PRINT_CROWLIST_FLD () :
		dwGaeaID(0),
			dwFindMob( 0 )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_PRINT_CROWLIST_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_PRINT_CROWLIST_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_PRINT_CROWLIST_FB : public NET_MSG_GENERIC
	{
		// crow의 MID, SID
		WORD				mID;
		WORD				sID;
		WORD				wNum;
		// MOB인지 NPC인지
		EMCROW				emCrow;

		SNET_GM_PRINT_CROWLIST_FB ()
			: mID(0)
			, sID(0)
			, wNum(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_PRINT_CROWLIST_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_PRINT_CROWLIST_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_NPC : public NET_MSG_GENERIC
	{
		SNATIVEID			nidNPC;

		SNET_GM_WHERE_NPC () :
		nidNPC(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WHERE_NPC;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_NPC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_NPC_FLD : public NET_MSG_GENERIC
	{	
		SNATIVEID			nidNPC;
		DWORD				dwGaeaID;

		SNET_GM_WHERE_NPC_FLD () :
		nidNPC(false),
			dwGaeaID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WHERE_NPC_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_NPC_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_NPC_FB : public NET_MSG_GENERIC
	{	
		int					nPosX;
		int					nPosY;

		SNET_GM_WHERE_NPC_FB () 
			: nPosX(0)
			, nPosY(0)
		{
			dwSize = sizeof(SNET_GM_WHERE_NPC_FB);
			nType = NET_MSG_GM_WHERE_NPC_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_NPC_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_NPC_ALL : public NET_MSG_GENERIC
	{
		SNET_GM_WHERE_NPC_ALL ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WHERE_NPC_ALL;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_NPC_ALL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_NPC_ALL_FLD : public NET_MSG_GENERIC
	{	
		DWORD				dwGaeaID;

		SNET_GM_WHERE_NPC_ALL_FLD () :
		dwGaeaID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WHERE_NPC_ALL_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_NPC_ALL_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_NPC_ALL_FB : public NET_MSG_GENERIC
	{	
		SNATIVEID			nidNPC;
		int					nPosX;
		int					nPosY;

		SNET_GM_WHERE_NPC_ALL_FB () 
			: nidNPC(false)
			, nPosX(0)
			, nPosY(0)
		{
			dwSize = sizeof(SNET_GM_WHERE_NPC_ALL_FB);
			nType = NET_MSG_GM_WHERE_NPC_ALL_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_NPC_ALL_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_GM_WHERE_PC_MAP : public NET_MSG_GENERIC
	{	
		DWORD				dwCHARID;

		SNET_GM_WHERE_PC_MAP () :
		dwCHARID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WHERE_PC_MAP;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_PC_MAP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_PC_MAP_FB : public NET_MSG_GENERIC
	{	
		DWORD				dwChannel;
		SNATIVEID			nidMAP;

		SNET_GM_WHERE_PC_MAP_FB () :
		dwChannel(0),
			nidMAP(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WHERE_PC_MAP_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_PC_MAP_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_PC_POS : public NET_MSG_GENERIC
	{	
		DWORD				dwCHARID;

		SNET_GM_WHERE_PC_POS () :
		dwCHARID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WHERE_PC_POS;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_PC_POS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WHERE_PC_POS_FB : public NET_MSG_GENERIC
	{	
		bool				bFOUND;
		int					nPosX;
		int					nPosY;

		SNET_GM_WHERE_PC_POS_FB () :
		bFOUND(false),
			nPosX(0),
			nPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WHERE_PC_POS_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WHERE_PC_POS_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_BIGHEAD : public NET_MSG_GENERIC
	{	
		bool				bBIGHEAD;
		float				fSize;

		SNET_GM_BIGHEAD () :
		bBIGHEAD(false),fSize(4.0f)

		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_BIGHEAD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_BIGHEAD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_BIGHEAD_BRD : public NET_MSG_GENERIC
	{	
		bool				bBIGHEAD;
		float				fSize;

		SNET_GM_BIGHEAD_BRD () :
		bBIGHEAD(false),fSize(4.0f)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_BIGHEAD_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_BIGHEAD_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_BIGHAND : public NET_MSG_GENERIC
	{	
		bool				bBIGHAND;
		float				fSize;

		SNET_GM_BIGHAND () :
		bBIGHAND(false),fSize(2.0f)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_BIGHAND;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_BIGHAND)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_BIGHAND_BRD : public NET_MSG_GENERIC
	{	
		bool				bBIGHAND;
		float				fSize;

		SNET_GM_BIGHAND_BRD () :
		bBIGHAND(false),fSize(2.0f)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_BIGHAND_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_BIGHAND_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_FREEPK : public NET_MSG_GENERIC
	{	
		DWORD				dwPKTIME;

		SNET_GM_FREEPK () :
		dwPKTIME(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_FREEPK;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_FREEPK)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_FREEPK_BRD : public NET_MSG_GENERIC
	{	
		DWORD				dwPKTIME;
		bool				bSTATEUPDATE;

		SNET_GM_FREEPK_BRD () :
		dwPKTIME(0),
			bSTATEUPDATE(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_FREEPK_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_FREEPK_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_FREEPK_GM_COMMAND_CA : public NET_MSG_GENERIC
	{	
		bool				bBool;
		SNET_FREEPK_GM_COMMAND_CA () :
		bBool(0)
		{
			dwSize = sizeof(*this);
			nType = NET_FREEPK_GM_COMMAND_CA;
			MIN_STATIC_ASSERT(sizeof(SNET_FREEPK_GM_COMMAND_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_VIEWALLPLAYER : public NET_MSG_GENERIC
	{
		SNET_GM_VIEWALLPLAYER () 
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_VIEWALLPLAYER;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_VIEWALLPLAYER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_SHOP_INFO_REQ : public NET_MSG_GENERIC
	{	
		bool				bBasicInfo;
		SNATIVEID			sSearchItemID;

		SNET_GM_SHOP_INFO_REQ () 
			: sSearchItemID(NATIVEID_NULL())
			, bBasicInfo(TRUE)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_SHOP_INFO_REQ;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_SHOP_INFO_REQ)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_GM_SHOP_INFO_FB : public NET_MSG_GENERIC
	{
		enum { FIRST, MIDDLE, END };

		
		char szBasicInfo[MAX_CHAR_LENGTH];
		bool bBasicInfo;
		BYTE bSTATE;

		SNET_GM_SHOP_INFO_FB () 
			: bBasicInfo(TRUE)
			, bSTATE(MIDDLE)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_SHOP_INFO_FB;
			memset(szBasicInfo, 0, sizeof(char) * (MAX_CHAR_LENGTH));
            MIN_STATIC_ASSERT(sizeof(SNET_GM_SHOP_INFO_FB)<=NET_DATA_BUFSIZE);
		}

        void SetData(const std::string& MsgInfo)
        {
            StringCchCopy(szBasicInfo, MAX_CHAR_LENGTH, MsgInfo.c_str());
        }
	};


	struct SNET_GM_VIEWWORKEVENT : public NET_MSG_GENERIC
	{
		SNET_GM_VIEWWORKEVENT () 
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_VIEWWORKEVENT;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_VIEWWORKEVENT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_VIEWWORKEVENT_FB : public NET_MSG_GENERIC
	{
        enum { INFO_SIZE = 256, };
		char szWorkEventInfo[INFO_SIZE];

		SNET_GM_VIEWWORKEVENT_FB() 
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_VIEWWORKEVENT_FB;
			memset(szWorkEventInfo, 0, sizeof(CHAR) * INFO_SIZE);
            MIN_STATIC_ASSERT(sizeof(SNET_GM_VIEWWORKEVENT_FB)<=NET_DATA_BUFSIZE);
		}

        void SetData(const std::string& Msg)
        {
            StringCchCopy(szWorkEventInfo, INFO_SIZE, Msg.c_str());
        }
	};

	struct SNET_GM_VIEWALLPLAYER_FLD_REQ : public NET_MSG_GENERIC
	{	
		DWORD				dwID;

		SNET_GM_VIEWALLPLAYER_FLD_REQ ()
			: dwID(0)
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_VIEWALLPLAYER_FLD_REQ;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_VIEWALLPLAYER_FLD_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_VIEWALLPLAYER_FLD_FB : public NET_MSG_GENERIC
	{	
		DWORD				dwCHARID;
		char				szCHARNAME[CHAR_SZNAME];
		DWORD				dwPlayerNum;


		SNET_GM_VIEWALLPLAYER_FLD_FB ()
			: dwCHARID(0),
			dwPlayerNum(0)
		{
			dwSize = sizeof(SNET_GM_VIEWALLPLAYER_FLD_FB);
			nType  = NET_MSG_GM_VIEWALLPLAYER_FLD_FB;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));		
                        MIN_STATIC_ASSERT(sizeof(SNET_GM_VIEWALLPLAYER_FLD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WARNING_MSG : public NET_MSG_GENERIC
	{	
		DWORD				dwGaeaID;
		bool				bOn;

		SNET_GM_WARNING_MSG () :
		dwGaeaID(0),
			bOn(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WARNING_MSG;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WARNING_MSG)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WARNING_MSG_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		bool				bOn;

		SNET_GM_WARNING_MSG_FLD () :
		dwGaeaID(0),
			bOn(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WARNING_MSG_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WARNING_MSG_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_WARNING_MSG_BRD : public NET_MSG_GENERIC
	{
		bool				bOn;

		SNET_GM_WARNING_MSG_BRD () :
		bOn(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_WARNING_MSG_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_WARNING_MSG_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_COUNTDOWN_MSG_BRD : public NET_MSG_GENERIC
	{
		INT					nCount;

		SNET_GM_COUNTDOWN_MSG_BRD () :
		nCount(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_COUNTDOWN_MSG_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_COUNTDOWN_MSG_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_COUNTRECORDER_INFO_REQ : public NET_MSG_GENERIC
	{
		enum {
			EMCOUNTRECORDER_DROPOUTCROW_CNT = 0,
			EMCOUNTRECORDER_DROPITEM_CNT,
			EMCOUNTRECORDER_MONEY_CNT,
			EMCOUNTRECORDER_ENTERPLAYER_CNT,
			EMCOUNTRECORDER_ELAPSEDTIME,
			EMCOUNTRECORDER_RESET,
		};
		DWORD		dwFlag;
		SNATIVEID	sMapID;
		DWORD		dwGaeaID;
		SNET_GM_COUNTRECORDER_INFO_REQ ()
			: dwFlag(-1)
			, sMapID(SNATIVEID(false))
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_COUNTRECORDER_INFO_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_COUNTRECORDER_INFO_REQ )<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_COUNTRECORDER_INFO_FB : public NET_MSG_GENERIC
	{
		enum {
			EMCOUNTRECORDER_DROPOUTCROW_CNT = 0,
			EMCOUNTRECORDER_DROPITEM_CNT,
			EMCOUNTRECORDER_MONEY_CNT,
			EMCOUNTRECORDER_ENTERPLAYER_CNT,
			EMCOUNTRECORDER_ELAPSEDTIME,
			EMCOUNTRECORDER_RESET_SOK,
		};
		DWORD		dwReqFlag;
		DWORD		dwGaeaID;
		SNATIVEID	sTargetNID;
		long long	lCount;
		SNET_GM_COUNTRECORDER_INFO_FB ()
			: dwReqFlag(-1)
			, sTargetNID(SNATIVEID(false))
			, lCount(0L)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_COUNTRECORDER_INFO_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_COUNTRECORDER_INFO_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_GEN : public NET_MSG_GENERIC
	{
		WORD				wMainID;
		WORD				wSubID;

		WORD				wPosX;
		WORD				wPosY;

		SNET_GM_MOB_GEN () :
		wMainID(0),
			wSubID(0),

			wPosX(0),
			wPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_GEN;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_GEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_GEN_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		WORD				wMainID;
		WORD				wSubID;

		WORD				wPosX;
		WORD				wPosY;

		SNET_GM_MOB_GEN_FLD () :
		dwGaeaID(0),
			wMainID(0),
			wSubID(0),

			wPosX(0),
			wPosY(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_GEN_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_GEN_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_GEN_EX : public NET_MSG_GENERIC
	{
		WORD				wMobMID;
		WORD				wMobSID;
		WORD				wMapMID;
		WORD				wMapSID;
		WORD				wPosX;
		WORD				wPosY;
		WORD				wRange;
		WORD				wNum;
		BOOL				bThisChannel;

		SNET_GM_MOB_GEN_EX () :
		wMobMID(0),
			wMobSID(0),
			wMapMID(0),
			wMapSID(0),
			wPosX(0),
			wPosY(0),
			wRange(0),
			wNum(0),
			bThisChannel(FALSE)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_GEN_EX;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_GEN_EX)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_GEN_EX_FLD : public NET_MSG_GENERIC
	{
		WORD				wMobMID;
		WORD				wMobSID;
		WORD				wMapMID;
		WORD				wMapSID;
		WORD				wPosX;
		WORD				wPosY;
		WORD				wRange;
		WORD				wNum;

		DWORD				dwGaeaID;

		SNET_GM_MOB_GEN_EX_FLD () :
		wMobMID(0),
			wMobSID(0),
			wMapMID(0),
			wMapSID(0),
			wPosX(0),
			wPosY(0),
			wRange(0),
			wNum(0),
			dwGaeaID(-1)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_GEN_EX_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_GEN_EX_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_DEL_EX : public NET_MSG_GENERIC
	{
		WORD				wMobMID;
		WORD				wMobSID;
		WORD				wMapMID;
		WORD				wMapSID;
		BOOL				bThisChannel;

		SNET_GM_MOB_DEL_EX () :
		wMobMID(0),
			wMobSID(0),
			wMapMID(0),
			wMapSID(0),
			bThisChannel(FALSE)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_DEL_EX;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_DEL_EX)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_DEL_EX_FLD : public NET_MSG_GENERIC
	{
		WORD				wMobMID;
		WORD				wMobSID;
		WORD				wMapMID;
		WORD				wMapSID;

		SNET_GM_MOB_DEL_EX_FLD () :
		wMobMID(0),
			wMobSID(0),
			wMapMID(0),
			wMapSID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_DEL_EX_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_DEL_EX_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_DEL : public NET_MSG_GENERIC
	{	
		bool				bMaterial;
		SNATIVEID			sMobID;

		SNET_GM_MOB_DEL () :
		sMobID( false ),
			bMaterial(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_DEL;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_DEL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_DEL_FLD : public NET_MSG_GENERIC
	{	
		bool				bMaterial;
		SNATIVEID			sMobID;

		DWORD				dwGaeaID;

		SNET_GM_MOB_DEL_FLD () :
		dwGaeaID(0),
			sMobID( false ),
			bMaterial(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_DEL_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_DEL_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_LEV : public NET_MSG_GENERIC
	{
		enum { FILE_NAME = 128, };

		WORD				wMAP_MID;
		WORD				wMAP_SID;

		DWORD				dwLayer;

		SNET_GM_MOB_LEV(void)
			: wMAP_MID(USHRT_MAX)
			, wMAP_SID(USHRT_MAX)
			, dwLayer(UINT_MAX)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_LEVEL;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_LEV)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_LEV_LAYER : public NET_MSG_GENERIC
	{
		WORD				wMAP_MID;
		WORD				wMAP_SID;
		WORD				unLayer;

		SNET_GM_MOB_LEV_LAYER () :
			wMAP_MID(USHRT_MAX)
		,	wMAP_SID(USHRT_MAX)
		,	unLayer(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_LEVEL_LAYER;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_LEV_LAYER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_LEV_CLEAR : public NET_MSG_GENERIC
	{
		WORD				wMAP_MID;
		WORD				wMAP_SID;

		SNET_GM_MOB_LEV_CLEAR () :
		wMAP_MID(USHRT_MAX),
			wMAP_SID(USHRT_MAX)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_LEVEL_CLEAR;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_LEV_CLEAR)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_LEV_LIST : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		WORD				wMAP_MID;
		WORD				wMAP_SID;

		SNET_GM_MOB_LEV_LIST () 
			: dwGaeaID(GAEAID_NULL)
			, wMAP_MID(USHRT_MAX)
			, wMAP_SID(USHRT_MAX)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_MOB_LEVEL_LIST;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_LEV_LIST)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_MOB_LEV_LIST_FC : public NET_MSG_GENERIC
	{
		DWORD nLayer;
		WORD wMAP_MID;
		WORD wMAP_SID;

		SNET_GM_MOB_LEV_LIST_FC ()
			: nLayer(0)
			, wMAP_MID(0)
			, wMAP_SID(0)
		{
			dwSize = sizeof(SNET_GM_MOB_LEV_LIST_FC);
			nType = NET_MSG_GM_MOB_LEVEL_LIST_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_MOB_LEV_LIST_FC) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CHAT_BLOCK_UACCOUNT : public NET_MSG_GENERIC
	{
		char				szUACCOUNT[USR_ID_LENGTH+1];
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_UACCOUNT () 
			: dwBLOCK_MINUTE(0)
		{
			dwSize = sizeof(SNET_GM_CHAT_BLOCK_UACCOUNT);
			nType = NET_MSG_GM_CHAT_BLOCK_UACCOUNT;
			memset(szUACCOUNT, 0, sizeof(char) * (USR_ID_LENGTH+1));
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAT_BLOCK_UACCOUNT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CHAT_BLOCK_CHARNAME : public NET_MSG_GENERIC
	{
		char				szCHARNAME[CHAR_SZNAME];
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_CHARNAME () :
		dwBLOCK_MINUTE(0)
		{
			dwSize = sizeof(SNET_GM_CHAT_BLOCK_CHARNAME);
			nType = NET_MSG_GM_CHAT_BLOCK_CHARNAME;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAT_BLOCK_CHARNAME)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAT_BLOCK_CHARID : public NET_MSG_GENERIC
	{
		DWORD				dwCHARID;
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_CHARID () :
		dwCHARID(0),
			dwBLOCK_MINUTE(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_CHAT_BLOCK_CHARID;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAT_BLOCK_CHARID)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CHAT_BLOCK_FB : public NET_MSG_GENERIC
	{
		bool				bBLOCK;
		char				szUACCOUNT[USR_ID_LENGTH+1];
		char				szCHARNAME[CHAR_SZNAME];
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_FB () 
			: bBLOCK(false)
			, dwBLOCK_MINUTE(0)
		{
			dwSize = sizeof(SNET_GM_CHAT_BLOCK_FB);
			nType = NET_MSG_GM_CHAT_BLOCK_FB;
			memset(szUACCOUNT, 0, sizeof(char) * (USR_ID_LENGTH+1));
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAT_BLOCK_FB)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAT_BLOCK_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwCHARID;
		DWORD				dwBLOCK_MINUTE;

		SNET_GM_CHAT_BLOCK_FLD () :
		dwCHARID(0),
			dwBLOCK_MINUTE(0)
		{
			dwSize = sizeof(SNET_GM_CHAT_BLOCK_FLD);
			nType = NET_MSG_GM_CHAT_BLOCK_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAT_BLOCK_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	/**
		GM이 해당 유저를 채팅금지 시킬 경우
		해당 유저에게 채팅차단상태, 시간을 알려주기 위한 패킷
	*/
	struct SNET_GM_CHAT_BLOCK_NOTIFY : public NET_MSG_GENERIC
	{
		__time64_t	tChatBlock;	        // 채팅 차단 해제 시간
		DWORD		dwBLOCK_MINUTE;		// 채팅 차단 해제 시간

		SNET_GM_CHAT_BLOCK_NOTIFY() 
		{
			dwSize = sizeof(SNET_GM_CHAT_BLOCK_NOTIFY);
			nType = NET_MSG_GM_CHAT_BLOCK_NOTIFY;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAT_BLOCK_NOTIFY)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_USER_CHAR_INFO_4NAME : public NET_MSG_GENERIC
	{	
		char				szCHARNAME[CHAR_SZNAME];

		SNET_USER_CHAR_INFO_4NAME ()
		{
			dwSize = sizeof(SNET_USER_CHAR_INFO_4NAME);
			nType = NET_MSG_USER_CHAR_INFO_4NAME;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			MIN_STATIC_ASSERT(sizeof(SNET_USER_CHAR_INFO_4NAME)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAR_INFO_4NAME : public NET_MSG_GENERIC
	{	
		char				szCHARNAME[CHAR_SZNAME];

		SNET_GM_CHAR_INFO_4NAME ()
		{
			dwSize = sizeof(SNET_GM_CHAR_INFO_4NAME);
			nType = NET_MSG_GM_CHAR_INFO_4NAME;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAR_INFO_4NAME)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAR_INFO_4CHARID : public NET_MSG_GENERIC
	{	
		DWORD				dwCHARID;

		SNET_GM_CHAR_INFO_4CHARID () :
		dwCHARID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_CHAR_INFO_4CHARID;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAR_INFO_4CHARID)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_USER_CHAR_INFO_FLD : public NET_MSG_GENERIC
	{	
		DWORD				dwChaNumRequest;	//요청한 캐릭터
		DWORD				dwCHARID;

		SNET_USER_CHAR_INFO_FLD ()
			:dwChaNumRequest(0)
			,dwCHARID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_USER_CHAR_INFO_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_USER_CHAR_INFO_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_CHAR_INFO_FLD : public NET_MSG_GENERIC
	{	
		DWORD				dwChaNumRequest;	//요청한 캐릭터
		DWORD				dwCHARID;

		SNET_GM_CHAR_INFO_FLD ()
			:dwChaNumRequest(0)
			,dwCHARID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_CHAR_INFO_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAR_INFO_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_USER_CHAR_INFO_AGT_FB : public NET_MSG_GENERIC
	{
		char				szCHARNAME[CHAR_SZNAME];

		SNET_USER_CHAR_INFO_AGT_FB () 
		{
			dwSize = sizeof(SNET_USER_CHAR_INFO_AGT_FB);
			nType = NET_MSG_USER_CHAR_INFO_AGT_FB;
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			MIN_STATIC_ASSERT(sizeof(SNET_USER_CHAR_INFO_AGT_FB)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAR_INFO_AGT_FB : public NET_MSG_GENERIC
	{
		DWORD				dwCHARID;
		char				szUACCOUNT[USR_ID_LENGTH+1];
		char				szCHARNAME[CHAR_SZNAME];
		DWORD				dwSERVER;
		DWORD				dwCHANNEL;
		DWORD				dwGAEAID;

		SNET_GM_CHAR_INFO_AGT_FB () 
			: dwCHARID(0)
			, dwSERVER(0)
			, dwCHANNEL(0)
			, dwGAEAID(0)
		{
			dwSize = sizeof(SNET_GM_CHAR_INFO_AGT_FB);
			nType = NET_MSG_GM_CHAR_INFO_AGT_FB;
			memset(szUACCOUNT, 0, sizeof(char) * (USR_ID_LENGTH+1));
			memset(szCHARNAME, 0, sizeof(char) * (CHAR_SZNAME));
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAR_INFO_AGT_FB)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_USER_CHAR_INFO_FLD_FB : public NET_MSG_GENERIC
	{	
		DWORD				dwChaNumRequest; //요청한 캐릭터 (응답을 받을 주체)
		char				szCLUB[CHAR_SZNAME];

		SNET_USER_CHAR_INFO_FLD_FB () 
			: dwChaNumRequest(0)
		{
			dwSize = sizeof(SNET_USER_CHAR_INFO_FLD_FB);
			nType = NET_MSG_USER_CHAR_INFO_FLD_FB;
			memset(szCLUB, 0, sizeof(char) * (CHAR_SZNAME));
			MIN_STATIC_ASSERT(sizeof(SNET_USER_CHAR_INFO_FLD_FB)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_GM_CHAR_INFO_FLD_FB : public NET_MSG_GENERIC
	{		
		DWORD				dwChaNumRequest;	//요청한 캐릭터 (응답을 받을 주체)

		DWORD				dwCHARID;
		SNATIVEID			nidMAP;
		WORD				wPOSX;
		WORD				wPOSY;

		WORD				wLEVEL;
		GLDWDATA			sHP;
		GLDWDATA			sMP;
		GLDWDATA			sSP;
		GLLLDATA			sEXP;
		char				szCLUB[CHAR_SZNAME];

		SNET_GM_CHAR_INFO_FLD_FB () 
			: dwChaNumRequest(0)
			, dwCHARID(0)
			, wPOSX(0)
			, wPOSY(0)
			, wLEVEL(0)
		{
			dwSize = sizeof(SNET_GM_CHAR_INFO_FLD_FB);
			nType = NET_MSG_GM_CHAR_INFO_FLD_FB;
			memset(szCLUB, 0, sizeof(char) * (CHAR_SZNAME));
			MIN_STATIC_ASSERT(sizeof(SNET_GM_CHAR_INFO_FLD_FB)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNET_LEVEL_EVENT_END_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwMapID;
		DWORD				dwGateID;
		DWORD				dwHallMapID;
		GLLEV_POS			levPos[MAX_EVENT_POS];

		SNET_LEVEL_EVENT_END_FLD () 
			: dwMapID(0)
			, dwGateID(0)
			, dwHallMapID(0)
		{
			dwSize = sizeof(SNET_LEVEL_EVENT_END_FLD);
			nType = NET_MSG_GCTRL_LEVEL_EVENT_END_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_LEVEL_EVENT_END_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_LEVEL_EVENT_WARNING_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwMapID;
		bool				bOn;

		SNET_LEVEL_EVENT_WARNING_FLD ()
			: dwMapID(0)
			, bOn(false)
		{
			dwSize = sizeof(SNET_LEVEL_EVENT_WARNING_FLD);
			nType = NET_MSG_GCTRL_LEVEL_EVENT_WARNING_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_LEVEL_EVENT_WARNING_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_LEVEL_EVENT_COUNTDOWN_FLD : public NET_MSG_GENERIC
	{
		DWORD				dwMapID;
		INT					nCount;

		SNET_LEVEL_EVENT_COUNTDOWN_FLD ()
			: dwMapID(0)
			, nCount(0)
		{
			dwSize = sizeof(SNET_LEVEL_EVENT_COUNTDOWN_FLD);
			nType = NET_MSG_GCTRL_LEVEL_EVENT_COUNTDOWN_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_LEVEL_EVENT_COUNTDOWN_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SERVER_LOGINMAINTENANCE_AGT : public NET_MSG_GENERIC
	{	
		bool				bOn;
		SNET_SERVER_LOGINMAINTENANCE_AGT ()
			: bOn( false )
		{
			dwSize = sizeof( SNET_SERVER_LOGINMAINTENANCE_AGT );
			nType = NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_AGT;
			MIN_STATIC_ASSERT(sizeof(SNET_SERVER_LOGINMAINTENANCE_AGT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_DUPLICATE_CLOSEGAME : public NET_MSG_GENERIC
	{
		DWORD				dwID;

		SNET_DUPLICATE_CLOSEGAME () :
		dwID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_DUPLICATE_CLOSEGAME;
			MIN_STATIC_ASSERT(sizeof(SNET_DUPLICATE_CLOSEGAME)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//mjeon.Add
	//Ping packet
	struct SNET_GM_PING : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		DWORD				dwSent;			//Ping을 보낸 쪽에서 Ping을 보낸 시간 저장		
		DWORD				dwRecvd;		//Ping을 받은 쪽에서 Ping을 받은 시간 저장 (or Pong을 보낸시간)
		//DWORD				dwArrvd;		//Ping을 보낸 쪽에서 Pong을 받은 시간 저장 (따로 패킷에 저장할 필요는 없음)
		BYTE				bServerType;	//0: AgentServer	1: FieldServer

		SNET_GM_PING () :
		dwSent(0),
			dwRecvd(0),
			bServerType(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GM_PING;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_PING)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//mjeon.Add
	//Packets for PINGTRACE command
	//
	struct SNET_GM_PINGTRACE_ON : public NET_MSG_GENERIC
	{
		SNET_GM_PINGTRACE_ON () 
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_PINGTRACE_ON;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_PINGTRACE_ON)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_PINGTRACE_ON_FB : public NET_MSG_GENERIC
	{
		SNET_GM_PINGTRACE_ON_FB () 
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_PINGTRACE_ON_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_PINGTRACE_ON_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_PINGTRACE_OFF : public NET_MSG_GENERIC
	{
		SNET_GM_PINGTRACE_OFF () 
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_PINGTRACE_OFF;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_PINGTRACE_OFF)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_PINGTRACE_OFF_FB : public NET_MSG_GENERIC
	{
		SNET_GM_PINGTRACE_OFF_FB () 
		{
			dwSize = sizeof(*this);
			nType  = NET_MSG_GM_PINGTRACE_OFF_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_PINGTRACE_OFF_FB)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNET_GM_WHISPER_STATE : public NET_MSG_GENERIC
    {
        bool bOn;

        SNET_GM_WHISPER_STATE (bool _bOn) : bOn (_bOn)
        {
            dwSize = sizeof(*this);
            nType  = NET_MSG_GM_WHISPER_STATE;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_WHISPER_STATE)<=NET_DATA_BUFSIZE);			
        }
    };

    struct SNET_GM_WHISPER_STATE_FB : public NET_MSG_GENERIC
    {
        bool bOn;

        SNET_GM_WHISPER_STATE_FB (bool _bOn) : bOn (_bOn)
        {
            dwSize = sizeof(*this);
            nType  = NET_MSG_GM_WHISPER_STATE_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_GM_WHISPER_STATE_FB)<=NET_DATA_BUFSIZE);
        }
    };

    //! Client->Agent
    struct NET_GM_LOG_ITEM_RELOAD_CA : public NET_MSG_GENERIC
    {
        NET_GM_LOG_ITEM_RELOAD_CA()
        {
            dwSize = sizeof(NET_GM_LOG_ITEM_RELOAD_CA);
            nType = NET_MSG_GM_LOG_ITEM_RELOAD_CA;
            MIN_STATIC_ASSERT(sizeof(NET_GM_LOG_ITEM_RELOAD_CA)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent->Session
    struct NET_GM_LOG_ITEM_RELOAD_AS : public NET_MSG_GENERIC
    {
        NET_GM_LOG_ITEM_RELOAD_AS()
        {
            dwSize = sizeof(NET_GM_LOG_ITEM_RELOAD_AS);
            nType = NET_MSG_GM_LOG_ITEM_RELOAD_AS;
            MIN_STATIC_ASSERT(sizeof(NET_GM_LOG_ITEM_RELOAD_AS)<=NET_DATA_BUFSIZE);
        }
    };

    //! Session->All server
    struct NET_GM_LOG_ITEM_RELOAD_SS : public NET_MSG_GENERIC
    {
        NET_GM_LOG_ITEM_RELOAD_SS()
        {
            dwSize = sizeof(NET_GM_LOG_ITEM_RELOAD_SS);
            nType = NET_MSG_GM_LOG_ITEM_RELOAD_SS;
            MIN_STATIC_ASSERT(sizeof(NET_GM_LOG_ITEM_RELOAD_SS)<=NET_DATA_BUFSIZE);
        }
    };

    //! Client->Agent
    struct NET_GM_GAME_DATA_UPDATE_CA : public NET_MSG_GENERIC
    {
		int nDataType;

		NET_GM_GAME_DATA_UPDATE_CA() : nDataType( 0 )
        {
            dwSize = sizeof(NET_GM_GAME_DATA_UPDATE_CA);
            nType = NET_MSG_GM_GAME_DATA_UPDATE_CA;
            MIN_STATIC_ASSERT(sizeof(NET_GM_GAME_DATA_UPDATE_CA)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent->Session
    struct NET_GM_GAME_DATA_UPDATE_AS : public NET_MSG_GENERIC
    {
		int nDataType;

		NET_GM_GAME_DATA_UPDATE_AS() : nDataType( 0 )
        {
            dwSize = sizeof(NET_GM_GAME_DATA_UPDATE_AS);
            nType = NET_MSG_GM_GAME_DATA_UPDATE_AS;
            MIN_STATIC_ASSERT(sizeof(NET_GM_GAME_DATA_UPDATE_AS)<=NET_DATA_BUFSIZE);
        }
    };

    //! Session->All server
    struct NET_GM_GAME_DATA_UPDATE_SS : public NET_MSG_GENERIC
    {
		int nDataType;

		NET_GM_GAME_DATA_UPDATE_SS() : nDataType( 0 )
        {
            dwSize = sizeof(NET_GM_GAME_DATA_UPDATE_SS);
            nType = NET_MSG_GM_GAME_DATA_UPDATE_SS;
            MIN_STATIC_ASSERT(sizeof(NET_GM_GAME_DATA_UPDATE_SS)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNET_MSG_CATCH_DELAY : public NET_MSG_GENERIC
	{
		SNET_MSG_CATCH_DELAY ()
		{
			dwSize = sizeof( SNET_MSG_CATCH_DELAY );
			nType = NET_MSG_CATCH_DELAY;
			MIN_STATIC_ASSERT( sizeof( SNET_MSG_CATCH_DELAY ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_GM_MSG_DELAY_MODIFY : public NET_MSG_GENERIC
	{
		int nMsgType;
		float fSecond;

		SNET_GM_MSG_DELAY_MODIFY ()
			: nMsgType( 0 )
			, fSecond( 0.0f )
		{
			dwSize = sizeof( SNET_GM_MSG_DELAY_MODIFY );
			nType = NET_MSG_GM_MSG_DELAY_MODIFY;
			MIN_STATIC_ASSERT( sizeof( SNET_GM_MSG_DELAY_MODIFY ) <= NET_DATA_BUFSIZE );
		}
	};    

	struct SNETPC_GM_REFERENCE_COUNT_CAF : public NET_MSG_GENERIC
	{
		int nCountMax;

		SNETPC_GM_REFERENCE_COUNT_CAF()
			: nCountMax( 300 )
		{
			dwSize = sizeof( SNETPC_GM_REFERENCE_COUNT_CAF );
			nType = NET_MSG_REFERENCE_COUNT_CAF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_GM_REFERENCE_COUNT_CAF ) <= NET_DATA_BUFSIZE );
		}
	};

    struct SNETPC_GM_LOAD_IPEVENT : public NET_MSG_GENERIC
    {
        SNETPC_GM_LOAD_IPEVENT()
        {
            dwSize = sizeof( SNETPC_GM_LOAD_IPEVENT );
            nType = NET_MSG_GM_LOAD_IPEVENT;
            MIN_STATIC_ASSERT( sizeof( SNETPC_GM_LOAD_IPEVENT ) <= NET_DATA_BUFSIZE );
        }
    };

    struct SNETPC_GM_SET_IPEVENT : public NET_MSG_GENERIC
    {
        bool bEnable;

        SNETPC_GM_SET_IPEVENT()
            : bEnable(false)
        {
            dwSize = sizeof( SNETPC_GM_SET_IPEVENT );
            nType = NET_MSG_GM_SET_IPEVENT;
            MIN_STATIC_ASSERT( sizeof( SNETPC_GM_SET_IPEVENT ) <= NET_DATA_BUFSIZE );
        }
    };

    struct SNETPC_GM_ACQUIRE_EXP : public NET_MSG_GENERIC
    {
        bool  bIsToggle;
        bool  bEnable;
        DWORD dwID;

        SNETPC_GM_ACQUIRE_EXP()
            : bIsToggle(false),
              bEnable(false),
              dwID(0)
        {
            dwSize = sizeof( SNETPC_GM_ACQUIRE_EXP );
            nType = NET_MSG_GM_ACQUIRE_EXP;
            MIN_STATIC_ASSERT( sizeof( SNETPC_GM_ACQUIRE_EXP ) <= NET_DATA_BUFSIZE );
        }
    };

    struct SNETPC_GM_ACQUIRE_EXP_ANS : public NET_MSG_GENERIC
    {
        bool  bEnable;

        SNETPC_GM_ACQUIRE_EXP_ANS()
            : bEnable(false)
        {
            dwSize = sizeof( SNETPC_GM_ACQUIRE_EXP_ANS );
            nType = NET_MSG_GM_ACQUIRE_EXP_ANS;
            MIN_STATIC_ASSERT( sizeof( SNETPC_GM_ACQUIRE_EXP_ANS ) <= NET_DATA_BUFSIZE );
        }
    };

	// Emulator, Client->Field;
	struct SNET_GM_FLYCAMERACONTROL_CF : public NET_MSG_GENERIC
	{
		BOOL	bOn;
		SNET_GM_FLYCAMERACONTROL_CF(void): bOn(FALSE)
		{
			dwSize = sizeof(SNET_GM_FLYCAMERACONTROL_CF);
			nType = NET_MSG_GM_FLYCAMERACONTROL_CF;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_FLYCAMERACONTROL_CF)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_FLYCAMERACONTROL_FC : public NET_MSG_GENERIC
	{
		BOOL bOn;
		SNET_GM_FLYCAMERACONTROL_FC(void): bOn(FALSE)
		{
			dwSize = sizeof(SNET_GM_FLYCAMERACONTROL_FC);
			nType = NET_MSG_GM_FLYCAMERACONTROL_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_FLYCAMERACONTROL_FC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_FLYCAMERA_SETVIEWSIGHT_CF : public NET_MSG_GENERIC
	{
		float fSight;
		SNET_GM_FLYCAMERA_SETVIEWSIGHT_CF(void) : fSight(0.0f)
		{
			dwSize = sizeof(SNET_GM_FLYCAMERA_SETVIEWSIGHT_CF);
			nType = NET_MSG_GM_FLYCAMERA_SETVIEWSIGHT_CF;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_FLYCAMERA_SETVIEWSIGHT_CF)<=NET_DATA_BUFSIZE);
		}
	};

	// Client->Agent
	struct SNET_GM_FLYCAMERACONTROL_CA : public NET_MSG_GENERIC
	{
		BOOL	bOn;
		SNET_GM_FLYCAMERACONTROL_CA(void): bOn(FALSE)
		{
			dwSize = sizeof(SNET_GM_FLYCAMERACONTROL_CA);
			nType = NET_MSG_GM_FLYCAMERACONTROL_CA;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_FLYCAMERACONTROL_CA)<=NET_DATA_BUFSIZE);
		}
	};

	// Agent->Client
	struct SNET_GM_FLYCAMERACONTROL_FB : public NET_MSG_GENERIC
	{
		BOOL	bOn;
		SNET_GM_FLYCAMERACONTROL_FB(void): bOn(FALSE)
		{
			dwSize = sizeof(SNET_GM_FLYCAMERACONTROL_FB);
			nType = NET_MSG_GM_FLYCAMERACONTROL_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_FLYCAMERACONTROL_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GM_FLYCAMERACONTROL_MOVE_CF : public NET_MSG_GENERIC
	{
		D3DXVECTOR3 vPosition;
		SNET_GM_FLYCAMERACONTROL_MOVE_CF(void)
			: vPosition(0.0f, 0.0f, 0.0f)
		{
			dwSize = sizeof(SNET_GM_FLYCAMERACONTROL_MOVE_CF);
			nType = NET_MSG_GM_FLYCAMERACONTROL_MOVE_CF;
			MIN_STATIC_ASSERT(sizeof(SNET_GM_FLYCAMERACONTROL_MOVE_CF)<=NET_DATA_BUFSIZE);
		}
	};

	//! Client->Agent
	struct NET_GM_NOTIFY_MONEY_CAF : public NET_MSG_GENERIC
	{
		LONGLONG money;
		NET_GM_NOTIFY_MONEY_CAF()
		{
			money = 0;
			dwSize = sizeof( NET_GM_NOTIFY_MONEY_CAF );
			nType = NET_MSG_GM_NOTIFY_MONEY_CAF;
			MIN_STATIC_ASSERT( sizeof( NET_GM_NOTIFY_MONEY_CAF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_MACROMODE_ONOFF_CF : public NET_MSG_GENERIC
	{
		bool bOn;
		SNET_MACROMODE_ONOFF_CF(void)
			: bOn(false)
		{
			dwSize = sizeof(SNET_MACROMODE_ONOFF_CF);
			nType = NET_MSG_MACROMODE_ONOFF_CF;
			MIN_STATIC_ASSERT(sizeof(SNET_MACROMODE_ONOFF_CF)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_MACROMODE_ONOFF_FB : public NET_MSG_GENERIC
	{
		enum // MSG
		{
			MACRO_ERROR_NOTINIT = -1,

			// 활성화 실패시;
			MACROON_OK,// = 0,
			MACROON_ERROR_NONE,
			MACROON_ERROR_LACKTIME,	// 시간 부족;
			// 비활성화 실패시;
			MACROOFF_OK,// = 0,
			MACROOFF_ERROR_NONE,
		};
		DWORD dwMsg;
		SNET_MACROMODE_ONOFF_FB(void)
			: dwMsg(MACRO_ERROR_NOTINIT)
		{
			dwSize = sizeof(SNET_MACROMODE_ONOFF_FB);
			nType = NET_MSG_MACROMODE_ONOFF_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_MACROMODE_ONOFF_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_MACROMODE_CLOSE_FC : public NET_MSG_GENERIC
	{
		enum 
		{
			MACROCLOSE_NONE = 0,
			MACROCLOSE_TIMEOVER,
		};
		DWORD dwMsg;
		SNET_MACROMODE_CLOSE_FC(void)
			: dwMsg(MACROCLOSE_NONE)
		{
			dwSize = sizeof(SNET_MACROMODE_CLOSE_FC);
			nType = NET_MSG_MACROMODE_CLOSE_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_MACROMODE_CLOSE_FC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_MACROMODE_STATE_NOTIFY_FC : public NET_MSG_GENERIC
	{
		enum 
		{
			MACROSTATE_NONE = 0,
			MACROSTATE_TIMEFILL_NOTIFY,
			MACROSTATE_TIMEFILL_FAILD_MAXTIME,
		};
		DWORD dwMsg;
		DWORD dwData;
		SNET_MACROMODE_STATE_NOTIFY_FC(void)
			: dwMsg(MACROSTATE_NONE)
			, dwData(0)
		{
			dwSize = sizeof(SNET_MACROMODE_STATE_NOTIFY_FC);
			nType = NET_MSG_MACROMODE_STATE_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_MACROMODE_STATE_NOTIFY_FC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_MACROMODE_REQNEARESTCROWINFO_CF : public NET_MSG_GENERIC
	{
		SNET_MACROMODE_REQNEARESTCROWINFO_CF(void)
		{
			dwSize = sizeof(SNET_MACROMODE_REQNEARESTCROWINFO_CF);
			nType = NET_MSG_MACROMODE_REQNEARESTCROWINFO_CF;
			MIN_STATIC_ASSERT(sizeof(SNET_MACROMODE_REQNEARESTCROWINFO_CF)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_MACROMODE_NEARESTCROWINFO_FC : public NET_MSG_GENERIC
	{
		bool bFind;
		D3DXVECTOR3 vPosition;
		SNET_MACROMODE_NEARESTCROWINFO_FC(void)
			: bFind(false)
		{
			dwSize = sizeof(SNET_MACROMODE_NEARESTCROWINFO_FC);
			nType = NET_MSG_MACROMODE_NEARESTCROWINFO_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_MACROMODE_NEARESTCROWINFO_FC)<=NET_DATA_BUFSIZE);
		}
	};

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.11, 확률제어 데이터 등록요청
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA : public NET_MSG_GENERIC
    {
        SNATIVEID       sBoxID;    /* RandomBox ID */
        SNATIVEID       sItemID;   /* Item ID */
        INT32           nReqCount; /* 당첨까지 소요되는 Randombox 오픈 수 */
        DWORD           dwChaNum;  /* 당첨 Target ChaNum, GM Command에서 ChaNum이 넘어오지 않으면 0으로 Setting */
        
        //SNET_MSG_GCTL_RANDOMBOX_CHANCE_REGISTER_REQ_CA(const SNATIVEID& BoxId, const SNATIVEID& ItemId, INT32 ReqCount, DWORD ChaNum)
        //    : sBoxID(BoxId)
        //    , sItemID(ItemId)
        //    , nReqCount(ReqCount)
        //    , dwChaNum(ChaNum)
        //    , dwGMNum(0)
        SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA()
            : sBoxID(false)
            , sItemID(false)
            , nReqCount(0)
            , dwChaNum(0)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.11, 확률제어 데이터 등록요청 응답
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC : public NET_MSG_GENERIC
    {
        enum
        {
            REP_FLAG_SUCESS,       /* 정상 처리 */
            REP_FLAG_FAIL,         /* 일반 오류 */
            REQ_FLAG_GMGRADE_ERR,  /* GM 등급 오류 */
            REP_FLAG_BOXID_ERR,    /* 박스 아이디 오류 */
            REP_FLAG_ITEM_ERR,     /* 아이템 오류 */
            REP_FLAG_COUNT_ERR,    /* Count 오류 */
            REP_FLAG_CHANUM_ERR,   /* ChaNum 오류 */
            REP_FLAG_DUPLICATE_ERR /* 데이터 중복 오류 */
        };
        
        WORD    wRepFlag; // enum 참조
        
        SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC()
            : wRepFlag(REP_FLAG_SUCESS)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.11, 확률제어 데이터 등록요청
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH : public NET_MSG_GENERIC
    {
        DWORD           dwClientSlot; /* Client Socket id */
        SNATIVEID       sBoxID;       /* RandomBox ID */
        SNATIVEID       sItemID;      /* Item ID */
        INT32           nReqCount;    /* 당첨까지 소요되는 Randombox 오픈 수 */
        DWORD           dwChaNum;     /* 당첨 Target ChaNum, GM Command에서 ChaNum이 넘어오지 않으면 0으로 Setting */
        DWORD           dwGMNum;      /* 요청 GM ID */
        
        SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH()
            : dwClientSlot(0)
            , sBoxID(false)
            , sItemID(false)
            , nReqCount(0)
            , dwChaNum(0)
            , dwGMNum(0)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.11, 확률제어 데이터 등록요청 응답
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA : public NET_MSG_GENERIC
    {
        enum
        {
            REP_FLAG_SUCESS,        /* 정상 처리 */
            REP_FLAG_FAIL,          /* 일반 오류 */
            REP_FLAG_DUPLICATE_ERR  /* 데이터 중복 오류 */
        };
        
        WORD    wRepFlag;     // 처리결과
        DWORD   dwClientSlot; // Client Socket id
        
        SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA()
            : wRepFlag(REP_FLAG_SUCESS)
            , dwClientSlot(0)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, 확률제어 등록 데이터 조회 요청
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_CA : public NET_MSG_GENERIC
    {
        SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_CA()
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_CA);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_CA;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_CA) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, 확률제어 등록 데이터 조회 요청 응답
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC : public NET_MSG_GENERIC
    {
        enum
        {
            REP_FLAG_SUCESS,       /* 정상 처리 */
            REP_FLAG_FAIL,         /* 일반 오류 */
            REQ_FLAG_GMGRADE_ERR,  /* GM 등급 오류 */
            REP_FLAG_NOTFOUND_ERR  /* 데이터없음 */
        };
        
        WORD            wRepFlag; // enum 참조
        INT32           nIndex;         /* 데이터 유니크 키(조회에서 찾을수 있음) */
        SNATIVEID       sBoxID;         /* RandomBox ID */
        SNATIVEID       sItemID;        /* Item ID */
        INT32           nReqCount;      /* 당첨까지 소요되는 Randombox 오픈 수 */
        DWORD           dwTargetChaNum; /* 당첨대상 ChaNum, 0이면 대상없음(전체) */
        DWORD           dwGMNum;        /* 요청 GM ID */
        INT32           nOpenCount;     /* 실제 오픈된 박스수 */
        
        SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC()
            : nIndex(0)
            , sBoxID(false)
            , sItemID(false)
            , nReqCount(0)
            , dwTargetChaNum(0)
            , dwGMNum(0)
            , nOpenCount(0)
            , wRepFlag(REP_FLAG_SUCESS)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, 확률제어 등록 데이터 조회 요청
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH : public NET_MSG_GENERIC
    {
        DWORD           dwClientSlot; /* Client Socket id */
        
        SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH()
            : dwClientSlot(0)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, 확률제어 등록 데이터 조회 요청 응답
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA : public NET_MSG_GENERIC
    {
        enum
        {
            REP_FLAG_SUCESS,        /* 정상 처리 */
            REP_FLAG_FAIL,          /* 일반 오류 */
            REP_FLAG_NOTFOUND_ERR  /* 데이터없음 */
        };
        
        WORD            wRepFlag;     // 처리결과
        DWORD           dwClientSlot; // Client Socket id
        INT32           nIndex;         /* 데이터 유니크 키(조회에서 찾을수 있음) */
        SNATIVEID       sBoxID;         /* RandomBox ID */
        SNATIVEID       sItemID;        /* Item ID */
        INT32           nReqCount;      /* 당첨까지 소요되는 Randombox 오픈 수 */
        DWORD           dwTargetChaNum; /* 당첨대상 ChaNum, 0이면 대상없음(전체) */
        DWORD           dwGMNum;        /* 요청 GM ID */
        INT32           nOpenCount;     /* 실제 오픈된 박스수 */
        
        SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA()
            : wRepFlag(REP_FLAG_SUCESS)
            , dwClientSlot(0)
            , nIndex(0)
            , sBoxID(false)
            , sItemID(false)
            , nReqCount(0)
            , dwTargetChaNum(0)
            , dwGMNum(0)
            , nOpenCount(0)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, 확률제어 등록 데이터 삭제 요청
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA : public NET_MSG_GENERIC
    {
        INT32           nIndex; /* 데이터 유니크 키(조회에서 찾을수 있음) */

        SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA()
            :nIndex(0)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, 확률제어 등록 데이터 삭제 요청 응답
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC : public NET_MSG_GENERIC
    {
        enum
        {
            REP_FLAG_SUCESS,       /* 정상 처리 */
            REP_FLAG_FAIL,         /* 일반 오류 */
            REQ_FLAG_GMGRADE_ERR,  /* GM 등급 오류 */
            REP_FLAG_NOTFOUND_ERR  /* 데이터없음 */
        };
        
        WORD            wRepFlag;
        
        SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC()
            : wRepFlag(REP_FLAG_SUCESS)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, 확률제어 등록 데이터 삭제 요청
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH : public NET_MSG_GENERIC
    {
        DWORD           dwClientSlot; /* Client Socket id */
        INT32           nIndex; /* 데이터 유니크 키(조회에서 찾을수 있음) */
        
        SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH()
            : dwClientSlot(0)
            , nIndex(0)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.17, 확률제어 등록 데이터 삭제 요청 응답
     * modified : 
     */
    struct SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA : public NET_MSG_GENERIC
    {
        enum
        {
            REP_FLAG_SUCESS,        /* 정상 처리 */
            REP_FLAG_FAIL,          /* 일반 오류 */
            REP_FLAG_NOTFOUND_ERR   /* 데이터없음 */
        };
        
        WORD            wRepFlag;     // 처리결과
        DWORD           dwClientSlot; // Client Socket id
        
        SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA()
            : wRepFlag(REP_FLAG_SUCESS)
            , dwClientSlot(0)
        {
            dwSize = sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA);
            nType = NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.16, Randombox Open시 확률제어 적용 여부 요청
     * modified : 
     */
    struct SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH : public NET_MSG_GENERIC
    {
        DWORD           dwGaeaID;     /* Client gaeaid */
        DWORD           dwChaNum;     /* ChaNum */
        WORD            wPosX;        /* Inventory Item Position X */
        WORD            wPosY;        /* Inventory Item Position Y */
        SNATIVEID       sBoxID;       /* RandomBox ID */
        
        SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH()
            : dwGaeaID(0)
            , dwChaNum(0)
            , wPosX(0)
            , wPosY(0)
            , sBoxID(false)
        {
            dwSize = sizeof(SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH);
            nType = NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.16, Randombox Open시 확률제어 적용 여부 요청 응답
     * modified : 
     */
    struct SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF : public NET_MSG_GENERIC
    {
        enum
        {
            REP_FLAG_WINNER,        /* 당첨 */
            REP_FLAG_NORMAL,        /* 진행중 */
            REP_FLAG_FAIL           /* 일반오류 */
        };
        
        DWORD           dwGaeaID;       /* Client gaeaid */
        DWORD           dwChaNum;       /* ChaNum */
        WORD            wPosX;          /* Inventory Item Position X */
        WORD            wPosY;          /* Inventory Item Position Y */
        SNATIVEID       sBoxID;         /* RandomBox ID */
        SNATIVEID       sItemID;        /* Item ID */
        INT32           nReqCount;      /* 당첨까지 소요되는 Randombox 오픈 수 */
        DWORD           dwTargetChaNum; /* 당첨대상 ChaNum, 0이면 대상없음(전체) */
        DWORD           dwGMNum;        /* 요청 GM ID */
        INT32           nOpenCount;     /* 실제 오픈된 박스수 */
        WORD            wRepFlag;       /* 처리결과 */
        INT32           nIndexAll;      /* 확률제어 데이터 유니크 인덱스  전체 */
        INT32           nIndexCha;      /* 확률제어 데이터 유니크 인덱스  개별 */
        
        SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF()
            : dwGaeaID(0)
            , dwChaNum(0)
            , wPosX(0)
            , wPosY(0)
            , sBoxID(false)
            , sItemID(false)
            , nReqCount(0)
            , dwTargetChaNum(0)
            , dwGMNum(0)
            , nOpenCount(0)
            , wRepFlag(REP_FLAG_NORMAL)
            , nIndexAll(0)
            , nIndexCha(0)
        {
            dwSize = sizeof(SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF);
            nType = NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.16, Randombox Open시 확률제어 완료 여부
     * modified : 
     */
    struct SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH : public NET_MSG_GENERIC
    {
        BOOL            bComplete;    /* 확률제어 데이터 정상 처리 여부 */
        INT32           nIndexAll;    /* 확률제어 데이터 유니크 인덱스  전체 */
        INT32           nIndexCha;    /* 확률제어 데이터 유니크 인덱스  개별 */
        
        SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH()
            : bComplete(false)
            , nIndexAll(0)
            , nIndexCha(0)
        {
            dwSize = sizeof(SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH);
            nType = NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH) <= NET_DATA_BUFSIZE);
        }
    };

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.23, Randombox Open시 당첨확률이 5%미만의 경우 공지 메시지를 전달
     * modified : sckim, 2016.09.20, Redmine #4544 확률/아이템 리스트 제어 기능 적용 
     */
	struct SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA : public NET_MSG_GENERIC
	{
		DWORD       dwTargetChanum;
        SITEMCUSTOM sRandomboxID;
		SITEMCUSTOM sItemID;

		SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA(const DWORD TargetChanum, const SITEMCUSTOM& RandomboxID, const SITEMCUSTOM& ItemID)
			: dwTargetChanum(TargetChanum)
			, sRandomboxID(RandomboxID)
			, sItemID(ItemID)
		{
			dwSize = sizeof(SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA);
			nType = NET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA;
			MIN_STATIC_ASSERT(sizeof(SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA) <= NET_DATA_BUFSIZE);
		}
	};

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.23, Randombox Open시 당첨확률이 5%미만의 경우 공지 메시지를 전달
     * modified : sckim, 2016.09.20, Redmine #4544 확률/아이템 리스트 제어 기능 적용 
     */
	struct SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC : public NET_MSG_GENERIC
	{
		CHAR        szTargetName[CHAR_SZNAME + 1];
        SITEMCUSTOM sRandomboxID;
		SITEMCUSTOM sItemID;

		SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC(const std::string& TargetName, const SITEMCUSTOM& RandomboxID, const SITEMCUSTOM& ItemID)
			: sRandomboxID(RandomboxID)
			, sItemID(ItemID)
		{
			dwSize = sizeof(SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC);
			nType = NET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC;
			MIN_STATIC_ASSERT(sizeof(SNET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC) <= NET_DATA_BUFSIZE);

			::StringCchCopy(szTargetName, CHAR_SZNAME, TargetName.c_str());
		}
	};

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    struct SNET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF : public NET_MSG_GENERIC
    {
        SNET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF()
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF;
            MIN_STATIC_ASSERT(sizeof(SNET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF) <= NET_DATA_BUFSIZE);
        }
    };

	// Revert to default structure packing
#pragma pack()
};
