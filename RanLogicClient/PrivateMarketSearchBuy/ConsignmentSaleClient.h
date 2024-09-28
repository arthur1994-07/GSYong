#pragma once

#include "../../RanLogic/Market/MarketDefine.h"
#include "../../RanLogic/Market/GLConsignmentSaleDefine.h"

#include "PrivateMarketClientDefine.h"

class GLGaeaClient;

struct SINVENITEM;

class ConsignmentSaleClient
{
public:
    enum emListState
    {
        emListEmpty,
        emListUpdating,
        emListUpdateFinish,
    };

    
    ConsignmentSaleClient(GLGaeaClient* pGaeaClient);
    ~ConsignmentSaleClient();

    void Reset();

    void ReqRegistItem( const SINVENITEM& sInvenItem, LONGLONG llSellPrice, WORD wTurnNum, int nTimeTableIndex, private_market::eSellType SellType );
    void AckRegistItem( const private_market::ConsignmentSaleItem& SaleItem );

    void ReqUnregist( sc::MGUID& Guid );
    void AckUnregist( std::string& strGuid );

    // 목록 업데이트
    void ReqListUpdate(); 
    void AckListUpdate( const private_market::SALE_ITEMS_VECTOR& SaleItems, bool bLast );

    void UiRefresh();

    // 물품 구매를 하였다. 
    void AckPurchase( std::string strGuid, int nPurchaseCount );

    const private_market::SALE_ITEMS_VECTOR& GetList();

	const private_market::ConsignmentSaleItem* GetConsignmentSaleItem( DWORD dwIndex );

    // 수수료 갖고오기.
    //int GetFee( int nTimeTableIndex, const SITEMCUSTOM& sItemCustom );

private:
    private_market::SALE_ITEMS_VECTOR   m_vecSaleItems;
    emListState         m_emListState;
    GLGaeaClient*       m_pGaeaClient;

    __time64_t          m_tLastUpdatetime;
};
