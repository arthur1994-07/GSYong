#ifndef _GLOGIC_CLUB_AUTH_DATA_H_
#define _GLOGIC_CLUB_AUTH_DATA_H_

#pragma once

#include <boost/unordered_map.hpp>
#include "./GLClubDefine.h"

namespace club
{

//! Ŭ�� ���� ����
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
    std::string m_AutorityDesc[AUTHORITY_NUMBER]; //! ���� ���� ���ڿ�
    AUTH_GRADE m_Grade[AUTHORITY_GRADE]; //! Ŭ�� ����   
    
    MONSTER_MAP m_LogDeadByMonster; //! Ư�� ���Ϳ��� ����� �α׸� ����
    MONSTER_MAP m_LogKillMonster; //! Ư�� ���͸� �����߸��� �α׸� ����

    int m_LogMemberLevelOver; //! Ŭ������� �ش� ���� �̻��̸� �α׸� �����.
    std::vector<int> m_LogMemberLevelReach; //! �ش� ������ �����ϸ� �α׸� �����.

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

    //! Ư�� ���Ϳ��� ����� �α׸� ����
    bool IsLogDeadByMonster(const SNATIVEID& MonsterID) const;

    //! Ư�� ���͸� �����߸��� �α׸� ����
    bool IsLogKillMonster(const SNATIVEID& MonsterID) const;
    
    //! �ش� ������ �����ϸ� �α׸� ����°�?
    bool IsLogMemberLevel(int ChaLevel) const;
};

} // namespace club
#endif // _GLOGIC_CLUB_AUTH_DATA_H_