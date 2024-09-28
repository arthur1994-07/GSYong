#ifndef _RAN_LOGIC_MSG_WORLD_BATTLE_H_
#define _RAN_LOGIC_MSG_WORLD_BATTLE_H_

#pragma once

#include "../../SigmaCore/gassert.h"
#include "../WorldBattle/WorldBattleDefine.h"
#include "../s_NetGlobal.h"

namespace wb
{
    //! Client->Agent Club country info
    struct NET_CLUB_COUNTRY_COUNT_CA : public NET_MSG_GENERIC
    {        
        NET_CLUB_COUNTRY_COUNT_CA()
        {
            dwSize = sizeof(NET_CLUB_COUNTRY_COUNT_CA);
            nType = NET_MSG_CLUB_COUNTRY_COUNT_CA;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_COUNTRY_COUNT_CA)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent->Client Club country info
    struct NET_CLUB_COUNTRY_COUNT_START_AC : public NET_MSG_GENERIC
    {
        NET_CLUB_COUNTRY_COUNT_START_AC()
        {
            dwSize = sizeof(NET_CLUB_COUNTRY_COUNT_START_AC);
            nType = NET_MSG_CLUB_COUNTRY_COUNT_START_AC;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_COUNTRY_COUNT_START_AC)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent->Client Club country info
    struct NET_CLUB_COUNTRY_COUNT_AC : public NET_MSG_GENERIC
    {
        COUNTRY_CLUB_COUNT_INFO m_Data;
        NET_CLUB_COUNTRY_COUNT_AC(const COUNTRY_CLUB_COUNT_INFO& Data)
            : m_Data(Data)
        {
            dwSize = sizeof(NET_CLUB_COUNTRY_COUNT_AC);
            nType = NET_MSG_CLUB_COUNTRY_COUNT_AC;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_COUNTRY_COUNT_AC)<=NET_DATA_BUFSIZE);
        }
    };

    //! Agent->Client Club country info
    struct NET_CLUB_COUNTRY_COUNT_END_AC : public NET_MSG_GENERIC
    {
        NET_CLUB_COUNTRY_COUNT_END_AC()
        {
            dwSize = sizeof(NET_CLUB_COUNTRY_COUNT_END_AC);
            nType = NET_MSG_CLUB_COUNTRY_COUNT_END_AC;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_COUNTRY_COUNT_END_AC)<=NET_DATA_BUFSIZE);
        }
    };

    //! Client->Agent Club country info
    struct NET_CLUB_COUNTRY_CA : public NET_MSG_GENERIC
    {
        EM_COUNTRY m_Country;
        NET_CLUB_COUNTRY_CA(EM_COUNTRY emCountry)
            : m_Country(emCountry)
        {
            dwSize = sizeof(NET_CLUB_COUNTRY_CA);
            nType = NET_MSG_CLUB_COUNTRY_CA;
            MIN_STATIC_ASSERT(sizeof(NET_CLUB_COUNTRY_CA)<=NET_DATA_BUFSIZE);
        }
    };

    //! Client->Agent Request club name
    struct NET_REQ_DROP_CHAR_CLUB_NAME_CA
    {
        std::vector<DWORD> m_vecClubDbNum;
        std::vector<DWORD> m_vecGaeaId;

        MSGPACK_DEFINE(m_vecClubDbNum, m_vecGaeaId)
        //NET_MSG_REQ_DROP_CHAR_CLUB_NAME_CA
        NET_REQ_DROP_CHAR_CLUB_NAME_CA()
        {}
        void Add( DWORD dwClubDbNum, DWORD dwGaeId )
        {
            m_vecGaeaId.push_back( dwGaeId );
            m_vecClubDbNum.push_back( dwClubDbNum );
        }
        void Clear()
        {
            m_vecClubDbNum.clear();
            m_vecGaeaId.clear();
        }
    };

    struct NET_UPDATE_DROP_CHAR_CLUB_NAME_AC : public NET_MSG_GENERIC
    {        
        DWORD m_GaeaId;
        DWORD m_ClubDbNum;
        NET_UPDATE_DROP_CHAR_CLUB_NAME_AC(DWORD GaeaId, DWORD ClubDbNum)
            : m_GaeaId(GaeaId)
            , m_ClubDbNum(ClubDbNum)
        {
            dwSize = sizeof(NET_UPDATE_DROP_CHAR_CLUB_NAME_AC);
            nType = NET_MSG_UPDATE_DROP_CHAR_CLUB_NAME_AC;
            MIN_STATIC_ASSERT(sizeof(NET_UPDATE_DROP_CHAR_CLUB_NAME_AC)<=NET_DATA_BUFSIZE);
        }
    };

    //! Client->Agent 유저 국가 변경 요청
    struct NET_USER_CHANGE_COUNTRY_CA : public NET_MSG_GENERIC
    {
        wb::EM_COUNTRY m_Country;

        NET_USER_CHANGE_COUNTRY_CA(wb::EM_COUNTRY Country)
            : m_Country(Country)
        {
            dwSize = sizeof(NET_USER_CHANGE_COUNTRY_CA);
            nType = NET_MSG_USER_CHANGE_COUNTRY_CA;
            MIN_STATIC_ASSERT(sizeof(NET_USER_CHANGE_COUNTRY_CA)<=NET_DATA_BUFSIZE);
        }
    };

    enum EM_USER_COUNTRY_CHANGE_RESULT
    {
        USER_COUNTRY_CHANGE_SUCCESS = 1,
        USER_COUNTRY_CHANGE_USER_UNKNOWN_COUNTRY, //! 이미 국가가 설정되어 있음
        USER_COUNTRY_CHANGE_NEW_UNKNOWN_COUNTRY, //! 바꾸려는 국가를 알수없음
        USER_COUNTRY_CHANGE_SAME_COUNTRY, //! 바꾸려는 국가가 기존 국가와 같음
        USER_COUNTRY_CHANGE_NOT_EXIST_COUNTRY, //! 설정파일에 설정되지 않는 국가임
    };

    //! Agent->Client 유저 국가 변경 요청
    struct NET_USER_CHANGE_COUNTRY_AC : public NET_MSG_GENERIC
    {
        EM_USER_COUNTRY_CHANGE_RESULT m_Result;
        wb::EM_COUNTRY m_Country;

        NET_USER_CHANGE_COUNTRY_AC(
            wb::EM_COUNTRY Country,
            EM_USER_COUNTRY_CHANGE_RESULT Result)
            : m_Country(Country)
            , m_Result(Result)
        {
            dwSize = sizeof(NET_USER_CHANGE_COUNTRY_AC);
            nType = NET_MSG_USER_CHANGE_COUNTRY_AC;
            MIN_STATIC_ASSERT(sizeof(NET_USER_CHANGE_COUNTRY_AC)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNET_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD : public NET_MSG_GENERIC
	{	
		DWORD dwGuidanceID;
		DWORD dwCountryCode;

		SNET_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD ()
		{
			dwSize = sizeof(SNET_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD);
			nType = NET_MSG_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD)<=NET_DATA_BUFSIZE);
		}
	};

} // namespace wb
#endif