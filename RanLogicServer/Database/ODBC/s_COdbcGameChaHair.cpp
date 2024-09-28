#include "../../pch.h"
#include "./s_COdbcManager.h"
#include "./OdbcStmt.h"
#include "../DBAction/DbAction.h"

#include "../../../SigmaCore/DebugInclude.h"

int COdbcManager::SetChaHairColor(DWORD dwChaNum, int nHairColor)
{
    sc::writeLogError("COdbcManager::SetChaHairColor. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nHairColor < 0)
		return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call UpdateChaHairColor(%u,%d,?)}", dwChaNum, nHairColor );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	return nReturn;
*/
}

int COdbcManager::SetChaHairStyle(DWORD dwChaNum, int nHairStyle)
{
    sc::writeLogError("COdbcManager::SetChaHairStyle. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nHairStyle < 0)
		return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call UpdateChaHairStyle(%u,%d,?)}", dwChaNum, nHairStyle );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	return nReturn;
*/
}


int COdbcManager::SetChaFaceStyle(DWORD dwChaNum, int nFaceStyle)
{
    sc::writeLogError("COdbcManager::SetChaFaceStyle. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (nFaceStyle < 0)
		return sc::db::DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call UpdateChaFaceStyle(%u,%d,?)}", dwChaNum, nFaceStyle );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	return nReturn;
*/
}


int COdbcManager::SetChaGenderChange(DWORD dwChaNum, int nClass, int nSex, int nFace, int nHair, int nHairColor)
{
    sc::writeLogError("COdbcManager::SetChaGenderChange. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (dwChaNum <= 0 || nClass <= 0 || nHairColor < 0)
	{
		Print(_T("COdbcManager::SetChaGenderChange"));
		return sc::db::DB_ERROR;
	}

	TCHAR szTemp[256] = {0};
	_snprintf( szTemp, 128, "{call sp_UpdateChaGender(%u,%d,%d,%d,%d,%d,?)}",
        dwChaNum, nClass, nSex, nFace, nHair, nHairColor);

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;
*/
}

int COdbcManager::SetChaSchoolChange(DWORD dwChaNum, int nSchool)
{
    sc::writeLogError("COdbcManager::SetChaSchoolChange. Use ADO");
    return sc::db::DB_ERROR;
}
