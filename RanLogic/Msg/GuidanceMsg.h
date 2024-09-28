#pragma once

#include "../s_NetGlobal.h"

//! ����Ŭ�� �� ���� ��û.
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
		std::string				strName;	// ������ �̸�
		float					fCommision;	// ��������
		std::vector< DWORD >	vecMap;		// �����Ḧ ¡���ϴ� ��

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

//! GM�� ����Ŭ�� ���� ��û
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
    DWORD m_Id;                   //! ���� id
    DWORD m_ClubId;               //! ����Ŭ�� id
    float m_Commission;           //! ��������
    DWORD m_Country;              //! �����ڵ�
    std::string m_ClubName;       //! ����Ŭ�� �̸�
    std::string m_GuidName;       //! �����̸�    
    std::vector<DWORD> m_GuidMap; //! �����Ḧ ¡���ϴ� ��

    MSGPACK_DEFINE(m_Id, m_ClubId, m_Commission, m_Country, m_ClubName, m_GuidName, m_GuidMap);

    NET_GM_GUIDANCE_INFO_AC()
        : m_Id(0)
        , m_ClubId(0)
        , m_Commission(0.0f)
        , m_Country(0)
    {
    }
};

//! ����Ŭ�� ����
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

//!������ ON/OFF
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