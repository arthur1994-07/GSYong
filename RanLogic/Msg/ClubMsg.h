#ifndef _GLOGIC_CLUB_MSG_H_
#define _GLOGIC_CLUB_MSG_H_

#pragma once

#include "../s_NetGlobal.h"
#include "../Club/GLClubDefine.h"
/*
struct NET_CLUB_CHAR_LEVEL_CHANGE_AC : public NET_MSG_GENERIC
{
    DWORD m_ChaDbNum;
    WORD m_ChaLevel;

    NET_CLUB_CHAR_LEVEL_CHANGE_AC(DWORD ChaDbNum, WORD ChaLevel)
        : m_ChaDbNum(ChaDbNum)
        , m_ChaLevel(ChaLevel)
    {
        MIN_STATIC_ASSERT(sizeof(NET_CLUB_CHAR_LEVEL_CHANGE_AC)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_CLUB_CHAR_LEVEL_CHANGE_AC);
        nType = NET_MSG_CLUB_CHAR_LEVEL_CHANGE_AC;        
    }
};
*/
//! 클럽 권한 설정 Client->Agent
struct NET_CLUB_GRADE_FLAG_CHANGE_CA 
{
    std::vector<club::GRADE_FLAG> m_Data;

    MSGPACK_DEFINE(m_Data);

    NET_CLUB_GRADE_FLAG_CHANGE_CA()
    {
        //MIN_STATIC_ASSERT(sizeof(NET_CLUB_GRADE_FLAG_CHANGE_CA)<=NET_DATA_BUFSIZE);
        //dwSize = sizeof(NET_CLUB_GRADE_FLAG_CHANGE_CA);
        //nType  = NET_MSG_CLUB_GRADE_FLAG_CHANGE_CA;
    }
};

struct NET_CLUB_GRADE_FLAG_CHANGE_AC
{    
    std::string m_ChaName;
    std::vector<club::GRADE_FLAG> m_Data;

    MSGPACK_DEFINE(m_ChaName, m_Data);

    NET_CLUB_GRADE_FLAG_CHANGE_AC()
    {
    }
};

//! 클럽 권한 변경 Agent->Field
struct NET_CLUB_GRADE_FLAG_CHANGE_AF //: public NET_MSG_GENERIC
{
    DWORD m_ClubDbNum;
    std::vector<club::GRADE_FLAG> m_Data;

    MSGPACK_DEFINE(m_ClubDbNum, m_Data);
    
    NET_CLUB_GRADE_FLAG_CHANGE_AF()
        : m_ClubDbNum(0)
    {
    }

    NET_CLUB_GRADE_FLAG_CHANGE_AF(DWORD ClubDbNum)
        : m_ClubDbNum(ClubDbNum)
    {
        //MIN_STATIC_ASSERT(sizeof(NET_CLUB_GRADE_FLAG_CHANGE_AF)<=NET_DATA_BUFSIZE);
        //dwSize = sizeof(NET_CLUB_GRADE_FLAG_CHANGE_AF);
        //nType  = NET_MSG_CLUB_GRADE_FLAG_CHANGE_AF;
    }
};

//! 클럽 공개 변경 Client->Agent
struct NET_CLUB_PULIC_FLAG_CHANGE_CA : public NET_MSG_GENERIC
{
    DWORD m_PublicFlag;

    NET_CLUB_PULIC_FLAG_CHANGE_CA(DWORD PublicFlag)
        : m_PublicFlag(PublicFlag)
    {
        MIN_STATIC_ASSERT(sizeof(NET_CLUB_PULIC_FLAG_CHANGE_CA)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_CLUB_PULIC_FLAG_CHANGE_CA);
        nType  = NET_MSG_CLUB_PULIC_FLAG_CHANGE_CA;
    }
};

//! 클럽 공개 변경 Agent->Client
struct NET_CLUB_PULIC_FLAG_CHANGE_AC : public NET_MSG_GENERIC
{
    DWORD m_PublicFlag;
    char  m_ChaName[CHAR_SZNAME];

    NET_CLUB_PULIC_FLAG_CHANGE_AC(DWORD PublicFlag, const char* szChaName)
        : m_PublicFlag(PublicFlag)
    {
        MIN_STATIC_ASSERT(sizeof(NET_CLUB_PULIC_FLAG_CHANGE_AC)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_CLUB_PULIC_FLAG_CHANGE_AC);
        nType  = NET_MSG_CLUB_PULIC_FLAG_CHANGE_AC;

        if (szChaName)
            StringCchCopy(m_ChaName, CHAR_SZNAME, szChaName);
    }
};

//! 클럽 공개 변경 Agent->Field
struct NET_CLUB_PULIC_FLAG_CHANGE_AF : public NET_MSG_GENERIC
{
    DWORD m_ClubDbNum;
    DWORD m_PublicFlag;

    NET_CLUB_PULIC_FLAG_CHANGE_AF(DWORD ClubDbNum, DWORD PublicFlag)
        : m_ClubDbNum(ClubDbNum)
        , m_PublicFlag(PublicFlag)
    {
        MIN_STATIC_ASSERT(sizeof(NET_CLUB_PULIC_FLAG_CHANGE_AF)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_CLUB_PULIC_FLAG_CHANGE_AF);
        nType  = NET_MSG_CLUB_PULIC_FLAG_CHANGE_AF;
    }
};

//! 클럽 자동 탈퇴 설정 변경 Client->Agent
struct NET_CLUB_AUTO_KICKC_CA : public NET_MSG_GENERIC
{
    club::AUTO_KICK_OUT m_KickOut;

    NET_CLUB_AUTO_KICKC_CA(club::AUTO_KICK_OUT KickOut)
        : m_KickOut(KickOut)
    {
        MIN_STATIC_ASSERT(sizeof(NET_CLUB_AUTO_KICKC_CA)<=NET_DATA_BUFSIZE);

        dwSize = sizeof(NET_CLUB_AUTO_KICKC_CA);
        nType  = NET_MSG_CLUB_AUTO_KICK_CA;
    }
};

struct CLUB_AUTO_KICK_AC
{
    std::string m_ChaName;
    club::AUTO_KICK_OUT m_KickOut;

    MSGPACK_DEFINE(m_ChaName, m_KickOut);

    CLUB_AUTO_KICK_AC()
    {
    }

    CLUB_AUTO_KICK_AC(
        const std::string& ChaName,
        const club::AUTO_KICK_OUT& KickOut)
        : m_ChaName(ChaName)
        , m_KickOut(KickOut)
    {
    }    
};

struct NET_CLUB_AUTO_KICKC_AF
{
    DWORD m_ClubDbNum;
    club::AUTO_KICK_OUT m_KickOut;

    MSGPACK_DEFINE(m_ClubDbNum, m_KickOut);

    NET_CLUB_AUTO_KICKC_AF()
    {
    }

    NET_CLUB_AUTO_KICKC_AF(DWORD ClubDbNum, club::AUTO_KICK_OUT KickOut)
        : m_ClubDbNum(ClubDbNum)
        , m_KickOut(KickOut)
    {
    }
};

struct NET_CLUB_NEWBIE_NOTICE_CA
{
    std::string m_NoticeNewbie; //[EMCLUB_NOTICE_LEN];

    MSGPACK_DEFINE(m_NoticeNewbie);

    NET_CLUB_NEWBIE_NOTICE_CA()
    {
    }
};

struct NET_CLUB_NEWBIE_NOTICE_AC
{
    std::string m_ChaName;
    std::string m_NoticeNewbie; //[EMCLUB_NOTICE_LEN];

    MSGPACK_DEFINE(m_ChaName, m_NoticeNewbie);

    NET_CLUB_NEWBIE_NOTICE_AC()
    {
    }
};

struct NET_CLUB_NEWBIE_NOTICE_AF
{
    DWORD m_ClubDbNum;
    std::string m_NoticeNewbie; //[EMCLUB_NOTICE_LEN];

    MSGPACK_DEFINE(m_ClubDbNum, m_NoticeNewbie);

    NET_CLUB_NEWBIE_NOTICE_AF()
    {
    }
};

//! Client->Agent
struct NET_CLUB_LOG_CA
{
    BYTE m_LogType; // club::EM_LOG_TYPE
    WORD m_Page;
    WORD m_PageSize;

    MSGPACK_DEFINE(m_LogType, m_Page, m_PageSize);

    NET_CLUB_LOG_CA()
        : m_LogType(0)
        , m_Page(0)
        , m_PageSize(0)
    {
    }
};

//! Agent->Cache
struct NET_CLUB_LOG_AH 
{
	BYTE m_LogType; // club::EM_LOG_TYPE
    WORD m_Page;
    WORD m_PageSize;
    DWORD m_ClubDbNum;
    DWORD m_ChaDbNum;

    MSGPACK_DEFINE(m_LogType, m_Page, m_PageSize, m_ClubDbNum, m_ChaDbNum);

    NET_CLUB_LOG_AH()
        : m_LogType(0)
		, m_Page(0)
		, m_PageSize(0)
		, m_ClubDbNum(0)
        , m_ChaDbNum(0)
    {
    }    
};

//! DB Thread->Cache
struct NET_CLUB_LOG_DH
{
    DWORD m_ClientSlot;
    DWORD m_ChaDbNum;
    club::LOG_DATA m_Log;

    MSGPACK_DEFINE(m_ClientSlot, m_ChaDbNum, m_Log);

    NET_CLUB_LOG_DH(DWORD ClientSlot, DWORD ChaDbNum, const club::LOG_DATA& Log)    
        : m_ClientSlot(ClientSlot)
        , m_ChaDbNum(ChaDbNum)
        , m_Log(Log)
    {
    }

    NET_CLUB_LOG_DH()
        : m_ClientSlot(0)
        , m_ChaDbNum(0)
    {
    }
};

//! Cache->Agent
struct NET_CLUB_LOG_HA
{
    DWORD m_ChaDbNum;
    club::LOG_DATA m_Log;

    MSGPACK_DEFINE(m_ChaDbNum, m_Log);

    NET_CLUB_LOG_HA()
        : m_ChaDbNum(0)
    {
    }
};

//! DB Thread->Cache
struct NET_CLUB_LOG_DH_END
{
    DWORD m_ClientSlot;
    DWORD m_ChaDbNum;
    WORD  m_ResultSize;

    MSGPACK_DEFINE(m_ClientSlot, m_ChaDbNum, m_ResultSize);

    NET_CLUB_LOG_DH_END()
        : m_ClientSlot(0)
        , m_ChaDbNum(0)
        , m_ResultSize(0)
    {
    }

    NET_CLUB_LOG_DH_END(DWORD ClientSlot, DWORD ChaDbNum, WORD ResultSize)
        : m_ClientSlot(ClientSlot)
        , m_ChaDbNum(ChaDbNum)
        , m_ResultSize(ResultSize)
    {
    }
};

//! Cache->Agent
struct NET_CLUB_LOG_HA_END
{
    DWORD m_ChaDbNum;
    WORD  m_ResultSize;

    MSGPACK_DEFINE(m_ChaDbNum, m_ResultSize);

    NET_CLUB_LOG_HA_END()
        : m_ChaDbNum(0)
        , m_ResultSize(0)
    {
    }
};

//! Agent->Client
struct NET_CLUB_LOG_AC_END
{
    WORD  m_ResultSize;

    MSGPACK_DEFINE(m_ResultSize);

    NET_CLUB_LOG_AC_END()
        : m_ResultSize(0)
    {
    }
};

struct NET_CLUB_LOG
{
    DWORD m_ClubDbNum;
    BYTE m_Type;
    std::string m_LogMsg;

    MSGPACK_DEFINE(m_ClubDbNum, m_Type, m_LogMsg);

    NET_CLUB_LOG()
        : m_ClubDbNum(0)
        , m_Type(0)
    {
    }
};

struct NET_CLUB_GRADE_NAME_CHANGE_CA
{
    BYTE m_Index;
    std::string m_GradeName;

    MSGPACK_DEFINE(m_Index, m_GradeName);

    NET_CLUB_GRADE_NAME_CHANGE_CA()
        : m_Index(0)
    {
    }
};

struct NET_CLUB_GRADE_NAME_CHANGE_RESULT_AC
{
    bool m_Result;

    MSGPACK_DEFINE(m_Result);

    NET_CLUB_GRADE_NAME_CHANGE_RESULT_AC()
        : m_Result(false)
    {
    }
};

struct NET_CLUB_GRADE_NAME_CHANGE_AC
{
    BYTE m_Index;
    std::string m_GradeName;

    MSGPACK_DEFINE(m_Index, m_GradeName);

    NET_CLUB_GRADE_NAME_CHANGE_AC()
        : m_Index(0)        
    {
    }
};

struct NET_CLUB_GRADE_NAME_CHANGE_AF 
{
	BYTE m_Index;
	DWORD m_ClubDbNum;
    std::string m_GradeName;    

    MSGPACK_DEFINE(m_Index, m_ClubDbNum, m_GradeName);

    NET_CLUB_GRADE_NAME_CHANGE_AF()
        : m_ClubDbNum(0)
		, m_Index(0)
    {
    }
};

struct NET_CLUB_MEMBER_GRADE_CHANGE_CA
{
    DWORD m_MemberDbNum;
    BYTE m_Index;

    MSGPACK_DEFINE(m_MemberDbNum, m_Index);

    NET_CLUB_MEMBER_GRADE_CHANGE_CA()
        : m_MemberDbNum(0)
        , m_Index(0)
    {
    }
};

struct NET_CLUB_MEMBER_GRADE_CHANGE_AF
{
    DWORD m_ClubDbNum;
    DWORD m_MemberDbNum;
    BYTE m_Index;

    MSGPACK_DEFINE(m_ClubDbNum, m_MemberDbNum, m_Index);

    NET_CLUB_MEMBER_GRADE_CHANGE_AF()
        : m_ClubDbNum(0)
        , m_MemberDbNum(0)
        , m_Index(0)
    {
    }
};

struct NET_CLUB_MEMBER_GRADE_CHANGE_AC2
{
    DWORD m_MemberDbNum;
    BYTE  m_GradeIndex;

    MSGPACK_DEFINE(m_MemberDbNum, m_GradeIndex);

    NET_CLUB_MEMBER_GRADE_CHANGE_AC2()
        : m_MemberDbNum(0)
        , m_GradeIndex(0)
    {
    }
};

//! 클럽 가입시간 리셋
struct NET_CLUB_JOIN_TIME_RESET_CA
{
    std::string m_ChaName;

    MSGPACK_DEFINE(m_ChaName);

    NET_CLUB_JOIN_TIME_RESET_CA()
    {
        // NET_MSG_CLUB_JOIN_TIME_RESET_CA
    }

    NET_CLUB_JOIN_TIME_RESET_CA(const std::string& ChaName)
        : m_ChaName(ChaName)
    {
    }
};

//! 클럽 가입시간 리셋 결과
struct NET_CLUB_JOIN_TIME_RESET_AC
{
    enum RESULT 
    {
        NOT_EXIST_CHAR, //! 캐릭터가 온라인 상태가 아님
        NOT_PERMISSION, //! 권한없음
        SUCCESS,        //! 성공
    };

    int m_Result;
    std::string m_ChaName;

    MSGPACK_DEFINE(m_Result, m_ChaName);

    NET_CLUB_JOIN_TIME_RESET_AC()
        : m_Result(NOT_EXIST_CHAR)
    {
    }

    NET_CLUB_JOIN_TIME_RESET_AC(const std::string& ChaName, int Result)
        : m_ChaName(ChaName)
        , m_Result(Result)
    {
        // NET_MSG_CLUB_JOIN_TIME_RESET_AC
    }
};

//! 클럽 가입시간 리셋
struct NET_CLUB_JOIN_TIME_RESET_AF
{
    DWORD m_ChaDbNum;
    __time64_t m_Time;

    MSGPACK_DEFINE(m_ChaDbNum, m_Time);

    NET_CLUB_JOIN_TIME_RESET_AF()
        : m_Time(10800) // 1970-1-1-12:00
    {
        // NET_MSG_CLUB_JOIN_TIME_RESET_AF
    }

    NET_CLUB_JOIN_TIME_RESET_AF(DWORD ChaDbNum, const __time64_t& Time)
        : m_ChaDbNum(ChaDbNum)
        , m_Time(Time)
    {
    }
};

//! 클럽 동맹시간 리셋
struct NET_CLUB_ALLIANCE_TIME_RESET_CA
{
    std::string m_ClubName;

    MSGPACK_DEFINE(m_ClubName);

    NET_CLUB_ALLIANCE_TIME_RESET_CA()
    {
        // NET_MSG_CLUB_ALLIANCE_TIME_RESET_CA
    }

    NET_CLUB_ALLIANCE_TIME_RESET_CA(const std::string& ClubName)
        : m_ClubName(ClubName)
    {
    }
};

//! 클럽 동맹시간 리셋
struct NET_CLUB_ALLIANCE_TIME_RESET_AC
{
    enum RESULT 
    {
        NOT_EXIST_CLUB,
        HAVE_NOT_PERMISSION,
        SUCCESS,
    };

    int m_Result;
    std::string m_ClubName;

    MSGPACK_DEFINE(m_Result, m_ClubName);

    NET_CLUB_ALLIANCE_TIME_RESET_AC()
    {
        // NET_MSG_CLUB_ALLIANCE_TIME_RESET_AC
    }

    NET_CLUB_ALLIANCE_TIME_RESET_AC(const std::string& ClubName, RESULT Result)
        : m_ClubName(ClubName)
        , m_Result(Result)
    {
    }
};

//! 클럽 동맹시간 리셋
struct NET_CLUB_ALLIANCE_TIME_RESET_AF
{
    DWORD m_ClubDbNum;
    __time64_t m_Time;

    MSGPACK_DEFINE(m_ClubDbNum, m_Time);

    NET_CLUB_ALLIANCE_TIME_RESET_AF()
    {
        // NET_MSG_CLUB_ALLIANCE_TIME_RESET_AF
    }

    NET_CLUB_ALLIANCE_TIME_RESET_AF(DWORD ClubDbNum, const __time64_t& Time)
        : m_ClubDbNum(ClubDbNum)
        , m_Time(Time)
    {
    }
};

#endif