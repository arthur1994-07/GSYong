#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"

#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../RanLogic/Msg/GLContrlPrivateMarket.h"
#include "../RanLogic/Msg/CharacterMsg.h"
#include "../RanLogic/Msg/ConsignmentSaleMsg.h"
#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Market/MarketDefine.h"
#include "../RanLogic/Market/GLSearchDelay.h"
#include "../RanLogic/Market/GLPrivateMarketItemCache.h"
#include "../RanLogic/GLUseFeatures.h"

#include "../RanLogicClient/PrivateMarketSearchBuy/PrivateMarketClient.h"
#include "PrivateMarketSearchBuy/ConsignmentSaleClient.h"

#include "./Char/GLCharacter.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

HRESULT GLGaeaClient::ReqPrivateMarketItemSearch( private_market::sSaleItemSearchandSortBasic* pBasic )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		return E_FAIL;
	}

	if ( NULL == pBasic || NULL == m_pPrivateMarketSearchDelay )
	{
		return E_FAIL;
	}

	if ( m_pPrivateMarketSearchDelay->IsSearchLimitTime() )
	{
		// �˻� ���� �ð��� �ɷȴ�.
		PrivateMarketError( private_market::eSearchDelay );
		return E_FAIL;
	}

	// ���� ������ Ŭ����
	GetPrivateMarketClient()->ClearSearchItemdate();

	// ĳ�õǾ� �����ʰ� ���� ���ǰ� �ٸ���. ��� �����Ѵ�. ���� �˻��ϴ� ���̴�.
	private_market::GLPrivateMarketItemCache::Instance()->Reset();

	GLMSG::SNETPC_PMARKET_ITEM_SEARCH_BASIC_CF NetMsgCF( *pBasic );
	msgpack::sbuffer SendBuffer;
	msgpack::pack( SendBuffer, NetMsgCF );
	NETSENDTOFIELD( NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_BASIC_CF, SendBuffer );

	return S_OK;
}

HRESULT GLGaeaClient::ReqPrivateMarketItemSearchByName( private_market::sSaleItemSearchandSortName* pName )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		return E_FAIL;
	}

	if ( NULL == pName || NULL == m_pPrivateMarketSearchDelay )
	{
		return E_FAIL;
	}

	if ( pName->strItemName.empty() )
	{
		return E_FAIL;
	}

	if ( m_pPrivateMarketSearchDelay->IsSearchLimitTime() )
	{
		// �˻� ���� �ð��� �ɷȴ�.
		PrivateMarketError( private_market::eSearchDelay );
		return E_FAIL;
	}

	//���� ������ Ŭ����
	GetPrivateMarketClient()->ClearSearchItemdate();

	// ĳ�õǾ� �����ʰ� ���� ���ǰ� �ٸ���. ��� �����Ѵ�. ���� �˻��ϴ� ���̴�.
	private_market::GLPrivateMarketItemCache::Instance()->Reset();

	sc::string::trim( pName->strItemName );

	GLMSG::SNETPC_PMARKET_ITEM_SEARCH_NAME_CF NetMsgCF;
	NetMsgCF.wPage = pName->wPage;
	NetMsgCF.nSortOption = pName->nSortOption;
	NetMsgCF.nSortMethod = pName->nSortMethod;
	NetMsgCF.SetName( pName->strItemName.c_str() );
	NETSENDTOFIELD( &NetMsgCF );
	return S_OK;
}

void GLGaeaClient::MsgPrivateMarketItemNumTotalFC( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC* ) pMsg;

	WORD wItemTotalNum = pPacket->wItemTotalNum;
	GetPrivateMarketClient()->SetTotalItemNum(wItemTotalNum);
	// �˻� ���: ������ �� ����
}

void GLGaeaClient::MsgPrivateMarketItemNumFC( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_ITEM_NUM_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_NUM_HFC* ) pMsg;

	pPacket->wItemNum;
	pPacket->dwChaDbNum;

	// �˻� ���: ������ ����
}

void GLGaeaClient::MsgPrivateMarketItemFC( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_ITEM_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_HFC* ) pMsg;

	if (sizeof(pPacket->sItem.szSellerName) != 0)
		this->GetPrivateMarketClient()->SetSearchResultData(pPacket->sItem);

	// �˻� ���: ���� ������
	
	// ĳ�ÿ� ������ �ִ´�.
	private_market::GLPrivateMarketItemCache::Instance()->InsertItem( pPacket->sItem );
}

void GLGaeaClient::MsgPrivateMarketItemEndFC( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_ITEM_END_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_END_HFC* ) pMsg;

	// �˻� ���: ������ ���� ��
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_UpdateSearchPage );

	// ĳ�ÿ��� ������ ������ �� �ִ�.
	/*std::vector< private_market::sSaleItem > vecItem;
	if ( private_market::eSuccess == private_market::GLPrivateMarketItemCache::Instance()->GetItem( vecItem ) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_UpdateSearchPage );
		return;
	}*/

	// vecItem �� �˻������ ����ִ�. �������� ����Ѵ�.
}

HRESULT GLGaeaClient::ReqPrivateMarketItemBuy( std::vector< private_market::sPurchaseQueue >& vecPurchase )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		return E_FAIL;
	}

	if ( NULL == m_pPrivateMarketPurchseQueue )
	{
		return E_FAIL;
	}

	// ���� ������ ���ΰ�?
	GLCHARLOGIC& rCharLogic = GetCharacterLogic();
	if ( false == private_market::GLPrivateMarketDefine::Instance()->CanPurchase( GetActiveMapID().getGaeaMapID().Id() ) )
	{
		PrivateMarketError( private_market::eNotPurchaseMap );
		return E_FAIL;
	}

	if ( m_pPrivateMarketPurchseQueue->IsPurchasing() )
	{
		return E_FAIL;
	}

	if ( vecPurchase.empty() )
	{
		return E_FAIL;
	}

	m_pPrivateMarketPurchseQueue->Push( vecPurchase );

	return ProcessPrivateMarketBuyQueue();
}

HRESULT GLGaeaClient::ProcessPrivateMarketBuyQueue()
{
	if ( m_pPrivateMarketPurchseQueue->Empty() )
	{
		return E_FAIL;
	}

	private_market::sPurchaseQueue& rPurchase = m_pPrivateMarketPurchseQueue->Front();
	if ( 0 == rPurchase.llPrice )
	{
		return E_FAIL;
	}

	GLCHARLOGIC& rCharLogic = GetCharacterLogic();

	// �� ������ �� �� ����.
	private_market::sSaleItem sItem;
	private_market::GLPrivateMarketItemCache::Instance()->GetItem( rPurchase.sItemGUID, sItem );
	if ( rCharLogic.CharDbNum() == sItem.dwSellerChaDbNum )
	{
		m_pPrivateMarketPurchseQueue->Reset();
		PrivateMarketError( private_market::eCantBuyMyItem );
		return E_FAIL;
	}

    if( sItem.nSellType == private_market::eSellType_ConsignmentSale_RanPoint )
    {
        if ( GetCharacter()->PointShopPoint() < ( ::_abs64( rPurchase.llPrice ) * rPurchase.dwItemTurnNum ) )
        {
            // �� ����
            m_pPrivateMarketPurchseQueue->Reset();
            PrivateMarketError( private_market::eRanPointLack );
            return E_FAIL;
        }
    }
    else
    {
        if ( rCharLogic.GetInvenMoney() < ( ::_abs64( rPurchase.llPrice ) * rPurchase.dwItemTurnNum ) )
        {
            // �� ����
            m_pPrivateMarketPurchseQueue->Reset();
            PrivateMarketError( private_market::eMoneyLack );
            return E_FAIL;
        }
    }


	GLMSG::SNETPC_PMARKET_ITEM_BUY_CF NetMsgCF;
	NetMsgCF.sItemGUID = rPurchase.sItemGUID;
	NetMsgCF.dwItemTurnNum = rPurchase.dwItemTurnNum;
	NetMsgCF.llPrice = rPurchase.llPrice;
    NetMsgCF.nSellType = sItem.nSellType;

	NETSENDTOFIELD( &NetMsgCF );

	m_pPrivateMarketPurchseQueue->Pop();

	return S_OK;
}

void GLGaeaClient::MsgPrivateMarketCommonFB( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_COMMON_FB* pPacket = ( GLMSG::SNETPC_PMARKET_COMMON_FB* ) pMsg;
		
	PrivateMarketError( pPacket->nFeedback, &pPacket->sItemGUID );

	ProcessPrivateMarketBuyQueue();
}

void GLGaeaClient::PrivateMarketError( int nFeedback, UUID* pItemGUID /*= NULL*/ )
{
	bool bUpdate = false;

	switch ( nFeedback )
	{
	case private_market::eSuccess:
		{
			// �ŷ��� ������ ��쿡�� �� �޽����� �޴´�.
			// �������� ���� �Ͽ����ϴ�.
			if ( pItemGUID )
			{
				UUID ItemGUID = *pItemGUID;
				// ����Ʈ���� �ش� �������� �����Ѵ�.
				PrivateMarketItemStateText(ID2GAMEINTEXT("MAKET_SEARCH_BUY_SUCCESS",0), ItemGUID );
				GetPrivateMarketClient()->DelWhishPageData(ItemGUID);
				
				bUpdate = true;
			}
		}
		break;

	case private_market::eNotExistSaleItem:
		{
			// �����Ϸ��� �������� ����.
			// ����Ʈ���� �ش� �������� �����Ѵ�.
			// �����Ϸ��� �������� �����ϴ�.
			if ( pItemGUID )
			{
				UUID ItemGUID = *pItemGUID;
				// ����Ʈ���� �ش� �������� �����Ѵ�.
				PrivateMarketItemStateText(ID2GAMEINTEXT("MAKET_SEARCH_BUY_NO_ITEM",0), ItemGUID );
				GetPrivateMarketClient()->DelWhishPageData(ItemGUID);

				bUpdate = true;
			}
		}
		break;

	case private_market::eItemLack:
		{
			// �����Ϸ��� �ϴ� ������ŭ �������� ����.(���� ����)
			// �����Ϸ��� �������� ������ �����մϴ�.

			if ( pItemGUID )
			{
				UUID ItemGUID = *pItemGUID;
				// ����Ʈ���� �ش� �������� �����Ѵ�.
				PrivateMarketItemStateText(ID2GAMEINTEXT("MAKET_SEARCH_BUY_NOT_ENOUGH_COUNT",0), ItemGUID );
			}
		}
		break;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_Error,
		"-n, -b", nFeedback, bUpdate );
}

void GLGaeaClient::PrivateMarketItemStateText( const char* szItemStateText, UUID pItemGUID )
{

	std::vector<private_market::sSaleItem*> vecSearchPageData = GetPrivateMarketClient()->GetSearchResultData();

	BOOST_FOREACH( private_market::sSaleItem* SearchOption, vecSearchPageData )
	{
		if( SearchOption->sItemGUID == pItemGUID )
		{
			SITEMCUSTOM& sItemCustom = SearchOption->sItemcustom;
			PrintMsgText( NS_UITEXTCOLOR::RED, sc::string::format( "%1% %2%", sItemCustom.GETNAME().c_str(), szItemStateText) );
		}
	}
}

HRESULT GLGaeaClient::ReqPrivateMarketLog()
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		return E_FAIL;
	}

	if ( m_pPrivateMarketLogDelay && m_pPrivateMarketLogDelay->IsSearchLimitTime() )
	{
		// �˻� ���� �ð��� �ɷȴ�.
		//PrivateMarketError( private_market::eSearchDelay );
		return E_FAIL;
	}

	GLMSG::SNETPC_PMARKET_LOG_CF NetMsgCF;
	NETSENDTOFIELD( &NetMsgCF );

	return S_OK;
}

void GLGaeaClient::MsgPrivateMarketLogNumFC( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_LOG_NUM_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_LOG_NUM_HFC* ) pMsg;
	GetPrivateMarketClient()->ClearLogPage();
	GetPrivateMarketClient()->SetTotalLogCount(pPacket->wLogNum);
	// �ŷ�����: ����
}

void GLGaeaClient::MsgPrivateMarketLogFC( NET_MSG_GENERIC* pMsg )
{
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) pMsg;
	GLMSG::SNETPC_PMARKET_LOG_HFC RecvData;
	if ( false == msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
	{
		return;
	}
	GetPrivateMarketClient()->SetLogPageData(RecvData.sLog);

	// �ŷ�����: ���� �ŷ�����
}

void GLGaeaClient::MsgPrivateMarketLogEndFC( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_LOG_END_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_LOG_END_HFC* ) pMsg;
	GetPrivateMarketClient()->SetLogEnd(TRUE);
	// �ŷ�����: �ŷ����� ���� ��

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_UpdateLog );
}

void GLGaeaClient::PrivateMarketDelayLoad()
{
	if ( m_pPrivateMarketSearchDelay )
	{
		m_pPrivateMarketSearchDelay->SetDelay( private_market::GLPrivateMarketDefine::Instance()->GetSearchDelayMillisecond() );
	}

	if ( m_pPrivateMarketLogDelay )
	{
		m_pPrivateMarketLogDelay->SetDelay( private_market::GLPrivateMarketDefine::Instance()->GetLogDelayMillisecond() );
	}
}

DWORD GLGaeaClient::GetRemainSearchLimitTime()
{
	return m_pPrivateMarketSearchDelay->GetRemainSearchLimitTimeClient();
}

void GLGaeaClient::MsgPrivateMarketStateFC( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_PMARKET_STATE_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_STATE_HFC* ) pMsg;

	PrintConsoleText( "Private Market current setting start." );
	PrintConsoleText( sc::string::format( "search delay = %f", pPacket->fSearchDelay ) );
	PrintConsoleText( sc::string::format( "log delay = %f", pPacket->fLogDelay ) );
	PrintConsoleText( sc::string::format( "search item number per page = %d", pPacket->wSearchItemNumberPerPage ) );
	PrintConsoleText( sc::string::format( "log number per page = %d", pPacket->wLogNumberPerPage ) );
	PrintConsoleText( sc::string::format( "log number max per character = %d", pPacket->wLogNumberMaxPerCharacter ) );
	PrintConsoleText( sc::string::format( "search option = %d", pPacket->nSearchOption ) );

	if ( pPacket->bCheckChannel )
	{
		PrintConsoleText( "channel check on." );
	}
	else
	{
		PrintConsoleText( "channel check off." );
	}

	if ( pPacket->bCheckSchool )
	{
		PrintConsoleText( "school check on." );
	}
	else
	{
		PrintConsoleText( "school check off." );
	}

	if ( pPacket->bSaleCommission )
	{
		PrintConsoleText( "commission on." );
	}
	else
	{
		PrintConsoleText( "commission off." );
	}

	if ( pPacket->bAllPage )
	{
		PrintConsoleText( "search all page on." );
	}
	else
	{
		PrintConsoleText( "search all page off." );
	}

	PrintConsoleText( "Private Market current setting end." );
}


void GLGaeaClient::MsgConSignmentSaleListAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_LIST_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;


    m_pConsignmentSale->AckListUpdate( RecvData.vecSaleItems, RecvData.m_bIsLast );
}

void GLGaeaClient::MsgConSignmentSaleRegistAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_CONSIGNMENT_ADD_ITEM_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

//    if( RecvData.bAddSucess )
//		m_pConsignmentSale->ReqListUpdate();
//        m_pConsignmentSale->AckRegistItem( RecvData.SaleItem );
//     else
//         int i =0;    
}

void GLGaeaClient::MsgConSignmentSaleSendBackAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

//     if( RecvData.bIsHold )
//     {
//         //PrintMsgText( NS_UITEXTCOLOR::DISABLE, "�ٸ� ĳ���Ͱ� �������� ���� ���Դϴ�." );
//     }

	if ( RecvData.nResult==private_market::eNotExistSaleItem 
		|| RecvData.nResult==private_market::eSuccess )
    {
        //�޽��� ���
        //PrintMsgText( NS_UITEXTCOLOR::ENABLE, "�ݼ� ����" );
        // �׸��� ��� ������Ʈ
        m_pConsignmentSale->ReqListUpdate();
        //m_pConsignmentSale->AckUnregist( RecvData.strGUID );
    }
    else
    {
        //PrintMsgText( NS_UITEXTCOLOR::DISABLE, "�ݼۿ� �����߽��ϴ�. ����Ŀ� �ٽ� �õ��ϼ���." );
    }

}

void GLGaeaClient::MsgConSignmentSalePurchaseAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    m_pConsignmentSale->AckPurchase( RecvData.strGUID, RecvData.nPurchaseCount ); 

}

void GLGaeaClient::MsgConSignmentSaleSlotExpirationDate( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    if( GetCharacterData().GetConsignmentSaleSlotExpansionDate() == RecvData.ExpirationDate )
        return;

    GetCharacterData().SetConsignmentSaleSlotExpansionDate( RecvData.ExpirationDate );
    m_pConsignmentSale->UiRefresh();

	// �������� ��� �������� �޽����� ����Ѵ�.
	if ( RecvData.bItemUsed )
		PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("RAN_POINT_SLOT_EXPIRATION") );//ID2GAMEINTEXT("RAN_POINT_SLOT_EXPIRATION") );

}
