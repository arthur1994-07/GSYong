#include "pch.h"
#include "PVEDB.h"

SPVEDBFile s_dbtime;

/**
    DB ���ϸ��� ó�� �Ѵ�.
    ���ó� �Ѱ����� ���ϸ��� ó�������ν� �����ϰ� ��ó�� �� �ִ�.

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
			�츮�� Ư�� ��(Hour)�� �������� �ʱ�ȭ ��Ų��.

		 */
		int nRefeshHour = 5;

		/**
			���� Ư����(Hour) ���� �̶�� ����(Day)�� DB�� ��� �Ѵ�.

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
    DB ������ �����ϴ��� üũ �Ѵ�.
    �츮�� �̸� ���� �ʱ�ȭ�� üũ �� �� �ִ�.

 */
bool IsPVEDBFile()
{
    const CTime cCurTime = CTime::GetCurrentTime();
    const int nYear = cCurTime.GetYear();
    const int nMonth = cCurTime.GetMonth();
    const int nDay = cCurTime.GetDay();
	const int nHour = cCurTime.GetHour();

	/**
		������ ���� ���� �Ǹ� ���ο� DB �� �����.

	 */
	if (s_dbtime.strFile.empty())
	{
		return false;
	}

	/**
		��� ���� �ٸ��ٸ� ������ ���ο� DB �� �����.

	 */
    if (nYear != s_dbtime.nYear || nMonth != s_dbtime.nMonth)
    {
        return false;
    }

	/**
		���� �ٸ��ٸ� ���� �ð��� �ʰ��� ��� ���ο� DB �� �����.

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
