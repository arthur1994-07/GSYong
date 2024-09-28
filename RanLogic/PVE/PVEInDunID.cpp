#include "pch.h"
#include "PVEInDunID.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"

SPVEInDunID::SPVEInDunID()
{
	Init();
}

void SPVEInDunID::Init()
{
	dwCharDbNum = 0;
	dwMapID = SNATIVEID::ID_NULL;
	nLevel = 0;
	nNum = 0;
}

bool SPVEInDunID::IsVaild() const 
{
	return dwCharDbNum != 0;
}

bool SPVEInDunID::operator < (const SPVEInDunID& rhs) const
{
    // 1차로 고유  DB ID 기준
    if (dwCharDbNum < rhs.dwCharDbNum)
    {
        return true;
    }

    if (dwCharDbNum > rhs.dwCharDbNum)
    {
        return false;
    }

    // 2차로 인던 맵 ID 기준
    if (dwMapID < rhs.dwMapID)
    {
        return true;
    }

    if (dwMapID > rhs.dwMapID)
    {
        return false;
    }

    // 3차로 인던 등급 기준
    if (nLevel < rhs.nLevel)
    {
        return true;
    }

    if (nLevel > rhs.nLevel)
    {
        return false;
    }

	// 4차로 인던 생성 카운트
	if (nNum < rhs.nNum)
	{
		return true;
	}

	if (nNum > rhs.nNum)
	{
		return false;
	}

    return false;
}

bool SPVEInDunID::operator == (const SPVEInDunID& rhs) const
{
	if (this == &rhs)
	{
		return true;
	}

	if (dwCharDbNum != rhs.dwCharDbNum)
	{
		return false;
	}

	if (dwMapID != rhs.dwMapID)
	{
		return false;
	}

	if (nLevel != rhs.nLevel)
	{
		return false;
	}

	if (nNum != rhs.nNum)
	{
		return false;
	}

	return true;
}

bool SPVEInDunID::operator != (const SPVEInDunID& rhs) const
{
	return operator==(rhs) == false;
}
