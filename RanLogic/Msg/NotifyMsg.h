#pragma once

#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/String/MinGuid.h"
#include "../Notify/NotifyDefine.h"
#include "../s_NetGlobal.h"

//! 2012-07-05 jgkim
namespace GLMSG
{
    //! Notify request Client->Agent
    struct NOTIFY_REQ_CA : public NET_MSG_GENERIC
    {
        NOTIFY_REQ_CA()
        {
            MIN_STATIC_ASSERT(sizeof(NOTIFY_REQ_CA)<=NET_DATA_BUFSIZE);

            dwSize = sizeof(NOTIFY_REQ_CA);
            nType  = NET_MSG_NOTIFY_REQ_CA;
        }
    };

//#pragma pack(8)
    //! Notify data/Agent->Client
    struct NOTIFY_DATA_AC : public NET_MSG_GENERIC
    {
        enum { 
            SIZE1 = sizeof(sc::MGUID),
            SIZE2 = sizeof(NET_MSG_GENERIC),
            SIZE3 = sizeof(int),
            SIZE4 = sizeof(__time64_t),
            SIZE5 = sizeof(int),
            BASE_SIZE = (SIZE1 + SIZE2 + SIZE3 + SIZE4 + SIZE5),
            STR_BUF_SIZE = NET_DATA_BUFSIZE - BASE_SIZE,
        };
        
        __time64_t m_Date; //! 등록 시간
        sc::MGUID  m_Guid; //! 알림 고유번호
        int m_Type; //! 알림 타입
        int m_Reserved1;
        char m_JsonData[STR_BUF_SIZE]; //! Json data

        NOTIFY_DATA_AC(
            int Type,
            const sc::MGUID& Guid,
            const __time64_t& Date)
            : m_Type(Type)
            , m_Guid(Guid)
            , m_Date(Date)
            , m_Reserved1(0)
        {
            MIN_STATIC_ASSERT(sizeof(NOTIFY_DATA_AC)<=NET_DATA_BUFSIZE);

            dwSize = BASE_SIZE;
            nType  = NET_MSG_NOTIFY_DATA_AC;
        }

        void SetJsonData(const std::string& JsonData)
        {
            StringCchCopy(m_JsonData, STR_BUF_SIZE, JsonData.c_str());
            dwSize = static_cast<DWORD> (BASE_SIZE + strnlen(m_JsonData, STR_BUF_SIZE-1) + 1);
        }
    };
//#pragma pack()

    //! Notify data end/Agent->Client
    struct NOTIFY_DATA_AC_END : public NET_MSG_GENERIC
    {        
        NOTIFY_DATA_AC_END()
        {
            MIN_STATIC_ASSERT(sizeof(NOTIFY_DATA_AC_END)<=NET_DATA_BUFSIZE);

            dwSize = sizeof(NOTIFY_DATA_AC_END);
            nType  = NET_MSG_NOTIFY_DATA_AC_END;
        }
    };

} // namespace GLMSG