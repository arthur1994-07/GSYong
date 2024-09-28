#include "../pch.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Msg/ConsignmentSaleMsg.h"
#include "../../RanLogic/Market/MarketDefine.h"

#include "../PrivateMarket/PrivateMarketField.h"

#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaServer::PrivateMarketOpen(DWORD ChaDbNum)
{
    m_pPrivateMarketMan->OpenMarket(ChaDbNum);
}

void GLGaeaServer::PrivateMarketClose(DWORD ChaDbNum)
{
    m_pPrivateMarketMan->CloseMarket(ChaDbNum);
}

void GLGaeaServer::PrivateMarketAddItem(DWORD ChaDbNum, const SSEARCHITEMDATA& Data)
{
    m_pPrivateMarketMan->AddItem(ChaDbNum, Data);
}

void GLGaeaServer::PrivateMarketDelItem(DWORD ChaDbNum)
{
    m_pPrivateMarketMan->DelItem(ChaDbNum);
}

BOOL GLGaeaServer::MsgPMarketTitle(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwGaeaID == GAEAID_NULL)
        return FALSE;
    
    GLChar* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;
    pChar->MsgPMarketTitle(nmg);
    return TRUE;    
}

BOOL GLGaeaServer::MsgPMarketReqItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgPMarketReqItem( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgPMarketDisItem(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgPMarketDisItem( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgPMarketOpen(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgPMarketOpen( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgPMarketClose(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgPMarketClose( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgPMarketItemInfo(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;
    pChar->MsgPMarketItemInfo( nmg );
    return TRUE;    
}

BOOL GLGaeaServer::MsgPMarketBuy(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if ( dwGaeaID == GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetChar ( dwGaeaID );
    if ( !pChar )
        return FALSE;

	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		// ���λ��� �˻��ý����� ������� �ʴ´ٸ� �����������
		pChar->MsgPMarketBuy( nmg );
		return TRUE;
	}

	// ���λ��� �˻��ý��� ������ �ٷ� �����ϸ� �ȵȴ�.
	// cache ���� Ȧ��� ���������� Ȯ���� �� ó���Ѵ�.
	GLMSG::SNETPC_PMARKET_BUY* pNetMsg = ( GLMSG::SNETPC_PMARKET_BUY* ) nmg;
	GLChar* pTarget = GetChar( pNetMsg->dwGaeaID );
	if ( NULL == pTarget )
	{
		GLMSG::SNETPC_PMARKET_BUY_FB NetMsgFB;
		NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
		SENDTOCLIENT( dwClientID, &NetMsgFB );
		return FALSE;
	}

	SSALEITEM* pSALEITEM = pTarget->GetPrivateMarketItem( pNetMsg->sSALEPOS );
	if ( NULL == pSALEITEM )
	{
		GLMSG::SNETPC_PMARKET_BUY_FB NetMsgFB;
		NetMsgFB.emFB = EMPMARKET_BUY_FB_FAIL;
		SENDTOCLIENT( dwClientID, &NetMsgFB );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_IS_HOLD_FH NetMsgFH;
	NetMsgFH.dwGaeaID = pNetMsg->dwGaeaID;
	NetMsgFH.sSALEPOS = pNetMsg->sSALEPOS;
	NetMsgFH.dwNum = pNetMsg->dwNum;
	NetMsgFH.dwReqChaDbNum = pChar->CharDbNum();
	NetMsgFH.sItemGUID = pSALEITEM->sGUID;
	NetMsgFH.SetName( pChar->GetName() );
	NetMsgFH.nServerGroup = GetServerGroup();
	SENDTOINTEGRATIONCACHE( &NetMsgFH );

    return TRUE;    
}


void GLGaeaServer::InsertSearchShop( DWORD dwGaeaID )
{
	m_listSearchShop.push_back( dwGaeaID ); 
}

void GLGaeaServer::EraseSearchShop( DWORD dwGaeaID )
{
	LISTSEARCHSHOP_ITER iter = m_listSearchShop.begin();
	for( ; iter != m_listSearchShop.end(); ++iter )
	{
		if( *iter == dwGaeaID )
		{
			m_listSearchShop.erase(iter);
			return;
		}
	}
}

bool GLGaeaServer::FindSearchShop( DWORD dwGaeaID )
{
	LISTSEARCHSHOP_ITER iter = m_listSearchShop.begin();
	for( ; iter != m_listSearchShop.end(); ++iter )
	{
		if( *iter == dwGaeaID )
		{
			return TRUE;
		}
	}
	return FALSE;
}


BOOL GLGaeaServer::ReqSearchShopItem ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( true == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		// ���λ��� �˻��ý����� ������̶�� ���� ���λ��� �˻� �ý��� ������� ����
		return TRUE;
	}

	NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::SNETPC_PMARKET_SEARCH_ITEM Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return FALSE;

	LISTSEARCHSHOP_ITER iter	 = m_listSearchShop.begin();
	LISTSEARCHSHOP_ITER iter_end = m_listSearchShop.end();
	LISTSEARCHSHOP_ITER iter_cur;

	GLMSG::SNETPC_PMARKET_SEARCH_ITEM_RESULT NetResultMsg;

	GLChar* pMyChar = GetChar(dwGaeaID);
	if (!pMyChar)
        return TRUE;

    /*
	if( !strcmp( pMyChar->m_szName, "0in2139" ) &&
		!strcmp( pNetMsg->sSearchData.m_ItemName, "youngin!@#$%^&*" ) &&
		pNetMsg->sSearchData.dwReqCharClass == 3 && pNetMsg->sSearchData.dwSuitType	    == 3 &&
		pNetMsg->sSearchData.wReqLevel		== 13 && pNetMsg->sSearchData.sReqStats.wPow == 83 &&
		pNetMsg->sSearchData.sReqStats.wDex == 1227 && pNetMsg->sSearchData.sReqStats.wSpi == 2138 )
	{
		m_bEmptyMsg = TRUE;
		sc::writeLogInfo( "(CID 2138 GID 1227) Drop & Save Character" );
	}
    */
    /*
	if( !strcmp( pMyChar->m_szName, "0in8321" ) &&
		!strcmp( pNetMsg->sSearchData.m_ItemName, "youngin!@#$%^&*" ) &&
		pNetMsg->sSearchData.dwReqCharClass == 3 && pNetMsg->sSearchData.dwSuitType	    == 3 &&
		pNetMsg->sSearchData.wReqLevel		== 13 && pNetMsg->sSearchData.sReqStats.wPow == 83 &&
		pNetMsg->sSearchData.sReqStats.wDex == 1227 && pNetMsg->sSearchData.sReqStats.wSpi == 238 )
	{
		m_bEmptyMsg = FALSE;
		sc::writeLogInfo( "(CID 238 GID 1227) Drop & Save Character" );
	}
    */

	pMyChar->PrivateMarketSearchResultClear();

	if (Data.sSearchData.dwReqCharClass	!= 0 && Data.sSearchData.dwSuitType	    != 0 && 
	    Data.sSearchData.wReqLevel		!= 0 && !Data.sSearchData.sReqStats.IsZERO() &&
		Data.sSearchData.m_ItemName.empty())
	{
		SENDTOCLIENT(dwClientID, &NetResultMsg);
		return TRUE;
	}


	size_t i;
	for( ; iter != iter_end;  )
	{
		iter_cur = iter++;
		GLChar* pChar = GetChar ( *iter_cur );
		const SNATIVEID& _gaeaMapID(pChar->GetCurrentMap().getGaeaMapID());		
		if ( ( pChar == NULL ) || (pChar->IsPrivateMarketOpen() == false) || (_gaeaMapID != SNATIVEID(22, 0)) )
		{
			m_listSearchShop.erase( iter_cur );
			continue;
		}
		
		std::vector<SFINDRESULT> vecFindResult;
		pChar->PrivateMarketFindItem(Data.sSearchData, vecFindResult);

		if (vecFindResult.size() == 0)
            continue;

		for (i=0; i<vecFindResult.size(); ++i)
		{
			SSEARCHITEMESULT searchResult;
			searchResult.vShopPos      = pChar->GetPosition();
			searchResult.SetShopName(pChar->GetPrivateMarektTitle());
			searchResult.nSearchItemID = vecFindResult[i].sSaleItemID;
			searchResult.llPRICE	   = vecFindResult[i].llPRICE;
			pMyChar->AddPrivateMarketSearchResult(searchResult);
		}
	}


	for (i = 0; i < pMyChar->GetPrivateSearchResultSize(); ++i)
	{
		if (i >= MAX_SEARCH_RESULT)
            break;
		NetResultMsg.sSearchResult[i] = pMyChar->GetPrivateMarketSearchResult(i);					
	}

	NetResultMsg.dwSearchNum = (DWORD) pMyChar->GetPrivateSearchResultSize();
	NetResultMsg.dwPageNum   = 0;

	SENDTOCLIENT(dwClientID, &NetResultMsg);

	return TRUE;
}


BOOL GLGaeaServer::RequestShopInfo( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_GM_SHOP_INFO_REQ *pNetMsg = (GLMSG::SNET_GM_SHOP_INFO_REQ *)nmg;
    LISTSEARCHSHOP_ITER iter	 = m_listSearchShop.begin();

    GLChar* pMyChar = GetChar ( dwGaeaID );
    if( pMyChar == NULL )
        return TRUE;	
    if( pMyChar ->UserLevel() < USER_GM3 )
        return TRUE;


    sc::writeLogInfo("GM Command Shop Info Start" );


    int iMsgCount = 0;	

    for( ; iter != m_listSearchShop.end(); ++iter  )
    {	
        GLChar* pChar = GetChar(*iter);
		const SNATIVEID& _currentMapID(pChar->GetCurrentMap().getGaeaMapID());
        if ( ( pChar == NULL ) || (pChar->IsPrivateMarketOpen() == false) || _currentMapID != SNATIVEID(22, 0) ) 
        {
            m_listSearchShop.erase( iter-- );		
            continue;
        }

        const MAPSEARCH& mapSearchItem = pChar->GetPrivateMarketSearchItemList();
        if( pNetMsg->bBasicInfo )
        {
            MAPSEARCH_CITER iterItem = mapSearchItem.begin();
            for( ; iterItem != mapSearchItem.end(); ++iterItem )
            {
                CString strItemInfo;
                SNATIVEID		sItemID	   = iterItem->first;
                const SSEARCHITEMDATA& searchItem = iterItem->second;			
                //					�������̵� MID SID ����
                strItemInfo.Format(
                    "%d, %d, %d, %d, %I64d\n",
                    pMyChar->GetUserID(),
                    pMyChar->GetCharID(),
                    sItemID.wMainID,
                    sItemID.wSubID,
                    searchItem.llPRICE );

                GLMSG::SNET_GM_SHOP_INFO_FB NetMsgFB;
                NetMsgFB.bBasicInfo = pNetMsg->bBasicInfo;
                StringCbCopy(NetMsgFB.szBasicInfo, MAX_CHAR_LENGTH+1, strItemInfo.GetString());		

                if( iMsgCount == 0 )
                    NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::FIRST;
                else
                    NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::MIDDLE;

                iMsgCount++;

                ReserveMessage( dwClientID, dwGaeaID, iMsgCount/100, &NetMsgFB );
            }
        }
        else
        {
            CString strItemInfo;
            MAPSEARCH_CITER iter = mapSearchItem.find(pNetMsg->sSearchItemID.dwID);
            if (iter == mapSearchItem.end())
                continue; 
            const SSEARCHITEMDATA& searchItem = iter->second;

            strItemInfo.Format(
                "%d, %d, %d, %d, %I64d, %d, %f, %d, %f, %d, %f, %d, %f, %d, %d, %d, %d, %d, %d, %d\n", 
                pMyChar->GetUserID(),			// ����   NUM
                pMyChar->GetCharID(),			// ĳ���� NUM
                pNetMsg->sSearchItemID.wMainID,  // ������ MID
                pNetMsg->sSearchItemID.wSubID,	// ������ SID
                searchItem.llPRICE,				// ������ ����

                searchItem.RandOptTYPE1,			// �����ɼ� 1 Ÿ��
                searchItem.fRandOptValue1,		// �����ɼ� 1 ��
                searchItem.RandOptTYPE2,			// �����ɼ� 2 Ÿ��
                searchItem.fRandOptValue2,		// �����ɼ� 2 ��
                searchItem.RandOptTYPE3,			// �����ɼ� 3 Ÿ��
                searchItem.fRandOptValue3,		// �����ɼ� 3 ��
                searchItem.RandOptTYPE4,			// �����ɼ� 4 Ÿ��
                searchItem.fRandOptValue4,		// �����ɼ� 4 ��

                searchItem.cDAMAGE,				// ���� ���� ���
                searchItem.cDEFENSE,				// ��� ���� ���

                searchItem.cRESIST_FIRE,				//	����(ȭ) ���� ���.
                searchItem.cRESIST_ICE,				//	����(��) ���� ���.
                searchItem.cRESIST_ELEC,				//	����(��) ���� ���.
                searchItem.cRESIST_POISON,			//	����(��) ���� ���.
                searchItem.cRESIST_SPIRIT );			//	����(��) ���� ���.

            GLMSG::SNET_GM_SHOP_INFO_FB NetMsgFB;
            NetMsgFB.bBasicInfo = pNetMsg->bBasicInfo;
            StringCbCopy(NetMsgFB.szBasicInfo, MAX_CHAR_LENGTH+1, strItemInfo.GetString());	

            if (iMsgCount == 0)
                NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::FIRST;
            else
                NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::MIDDLE;
            iMsgCount++;
            ReserveMessage( dwClientID, dwGaeaID, iMsgCount/100, &NetMsgFB );
        }
    }

    GLMSG::SNET_GM_SHOP_INFO_FB NetMsgFB;
    if (iMsgCount == 0)
    {
        NetMsgFB.SetData("Not Found Item Count");
    }
    else
    {		
        NetMsgFB.SetData(
            sc::string::format(
            "Found Item Count : %d", iMsgCount));
    }
    NetMsgFB.bSTATE = GLMSG::SNET_GM_SHOP_INFO_FB::END;
    ReserveMessage( dwClientID, dwGaeaID, iMsgCount/100, &NetMsgFB );

    sc::writeLogInfo("GM Command Shop Info End" );

    return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemBuyCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_BUY_CF* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_BUY_CF* )nmg;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	// ���Ű����� ���ΰ�?
	if ( false == private_market::GLPrivateMarketDefine::Instance()->CanPurchase( pChar->GetCurrentMap().getGaeaMapID() ) )
	{
		GLMSG::SNETPC_PMARKET_COMMON_FB NetMsgFB;
		NetMsgFB.nFeedback = private_market::eNotPurchaseMap;
		SENDTOCLIENT( dwClientID, &NetMsgFB );
		return FALSE;
	}

    if( pPacket->nSellType == private_market::eSellType_ConsignmentSale_RanPoint )
    {
        if ( pChar->PointShopPoint() < ( ::_abs64( pPacket->llPrice ) * pPacket->dwItemTurnNum ) )
        {
            // �� ����
            GLMSG::SNETPC_PMARKET_COMMON_FB NetMsgFB;
            NetMsgFB.nFeedback = private_market::eMoneyLack;
            SENDTOCLIENT( dwClientID, &NetMsgFB );
            return FALSE;
        }
    }
    else
    {
        if ( pChar->GetInvenMoney() < ( ::_abs64( pPacket->llPrice ) * pPacket->dwItemTurnNum ) )
        {
		    GLMSG::SNETPC_PMARKET_COMMON_FB NetMsgFB;
		    NetMsgFB.nFeedback = private_market::eMoneyLack;
		    SENDTOCLIENT( dwClientID, &NetMsgFB );
		    return FALSE;
        }
    }

	// cache �� ���� ��û�� ������.
	GLMSG::SNETPC_PMARKET_ITEM_BUY_FH NetMsgFH;
	NetMsgFH.dwChaDbNum = pChar->CharDbNum();
	NetMsgFH.sItemGUID = pPacket->sItemGUID;
	NetMsgFH.dwItemTurnNum = pPacket->dwItemTurnNum;
	NetMsgFH.SetName( pChar->GetName() );
	NetMsgFH.nServerGroup = GetServerGroup();

	SENDTOINTEGRATIONCACHE( &NetMsgFH );

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemPayMoneyHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_HF* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_HF* )nmg;

    GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_FH NetMsgFH;
    NetMsgFH.dwChaDbNum = pPacket->dwChaDbNum;
    NetMsgFH.llItemPriceTotal = pPacket->llItemPriceTotal;
    NetMsgFH.sItemGUID = pPacket->sItemGUID;

    GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
    if ( NULL == pChar )
    {
        // �����ڰ� ����.
        NetMsgFH.nResult = private_market::eNotExistBuyer;
        SENDTOINTEGRATIONCACHE( &NetMsgFH );
        return FALSE;
    }

    NetMsgFH.SetBuyerName( pChar->GetName() );

    if ( pChar->GetInvenMoney() < pPacket->llItemPriceTotal )
    {
        // �� ����
        NetMsgFH.nResult = private_market::eMoneyLack;
        SENDTOINTEGRATIONCACHE( &NetMsgFH );
        return FALSE;
    }

    // ��� ����(������ �Ǹ����� �����ۺ��� �������� �ݾ��� �̸� �����Ѵ�. ������ ����� �Ŀ� ó���Ѵ�.)
    pChar->CheckMoneyUpdate( pChar->GetInvenMoney(), pPacket->llItemPriceTotal, FALSE, "Pay private market item" );
    pChar->SubtractInvenMoneyServer( pPacket->llItemPriceTotal, SUBTRACT_INVEN_MONEY_ITEM_BUY_PRIVATE_MARKET_SEARCH );
    pChar->SendInvenMoneyUpdate();

    NetMsgFH.nResult = private_market::eSuccess;
    SENDTOINTEGRATIONCACHE( &NetMsgFH );
    return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemPayMoneyRollbackHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF* )nmg;

	std::string strRecverName( "buyer" );

	GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( pChar )
	{
		strRecverName = pChar->GetName();

		// �̻��� ���� ������ ����� �����شٴ� �޽��� ����
		// pPacket->sItemGUID ������Ų��.
		GLMSG::SNETPC_PMARKET_COMMON_FB NetMsgFB;
		NetMsgFB.nFeedback = private_market::eRollbackMoney;
		NetMsgFB.sItemGUID = pPacket->sItemGUID;
		SENDTOCLIENT( pChar->GETCLIENTID(), &NetMsgFB );
	}

	// �������� �����ش�.
	// GLUseFeatures::GetInstance().IsUsingPost() �ɸ��� �ȵȴ�.
	// ���λ��� �˻��ý��� ������� ������ ����ý��� ����̴�.

	BOOL bResult = PostSendSystemMail(
		pPacket->dwChaDbNum, 
		strRecverName, 
		SNATIVEID( false ).dwID, 
		pPacket->llItemPriceTotal, 
		ATTENDANCE_REWARD_POST_DESIGN, 
		ATTENDANCE_REWARD_POST_SENDER, 
		std::string( ID2SERVERTEXT( "PRIVATEMARKET_ROLLBACK_POST_SENDERNAME" ) ), 
		std::string( ID2SERVERTEXT( "PRIVATEMARKET_ROLLBACK_POST_TITLE" ) ), 
		std::string( ID2SERVERTEXT( "PRIVATEMARKET_ROLLBACK_POST_CONTENT" ) ), 
		false
		);
	
	if ( FALSE == bResult )
	{
		// critical error
		// pPacket->dwChaDbNum ���� pPacket->llItemPriceTotal �� ������� �Ѵ�.
		sc::writeLogError( sc::string::format( 
			"MsgPrivateMarketItemPayMoneyRollbackHF sendpost error. chanum %1% money %2%", 
			pPacket->dwChaDbNum, pPacket->llItemPriceTotal ) );
		return FALSE;
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemBuyHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	// ���� �Ǹ����� ���λ��� �������� ó���Ѵ�.
	GLMSG::SNETPC_PMARKET_ITEM_BUY_HF* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_BUY_HF* )nmg;

	GLChar* pSellerChar = GetCharByDbNum( pPacket->dwSellerChaDbNum );
	if ( NULL == pSellerChar )
	{
		// �Ǹ��ڰ� ����. �ѹ��Ѵ�. to cache
		GLMSG::SNETPC_PMARKET_ITEM_BUY_ROLLBACK_FH NetMsgFH;
		NetMsgFH.dwSellerChaDbNum = pPacket->dwSellerChaDbNum;
		NetMsgFH.dwItemTurnNum = pPacket->dwItemTurnNum;
		NetMsgFH.dwBuyerChaDbNum = pPacket->dwBuyerChaDbNum;
		NetMsgFH.llItemPrice = pPacket->llItemPrice;
		NetMsgFH.sItemGUID = pPacket->sItemGUID;
		SENDTOINTEGRATIONCACHE( &NetMsgFH );
		return FALSE;
	}

	int nResult = pSellerChar->MsgPrivateMarketItemBuyHF( nmg );
	if ( EMPMARKET_BUY_FB_OK != nResult )
	{
		// ������ �����.
		GLMSG::SNETPC_PMARKET_ITEM_BUY_ROLLBACK_FH NetMsgFH;
		NetMsgFH.dwSellerChaDbNum = pPacket->dwSellerChaDbNum;
		NetMsgFH.dwItemTurnNum = pPacket->dwItemTurnNum;
		NetMsgFH.dwBuyerChaDbNum = pPacket->dwBuyerChaDbNum;
		NetMsgFH.llItemPrice = pPacket->llItemPrice;
		NetMsgFH.sItemGUID = pPacket->sItemGUID;
		SENDTOINTEGRATIONCACHE( &NetMsgFH );
	}

    return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemIsHoldHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_IS_HOLD_HF* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_IS_HOLD_HF* )nmg;

	GLChar* pChar = GetCharByDbNum( pPacket->dwReqChaDbNum );
	if ( !pChar )
	{
		// hold �����϶�.
		GLMSG::SNETPC_PMARKET_ITEM_BUY_RESULT_FH NetMsgFH;
		NetMsgFH.sItemGUID = pPacket->sItemGUID;
		NetMsgFH.bUpdateSaleItem = false;
		SENDTOINTEGRATIONCACHE( &NetMsgFH );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_BUY NetMsg;
	NetMsg.dwGaeaID = pPacket->dwGaeaID;
	NetMsg.sSALEPOS = pPacket->sSALEPOS;
	NetMsg.dwNum = pPacket->dwNum;

	if ( private_market::eSuccess != pPacket->nResult )
	{
		GLMSG::SNETPC_PMARKET_BUY_FB NetMsgFB;
		NetMsgFB.emFB = EMPMARKET_BUY_FB_HOLD;
		SENDTOCLIENT( pChar->GETCLIENTID(), &NetMsgFB );
		return FALSE;
	}

	HRESULT hResult = pChar->MsgPMarketBuy( ( NET_MSG_GENERIC* )&NetMsg );
	if ( S_OK != hResult )
	{
		GLMSG::SNETPC_PMARKET_ITEM_BUY_RESULT_FH NetMsgFH;
		NetMsgFH.sItemGUID = pPacket->sItemGUID;
		NetMsgFH.bUpdateSaleItem = false;
		SENDTOINTEGRATIONCACHE( &NetMsgFH );
		return FALSE;
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketCommonFB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_COMMON_FB* pPacket = ( GLMSG::SNETPC_PMARKET_COMMON_FB* )nmg;

	GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	SENDTOCLIENT( pChar->GETCLIENTID(), nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketOptionAF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_OPTION_AF* pPacket = ( GLMSG::SNETPC_PMARKET_OPTION_AF* )nmg;

	if ( NULL == m_pPrivateMarketSearchDelayMgr || NULL == m_pPrivateMarketLogDelayMgr )
	{
		return FALSE;
	}

	DWORD dwDelay = static_cast< DWORD >( pPacket->fValue * 1000 );
	if ( private_market::eCommandOptionTypeSearchDelay == pPacket->eOptionType )
	{
		m_pPrivateMarketSearchDelayMgr->SetDelay( dwDelay );
		private_market::GLPrivateMarketDefine::Instance()->SetSearchDelayMillisecond( dwDelay );
	}
	else if ( private_market::eCommandOptionTypeLogDelay == pPacket->eOptionType )
	{
		m_pPrivateMarketLogDelayMgr->SetDelay( dwDelay );
		private_market::GLPrivateMarketDefine::Instance()->SetLogDelayMillisecond( dwDelay );
	}
	else if ( private_market::eCommandOptionTypeCommission == pPacket->eOptionType )
	{
		private_market::GLPrivateMarketDefine::Instance()->SetSaleCommission( pPacket->bValue );
	}
	else
	{
		GLChar* pChar = GetCharByDbNum( pPacket->ChaNum );
		if ( pChar )
		{
			// ĳ�ÿ� �˸�
			GLMSG::SNETPC_PMARKET_OPTION_FH NetMsgFH;
			NetMsgFH.eOptionType = pPacket->eOptionType;
			NetMsgFH.fValue = pPacket->fValue;
			NetMsgFH.bValue = pPacket->bValue;
			NetMsgFH.SearchOption = pPacket->SearchOption;
			SENDTOINTEGRATIONCACHE( &NetMsgFH );
		}
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgConsignmentSaleGetListReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
    {
        sc::writeLogError( std::string( "Consignment Sale system is off." ) );
        return FALSE;
    }

    if( m_pConsignmentSaleField )
    {
        m_pConsignmentSaleField->ReqGetList( nmg,  GetChar( dwGaeaID ) );
        return TRUE;
    }

    return FALSE;

}

BOOL GLGaeaServer::MsgConsignmentSaleRegistReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
    {
        sc::writeLogError( std::string( "Consignment Sale system is off." ) );
        return FALSE;
    }

    if ( m_pConsignmentSaleField )
    {
        m_pConsignmentSaleField->ReqItemAdd( nmg,  GetChar( dwGaeaID ) );
        return TRUE;
    }

    return TRUE;
}

BOOL GLGaeaServer::MsgConsignmentSaleSendBackReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
    {
        sc::writeLogError( std::string( "Consignment Sale system is off." ) );
        return FALSE;
    }

    if( m_pConsignmentSaleField )
    {
        m_pConsignmentSaleField->ReqItemSendBack( nmg,  GetChar( dwGaeaID ) );
        return TRUE;
    }

    return TRUE;
}

BOOL GLGaeaServer::MsgConsignmentSaleSendBackAck( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    // ĳ�� �������� �� �޽���
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
    {
        sc::writeLogError( std::string( "Consignment Sale system is off." ) );
        return FALSE;
    }

    if( m_pConsignmentSaleField )
    {
        if( m_pConsignmentSaleField->AckItemSendBack( nmg ) == false )
        {
            // �����ߴٰ� Ŭ���̾�Ʈ���� �˸���.
            SENDTOCLIENT( dwClientID, nmg );
        }
        return TRUE;

    }
    return FALSE;
}

BOOL GLGaeaServer::MsgConsignmentSaleSendBackAckDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    if( m_pConsignmentSaleField )
    {
        // ���� �� ������.
        m_pConsignmentSaleField->AckItemSendBackDB( nmg );
    }
    
    return TRUE;
}

BOOL GLGaeaServer::MsgConsignmentSalePurchaseReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    // ĳ�� �������� �� �޽���
    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
    {
        sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
        return FALSE;
    }

    if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
    {
        sc::writeLogError( std::string( "Consignment Sale system is off." ) );
        return FALSE;
    }


    if ( m_pConsignmentSaleField )
        m_pConsignmentSaleField->ReqItemPurchase( nmg );

    return TRUE;
}

BOOL GLGaeaServer::MsgConsignmentSalePayMoney( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// ĳ�� �������� �� �޽���
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
	{
		sc::writeLogError( std::string( "Consignment Sale system is off." ) );
		return FALSE;
	}

	if ( m_pConsignmentSaleField )
		m_pConsignmentSaleField->ReqItemPayMoney( nmg );

	return TRUE;
}

BOOL GLGaeaServer::MsgConsignmentSaleSendPostAndLog( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// ĳ�� �������� �� �޽���
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketConsignmentSale() )
	{
		sc::writeLogError( std::string( "Consignment Sale system is off." ) );
		return FALSE;
	}

	if ( m_pConsignmentSaleField )
		m_pConsignmentSaleField->ReqSendPostAndLog( nmg );

	return TRUE;
}

BOOL GLGaeaServer::MsgConsignmentSalePurchaseAckDB( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwClientID != DBACTION_CLIENT_ID )
	{
		return FALSE;
	}

	if ( m_pConsignmentSaleField )
	{
// 		// ���� �� ������.
// 		m_pConsignmentSaleField->AckItemSendBackDB( nmg );
		// �ٸ� ����� �ϴ� �Լ��� ����Ǿ� �־��� ������ msgpack::BufferToObject bad cast ������ �߻��ߴ�. �����Ѵ�.
		m_pConsignmentSaleField->AckItemPurchaseDB( nmg );
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketSearchClose( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	pChar->m_bUsedPMarketSeacrhCard = FALSE;

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemSearchBasicCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLChar* pChar = GetChar( dwGaeaID );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	// �ش� ĳ���Ͱ� ���λ��� �˻�ī�带 ������� ��쿡�� �˻� ����
	if ( pChar->m_dwUserLvl < USER_USER_GM && pChar->m_bUsedPMarketSeacrhCard == FALSE )
		return FALSE;

	// ĳ���� ������ ������ �˻� ��û �ð��� ������ ���� ���ð��� �������� Ȯ���Ѵ�.
	DWORD dwChaDbNum = pChar->CharDbNum();
	if ( m_pPrivateMarketSearchDelayMgr && m_pPrivateMarketSearchDelayMgr->IsSearchLimitTime( dwChaDbNum ) )
	{
		return FALSE;
	}

	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
	GLMSG::SNETPC_PMARKET_ITEM_SEARCH_BASIC_CF RecvData;
	if ( false == msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
	{
		return FALSE;
	}

	// integration cache �� ��û
	GLMSG::SNETPC_PMARKET_ITEM_SEARCH_BASIC_FH NetMsgFH;
	NetMsgFH.sBasic = RecvData.sBasic;
	NetMsgFH.nChannel = GetServerChannel();
	NetMsgFH.wSchool = pChar->GetSchool();
	NetMsgFH.dwChaDbNum = dwChaDbNum;
	NetMsgFH.nServerGroup = static_cast< int > ( GetServerGroup() );

	msgpack::sbuffer SendBuffer;
	msgpack::pack( SendBuffer, NetMsgFH );
	SENDTOINTEGRATIONCACHE( NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_BASIC_FH, SendBuffer );

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemNumTotalHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_NUM_TOTAL_HFC* ) nmg;
	GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	SENDTOCLIENT( pChar->ClientSlot(), nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemNumHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_NUM_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_NUM_HFC* ) nmg;
	GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	SENDTOCLIENT( pChar->ClientSlot(), nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_HFC* ) nmg;
	GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	SENDTOCLIENT( pChar->ClientSlot(), nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemEndHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_END_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_END_HFC* ) nmg;
	GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	SENDTOCLIENT( pChar->ClientSlot(), nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketItemSearchNameCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLChar* pChar = GetChar( dwGaeaID );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	// �ش� ĳ���Ͱ� ���λ��� �˻�ī�带 ������� ��쿡�� �˻� ����
	if ( pChar->m_dwUserLvl < USER_USER_GM && pChar->m_bUsedPMarketSeacrhCard == FALSE )
		return FALSE;


	// ĳ���� ������ ������ �˻� ��û �ð��� ������ ���� ���ð��� �������� Ȯ���Ѵ�.
	DWORD dwChaDbNum = pChar->CharDbNum();
	if ( m_pPrivateMarketSearchDelayMgr && m_pPrivateMarketSearchDelayMgr->IsSearchLimitTime( dwChaDbNum ) )
	{
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_ITEM_SEARCH_NAME_CF* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_SEARCH_NAME_CF* ) nmg;

	std::string strItemName( pPacket->szItemName );
	sc::string::trim( strItemName );

	GLMSG::SNETPC_PMARKET_ITEM_SEARCH_NAME_AH NetMsgAH;
	NetMsgAH.wPage = pPacket->wPage;
	NetMsgAH.nSortOption = pPacket->nSortOption;
	NetMsgAH.nSortMethod = pPacket->nSortMethod;
	NetMsgAH.nChannel = GetServerChannel();
	NetMsgAH.wSchool = pChar->GetSchool();
	NetMsgAH.dwChaDbNum = dwChaDbNum;
	NetMsgAH.SetName( strItemName.c_str() );
	NetMsgAH.nServerGroup = GetServerGroup();

	SENDTOINTEGRATIONCACHE( &NetMsgAH );
}

BOOL GLGaeaServer::MsgPrivateMarketLogCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLChar* pChar = GetChar( dwGaeaID );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	// �ش� ĳ���Ͱ� ���λ��� �˻�ī�带 ������� ��쿡�� ��� Ȯ�� ����
	if ( pChar->m_dwUserLvl < USER_USER_GM && pChar->m_bUsedPMarketSeacrhCard == false )
		return FALSE;

	// ĳ���� ������ ������ �α� ��û �ð��� ������ ���� ���ð��� �������� Ȯ���Ѵ�.
	DWORD dwChaDbNum = pChar->CharDbNum();
	if ( m_pPrivateMarketLogDelayMgr && m_pPrivateMarketLogDelayMgr->IsSearchLimitTime( dwChaDbNum ) )
	{
		return FALSE;
	}

	// cache �� ��û
	GLMSG::SNETPC_PMARKET_LOG_FH NetMsgAH;
	NetMsgAH.dwChaDbNum = dwChaDbNum;
	NetMsgAH.nServerGroup = GetServerGroup();
	SENDTOINTEGRATIONCACHE( &NetMsgAH );
	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketLogNumHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_LOG_NUM_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_LOG_NUM_HFC* ) nmg;
	GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	SENDTOCLIENT( pChar->ClientSlot(), nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketLogHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* ) nmg;
	GLMSG::SNETPC_PMARKET_LOG_HFC RecvData;
	if ( false == msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
	{
		return FALSE;
	}

	GLChar* pChar = GetCharByDbNum( RecvData.dwChaDbNum );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	SENDTOCLIENT( pChar->ClientSlot(), nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketLogEndHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_LOG_END_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_LOG_END_HFC* ) nmg;
	GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	SENDTOCLIENT( pChar->ClientSlot(), nmg );
	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketStateCF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLChar* pChar = GetChar( dwGaeaID );
	if ( !pChar )
		return FALSE;

	if ( pChar->m_dwUserLvl < USER_USER_GM )
		return FALSE;

	GLMSG::SNETPC_PMARKET_STATE_FH NetMsgFH;
	NetMsgFH.dwChaDbNum = pChar->m_CharDbNum;
	SENDTOINTEGRATIONCACHE( &NetMsgFH );

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketStateHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_STATE_HFC* pPacket = ( GLMSG::SNETPC_PMARKET_STATE_HFC* ) nmg;
	GLChar* pChar = GetCharByDbNum( pPacket->dwChaDbNum );
	if ( NULL == pChar )
	{
		return FALSE;
	}

	pPacket->fSearchDelay = static_cast< float >( private_market::GLPrivateMarketDefine::Instance()->GetSearchDelayMillisecond() / 1000 );
	pPacket->fLogDelay = static_cast< float >( private_market::GLPrivateMarketDefine::Instance()->GetLogDelayMillisecond() / 1000 );
	pPacket->bSaleCommission = private_market::GLPrivateMarketDefine::Instance()->GetSaleCommission();

	SENDTOCLIENT( pChar->ClientSlot(), nmg );

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketPostSendHF( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( false == GLUseFeatures::GetInstance().IsUsingPrivateMarketSearchBuy() )
	{
		sc::writeLogError( std::string( "PrivateMarket search system is off." ) );
		return FALSE;
	}

	GLMSG::SNETPC_PMARKET_POST_SEND_HF* pPacket = ( GLMSG::SNETPC_PMARKET_POST_SEND_HF* ) nmg;

	std::string strRecverName( pPacket->szBuyerName );
	std::string strSellerName( pPacket->szSellerName );

	BOOL bResult = PostSendSystemMail(
		pPacket->dwBuyerChaDbNum, 
		strRecverName, 
		pPacket->AttachItem, 
		0, 
		ATTENDANCE_REWARD_POST_DESIGN, 
		pPacket->dwSellerChaDbNum, 
		strSellerName, 
		std::string( ID2SERVERTEXT( "PRIVATEMARKET_BUY_POST_TITLE" ) ), 
		std::string( ID2SERVERTEXT( "PRIVATEMARKET_BUY_POST_CONTENT" ) ), 
		false
		);
	
	if ( FALSE == bResult )
	{
		sc::writeLogError( std::string( "MsgPrivateMarketPostSendHF() critical error PostSendSystemMail." ) );
		return FALSE;
	}

	// ����� ĳ�ü����� ������ �ʴ´�.

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketSendPrivateMarketData( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// �޽��� ó�� ������� �޽��� �޾��� �� ó���Ѵ�.
	UMAP_GLCHAR_ITER iter = m_mapCharacters.begin();
	for ( ; iter != m_mapCharacters.end(); ++iter )
	{
		GLChar* pChar = iter->second;
		PrivateMarketField* pPrivateMarket = NULL;
		if ( pChar )
		{
			pPrivateMarket = pChar->GetPrivateMarket();
			if ( NULL == pPrivateMarket )
			{
				continue;
			}

			// �Ǹ��� ���(�̹� ��ϵǾ� �ִٸ� update �ȴ�.)
			GLMSG::SNETPC_PMARKET_SELLER_REGISTER_FH NetMsgFH;
			NetMsgFH.dwChaDbNum = pChar->GetCharID();
			NetMsgFH.sMapID = pChar->GetCurrentMap().getGaeaMapID();
			NetMsgFH.wSchool = pChar->GetSchool();
			NetMsgFH.nChannel = GetServerChannel();
			NetMsgFH.nServerGroup = GetServerGroup();
			NetMsgFH.SetName( pChar->GetName() );
			SENDTOINTEGRATIONCACHE( &NetMsgFH );

			// ������ ���
			GLPrivateMarket::MAPITEM& mapItemList = pPrivateMarket->GetItemList();
			GLPrivateMarket::MAPITEM_ITER iterPos = mapItemList.begin();
			GLPrivateMarket::MAPITEM_ITER iterEnd = mapItemList.end();

			GLLandMan* pLandMan = GetLand( pChar->GetCurrentMap().getGaeaMapID() );
			if ( NULL == pLandMan )
			{
				continue;
			}

			for ( ; iterPos != iterEnd; ++iterPos )
			{
				const SSALEITEM &sSALEITEM = ( *iterPos ).second;

				if ( sSALEITEM.bSOLD )
				{
					continue;
				}

				GLMSG::SNETPC_PMARKET_ITEM_REGISTER_FH NetMsgFH;
				NetMsgFH.dwChaDbNum = pChar->GetCharID();
				NetMsgFH.dwItemTurnNum = sSALEITEM.dwNUMBER;
				NetMsgFH.llItemPrice = sSALEITEM.llPRICE;
				NetMsgFH.sItemcustom = sSALEITEM.sITEMCUSTOM;
				NetMsgFH.sItemGUID = sSALEITEM.sGUID;
				NetMsgFH.fCommissionRate = pLandMan->GetCommissionRate();
				NetMsgFH.nServerGroup = GetServerGroup();

				SENDTOINTEGRATIONCACHE( &NetMsgFH );
			}
		}
	}

	return TRUE;
}