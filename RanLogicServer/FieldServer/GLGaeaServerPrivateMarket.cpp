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
		// 개인상점 검색시스템을 사용하지 않는다면 예전방식으로
		pChar->MsgPMarketBuy( nmg );
		return TRUE;
	}

	// 개인상점 검색시스템 때문에 바로 구입하면 안된다.
	// cache 에서 홀드된 아이템인지 확인한 후 처리한다.
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
		// 개인상점 검색시스템을 사용중이라면 기존 개인상점 검색 시스템 사용하지 않음
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
                //					유저아이디 MID SID 가격
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
                pMyChar->GetUserID(),			// 유저   NUM
                pMyChar->GetCharID(),			// 캐릭터 NUM
                pNetMsg->sSearchItemID.wMainID,  // 아이템 MID
                pNetMsg->sSearchItemID.wSubID,	// 아이템 SID
                searchItem.llPRICE,				// 아이템 가격

                searchItem.RandOptTYPE1,			// 랜덤옵션 1 타입
                searchItem.fRandOptValue1,		// 랜덤옵션 1 값
                searchItem.RandOptTYPE2,			// 랜덤옵션 2 타입
                searchItem.fRandOptValue2,		// 랜덤옵션 2 값
                searchItem.RandOptTYPE3,			// 랜덤옵션 3 타입
                searchItem.fRandOptValue3,		// 랜덤옵션 3 값
                searchItem.RandOptTYPE4,			// 랜덤옵션 4 타입
                searchItem.fRandOptValue4,		// 랜덤옵션 4 값

                searchItem.cDAMAGE,				// 공격 연마 등급
                searchItem.cDEFENSE,				// 방어 연마 등급

                searchItem.cRESIST_FIRE,				//	저항(화) 개조 등급.
                searchItem.cRESIST_ICE,				//	저항(빙) 개조 등급.
                searchItem.cRESIST_ELEC,				//	저항(전) 개조 등급.
                searchItem.cRESIST_POISON,			//	저항(독) 개조 등급.
                searchItem.cRESIST_SPIRIT );			//	저항(정) 개조 등급.

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

	// 구매가능한 맵인가?
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
            // 돈 부족
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

	// cache 에 구매 요청을 보낸다.
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
        // 구매자가 없다.
        NetMsgFH.nResult = private_market::eNotExistBuyer;
        SENDTOINTEGRATIONCACHE( &NetMsgFH );
        return FALSE;
    }

    NetMsgFH.SetBuyerName( pChar->GetName() );

    if ( pChar->GetInvenMoney() < pPacket->llItemPriceTotal )
    {
        // 돈 부족
        NetMsgFH.nResult = private_market::eMoneyLack;
        SENDTOINTEGRATIONCACHE( &NetMsgFH );
        return FALSE;
    }

    // 대금 지불(구조상 판매자의 아이템보다 구매자의 금액을 미리 인출한다. 문제가 생기면 후에 처리한다.)
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

		// 이상이 생겨 아이템 대금을 돌려준다는 메시지 전송
		// pPacket->sItemGUID 삭제시킨다.
		GLMSG::SNETPC_PMARKET_COMMON_FB NetMsgFB;
		NetMsgFB.nFeedback = private_market::eRollbackMoney;
		NetMsgFB.sItemGUID = pPacket->sItemGUID;
		SENDTOCLIENT( pChar->GETCLIENTID(), &NetMsgFB );
	}

	// 우편으로 돌려준다.
	// GLUseFeatures::GetInstance().IsUsingPost() 걸리면 안된다.
	// 개인상점 검색시스템 사용전제 조건은 우편시스템 사용이다.

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
		// pPacket->dwChaDbNum 에게 pPacket->llItemPriceTotal 을 돌려줘야 한다.
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

	// 실제 판매자의 개인상점 아이템을 처리한다.
	GLMSG::SNETPC_PMARKET_ITEM_BUY_HF* pPacket = ( GLMSG::SNETPC_PMARKET_ITEM_BUY_HF* )nmg;

	GLChar* pSellerChar = GetCharByDbNum( pPacket->dwSellerChaDbNum );
	if ( NULL == pSellerChar )
	{
		// 판매자가 없다. 롤백한다. to cache
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
		// 문제가 생겼다.
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
		// hold 해제하라.
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
			// 캐시에 알림
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
    // 캐쉬 서버에서 온 메시지
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
            // 실패했다고 클라이언트에게 알린다.
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
        // 실패 할 수없다.
        m_pConsignmentSaleField->AckItemSendBackDB( nmg );
    }
    
    return TRUE;
}

BOOL GLGaeaServer::MsgConsignmentSalePurchaseReq( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
    // 캐쉬 서버에서 온 메시지
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
	// 캐쉬 서버에서 온 메시지
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
	// 캐쉬 서버에서 온 메시지
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
// 		// 실패 할 수없다.
// 		m_pConsignmentSaleField->AckItemSendBackDB( nmg );
		// 다른 기능을 하는 함수가 연결되어 있었기 때문에 msgpack::BufferToObject bad cast 에러가 발생했다. 변경한다.
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

	// 해당 캐릭터가 개인상점 검색카드를 사용중일 경우에만 검색 가능
	if ( pChar->m_dwUserLvl < USER_USER_GM && pChar->m_bUsedPMarketSeacrhCard == FALSE )
		return FALSE;

	// 캐릭터 단위로 마지막 검색 요청 시간을 저장해 놓고 대기시간이 지났는지 확인한다.
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

	// integration cache 에 요청
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

	// 해당 캐릭터가 개인상점 검색카드를 사용중일 경우에만 검색 가능
	if ( pChar->m_dwUserLvl < USER_USER_GM && pChar->m_bUsedPMarketSeacrhCard == FALSE )
		return FALSE;


	// 캐릭터 단위로 마지막 검색 요청 시간을 저장해 놓고 대기시간이 지났는지 확인한다.
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

	// 해당 캐릭터가 개인상점 검색카드를 사용중일 경우에만 기록 확인 가능
	if ( pChar->m_dwUserLvl < USER_USER_GM && pChar->m_bUsedPMarketSeacrhCard == false )
		return FALSE;

	// 캐릭터 단위로 마지막 로그 요청 시간을 저장해 놓고 대기시간이 지났는지 확인한다.
	DWORD dwChaDbNum = pChar->CharDbNum();
	if ( m_pPrivateMarketLogDelayMgr && m_pPrivateMarketLogDelayMgr->IsSearchLimitTime( dwChaDbNum ) )
	{
		return FALSE;
	}

	// cache 에 요청
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

	// 결과는 캐시서버에 보내지 않는다.

	return TRUE;
}

BOOL GLGaeaServer::MsgPrivateMarketSendPrivateMarketData( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	// 메시지 처리 방식으로 메시지 받았을 때 처리한다.
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

			// 판매자 등록(이미 등록되어 있다면 update 된다.)
			GLMSG::SNETPC_PMARKET_SELLER_REGISTER_FH NetMsgFH;
			NetMsgFH.dwChaDbNum = pChar->GetCharID();
			NetMsgFH.sMapID = pChar->GetCurrentMap().getGaeaMapID();
			NetMsgFH.wSchool = pChar->GetSchool();
			NetMsgFH.nChannel = GetServerChannel();
			NetMsgFH.nServerGroup = GetServerGroup();
			NetMsgFH.SetName( pChar->GetName() );
			SENDTOINTEGRATIONCACHE( &NetMsgFH );

			// 아이템 등록
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