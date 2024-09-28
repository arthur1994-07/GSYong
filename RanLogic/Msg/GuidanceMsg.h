#pragma once

#include "../s_NetGlobal.h"

//! 선도클럽 맵 정보 요청.
struct NET_GUIDANCE_MAP_INFO_CA : public NET_MSG_CHARACTER
{
    DWORD ClubDbNum;

    NET_GUIDANCE_MAP_INFO_CA( DWORD _ClubDbNum )
        : ClubDbNum(_ClubDbNum)
    {
        MIN_STATIC_ASSERT(sizeof(NET_GUIDANCE_MAP_INFO_CA)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_GUIDANCE_MAP_INFO_CA);
        nType  = NET_MSG_GUIDANCE_MAP_INFO_CA; // EMNET_MSG
    }
};

struct NET_GUIDANCE_MAP_INFO_AC
{
	struct SGuidanceInfo
	{
		std::string				strName;	// 선도전 이름
		float					fCommision;	// 수수료율
		std::vector< DWORD >	vecMap;		// 수수료를 징수하는 맵

		MSGPACK_DEFINE( strName, fCommision, vecMap );

		SGuidanceInfo()
			: fCommision( 0.0f )
		{
		}
	};

	std::vector< SGuidanceInfo > m_GuidMap;

    MSGPACK_DEFINE( m_GuidMap );

    NET_GUIDANCE_MAP_INFO_AC()
    {
    }
};

//! GM이 선도클럽 정보 요청
struct NET_GM_GUIDANCE_INFO_CA : public NET_MSG_GENERIC
{
    NET_GM_GUIDANCE_INFO_CA()
    {
        MIN_STATIC_ASSERT(sizeof(NET_GM_GUIDANCE_INFO_CA)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_GM_GUIDANCE_INFO_CA);
        nType  = NET_MSG_GM_GUIDANCE_INFO_CA; // EMNET_MSG
    }
};

struct NET_GM_GUIDANCE_INFO_AC
{
    DWORD m_Id;                   //! 관리 id
    DWORD m_ClubId;               //! 선도클럽 id
    float m_Commission;           //! 수수료율
    DWORD m_Country;              //! 국가코드
    std::string m_ClubName;       //! 선도클럽 이름
    std::string m_GuidName;       //! 관리이름    
    std::vector<DWORD> m_GuidMap; //! 수수료를 징수하는 맵

    MSGPACK_DEFINE(m_Id, m_ClubId, m_Commission, m_Country, m_ClubName, m_GuidName, m_GuidMap);

    NET_GM_GUIDANCE_INFO_AC()
        : m_Id(0)
        , m_ClubId(0)
        , m_Commission(0.0f)
        , m_Country(0)
    {
    }
};

//! 선도클럽 변경
struct NET_GM_GUIDANCE_CHANGE_CA : public NET_MSG_GENERIC
{
    DWORD m_GuidanceId;
    DWORD m_ClubDbNum;

    NET_GM_GUIDANCE_CHANGE_CA(DWORD GuidanceId, DWORD ClubDbNum)
        : m_GuidanceId(GuidanceId)
        , m_ClubDbNum(ClubDbNum)
    {
        MIN_STATIC_ASSERT(sizeof(NET_GM_GUIDANCE_CHANGE_CA)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_GM_GUIDANCE_CHANGE_CA);
        nType  = NET_MSG_GM_GUIDANCE_CHANGE_CA; // EMNET_MSG
    }
};

//!선도전 ON/OFF
struct NET_GM_GUIDANCE_ON_OFF_CA : public NET_MSG_GENERIC
{
	enum
	{
		END = 0,
		START = 1,
		ALL_START = 2,
		ALL_END = 3,
	};

	DWORD m_GuidanceId;
	int m_nFlag;

	NET_GM_GUIDANCE_ON_OFF_CA(DWORD GuidanceId, int nFlag)
		: m_GuidanceId(GuidanceId)
		, m_nFlag(nFlag)
	{
		MIN_STATIC_ASSERT(sizeof(NET_GM_GUIDANCE_ON_OFF_CA)<=NET_DATA_BUFSIZE);

		dwSize = sizeof(NET_GM_GUIDANCE_ON_OFF_CA);
		nType  = NET_MSG_GM_GUIDANCE_ON_OFF_CA; // EMNET_MSG
	}
};