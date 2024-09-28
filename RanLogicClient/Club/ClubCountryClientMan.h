#ifndef _GLOGIC_CLIENT_CLUB_COUNTRY_CLIENT_MAN_H_
#define _GLOGIC_CLIENT_CLUB_COUNTRY_CLIENT_MAN_H_

#pragma once

#include <string>
#include <vector>
#include "../../SigmaCore/Database/Sqlite/SQLite.h"
#include "../../RanLogic/WorldBattle/WorldBattleDefine.h"

namespace wb
{

//! Client, 국가/클럽 관리
class ClubCountryClientMan
{
public:
    ClubCountryClientMan();
    ~ClubCountryClientMan();

private:
    SQLite::Database m_SQLite;

private:
    bool MakeDb();
    void MakeOrderQuery(const std::vector<wb::search::OrderBy>& OrderList, std::string& Query);

public:
    bool AddClub(
        wb::EM_COUNTRY CountryCode,
        DWORD ClubDbNum,
        std::string& ClubName,
        DWORD ClubRank,
        int MasterLevel,
        std::string& MasterName,
        __time64_t OrganizeDate,
        DWORD MemberNum,
        std::string& ClubNotice);

    void Search(
        const std::string& SearchStr,
        const std::vector<wb::search::OrderBy>& Order,
        std::vector<wb::CLUB_INFO_BASE>& vInfo);

    void Reset();
    std::string GetClubName(DWORD ClubDbNum);
};

} // namespace wb

#endif // _GLOGIC_CLIENT_CLUB_COUNTRY_CLIENT_MAN_H_