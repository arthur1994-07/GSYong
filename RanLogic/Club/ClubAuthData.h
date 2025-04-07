#ifndef _GLOGIC_CLUB_AUTH_DATA_H_
#define _GLOGIC_CLUB_AUTH_DATA_H_

#pragma once

#include <boost/unordered_map.hpp>
#include "./GLClubDefine.h"

namespace club
{

//! 클럽 권한 설명
class ClubDefineData
{
public:
    typedef boost::unordered_map<DWORD, std::string> MONSTER_MAP;
    typedef MONSTER_MAP::iterator                    MONSTER_MAP_ITER;
    typedef MONSTER_MAP::const_iterator              MONSTER_MAP_CITER;
    typedef MONSTER_MAP::value_type                  MONSTER_MAP_VALUE;

public:
    ClubDefineData();
    ~ClubDefineData();

protected:
    std::string m_FilePath;
    std::string m_AutorityDesc[AUTHORITY_NUMBER]; //! 권한 설명 문자열
    AUTH_GRADE m_Grade[AUTHORITY_GRADE]; //! 클럽 권한   
    
    MONSTER_MAP m_LogDeadByMonster; //! 특정 몬스터에게 사망시 로그를 남김
    MONSTER_MAP m_LogKillMonster; //! 특정 몬스터를 쓰러뜨릴시 로그를 남김

    int m_LogMemberLevelOver; //! 클럽멤버가 해당 레벨 이상이면 로그를 남긴다.
    std::vector<int> m_LogMemberLevelReach; //! 해당 레벨에 도달하면 로그를 남긴다.

protected:
    bool LoadAuthDesc();
    bool LoadAuthBase();
    bool LoadLogDeleteTime();
    bool LoadLogDeadByMonster();
    bool LoadLogKillMonster();
    bool LoadLogMemberLevel();

public:
    bool Load(const std::string& FileName);
    size_t GetDescSize() const { return AUTHORITY_NUMBER; }
    std::string GetDesc(size_t Index) const;
	const char* GetAuthDesc(size_t Index) const;
	const char* GetGradeName(size_t Index) const;

    size_t GetGradeSize() const { return AUTHORITY_GRADE; }
    AUTH_GRADE GetGrade(size_t Index) const;

    //! 특정 몬스터에게 사망시 로그를 남김
    bool IsLogDeadByMonster(const SNATIVEID& MonsterID) const;

    //! 특정 몬스터를 쓰러뜨릴시 로그를 남김
    bool IsLogKillMonster(const SNATIVEID& MonsterID) const;
    
    //! 해당 레벨에 도달하면 로그를 남기는가?
    bool IsLogMemberLevel(int ChaLevel) const;
};

} // namespace club
#endif // _GLOGIC_CLUB_AUTH_DATA_H_