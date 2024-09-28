#include "../../pch.h"
#include "../../Server/ServerCfg.h"
#include "../../Server/NetServer.h"
#include "../DBAction/DbAction.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::CreateNewCharacter(SCHARDATA2* pCharData2)
{
    sc::writeLogError("COdbcManager::CreateNewCharacter. Use ADO");
    return sc::db::DB_ERROR;
}

int COdbcManager::CharacterInfoBackup(SCHARDATA2* pCharData2)
{
    sc::writeLogError("COdbcManager::CharacterInfoBackup. Use ADO");
    return sc::db::DB_ERROR;
}


// 대만,홍콩
// 캐릭터 생성시 Temp Table에 캐릭터 레벨과 금액 정보를 암호화 해서 저장해 둔다.
int	COdbcManager::InsertCharInfoTemp( int nChaNum )
{
	std::tr1::shared_ptr<db::ODBC_STMT> spConn2 = m_pGameDB->GetConnection();
	if (!spConn2)
        return sc::db::DB_ERROR;
	SQLRETURN	sReturn = 0;

	char szTempMoney[CHAR_TEMPMONEY];
	LONGLONG lnTempMoney = 0;
	lnTempMoney <<= 0x04;
	sprintf( szTempMoney, "%I64d", lnTempMoney );
	size_t i;
	for( i = 0; i < strlen(szTempMoney); i++ )
	{
		szTempMoney[i] += 20;
	}

	int ChaNum = nChaNum;
	int nChaLevel = 1;	

	TCHAR szTemp[256] = {0};

	_snprintf( szTemp, 256, "INSERT INTO ChaLastInfo(ChaNum, ChaLevel, ChaMoney )"
		" Values( %d,%d,'%s' )", ChaNum, nChaLevel, szTempMoney );

	sReturn = ::SQLExecDirect(spConn2->hStmt,
		(SQLCHAR*)szTemp, 
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		Print(GetErrorString(spConn2->hStmt));
		//m_pGameDB->FreeConnection(spConn2);
		return sc::db::DB_ERROR;
	}

	//m_pGameDB->FreeConnection(spConn2);

	return sc::db::DB_OK;
}
