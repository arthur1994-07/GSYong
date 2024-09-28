#include "stdafx.h"
#include <iostream>
#include "../String/StringUtil.h"
#include "./DateTime2.h"

#include "../DebugInclude.h"

namespace sc
{
    namespace time
    {
        /*
        // ----------------------------------------------------------------------------   

        // The upper date limit is 12/31/3000. The lower limit is 1/1/1970 12:00:00 AM GMT.
        // 23:59:59, December 31, 3000
        //CTime LowTime(1970, 1, 1, 12, 0, 0);
        //CTime HighTime(3000, 12, 31, 23, 59, 59);
        const __time64_t tLowTime = 10800; //LowTime.GetTime();
        const __time64_t tHighTime = 32535183599; //HighTime.GetTime();

        bool ValidTime(const __time64_t& Time)
        {    
            if (tLowTime <= Time && Time <= tHighTime)
                return true;
            else
                return false;
        }

        void AdjustTime(__time64_t& Time)
        {
            if (tLowTime > Time || Time > tHighTime)
                Time = tLowTime;
        }

        __time64_t GetCurrentTime()
        {
            CTime t(CTime::GetCurrentTime());
            return t.GetTime();
        }

        __time64_t GetTime(const TIMESTAMP_STRUCT& Date)
        {
            CTime Temp(Date.year, Date.month, Date.day, Date.hour, Date.minute, Date.second);
            return Temp.GetTime();
        }

        __time64_t GetTime(const _variant_t& Date)
        {
            SYSTEMTIME SysDate;
            VariantTimeToSystemTime(Date.date, &SysDate);
            CTime ctDate(SysDate);
            return ctDate.GetTime();
        }

        //! time 값을 string 으로 만든다.        
        std::string DateTimeFormat(const __time64_t Time, bool bTimeInclude, bool bSecInclude)
        {
            return sc::string::DateTimeFormat(Time, bTimeInclude, bSecInclude);
        }
        */

        // ----------------------------------------------------------------------------
    } // namespace time
} // namespace sc