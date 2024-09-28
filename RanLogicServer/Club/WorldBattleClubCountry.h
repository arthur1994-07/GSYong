#ifndef _GLOGIC_SERVER_WORLD_BATTLE_CLUB_COUNTRY_H_
#define _GLOGIC_SERVER_WORLD_BATTLE_CLUB_COUNTRY_H_

#pragma once

#include <vector>
#include "../../SigmaCore/Database/Sqlite/SQLite.h"
#include "../../RanLogic/WorldBattle/WorldBattleDefine.h"

//struct GLCLUBINFO;

//! namespace world battle 2012-02-13 jgkim
namespace wb
{

enum EM_COUNTRY;

class ClubCountry
{
public:
    ClubCountry();
    ~ClubCountry();

protected:
    __time64_t m_LastUpdateDate;
    SQLite::Database m_SQLite;
    std::vector<wb::COUNTRY_CLUB_COUNT_INFO> m_vClubInfo;

private:
    bool MakeDb();
    
public:
    //! bMakeCacheData 미리 cache data 를 만들어 둘 것인가?
    bool Insert(wb::EM_COUNTRY emCountry, DWORD ClubDbNum, bool bMakeCacheData);
    void Delete(DWORD ClubDbNum);
    void Search(wb::EM_COUNTRY emCountry, std::vector<DWORD>& vClubDbNum);
    
    //! 국가/클럽수 미리 만들어 둔다
    void MakeCountryClub();
    const std::vector<wb::COUNTRY_CLUB_COUNT_INFO>& GetCountry();
    
    //! 국가코드로 해당 국가에 속하는 클럽의 DB 번호를 가져온다
    //! CountryCode : 국가
    //! vClubDbNum : 클럽번호
    void GetClub(wb::EM_COUNTRY CountryCode, std::vector<DWORD>& vClubDbNum);
};

} // namespace wb
#endif // _GLOGIC_SERVER_WORLD_BATTLE_CLUB_COUNTRY_H_