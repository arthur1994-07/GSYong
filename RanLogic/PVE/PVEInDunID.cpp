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
    // 1���� ����  DB ID ����
    if (dwCharDbNum < rhs.dwCharDbNum)
    {
        return true;
    }

    if (dwCharDbNum > rhs.dwCharDbNum)
    {
        return false;
    }

    // 2���� �δ� �� ID ����
    if (dwMapID < rhs.dwMapID)
    {
        return true;
    }

    if (dwMapID > rhs.dwMapID)
    {
        return false;
    }

    // 3���� �δ� ��� ����
    if (nLevel < rhs.nLevel)
    {
        return true;
    }

    if (nLevel > rhs.nLevel)
    {
        return false;
    }

	// 4���� �δ� ���� ī��Ʈ
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
