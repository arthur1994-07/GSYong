#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "./ClubAuthGrade.h"

#include "../../SigmaCore/DebugInclude.h"

namespace club
{

AuthGrade::AuthGrade()
{
}

AuthGrade::~AuthGrade()
{
}

void AuthGrade::Reset()
{
    for (size_t i=0; i<AUTHORITY_GRADE; ++i)
        m_Grade[i].Reset();
}

AUTH_GRADE AuthGrade::GetData(size_t Index)
{
    if (Index < AUTHORITY_GRADE)
    {
        return m_Grade[Index];
    }
    else
    {
        std::string Err(sc::string::format("overflow %1%", Index));
        AUTH_GRADE Temp(Err, 0);
        return Temp;
    }
}

DWORD AuthGrade::GetFlag(size_t Index)
{
    if (Index < AUTHORITY_GRADE)
        return m_Grade[Index].m_Flag;
    else
        return 0;
}

bool AuthGrade::SetFlag(size_t Index, DWORD Flag)
{
    if (Index < AUTHORITY_GRADE)
    {
        m_Grade[Index].m_Flag = Flag;
        return true;
    }
    else
    {
        return false;
    }
}

bool AuthGrade::Add(size_t Index, const AUTH_GRADE& Grade)
{
    if (Index < AUTHORITY_GRADE)
    {
        m_Grade[Index] = Grade;
        return true;
    }
    else
    {
        return false;
    }
}

bool AuthGrade::Change(size_t Index, const AUTH_GRADE& Grade)
{
    if (Index < AUTHORITY_GRADE)
    {
        m_Grade[Index] = Grade;
        return true;
    }
    else
    {
        return false;
    }
}

bool AuthGrade::Change(size_t IndexA, size_t IndexB)
{
    if (IndexA < AUTHORITY_GRADE && IndexB < AUTHORITY_GRADE)
    {
        AUTH_GRADE Temp = m_Grade[IndexA];
        m_Grade[IndexA] = m_Grade[IndexB];
        m_Grade[IndexB] = Temp;
        return true;
    }
    else
    {
        return false;
    }
}

bool AuthGrade::CheckAuth(size_t Index, AUTHORITY Auth)
{
    if (Index < AUTHORITY_GRADE)
        return m_Grade[Index].m_Flag & Auth ? true : false;
    else
        return false;
}

std::string AuthGrade::GetName(size_t Index) const
{
    if (Index < AUTHORITY_GRADE)
        return m_Grade[Index].m_Desc;
    else
        return std::string("");
}

bool AuthGrade::HaveSameName(const std::string& GradeName) const
{
    for (size_t i=0; i<AUTHORITY_GRADE; ++i)
    {
        if (m_Grade[i].m_Desc == GradeName)
            return true;
    }
    return false;
}

bool AuthGrade::ChangeName(size_t Index, const std::string& GradeName)
{
    if (Index >= AUTHORITY_GRADE)
        return false;
    if (HaveSameName(GradeName))
        return false;
    m_Grade[Index].m_Desc = GradeName;
    return true;
}

} // namespace club