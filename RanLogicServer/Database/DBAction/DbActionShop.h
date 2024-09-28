#ifndef _DB_ACTION_SHOP_H_
#define _DB_ACTION_SHOP_H_

#pragma once

#include "../../../RanLogic/Character/GLCharData.h"
#include "DbAction.h"

namespace db
{

//! Shop 에서 구입한 아이템을 가져온다. 
class GetPurchaseItem : public db::DbAction
{
public:
    GetPurchaseItem(const CString& strUID, DWORD dwCharID, bool bFirstJoin); 
    virtual ~GetPurchaseItem() {};
    virtual int Execute(NetServer* pServer) override;

protected:
	DWORD	m_dwCharID;
    CString m_strUID;
    bool    m_bFirstJoin;
    std::vector<SHOPPURCHASE> m_vItem;
};

//! 실제로 가져갈 수 있는 상품인지 확인한다.
//! /param nFlag (0 : 구매, 1 : 구매완료, 2 : 구매취소신청, 3 : 구매취소처리)
//! /output 1 : 가져갈수 있음, 이외의값 : 가져갈수 없음 
class SetPurchaseItem : public db::DbAction
{
public:
    SetPurchaseItem(const CString& strPurKey, int nFlag=1);
    virtual ~SetPurchaseItem() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    CString m_strPurKey;
    int m_nFlag;
};

//! 구입한 아이템을 인벤에 넣기에 실패했을 경우 flag 리셋
class PurchaseItem_Restore : public db::DbAction
{
public:
    PurchaseItem_Restore(const CString& strPurKey);
    virtual ~PurchaseItem_Restore() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    CString m_strPurKey;
};

//! 구입한 아이템을 인벤토리로 가져옵니다.
class PurchaseItem_Get : public db::DbAction
{
public:
    PurchaseItem_Get(
        DWORD dwClient,
        DWORD dwUserID,
        const CString& strPurKey,
        SNATIVEID nidITEM,
        DWORD dwInvenPos);
    virtual ~PurchaseItem_Get() {};
    virtual int Execute(NetServer* pServer) override;

protected:
	DWORD		m_ClientSlot;
	DWORD		m_dwUserID;
    CString		m_strPurKey;
	SNATIVEID	m_nidITEM;
	DWORD		m_dwInvenPos;
};

//! PointShop 판매 물품 정보를 가져온다.
class GetPointShop : public DbAction
{
public:
    GetPointShop(bool bReload, bool bStandAlone);
    virtual ~GetPointShop();
    virtual int Execute(NetServer* pServer) override;

private:
    bool m_bReload;
	bool m_bStandAlone;
};

//! PointShop 판매 로그
class SaleLogInsert : public DbAction
{
public:
    SaleLogInsert(
        const SNATIVEID& ItemId,
        __int64 Price,
        EM_BILLING_SELL_TYPE emType,
        int UserDbNum,
        int ServerNum,
        int ChaDbNum,
        int ChaLevel);
    virtual ~SaleLogInsert();
    virtual int Execute(NetServer* pServer) override;

private:
    SNATIVEID m_ItemId;
    __int64 m_Price;
    EM_BILLING_SELL_TYPE m_emType;
    int m_UserDbNum;
    int m_ServerNum;
    int m_ChaDbNum;
    int m_ChaLevel;
};

} // namespace db
#endif // _DB_ACTION_SHOP_H_