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
    //! bMakeCacheData �̸� cache data �� ����� �� ���ΰ�?
    bool Insert(wb::EM_COUNTRY emCountry, DWORD ClubDbNum, bool bMakeCacheData);
    void Delete(DWORD ClubDbNum);
    void Search(wb::EM_COUNTRY emCountry, std::vector<DWORD>& vClubDbNum);
    
    //! ����/Ŭ���� �̸� ����� �д�
    void MakeCountryClub();
    const std::vector<wb::COUNTRY_CLUB_COUNT_INFO>& GetCountry();
    
    //! �����ڵ�� �ش� ������ ���ϴ� Ŭ���� DB ��ȣ�� �����´�
    //! CountryCode : ����
    //! vClubDbNum : Ŭ����ȣ
    void GetClub(wb::EM_COUNTRY CountryCode, std::vector<DWORD>& vClubDbNum);
};

} // namespace wb
#endif // _GLOGIC_SERVER_WORLD_BATTLE_CLUB_COUNTRY_H_