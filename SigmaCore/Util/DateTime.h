#ifndef _SC_DATE_TIME_H_
#define _SC_DATE_TIME_H_

#pragma once

#include <sqltypes.h>
#include <comutil.h>

namespace sc
{
    namespace time
    {
        // The upper date limit is 12/31/3000. The lower limit is 1/1/1970 12:00:00 AM GMT.
        // 23:59:59, December 31, 3000
        //CTime LowTime(1970, 1, 1, 12, 0, 0);
        //CTime HighTime(3000, 12, 31, 23, 59, 59);
        extern const __time64_t tLowTime;
        extern const __time64_t tHighTime;

        bool ValidTime(const __time64_t& Time);
        
        void AdjustTime(__time64_t& Time);
        void AdjustTime(TIMESTAMP_STRUCT& Time);

        __time64_t GetCurrentTime();
        std::string GetCurrentTimeStr();
        __time64_t GetTime(const TIMESTAMP_STRUCT& Date);
        __time64_t GetTime(const _variant_t& Date);
		DBTIMESTAMP GetTimeStamp(const __time64_t& Time);
        
        //! time 값을 string 으로 만든다.        
        std::string DateTimeFormat(const __time64_t& Time, bool bTimeInclude=false, bool bSecInclude=false);        
    }    
} // namespace sc

#endif // _SC_DATE_TIME_H_