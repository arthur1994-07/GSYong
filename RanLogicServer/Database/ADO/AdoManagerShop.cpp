#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../RanLogic/PointShop/PointShopDefine.h"
#include "./ADOManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

int AdoManager::PointShopUpdateDate(__time64_t& UpdateTime, bool bStandAlone)
{
	std::string strConnString;
	if ( bStandAlone )
	{
		strConnString = m_GameDBConnString;
	}
	else
	{
		strConnString = m_ShopDBConnString;
	}

    sc::db::AdoExt Ado(strConnString);
    if (!Ado.Execute4Cmd("dbo.sp_ShopUpdateDate", adCmdStoredProc))
        return sc::db::DB_ERROR;
    
    if (Ado.GetEOF())
    {
        Ado.PRINT("PointShopUpdateDate RecordSet is Empty.");
        return sc::db::DB_ERROR;
    }
    
    _variant_t vartShopDate;
    do
    {            
        Ado.GetCollect(0, vartShopDate);            
    } while (Ado.Next());

    if (!Ado.IsCollectedAll())
        return sc::db::DB_ERROR;
    
    SYSTEMTIME stShopDate;        
    VariantTimeToSystemTime(vartShopDate.date, &stShopDate);
    CTime ctShopDate(stShopDate);
    UpdateTime = ctShopDate.GetTime();

    return sc::db::DB_OK;
}

int AdoManager::PointShopList(__time64_t& UpdateTime, std::vector<pointshop::SellItem>& ItemList, bool bStandAlone)
{
    PointShopUpdateDate(UpdateTime, bStandAlone);

	std::string strConnString;
	if ( bStandAlone )
	{
		strConnString = m_GameDBConnString;
	}
	else
	{
		strConnString = m_ShopDBConnString;
	}

    sc::db::AdoExt Ado(strConnString);
    if (!Ado.Execute4Cmd("dbo.sp_PointShopSelect", adCmdStoredProc))
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
    {
        Ado.PRINT("ADO:There's no data. RecordSet is Empty.");
        return sc::db::DB_ERROR;
    }
    
    int ItemMid = 0;
    int ItemSid = 0;
    int ItemPrice  = 0;
    do
    {            
        Ado.GetCollect(0, ItemMid);
        Ado.GetCollect(1, ItemSid);
        Ado.GetCollect(2, ItemPrice);
        if (!Ado.IsCollectedAll())
            return sc::db::DB_ERROR;

        pointshop::SellItem Item(ItemMid, ItemSid, ItemPrice);
        ItemList.push_back(Item);
    } while (Ado.Next());

    return sc::db::DB_OK;
}

//! Billing 관련 Shop, Item 판매 정보 로그
int AdoManager::ShopSaleLog(
    const SNATIVEID& ItemId,
    __int64 Price,
    EM_BILLING_SELL_TYPE Type,
    int UserDbNum,
    int ServerNum,
    int ChaDbNum,
    int ChaLevel)
{
    sc::db::AdoExt Ado(m_ShopDBConnString);    
    
    APPEND_IPARAM_INT  (Ado, "@ItemMid",   ItemId.Mid());
    APPEND_IPARAM_INT  (Ado, "@ItemSid",   ItemId.Sid());
    APPEND_IPARAM_INT  (Ado, "@Price",     Price);
    APPEND_IPARAM_INT  (Ado, "@UserNum",   UserDbNum);
    APPEND_IPARAM_SMALL(Ado, "@ServerNum", static_cast<unsigned short> (ServerNum));
    APPEND_IPARAM_INT  (Ado, "@ChaNum",    ChaDbNum);
    APPEND_IPARAM_INT  (Ado, "@ChaLevel",  ChaLevel);
    APPEND_IPARAM_SMALL(Ado, "@SaleType",  static_cast<unsigned short> (Type));

    return ExecuteStoredProcedure("dbo.sp_SaleLogInsert", Ado);    
}

//! Shop 에서 구입한 아이템을 가져온다.
//! \param strUID 유저ID
//! \param &v 상품을 담을 벡터
//! \return 
int AdoManager::GetPurchaseItem(const CString& strUID, std::vector<SHOPPURCHASE> &v)
{
    sc::writeLogError("Do not call this function. GetPurchaseItem");
    return sc::db::DB_ERROR;
}

//! 실제로 가져갈 수 있는 상품인지 확인한다.
//! \param strPurKey 구입코드
//! \param nFlag(0 : 구매, 1 : 구매완료, 2 : 구매취소신청, 3 : 구매취소처리)
//! \return 1:가져갈수 있음 이외의값:가져갈수 없음
int AdoManager::SetPurchaseItem(const CString& strPurKey, int nFlag)
{    
    sc::db::AdoExt Ado(m_ShopDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamVarchar("@Purkey", strPurKey.GetString(), strPurKey.GetLength());
    Ado.AppendIParamInteger    ("@Purflag", nFlag);
        
    Ado.AppendOParamInteger("@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.sp_purchase_change_state", "@nReturn");
}

int AdoManager::InsertShopItemMap(int nItemMID, int nItemSID)
{
    sc::db::AdoExt Ado(m_ShopDBConnString);
    Ado.AppendRParamInteger();

    Ado.AppendIParamInteger("@ItemMain", nItemMID);
    Ado.AppendIParamInteger("@ItemSub", nItemSID);
        
    Ado.AppendOParamInteger("@Return");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.ShopItemMap_Exists", "@Return");
}

int AdoManager::InsertUserAttend(
    int nUserNum, CString strUserName, __time64_t tAttendTime, int nCount,
    int nAttendReward, int nItemMID, int nItemSID)
{
    if (nUserNum < 0)
    {
        sc::writeLogError("AdoManager::InsertUserAttend nUserNum < 0");
        return sc::db::DB_ERROR;
    }

    CTime cTemp(tAttendTime);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

    sc::db::AdoExt Ado(m_ShopDBConnString);
    Ado.AppendRParamInteger();
    
    Ado.AppendIParamInteger    ("@UserNum", nUserNum);
    Ado.AppendIParamVarchar("@UserID", strUserName.GetString(), strUserName.GetLength());
    Ado.AppendIParamVarchar("@LoginDate", strTime.GetString(), strTime.GetLength());
    Ado.AppendIParamInteger    ("@LogCnt", nCount);
    Ado.AppendIParamInteger    ("@GetItem", nAttendReward);
    Ado.AppendIParamInteger    ("@ItemMain", nItemMID);
    Ado.AppendIParamInteger    ("@ItemSub", nItemSID);
        
    Ado.AppendOParamInteger("@Return");

    int nOutput = Ado.ExecuteStoredProcedureIntReturn("dbo.AttendLog_Create", "@Return");
    
    if (nOutput != sc::db::DB_OK) 
    {        
        if (nOutput == sc::db::DB_ERROR)
        {
            sc::writeLogError(
                sc::string::format(
                    "DB:InsertUserAttend - Insert Log Attend %d %s %s %d %d %d %d",
                    nUserNum,
                    strUserName.GetString(),
                    strTime.GetString(),
                    nCount,
                    nAttendReward,
                    nItemMID,
                    nItemSID));
        }
        else if (nOutput == sc::db::DB_INSERT_ITEM)
        {
            sc::writeLogError(
                sc::string::format(
                    "DB:InsertUserAttend - Insert Item %d %s %s %d %d %hd %hd",
                    nUserNum,
                    strUserName.GetString(),
                    strTime.GetString(),
                    nCount,
                    nAttendReward,
                    nItemMID,
                    nItemSID));			
        }
        else if ( nOutput == sc::db::DB_NOT_ITEM )
        {
            sc::writeLogError(
                sc::string::format(
                    "DB:InsertUserAttend - No ShopItemMap %d %s %s %d %d %hd %hd",
                    nUserNum,
                    strUserName.GetString(),
                    strTime.GetString(), 
                    nCount,
                    nAttendReward,
                    nItemMID,
                    nItemSID));
        }
    }
    return nOutput;
}

} // namespace db