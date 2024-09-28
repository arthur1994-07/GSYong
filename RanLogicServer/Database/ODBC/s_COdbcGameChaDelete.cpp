#include "../../pch.h"
#include "./s_COdbcManager.h"
#include "./OdbcStmt.h"
#include "../DBAction/DbAction.h"

#include "../../../SigmaCore/DebugInclude.h"


int COdbcManager::DeleteCharacter(int nUsrNum, int nChaNum)
{
    sc::writeLogError("COdbcManager::DeleteCharacter. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call sp_delete_character(%d, ?)}", nChaNum );

	return m_pGameDB->ExecuteSpInt(szTemp);
*/
}

int	COdbcManager::DelCharacter(int nUsrNum, int nChaNum, const char* szPass2)
{
    sc::writeLogError("COdbcManager::DelCharacter. Use ADO");
    return sc::db::DB_ERROR;
/*
	if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG)
	{
		TCHAR szTemp[128] = {0};
		_snprintf( szTemp, 128, "{call sp_UserCheckPass(%d,'%s', ?)}", nUsrNum, szPass2 );

		int nReturn = m_pUserDB->ExecuteSpInt(szTemp);

		if( nReturn == SQL_SUCCESS )
			return DeleteCharacter(nUsrNum, nChaNum);    
		else 
			return sc::db::DB_ERROR;
	}
	else
	{
		SQLRETURN sReturn = 0;
		std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
		if (!spConn)
			return sc::db::DB_ERROR;


		TCHAR szTemp[512] = {0};
		_snprintf( szTemp, 512, "SELECT UserNum FROM dbo.UserInfo WITH (NOLOCK) WHERE UserNum=%d AND UserPass2='%s'", nUsrNum, szPass2 );

		sReturn = ::SQLExecDirect(spConn->hStmt,
			(SQLCHAR*) szTemp, 
			SQL_NTS);

		if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
		{
			// Log 에 빈번하게 발생해서 삭제처리 했음
			return sc::db::DB_ERROR;
		}	

		while (true)
		{
			sReturn = ::SQLFetch(spConn->hStmt);
			if (sReturn == SQL_NO_DATA)
			{
				return sc::db::DB_ERROR;
			}

			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				Print(szTemp);		
				Print(GetErrorString(spConn->hStmt));
				return sc::db::DB_ERROR;
			}

			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				break;
			}
			else
			{
				break;
			}	
		}
		return DeleteCharacter(nUsrNum, nChaNum);
	}
*/
}

int	COdbcManager::DaumDelCharacter(int nUsrNum, int nChaNum, const char* szPass2)
{
    sc::writeLogError("COdbcManager::DaumDelCharacter. Use ADO");
    return sc::db::DB_ERROR;
	//return DeleteCharacter(nUsrNum, nChaNum);
}

int	COdbcManager::IdnDelCharacter(int nUsrNum, int nChaNum)
{
    sc::writeLogError("COdbcManager::IdnDelCharacter. Use ADO");
    return sc::db::DB_ERROR;
	//return DeleteCharacter(nUsrNum, nChaNum);
}

int	COdbcManager::GspDelCharacter(int nUsrNum, int nChaNum)
{
    sc::writeLogError("COdbcManager::GspDelCharacter. Use ADO");
    return sc::db::DB_ERROR;
	//return DeleteCharacter(nUsrNum, nChaNum);
}

int	COdbcManager::TerraDelCharacter(int nUsrNum, int nChaNum)
{
    sc::writeLogError("COdbcManager::TerraDelCharacter. Use ADO");
    return sc::db::DB_ERROR;
	//return DeleteCharacter(nUsrNum, nChaNum);
}

int COdbcManager::ExciteDelCharacter(int nUsrNum, int nChaNum)
{
    sc::writeLogError("COdbcManager::ExciteDelCharacter. Use ADO");
    return sc::db::DB_ERROR;
	//return DeleteCharacter(nUsrNum, nChaNum);
}

int COdbcManager::JapanDelCharacter(int nUsrNum, int nChaNum)
{
    sc::writeLogError("COdbcManager::JapanDelCharacter. Use ADO");
    return sc::db::DB_ERROR;
	//return DeleteCharacter(nUsrNum, nChaNum);
}

int	COdbcManager::GsDelCharacter(int nUsrNum, int nChaNum, const char* szPass2 )
{
    sc::writeLogError("COdbcManager::GsDelCharacter. Use ADO");
    return sc::db::DB_ERROR;
	//return DeleteCharacter(nUsrNum, nChaNum);
}
