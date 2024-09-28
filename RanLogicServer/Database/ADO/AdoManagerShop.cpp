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

//! Billing ���� Shop, Item �Ǹ� ���� �α�
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

//! Shop ���� ������ �������� �����´�.
//! \param strUID ����ID
//! \param &v ��ǰ�� ���� ����
//! \return 
int AdoManager::GetPurchaseItem(const CString& strUID, std::vector<SHOPPURCHASE> &v)
{
    sc::writeLogError("Do not call this function. GetPurchaseItem");
    return sc::db::DB_ERROR;
}

//! ������ ������ �� �ִ� ��ǰ���� Ȯ���Ѵ�.
//! \param strPurKey �����ڵ�
//! \param nFlag(0 : ����, 1 : ���ſϷ�, 2 : ������ҽ�û, 3 : �������ó��)
//! \return 1:�������� ���� �̿��ǰ�:�������� ����
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