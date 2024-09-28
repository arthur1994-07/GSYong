#include "stdafx.h"
#include <iostream>
#include <oledb.h>
#include "../String/StringUtil.h"
#include "./DateTime.h"

#include "../DebugInclude.h"


namespace sc
{
    namespace time
    {
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

void AdjustTime(TIMESTAMP_STRUCT& Time)
{
    if (Time.year < 1970 || Time.year >= 2999)
    {
        Time.year = 1970;
        Time.month = 2;
        Time.day = 1;
        Time.hour = 1;
        Time.minute = 1;
        Time.second = 1;
    }
}

__time64_t GetCurrentTime()
{
    CTime t(CTime::GetCurrentTime());
    return t.GetTime();
}

std::string GetCurrentTimeStr()
{
    __time64_t CurrentTime = GetCurrentTime();
    return DateTimeFormat(CurrentTime, true, true);
}

__time64_t GetTime(const TIMESTAMP_STRUCT& Date)
{
    try
    {
        CTime Temp(Date.year, Date.month, Date.day, Date.hour, Date.minute, Date.second);
        return Temp.GetTime();
    }
    catch (CException*e)
    {        
        e->Delete();
        return tLowTime;
    }
    catch (...)
    {
        return tLowTime;
    }
}

__time64_t GetTime(const _variant_t& Date)
{
    try
    {    
        SYSTEMTIME SysDate;
        if (VariantTimeToSystemTime(Date.date, &SysDate))
        {
            CTime ctDate(SysDate);
            return ctDate.GetTime();
        }
        else
        {
            return tLowTime;
        }
    }
    catch (CException*e)
    {        
        e->Delete();
        return tLowTime;
    }
    catch (...)
    {
        return tLowTime;
    }    
}

DBTIMESTAMP GetTimeStamp(const __time64_t& Time)
{
    DBTIMESTAMP Temp;
    Temp.year     = 1970;
    Temp.month    = 1;
    Temp.day      = 1;
    Temp.hour     = 1;
    Temp.minute   = 0;
    Temp.second   = 0;
    Temp.fraction = 0;

    try
    {
	    CTime TempTime(Time);
	    Temp.year     = TempTime.GetYear();
	    Temp.month    = TempTime.GetMonth();
	    Temp.day      = TempTime.GetDay();
	    Temp.hour     = TempTime.GetHour();
	    Temp.minute   = TempTime.GetMinute();
	    Temp.second   = TempTime.GetSecond();
	    Temp.fraction = 0;
	    return Temp;
    }
    catch (CException*e)
    {        
        e->Delete();
        return Temp;
    }
    catch (...)
    {
        return Temp;
    }
}

//! time 값을 string 으로 만든다.        
std::string DateTimeFormat(const __time64_t& Time, bool bTimeInclude, bool bSecInclude)
{
    return sc::string::DateTimeFormat(Time, bTimeInclude, bSecInclude);
}

// ----------------------------------------------------------------------------
    } // namespace time
} // namespace sc