#pragma once

#include "../../../SigmaCore/String/MinGuid.h"
#include "../../../SigmaCore/Util/DateTime.h"

#include "../DbDefineLog.h"
#include "DbAction.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogic/MarKet/GLConsignmentSaleDefine.h"

namespace db
{
// ������ ���  
class ConsignmentSaleCheck : public db::DbAction
{
public:
    ConsignmentSaleCheck( private_market::ConsignmentSaleItem& sSaleItem, float fCommissionRate, DWORD dwFee )
        : m_SaleItem( sSaleItem )
        , m_fCommissionRate( fCommissionRate )
		, m_dwFee(dwFee) {};

    virtual ~ConsignmentSaleCheck() {};
    virtual int Execute( NetServer* pServer ) override;

private:
    private_market::ConsignmentSaleItem m_SaleItem;
    float   m_fCommissionRate;

	DWORD	m_dwFee;		// ��ǰ ��Ϻ�	( �α� ��� �� )
};

//������ �����ȯ
class ConsignmentSaleSendBack : public db::DbAction
{
public:
    ConsignmentSaleSendBack(  sc::MGUID& Guid, const SPOSTINFO& PostInfo, float fCommissionRate )
        : m_Guid( Guid )
        , m_PostInfo( PostInfo )
        , m_fCommissionRate(fCommissionRate){};              
        virtual ~ConsignmentSaleSendBack() {};
    virtual int Execute(NetServer* pServer) override;

private:
    sc::MGUID   m_Guid;
    SPOSTINFO	m_PostInfo;
    float       m_fCommissionRate;
};


// ������ ����
// ���� ���� �߼��ؾ��Ѵ�.
class ConsignmentSalePurchase : public db::DbAction
{
public:
	ConsignmentSalePurchase( sc::MGUID& Guid, int nPurchaseNumber, const SPOSTINFO& PostInfo, GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_FB& Msg )
        : m_Guid( Guid )
        , m_nPurchaseNumber( nPurchaseNumber )
        , m_PostInfo( PostInfo )
		, m_MsgFB( Msg ) {};
    virtual ~ConsignmentSalePurchase() {};
    virtual int Execute( NetServer* pServer ) override;

private:
    sc::MGUID   m_Guid;
    int         m_nPurchaseNumber;
    SPOSTINFO	m_PostInfo;
	GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_FB m_MsgFB;
};

/*
class ConsignmentSalePurchaseForCacheServer : public db::DbAction
{
public:
	ConsignmentSalePurchaseForCacheServer( sc::MGUID& Guid, int nPurchaseNumber, NET_CONSIGNMENT_SALE_PAY_MONEY_ACK& msg )
		: m_Guid( Guid )
		, m_nPurchaseNumber( nPurchaseNumber )
		, m_BasicMsg( msg ) {};
	virtual ~ConsignmentSalePurchaseForCacheServer() {};
	virtual int Execute( NetServer* pServer ) override;

private:
	sc::MGUID   m_Guid;
	int         m_nPurchaseNumber;
	NET_CONSIGNMENT_SALE_PAY_MONEY_ACK m_BasicMsg;

};

class ConsignmentSaleSendPostAndLog : public db::DbAction
{
public:
	ConsignmentSaleSendPostAndLog( sc::MGUID& Guid, int nPurchaseNumber, const SPOSTINFO& PostInfo, NET_CONSIGNMENT_SALE_SEND_POST_REQ& msg )
		: m_Guid( Guid )
		, m_nPurchaseNumber( nPurchaseNumber )
		, m_PostInfo( PostInfo )
		, m_BasicMsg( msg ) {};
	virtual ~ConsignmentSaleSendPostAndLog() {};
	virtual int Execute( NetServer* pServer ) override;

private:
	sc::MGUID   m_Guid;
	int         m_nPurchaseNumber;
	SPOSTINFO	m_PostInfo;
	NET_CONSIGNMENT_SALE_SEND_POST_REQ m_BasicMsg;
};
*/

//��� �ޱ�(������)
class ConsignmentSaleGetList : public db::DbAction
{
public:
    ConsignmentSaleGetList( const DWORD& dwChaDbNum, int& nListCount )  // ��� �ޱ�( ������ )_�ʵ弭��
        : m_dwChaDbNum( dwChaDbNum )
        , m_nListCount(nListCount)
    {
        m_nListCount = 0;
    }           

    virtual ~ConsignmentSaleGetList() {};
    virtual int Execute(NetServer* pServer) override;

private:
    DWORD m_dwChaDbNum;
    int& m_nListCount;
};

//��� �ޱ�(������)
class ConsignmentSaleGetListChche : public db::DbAction
{
public:
    ConsignmentSaleGetListChche( float fCommissionRate )                    // ��� �ޱ�( ������ )_ĳ������
        : m_dwChaDbNum( 0 )
        , m_fCommissionRate( fCommissionRate ){}           
    virtual ~ConsignmentSaleGetListChche() {};
    virtual int Execute(NetServer* pServer) override;

private:
    DWORD   m_dwChaDbNum;
    float   m_fCommissionRate;
};

// ConfignmentSaleSlotExpansionDateSet
class ConsignmentSaleSlotExpansionDateUpdate : public db::DbAction
{
public:
    ConsignmentSaleSlotExpansionDateUpdate( DWORD dwChaDbNum, __time64_t tExpansionDate )
        : m_dwChaDbNum( dwChaDbNum )
        , m_tExpansionDate( tExpansionDate ){}           
    virtual ~ConsignmentSaleSlotExpansionDateUpdate() {};
    virtual int Execute(NetServer* pServer) override;

private:
    DWORD   m_dwChaDbNum;
    __time64_t m_tExpansionDate;
};

//��� �ޱ�(������)
class ConsignmentSaleLogInsert : public db::DbAction
{
public:
	ConsignmentSaleLogInsert( const private_market::ConsignmentSaleItem& SaleItem, LONGLONG llFee, private_market::LOG_STATE logType )                    // ��� �ޱ�( ������ )_ĳ������
		: m_SaleItem( SaleItem )
		, m_llFee( llFee )
		, m_logType( logType )
	{
		if ( !m_SaleItem.strGuid.empty() )
		{
			char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
			StringCchCopy(szGuid, sc::string::UUID_LENGTH_BRACES, m_SaleItem.strGuid.c_str());
			sc::string::stringToUUID( szGuid, m_guid );
		}
	}

	ConsignmentSaleLogInsert( const sc::MGUID& guid, const private_market::ConsignmentSaleItem& SaleItem, LONGLONG llFee, private_market::LOG_STATE logType )
		: m_guid( guid )
		, m_SaleItem( SaleItem )
		, m_llFee( llFee )
		, m_logType( logType ){}
	virtual ~ConsignmentSaleLogInsert() {};
	virtual int Execute(NetServer* pServer) override;

private:
	private_market::ConsignmentSaleItem m_SaleItem;
	private_market::LOG_STATE m_logType;
	LONGLONG m_llFee;
	sc::MGUID m_guid;
};


} // namespace db
// #endif //! _DB_ACTION_LOG_H_
