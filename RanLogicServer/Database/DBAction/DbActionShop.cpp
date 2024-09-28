#include "../../pch.h"
#include "../../RanLogic/PointShop/PointShopDefine.h"
#include "../../RanLogic/Msg/PointShopMsg.h"
#include "../../Character/GLChar.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "../DbManager.h"
#include "DbActionShop.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

//! Shop ���� ������ �������� �����´�.
//! ������ �������� ������ ���� �� �� �ִ� �������� �ƴϴ�.
//! �������� ���������� ������ ������ �� �ִ��� �ٽ� Ȯ���ؾ� �Ѵ�.
//! ���� ���� ���ſ� ��� �� ���ӳ� ĳ������ ����ȭ ����.
GetPurchaseItem::GetPurchaseItem(const CString& strUID, DWORD dwCharID, bool bFirstJoin)
    : DbAction(SHOP_DB)
    , m_strUID(strUID)
    , m_dwCharID(dwCharID)
    , m_bFirstJoin(bFirstJoin)
{    
}

int GetPurchaseItem::Execute(NetServer* pServer)
{
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return NET_ERROR;
    
    m_pDbManager->GetPurchaseItem(m_strUID, m_vItem);
    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
	GLChar* pChar = pGaeaServer->GetCharByDbNum(m_dwCharID);
	if (pChar)
	{
		pChar->SETSHOPPURCHASE(m_vItem);
		pChar->SENDBOXITEMINFO(m_bFirstJoin);
	}
    return NET_OK;
}

//! ������ ������ �� �ִ� ��ǰ���� Ȯ���Ѵ�.
//! �Է°�
//! nFlag (0 : ����, 1 : ���ſϷ�, 2 : ������ҽ�û, 3 : �������ó��)
//! ��°�
//! 1 : �������� ����
//! �̿��ǰ� : �������� ����
SetPurchaseItem::SetPurchaseItem(const CString& strPurKey, int nFlag )
    : DbAction(SHOP_DB)
    , m_strPurKey(strPurKey)
    , m_nFlag(nFlag)
{    
}

int SetPurchaseItem::Execute(NetServer* pServer)
{
    return m_pDbManager->SetPurchaseItem(m_strPurKey, m_nFlag);
}

//! ������ �������� �κ��� �ֱ⿡ �������� ��� flag ����
PurchaseItem_Restore::PurchaseItem_Restore(const CString& strPurKey)
    : DbAction(SHOP_DB)
    , m_strPurKey(strPurKey)
{    
}

int PurchaseItem_Restore::Execute(NetServer* pServer)
{
    return m_pDbManager->SetPurchaseItem(m_strPurKey, 0);
}

//! ������ �������� �κ����� �����ɴϴ�.
PurchaseItem_Get::PurchaseItem_Get(
	DWORD dwClient,
	DWORD dwUserID,
	const CString& strPurKey,
	SNATIVEID nidITEM,
	DWORD dwInvenPos )
	: DbAction(SHOP_DB)
    , m_ClientSlot( dwClient )
	, m_dwUserID( dwUserID )
	, m_dwInvenPos( dwInvenPos )
    , m_strPurKey(strPurKey)
    , m_nidITEM(nidITEM)
{	
}

int PurchaseItem_Get::Execute(NetServer* pServer)
{
	if (!pServer)
	{
		return sc::db::DB_ERROR;
	}
	else
	{
		int nRET = m_pDbManager->SetPurchaseItem( m_strPurKey, 1 );
		if ( nRET==1 )
		{
			//	Note : �κ��� ������ �ֱ⸦ ��û�Ѵ�.
			//
			GLMSG::SNET_CHARGED_ITEM2_INVEN NetMsg;
			NetMsg.dwUserID = m_dwUserID;
			NetMsg.nidITEM = m_nidITEM;
			StringCchCopy ( NetMsg.szPurKey, PURKEY_LENGTH+1, m_strPurKey.GetString() );
			NetMsg.dwInvenPos = m_dwInvenPos;

			CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
			pTemp->InsertMsg ( m_ClientSlot, (char*) &NetMsg );
		}
		return nRET;
	}
}

//! PointShop �Ǹ� ��ǰ ������ �����´�.
GetPointShop::GetPointShop(bool bReload, bool bStandAlone)
    : DbAction(SHOP_DB)
    , m_bReload(bReload)
	, m_bStandAlone(bStandAlone)
{
}

GetPointShop::~GetPointShop()
{
}

int GetPointShop::Execute(NetServer* pServer)
{
    CTime CurrentTime(CTime::GetCurrentTime());
    __time64_t ShopUpdateTime = CurrentTime.GetTime();
    std::vector<pointshop::SellItem> SellList;
    m_pDbManager->PointShopList(ShopUpdateTime, SellList, m_bStandAlone);
    
    pointshop::NET_DATA_START_DS MsgStart(ShopUpdateTime);
    pServer->InsertMsg(DBACTION_CLIENT_ID, &MsgStart);
    
    for (size_t i=0; i<SellList.size(); ++i)
    {
        pointshop::NET_DATA_DS MsgData(SellList[i]);
        pServer->InsertMsg(DBACTION_CLIENT_ID, &MsgData);
    }
    
    pointshop::NET_DATA_END_DS MsgEnd(m_bReload);
    pServer->InsertMsg(DBACTION_CLIENT_ID, &MsgEnd);

    return sc::db::DB_OK;
}

//! PointShop �Ǹ� �α�
SaleLogInsert::SaleLogInsert(
    const SNATIVEID& ItemId,
    __int64 Price,
    EM_BILLING_SELL_TYPE emType,
    int UserDbNum,
    int ServerNum,
    int ChaDbNum,
    int ChaLevel)
    : DbAction(SHOP_DB)
    , m_ItemId(ItemId)
    , m_Price(Price)
    , m_emType(emType)
    , m_UserDbNum(UserDbNum)
    , m_ServerNum(ServerNum)
    , m_ChaDbNum(ChaDbNum)
    , m_ChaLevel(ChaLevel)
{
}

SaleLogInsert::~SaleLogInsert()
{
}

int SaleLogInsert::Execute(NetServer* pServer)
{
    return m_pDbManager->ShopSaleLog(
        m_ItemId,
        m_Price,
        m_emType,
        m_UserDbNum,
        m_ServerNum,
        m_ChaDbNum,
        m_ChaLevel);
}

} // namespace db