#ifndef _GL_SERVER_MSG_H_
#define _GL_SERVER_MSG_H_

#pragma once

#include "../../SigmaCore/MsgPack/MinMsgPack.h"

#include "../s_NetGlobal.h"

// Full ServerInfo 
struct NET_SERVER_INFO : public NET_MSG_GENERIC
{    
    G_SERVER_INFO gsi;
    NET_SERVER_INFO(const G_SERVER_INFO& _gsi)
        : gsi(_gsi)
    {
        nType = NET_MSG_SVR_CLOSECLIENT;
        dwSize = sizeof(NET_SERVER_INFO);
        MIN_STATIC_ASSERT(sizeof(NET_SERVER_INFO)<=NET_DATA_BUFSIZE);
    }

    inline void SetType(EMNET_MSG MsgType)
    {
        nType = MsgType;
    }
};

struct NET_SERVER_CUR_INFO : public NET_MSG_GENERIC
{
    G_SERVER_CUR_INFO	gsci;
    NET_SERVER_CUR_INFO()
    {
        nType  = NET_MSG_SND_CUR_STATE;
        dwSize = sizeof(NET_SERVER_CUR_INFO);
        MIN_STATIC_ASSERT(sizeof(NET_SERVER_CUR_INFO)<=NET_DATA_BUFSIZE);
    }
};

struct NET_SERVER_CHANNEL_INFO : public NET_MSG_GENERIC
{
    int nServerGroup;
    int nChannel;
    int nChannelCurrentUser;
    int nChannelMaxUser;
    bool bPk;

    NET_SERVER_CHANNEL_INFO()
    {		
        nType = NET_MSG_SND_CHANNEL_STATE;
        dwSize = sizeof(NET_SERVER_CHANNEL_INFO);

        nServerGroup        = 0;
        nChannel            = 0;
        nChannelCurrentUser = 0;
        nChannelMaxUser     = 0;
        bPk                 = true;
        MIN_STATIC_ASSERT(sizeof(NET_SERVER_CHANNEL_INFO)<=NET_DATA_BUFSIZE);
    }
};

struct NET_SERVER_CHANNEL_FULL_INFO : public NET_MSG_GENERIC
{
    int nServerGroup;
    int nChannel;
    bool bChannelFull;
    int nChannelCurrentUser;

    NET_SERVER_CHANNEL_FULL_INFO()
    {		
        nType = NET_MSG_SND_CHANNEL_FULL_STATE;
        dwSize = sizeof(NET_SERVER_CHANNEL_FULL_INFO);

        nServerGroup = 0;
        nChannel     = 0;
        bChannelFull = FALSE;
        nChannelCurrentUser = 0;
        MIN_STATIC_ASSERT(sizeof(NET_SERVER_CHANNEL_FULL_INFO)<=NET_DATA_BUFSIZE);
    }
};

struct NET_CUR_INFO_LOGIN : public NET_MSG_GENERIC
{
    G_SERVER_CUR_INFO_LOGIN gscil;
    NET_CUR_INFO_LOGIN()
    {
        nType = EMNET_MSG(0);
        dwSize = sizeof(NET_CUR_INFO_LOGIN);
        MIN_STATIC_ASSERT(sizeof(NET_CUR_INFO_LOGIN)<=NET_DATA_BUFSIZE);
    }

    void SetType(EMNET_MSG MsgType)
    {
        nType = MsgType;
    }

    void SetData(const G_SERVER_CUR_INFO_LOGIN& _gscil)
    {
        gscil = _gscil;
    }
};

struct NET_RECONNECT_FILED : public NET_MSG_GENERIC
{
    NET_RECONNECT_FILED()
    {
        nType = NET_MSG_RECONNECT_FIELD;
        dwSize = sizeof(NET_RECONNECT_FILED);
        MIN_STATIC_ASSERT(sizeof(NET_RECONNECT_FILED)<=NET_DATA_BUFSIZE);
    }
};

struct NET_RECONNECT_MATCH : public NET_MSG_GENERIC
{
    NET_RECONNECT_MATCH()
    {
        nType = NET_MSG_RECONNECT_MATCH;
        dwSize = sizeof(NET_RECONNECT_MATCH);
        MIN_STATIC_ASSERT(sizeof(NET_RECONNECT_MATCH)<=NET_DATA_BUFSIZE);
    }
};

//! Session:Client socket closed
struct NET_CLOSE_CLIENT_SESSION : public NET_MSG_GENERIC
{
    DWORD m_Client;
    NET_CLOSE_CLIENT_SESSION(DWORD Client)
        : m_Client(Client)
    {
        nType = NET_MSG_CLOSE_CLIENT_SESSION;
        dwSize = sizeof(NET_CLOSE_CLIENT_SESSION);
        MIN_STATIC_ASSERT(sizeof(NET_CLOSE_CLIENT_SESSION)<=NET_DATA_BUFSIZE);
    }

    inline DWORD ClientSlot() const { return m_Client; }
};

//-----------------------------------------------------------------------------------------------//
// for global auth
struct NET_CLOSE_CLIENT_AUTH : public NET_MSG_GENERIC
{
    DWORD m_Client;
    NET_CLOSE_CLIENT_AUTH(DWORD Client)
        : m_Client(Client)
    {
        nType = NET_MSG_CLOSE_CLIENT_AUTH;
        dwSize = sizeof(NET_CLOSE_CLIENT_AUTH);
        MIN_STATIC_ASSERT(sizeof(NET_CLOSE_CLIENT_AUTH)<=NET_DATA_BUFSIZE);
    }

    inline DWORD ClientSlot() const { return m_Client; }
};

struct NET_AUTH_CERTIFICATION_REQUEST : public NET_MSG_GENERIC
{    
    G_AUTH_INFO gsi;

    NET_AUTH_CERTIFICATION_REQUEST(const G_AUTH_INFO& _gsi)
        : gsi(_gsi)
    {
        nType = NET_MSG_AUTH_CERTIFICATION_REQUEST;
        dwSize = sizeof(NET_AUTH_CERTIFICATION_REQUEST);
        MIN_STATIC_ASSERT(sizeof(NET_AUTH_CERTIFICATION_REQUEST)<=NET_DATA_BUFSIZE);
    }
};

struct NET_AUTH_CERTIFICATION_ANS : public NET_MSG_GENERIC
{    
    G_AUTH_INFO gsi;

    NET_AUTH_CERTIFICATION_ANS(const G_AUTH_INFO& _gsi)
        : gsi(_gsi)
    {
        nType = NET_MSG_AUTH_CERTIFICATION_ANS;
        dwSize = sizeof(NET_AUTH_CERTIFICATION_ANS);
        MIN_STATIC_ASSERT(sizeof(NET_AUTH_CERTIFICATION_ANS)<=NET_DATA_BUFSIZE);
    }
};

//SendCountryAccessApprove
struct NET_COUNTRY_ACCESS_APPROVE
{    
	//NET_MSG_COUNTRY_ACCESS_APPROVE

	std::vector<int> vecContryList;
	MSGPACK_DEFINE(vecContryList);

	NET_COUNTRY_ACCESS_APPROVE()
	{
		vecContryList.clear();
	}
};


struct NET_AUTH_STATE_REQ : public NET_MSG_GENERIC
{   
    NET_AUTH_STATE_REQ()
    {
        nType = NET_MSG_AUTH_STATE_REQ;
        dwSize = sizeof(NET_AUTH_STATE_REQ);
        MIN_STATIC_ASSERT(sizeof(NET_AUTH_STATE_REQ)<=NET_DATA_BUFSIZE);
    }
};

struct NET_AUTH_STATE_ANS : public NET_MSG_GENERIC
{   
	G_AUTH_CUR_INFO gci;

    NET_AUTH_STATE_ANS(const G_AUTH_CUR_INFO& _gci)	: gci(_gci)
    {
        nType = NET_MSG_AUTH_STATE_ANS;
        dwSize = sizeof(NET_AUTH_STATE_ANS);
        MIN_STATIC_ASSERT(sizeof(NET_AUTH_STATE_ANS)<=NET_DATA_BUFSIZE);
    }
};

struct NET_AUTH_EVENT_INFO_REQ : public NET_MSG_GENERIC
{   
    NET_AUTH_EVENT_INFO_REQ()
    {
        nType = NET_MSG_AUTH_EVENT_INFO_REQ;
        dwSize = sizeof(NET_AUTH_EVENT_INFO_REQ);
        MIN_STATIC_ASSERT(sizeof(NET_AUTH_EVENT_INFO_REQ)<=NET_DATA_BUFSIZE);
    }
};

struct NET_EVENT_INFO_AUTH_ANS : public NET_MSG_GENERIC
{   
	G_AUTH_EVENT_INFO gei;

    NET_EVENT_INFO_AUTH_ANS(const G_AUTH_EVENT_INFO& _gei)	: gei(_gei)
    {
        nType = NET_MSG_AUTH_EVENT_INFO_ANS;
        dwSize = sizeof(NET_EVENT_INFO_AUTH_ANS);
        MIN_STATIC_ASSERT(sizeof(NET_EVENT_INFO_AUTH_ANS)<=NET_DATA_BUFSIZE);
    }
};


struct NET_AUTH_SERVER_STATE_REQ : public NET_MSG_GENERIC
{   
    NET_AUTH_SERVER_STATE_REQ()
    {
        nType = NET_MSG_AUTH_SERVER_STATE_REQ;
        dwSize = sizeof(NET_AUTH_SERVER_STATE_REQ);
        MIN_STATIC_ASSERT(sizeof(NET_AUTH_SERVER_STATE_REQ)<=NET_DATA_BUFSIZE);
    }
};

struct NET_AUTH_SERVER_STATE_ANS : public NET_MSG_GENERIC
{   
	G_AUTH_INFO gci;

    NET_AUTH_SERVER_STATE_ANS(const G_AUTH_INFO& _gci)	: gci(_gci)
    {
        nType = NET_MSG_AUTH_SERVER_STATE_ANS;
        dwSize = sizeof(NET_AUTH_SERVER_STATE_ANS);
        MIN_STATIC_ASSERT(sizeof(NET_AUTH_SERVER_STATE_ANS)<=NET_DATA_BUFSIZE);
    }
};

struct NET_AUTH_SERVER_CONNECTION_STATE : public NET_MSG_GENERIC
{    
    bool m_bDisconnection;

    NET_AUTH_SERVER_CONNECTION_STATE()
        : m_bDisconnection(false)
    {
        nType = NET_MSG_SND_AUTH_SVR_INFO;
        dwSize = sizeof(NET_AUTH_SERVER_CONNECTION_STATE);
        MIN_STATIC_ASSERT(sizeof(NET_AUTH_SERVER_CONNECTION_STATE)<=NET_DATA_BUFSIZE);
    }
};

//-----------------------------------------------------------------------------------------------//

//! Agent->Cache:캐릭터 리스트 요청
struct NET_CHA_LIST_AH : public NET_MSG_GENERIC
{
    int m_UserNum;
    int m_ServerGroup;
    DWORD m_ClientSlot;
    TCHAR m_Ip[MAX_IP_LENGTH+1];
    USHORT m_Port;

    NET_CHA_LIST_AH(int UserNum, int ServerGroup, DWORD ClientSlot, const TCHAR* IpAddress, USHORT Port)
        : m_UserNum(UserNum)
        , m_ServerGroup(ServerGroup)
        , m_ClientSlot(ClientSlot)
        , m_Port(Port)
    {
        nType = NET_MSG_CHA_LIST_AH;
        dwSize = sizeof(NET_CHA_LIST_AH);
        StringCchCopy(m_Ip, MAX_IP_LENGTH+1, IpAddress);
        MIN_STATIC_ASSERT(sizeof(NET_CHA_LIST_AH)<=NET_DATA_BUFSIZE);
    }
};

//! 현재 서버에서 해당 유저가 가진 캐릭터의 번호를 넘겨준다.
//! Database->Cache Server
struct NET_CHA_BBA_INFO_DH : public NET_MSG_GENERIC
{
    DWORD m_CacheServerSlot;
    DWORD m_AgentServerSlot;
    int	m_ChaServerTotalNum; // 현재 이 서버에서 생성된 캐릭터 갯수
    int	m_ChaDbNum[MAX_ONESERVERCHAR_NUM];
    USHORT m_uPort;
    char m_Ip[MAX_IP_LENGTH+1];
    NET_CHA_BBA_INFO_DH(DWORD CacheServerSlot, DWORD AgentServerSlot, USHORT Port, const std::string& IpAddress)
        : m_CacheServerSlot(CacheServerSlot)
        , m_AgentServerSlot(AgentServerSlot)
        , m_uPort(Port)
        , m_ChaServerTotalNum(0)
    {		
        dwSize = sizeof(NET_CHA_BBA_INFO_DH);
        nType = NET_MSG_CHA_BAINFO_DH;		
        for (int i=0; i<MAX_ONESERVERCHAR_NUM; ++i)
        {
            m_ChaDbNum[i] = 0;
        }
        StringCchCopy(m_Ip, MAX_IP_LENGTH+1, IpAddress.c_str());
        MIN_STATIC_ASSERT(sizeof(NET_CHA_BBA_INFO_DH)<=NET_DATA_BUFSIZE);
    }

    void SetChaNum(size_t Index, int ChaDbNum)
    {
        if (Index >=MAX_ONESERVERCHAR_NUM)
            return;
        else
            m_ChaDbNum[Index] = ChaDbNum;
    }

    void SetChaTotalNum(size_t TotalNum)
    {        
        if (TotalNum >= MAX_ONESERVERCHAR_NUM)
            m_ChaServerTotalNum = MAX_ONESERVERCHAR_NUM;
        else
            m_ChaServerTotalNum = static_cast<int> (TotalNum);
    }
};
//typedef NET_CHA_BBA_INFO_DH* LPNET_CHA_BBA_INFO;

//! 현재 서버에서 해당 유저가 가진 캐릭터의 번호를 넘겨준다.
//! Cache Server->Agent
struct NET_CHA_BBA_INFO_HA : public NET_MSG_GENERIC
{
    DWORD m_AgentServerSlot;
    int	m_ChaServerTotalNum; // 현재 이 서버에서 생성된 캐릭터 갯수
    int	m_ChaDbNum[MAX_ONESERVERCHAR_NUM];
    USHORT m_uPort;
    char m_Ip[MAX_IP_LENGTH+1];
    NET_CHA_BBA_INFO_HA(DWORD AgentServerSlot, USHORT Port, const std::string& IpAddress)
        : m_AgentServerSlot(AgentServerSlot)
        , m_uPort(Port)
        , m_ChaServerTotalNum(0)
    {		
        dwSize = sizeof(NET_CHA_BBA_INFO_HA);
        nType = NET_MSG_CHA_BAINFO_HA;
        for (int i=0; i<MAX_ONESERVERCHAR_NUM; ++i)
        {
            m_ChaDbNum[i] = 0;
        }
        StringCchCopy(m_Ip, MAX_IP_LENGTH+1, IpAddress.c_str());
        MIN_STATIC_ASSERT(sizeof(NET_CHA_BBA_INFO_HA)<=NET_DATA_BUFSIZE);
    }

    void SetChaNum(size_t Index, int ChaDbNum)
    {
        if (Index >=MAX_ONESERVERCHAR_NUM)
            return;
        else
            m_ChaDbNum[Index] = ChaDbNum;
    }

    void SetChaTotalNum(size_t TotalNum)
    {        
        if (TotalNum >= MAX_ONESERVERCHAR_NUM)
            m_ChaServerTotalNum = MAX_ONESERVERCHAR_NUM;
        else
            m_ChaServerTotalNum = static_cast<int> (TotalNum);
    }
};

//! 현재 서버에서 해당 유저가 가진 캐릭터의 번호를 넘겨준다.
//! Agent->Client
struct NET_CHA_BBA_INFO_AC : public NET_MSG_GENERIC
{
    int	m_ChaServerTotalNum; // 현재 이 서버에서 생성된 캐릭터 갯수
    int	m_ChaDbNum[MAX_ONESERVERCHAR_NUM];
    NET_CHA_BBA_INFO_AC()        
        : m_ChaServerTotalNum(0)
    {		
        dwSize = sizeof(NET_CHA_BBA_INFO_AC);
        nType = NET_MSG_CHA_BAINFO_AC;
        for (int i=0; i<MAX_ONESERVERCHAR_NUM; ++i)
        {
            m_ChaDbNum[i] = 0;
        }
        MIN_STATIC_ASSERT(sizeof(NET_CHA_BBA_INFO_AC)<=NET_DATA_BUFSIZE);
    }

    void SetChaNum(size_t Index, int ChaDbNum)
    {
        if (Index >=MAX_ONESERVERCHAR_NUM)
            return;
        else
            m_ChaDbNum[Index] = ChaDbNum;
    }

    void SetChaTotalNum(size_t TotalNum)
    {        
        if (TotalNum >= MAX_ONESERVERCHAR_NUM)
            m_ChaServerTotalNum = MAX_ONESERVERCHAR_NUM;
        else
            m_ChaServerTotalNum = static_cast<int> (TotalNum);
    }
};

//! 디폴트에서 분리된 설정값 전송
//! Agent->Client
struct NET_DEFAULT_GAME_OPTION : public NET_MSG_GENERIC
{
	int bPARTY_2OTHERSCHOOL;
	int bCLUB_2OTHERSCHOOL;
	float fITEM_DROP_SCALE;
	float fMONEY_DROP_SCALE;
	float fITEM_DROP_LOWER_SCALE;
	DWORD dwCRITICAL_MAX;
	float fNONPK_ITEM_DROP;
	float fPK_ITEM_DROP;

	NET_DEFAULT_GAME_OPTION()        
	{		
		dwSize = sizeof( NET_DEFAULT_GAME_OPTION );
		nType = NET_MSG_DEFAULT_GAME_OPTION;
		bPARTY_2OTHERSCHOOL = FALSE;
		bCLUB_2OTHERSCHOOL = FALSE;
		fITEM_DROP_SCALE = 1.0f;
		fMONEY_DROP_SCALE = 1.0f;
		fITEM_DROP_LOWER_SCALE = 1.0f;
		dwCRITICAL_MAX = 40;
		fNONPK_ITEM_DROP = 0.0f;
		fPK_ITEM_DROP = 0.0f;
		MIN_STATIC_ASSERT(sizeof(NET_DEFAULT_GAME_OPTION)<=NET_DATA_BUFSIZE);
	}
};

// 아이템 변환을 위해서 잠시 메시지 릴레이를 한다.
// NET_CHA_BBA_INFO_DA 의 내용 중 일부분이다.
struct NET_CHA_BBA_INFO_DA : public NET_MSG_GENERIC
{
	DWORD m_AgentServerSlot;
	int	m_ChaServerTotalNum;
	int	m_ChaDbNum[MAX_ONESERVERCHAR_NUM];
	NET_CHA_BBA_INFO_DA()
		: m_AgentServerSlot( GAEAID_NULL )
		, m_ChaServerTotalNum( 0 )
	{		
		dwSize = sizeof( NET_CHA_BBA_INFO_DA );
		nType = NET_MSG_CHA_BAINFO_DA;
		for ( int i = 0; i < MAX_ONESERVERCHAR_NUM; ++i )
		{
			m_ChaDbNum[i] = 0;
		}
		MIN_STATIC_ASSERT( sizeof( NET_CHA_BBA_INFO_DA ) <= NET_DATA_BUFSIZE );
	}

	void SetChaNum( size_t Index, int ChaDbNum )
	{
		if ( Index >=MAX_ONESERVERCHAR_NUM )
			return;
		else
			m_ChaDbNum[Index] = ChaDbNum;
	}

	void SetChaTotalNum( size_t TotalNum )
	{        
		if ( TotalNum >= MAX_ONESERVERCHAR_NUM )
			m_ChaServerTotalNum = MAX_ONESERVERCHAR_NUM;
		else
			m_ChaServerTotalNum = static_cast< int >( TotalNum );
	}
};

//! Agent-Field
struct NET_I_AM_AGENT : public NET_MSG_GENERIC
{
    enum { MAGIC_NUM = 32498, };

    DWORD MagicNum;
	DWORD dwSvrGroupNum;	//for MatchServer
    unsigned int CrcValue;

    NET_I_AM_AGENT()
        : MagicNum(MAGIC_NUM)
		, dwSvrGroupNum(0)
        , CrcValue(0)
    {
        nType = NET_MSG_I_AM_AGENT;
        dwSize = sizeof(NET_I_AM_AGENT);
        MIN_STATIC_ASSERT(sizeof(NET_I_AM_AGENT)<=NET_DATA_BUFSIZE);
    }
};

//! Instance-Match
struct NET_NEW_INSTANCE: public NET_MSG_GENERIC
{
    enum { MAGIC_NUM = 32698, };

    DWORD MagicNum;
	DWORD dwSvrNum;		//InstanceServer Number
    unsigned int CrcValue;

    NET_NEW_INSTANCE(DWORD _dwSvrNum)
        : MagicNum(MAGIC_NUM)
		, dwSvrNum(_dwSvrNum)
        , CrcValue(0)
    {
        nType = NET_MSG_NEW_INSTANCE;
        dwSize = sizeof(NET_NEW_INSTANCE);
        MIN_STATIC_ASSERT(sizeof(NET_NEW_INSTANCE)<=NET_DATA_BUFSIZE);
    }
};

struct NET_DISCONNECT_CHARACTER : public NET_MSG_GENERIC
{
	DWORD ChaDbNum;
	NET_DISCONNECT_CHARACTER()
	{
		nType  = NET_MSG_DISCONNECT_CHARACTER;
		dwSize = sizeof( NET_DISCONNECT_CHARACTER );
		MIN_STATIC_ASSERT( sizeof( NET_DISCONNECT_CHARACTER ) <= NET_DATA_BUFSIZE );
	}
};

namespace GLMSG
{
	enum
	{
		NET_MSG_FIELD_STATE_MEMORY,
		NET_MSG_FIELD_STATE_MEMORY_PER_LAND,
	};
	//! Field to Agent;
	struct NET_FIELD_STATE : public NET_MSG_GENERIC
	{
		enum { MAGIC_NUM = 12098 };
		const DWORD nSubType;
		const DWORD nFieldServer;
		const DWORD MagicNum;
		const DWORD nFieldChannel;		

		NET_FIELD_STATE(const DWORD _nSize, const DWORD _nSubType, const DWORD _nFieldServer, const DWORD _nFieldChannel)
			: nSubType(_nSubType)			
			, nFieldServer(_nFieldServer)
			, MagicNum(MAGIC_NUM)
			, nFieldChannel(_nFieldChannel)
		{
			nType = NET_MSG_FIELD_STATE;
			dwSize = _nSize;
			MIN_STATIC_ASSERT(sizeof(NET_FIELD_STATE) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_FIELD_STATE_MEMORY : public NET_FIELD_STATE
	{
		int nUsedMemory; // 사용중인 메모리 양;

		NET_FIELD_STATE_MEMORY(const DWORD _nFieldServer, const DWORD _nFieldChannel)
			: NET_FIELD_STATE(sizeof(NET_FIELD_STATE_MEMORY), GLMSG::NET_MSG_FIELD_STATE_MEMORY, _nFieldServer, _nFieldChannel)
		{
			MIN_STATIC_ASSERT(sizeof(NET_FIELD_STATE_MEMORY) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_FIELD_STATE_MEMORY_PER_LAND : public NET_FIELD_STATE
	{
		struct MemoryUsedPerLand
		{
			DWORD nMainMapID;
			DWORD nUsedMemory;
		};
		enum
		{
			NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_FIELD_STATE) - sizeof(DWORD)) / sizeof(MemoryUsedPerLand),
			HEADER_NSIZE = sizeof(NET_FIELD_STATE) + sizeof(DWORD),
		};
		DWORD nLand;
		MemoryUsedPerLand memoryUsedPerLand[NSIZE];

		const bool addLand(const DWORD _nMainMapID, const DWORD _nUsedMemory)
		{
			if ( this->nLand == NSIZE )
				return false;

			const DWORD _nIndex(this->nLand);
			this->memoryUsedPerLand[_nIndex].nMainMapID = _nMainMapID;
			this->memoryUsedPerLand[_nIndex].nUsedMemory = _nUsedMemory;

			this->dwSize += sizeof(MemoryUsedPerLand);
			++this->nLand;

			if ( this->nLand == NSIZE )
				return false;

			return true;
		}

		void reset(void)
		{
			this->nLand = 0;
			this->dwSize = HEADER_NSIZE;
		}

		const bool isValid(void) const
		{
			return this->nLand != 0;
		}

		NET_FIELD_STATE_MEMORY_PER_LAND(void)
			: NET_FIELD_STATE(HEADER_NSIZE, GLMSG::NET_MSG_FIELD_STATE_MEMORY_PER_LAND, -1, -1)
            , nLand(0)
		{
            memset(memoryUsedPerLand, 0, sizeof(memoryUsedPerLand));
			MIN_STATIC_ASSERT(sizeof(NET_FIELD_STATE_MEMORY_PER_LAND) <=  NET_DATA_BUFSIZE);
		}
	};

	struct NET_JACKPOT_PUSH : public NET_MSG_GENERIC
	{
		SNATIVEID ItemID;
		SNATIVEID RequiredLevel;
		DWORD GameMoney;
		bool bMain;

		NET_JACKPOT_PUSH()
			: ItemID( SNATIVEID( false ) )
			, RequiredLevel( SNATIVEID( false ) )
			, GameMoney( 0 )
			, bMain( true )
		{
			dwSize = sizeof( NET_JACKPOT_PUSH );
			nType = NET_MSG_GCTRL_JACKPOT_PUSH;
			MIN_STATIC_ASSERT( sizeof( NET_JACKPOT_PUSH ) <= NET_DATA_BUFSIZE );
		}
	};

	struct NET_JACKPOT_NOTIFY : public NET_MSG_GENERIC
	{
		SNATIVEID MapID;
		SNATIVEID CrowID;
		SNATIVEID ItemID;
		char szName[CHAR_SZNAME];

		NET_JACKPOT_NOTIFY()
			: MapID( SNATIVEID( false ) )
			, CrowID( SNATIVEID( false ) )
			, ItemID( SNATIVEID( false ) )
		{
			dwSize = sizeof( NET_JACKPOT_NOTIFY );
			nType = NET_MSG_GCTRL_JACKPOT_NOTIFY;

			::memset( szName, 0x00, sizeof( char ) * CHAR_SZNAME );

			MIN_STATIC_ASSERT( sizeof( NET_JACKPOT_NOTIFY ) <= NET_DATA_BUFSIZE );
		}

		void SetName( const char* _szName )
		{
			if ( _szName )
				StringCchCopy( szName, CHAR_SZNAME, _szName );
		}
	};

	struct NET_JACKPOT_OPTION : public NET_MSG_GENERIC
	{
		int Case;
		SNATIVEID ItemID;
		SNATIVEID RequiredLevel;
		float DelaySeconds;
		DWORD GameMoney;
		bool bMain;

		NET_JACKPOT_OPTION()
			: Case( -1 )
			, ItemID( SNATIVEID( false ) )
			, RequiredLevel( SNATIVEID( false ) )
			, DelaySeconds( 86400.0f )
			, GameMoney( 0 )
			, bMain( true )
		{
			dwSize = sizeof( NET_JACKPOT_OPTION );
			nType = NET_MSG_GCTRL_JACKPOT_OPTION;
			MIN_STATIC_ASSERT( sizeof( NET_JACKPOT_OPTION ) <= NET_DATA_BUFSIZE );
		}
	};
}


#endif // _GL_SERVER_MSG_H_
