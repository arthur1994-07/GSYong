#include "pch.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "./GLContrlClubMsg.h"

#include "../../SigmaCore/DebugInclude.h"

namespace GLMSG
{   
    SCLUBMEMBER::SCLUBMEMBER()
        : m_ChaDbNum(0)
        , m_Flag(0)
        , m_ChaLevel(0)
        , m_Online(false)
    {
        memset(m_Name, 0, CHAR_SZNAME);
        m_LastPlayTime = sc::time::GetCurrentTime();
    }

    SCLUBMEMBER::SCLUBMEMBER(
        DWORD ChaDbNum,
        DWORD Flags,
        WORD ChaLevel,
        const char* ChaName,
        const __time64_t& LastPlayTime,
        bool Online)
        : m_ChaDbNum(ChaDbNum)
        , m_Flag(Flags)
        , m_ChaLevel(ChaLevel)
        , m_LastPlayTime(LastPlayTime)
        , m_Online(Online)        
    {
        SetName(ChaName);
    }

} // namespace GLMSG