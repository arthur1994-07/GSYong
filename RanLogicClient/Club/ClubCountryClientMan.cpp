#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "./ClubCountryClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace wb
{

ClubCountryClientMan::ClubCountryClientMan()
{
    MakeDb();
}

ClubCountryClientMan::~ClubCountryClientMan()
{
    if (m_SQLite.IsOpen())
        m_SQLite.Close();
}

void ClubCountryClientMan::Reset()
{
    std::string Query("DELETE FROM wbCountryClubInfo;");
    m_SQLite.ExecuteSQL(Query);
}

bool ClubCountryClientMan::MakeDb()
{
    // memory db 肺 包府茄促.
    if (SQLITE_OK != m_SQLite.Open(":memory:"))
    {
        sc::ErrorMessageBox(std::string("ClubCountryClientMan::MakeDb Open failed"), false);
        return false;
    }

    // Table 积己
    const char* CreateTable = "\
CREATE TABLE wbCountryClubInfo( \
    CountryCode     INTEGER NOT NULL, \
    ClubDbNum       INTEGER NOT NULL, \
    ClubName        TEXT    NOT NULL, \
    ClubRank        INTEGER NOT NULL, \
    MaserLevel      INTEGER NOT NULL, \
    MasterName      TEXT    NOT NULL, \
    OrganizeDate    INTEGER NOT NULL, \
    MemberNum       INTEGER NOT NULL, \
    ClubNotice      TEXT    NOT NULL, \
    PRIMARY KEY(CountryCode, ClubDbNum)); \
CREATE INDEX Idx_wbCountryClubInfoClubName ON wbCountryClubInfo (ClubName); \
CREATE INDEX Idx_wbCountryClubInfoMasterName ON wbCountryClubInfo (MasterName);" ;

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

bool ClubCountryClientMan::AddClub(
    wb::EM_COUNTRY CountryCode,
    DWORD ClubDbNum,
    std::string& ClubName,
    DWORD ClubRank,
    int MasterLevel,
    std::string& MasterName,
    __time64_t OrganizeDate,
    DWORD MemberNum,
    std::string& ClubNotice)
{
    sc::string::replaceForDb(ClubName);
    sc::string::replaceForDb(MasterName);
    sc::string::replaceForDb(ClubNotice);

    std::string Query(
        sc::string::format(" \
INSERT INTO wbCountryClubInfo(\
    CountryCode, ClubDbNum, ClubName, ClubRank, MaserLevel, \
    MasterName, OrganizeDate, MemberNum, ClubNotice) VALUES \
    (%1%, %2%, '%3%', %4%, %5%, '%6%', %7%, %8%, '%9%');",
    CountryCode,
    ClubDbNum,
    ClubName,
    ClubRank,
    MasterLevel,
    MasterName,
    OrganizeDate,
    MemberNum,
    ClubNotice));

    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
    {
        //sc::ErrorMessageBox(Query, false);
        return false;
    }
    else
    {
        return true;
    }
}

void ClubCountryClientMan::Search(
    const std::string& SearchStr,
    const std::vector<wb::search::OrderBy>& Order,
    std::vector<wb::CLUB_INFO_BASE>& vInfo)
{
    std::string KeyWord(SearchStr);
    sc::string::replaceForDb(KeyWord);

    std::string Query("SELECT CountryCode, ClubDbNum, ClubName, ClubRank, MaserLevel, MasterName, OrganizeDate, MemberNum, ClubNotice FROM wbCountryClubInfo ");
    
    if (!SearchStr.empty())
    {
        Query.append(
            sc::string::format(
                " WHERE ClubName LIKE '%%%1%%%' OR MasterName LIKE '%%%1%%%' ",
                KeyWord));
    }

    if (Order.empty())
    {
        Query.append(" ORDER BY ClubName;");
    }
    else
    {
        MakeOrderQuery(Order, Query);
    }

    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return;

        for (int i=0; i<nRowCount; ++i)
        {
            wb::EM_COUNTRY CountryCode = static_cast<wb::EM_COUNTRY> (rTable.GetValue<int> (0));
            DWORD ClubDbNum         = rTable.GetValue<DWORD> (1);
            std::string ClubName    = rTable.GetValue<std::string> (2);
            DWORD ClubRank          = rTable.GetValue<DWORD> (3);
            int MasterLevel         = rTable.GetValue<int> (4);
            std::string MasterName  = rTable.GetValue<std::string> (5);
            __time64_t OrganizeDate = rTable.GetValue<__time64_t> (6);
            DWORD MemberNum         = rTable.GetValue<DWORD> (7);
            std::string ClubNotice  = rTable.GetValue<std::string> (8);

            wb::CLUB_INFO_BASE ClubInfo(
                CountryCode,
                ClubDbNum,
                ClubRank,
                MemberNum,
                MasterLevel,
                OrganizeDate,
                ClubName,
                MasterName,
                ClubNotice);

            vInfo.push_back(ClubInfo);
            rTable.GoNext();
        }
    }
}

void ClubCountryClientMan::MakeOrderQuery(const std::vector<wb::search::OrderBy>& OrderList, std::string& Query)
{
    if (OrderList.size() > 0)
    {
        Query.append(" ORDER BY ");
        for (size_t i=0; i<OrderList.size(); ++i)
        {
            bool bType = false;
            const wb::search::OrderBy& Order = OrderList[i];
            switch (Order.m_Type)
            {
            case wb::search::CLUB_NAME_SEARCH:
                Query.append("ClubName ");
                bType = true;
                break;

            case wb::search::CLUB_RANK:
                Query.append("ClubRank ");
                bType = true;
                break;

            case wb::search::CLUB_MASTER_LEVEL:
                Query.append("MaserLevel ");
                bType = true;
                break;

            case wb::search::CLUB_MASTER_NAME:
                Query.append("MasterName ");
                bType = true;
                break;

            case wb::search::CLUB_MEMBER_NUM:
                Query.append("MemberNum ");
                bType = true;
                break;

            default:
                break;
            }

            if (bType)
            {
                if (Order.m_AscDsc == wb::search::ASC)
                    Query.append("ASC");
                else
                    Query.append("DESC");

                if (i != (OrderList.size()-1))
                    Query.append(", ");
            }
        }
    }
}

std::string ClubCountryClientMan::GetClubName(DWORD ClubDbNum)
{
    std::string ClubName("");
    std::string Query(
        sc::string::format(
            "SELECT ClubName FROM wbCountryClubInfo WHERE ClubDbNum=%1%;",
            ClubDbNum));
    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return ClubName;

        for (int i=0; i<nRowCount; ++i)
        {            
            ClubName = rTable.GetValue<std::string> (0);            
            rTable.GoNext();
        }
    }

    return ClubName;
}

} // namespace wb