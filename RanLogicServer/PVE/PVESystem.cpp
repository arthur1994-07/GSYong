#include "pch.h"
#include "PVESystem.h"
#include "PVESystemQuery.h"
#include "PVEIndun.h"
#include <direct.h>

#include "../TriggerSystem/ServerTriggerSystem.h"
#include "../Party/GLPartyAgent.h"

#include "../../sigmacore/util/MinMessageBox.h"
#include "../../sigmacore/string/StringFormat.h"
#include "../../sigmacore/database/sqlite/sqlite.h"

#include "Internal.hpp"

using namespace PVESystem;
using namespace TriggerSystem;

bool PVESystem::InitPVEDB()
{
    ResetDB();
	return true;
}

void PVESystem::DestroyPVEDB()
{
    if (s_db.IsOpen())
    {
        s_db.Close();
    }
}

void PVESystem::InitPVECache()
{    
    TriggerSystem::SendMessage(&s_cache);
}

void PVESystem::DestroyPVECache()
{
    s_cache.mapinfo.clear();
}

void PVESystem::DestroyPVEIndun()
{
	s_indun.clear();
}

bool PVESystem::GetMapInfo(TriggerSystem::SServerTriggerMapInfo& info)
{
    TriggerMapInfoContainerIterator it = s_cache.mapinfo.find(info.mapid);

    if (it != s_cache.mapinfo.end())
    {
        info = it->second;
        return true;
    }

    return false;
}

bool PVESystem::GetMapTable(MAP_TABLE& tbl)
{
	if (s_db.IsOpen() == false)
	{
		return false;
	}

	TSTRING strID = sc::string::format(_T("%d_%d_%d"), tbl.dwCharDbNum, tbl.dwMapID, tbl.nLevel);
	TSTRING strQuery = sc::string::format(strSelectMainQuery, strID);

	SQLite::TablePtr pTable = s_db.QuerySQL2(strQuery);

	if (pTable.m_pTable == 0 ||			// 예외 상태
		pTable().GetRowCount() == 0)	// 레코드 미 삽입 상태
	{
		tbl.strID = strID;

		// 초기값 설정
		tbl.nInCount = 0;
		tbl.nGenCount = 0;
		tbl.tGenTime = 0;

		strQuery = sc::string::format(strInsertQuery, tbl.strID, tbl.dwCharDbNum, 
			tbl.dwMapID, tbl.nInCount, tbl.nGenCount, tbl.nLevel, tbl.tGenTime);

		// 레코드가 없을 경우 새로 생성 한다.
		if (s_db.ExecuteSQL(strQuery) != SQLITE_OK)
		{
			OutputDataBaseError(_T("Insert Record Error"));
			return false;
		}
	}
	else
	{
		// 레코드는 유일성을 보장 해야 한다.
		if (pTable().GetRowCount() == 1)
		{
			tbl.strID = pTable().GetValue<TSTRING>(0);
			tbl.dwCharDbNum = pTable().GetValue<DWORD>(1);
			tbl.dwMapID = pTable().GetValue<DWORD>(2);
			tbl.nInCount = pTable().GetValue<int>(3);
			tbl.nGenCount = pTable().GetValue<int>(4);
            tbl.nLevel = pTable().GetValue<int>(5);
			tbl.tGenTime = pTable().GetValue<_TIME_TYPE>(6);
		}
		else
		{
			OutputDataBaseError(_T("Primary Key Error"));
			return false;
		}
	}

	return true;
}

bool PVESystem::GetMapTable( MAP_TABLE& tbl OUT, const DWORD& dwChaNum IN, const DWORD& dwMapID IN, const int& nLevel IN )
{
    tbl.dwCharDbNum = dwChaNum;
    tbl.dwMapID =  dwMapID;
    tbl.nLevel = nLevel;

    return GetMapTable(tbl);
}

const MAP_TABLE PVESystem::GetMapTable(const SPVEInDunID& id)
{
	MAP_TABLE tbl;
	tbl.dwCharDbNum = id.dwCharDbNum;
	tbl.dwMapID = id.dwMapID;
	tbl.nLevel = id.nLevel;
	GetMapTable(tbl);
	return tbl;
}

bool PVESystem::SetMapTable(const MAP_TABLE& tbl)
{
	TSTRING strQuery = sc::string::format(strUpdateQuery, tbl.strID, tbl.dwCharDbNum, 
		tbl.dwMapID, tbl.nInCount, tbl.nGenCount, tbl.nLevel, tbl.tGenTime);

	if (s_db.ExecuteSQL(strQuery) != SQLITE_OK)
	{
		OutputDataBaseError(_T("Update Record Error"));
		return false;
	}

	return true;
}

bool PVESystem::CollectMapTable(DWORD dwChaNum, MapTableContainer& con, MAP_TABLE& tbl)
{
    if (s_db.IsOpen() == false)
    {
        return false;
    }

    TSTRING strQuery = sc::string::format(strSelectSubQuery, dwChaNum);

    SQLite::TablePtr pTable = s_db.QuerySQL2(strQuery);

	ZeroMemory(&tbl, sizeof(tbl));

    if (pTable().GoFirst())
    {
		do
		{
			con.push_back(MAP_TABLE());
			con.back().strID = pTable().GetValue<TSTRING>(0);
			con.back().dwCharDbNum = pTable().GetValue<DWORD>(1);
			con.back().dwMapID = pTable().GetValue<DWORD>(2);
			con.back().nInCount = pTable().GetValue<int>(3);
			con.back().nGenCount = pTable().GetValue<int>(4);
			con.back().nLevel = pTable().GetValue<int>(5);
			con.back().tGenTime = pTable().GetValue<_TIME_TYPE>(6);

			TriggerSystem::SServerTriggerMapInfo info;
			info.mapid.dwID = con.back().dwMapID;

			if (PVESystem::GetMapInfo(info))
			{
				// 클라이언트에는 남은 시간을 넘겨줌에 주의 한다.
				con.back().tGenTime = DiffTime(con.back().tGenTime, info.owntime);
			}

			if (con.back().tGenTime == 0)
			{
				continue;
			}

			tbl = con.back();

			/**
				시간을 0으로 설정 함에 주의 해야 한다.
				우리는 귀속 인던 정보는 tbl 에 넣어 줌으로
				이 값은 0 으로 설정 한다.

			 */
			con.back().tGenTime = 0;
		}
		while (pTable().GoNext());
	}

	if (tbl.tGenTime != 0)
	{
		return true;
	}

	/**
		자신이 소속된 인던의 정보를 출력 한다.
		이것은 인던을 소유한 사람 말고 참여한 사람에게 전달하기 위해 필요하다.

	 */
	CPVEInDun* pIndun = FindInstantDungeon(dwChaNum);

	if (pIndun)
	{
		tbl = GetMapTable(pIndun->GetID());

		TriggerSystem::SServerTriggerMapInfo info;
		info.mapid.dwID = tbl.dwMapID;

		if (PVESystem::GetMapInfo(info))
		{
			// 클라이언트에는 남은 시간을 넘겨줌에 주의 한다.
			tbl.tGenTime = DiffTime(tbl.tGenTime, info.owntime);
		}
	}

    return true;
}

void PVESystem::SetCurTime(MAP_TABLE& tbl)
{
    tbl.tGenTime = CTime::GetCurrentTime().GetTime();
}

_TIME_TYPE PVESystem::DiffTime(_TIME_TYPE timestamp, int sec)
{
    CTimeSpan diff = CTime::GetCurrentTime() - CTime(timestamp);
    LONGLONG temp = LONGLONG(sec) - diff.GetTotalSeconds();
    return temp > 0 ? _TIME_TYPE(temp) : 0;
}

CPVEInDun* PVESystem::FindInstantDungeon(const SPVEInDunID& id)
{
    PVEInDunContainerIt it = s_indun.find(id);

    if (it != s_indun.end())
    {
        return &(*it).second;
    }
    else
    {
		TriggerSystem::SServerTriggerMapInfo info;
		info.mapid.dwID = id.dwMapID;

		if (PVESystem::GetMapInfo(info) == false)
		{
			return 0;
		}

		CPVEInDun* pDungeon = &s_indun[id];

		if (pDungeon)
		{
			pDungeon->SetID(id);
			pDungeon->SetInfo(info);
		}

		return pDungeon;
    }
}

void PVESystem::InsertInDunMember(const DWORD dwChaDbNum, const SPVEInDunID& sInDunID)
{
    s_member[dwChaDbNum] = sInDunID;
}

void PVESystem::RemoveInDunMember(const DWORD dwChaDbNum)
{
    PVEInDunMemberContainerIt it = s_member.find(dwChaDbNum);

    if (it != s_member.end())
    {
        s_member.erase(it);
    }
}

CPVEInDun* PVESystem::FindInstantDungeon(const DWORD dwChaDbNum)
{
    PVEInDunMemberContainerIt it = s_member.find(dwChaDbNum);

    if (it != s_member.end())
    {
        return FindInstantDungeon(it->second);
    }

    return 0;
}

void PVESystem::FrameMove(float fElpaseTime)
{
    if (IsPVEDBFile() == false)
    {
        ResetDB();
    }

    for (PVEInDunContainerIt it = s_indun.begin(); it != s_indun.end(); ++it)
    {
        (*it).second.FrameMove(fElpaseTime);
    }
}
