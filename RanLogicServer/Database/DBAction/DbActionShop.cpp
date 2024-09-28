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

//! Shop 에서 구입한 아이템을 가져온다.
//! 가져온 아이템은 실제로 가져 갈 수 있는 아이템이 아니다.
//! 아이템을 가져갈때는 실제로 가져갈 수 있는지 다시 확인해야 한다.
//! 웹을 통한 구매와 취소 및 게임내 캐릭터의 동기화 문제.
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

//! 실제로 가져갈 수 있는 상품인지 확인한다.
//! 입력값
//! nFlag (0 : 구매, 1 : 구매완료, 2 : 구매취소신청, 3 : 구매취소처리)
//! 출력값
//! 1 : 가져갈수 있음
//! 이외의값 : 가져갈수 없음
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

//! 구입한 아이템을 인벤에 넣기에 실패했을 경우 flag 리셋
PurchaseItem_Restore::PurchaseItem_Restore(const CString& strPurKey)
    : DbAction(SHOP_DB)
    , m_strPurKey(strPurKey)
{    
}

int PurchaseItem_Restore::Execute(NetServer* pServer)
{
    return m_pDbManager->SetPurchaseItem(m_strPurKey, 0);
}

//! 구입한 아이템을 인벤으로 가져옵니다.
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
			//	Note : 인벤에 아이템 넣기를 요청한다.
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

//! PointShop 판매 물품 정보를 가져온다.
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

//! PointShop 판매 로그
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