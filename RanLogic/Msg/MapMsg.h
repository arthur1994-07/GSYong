
#pragma once

#include "../s_NetGlobal.h"
#include "../Character/GLCharData.h"
#include "../Party/GLPartyDefine.h"

namespace GLMSG
{


//////////////////////////////////////////////////////////////////////////
struct NET_MAP_SEARCH_NAME_REQ_CA : public NET_MSG_GENERIC
{
    CHAR  m_szChaName[CHR_ID_LENGTH+1]; // 캐릭터명
	DWORD m_SenderChaDBNum;
	BOOL	m_bGM;
    DWORD m_mapID;

	NET_MAP_SEARCH_NAME_REQ_CA()
	{
		static const int Size = sizeof(NET_MAP_SEARCH_NAME_REQ_CA);
		dwSize = Size;
		nType  = NET_MSG_MAP_SEARCH_NAME_REQ_CA;
		MIN_STATIC_ASSERT( Size <=NET_DATA_BUFSIZE);

		ZeroMemory(m_szChaName,sizeof(m_szChaName));
		m_mapID = (DWORD)-1;
		m_SenderChaDBNum = 0;
		m_bGM = FALSE;
	}

    void SetName(const std::string& SearchName)
    {
        StringCchCopy(m_szChaName, CHR_ID_LENGTH+1, SearchName.c_str());
    }
};

struct NET_MAP_SEARCH_NAME_REQ_CF : public NET_MSG_GENERIC
{
    CHAR  m_szChaName[CHR_ID_LENGTH+1]; // 캐릭터명
	DWORD	m_dwGaeaID;
	DWORD m_SenderChaDBNum;
	BOOL	m_bGM;
    DWORD m_mapID;

	NET_MAP_SEARCH_NAME_REQ_CF()
	{
		static const int Size = sizeof(NET_MAP_SEARCH_NAME_REQ_CF);
		dwSize = Size;
		nType  = NET_MSG_MAP_SEARCH_NAME_REQ_CF;
		MIN_STATIC_ASSERT( Size <=NET_DATA_BUFSIZE);

		ZeroMemory(m_szChaName,sizeof(m_szChaName));
		m_mapID = (DWORD)-1;
		m_SenderChaDBNum = 0;
		m_dwGaeaID = 0;
		m_bGM = FALSE;
	}

    void SetName(const std::string& SearchName)
    {
        StringCchCopy(m_szChaName, CHR_ID_LENGTH+1, SearchName.c_str());
    }
};

struct NET_MAP_SEARCH_NAME_1_ACK_AC : public NET_MSG_GENERIC
{
	NET_MAP_SEARCH_NAME_1_ACK_AC()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_SEARCH_NAME_1_ACK_AC;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);
		
		
		m_NameCount = 0;
	}

	int m_NameCount;
};

struct NET_MAP_SEARCH_NAME_2_ACK_AC //: public NET_MSG_GENERIC
{
    int	        m_Level;
    std::string m_ChaName; //[CHR_ID_LENGTH+1]; // 캐릭터명

    MSGPACK_DEFINE(m_Level, m_ChaName);

	NET_MAP_SEARCH_NAME_2_ACK_AC()
        : m_Level(0)
	{
		//static const int Size = sizeof(*this);
		//dwSize = Size;
		//nType  = NET_MSG_MAP_SEARCH_NAME_2_ACK_AC;
		//MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);		
		//ZeroMemory(m_ChaName,sizeof(m_ChaName));
	}	
};


struct NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_CA : public NET_MSG_GENERIC
{
	NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_CA()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_REQ_CA;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		ZeroMemory(m_szChaName,sizeof(m_szChaName));
	}

	//////////////////////////////////////////////////////////////////////////
	CHAR				m_szChaName[CHR_ID_LENGTH+1]; // 캐릭터명

};

struct NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_AF : public NET_MSG_GENERIC
{
	NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_AF()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_REQ_AF;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		ZeroMemory(m_szChaName,sizeof(m_szChaName));
		m_AskerDBNum = (DWORD)-1;
	}

	CHAR				m_szChaName[CHR_ID_LENGTH+1]; // 캐릭터명
	DWORD				m_AskerDBNum;
};

struct NET_MAP_SEARCH_NAME_DETAIL_INFO_ACK_FA //: public NET_MSG_GENERIC
{
    // title  badge
    int  m_Level;
    int  m_wSchool;
    int  m_emClass;
    bool m_isParty;
    DWORD m_dwGaeaID;
    DWORD m_AskerDBNum;
    std::string m_ChaName; //[CHR_ID_LENGTH+1]; // 캐릭터명
    std::string m_Title; //[ EM_TITLEID_MAXLENGTH+1 ];
    std::string m_ClubName; //[CHAR_SZNAME+1];

    MSGPACK_DEFINE(m_Level, m_wSchool, m_emClass, m_isParty, m_dwGaeaID, m_AskerDBNum, m_ChaName, m_Title, m_ClubName);

	NET_MAP_SEARCH_NAME_DETAIL_INFO_ACK_FA()
	{
        //nType = NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_ACK_FA;		
		m_Level = 0;
		m_wSchool = -1;
		m_emClass = -1;
		m_isParty = false;
		m_dwGaeaID = (DWORD)-1;
		m_AskerDBNum = (DWORD)-1;
	}

};

struct NET_MAP_SEARCH_NAME_DETAIL_INFO_ACK_AC //: public NET_MSG_GENERIC
{
    int  m_Level;
    int  m_wSchool;
    int  m_emClass;
    DWORD m_dwGaeaID;
    bool m_isParty;
    std::string m_ChaName; //[CHR_ID_LENGTH+1]; 
    std::string m_Title; //[ EM_TITLEID_MAXLENGTH+1 ];    
    std::string m_ClubName; //[CHAR_SZNAME+1];    

    MSGPACK_DEFINE(m_Level, m_wSchool, m_emClass, m_dwGaeaID, m_isParty, m_ChaName, m_Title, m_ClubName);

	NET_MAP_SEARCH_NAME_DETAIL_INFO_ACK_AC()
	{
		//nType  = NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_ACK_AC;
		//MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);
		m_Level = 0;
		m_wSchool = -1;
		m_emClass = -1;
		m_isParty = false;
		m_dwGaeaID = (DWORD)-1;
	}
};

//////////////////////////////////////////////////////////////////////////


struct NET_MAP_SEARCH_NAME_POSITION_REQ_CA : public NET_MSG_GENERIC
{
	NET_MAP_SEARCH_NAME_POSITION_REQ_CA()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_SEARCH_NAME_POSITION_REQ_CA;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		ZeroMemory(m_szChaName,sizeof(m_szChaName));
	}

	//////////////////////////////////////////////////////////////////////////
	CHAR				m_szChaName[CHR_ID_LENGTH+1]; // 캐릭터명

};


struct NET_MAP_SEARCH_NAME_POSITION_REQ_AF : public NET_MSG_GENERIC
{
	NET_MAP_SEARCH_NAME_POSITION_REQ_AF()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_SEARCH_NAME_POSITION_REQ_AF;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		ZeroMemory(m_szChaName,sizeof(m_szChaName));
		m_AskerDBNum = (DWORD)-1;

	}

	//////////////////////////////////////////////////////////////////////////
	CHAR				m_szChaName[CHR_ID_LENGTH+1]; // 캐릭터명
	DWORD			m_AskerDBNum;

};

struct NET_MAP_SEARCH_NAME_POSITION_ACK_FA : public NET_MSG_GENERIC
{
	NET_MAP_SEARCH_NAME_POSITION_ACK_FA()
		: m_vPosition(0.0f, 0.0f, 0.0f)
		, m_AskerDBNum(DWORD(-1))
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_SEARCH_NAME_POSITION_ACK_FA;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		ZeroMemory(m_szChaName,sizeof(m_szChaName));
	}

	//////////////////////////////////////////////////////////////////////////
	int					m_Level;
	CHAR				m_szChaName[CHR_ID_LENGTH+1]; // 캐릭터명
	MapID			m_mapID;
	D3DXVECTOR3	m_vPosition;
	DWORD			m_AskerDBNum;
};

struct NET_MAP_SEARCH_NAME_POSITION_ACK_AC : public NET_MSG_GENERIC
{
	NET_MAP_SEARCH_NAME_POSITION_ACK_AC()
		: m_Level(0)
		, m_vPosition(0.0f, 0.0f, 0.0f)
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_SEARCH_NAME_POSITION_ACK_AC;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		ZeroMemory(m_szChaName,sizeof(m_szChaName));
	}

	//////////////////////////////////////////////////////////////////////////
	int					m_Level;
	CHAR				m_szChaName[CHR_ID_LENGTH+1]; // 캐릭터명
	DWORD			m_mapID;
	D3DXVECTOR3			m_vPosition;
};

struct NET_MAP_CREATE_PARTY_BOARD_REQ_CA //: public NET_MSG_GENERIC
{
    // 100 을 직접 쓴이유는 특정 디파인 상수값을 정의해서 서버 클라가 동시에  참조하는 파일을 만들기엔 애매함
    std::string m_Title; //[100 + 1];
    DWORD m_baseMapID;

    MSGPACK_DEFINE(m_Title, m_baseMapID);

	NET_MAP_CREATE_PARTY_BOARD_REQ_CA()
	{
        //nType  = NET_MSG_MAP_CREATE_PARTY_BOARD_REQ_CA;
	}
};

//////////////////////////////////////////////////////////////////////////

struct NET_MAP_DELETE_PARTY_BOARD_REQ_CA : public NET_MSG_GENERIC
{
	NET_MAP_DELETE_PARTY_BOARD_REQ_CA()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_DELETE_PARTY_BOARD_REQ_CA;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		m_forRefreshMapID = (DWORD)-1;
	}

	DWORD m_forRefreshMapID;
};

struct NET_MAP_REFRESH_PARTY_BOARD_REQ_CA : public NET_MSG_GENERIC
{
	NET_MAP_REFRESH_PARTY_BOARD_REQ_CA()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_REFRESH_PARTY_BOARD_REQ_CA;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		m_MapID = (DWORD)-1;
	}

	DWORD m_MapID;
};


//////////////////////////////////////////////////////////////////////////

struct NET_MAP_PARTY_INFO_LIST_BOARD_1_ACK_AC : public NET_MSG_GENERIC
{	
	NET_MAP_PARTY_INFO_LIST_BOARD_1_ACK_AC()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_PARTY_INFO_LIST_BOARD_1_ACK_AC;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		m_MapID = (DWORD)-1;
		m_Count = (DWORD)-1;

		m_isExistMyBoard = false;
	}


	bool   m_isExistMyBoard;
	DWORD  m_MapID;
	DWORD  m_Count;
};

struct NET_MAP_PARTY_INFO_LIST_BOARD_2_ACK_AC //: public NET_MSG_GENERIC
{
    DWORD m_MaxPlayer;
    DWORD m_CurrentPlayer;
    std::string m_ChaName; //[CHR_ID_LENGTH+1]; // 캐릭터명
    std::string m_Title; //[100 + 1];

    MSGPACK_DEFINE(m_MaxPlayer, m_CurrentPlayer, m_ChaName, m_Title);

	NET_MAP_PARTY_INFO_LIST_BOARD_2_ACK_AC()
	{
		//nType  = NET_MSG_MAP_PARTY_INFO_LIST_BOARD_2_ACK_AC;		
		m_MaxPlayer = 8;
		m_CurrentPlayer = 0;		
	}
};

struct NET_MAP_PARTY_INFO_DETAIL_REQ_CA : public NET_MSG_GENERIC
{	
	NET_MAP_PARTY_INFO_DETAIL_REQ_CA()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_PARTY_INFO_DETAIL_REQ_CA;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		ZeroMemory(m_szChaName,sizeof(m_szChaName));
		m_MapID = (DWORD)-1;
	}


	CHAR  m_szChaName[CHR_ID_LENGTH+1]; // 캐릭터명
	DWORD m_MapID;
};


struct NET_MAP_PARTY_INFO_DETAIL_1_ACK_AC : public NET_MSG_GENERIC
{	
	NET_MAP_PARTY_INFO_DETAIL_1_ACK_AC()
	{
		static const int Size = sizeof(*this);
		dwSize = Size;
		nType  = NET_MSG_MAP_PARTY_INFO_DETAIL_1_ACK_AC;
		MIN_STATIC_ASSERT(Size<=NET_DATA_BUFSIZE);

		m_MapID = (DWORD)-1;
		m_Count = 0;
	}

	DWORD m_MapID;
	DWORD m_Count;
};

struct NET_MAP_PARTY_INFO_DETAIL_2_ACK_AC : public NET_MSG_GENERIC
{
    GLPARTY_LINK_MEM m_PartyMember;

    //MSGPACK_DEFINE(m_PartyMember);

	NET_MAP_PARTY_INFO_DETAIL_2_ACK_AC()
	{
        dwSize = sizeof(NET_MAP_PARTY_INFO_DETAIL_2_ACK_AC);
		nType  = NET_MSG_MAP_PARTY_INFO_DETAIL_2_ACK_AC;
	}
};

	

}//GLMSG


