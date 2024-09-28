#ifndef _GLOGIC_CLUB_AUTH_GRADE_H_
#define _GLOGIC_CLUB_AUTH_GRADE_H_

#pragma once

#include "./GLClubDefine.h"

namespace club
{

class AuthGrade
{
public:
    AuthGrade();
    ~AuthGrade();

protected:
    AUTH_GRADE m_Grade[AUTHORITY_GRADE]; //! 클럽 권한

public:
    void Reset();
    bool Add(size_t Index, const AUTH_GRADE& Grade);
    bool Change(size_t Index, const AUTH_GRADE& Grade);
    bool Change(size_t IndexA, size_t IndexB);
    AUTH_GRADE GetData(size_t Index);
    DWORD GetFlag(size_t Index);
    bool SetFlag(size_t Index, DWORD Flag);
    bool CheckAuth(size_t Index, AUTHORITY Auth);
    

    std::string GetName(size_t Index) const;
    bool HaveSameName(const std::string& GradeName) const;
    bool ChangeName(size_t Index, const std::string& GradeName);
    

    WORD Size() const { return AUTHORITY_GRADE; }
};

} // namespace club

#endif // _GLOGIC_CLUB_AUTH_GRADE_H_