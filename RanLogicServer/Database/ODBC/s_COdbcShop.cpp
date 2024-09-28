#include "../../pch.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

int COdbcManager::GetPurchaseItem(const CString& strUID, std::vector<SHOPPURCHASE> &v)
{
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pShopDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;    

	/*
    TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "SELECT Purkey, ItemMain, ItemSub, PurPrice FROM viewShopPurchase WHERE "
							"useruid='%s' AND PurFlag=0", strUID.GetString() );
    */
    std::string QueryStr(
        sc::string::format(
            "SELECT Purkey, ItemMain, ItemSub, PurPrice FROM dbo.viewShopPurchase WHERE UserUID='%1%' AND PurFlag=0",
            strUID.GetString()));

	SQLCHAR    szPurKey[PURKEY_LENGTH+1] = {0}; SQLINTEGER cbPurKey = SQL_NTS;
    SQLINTEGER nItemMain = 0, cbItemMain = SQL_NTS;
    SQLINTEGER nItemSub  = 0, cbItemSub  = SQL_NTS;
    SQLRETURN  sReturn=0;
    SQLINTEGER ItemPrice = 0, cbItemPrice = SQL_NTS;

    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) QueryStr.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(QueryStr);		
		Print(GetErrorString(spConn->hStmt));
        return sc::db::DB_ERROR;
	}

    while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(QueryStr);		
			Print(GetErrorString(spConn->hStmt));
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{   
            ::SQLGetData(spConn->hStmt, 1, SQL_C_CHAR, szPurKey,   PURKEY_LENGTH+1, &cbPurKey);
            ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG, &nItemMain, 0, &cbItemMain);
            ::SQLGetData(spConn->hStmt, 3, SQL_C_LONG, &nItemSub,  0, &cbItemSub);
            ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG, &ItemPrice, 0, &cbItemPrice);

            SHOPPURCHASE sTemp;
            if (cbPurKey != 0 && cbPurKey != -1)
                sTemp.strPurKey = (char*) szPurKey;
            sTemp.wItemMain = static_cast<WORD> (nItemMain);
            sTemp.wItemSub  = static_cast<WORD> (nItemSub);
            sTemp.m_Price = ItemPrice;
            v.push_back(sTemp);
		}
		else
		{
			break;
		}
	}
    return sc::db::DB_OK;
}

int COdbcManager::SetPurchaseItem(const CString& strPurKey, int nFlag)
{
    sc::writeLogError("COdbcManager::SetPurchaseItem. Use ADO");
    return sc::db::DB_ERROR;
/*
	TCHAR szTemp[256] = {0};
	_snprintf(szTemp, 256, "{call sp_purchase_change_state('%s',%d,?)}", strPurKey.GetString(), nFlag);
	return m_pShopDB->ExecuteSpInt(szTemp);
*/
}