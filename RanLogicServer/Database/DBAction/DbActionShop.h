#ifndef _DB_ACTION_SHOP_H_
#define _DB_ACTION_SHOP_H_

#pragma once

#include "../../../RanLogic/Character/GLCharData.h"
#include "DbAction.h"

namespace db
{

//! Shop ���� ������ �������� �����´�. 
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

//! ������ ������ �� �ִ� ��ǰ���� Ȯ���Ѵ�.
//! /param nFlag (0 : ����, 1 : ���ſϷ�, 2 : ������ҽ�û, 3 : �������ó��)
//! /output 1 : �������� ����, �̿��ǰ� : �������� ���� 
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

//! ������ �������� �κ��� �ֱ⿡ �������� ��� flag ����
class PurchaseItem_Restore : public db::DbAction
{
public:
    PurchaseItem_Restore(const CString& strPurKey);
    virtual ~PurchaseItem_Restore() {};
    virtual int Execute(NetServer* pServer) override;

protected:
    CString m_strPurKey;
};

//! ������ �������� �κ��丮�� �����ɴϴ�.
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

//! PointShop �Ǹ� ��ǰ ������ �����´�.
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

//! PointShop �Ǹ� �α�
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