#include "stdafx.h"
#include "./LogPacket.h"

/*
#include "./PacketAnalysisSQLite.h"
#include "./LogPacket.h"

#include "../DebugInclude.h"

using namespace CORE_COMMON;

LogPacket& LogPacket::getInstance()
{
	static LogPacket Instance;
	return Instance;
}

LogPacket::LogPacket()
	: m_pImpl(NULL)
{
	
}

LogPacket::~LogPacket()
{
	if ( NULL != m_pImpl )
	{
		delete m_pImpl;
		m_pImpl = NULL;
	}
}

void LogPacket::add( WORD Type, WORD Size )
{
	m_pImpl->add( Type, Size );
}

void LogPacket::start()
{
	if ( NULL == m_pImpl )
	{
		m_pImpl = new CORE_COMMON::PacketAnalysisSQLite;
		m_pImpl->start();
	}
}

void LogPacket::end()
{
	if ( NULL != m_pImpl )
		m_pImpl->end();
}
*/


using namespace sc;

LogPacket::LogPacket()
{

}

LogPacket::~LogPacket()
{

}

void LogPacket::BeginRead()
{
    GetDate(m_strReadHeader);
    m_strReadHeader += _T(" RecvFromSvr : %-15s");
}

void LogPacket::EndRead(int nType, int nPacketTotalCount, CTimeSpan spanTime)
{
    const TCHAR* pName = GetServerPacketType(nType);
	TCHAR * pszPacketMsgLog = new TCHAR[2048];

    if (pName == 0)
    {
		if( nPacketTotalCount != 0 )
		{	
			wsprintf( pszPacketMsgLog, "%d - Cnt [%d], LastTime [%d]", nType, nPacketTotalCount, spanTime );
			PrintfWithString(&m_strReadHeader, pszPacketMsgLog);
		}
		else
		{
			PrintfWithString(&m_strReadHeader, _T("%d"));
			PrintfWithString(&m_strReadHeader, nType);
		}
    }
    else
    {
		if( nPacketTotalCount != 0 )
		{	
			wsprintf( pszPacketMsgLog, "%s - Cnt [%d], LastTime [%d]", pName, nPacketTotalCount, spanTime );
			PrintfWithString(&m_strReadHeader, pszPacketMsgLog);
		}
		else
		{
			PrintfWithString(&m_strReadHeader, pName);
		}
    }

    Write(m_strReadHeader.c_str());
    Write(m_strReadContents.str().c_str());
    Write(_T("\n"));

    m_strReadContents.clear();
    m_strReadContents.str( _T("") );
    m_strReadHeader.clear();
}

void LogPacket::BeginWrite()
{
    GetDate(m_strSendHeader);
    m_strSendHeader += _T(" SendToSvr : %-15s");
}

void LogPacket::EndWrite(int nType, int nPacketTotalCount, CTimeSpan spanTime)
{
    const TCHAR* pName = GetServerPacketType(nType);
	TCHAR * pszPacketMsgLog = new TCHAR[2048];

	if (pName == 0)
    {
		if( nPacketTotalCount != 0 )
		{	
			wsprintf( pszPacketMsgLog, "%d - Cnt [%d], LastTime [%d]", nType, nPacketTotalCount, spanTime );
			PrintfWithString(&m_strSendHeader, pszPacketMsgLog);
		}
		else
		{
			PrintfWithString(&m_strSendHeader, _T("%d"));
			PrintfWithString(&m_strSendHeader, nType);
		}
    }
    else
    {
		if( nPacketTotalCount != 0 )
		{	
			wsprintf( pszPacketMsgLog, "%s - Cnt [%d], LastTime [%d]", pName, nPacketTotalCount, spanTime );
			PrintfWithString(&m_strSendHeader, pszPacketMsgLog);
		}
		else
		{
			PrintfWithString(&m_strSendHeader, pName);
		}
    }

    Write(m_strSendHeader.c_str());
    Write(m_strSendContents.str().c_str());
    Write(_T("\n"));

    m_strSendContents.clear();
    m_strSendContents.str(_T(""));
    m_strSendHeader.clear();
}

void LogPacket::GetDate(tstring& out, TCHAR* pType)
{
    // 전체 사이즈 상수
    const static int size = 7;

    // 임시 변수
    tstring temp = pType; 

    // 테이블 설정
    const static TCHAR* str_table[size] =
    { _T("YY"), _T("MM"), _T("DD"), _T("HH"), _T("mm"), _T("SS"), _T("ss") };
    const static TCHAR* str_type_table[size] = 
    { _T("%d"), _T("%02d"), _T("%02d"), _T("%02d"), _T("%02d"), _T("%02d"), _T("%03d") };
    int time_table[size] = { 0 };

    // 현재 시간 설정
    time_t timer;
    struct tm tblock;
    timer = time(NULL);
    localtime_s(&tblock, &timer);

    time_table[0] = tblock.tm_year + 1900;
    time_table[1] = tblock.tm_mon + 1;
    time_table[2] = tblock.tm_mday;
    time_table[3] = tblock.tm_hour;
    time_table[4] = tblock.tm_min;
    time_table[5] = tblock.tm_sec;
    time_table[6] = timer % 1000;

    // 문자열 변경
    for (int i = 0; i < size; ++i)
    {
        tstring::size_type it = temp.find(str_table[i]);

        if (it != tstring::npos)
        {
            _sntprintf_s(m_buff, sizeof(m_buff), str_type_table[i], time_table[i]);
            temp.replace(it, 2, m_buff);
        }
    }

    // 출력 한다.
    out += temp;
}

void LogPacket::PrintfWithString(tstring* pStr, ...)
{
    va_list vaList;
    va_start(vaList, pStr);
    _vsntprintf_s(m_buff, sizeof(m_buff), pStr->c_str(), vaList);
    va_end(vaList);
    *pStr = m_buff;
}
