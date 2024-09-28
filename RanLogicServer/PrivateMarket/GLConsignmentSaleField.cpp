#include "pch.h"
#include "GLConsignmentSaleField.h"

#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../enginelib/GUInterface/UITextUtil.h"
#include "../../enginelib/GUInterface/UITextControl.h"

#include "../../RanLogic/Msg/ConsignmentSaleMsg.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLogicData.h"

#include "../Character/GLChar.h"
#include "../Character/GLChar.h"

#include "../Database/DBAction/DbActionGamePost.h"
#include "../Database/DBAction/DbActionGameConsignmentSale.h"

#include "../Util/GLItemLimit.h"

#include "../FieldServer/GLGaeaServer.h"


namespace
{
	//const char* MARKET_RANPOINT_SERVER_TEXT_0 = "마감 %d시간 이내";
}

bool GLConsignmentSaleField::ReqGetList( NET_MSG_GENERIC* nmg, GLChar* pChar )
{
    if( pChar == NULL )
        return false;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_LIST_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return false;

    //반복 요청을 막는다.
    if( m_mapUserSlotCount[RecvData.m_dwChaDbNum] < 0 )
        return false;

    m_mapUserSlotCount[RecvData.m_dwChaDbNum] = -1;

    if( pChar->CharDbNum() != RecvData.m_dwChaDbNum )
        return false;

    GLMSG::NET_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK SendData;
    {
        SendData.ExpirationDate = pChar->GetConsignmentSaleSlotExpansionDate();
    }
    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK, PackBuffer );

    m_pGaeaServer->AddGameAdoJob( 
        db::DbActionPtr( new db::ConsignmentSaleGetList( pChar->CharDbNum(), m_mapUserSlotCount[RecvData.m_dwChaDbNum] )) );
    

    return true;
}

bool GLConsignmentSaleField::ReqItemAdd( NET_MSG_GENERIC* nmg, GLChar* pChar )
{
    if ( pChar == NULL )
	{
        return false;
	}

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
    GLMSG::NET_CONSIGNMENT_ADD_ITEM_REQ RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return false;

	// 아이템 수량이 0 인것은 등록할 수 없다. 해당 아이템이 등록된다면 반환등에서 오류가 발생됨 ( 메일에 첨부아이템이 없어짐 )
	if ( RecvData.m_wTurnNum == 0 )
		return false;

    // 존재하는 아이템인지 확인
    const SITEM* pITEM = GLogicData::GetInstance().GetItem( RecvData.m_NativeID );
    if ( !pITEM )
        return false;

	if ( RecvData.m_llPrice <= 0 ||
		RecvData.m_wTurnNum <= 0 ||
		W_LIMITED_POINT_TRADE_COUNT < RecvData.m_wTurnNum )
		return false;

    // 최소 이하로 금액을 내릴 수 없으며 리미트 이상 금액으로 올릴 수 없다;
	if ( RecvData.m_llPrice < private_market::GLPrivateMarketDefine::Instance()->RanPointSaleMinPrice() ||
		LL_LIMITED_A_UNIT_PRICE < RecvData.m_llPrice )
		return false;

    SITEMCUSTOM sItemCustom( RecvData.m_NativeID );

    // 게임머니 거래인가?
    std::vector< private_market::GameMoneyItemFee > vecMoneyPeriodFee 
        = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

    bool bIsGameMoney = false;

	// GetMoneySellPeriod, 검색하는 거 수정하자.
    for( unsigned int i = 0; i < vecMoneyPeriodFee.size(); i++ )
    {
        const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[i];

        if ( value.sNativeID == RecvData.m_NativeID )
        {
            bIsGameMoney = true;
            break;
        }
    }

    DWORD dwFee = private_market::GLPrivateMarketDefine::Instance()->CalcFee( RecvData.m_wPeriodIndex, sItemCustom );
	
	// 기존 기획서에는 수수료가 없는경우 거래 불가이지만 GS에 적용할때 동일한 기능을 가진 포장지가 추가되어 있어서 수수료를 0이 가능하도록 변경됨 //if( dwFee <= 0 )
    if ( dwFee < 0 )
        return false;

    // 확장 슬롯 시간 계산
    WORD wCurrentSlotNum = private_market::GLPrivateMarketDefine::Instance()->SellResgisterBasicNum();

    const __time64_t& tExpansionDate = pChar->GetConsignmentSaleSlotExpansionDate();    
    const __time64_t& tCurTime = CTime::GetCurrentTime().GetTime();

    if ( 0 < tExpansionDate - tCurTime )
        wCurrentSlotNum += private_market::GLPrivateMarketDefine::Instance()->SellResgisterExpandAddNum();

    if ( wCurrentSlotNum <= m_mapUserSlotCount[RecvData.m_dwChaDbNum] )
    {
        // 더 이상 등록 할 수 없습니다.
        return false;
    }

    // 실패할경우 실패 메시지
    GLMSG::NET_CONSIGNMENT_ADD_ITEM_ACK SendData;
    SendData.bAddSucess = false;
    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );

    // 아이템 수수료를 확인한다.
    if ( pChar->PointShopPoint() <  dwFee )
        return false;

	// 골드거래 등록이 꺼저있으면 등록할수 없다.
	if ( private_market::GLPrivateMarketDefine::Instance()->GoldTrade() == false && RecvData.m_bySellType == private_market::eSellType_ConsignmentSale_Gold  ) 
		return false;

    // 아이템 판매 타입을 확인
    if ( RecvData.m_bySellType <= private_market::eSellType_PrivateMarket
        && private_market::eSellType_ConsignmentSale_END <= RecvData.m_bySellType )
    {
        m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
        return false;
    }

	// 위탁판매 고유키 생성
	// 게임머니 판매의 경우 아이템 guid 도 이 값으로 세팅되며, 이 값은 위탁판매 고유키가 된다.
	sc::MGUID guid;
	sc::SeqUniqueGUID::Instance()->getGUID( guid );

    if ( bIsGameMoney )
    {
        LONGLONG llGameMoney = pITEM->sBasicOp.dwSellPrice * RecvData.m_wTurnNum;

        // 소지 골드 부족. 물품 등록 불가.
        if ( pChar->GetInvenMoney() < llGameMoney )
            return false;
        
        pChar->SubtractInvenMoneyAndUpdate( llGameMoney, SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_REGISTRATION );

		// 클라이언트에서는 1개로 고정해서 보내지만, 실제 아이템 개수를 2이상 등록을 해도 정상적으로 동작하도록 개수 조정한다.
		sItemCustom.wTurnNum = RecvData.m_wTurnNum;
		sItemCustom.SetGuid( guid );

		// 통계 및 로그 수정
		pChar->LogMoneyExchangeWithItemGuid( 
			gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, pChar->m_CharDbNum, 
			-llGameMoney, 
			ITEM_ROUTE_CONSIGMENT_SALE, 
			pChar->GetCurrentMap(), 
			LOG_MONEY_CONSIGNMENT_SALE_GAMEMONEY_SELL, 
			sItemCustom.guid );

        pChar->LogMoneyExchangeWithItemGuid( 
			gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, pChar->m_CharDbNum, 
			pChar->GetInvenMoney(), 
			MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY, 
			pChar->GetCurrentMap(), 
			LOG_MONEY_CURRENT_INVEN_BY_CONSIGEMENT, 
			sItemCustom.guid );
    }
    else
    {     
        // 인벤 아이템을 갖고 온다.
        SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( RecvData.m_nInvenPosX, RecvData.m_nInvenPosY );
        if ( !pInvenItem )
        {
            m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
            return false;
        }

		if ( pInvenItem->sItemCustom.IS_GUID_NULL() )
		{
			sc::writeLogError( std::string( "GLConsignmentSaleField::ReqItemAdd pInvenItem->sItemCustom.IS_GUID_NULL()" ) );
			m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
			return false;
		}

		sItemCustom = pInvenItem->sItemCustom;

        // 판매하려는 아이템정보와 실제 아이템 ID를 비교한다.
        if ( pInvenItem->GetNativeID() != RecvData.m_NativeID )
        {
            m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
            return false;
        }

        // 거래불가능하면
		if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
		{
			// 코스튬이 붙어 있다면 거래 불가
			return false;
		}
		else if ( sItemCustom.IsTimeLimit() == true )
		{
			// 시효성 아이템 거래 불가
			return false;
		}
		else if ( sItemCustom.IsGM_GENITEM() )
		{
			// #3989 GmCharEdit 에서 거래불가로 넣어준 아이템을 클럽창고에 넣을 수 있는 문제 수정, 지엠에디트 체크하지 않던
			// 아이템교환, 포인트거래에도 검사하도록 추가
			return false;
		}
		else if ( ( sItemCustom.IsExchange() != NULL ) == false )
        {
            // 란 포인트 거래만 거래 가능
            if ( RecvData.m_bySellType != private_market::eSellType_ConsignmentSale_RanPoint )
                return false;

            // 거래 가능횟수를 초과하는지 검사
            if ( pITEM->sBasicOp.RanPointTradeCount() <= pInvenItem->sItemCustom.wRanPointTradeCount )
			{
                m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
                return false;
            }
        }

        // 아이템의 겹침 갯수를 초과할 수 없다.
        if( pInvenItem->TurnNum() < RecvData.m_wTurnNum )
        {
            m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
            return false;
        }

        // 모든 아이템을 등록한다면 삭제시키고
        // 아닌경우에는 수만 감소시킨다.
        if ( pInvenItem->TurnNum() == RecvData.m_wTurnNum )
        {
			// #item
			// 우선 동작하도록 만들어두고, 우편, 개인상점, 위탁판매 개선하자.
			// 서버간 아이템 이동 가능하고, 아이템 guid 를 그대로 유지하기 위해서 디비에서 삭제하는 방식의 함수 호출
			if ( pChar->InvenCompletelyDeleteItem( pInvenItem->wPosX, pInvenItem->wPosY, true, FALSE, 90345 ) == false )
                return false;
        }
        else
        {
            pInvenItem->sItemCustom.wTurnNum -= RecvData.m_wTurnNum;

            //	[자신에게] Inventory 아이템 소모됨.
            GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
            NetMsg_Inven_Update.dwChaNum = pChar->CharDbNum();
            NetMsg_Inven_Update.wPosX = pInvenItem->wPosX;
            NetMsg_Inven_Update.wPosY = pInvenItem->wPosY;
            NetMsg_Inven_Update.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
            m_pGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &NetMsg_Inven_Update );

        }
    }

    // 수수료를 뺀다.
    pChar->PointShopPointSub( dwFee, pointshop::CONSIGNMENT_SALE, pointshop::POINT_SUB_TYPE_CONSIGNMENT_ADD );

	// 통계 및 로그 수정
	pChar->LogMoneyExchangeWithItemGuid( 
		gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, 
		RecvData.m_llPrice, 
		ITEM_ROUTE_CONSIGMENT_SALE, 
		pChar->GetCurrentMap(), 
		LOG_RANPOINT_INFO_ITEM_PRICE_BY_CONSIGEMENT, 
		sItemCustom.guid );

	pChar->LogMoneyExchangeWithItemGuid( 
		gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, 
		dwFee, 
		ITEM_ROUTE_CONSIGMENT_SALE, 
		pChar->GetCurrentMap(), 
		LOG_RANPOINT_MINUS_FEE_BY_CONSIGEMENT, 
		sItemCustom.guid );

	// 통계 및 로그 수정
    pChar->LogItemExchange( 
        sItemCustom, 
        gamelog::ID_CHAR, pChar->CharDbNum(),
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_CONSIGMENT_SALE, 
        RecvData.m_wTurnNum,
		pChar->GetCurrentMap(), LOG_ITEM_INFO_REGIST_ITEM_BY_CONSIGEMENT,
        true );

    PackBuffer.clear();

    // NET_CONSIGNMENT_ADD_ITEM_ACK SendData 정보 채우기
	WORD wPeriod=0;
    private_market::ConsignmentSaleItem& saleItem = SendData.SaleItem;
    {
        // 거래 기간에 따라 기본 수수료를 부가한다.
        std::vector< private_market::PeriodFee > vecPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetSellPeriod();
        if ( 0 <= RecvData.m_wPeriodIndex && RecvData.m_wPeriodIndex < static_cast< int >( vecPeriodFee.size() ) )
             wPeriod = vecPeriodFee[RecvData.m_wPeriodIndex].wPeriod;

        saleItem.dwChaDbNum = pChar->CharDbNum();
        saleItem.strChaName = pChar->GetName();

		/*
		포인트거래로 구매한 아이템을 곧바로 다시 포인트거래 등록하면 아이템이 삭제되는 문제 처리
		포인트거래 디비 테이블에 같은 키값으로 삽입이 되지않기 때문에 발생하며(서버 로직에서 아이템을 먼저 삭제후 디비에 처리 요청하는 형태)
		아이템 구조 변경이전에는 발생하지 않았다. 왜냐면 아이템 구조 변경전에는 포인트거래 등록할 때마다 새로운 키값을 발급해서 사용하였기 때문
		아이템 구조가 변경되면서 포인트거래 키값은 등록 아이템의 키값을 사용하도록 변경되었다.
		그래서 판매자가 개인상점검색창을 열고 판매된 아이템 결과를 처리해야 실제 포인트거래 디비 테이블에서 그 아이템이 최종 삭제처리되는데
		삭제전에 구매자가 그 아이템을 바로 포인트거래 등록을 하려고 하니, 디비 테이블 고유값 위반으로 삽입이 되지않는 것이다.
		그래서 처음에 포인트 거래 등록할 때 키값을 신규로 생성해서 등록하도록 한다.
		키값 그대로 사용하는 방법으로 처리하려면 포인트거래 등록할 때 아이템이 있는지 확인하는 과정이 필요하고, 그런 과정이 있다고해도
		내가 이미 아이템을 구매했는데, 이전 판매자가 뒤처리를 해줘야 내가 팔 수 있다는 것이 웃기게 된다.
		그래서 신규 키값으로 포인트 거래 등록하는 방식으로 변경한다.

		20160826 khkim
		ConsignmentSale 테이블에 칼럼을 추가하고(우편에 아이템 변환 관련 칼럼 추가한 것처럼) 기본값은 0 변환이 필요한 것은 1이렇게 해서
		1인 것들의 아이템은 예전 아이템이기 때문에 그 테이블의 ItemGuid 를 아이템 키값으로 넣어주고, 아이템 상태도 insert 로 해야 할듯
		포인트거래 등록할때는 아이템 키값으로 등록하지 말고 - 구매 후 바로 등록 안되는 문제로 - 신규 키값으로 등록
		아이템 구매했을 때 예전에는 키값을 포인트거래 키값으로 아이템에다가 넣어줬는데, 이렇게 하니 업데이트 에러나면서 아이템이 사라짐
		아이템은 판매자가 올릴 때 아이템이니까 아이템 상태는 업데이트일테고, 거기에 키값만 변경해서 넣어주니 당연히 아이템이 없으니 업데이트가 안됨
		*/
        saleItem.SetGUID( guid );

        saleItem.llPrice = RecvData.m_llPrice;

        CTime TimeTemp = CTime::GetCurrentTime() + CTimeSpan( 0, wPeriod, 0, 5 );
        saleItem.ExpirationDate = TimeTemp.GetTime();

        saleItem.SellType = RecvData.m_bySellType;
        saleItem.sItemcustom = sItemCustom;

        saleItem.wTurnNum = RecvData.m_wTurnNum;
        saleItem.wTotalTurnNum = RecvData.m_wTurnNum;
		saleItem.ServerGroup = m_pGaeaServer->GetServerGroup();
    }

    m_pGaeaServer->AddGameAdoJob( 
        db::DbActionPtr( new db::ConsignmentSaleCheck( saleItem, private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee(), dwFee ) ) );

    // NET_CONSIGNMENT_ADD_ITEM_ACK SendData 메시지 팩
    SendData.bAddSucess = true;
    msgpack::pack( PackBuffer, SendData );

    m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );

    return true;
}

bool GLConsignmentSaleField::ReqItemSendBack( NET_MSG_GENERIC* nmg, GLChar* pChar )
{
    if ( pChar == NULL )
        return false;

    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return false;

    // 조작패킷인가?
    if ( pChar->CharDbNum() !=  RecvData.dwChaDbNum )
        return false;

	RecvData.nServerGroup = m_pGaeaServer->GetServerGroup();

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, RecvData );
    m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ, PackBuffer );

    return true;
}

bool GLConsignmentSaleField::AckItemSendBack( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return false;

	SPOSTINFO PostInfo;
	GetPostInfo( RecvData.dwChaDbNum, RecvData.dwChaDbNum, PostInfo );

	sc::MGUID Guid;
	char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
	StringCchCopy(szGuid, sc::string::UUID_LENGTH_BRACES, RecvData.strGUID.c_str());
	sc::string::stringToUUID(szGuid, Guid);


	// 실패(eNotExistSaleItem) 했다면 아이템이 모두 팔린것이고
	// 성공(eSuccess) 했다면 아이템이 캐쉬에서 정상처리 된것이다.
	
	// 그 이외에는 에러다.
	if ( RecvData.nResult==private_market::eNotExistSaleItem 
		|| RecvData.nResult==private_market::eSuccess )
	{
		m_pGaeaServer->AddGameAdoJob( 
			db::DbActionPtr( new db::ConsignmentSaleSendBack( Guid, PostInfo, private_market::GLPrivateMarketDefine::Instance()->GetSaleCommission() )) );
	}

	return false;
}


bool GLConsignmentSaleField::AckItemSendBackDB( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return false;

    GLChar* pChar = m_pGaeaServer->GetCharByDbNum( RecvData.SaleItem.dwChaDbNum );

    private_market::ConsignmentSaleItem& SaleItem = RecvData.SaleItem;
    if( pChar == NULL )
    {
        // 없으면 안된다. 이전 프레임에 검사 했기때문에 이런경우 수동으로 캐릭터를 복구해줘야한다.
        std::string strTemp = sc::string::format("Critical Error!!! AckItemSendBackDB() ChrNum:%1% ItemGenNum%2% ItemID:%3%_%4% TurnNum:%5%/%6% Price:%7%"
            , SaleItem.dwChaDbNum
            , 0//SaleItem.sItemcustom.GenNum()
            , SaleItem.sItemcustom.GetNativeID().wMainID
            , SaleItem.sItemcustom.GetNativeID().wSubID
            , SaleItem.wTurnNum
            , SaleItem.wTotalTurnNum
            , SaleItem.llPrice );
        sc::writeLogError( strTemp );

        return false;
    }

    SPOSTINFO PostInfo;
    GetPostInfo( RecvData.SaleItem.dwChaDbNum, SaleItem.dwChaDbNum, PostInfo );
    //////////////////////////////////////////////////////////////////////////
    // ACK 받아서 서버에서 처리 해야한다.
    // 돈을 받는다.    
    WORD wSaleTurnCount = SaleItem.wTotalTurnNum - SaleItem.sItemcustom.wTurnNum;

	LONGLONG llFeeTemp = 0;	// 로그 기록에 쓴다.
    if( 0 < wSaleTurnCount )
    {
        __int64 llNowPoint = pChar->PointShopPoint();

        // 판매 금액 = 판매한 숫자 * 판매가격
        __int64 nTotalPrice = wSaleTurnCount * SaleItem.llPrice;
        
        // 판매 금액 - 수수료
        double dFee = private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee();
        

        // 계산을 위해서 10000을 곱한다.
        static const unsigned int n10000 = 10000;

        __int64 nFee = static_cast<int>( dFee*n10000 ); // 1000
        __int64 llAddPoint = static_cast<__int64>( nTotalPrice * ( n10000 - nFee ) / n10000 );

		llFeeTemp = nTotalPrice-llAddPoint;

        switch( SaleItem.SellType ) 
        {
        case private_market::eSellType_ConsignmentSale_RanPoint:
            {
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 3 ) 판매 가격
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 4 ) 수수료
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 1 ) 이전 란 포인트
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 3 ) 현재 란 포인트
                std::string strTemp;
                //strTemp += "위탁 판매 내역\r\n\r\n"; //sc::string::format( "포인트샵 거래완료\r\n\r\n" );
                strTemp += sc::string::format( "%s : %s\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 3 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( nTotalPrice, 3, "," ) );
                strTemp += sc::string::format( "%s : %s\r\n\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 4 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llFeeTemp, 3, "," ) );
                strTemp += sc::string::format( "%s : %s\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 1 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llNowPoint, 3, "," ) );
                strTemp += sc::string::format( "%s : %s\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 2 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llNowPoint+llAddPoint, 3, "," ) );
                
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 0 ) 위탁판매 시스템
                GetPostInfo( 
                    ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 0 ),
                    strTemp,
                    ID2SERVERTEXT( "PRIVATEMARKET_ROLLBACK_POST_SENDERNAME" ),
                    pChar->GetName(),
                    0/*RecvData.SaleItem.dwChaDbNum*/,
                    pChar->CharDbNum(),
                    0,
                    0,
                    PostInfo );

                pChar->PointShopPointAdd( 0, llAddPoint, pointshop::CONSIGNMENT_SALE );
            }
            break;
        case private_market::eSellType_ConsignmentSale_Gold:
            {
                std::string strTemp;
                strTemp += sc::string::format( "%s : %s\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 3 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( nTotalPrice, 3, "," ) );
                strTemp += sc::string::format( "%s : %s\r\n\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 4 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llFeeTemp, 3, "," ) );

                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 0 ) 위탁판매 시스템
                GetPostInfo( 
                    ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 0 ),
                    strTemp,
                    ID2SERVERTEXT( "PRIVATEMARKET_ROLLBACK_POST_SENDERNAME" ),
                    pChar->GetName(),
                    0/*RecvData.SaleItem.dwChaDbNum*/,
                    pChar->CharDbNum(),
                    0,
                    0,
                    PostInfo );

                PostInfo.byPostType       = EMPOST_MONEY;   // 편지 타입
                PostInfo.llAttachMoney    = llAddPoint;    
            }
            break;
        }

        
    }

    // 아이템이 다 팔리지 않고 남아 있다면 아이템과 함께 전송한다.
    if( 0 < SaleItem.sItemcustom.wTurnNum )
    {
        // 게임머니 거래인가?
        bool bIsGameMoney = false;

        std::vector<private_market::GameMoneyItemFee> vecMoneyPeriodFee;
        vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();
        for( unsigned int i=0; i<vecMoneyPeriodFee.size(); i++ )
        {
            const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[i];

            if( value.sNativeID == SaleItem.sItemcustom.GetNativeID() )
            {
                bIsGameMoney = true;
                break;
            }
        }

        if( bIsGameMoney )
        {
            // 존재하는 아이템인지 확인
            const SITEM* pITEM = GLogicData::GetInstance().GetItem( SaleItem.sItemcustom.GetNativeID() );
            if ( !pITEM )
                return false;

            LONGLONG llGameMoney = pITEM->sBasicOp.dwSellPrice * SaleItem.sItemcustom.wTurnNum;

            PostInfo.bAttach01        = false;
            PostInfo.byPostType       = EMPOST_MONEY;   // 편지 타입
            PostInfo.llAttachMoney    = llGameMoney;      


			// 통계 및 로그 수정
			//pChar->LogMoneyExchange( gamelog::ID_CHAR, 0, gamelog::ID_CHAR, pChar->m_CharDbNum, llGameMoney, ITEM_ROUTE_CONSIGMENT_SENDBACK, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap() );
			//pChar->LogMoneyExchange( gamelog::ID_CHAR, 0, gamelog::ID_CHAR, pChar->m_CharDbNum, pChar->GetInvenMoney(), ITEM_ROUTE_CONSIGMENT_SENDBACK, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap() );
        }
        else
        {
            // 골드로 거래 했다면 돈을 담는다.
            if (SaleItem.SellType==private_market::eSellType_ConsignmentSale_Gold)
                PostInfo.byPostType       = EMPOST_MONEYITEM;   // 편지 타입

            // 아이템을 담는다.
            PostInfo.bAttach01 = true;
        }


		PostInfo.dwUserNum = 0;

        // 삭제(수정) 성공하고 아이템을 우편으로 보낸다.
        m_pGaeaServer->AddGameAdoJob(
            db::DbActionPtr(
            new CPostSendSystemMail( PostInfo, SaleItem.sItemcustom )));
    }
    else
    {
		PostInfo.dwUserNum = 0;

        // 삭제(수정) 성공하고 아이템을 우편으로 보낸다.
        m_pGaeaServer->AddGameAdoJob(
            db::DbActionPtr(
            new CPostSendSystemMail( PostInfo, SITEMCUSTOM() )));
    }

	if( pChar )
	{
		// 통계 및 로그 수정
		pChar->LogItemExchange(
			SaleItem.sItemcustom, 
			gamelog::ID_CHAR, pChar->CharDbNum(),
			gamelog::ID_CHAR, pChar->CharDbNum(),
			ITEM_ROUTE_CONSIGMENT_SENDBACK, 
			SaleItem.wTurnNum,
			( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_CONSIGEMENT_SENDBACK,
			true );

	}

    GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK SendData;
    {
        SendData.strGUID = SaleItem.GetStrGUID();
        SendData.dwChaDbNum = SaleItem.dwChaDbNum;
		SendData.nResult = private_market::eSuccess;
    }
    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    m_pGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK, PackBuffer );


	m_pGaeaServer->AddGameAdoJob(
		db::DbActionPtr(
		new db::ConsignmentSaleLogInsert( SaleItem.sItemcustom.guid, SaleItem, llFeeTemp, private_market::STATE_RETURN )));

    return true;
}

// 포인트거래 아이템 구매
// 구매자가 있는 필드서버로 와서 구매 처리한다.
// 일단, 게임머니 혹은 포인트를 차감하고, 필요한 데이터를 디비액션으로 전달해서 처리한다.
bool GLConsignmentSaleField::ReqItemPurchase( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
    GLMSG::NET_CONSIGNMENT_SALE_PURCHAS_REQ RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return false;

    // 구매자 검색
    GLChar* pChar = m_pGaeaServer->GetCharByDbNum( RecvData.dwBuyerDbNum );
    if ( pChar == NULL )
    {
        // 캐릭터가 없다.
		// 통합캐시서버에 결과 전송해서 홀드 취소시킨다.
		GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
		SendData.strGUID = RecvData.strGUID;

		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );
		m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

        return false;
    }

    switch( RecvData.nSellType )
    {
    case private_market::eSellType_ConsignmentSale_RanPoint:
        if ( pChar->PointShopPoint() < RecvData.llItemPriceTotal )
        {
			// 캐쉬 서버에게 중단됨을 알림
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			SendData.strGUID = RecvData.strGUID;
			
			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

            return false;
        }

        // 포인트 차감
		pChar->PointShopPointSub( RecvData.llItemPriceTotal, pointshop::CONSIGNMENT_SALE,  pointshop::POINT_SUB_TYPE_CINSIGNMENT_BUY );

		// 로그는 ConsignmentSalePurchase 안에서 처리된다. 판매자/구매자 서버그룹 확인이 ConsignmentSalePurchase 안에서 가능하기 때문

        break;
    case private_market::eSellType_ConsignmentSale_Gold:
        if ( pChar->GetInvenMoney() < RecvData.llItemPriceTotal )
        {
            // 돈 부족
			// 캐쉬 서버에게 중단됨을 알림
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			SendData.strGUID = RecvData.strGUID;

			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

            return false;
        }

        pChar->SubtractInvenMoneyAndUpdate( RecvData.llItemPriceTotal, SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY );
        
		// 로그는 ConsignmentSalePurchase 안에서 처리된다. 판매자/구매자 서버그룹 확인이 ConsignmentSalePurchase 안에서 가능하기 때문
        
		break;
    }

    // DB 처리후에 로그를 남기고 결과를 캐쉬로 전송한다.
    // Action에 모두 포함되어있다.
    sc::MGUID Guid;
    char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
    StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, RecvData.strGUID.c_str() );
    sc::string::stringToUUID( szGuid, Guid );

    // 우편 발송에 필요한 최소 정보
    SPOSTINFO PostInfo;
    GetPostInfo( RecvData.dwSellerDbNum, RecvData.dwBuyerDbNum, PostInfo );

	GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_FB MsgFB;
	MsgFB.strBuyerName = RecvData.strBuyerName;
	MsgFB.dwBuyerDbNum = RecvData.dwBuyerDbNum;
	MsgFB.strSellerName = RecvData.strSellerName;
	MsgFB.dwSellerDbNum = RecvData.dwSellerDbNum;
	MsgFB.strGUID = RecvData.strGUID;
	MsgFB.nPurchaseCount = RecvData.nPurchaseCount;
	MsgFB.llItemPriceTotal = RecvData.llItemPriceTotal;
	MsgFB.nSellType = RecvData.nSellType;

    m_pGaeaServer->AddGameAdoJob( 
        db::DbActionPtr( 
        new db::ConsignmentSalePurchase( Guid, RecvData.nPurchaseCount, PostInfo, MsgFB ) ) );
   
    return true;
}

bool GLConsignmentSaleField::ReqItemPayMoney( NET_MSG_GENERIC* nmg )
{
	/*
	// 구매자가 있는 필드서버이다.
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
	GLMSG::NET_CONSIGNMENT_SALE_PAY_MONEY RecvData;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
		return false;

	// 구매자 검색
	GLChar* pChar = m_pGaeaServer->GetCharByDbNum( RecvData.dwBuyerDbNum );
	if ( pChar == NULL )
	{
		// 구매중 취소
		// 홀드 취소
		// 캐쉬 서버에게 중단됨을 알림
		GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
		SendData.strGUID = RecvData.strGUID;

		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );
		m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );
		return false;
	}

	// 캐쉬에서 오는패킷이다
	switch( RecvData.nSellType )
	{
	case private_market::eSellType_ConsignmentSale_RanPoint:
		if ( pChar->PointShopPoint() < RecvData.llItemPriceTotal )
		{
			// 캐쉬 서버에게 중단됨을 알림
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			SendData.strGUID = RecvData.strGUID;

			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

			return FALSE;
		}

		//포인트 차감
		pChar->PointShopPointSub( RecvData.llItemPriceTotal, pointshop::CONSIGNMENT_SALE,  pointshop::POINT_SUB_TYPE_CINSIGNMENT_BUY );

		break;
	case private_market::eSellType_ConsignmentSale_Gold:
		if ( pChar->GetInvenMoney() < RecvData.llItemPriceTotal )
		{
			// 돈 부족
			// 캐쉬 서버에게 중단됨을 알림
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			SendData.strGUID = RecvData.strGUID;

			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

			return FALSE;
		}

		pChar->SubtractInvenMoneyAndUpdate( RecvData.llItemPriceTotal, SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY );

		//	Note : 돈 기록.
		// LogMoneyExchange 해야함

		break;
	}

	// 캐시에 결과 전송
	GLMSG::NET_CONSIGNMENT_SALE_PAY_MONEY_ACK SendData;

	SendData.strGUID = RecvData.strGUID;
	SendData.strBuyerName = RecvData.strBuyerName;
	SendData.dwBuyerDbNum = RecvData.dwBuyerDbNum;
	SendData.strSellerName = RecvData.strSellerName;
	SendData.dwSellerDbNum = RecvData.dwSellerDbNum;
	SendData.nPurchaseCount = RecvData.nPurchaseCount;
	SendData.llItemPriceTotal = RecvData.llItemPriceTotal;
	SendData.nSellType = RecvData.nSellType;
	SendData.bSameServerGroup = RecvData.bSameServerGroup;
	SendData.dwBuyerFieldSlot = RecvData.dwBuyerFieldSlot;

	msgpack::sbuffer PackBuffer;
	msgpack::pack( PackBuffer, SendData );
	m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );
	*/
	return true;
}

bool GLConsignmentSaleField::ReqSendPostAndLog( NET_MSG_GENERIC* nmg )
{
	/*
	// 구매자가 있는 필드서버이다.
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
	GLMSG::NET_CONSIGNMENT_SALE_SEND_POST_REQ RecvData;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
		return false;

	// DB 처리후에 로그를 남기고 결과를 캐쉬로 전송한다.
	// Action에 모두 포함되어있다.
	sc::MGUID Guid;
	char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
	StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, RecvData.strGUID.c_str() );
	sc::string::stringToUUID( szGuid, Guid );

	// 우편 발송에 필요한 최소 정보
	SPOSTINFO PostInfo;
	GetPostInfo( RecvData.dwSellerDbNum, RecvData.dwBuyerDbNum, PostInfo );

	m_pGaeaServer->AddGameAdoJob( 
		db::DbActionPtr( 
		new db::ConsignmentSalePurchase( Guid, RecvData.nPurchaseCount, PostInfo ) ) );
	*/
	return true;
}

bool GLConsignmentSaleField::AckItemPurchaseDB( NET_MSG_GENERIC* nmg )
{
	// 아이템 구매 처리 도중 에러가 발생한 경우
	// 캐릭터가 있으면 구매대금 돌려주고, 캐시에는 구매 취소 처리
	// 캐릭터가 없다면 우편으로 돌려주고, 캐시에는 구매 취소 처리
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
	GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_FB RecvData;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
		return false;

	// 구매자 검색
	GLChar* pChar = m_pGaeaServer->GetCharByDbNum( RecvData.dwBuyerDbNum );
	if ( pChar )
	{
		switch( RecvData.nSellType )
		{
		case private_market::eSellType_ConsignmentSale_RanPoint:
			pChar->PointShopPointAdd( 0, RecvData.llItemPriceTotal, pointshop::CONSIGNMENT_SALE_ERROR );
			break;
		case private_market::eSellType_ConsignmentSale_Gold:
			pChar->AddInvenMoneyAndUpdate( RecvData.llItemPriceTotal, ADD_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY_ERROR );
			break;
		}
	}
	else
	{
		// 우편 전송 코드 추가
		// 우선 로그만 남긴다.
		switch( RecvData.nSellType )
		{
		case private_market::eSellType_ConsignmentSale_RanPoint:
			{
				sc::writeLogError( sc::string::format( 
					"AckItemPurchaseDB pChar is null chanum %1% point %2%", 
					RecvData.dwBuyerDbNum, RecvData.llItemPriceTotal ) );
			}
			break;
		case private_market::eSellType_ConsignmentSale_Gold:
			{
				sc::writeLogError( sc::string::format( 
					"AckItemPurchaseDB pChar is null chanum %1% gold %2%", 
					RecvData.dwBuyerDbNum, RecvData.llItemPriceTotal ) );
			}
			break;
		}
	}

	GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
	SendData.strGUID = RecvData.strGUID;

	msgpack::sbuffer PackBuffer;
	msgpack::pack( PackBuffer, SendData );
	m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

	return true;
}

void GLConsignmentSaleField::GetPostInfo( DWORD dwSender, DWORD dwRecver, SPOSTINFO& PostInfo OUT )
{
	/*
    GLChar* pCharSender = m_pGaeaServer->GetCharByDbNum( dwSender );
    GLChar* pCharRecver = m_pGaeaServer->GetCharByDbNum( dwRecver );

    std::string strSender, strRecver;

    DWORD dwSendUserNum = 0, dwRecvUserNum = 0;
    
    if ( pCharSender )
    {
        strSender = pCharSender->GetName();
        dwSendUserNum = pCharSender->GetUserID();
    }
    else
    {
        strSender =  ID2SERVERTEXT( "PRIVATEMARKET_ROLLBACK_POST_SENDERNAME" );
    }
	*/

	GLChar* pCharRecver = m_pGaeaServer->GetCharByDbNum( dwRecver );

	std::string strSender, strRecver;

	DWORD dwSendUserNum = 0, dwRecvUserNum = 0;

	strSender =  ID2SERVERTEXT( "PRIVATEMARKET_ROLLBACK_POST_SENDERNAME" );

	// 구매자는 무조건 있어야 한다.
    if ( pCharRecver )
    {
        strRecver = pCharRecver->GetName();
        dwRecvUserNum = 0;	// 우편에 문제가 있었다. 유저넘을 넣으면 안된다.
    }
    else
    {
        strSender =  ID2SERVERTEXT( "PRIVATEMARKET_ROLLBACK_POST_SENDERNAME" );
    }

    GetPostInfo( 
        ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 0 )
        , sc::string::format( ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 5 ), ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 0 ) )
        , strSender, strRecver
        , 0/*dwSender*/, dwRecver
        , dwSendUserNum, dwRecvUserNum
        , PostInfo );
}

void GLConsignmentSaleField::GetPostInfo( 
    std::string     PostTitle,
    std::string     PostContent,
    std::string     SendChaName,
    std::string     RecvChaName,
    DWORD           SendChaNum,
    DWORD           RecvChaNUm,
    DWORD           SendUserNum,
    DWORD           RecvUserNum,
    SPOSTINFO&      PostInfo OUT )
{
    // 우편 발송에 필요한 최소 정보
    ZeroMemory( &PostInfo, sizeof( SPOSTINFO ) );
    {		
        PostInfo.iDesign        = ATTENDANCE_REWARD_POST_DESIGN;    // 편지지 디자인

        PostInfo.byPostClass    = EMPOST_SYSTEM;
        PostInfo.byPostState    = EMPOST_UNREAD;                    // 편지 상태
        PostInfo.byPostType 	= EMPOST_ITEM;                      // 편지 타입

        StringCchCopy( PostInfo.Title, sizeof( PostInfo.Title ), PostTitle.c_str() );
        StringCchCopy( PostInfo.Content, sizeof( PostInfo.Content ), PostContent.c_str() );

        PostInfo.llAttachMoney  = 0;                                // 첨부 골드
        PostInfo.llCharge       = 0;                                // 청구 골드

        PostInfo.iSender        = SendChaNum;                       // 보낸 사람
        PostInfo.iRecver        = RecvChaNUm;                       // 받는 사람

        PostInfo.dwUserNum      = SendUserNum;
        PostInfo.nUserNumRecver = RecvUserNum;

        StringCchCopy( PostInfo.SenderName, sizeof( PostInfo.SenderName ), SendChaName.c_str() );
        StringCchCopy( PostInfo.RecverName, sizeof( PostInfo.RecverName ), RecvChaName.c_str() );
    }
}
