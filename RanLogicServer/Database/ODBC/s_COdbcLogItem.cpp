#include "../../pch.h"
#include "./OdbcStmt.h"
#include "./s_COdbcManager.h"
#include "../DbDefineLog.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/* call sp_logitemexchange_insert (
    @NIDMain    int, @NIDSub     int,	@SGNum        int,	@SvrNum       int,	@FldNum   int,
	@MakeType   int, @MakeNum    money, @ItemAmount   int,	@ItemFromFlag int,	@ItemFrom int,
	@ItemToFlag int, @ItemTo     int,	@ExchangeFlag int,	@Damage       int,	@Defense  int,
	@Fire       int, @Ice        int,	@Poison       int,	@Electric     int,	@Spirit   int,
	@CostumeMID int, @CostumeSID int,	@TradePrice   money, ?)
*/

int COdbcManager::LogItemExchange(const LOG_ITEM_EXCHANGE& sEXCHANGE)
{
    /*int temp = 0;
    std::string Query(
        sc::string::format(
            "{call sp_logitemexchange_insert(%d,%d,%d,%d,%d,%d,%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,?)}",
			sEXCHANGE.m_nNID_M, sEXCHANGE.m_nNID_S, sEXCHANGE.m_nSGNum,sEXCHANGE.m_nSvrNum,
			sEXCHANGE.m_nFldNum, sEXCHANGE.m_nMakeType, sEXCHANGE.m_lnMakeNum, sEXCHANGE.m_nNum,
			sEXCHANGE.m_nFromType, sEXCHANGE.m_nItemFrom, sEXCHANGE.m_nToType, sEXCHANGE.m_nItemTo,
			sEXCHANGE.m_nExchangeFlag, sEXCHANGE.m_nDamage, sEXCHANGE.m_nDefense, sEXCHANGE.m_nFire,
			sEXCHANGE.m_nIce, sEXCHANGE.m_nPoison, sEXCHANGE.m_nElectric, sEXCHANGE.m_nSpirit,
			sEXCHANGE.m_nCOSTUME_MID, sEXCHANGE.m_nCOSTUME_SID, temp));
		
	int nReturn = m_pLogDB->ExecuteSpInt(Query);
	if (nReturn == 1)
        return sc::db::DB_OK;
	else*/
	return sc::db::DB_ERROR;
}

int	COdbcManager::LogMoneyExchange(const LOG_MONEY_EXCHANGE &_sLOG)
{
	/*int temp = 0;
    std::string Query(
        sc::string::format(
            "{call sp_logitemexchange_insert(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%I64d,?)}",
            temp, temp, _sLOG.m_nSGNum, _sLOG.m_nSvrNum, _sLOG.m_nFldNum,
            temp, temp, temp, _sLOG.m_FromType, _sLOG.m_nItemFrom,
            _sLOG.m_ToType, _sLOG.m_nItemTo, _sLOG.m_nExchangeFlag, temp, temp,
            temp, temp, temp, temp, temp,
            temp, temp, _sLOG.m_lnPrice));
	int nReturn = m_pLogDB->ExecuteSpInt(Query);
	if (nReturn == 1)
        return sc::db::DB_OK;
	else*/
	return sc::db::DB_ERROR;
}

int COdbcManager::LogPointShopExchange(int nFromType, DWORD ChaDbNum, int nExchangeFlag, WORD ItemMid, WORD ItemSid, int nPoint, int nExchangeFlagDetail )
{
	return sc::db::DB_ERROR;
}

int COdbcManager::LogItemConversion(
	const LOG_ITEM_CONVERSION &_sLOG )
{
	/*int temp = 0;
    std::string Query(
        sc::string::format(
	        "{call sp_logitemexchange_insert(%d,%d,%d,%d,%d,%d,%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,?)}",
            _sLOG.m_nNIDMain, _sLOG.m_nNIDSub, _sLOG.m_nSGNum, _sLOG.m_nSvrNum,
            _sLOG.m_nFldNum, _sLOG.m_nMakeType, _sLOG.m_lnMakeNum, temp,
            _sLOG.m_nFromType, _sLOG.m_nItemFrom, temp, temp, temp, 
            _sLOG.m_nDamage, _sLOG.m_nDefense, _sLOG.m_nFire, _sLOG.m_nIce,
            _sLOG.m_nPoison, _sLOG.m_nElectric, _sLOG.m_nSpirit, _sLOG.m_nCOSTUME_MID,
            _sLOG.m_nCOSTUME_SID, temp));
	int nReturn = m_pLogDB->ExecuteSpInt(Query);
	if (nReturn == 1)
        return sc::db::DB_OK;
	else*/
	return sc::db::DB_ERROR;	
}

LONGLONG COdbcManager::GetItemMaxNum(
	int nSGNum, 
    int nSvrNum, 
    int nFldNum, 
    std::vector<VIEWLOGITEMEXCHANGEMAX> &v)
{
    sc::writeLogError("COdbcManager::GetItemMaxNum. Use ADO");
    return sc::db::DB_ERROR;
/*
    SQLRETURN sReturn = 0;
	LONGLONG llMaxNum = 0; SQLINTEGER cbMaxNum=SQL_NTS;
    SQLINTEGER nNIDMain = 0, cbNIDMain =SQL_NTS;
    SQLINTEGER nNIDSub  = 0, cbNIDSub  =SQL_NTS;
    SQLINTEGER nMakeType= 0, cbMakeType=SQL_NTS;

    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pLogDB->GetConnection();
	if (!spConn)
        return sc::db::DB_ERROR;

    std::string Query(
        sc::string::format(
            "SELECT MaxNum, NIDMain, NIDSub, MakeType FROM dbo.LogItemMax WITH (NOLOCK) WHERE SGNum=%1% AND SvrNum=%2% AND FldNum=%3%",
            nSGNum, nSvrNum, nFldNum));

    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(Query);
		Print(GetErrorString(spConn->hStmt));
        return sc::db::DB_ERROR;
	}

    while (true)
	{
		sReturn = ::SQLFetch(spConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(Query);
			Print(GetErrorString(spConn->hStmt));            
            return sc::db::DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{   
            ::SQLGetData(spConn->hStmt, 1, SQL_C_SBIGINT, &llMaxNum, 0, &cbMaxNum);
            ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG,	 &nNIDMain, 0, &cbNIDMain);
            ::SQLGetData(spConn->hStmt, 3, SQL_C_LONG,	 &nNIDSub,  0, &cbNIDSub);
            ::SQLGetData(spConn->hStmt, 4, SQL_C_LONG,	 &nMakeType,0, &cbMakeType);

            VIEWLOGITEMEXCHANGEMAX sTemp;

            sTemp.llMaxNum  = llMaxNum;
            sTemp.nMakeType = nMakeType;
            sTemp.wItemMain = static_cast<WORD> (nNIDMain);
            sTemp.wItemSub  = static_cast<WORD> (nNIDSub);

            v.push_back(sTemp);
		}
		else
		{
			break;
		}
	}
    return sc::db::DB_OK;
*/
}

int COdbcManager::GetLogItem(std::vector<DWORD>& LogItemData)
{
    SQLRETURN sReturn = 0;
    SQLINTEGER ItemMain = 0, cbItemMain =SQL_NTS;
    SQLINTEGER ItemSub = 0, cbItemSub =SQL_NTS;
    
    std::tr1::shared_ptr<db::ODBC_STMT> spConn = m_pUserDB->GetConnection();
    if (!spConn)
        return sc::db::DB_ERROR;

    std::string Query("SELECT ItemMain, ItemSub FROM dbo.LogItemData WHERE ItemLog=1");

    sReturn = ::SQLExecDirect(spConn->hStmt, (SQLCHAR*) Query.c_str(), SQL_NTS);
    if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
    {
        Print(Query);
        Print(GetErrorString(spConn->hStmt));
        return sc::db::DB_ERROR;
    }

    while (true)
    {
        sReturn = ::SQLFetch(spConn->hStmt);
        if (sReturn == SQL_ERROR ||                                                          sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(Query);
            Print(GetErrorString(spConn->hStmt));            
            return sc::db::DB_ERROR;
        }

        if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
        {   
            ::SQLGetData(spConn->hStmt, 1, SQL_C_LONG, &ItemMain, 0, &cbItemMain);
            ::SQLGetData(spConn->hStmt, 2, SQL_C_LONG, &ItemSub, 0, &cbItemSub);
            
            SNATIVEID ItemID(static_cast<WORD> (ItemMain), static_cast<WORD> (ItemSub));
            LogItemData.push_back(ItemID.Id());
        }
        else
        {
            break;
        }
    }
    return sc::db::DB_OK;
}

int COdbcManager::LogItemListInsertUpdate(int ItemMid, int ItemSid, const std::string& ItemName)
{
    std::string ItemNameTemp(ItemName);
    sc::string::replaceForDb(ItemNameTemp);
    char szItemName[100] = {0};
    size_t NameLen = ItemNameTemp.length();
    StringCchCopy(szItemName, 100, ItemNameTemp.c_str());

    std::string Query(
        sc::string::format(
            "{call dbo.ItemListInsertUpdate(%1%, %2%, '%3%')}",
            ItemMid,
            ItemSid,
            szItemName));
    return m_pLogDB->ExecuteSp(Query);
}

int COdbcManager::UserCountryUpdate(int UserDbNum, wb::EM_COUNTRY Country)
{
    sc::writeLogError(
        std::string("COdbcManager::UserCountryUpdate. do not call this function. use ado."));
    return sc::db::DB_ERROR;
}