#include "pch.h"

#include "../../../SigmaCore/Util/DateTime.h"

#include "../../../RanLogic/Msg/ConsignmentSaleMsg.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../Server/CacheServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../Util/GLItemLimit.h"

#include "../../FieldServer/GLGaeaServer.h"

#include "../DbManager.h"
#include "DbActionGameConsignmentSale.h"



// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



namespace db
{

int ConsignmentSaleCheck::Execute( NetServer* pServer )
{
    CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
    if ( !pFieldServer )
        return sc::db::DB_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if ( !pGaeaServer || !m_pSubDbManager )
        return sc::db::DB_ERROR;

    if ( m_pSubDbManager->ConsignmentsSaleInsert( m_SaleItem ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    GLMSG::NET_CONSIGNMENT_ITEM_REGIST_REQ SendData;
    {
        SendData.SaleItem = m_SaleItem;
        SendData.fCommissionRate = m_fCommissionRate;
    }
    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_ITEM_REGIST_REQ, PackBuffer );

	// LOG 기록
	CTime timeTemp = m_SaleItem.ExpirationDate - CTime::GetCurrentTime().GetTime();
	m_pSubDbManager->ConsignmentSaleLogInsert(
		m_SaleItem.sItemcustom.guid,
		m_SaleItem.dwChaDbNum,
		m_SaleItem.SellType,
		private_market::STATE_REGIST,
		m_SaleItem.llPrice,
		m_dwFee,	//  등록시 포인트 비용
		m_SaleItem.sItemcustom.Mid(),
		m_SaleItem.sItemcustom.Sid(),
		0, 
		m_SaleItem.wTotalTurnNum,
		pFieldServer->GetServerGroup()/*m_SaleItem.sItemcustom.wRanPointTradeCount*/,
		timeTemp.GetTime() / 3600  );

    return sc::db::DB_OK;
}

int ConsignmentSaleSendBack::Execute( NetServer* pServer )
{
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return sc::db::DB_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer && !m_pSubDbManager )
        return sc::db::DB_ERROR;

    // 유저가 없으면 안된다.
    GLChar* pChar = pFieldServer->GetGLGaeaServer()->GetCharByDbNum( m_PostInfo.iRecver );
    if( pChar == NULL )
        return sc::db::DB_ERROR;


    //아이템을 갖고온다.
    private_market::ConsignmentSaleItem SaleItem;
    if( m_pSubDbManager->ConsignmentsSaleGet( m_Guid, SaleItem ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    // 아이템이 없을 수 없다.
    if( SaleItem.sItemcustom.GetNativeID() == NATIVEID_NULL() )
    {	
        sc::writeLogError( sc::string::format("ConsignmentSaleSendBack::Execute() NATIVEID_NULL! ChaNum:%1%, ITEMGUID:%2%", SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ));        
        return sc::db::DB_ERROR;
    }

    // 아이템 갯수는 무조껀 컬럼의 정보가 진짜( 내부의 바이너리는 일일이 수정하지 않기 때문 )
    SaleItem.sItemcustom.wTurnNum = SaleItem.wTurnNum;

    // 더이상 DB에 남아 있을 필요가 없다.
    if( m_pSubDbManager->ConsignmentsSaleDelete( m_Guid ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

	// 포인트거래 아이템 가져오는 스크립트 수정하지 말고, 포인트거래 올리는 코드에는 guid null 체크 기능이 있으니, 믿고
	// 아이템 상태가 none 이고 guid 가 guid null 이면 서버 코드에서 아이템 guid 를 생성, 아이템 변환전이기 때문에 아이템상태도 NONE 일거다. 기본값
	// 구매하는 코드에는 아래 코드가 추가되었으나, 아이템 변환전 데이터이고, 판매기간이 지난 아이템 가져오는 부분이 여기이고
	// 실제 아이템을 가여오면 우편으로 오게되는데 우편 첨부아이템을 받고 인벤에 삽입할 때 sp_ItemCreate 에러 발생함
	// 실제 guid 가 null 이고 아이템 상태는 db::DB_NONE 이다.
	if ( SaleItem.sItemcustom.IS_GUID_NULL() && db::DB_NONE == SaleItem.sItemcustom.GetDbState() )
	{
		sc::SeqUniqueGUID::Instance()->getGUID( SaleItem.sItemcustom.guid );
		SaleItem.sItemcustom.SetDbState( db::DB_INSERT, true );
		sc::writeLogInfo( sc::string::format( "ConsignmentSaleSendBack, old item structure sale guid is %1%", sc::string::uuidToString( SaleItem.GetGUID() ).c_str() ) );
	}

	// [2/20/2017 khkim]
	// 서버간 아이템이 이동되어도 guid 가 변경되지 않도록 변경하면서, 개인상점검색에서 거래되는 아이템은 디비에서 삭제되도록
	// 변경되었다.(포인트거래일 경우 등록할 때 삭제된다.) 등록취소를 하는 것이지만 아이템을 신규로 넣어주는 방식으로 변경
	// 이렇게 기능이 변경되면, 기존에 이미 등록되어 있던 아이템의 경우 따로 처리를 해주어야 한다. (GmCharEdit 로 작업 필요)
	// GmEdit 작업 따로 하지않고, 포인트거래 등록되어 있는 아이템들 모두 expire 처리한다. 
	// [ExpirationDate] = '1970-01-01 09:00:00.000' 으로 모두 업데이트 했다.
	// '1999-12-31 00:00:00.000' 는 __time64_t 로 변경하면 0 이다.
	// 이 업데이트전에 이미 등록되어 있던 아이템들은 만료처리가 되었기 때문에 포인트거래ui에서 찾아갈 수 밖에 없다. 이런 아이템들은
	// db::DB_INSERT 로 변경하지 않는다.
	if ( SaleItem.ExpirationDate != 0 )
	{
		SaleItem.sItemcustom.SetDbState( db::DB_INSERT, true );
	}

    GLMSG::NET_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB SendData;
    {
        SendData.SaleItem = SaleItem;
    }
    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    
    NET_MSG_PACK msg;
    if (msg.SetData(NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB, PackBuffer))
        pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );
        
    return sc::db::DB_OK;

}

int ConsignmentSalePurchase::Execute( NetServer* pServer )
{
    CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
    if ( !pFieldServer )
        return sc::db::DB_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if ( !pGaeaServer || !m_pSubDbManager )
        return sc::db::DB_ERROR;

    // 구매자가 없으면 안된다.
    GLChar* pChar = pGaeaServer->GetCharByDbNum( m_PostInfo.iRecver );
    if ( pChar == NULL )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error pChar == NULL buyer: %1%", 
			m_PostInfo.iRecver ) );

        return sc::db::DB_ERROR;
	}

    // 아이템을 갖고온다.
    private_market::ConsignmentSaleItem SaleItem;
    if ( m_pSubDbManager->ConsignmentsSaleGet( m_Guid, SaleItem ) == sc::db::DB_ERROR )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error ConsignmentsSaleGet fail buyer: %1% seller: %2% guid: %3%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );

        return sc::db::DB_ERROR;
	}

    // 아이템이 없을 수 없다.
    if ( SaleItem.sItemcustom.GetNativeID() == NATIVEID_NULL() )
    {
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error SaleItem.sItemcustom.GetNativeID() == NATIVEID_NULL() buyer: %1% seller: %2% guid: %3%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );

        return sc::db::DB_ERROR;
    }

    // 존재하는 아이템인지 확인
    const SITEM* pITEM = GLogicData::GetInstance().GetItem( SaleItem.sItemcustom.GetNativeID() );
    if ( !pITEM )
    {
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error !pITEM buyer: %1% seller: %2% guid: %3% mid: %4% sid: %5%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.GetNativeID().wMainID, SaleItem.sItemcustom.GetNativeID().wSubID ) );

        return sc::db::DB_ERROR;
    }

	// 구매할 때 소모한 비용 로그
	// ReqItemPurchase 에서 비용이 선처리되었고, 여기에서 로그 처리
	EMITEM_ROUTE ItemRoute = ITEM_ROUTE_CONSIGMENT_SALE;
	bool bServerIsDifferent = false;
	if ( SaleItem.ServerGroup != static_cast< int >( pFieldServer->GetServerGroup() ) )
	{
		bServerIsDifferent = true;
		ItemRoute = ITEM_ROUTE_CONSIGMENT_SALE_SERVERGROUP_DIFF;
	}

	switch ( m_MsgFB.nSellType )
	{
	case private_market::eSellType_ConsignmentSale_RanPoint:
		{
			pChar->LogMoneyExchangeWithItemGuid( 
				gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, 
				-m_MsgFB.llItemPriceTotal, 
				ItemRoute, 
				pChar->GetCurrentMap(), 
				LOG_RANPOINT_MINUS_BY_CONSIGEMENT, 
				SaleItem.sItemcustom.guid );
		}
		break;
	case private_market::eSellType_ConsignmentSale_Gold:
		{
			pChar->LogMoneyExchangeWithItemGuid( 
				gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, 
				-m_MsgFB.llItemPriceTotal, 
				ItemRoute, 
				pChar->GetCurrentMap(), 
				LOG_MONEY_MINUS_BY_CONSIGEMENT, 
				SaleItem.sItemcustom.guid );
		}
		break;
	}

	// 아이템 수량체크
	if ( SaleItem.sItemcustom.wTurnNum < m_nPurchaseNumber )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error SaleItem.sItemcustom.wTurnNum < m_nPurchaseNumber buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );

		return sc::db::DB_ERROR;
	}

    // 아이템 갯수는 무조껀 컬럼의 정보가 진짜( 내부의 바이너리는 일일이 수정하지 않기 때문 )
    SaleItem.sItemcustom.wTurnNum =  SaleItem.wTurnNum;

    // 포인트 거래 횟수를 증가 시킨다.
    SaleItem.sItemcustom.wRanPointTradeCount++;

    //여기사 -가 될순 없지만 0이 될순 있다.
    if ( m_pSubDbManager->ConsignmentsUpdateTurnNum( m_Guid, SaleItem.sItemcustom.wTurnNum - m_nPurchaseNumber ) == sc::db::DB_ERROR )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, m_MsgFB );

		NET_MSG_PACK msg;
		if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
			pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

		sc::writeLogError( sc::string::format( 
			"ConsignmentSalePurchase::Execute error ConsignmentsUpdateTurnNum fail buyer: %1% seller: %2% guid: %3%", 
			m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );

        return sc::db::DB_ERROR;
	}

    // 게임머니 거래인가?
    bool bIsGameMoney = false;

    std::vector< private_market::GameMoneyItemFee > vecMoneyPeriodFee;
    vecMoneyPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();
    for( unsigned int i=0; i<vecMoneyPeriodFee.size(); i++ )
    {
        const private_market::GameMoneyItemFee& value = vecMoneyPeriodFee[i];

        if ( value.sNativeID == SaleItem.sItemcustom.GetNativeID() )
        {
            bIsGameMoney = true;
            break;
        }
    }

    // 게임머니를 구매한경우 돈을 우편으로 첨부한다.
    if ( bIsGameMoney )
    {
		// 무조건 게임머니 구입은 1이라고 가정하고 코드가 되어 있어서 그에 맞추어서 수정함
        LONGLONG llGameMoney = pITEM->sBasicOp.dwSellPrice * m_nPurchaseNumber;

        m_PostInfo.bAttach01        = false;
        m_PostInfo.byPostType       = EMPOST_MONEY;                      // 편지 타입
        m_PostInfo.llAttachMoney    = llGameMoney;      
        
        if ( m_pSubDbManager->PostSend( m_PostInfo, SITEMCUSTOM() ) == sc::db::DB_ERROR )
        {
			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, m_MsgFB );

			NET_MSG_PACK msg;
			if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
				pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

			sc::writeLogError( sc::string::format( 
				"ConsignmentSalePurchase::Execute error PostSend(gamemoney) fail buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
				m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );

			// 위에서 개수 업데이트한거 롤백
			if ( m_pSubDbManager->ConsignmentsUpdateTurnNum( m_Guid, SaleItem.sItemcustom.wTurnNum ) == sc::db::DB_ERROR )
			{
				sc::writeLogError( sc::string::format( 
					"ConsignmentSalePurchase::Execute error updateturnnum fail buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
					m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );
			}

            return sc::db::DB_ERROR;
        }

		pChar->LogMoneyExchangeWithItemGuid( 
			gamelog::ID_CHAR, 0, gamelog::ID_CHAR, pChar->m_CharDbNum, 
			llGameMoney, 
			ItemRoute, 
			pChar->GetCurrentMap(), 
			LOG_MONEY_CONSIGEMENT_SALE_MONEY_BUY_SENT_POST, 
			SaleItem.sItemcustom.guid );
    }
    else
    {
        m_PostInfo.bAttach01 = true;

        // 아이템 전송
        SITEMCUSTOM SendItemCustom = SaleItem.sItemcustom;
        SendItemCustom.wTurnNum = m_nPurchaseNumber;

		// 아이템 등록할 때 원래 아이템 guid 를 사용하도록 변경되었으나, 아이템 구조 변경전에
		// 등록되어 있는 아이템들 때문에(우편 테이블처럼 칼럼을 추가해서 변환 처리한다던지 하는 번거로운 작업은 하지 않음)
		// guid 를 다시 한번 넣어준다. 아이템 등록 부분 수정후부터는 불필요한 작업이지만, 예전에 등록된 아이템 때문에 필요하다.
		// 아이템이 사라지는 문제로 이 부분 주석처리함
		// 아이템 구매했을 때 예전에는 키값을 포인트거래 키값으로 아이템에다가 넣어줬는데, 이렇게 하니 업데이트 에러나면서 아이템이 사라짐
		// 아이템은 판매자가 올릴 때 아이템이니까 아이템 상태는 업데이트일테고, 거기에 키값만 변경해서 넣어주니 당연히 아이템이 없으니 업데이트가 안됨
		/*SendItemCustom.SetGuid( m_Guid );*/

		// 포인트거래 아이템 가져오는 스크립트 수정하지 말고, 포인트거래 올리는 코드에는 guid null 체크 기능이 있으니, 믿고
		// 아이템 상태가 none 이고 guid 가 guid null 이면 서버 코드에서 아이템 guid 를 생성, 아이템 변환전이기 때문에 아이템상태도 NONE 일거다. 기본값
		if ( SendItemCustom.IS_GUID_NULL() && db::DB_NONE == SendItemCustom.GetDbState() )
		{
			sc::SeqUniqueGUID::Instance()->getGUID( SendItemCustom.guid );
			SendItemCustom.SetDbState( db::DB_INSERT, true );
			sc::writeLogInfo( sc::string::format( "ConsignmentSalePurchase, old item structure sale guid is %1%", sc::string::uuidToString( SaleItem.GetGUID() ).c_str() ) );
		}
		
		if ( pITEM->ISPILE() )
		{
			// 겹침 아이템은 신규 guid 생성한다.
			sc::SeqUniqueGUID::Instance()->getGUID( SendItemCustom.guid );
			SendItemCustom.GenerateBasicStat( false );
			SendItemCustom.GenerateLinkSkill();
			SendItemCustom.GenerateAddOption();
		}

		/*
		// 서버가 다르면, 아이템 상태 변경
		if ( pGaeaServer->GetServerGroup() != SaleItem.ServerGroup )
		{
			// 현재 아이템 테이블 구조가 아이템이 삭제되어도 실제 테이블에서 삭제 시키지 않기 때문에
			// 서버를 왔다갔다 하면 고유키 때문에 삽입이 안된다. 
			// 예를 들어 1서버의 A아이템을 2서버에서 구매를 하고, 다시 A아이템을 1서버에서 구매한다면 문제가 발생함
			// 그래서 키값 신규로 생성한다.
			sc::SeqUniqueGUID::Instance()->getGUID( SendItemCustom.guid );
			SendItemCustom.SetDbState( db::DB_INSERT, true );
		}
		*/

		// 2/20/2017 khkim
		// 서버간 아이템이 이동되어도 guid 가 변경되지 않도록 변경하면서, 개인상점검색에서 거래되는 아이템은 디비에서 삭제되도록
		// 변경되었다.(포인트거래일 경우 등록할 때 삭제된다.) 등록취소를 하는 것이지만 아이템을 신규로 넣어주는 방식으로 변경
		// 이렇게 기능이 변경되면, 기존에 이미 등록되어 있던 아이템의 경우 따로 처리를 해주어야 한다. (GmCharEdit 로 작업 필요)
		SendItemCustom.SetDbState( db::DB_INSERT, true );

        // 삭제(수정) 성공하고 아이템을 우편으로 보낸다.
        if ( m_pSubDbManager->PostSend( m_PostInfo, SendItemCustom ) == sc::db::DB_ERROR )
        {
			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, m_MsgFB );

			NET_MSG_PACK msg;
			if ( msg.SetData( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, PackBuffer ) )
				pFieldServer->InsertMsg( DBACTION_CLIENT_ID, &msg );

			sc::writeLogError( sc::string::format( 
				"ConsignmentSalePurchase::Execute error PostSend(item) fail buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
				m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );

			// 위에서 개수 업데이트한거 롤백
			if ( m_pSubDbManager->ConsignmentsUpdateTurnNum( m_Guid, SaleItem.sItemcustom.wTurnNum ) == sc::db::DB_ERROR )
			{
				sc::writeLogError( sc::string::format( 
					"ConsignmentSalePurchase::Execute error updateturnnum fail buyer: %1% seller: %2% guid: %3% turnnum: %4% purchasenum: %5%", 
					m_PostInfo.iRecver, SaleItem.dwChaDbNum, SaleItem.GetStrGUID(), SaleItem.sItemcustom.wTurnNum, m_nPurchaseNumber ) );
			}

            return sc::db::DB_ERROR;
        }
    }

    // 1. Send Notify to the Recver via AgentServer
    GLMSG::SNET_POST_NOTIFY msgNotify;
    msgNotify.llPostID = m_PostInfo.llPostID;	// completed postsend
    msgNotify.iRecver  = m_PostInfo.iRecver;
    pGaeaServer->SENDTOAGENT( &msgNotify );

	if ( bServerIsDifferent )
	{
		pChar->LogItemExchange( 
			SaleItem.sItemcustom,
			gamelog::ID_CHAR,
			m_PostInfo.iSender, 
			gamelog::ID_CHAR, 
			m_PostInfo.iRecver, 
			ItemRoute,
			m_nPurchaseNumber,
			pChar->GetCurrentMap(), 
			LOG_ITEM_CONSIGEMENT_BUY_RECV_BY_POST,
			true );
	}
	else
	{
		pChar->LogItemExchange( 
			SaleItem.sItemcustom,
			gamelog::ID_CHAR,
			m_PostInfo.iSender, 
			gamelog::ID_CHAR, 
			m_PostInfo.iRecver, 
			ItemRoute,
			m_nPurchaseNumber,
			pChar->GetCurrentMap(), 
			LOG_ITEM_CONSIGEMENT_BUY_RECV_BY_POST,
			true );
	}

    // cache 에 개인상점 판매된 아이템 상태 전송
    GLMSG::SNETPC_PMARKET_ITEM_BUY_RESULT_FH NetMsgFH;
    NetMsgFH.dwItemTurnNum = SaleItem.sItemcustom.wTurnNum - m_nPurchaseNumber;
    NetMsgFH.sItemGUID = m_Guid;
    NetMsgFH.bLog = true;
    pGaeaServer->SENDTOINTEGRATIONCACHE( &NetMsgFH );

    // 판매자가 있으면 판매되었음을 알린다.
	if ( false == bServerIsDifferent )
	{
		GLChar* pSeller = pGaeaServer->GetCharByDbNum( SaleItem.dwChaDbNum );
		if ( pSeller )
		{
			// 판매자에게 판매를 알림
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			{
				SendData.strGUID = SaleItem.GetStrGUID();
				SendData.nPurchaseCount = m_nPurchaseNumber;
			}
			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			pGaeaServer->SENDTOCLIENT( pSeller->ClientSlot(), NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );
		}
	}

	// LOG 기록
	CTime timeTemp = SaleItem.ExpirationDate - CTime::GetCurrentTime().GetTime();
	m_pSubDbManager->ConsignmentSaleLogInsert( 
		SaleItem.sItemcustom.guid,
		m_PostInfo.iRecver,
		SaleItem.SellType,
		private_market::STATE_BUY,
		SaleItem.llPrice,
		0,
		SaleItem.sItemcustom.Mid(),
		SaleItem.sItemcustom.Sid(),
		0,
		static_cast< WORD >( m_nPurchaseNumber )/*SaleItem.wTotalTurnNum*/,
		pFieldServer->GetServerGroup()/*SaleItem.sItemcustom.wRanPointTradeCount*/,
		timeTemp.GetTime() / 3600  );

    return sc::db::DB_OK;
}

/*
int ConsignmentSalePurchaseForCacheServer::Execute( NetServer* pServer )
{
	CacheServer* pCacheServer = reinterpret_cast< CacheServer* >( pServer );
	if ( pCacheServer == NULL )
		return sc::db::DB_ERROR;

	GLMSG::NET_CONSIGNMENT_SALE_UPDATE_ITEM_ACK SendData;
	SendData.Result = -1;
	SendData.strBuyerName = m_BasicMsg.strBuyerName;
	SendData.dwBuyerDbNum = m_BasicMsg.dwBuyerDbNum;
	SendData.strSellerName = m_BasicMsg.strSellerName;
	SendData.dwSellerDbNum = m_BasicMsg.dwSellerDbNum;

	SendData.strGUID = m_BasicMsg.strGUID;
	SendData.nPurchaseCount = m_BasicMsg.nPurchaseCount;
	SendData.llItemPriceTotal = m_BasicMsg.llItemPriceTotal;
	SendData.nSellType = m_BasicMsg.nSellType;
	SendData.dwBuyerFieldSlot = m_BasicMsg.dwBuyerFieldSlot;

	// 아이템을 갖고온다.
	private_market::ConsignmentSaleItem SaleItem;
	if ( m_pSubDbManager->ConsignmentsSaleGet( m_Guid, SaleItem ) == sc::db::DB_ERROR )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		return sc::db::DB_ERROR;
	}

	// 아이템이 없을 수 없다.
	if ( SaleItem.sItemcustom.GetNativeID() == NATIVEID_NULL() )
	{	
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		sc::writeLogError( sc::string::format( "ConsignmentSalePurchaseForCacheServer::Execute() NATIVEID_NULL! %1%, %2%", SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );   
		return sc::db::DB_ERROR;
	}

	// 존재하는 아이템인지 확인
	const SITEM* pITEM = GLogicData::GetInstance().GetItem( SaleItem.sItemcustom.GetNativeID() );
	if ( !pITEM )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		sc::writeLogError( sc::string::format( "ConsignmentSalePurchaseForCacheServer::Execute() NATIVEID_NULL! %1%, %2%", SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );   
		return sc::db::DB_ERROR;
	}

	// 아이템 수량체크
	if ( SaleItem.sItemcustom.wTurnNum < m_nPurchaseNumber )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		return sc::db::DB_ERROR;
	}

	// 아이템 갯수는 무조껀 컬럼의 정보가 진짜( 내부의 바이너리는 일일이 수정하지 않기 때문 )
	SaleItem.sItemcustom.wTurnNum =  SaleItem.wTurnNum;

	// 포인트 거래 횟수를 증가 시킨다.
	SaleItem.sItemcustom.wRanPointTradeCount++;

	//여기사 -가 될순 없지만 0이 될순 있다.
	if ( m_pSubDbManager->ConsignmentsUpdateTurnNum( m_Guid, SaleItem.sItemcustom.wTurnNum - m_nPurchaseNumber ) == sc::db::DB_ERROR )
	{
		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );

		NET_MSG_PACK MsgPack;
		if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
			pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

		sc::writeLogError( sc::string::format( "ConsignmentSalePurchaseForCacheServer::Execute() ConsignmentsUpdateTurnNum %1%, %2%", SaleItem.dwChaDbNum, SaleItem.GetStrGUID() ) );
		return sc::db::DB_ERROR;
	}

	SendData.Result = 0;
	SendData.Attach = SaleItem.sItemcustom;
	SendData.Attach.wTurnNum = m_nPurchaseNumber;

	// 아이템 등록할 때 원래 아이템 guid 를 사용하도록 변경되었으나, 아이템 구조 변경전에
	// 등록되어 있는 아이템들 때문에(우편 테이블처럼 칼럼을 추가해서 변환 처리한다던지 하는 번거로운 작업은 하지 않음)
	// guid 를 다시 한번 넣어준다. 아이템 등록 부분 수정후부터는 불필요한 작업이지만, 예전에 등록된 아이템 때문에 필요하다.
	SendData.Attach.SetGuid( m_Guid );

	if ( pITEM->ISPILE() )
	{
		// 겹침 아이템은 신규 guid 생성한다.
		sc::SeqUniqueGUID::Instance()->getGUID( SendData.Attach.guid );
	}

	if ( !m_BasicMsg.bSameServerGroup )
	{
		SendData.Attach.SetDbState( db::DB_INSERT, true );
	}

	msgpack::sbuffer PackBuffer;
	msgpack::pack( PackBuffer, SendData );

	NET_MSG_PACK MsgPack;
	if ( MsgPack.SetData( NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK, PackBuffer ) )
		pCacheServer->InsertMsg( DBACTION_CLIENT_ID, &MsgPack );

	return sc::db::DB_OK;
}
*/

int ConsignmentSaleGetList::Execute( NetServer* pServer )
{
    if( m_pSubDbManager == FALSE )
        return sc::db::DB_ERROR;

    // 패킷 사이즈가 커진다면 양을 조절해야 한다.
    // 현재는 n개까지 한패킷에 담아서 보낼 수 있다.
	// int nBunch = 7;

	// 위처럼 고정수치로 박게되면 문제가생김;
	// -64는 보내는 패킷내에 보면 불런, 벡터값이 있기때문, 패킹도중 어떤식으로 될지 모르기때문에 64로함;
	// 벡터 기본 32, 불런4;
    int nBunch = ( (NET_DATA_BUFSIZE-64)/ sizeof ( private_market::ConsignmentSaleItem ) );

    // 유저에게 보낸다.
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if( pFieldServer == NULL )
        return sc::db::DB_ERROR;

    private_market::SALE_ITEMS_VECTOR vecSaleItems;
    if( m_pSubDbManager->ConsignmentsSaleSelect( vecSaleItems, m_dwChaDbNum, pFieldServer->GetServerGroup() ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    if( pServer == NULL )
        return sc::db::DB_ERROR;

    GLChar* pChar = pFieldServer->GetGLGaeaServer()->GetCharByDbNum( m_dwChaDbNum );
    if( pChar && vecSaleItems.empty() == false )
    {
        m_nListCount = vecSaleItems.size();

        GLMSG::NET_CONSIGNMENT_SALE_ITEM_LIST_ACK SendData;
        int nPacketCount=0;
        for( UINT i=1 ; i<=vecSaleItems.size() ; i++ )
        {            
            SendData.vecSaleItems.push_back( vecSaleItems[i-1] );

            if( i==vecSaleItems.size() )
                SendData.m_bIsLast = true;
            else if( i%nBunch==0 )
                SendData.m_bIsLast = false;
            else
                continue;

            msgpack::sbuffer PackBuffer;
            msgpack::pack( PackBuffer, SendData );
            pFieldServer->SendClient( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_ACK, PackBuffer );
            SendData.vecSaleItems.clear();
        }
    }
    
    return sc::db::DB_OK;
}

int ConsignmentSaleGetListChche::Execute( NetServer* pServer )
{
    private_market::SALE_ITEMS_VECTOR vecSaleItems;
    if( m_pSubDbManager->ConsignmentsSaleSelect( vecSaleItems ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    // 서버에게 보낸다.
    CacheServer* pCacheServer = reinterpret_cast<CacheServer*> (pServer);
    if( pCacheServer == NULL )
        return sc::db::DB_ERROR;

    if( vecSaleItems.empty() == false )
    {
        GLMSG::NET_CONSIGNMENT_ITEM_REGIST_REQ SendData;

        for( UINT i=0 ; i<vecSaleItems.size() ; i++ )
        {            
            SendData.SaleItem = vecSaleItems[i];
            SendData.fCommissionRate  = m_fCommissionRate;

            msgpack::sbuffer PackBuffer;
            msgpack::pack( PackBuffer, SendData );

            NET_MSG_PACK SendData;
            if (SendData.SetData(NET_MSG_CONSIGNMENT_ITEM_REGIST_REQ, PackBuffer))
                pCacheServer->InsertMsg(DBACTION_CLIENT_ID, &SendData);
        }
    }
    return sc::db::DB_OK;
}


int ConsignmentSaleSlotExpansionDateUpdate::Execute( NetServer* pServer )
{
    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return sc::db::DB_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer && !m_pSubDbManager )
        return sc::db::DB_ERROR;

    if( m_pSubDbManager == FALSE )
        return sc::db::DB_ERROR;

    if( m_pSubDbManager->ConfignmentSaleSlotExpansionDateSet( m_dwChaDbNum, m_tExpansionDate ) == sc::db::DB_ERROR )
        return sc::db::DB_ERROR;

    return sc::db::DB_OK;
}


int ConsignmentSaleLogInsert::Execute( NetServer* pServer )
{
	CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
	if (!pFieldServer)
		return sc::db::DB_ERROR;

	// LOG 기록
	CTime timeTemp = m_SaleItem.ExpirationDate - CTime::GetCurrentTime().GetTime();
	
	if( m_pSubDbManager->ConsignmentSaleLogInsert(
			m_guid,
			m_SaleItem.dwChaDbNum,
			m_SaleItem.SellType,
			m_logType,
			m_SaleItem.llPrice,
			m_llFee,
			m_SaleItem.sItemcustom.Mid(),
			m_SaleItem.sItemcustom.Sid(),
			0, //m_SaleItem.sItemcustom.lnGenNum,
			m_SaleItem.wTurnNum/*m_SaleItem.wTotalTurnNum*/,
			pFieldServer->GetServerGroup()/*m_SaleItem.sItemcustom.wRanPointTradeCount*/,
			timeTemp.GetTime() / 3600 ) == sc::db::DB_ERROR )
	{
		return sc::db::DB_ERROR;
	}


    return sc::db::DB_OK;
}

} // namespace db
