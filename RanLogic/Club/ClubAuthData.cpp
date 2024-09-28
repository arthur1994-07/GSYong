#include "pch.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../Crow/GLCrowData.h"
#include "../Crow/GLCrowDataMan.h"
#include "./ClubAuthData.h"

#include "../../SigmaCore/DebugInclude.h"

namespace club
{

ClubDefineData::ClubDefineData()
    : m_LogMemberLevelOver(195)
{
}

ClubDefineData::~ClubDefineData()
{
}

bool ClubDefineData::Load(const std::string& FileName)
{
    m_FilePath = FileName;
    try
    {   
        sc::lua_init();
        if (!sc::lua_doFile(m_FilePath))
        {
            std::string ErrMsg(
                sc::string::format(
                "%1% file missing.(error 6512)",
                m_FilePath));
            sc::ErrorMessageBox(ErrMsg, false);
            return false;
        }

        LoadAuthDesc();
        LoadAuthBase();
        LoadLogDeleteTime();
        LoadLogDeadByMonster();
        LoadLogKillMonster();
        LoadLogMemberLevel();

        return true;
    }
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_FilePath,
            e.GetErrorMessage()));
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }    
}

bool ClubDefineData::LoadAuthDesc() 
{
    try
    {
        // Club AUTHORITY description
        // Index, L"Description"
        LuaPlus::LuaObject AuthDesc = sc::lua_getGlobalFromName("ClubAuthorityDesc");
		int nIndex = 0;
        for (LuaPlus::LuaTableIterator Iter(AuthDesc); Iter; Iter.Next())
        {
            for (LuaPlus::LuaTableIterator IterData(Iter.GetValue()); IterData; IterData.Next())
            {
                int DescIndex = static_cast<int> (IterData.GetValue().GetInteger()); IterData.Next();					

                if (DescIndex < 0 || DescIndex >= AUTHORITY_NUMBER)
                {
                    std::string ErrMsg(
                        sc::string::format(
                        "%1% ClubAuthorityDesc Index error. %2% < 0 or >= %3%",
                        m_FilePath,
                        DescIndex,
                        static_cast<int> (AUTHORITY_NUMBER)));
                    sc::ErrorMessageBox(ErrMsg, false);
                }
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == true )
				{
					const std::string strDesc(CGameTextMan::GetInstance().GetText("CLUB_AUTHORITYDESC", nIndex, CGameTextMan::EM_GAME_IN_TEXT));
					m_AutorityDesc[DescIndex] = strDesc;
				}
                else
                {
					std::wstring wDesc((const wchar_t*) IterData.GetValue().GetWString());
                    m_AutorityDesc[DescIndex] = sc::string::unicodeToAnsi(wDesc);
                }
				++nIndex;
            }
        }
        return true;
    }
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_FilePath,
            e.GetErrorMessage()));
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }
}

bool ClubDefineData::LoadAuthBase() 
{
    try
    {
        size_t Grade = 0;
        LuaPlus::LuaObject AuthBase = sc::lua_getGlobalFromName("ClubAuthorityBase");
        for (LuaPlus::LuaTableIterator Iter(AuthBase); Iter; Iter.Next())
        {
            for (LuaPlus::LuaTableIterator IterData(Iter.GetValue()); IterData; IterData.Next())
            {
                if (Grade >= AUTHORITY_GRADE)
                {
                    std::string ErrMsg(
                        sc::string::format(
                        "%1% ClubAuthorityBase Index error. %2% >= %3%",
                        m_FilePath,
                        Grade,
                        static_cast<int> (AUTHORITY_GRADE)));
                    sc::ErrorMessageBox(ErrMsg, false);
                    continue;
                }

				std::wstring wGradeName((const wchar_t*) IterData.GetValue().GetWString()); IterData.Next();
					
                DWORD AuthFlag = 0;
                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_MEMBER_JOIN;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_MEMBER_KICK;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_NOTICE_CHANGE;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_MARK_CHANGE;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_DEATH_MATCH;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_RANK_UP;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_CHANGE;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_ITEM;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_MONEY;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_ALLIANCE;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_ALLIANCE_DIS;
                IterData.Next();

                if (IterData.GetValue().GetBoolean())
                    AuthFlag |= AUTHORITY_CLUB_BATTLE;

				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					m_Grade[Grade].m_Desc = CGameTextMan::GetInstance().GetText( "CLUB_AUTHORITYBASE", Grade, CGameTextMan::EM_GAME_IN_TEXT);
				else
					m_Grade[Grade].m_Desc = sc::string::unicodeToAnsi(wGradeName);

                m_Grade[Grade].m_Flag = AuthFlag;
            }
            ++Grade;
        }

        return true;
    }
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_FilePath,
            e.GetErrorMessage()));
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }
}

std::string ClubDefineData::GetDesc(size_t Index) const
{
    if (Index < AUTHORITY_NUMBER)
    {
        return m_AutorityDesc[Index];
    }
    else
    {
        std::string Desc(
            sc::string::format(
                "ClubDefineData::GetDesc Index overflow %1%", Index));
        return Desc;
    }
}

const char* ClubDefineData::GetAuthDesc(size_t Index) const
{
	if (Index < AUTHORITY_NUMBER)
	{
		return m_AutorityDesc[Index].c_str();
	}
	else
	{
		std::string Desc(
			sc::string::format(
			"ClubDefineData::GetDesc Index overflow %1%", Index));
		return Desc.c_str();
	}
}

const char* ClubDefineData::GetGradeName(size_t Index) const
{
	if (Index < AUTHORITY_GRADE)
	{
		return m_Grade[Index].m_Desc.c_str();
	}
	else
	{
		std::string Desc(
			sc::string::format(
			"AUTH_GRADE Index overflow %1%", Index));
		AUTH_GRADE temp(Desc, 0);
		return temp.m_Desc.c_str();
	}
}

AUTH_GRADE ClubDefineData::GetGrade(size_t Index) const
{
    if (Index < AUTHORITY_GRADE)
    {
        return m_Grade[Index];
    }
    else
    {
        std::string Desc(
            sc::string::format(
                "AUTH_GRADE Index overflow %1%", Index));
        AUTH_GRADE temp(Desc, 0);
        return temp;
    }
}

bool ClubDefineData::LoadLogDeleteTime()
{
    /*
    -- 클럽 로그 지우기 시간
    ClubLogDefine =
    {
        -- 1일 = 60 * 60 * 24 = 86400
        DeleteTime = 86400,
    }
    */
    try
    {        
        LuaPlus::LuaObject ClubLogDefine = sc::lua_getGlobalFromName("ClubLogDefine");
        for (LuaPlus::LuaTableIterator Iter(ClubLogDefine); Iter; Iter.Next())
        {
            std::string strKey((const char*) Iter.GetKey().GetString());
            if (strKey == "DeleteTime")
                club::ClubLogDeleteTime = Iter.GetValue().GetInteger64();
        }
        return true;
    }
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_FilePath,
            e.GetErrorMessage()));
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }
}

bool ClubDefineData::LoadLogDeadByMonster()
{
    static const std::string ClubDefineRecordKilledByMonster("ClubRecordKilledByMonster");
    m_LogDeadByMonster.clear();

    try
    {
        LuaPlus::LuaObject ObjMonster = sc::lua_getGlobalFromName(ClubDefineRecordKilledByMonster.c_str());
        for (LuaPlus::LuaTableIterator Iter(ObjMonster); Iter; Iter.Next())
        {
            for (LuaPlus::LuaTableIterator IterData(Iter.GetValue()); IterData; IterData.Next())
            {
                SNATIVEID MonsterID;
                MonsterID.wMainID = static_cast<WORD> (IterData.GetValue().GetInteger()); IterData.Next();
                MonsterID.wSubID  = static_cast<WORD> (IterData.GetValue().GetInteger());                

                // Check monster
                const std::string CrowName = GLCrowDataMan::GetInstance().GetCrowName(MonsterID.Id());
                if (!CrowName.empty())
                {
                    m_LogDeadByMonster.insert(MONSTER_MAP_VALUE(MonsterID.Id(), CrowName));
                }
                else
                {
                    std::string ErrMsg(
                        sc::string::format(
                            "%1% %2% Monster not exist. %3%/%4%",
                            m_FilePath,
                            ClubDefineRecordKilledByMonster,
                            MonsterID.wMainID,
                            MonsterID.wSubID));
                    sc::ErrorMessageBox(ErrMsg, false);                    
                }
            }
        }
        return true;
    }
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_FilePath,
            e.GetErrorMessage()));
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }
}

bool ClubDefineData::IsLogDeadByMonster(const SNATIVEID& MonsterID) const
{
    MONSTER_MAP_CITER iter = m_LogDeadByMonster.find(MonsterID.Id());
    if (iter != m_LogDeadByMonster.end())
        return true;
    else
        return false;
}

bool ClubDefineData::LoadLogKillMonster()
{
    static const std::string ClubDefineRecordKillMonster("ClubRecordKillMonster");
    m_LogDeadByMonster.clear();

    try
    {
        LuaPlus::LuaObject ObjMonster = sc::lua_getGlobalFromName(ClubDefineRecordKillMonster.c_str());
        for (LuaPlus::LuaTableIterator Iter(ObjMonster); Iter; Iter.Next())
        {
            for (LuaPlus::LuaTableIterator IterData(Iter.GetValue()); IterData; IterData.Next())
            {
                SNATIVEID MonsterID;
                MonsterID.wMainID = static_cast<WORD> (IterData.GetValue().GetInteger()); IterData.Next();
                MonsterID.wSubID  = static_cast<WORD> (IterData.GetValue().GetInteger());                

                // Check monster
                const std::string CrowName = GLCrowDataMan::GetInstance().GetCrowName(MonsterID.Id());
                if (!CrowName.empty())
                {
                    m_LogKillMonster.insert(MONSTER_MAP_VALUE(MonsterID.Id(), CrowName));
                }
                else
                {
                    std::string ErrMsg(
                        sc::string::format(
                        "%1% %2% Monster not exist. %3%/%4%",
                        m_FilePath,
                        ClubDefineRecordKillMonster,
                        MonsterID.wMainID,
                        MonsterID.wSubID));
                    sc::ErrorMessageBox(ErrMsg, false);                    
                }
            }
        }
        return true;
    }
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_FilePath,
            e.GetErrorMessage()));
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }
}

bool ClubDefineData::IsLogKillMonster(const SNATIVEID& MonsterID) const
{
    MONSTER_MAP_CITER iter = m_LogKillMonster.find(MonsterID.Id());
    if (iter != m_LogKillMonster.end())
        return true;
    else
        return false;
}

bool ClubDefineData::LoadLogMemberLevel()
{
    static const std::string ClubRecordMemberLevel("ClubRecordMemberLevel");
    m_LogMemberLevelReach.clear();

    try
    {
        LuaPlus::LuaObject LevelObj = sc::lua_getGlobalFromName("ClubRecordMemberLevel");
        for (LuaPlus::LuaTableIterator Iter(LevelObj); Iter; Iter.Next())
        {
            std::string strKey((const char*) Iter.GetKey().GetString());
            if (strKey == "LevelOver")
                m_LogMemberLevelOver = Iter.GetValue().GetInteger();

            if (strKey == "LevelReach")
            {
                for (LuaPlus::LuaTableIterator IterLevel(Iter.GetValue()); IterLevel; IterLevel.Next())
                {
                    int LogLevel = IterLevel.GetValue().GetInteger();
                    m_LogMemberLevelReach.push_back(LogLevel);
                }
            }
        }
        return true;
    }
    catch (const LuaPlus::LuaException& e)
    {
        std::string ErrMsg(
            sc::string::format(
            "%1%, %2%",
            m_FilePath,
            e.GetErrorMessage()));
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }
}

bool ClubDefineData::IsLogMemberLevel(int ChaLevel) const
{
    if (ChaLevel >= m_LogMemberLevelOver)
        return true;

    for (size_t i=0; i<m_LogMemberLevelReach.size(); ++i)
    {
        if (m_LogMemberLevelReach[i] == ChaLevel)
            return true;
    }

    return false;
}

} // namespace club