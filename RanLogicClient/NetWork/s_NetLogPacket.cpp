#include "pch.h"
#include "./s_NetLogPacket.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../../SigmaCore/log/LogConsole.h"

#include <direct.h>

using namespace sc;

NetLogPacket::NetLogPacket() :
    m_pConsole(0)
{
#ifdef _PACKET_LOG
	memset(m_pLogC2SPacket, 0, sizeof(m_pLogC2SPacket));
	memset(m_pLogS2CPacket, 0, sizeof(m_pLogS2CPacket));
#endif
}

bool NetLogPacket::Init()
{
	if (m_pConsole == 0)
	{
		m_pConsole = new LogConsole;

		if (m_pConsole != 0)
		{
			m_pConsole->Init(_T("Ran Console"));
			CreateFilter();
			LoadProtocol();
		}
	}

    return m_pConsole != 0;
}

void NetLogPacket::Destroy()
{
    if (m_pConsole != 0)
    {
        m_pConsole->Destroy();
    }

	if (m_file.is_open())
	{
		m_file.close();
	}

    SAFE_DELETE(m_pConsole);
}

void NetLogPacket::BeginRead()
{
    m_lock[0].Lock();
	LogPacket::BeginRead();
}

void NetLogPacket::EndRead(int nType)
{
#ifdef _PACKET_LOG
#ifdef _USE_SIMPLE_PACKET_LOG
	if (m_setFilter.find(nType) == m_setFilter.end())
	{
		LogPacket::EndRead(nType);
	}
	else
	{
		m_strReadContents.clear();
		m_strReadContents.str( _T("") );
		m_strReadHeader.clear();   
	}
#else
	m_pLogS2CPacket[nType].nPacketTotalCount++;
	CTime preTime = m_pLogS2CPacket[nType].lastPacketTime;
	m_pLogS2CPacket[nType].lastPacketTime = CTime::GetCurrentTime();
	CTimeSpan spanTime = m_pLogS2CPacket[nType].lastPacketTime - preTime;

	LogPacket::EndRead(nType, m_pLogS2CPacket[nType].nPacketTotalCount, m_pLogS2CPacket[nType].nPacketTotalCount == 1 ? 0 : spanTime);
#endif
#endif
    m_lock[0].Unlock();
}

void NetLogPacket::BeginWrite()
{
    m_lock[1].Lock();
	LogPacket::BeginWrite();
}

void NetLogPacket::EndWrite(int nType)
{
#ifdef _PACKET_LOG
#ifdef _USE_SIMPLE_PACKET_LOG
	if (m_setFilter.find(nType) == m_setFilter.end())
	{
		LogPacket::EndWrite(nType);
	}
	else
	{
		m_strSendContents.clear();
		m_strSendContents.str(_T(""));
		m_strSendHeader.clear();
	}
#else
	m_pLogC2SPacket[nType].nPacketTotalCount++;
	CTime preTime = m_pLogC2SPacket[nType].lastPacketTime;
	m_pLogC2SPacket[nType].lastPacketTime = CTime::GetCurrentTime();
	CTimeSpan spanTime = m_pLogC2SPacket[nType].lastPacketTime - preTime;

	LogPacket::EndWrite(nType, m_pLogC2SPacket[nType].nPacketTotalCount, m_pLogC2SPacket[nType].nPacketTotalCount == 1 ? 0 : spanTime);
#endif
#endif
    m_lock[1].Unlock();
}

void NetLogPacket::Write(const TCHAR* pData)
{
	m_lock[2].Lock();

    if (m_pConsole != 0)
    {
        m_pConsole->write(pData, LOG_INFO);
    }	

	if (m_file.is_open() == false)
	{
		tstring name;
		_tmkdir( _T("Log") );
		GetDate( name, _T("log\\PacketLog_YY-MM-DD HH.txt") );
		m_file.open(name.c_str(), std::ios_base::out | std::ios_base::app);

		if (m_file.is_open())
		{
			m_file << pData;			
			m_file.close();
		}
	}

	m_lock[2].Unlock();
}

const TCHAR* NetLogPacket::GetServerPacketType(int nType)
{
	symbol_map_iterator it = m_mapSymbol.find(nType);
	
	if (it != m_mapSymbol.end())
	{
		return it->second.c_str();
	}

    return 0;
}

const TCHAR* NetLogPacket::GetClientPacketType(int nType)
{
    return GetServerPacketType(nType);
}

void NetLogPacket::CreateFilter()
{
    m_setFilter.insert(NET_MSG_HEARTBEAT_CLIENT_REQ);
    m_setFilter.insert(NET_MSG_HEARTBEAT_CLIENT_ANS);
    m_setFilter.insert(NET_MSG_ATTENDANCE_TASK_UPDATE_FC);
    m_setFilter.insert(NET_MSG_GCTRL_PERIOD);
    m_setFilter.insert(NET_MSG_GCTRL_GOTO);
    m_setFilter.insert(NET_MSG_GCTRL_DROP_OUT);
    m_setFilter.insert(NET_MSG_GCTRL_DROP_CROW);
    m_setFilter.insert(NET_MSG_CROW_MOVETO);
    m_setFilter.insert(NET_MSG_SERVERTIME_BRD);
    m_setFilter.insert(NET_MSG_GCTRL_ACTION_BRD);
    m_setFilter.insert(NET_MSG_GCTRL_UPDATE_STATE);	
	m_setFilter.insert(NET_MSG_GCTRL_UPDATE_HIRE_SUMMON_STATE);
	m_setFilter.insert(NET_MSG_UPDATE_CHARSLOT_FC);
    m_setFilter.insert(NET_MSG_GCTRL_UPDATE_STATE_BRD);
    m_setFilter.insert(NET_MSG_CHAR_POSITION_AC);
    m_setFilter.insert(NET_MSG_GCTRL_PARTY_MBR_UPDATE);
	m_setFilter.insert(NET_MSG_GCTRL_APPLY_SKILLEFFECT_FC);
}

const TCHAR* GetToken(LogPacket::tfstream& fs)
{
	static TCHAR		line[256];
	static TCHAR*		pTemp = 0;
	static const TCHAR* pToken = 0;
	const TCHAR*		Delimit = _T(" ,;\t{=()");

	if (pToken != 0)
	{
		pToken = _tcstok_s(NULL, Delimit, &pTemp);

		if (pToken != 0)
		{
			// 한줄 주석 제거
			if (_tcsstr(pToken, _T("//")) != pToken)
			{
				return pToken;
			}
		}

        return (pToken = 0);
	}

	while (fs.eof() == false)
	{
		fs.getline(line, 256);

		if (_tcslen(line) <= 0)
		{
            continue;
        }

		pToken = _tcstok_s(line, Delimit, &pTemp);

		if (pToken == 0)
		{
			continue;
		}

		// 한줄 주석 제거
		if (_tcsstr(pToken, _T("//")) == pToken)
		{
			continue;
		}

		// 여러줄 주석 제거
		if (_tcsstr(pToken, _T("/*")) == pToken)
		{
			do
			{
				if (_tcsstr(pToken, _T("*/")) == pToken)
				{
					return GetToken(fs);
				}
			}
			while ((pToken = GetToken(fs)) != 0);
		}

        // # 전처리 제거
        if (_tcsstr(pToken, _T("#")) == pToken)
        {
            return GetToken(fs);
        }

		return pToken;
	}

	return 0;
}

void NetLogPacket::RecursiveSymbol(string_map& kMap, tfstream& fs, int& nSum, int& nOperator)
{    
    const int OPERATOR_PLUS = 1;
    const TCHAR* pToken = GetToken(fs);

    if (pToken == 0)
    {
        return;
    }

    int nNum = nSum;

    // 연산자
    if (_tcsicmp(_T("+"), pToken) == 0)
    {
        nOperator = OPERATOR_PLUS;
    }

    // 심벌 처리
    if (_istalpha(pToken[0]))
    {
        string_map::iterator it = kMap.find(pToken);

        if (kMap.end() != it)
        {
            nNum = (*it).second;
        }
    }

    // 숫자
    if (_istdigit(pToken[0]))
    {
        nNum = _tstoi(pToken); 
    }

    // 연산자 처리
    if (nOperator == OPERATOR_PLUS)
    {
        nSum += nNum;
    }
    else
    {
        nSum = nNum;
    }

    RecursiveSymbol(kMap, fs, nSum, nOperator);
}

void NetLogPacket::LoadProtocol()
{
	tfstream fs;
	fs.open(_T("..\\RanLogic\\s_NetGlobal.h"), tfstream::ios_base::in);

	if (fs.is_open() == false)
	{
		return;
	}

    int	nPivot = 0;
    int	nOperator = 0;
    const TCHAR* pToken = 0;
    string_map kMap;

    bool bStart = false;

    while ( (pToken = GetToken(fs)) || fs.eof() == false )
    {
        if (pToken == 0)
        {
            continue;
        }

        if (bStart == false)
        {
            // 시작 심벌 검색
            if (_tcsicmp(_T("NET_MSG_BASE_ZERO"), pToken) == 0)
            {
                bStart = true;
            }
        }

        if (bStart)
        {
            int nSum = -1;

            // 심벌 매칭 및 연산 처리
            RecursiveSymbol(kMap, fs, nSum, nOperator);

            if (nSum != -1)
            {
                nPivot = nSum;
            }
            else
            {
                ++nPivot;
            }

            // 심벌 저장
            m_mapSymbol.erase(nPivot);
            m_mapSymbol.insert(std::make_pair(nPivot, pToken));
            kMap.insert(std::make_pair(pToken, nPivot));

            // 종료 심벌 검색
            if (_tcsicmp(_T("NET_MSG_GCTRL_MAX"), pToken) == 0)
            {
                break;
            }
        }
    }

	fs.close();
}
