#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "WorldBattleClubCountry.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace wb
{

ClubCountry::ClubCountry()
{
    CTime CurretDate(CTime::GetCurrentTime());
    m_LastUpdateDate = CurretDate.GetTime();
    // 250 개국
    m_vClubInfo.reserve(250);
    MakeDb();
}

ClubCountry::~ClubCountry()
{
    if (m_SQLite.IsOpen())
        m_SQLite.Close();
}

bool ClubCountry::MakeDb()
{
    // memory db 로 관리한다.
    if (SQLITE_OK != m_SQLite.Open(":memory:"))
    {
        sc::ErrorMessageBox(std::string("ClubCountry::MakeDb Open failed"), false);
        return false;
    }

    // Table 생성
    const char* CreateTable = "\
CREATE TABLE wbCountryClubInfoAgent( \
CountryCode     INTEGER NOT NULL, \
ClubDbNum       INTEGER NOT NULL); \
CREATE INDEX Idx_wbCountryClubInfoAgentCode ON wbCountryClubInfoAgent (CountryCode); \
CREATE INDEX Idx_wbCountryClubInfoAgentClubDbNum ON wbCountryClubInfoAgent (ClubDbNum);" ;

    if (SQLITE_OK != m_SQLite.ExecuteSQL(CreateTable))
    {
        sc::ErrorMessageBox(std::string("ClubCountry::MakeDb Create Table failed"), false);
        return false;
    }
    else
    {
        return true;
    }

}

bool ClubCountry::Insert(EM_COUNTRY emCountry, DWORD ClubDbNum, bool bMakeCacheData)
{
    std::string Query(
        sc::string::format(
            "INSERT INTO wbCountryClubInfoAgent(CountryCode, ClubDbNum) VALUES (%1%, %2%);",
            emCountry,
            ClubDbNum));
    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
    {
        sc::ErrorMessageBox(Query, false);
        return false;
    }
    else
    {
        if (bMakeCacheData)
            MakeCountryClub();
        return true;
    }
}

void ClubCountry::Delete(DWORD ClubDbNum)
{
    std::string Query(
        sc::string::format(
            "DELETE FROM wbCountryClubInfoAgent WHERE ClubDbNum=%1%",
            ClubDbNum));
    m_SQLite.ExecuteSQL(Query);
    MakeCountryClub();
}

void ClubCountry::Search(EM_COUNTRY emCountry, std::vector<DWORD>& vClubDbNum)
{
    std::string Query(
        sc::string::format(
            "SELECT ClubDbNum FROM wbCountryClubInfoAgent WHERE CountryCode = %1%;",
            emCountry));
    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return;

        for (int i=0; i<nRowCount; ++i)
        {
            DWORD ClubDbNum = rTable.GetValue<DWORD> (0);
            vClubDbNum.push_back(ClubDbNum);
            rTable.GoNext();
        }
    }
}

void ClubCountry::MakeCountryClub()
{
    m_vClubInfo.clear();

    std::string Query("SELECT CountryCode, COUNT(ClubDbNum) FROM wbCountryClubInfoAgent GROUP BY CountryCode;");
    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return;

        for (int i=0; i<nRowCount; ++i)
        {            
            int Country    = rTable.GetValue<int> (0);
            WORD ClubCount = rTable.GetValue<WORD> (1);
            wb::COUNTRY_CLUB_COUNT_INFO Info(EM_COUNTRY(Country), ClubCount);
            m_vClubInfo.push_back(Info);
            rTable.GoNext();
        }
    }
}

const std::vector<COUNTRY_CLUB_COUNT_INFO>& ClubCountry::GetCountry()
{
    return m_vClubInfo;
}

void ClubCountry::GetClub(wb::EM_COUNTRY CountryCode, std::vector<DWORD>& vClubDbNum)
{
    std::string Query(
        sc::string::format(
            "SELECT ClubDbNum FROM wbCountryClubInfoAgent WHERE CountryCode=%1%;", CountryCode));

    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return;

        for (int i=0; i<nRowCount; ++i)
        {
            DWORD ClubDbNum = rTable.GetValue<int> (0);
            vClubDbNum.push_back(ClubDbNum);
            rTable.GoNext();
        }
    }
}

} // namespace wb