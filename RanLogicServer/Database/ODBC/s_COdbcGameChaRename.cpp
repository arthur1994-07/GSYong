#include "../../pch.h"
#include "./s_COdbcManager.h"
#include "./OdbcStmt.h"
#include "../DBAction/DbAction.h"

#include "../../../SigmaCore/DebugInclude.h"

int COdbcManager::RenameCharacter(DWORD dwChaNum, char* szCharName)
{
    sc::writeLogError("COdbcManager::RenameCharacter. Use ADO");
    return sc::db::DB_ERROR;
/*
	SQLRETURN	sReturn = 0;	

	CString strName (szCharName);
	strName.Trim(_T(" ")); // �յ� ��������
	strName.Replace(_T("'"), _T("''")); // ' ���ڿ� '' �� ġȯ

	// ĳ���� �̸��� ���� ���̸� �Ѵ��� �����Ѵ�.
    if (strName.GetLength() >= (CHR_ID_LENGTH-1))
	{
		return sc::db::DB_ERROR;
	}
    
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call RenameCharacter(%d,'%s',?)}", dwChaNum, strName.GetString() ); 

	return m_pGameDB->ExecuteSpInt(szTemp);
*/
}