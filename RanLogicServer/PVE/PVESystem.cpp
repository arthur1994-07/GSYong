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

	if (pTable.m_pTable == 0 ||			// ���� ����
		pTable().GetRowCount() == 0)	// ���ڵ� �� ���� ����
	{
		tbl.strID = strID;

		// �ʱⰪ ����
		tbl.nInCount = 0;
		tbl.nGenCount = 0;
		tbl.tGenTime = 0;

		strQuery = sc::string::format(strInsertQuery, tbl.strID, tbl.dwCharDbNum, 
			tbl.dwMapID, tbl.nInCount, tbl.nGenCount, tbl.nLevel, tbl.tGenTime);

		// ���ڵ尡 ���� ��� ���� ���� �Ѵ�.
		if (s_db.ExecuteSQL(strQuery) != SQLITE_OK)
		{
			OutputDataBaseError(_T("Insert Record Error"));
			return false;
		}
	}
	else
	{
		// ���ڵ�� ���ϼ��� ���� �ؾ� �Ѵ�.
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
				// Ŭ���̾�Ʈ���� ���� �ð��� �Ѱ��ܿ� ���� �Ѵ�.
				con.back().tGenTime = DiffTime(con.back().tGenTime, info.owntime);
			}

			if (con.back().tGenTime == 0)
			{
				continue;
			}

			tbl = con.back();

			/**
				�ð��� 0���� ���� �Կ� ���� �ؾ� �Ѵ�.
				�츮�� �ͼ� �δ� ������ tbl �� �־� ������
				�� ���� 0 ���� ���� �Ѵ�.

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
		�ڽ��� �Ҽӵ� �δ��� ������ ��� �Ѵ�.
		�̰��� �δ��� ������ ��� ���� ������ ������� �����ϱ� ���� �ʿ��ϴ�.

	 */
	CPVEInDun* pIndun = FindInstantDungeon(dwChaNum);

	if (pIndun)
	{
		tbl = GetMapTable(pIndun->GetID());

		TriggerSystem::SServerTriggerMapInfo info;
		info.mapid.dwID = tbl.dwMapID;

		if (PVESystem::GetMapInfo(info))
		{
			// Ŭ���̾�Ʈ���� ���� �ð��� �Ѱ��ܿ� ���� �Ѵ�.
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
