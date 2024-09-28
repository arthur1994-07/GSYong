#include "pch.h"
#include "ConsignmentSaleClient.h"


#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/String/MinGuid.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../../RanLogic/Msg/ConsignmentSaleMsg.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

ConsignmentSaleClient::ConsignmentSaleClient( GLGaeaClient* pGaeaClient )
    : m_pGaeaClient(pGaeaClient)
{
    m_vecSaleItems.clear();
}

ConsignmentSaleClient::~ConsignmentSaleClient()
{

}

void ConsignmentSaleClient::Reset()
{
    m_emListState = emListEmpty;
    m_vecSaleItems.clear();
    m_tLastUpdatetime = CTime::GetCurrentTime().GetTime();
} 

void ConsignmentSaleClient::ReqRegistItem( const SINVENITEM& sInvenItem, LONGLONG llSellPrice, WORD wTurnNum, int nTimeTableIndex, private_market::eSellType SellType )
{   
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();

	if( private_market::GLPrivateMarketDefine::Instance()->GoldTrade() == false && SellType == private_market::eSellType_ConsignmentSale_Gold )
		return;

	if ( llSellPrice <= 0 || wTurnNum <= 0 || W_LIMITED_POINT_TRADE_COUNT < wTurnNum )
		return;

	if ( llSellPrice < private_market::GLPrivateMarketDefine::Instance()->RanPointSaleMinPrice() ||
		LL_LIMITED_A_UNIT_PRICE < llSellPrice )
		return;

    // ������ Ȯ��
    const SITEM* pItemData = GLogicData::GetInstance().GetItem( sInvenItem.sItemCustom.GetNativeID() );
    if ( !pItemData )
        return;

    std::vector<private_market::GameMoneyItemFee> vecMoneyPeriodFee;
    vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

    bool bIsGameMoney = false;
    for( unsigned int i=0; i<vecMoneyPeriodFee.size(); i++ )
    {
        const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[i];

        if( value.sNativeID == sInvenItem.sItemCustom.GetNativeID() )
        {
            bIsGameMoney = true;
            break;
        }
    }

    if( bIsGameMoney )
    {
        LONGLONG llMoney = pChar->GetInvenMoney();

        if( llMoney < pItemData->sBasicOp.dwSellPrice )
            return;
    }
    else
    {
        SINVENITEM* sInvenItemNow = m_pGaeaClient->GetCharacter()->m_cInventory.FindPosItem( sInvenItem.wPosX, sInvenItem.wPosY );

        if(  sInvenItemNow == NULL || sInvenItemNow->sItemCustom != sInvenItem.sItemCustom )
        {
            return;
        }
    }

    // ������Ʈ �ŷ��� �Ҽ� �ִ� �����ΰ�?
    // �����ᰡ 0�̶�� �ŷ��Ҵ� �����̴�.
    //WORD wFee = GetFee( nTimeTableIndex, pInvenItem.sItemCustom );
    DWORD dwFee = private_market::GLPrivateMarketDefine::Instance()->CalcFee( nTimeTableIndex, sInvenItem.sItemCustom );

    if( dwFee < 0 )
    {
        // ���� �ŷ��� �Ұ����� �������Դϴ�.
        return;
    }

	if ( sInvenItem.sItemCustom.IsGM_GENITEM() )
	{
		// #3989 GmCharEdit ���� �ŷ��Ұ��� �־��� �������� Ŭ��â�� ���� �� �ִ� ���� ����, ��������Ʈ üũ���� �ʴ�
		// �����۱�ȯ, ����Ʈ�ŷ����� �˻��ϵ��� �߰�
		return;
	}

    // �ŷ��Ұ����ϸ�
    //if( pItemData->CanExchange() == false )
	if( ( sInvenItem.sItemCustom.IsExchange() != NULL ) == false )
    {
        // �� ����Ʈ �ŷ��� �ŷ� ����
        if( SellType != private_market::eSellType_ConsignmentSale_RanPoint )
            return;

        // �ŷ� �����ϴٸ� �ŷ� ����Ƚ���� �ʰ��ϴ��� �˻�
        if( pItemData->sBasicOp.RanPointTradeCount() <= sInvenItem.sItemCustom.wRanPointTradeCount )
		{
            return;
        }
    }
    

    // ����� ���� ä������?
    //WORD wListLimit = private_market::GLPrivateMarketDefine::Instance()->SellResgisterBasicNum();
    
    // �̿���� ������̶��   
    //wListMaxCount += private_market::GLPrivateMarketDefine::Instance()->SellResgisterExpandAddNum();

    //if( wListLimit <= m_vecSaleItems.size() )
    //{
        // ���� ���̻� ��ǰ�� ��� �� �� �����ϴ�.
        //return;
    //}

    // �����ᰡ 0�� �ƴ϶�� �ŷ��� �����ᰡ �ִ���?
    __int64 nPoint = pChar->PointShopPoint();
    if( nPoint <= dwFee )
    {
        // ���� ��� �����ᰡ �����մϴ�.
        //return;
    }

    //---------------------------------------------------------------------------NET
    struct GLMSG::NET_CONSIGNMENT_ADD_ITEM_REQ SendData;
    {
        SendData.m_dwChaDbNum = pChar->CharDbNum();
        SendData.m_nInvenPosX = sInvenItem.wPosX;
        SendData.m_nInvenPosY = sInvenItem.wPosY;
        SendData.m_NativeID = sInvenItem.sItemCustom.GetNativeID();

        SendData.m_wTurnNum =  wTurnNum;
        SendData.m_wPeriodIndex = nTimeTableIndex;
        SendData.m_llPrice = llSellPrice;
        SendData.m_bySellType = static_cast<BYTE>( SellType );
    };


    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaClient->NETSENDTOFIELD( NET_MSG_CONSIGNMENT_ADD_ITEM_REQ, PackBuffer );
    
}

void ConsignmentSaleClient::AckRegistItem( const private_market::ConsignmentSaleItem& SaleItem )
{
    if( m_emListState == emListEmpty )
        ReqListUpdate();
    else
    {
        m_vecSaleItems.push_back( SaleItem );

		//!! SF_TODO
        //m_pInterface->PrivattMarketWindowRanPointPageRefresh();
    }


}

void ConsignmentSaleClient::ReqUnregist( sc::MGUID& Guid )
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
   
    std::string strGuid = sc::string::uuidToString(Guid);
    sc::string::upperString(strGuid);

    //---------------------------------------------------------------------------NET
    struct GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ SendData;
    {
        SendData.strGUID = strGuid; 
        SendData.dwChaDbNum = pChar->CharDbNum();
    };


    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaClient->NETSENDTOFIELD( NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ, PackBuffer );
}

void ConsignmentSaleClient::AckUnregist( std::string& strGuid )
{
    // ��Ŷ���� ������Ʈ ��Ű��.
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_LIST_REQ SendData;
    {
        SendData.m_dwChaDbNum = m_pGaeaClient->GetCharacter()->CharDbNum();
    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaClient->NETSENDTOFIELD( NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_REQ, PackBuffer );
}

void ConsignmentSaleClient::ReqListUpdate()
{
    __time64_t tCurrentTime = CTime::GetCurrentTime().GetTime();
    
    // ����� ���� ������Ʈ ��Ű�� �ʴ´�.
    if( 10 < m_tLastUpdatetime - m_tLastUpdatetime )
        return;

//     if( m_emListState == emListUpdating )
//         return;

        
    m_vecSaleItems.clear();
	UiRefresh();
	//!! SF_TODO
    //m_pInterface->PrivattMarketWindowRanPointPageRefresh();
    m_emListState = emListUpdating;

    
    // ��Ŷ���� ������Ʈ ��Ű��.
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_LIST_REQ SendData;
    {
        SendData.m_dwChaDbNum = m_pGaeaClient->GetCharacter()->CharDbNum();
    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaClient->NETSENDTOFIELD( NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_REQ, PackBuffer );

    m_tLastUpdatetime = tCurrentTime;
}


void ConsignmentSaleClient::AckListUpdate( const private_market::SALE_ITEMS_VECTOR& SaleItems, bool bLast )
{
    BOOST_FOREACH( const private_market::ConsignmentSaleItem& value, SaleItems )
        m_vecSaleItems.push_back( value );

    if( bLast )
    {
        m_emListState = emListUpdateFinish;

        UiRefresh();
    }
}

void ConsignmentSaleClient::UiRefresh()
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_UpdatePointTrade );
}

void ConsignmentSaleClient::AckPurchase( std::string strGuid, int nPurchaseCount )
{

    private_market::SALE_ITEMS_VECTOR::iterator it;
    it = m_vecSaleItems.begin();

    while (it != m_vecSaleItems.end())
    {
        if (strcmp(it->GetStrGUID().c_str(), strGuid.c_str()) == 0)
        {
            it->wTurnNum -= nPurchaseCount;
            UiRefresh();
        }

        it++;

    }
    
}


const private_market::SALE_ITEMS_VECTOR& ConsignmentSaleClient::GetList()
{
    //m_vecSaleItems.clear();
    // NET_CONSIGNMENT_ADD_ITEM_ACK SendData ���� ä���

    return m_vecSaleItems;
}

const private_market::ConsignmentSaleItem* ConsignmentSaleClient::GetConsignmentSaleItem( DWORD dwIndex )
{
	if ( dwIndex >= m_vecSaleItems.size() )
		return NULL;

	return &m_vecSaleItems[dwIndex];

}