#pragma once

#include <sqltypes.h>

namespace sc
{
    namespace time
    {
        /*
        // The upper date limit is 12/31/3000. The lower limit is 1/1/1970 12:00:00 AM GMT.
        // 23:59:59, December 31, 3000
        //CTime LowTime(1970, 1, 1, 12, 0, 0);
        //CTime HighTime(3000, 12, 31, 23, 59, 59);
        extern const __time64_t tLowTime;
        extern const __time64_t tHighTime;

        bool ValidTime(const __time64_t& Time);
        void AdjustTime(__time64_t& Time);
        __time64_t GetCurrentTime();
        __time64_t GetTime(const TIMESTAMP_STRUCT& Date);
        __time64_t GetTime(const _variant_t& Date);

        //! time ���� string ���� �����.        
        std::string DateTimeFormat(const __time64_t Time, bool bTimeInclude=false, bool bSecInclude=false);
        */
    }    
} // namespace sc
