#include "pch.h"
#include "PVEDB.h"

SPVEDBFile s_dbtime;

/**
    DB 파일명을 처리 한다.
    역시나 한곳에서 파일명을 처리함으로써 유연하게 대처할 수 있다.

 */
const TCHAR* GetPVEDBFile()
{
    if (s_dbtime.strFile.empty())
    {
        CTime cCurTime = CTime::GetCurrentTime();
        int nYear = cCurTime.GetYear();
        int nMonth = cCurTime.GetMonth();
        int nDay = cCurTime.GetDay();
		int nHour = cCurTime.GetHour();

		/**
			우리는 특정 시(Hour)를 기준으로 초기화 시킨다.

		 */
		int nRefeshHour = 5;

		/**
			만약 특정시(Hour) 이전 이라면 전날(Day)의 DB를 사용 한다.

		 */
		if (nHour < nRefeshHour)
		{
			cCurTime = cCurTime - CTimeSpan(1, 0, 0, 0);
			nDay = cCurTime.GetDay();
		}

        TCHAR temp[256] = {0};        
        _sntprintf_s(temp, 256, _T("PVEDB\\%d_%02d_%02d.db"), nYear, nMonth, nDay);

        s_dbtime.nYear = nYear;
        s_dbtime.nMonth = nMonth;
        s_dbtime.nDay = nDay;
		s_dbtime.nHour = nRefeshHour;
        s_dbtime.strFile = temp;
    }

    return s_dbtime.strFile.c_str();
}

/**
    DB 파일이 존재하는지 체크 한다.
    우리는 이를 통해 초기화를 체크 할 수 있다.

 */
bool IsPVEDBFile()
{
    const CTime cCurTime = CTime::GetCurrentTime();
    const int nYear = cCurTime.GetYear();
    const int nMonth = cCurTime.GetMonth();
    const int nDay = cCurTime.GetDay();
	const int nHour = cCurTime.GetHour();

	/**
		서버가 새로 가동 되면 새로운 DB 를 만든다.

	 */
	if (s_dbtime.strFile.empty())
	{
		return false;
	}

	/**
		년과 월이 다르다면 무조건 새로운 DB 를 만든다.

	 */
    if (nYear != s_dbtime.nYear || nMonth != s_dbtime.nMonth)
    {
        return false;
    }

	/**
		일이 다르다면 기준 시간을 초과할 경우 새로운 DB 를 만든다.

	 */
	if (nDay != s_dbtime.nDay && nHour >= s_dbtime.nHour)
	{
		return false;
	}

    return true;
}

void ResetPVEDBFile()
{
	s_dbtime.strFile.clear();
}
