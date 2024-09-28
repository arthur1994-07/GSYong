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
	//const char* MARKET_RANPOINT_SERVER_TEXT_0 = "���� %d�ð� �̳�";
}

bool GLConsignmentSaleField::ReqGetList( NET_MSG_GENERIC* nmg, GLChar* pChar )
{
    if( pChar == NULL )
        return false;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)nmg;
    GLMSG::NET_CONSIGNMENT_SALE_ITEM_LIST_REQ RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return false;

    //�ݺ� ��û�� ���´�.
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

	// ������ ������ 0 �ΰ��� ����� �� ����. �ش� �������� ��ϵȴٸ� ��ȯ��� ������ �߻��� ( ���Ͽ� ÷�ξ������� ������ )
	if ( RecvData.m_wTurnNum == 0 )
		return false;

    // �����ϴ� ���������� Ȯ��
    const SITEM* pITEM = GLogicData::GetInstance().GetItem( RecvData.m_NativeID );
    if ( !pITEM )
        return false;

	if ( RecvData.m_llPrice <= 0 ||
		RecvData.m_wTurnNum <= 0 ||
		W_LIMITED_POINT_TRADE_COUNT < RecvData.m_wTurnNum )
		return false;

    // �ּ� ���Ϸ� �ݾ��� ���� �� ������ ����Ʈ �̻� �ݾ����� �ø� �� ����;
	if ( RecvData.m_llPrice < private_market::GLPrivateMarketDefine::Instance()->RanPointSaleMinPrice() ||
		LL_LIMITED_A_UNIT_PRICE < RecvData.m_llPrice )
		return false;

    SITEMCUSTOM sItemCustom( RecvData.m_NativeID );

    // ���ӸӴ� �ŷ��ΰ�?
    std::vector< private_market::GameMoneyItemFee > vecMoneyPeriodFee 
        = private_market::GLPrivateMarketDefine::Instance()->GetMoneySellPeriod();

    bool bIsGameMoney = false;

	// GetMoneySellPeriod, �˻��ϴ� �� ��������.
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
	
	// ���� ��ȹ������ �����ᰡ ���°�� �ŷ� �Ұ������� GS�� �����Ҷ� ������ ����� ���� �������� �߰��Ǿ� �־ �����Ḧ 0�� �����ϵ��� ����� //if( dwFee <= 0 )
    if ( dwFee < 0 )
        return false;

    // Ȯ�� ���� �ð� ���
    WORD wCurrentSlotNum = private_market::GLPrivateMarketDefine::Instance()->SellResgisterBasicNum();

    const __time64_t& tExpansionDate = pChar->GetConsignmentSaleSlotExpansionDate();    
    const __time64_t& tCurTime = CTime::GetCurrentTime().GetTime();

    if ( 0 < tExpansionDate - tCurTime )
        wCurrentSlotNum += private_market::GLPrivateMarketDefine::Instance()->SellResgisterExpandAddNum();

    if ( wCurrentSlotNum <= m_mapUserSlotCount[RecvData.m_dwChaDbNum] )
    {
        // �� �̻� ��� �� �� �����ϴ�.
        return false;
    }

    // �����Ұ�� ���� �޽���
    GLMSG::NET_CONSIGNMENT_ADD_ITEM_ACK SendData;
    SendData.bAddSucess = false;
    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );

    // ������ �����Ḧ Ȯ���Ѵ�.
    if ( pChar->PointShopPoint() <  dwFee )
        return false;

	// ���ŷ� ����� ���������� ����Ҽ� ����.
	if ( private_market::GLPrivateMarketDefine::Instance()->GoldTrade() == false && RecvData.m_bySellType == private_market::eSellType_ConsignmentSale_Gold  ) 
		return false;

    // ������ �Ǹ� Ÿ���� Ȯ��
    if ( RecvData.m_bySellType <= private_market::eSellType_PrivateMarket
        && private_market::eSellType_ConsignmentSale_END <= RecvData.m_bySellType )
    {
        m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
        return false;
    }

	// ��Ź�Ǹ� ����Ű ����
	// ���ӸӴ� �Ǹ��� ��� ������ guid �� �� ������ ���õǸ�, �� ���� ��Ź�Ǹ� ����Ű�� �ȴ�.
	sc::MGUID guid;
	sc::SeqUniqueGUID::Instance()->getGUID( guid );

    if ( bIsGameMoney )
    {
        LONGLONG llGameMoney = pITEM->sBasicOp.dwSellPrice * RecvData.m_wTurnNum;

        // ���� ��� ����. ��ǰ ��� �Ұ�.
        if ( pChar->GetInvenMoney() < llGameMoney )
            return false;
        
        pChar->SubtractInvenMoneyAndUpdate( llGameMoney, SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_REGISTRATION );

		// Ŭ���̾�Ʈ������ 1���� �����ؼ� ��������, ���� ������ ������ 2�̻� ����� �ص� ���������� �����ϵ��� ���� �����Ѵ�.
		sItemCustom.wTurnNum = RecvData.m_wTurnNum;
		sItemCustom.SetGuid( guid );

		// ��� �� �α� ����
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
        // �κ� �������� ���� �´�.
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

        // �Ǹ��Ϸ��� ������������ ���� ������ ID�� ���Ѵ�.
        if ( pInvenItem->GetNativeID() != RecvData.m_NativeID )
        {
            m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
            return false;
        }

        // �ŷ��Ұ����ϸ�
		if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
		{
			// �ڽ�Ƭ�� �پ� �ִٸ� �ŷ� �Ұ�
			return false;
		}
		else if ( sItemCustom.IsTimeLimit() == true )
		{
			// ��ȿ�� ������ �ŷ� �Ұ�
			return false;
		}
		else if ( sItemCustom.IsGM_GENITEM() )
		{
			// #3989 GmCharEdit ���� �ŷ��Ұ��� �־��� �������� Ŭ��â�� ���� �� �ִ� ���� ����, ��������Ʈ üũ���� �ʴ�
			// �����۱�ȯ, ����Ʈ�ŷ����� �˻��ϵ��� �߰�
			return false;
		}
		else if ( ( sItemCustom.IsExchange() != NULL ) == false )
        {
            // �� ����Ʈ �ŷ��� �ŷ� ����
            if ( RecvData.m_bySellType != private_market::eSellType_ConsignmentSale_RanPoint )
                return false;

            // �ŷ� ����Ƚ���� �ʰ��ϴ��� �˻�
            if ( pITEM->sBasicOp.RanPointTradeCount() <= pInvenItem->sItemCustom.wRanPointTradeCount )
			{
                m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
                return false;
            }
        }

        // �������� ��ħ ������ �ʰ��� �� ����.
        if( pInvenItem->TurnNum() < RecvData.m_wTurnNum )
        {
            m_pGaeaServer->SENDTOCLIENT( pChar->GETCLIENTID(), NET_MSG_CONSIGNMENT_ADD_ITEM_ACK, PackBuffer );
            return false;
        }

        // ��� �������� ����Ѵٸ� ������Ű��
        // �ƴѰ�쿡�� ���� ���ҽ�Ų��.
        if ( pInvenItem->TurnNum() == RecvData.m_wTurnNum )
        {
			// #item
			// �켱 �����ϵ��� �����ΰ�, ����, ���λ���, ��Ź�Ǹ� ��������.
			// ������ ������ �̵� �����ϰ�, ������ guid �� �״�� �����ϱ� ���ؼ� ��񿡼� �����ϴ� ����� �Լ� ȣ��
			if ( pChar->InvenCompletelyDeleteItem( pInvenItem->wPosX, pInvenItem->wPosY, true, FALSE, 90345 ) == false )
                return false;
        }
        else
        {
            pInvenItem->sItemCustom.wTurnNum -= RecvData.m_wTurnNum;

            //	[�ڽſ���] Inventory ������ �Ҹ��.
            GLMSG::SNETPC_INVEN_DRUG_UPDATE NetMsg_Inven_Update;
            NetMsg_Inven_Update.dwChaNum = pChar->CharDbNum();
            NetMsg_Inven_Update.wPosX = pInvenItem->wPosX;
            NetMsg_Inven_Update.wPosY = pInvenItem->wPosY;
            NetMsg_Inven_Update.wTurnNum = pInvenItem->sItemCustom.wTurnNum;
            m_pGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &NetMsg_Inven_Update );

        }
    }

    // �����Ḧ ����.
    pChar->PointShopPointSub( dwFee, pointshop::CONSIGNMENT_SALE, pointshop::POINT_SUB_TYPE_CONSIGNMENT_ADD );

	// ��� �� �α� ����
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

	// ��� �� �α� ����
    pChar->LogItemExchange( 
        sItemCustom, 
        gamelog::ID_CHAR, pChar->CharDbNum(),
        gamelog::ID_CHAR, 0,
        ITEM_ROUTE_CONSIGMENT_SALE, 
        RecvData.m_wTurnNum,
		pChar->GetCurrentMap(), LOG_ITEM_INFO_REGIST_ITEM_BY_CONSIGEMENT,
        true );

    PackBuffer.clear();

    // NET_CONSIGNMENT_ADD_ITEM_ACK SendData ���� ä���
	WORD wPeriod=0;
    private_market::ConsignmentSaleItem& saleItem = SendData.SaleItem;
    {
        // �ŷ� �Ⱓ�� ���� �⺻ �����Ḧ �ΰ��Ѵ�.
        std::vector< private_market::PeriodFee > vecPeriodFee = private_market::GLPrivateMarketDefine::Instance()->GetSellPeriod();
        if ( 0 <= RecvData.m_wPeriodIndex && RecvData.m_wPeriodIndex < static_cast< int >( vecPeriodFee.size() ) )
             wPeriod = vecPeriodFee[RecvData.m_wPeriodIndex].wPeriod;

        saleItem.dwChaDbNum = pChar->CharDbNum();
        saleItem.strChaName = pChar->GetName();

		/*
		����Ʈ�ŷ��� ������ �������� ��ٷ� �ٽ� ����Ʈ�ŷ� ����ϸ� �������� �����Ǵ� ���� ó��
		����Ʈ�ŷ� ��� ���̺� ���� Ű������ ������ �����ʱ� ������ �߻��ϸ�(���� �������� �������� ���� ������ ��� ó�� ��û�ϴ� ����)
		������ ���� ������������ �߻����� �ʾҴ�. �ֳĸ� ������ ���� ���������� ����Ʈ�ŷ� ����� ������ ���ο� Ű���� �߱��ؼ� ����Ͽ��� ����
		������ ������ ����Ǹ鼭 ����Ʈ�ŷ� Ű���� ��� �������� Ű���� ����ϵ��� ����Ǿ���.
		�׷��� �Ǹ��ڰ� ���λ����˻�â�� ���� �Ǹŵ� ������ ����� ó���ؾ� ���� ����Ʈ�ŷ� ��� ���̺��� �� �������� ���� ����ó���Ǵµ�
		�������� �����ڰ� �� �������� �ٷ� ����Ʈ�ŷ� ����� �Ϸ��� �ϴ�, ��� ���̺� ������ �������� ������ �����ʴ� ���̴�.
		�׷��� ó���� ����Ʈ �ŷ� ����� �� Ű���� �űԷ� �����ؼ� ����ϵ��� �Ѵ�.
		Ű�� �״�� ����ϴ� ������� ó���Ϸ��� ����Ʈ�ŷ� ����� �� �������� �ִ��� Ȯ���ϴ� ������ �ʿ��ϰ�, �׷� ������ �ִٰ��ص�
		���� �̹� �������� �����ߴµ�, ���� �Ǹ��ڰ� ��ó���� ����� ���� �� �� �ִٴ� ���� ����� �ȴ�.
		�׷��� �ű� Ű������ ����Ʈ �ŷ� ����ϴ� ������� �����Ѵ�.

		20160826 khkim
		ConsignmentSale ���̺� Į���� �߰��ϰ�(���� ������ ��ȯ ���� Į�� �߰��� ��ó��) �⺻���� 0 ��ȯ�� �ʿ��� ���� 1�̷��� �ؼ�
		1�� �͵��� �������� ���� �������̱� ������ �� ���̺��� ItemGuid �� ������ Ű������ �־��ְ�, ������ ���µ� insert �� �ؾ� �ҵ�
		����Ʈ�ŷ� ����Ҷ��� ������ Ű������ ������� ���� - ���� �� �ٷ� ��� �ȵǴ� ������ - �ű� Ű������ ���
		������ �������� �� �������� Ű���� ����Ʈ�ŷ� Ű������ �����ۿ��ٰ� �־���µ�, �̷��� �ϴ� ������Ʈ �������鼭 �������� �����
		�������� �Ǹ��ڰ� �ø� �� �������̴ϱ� ������ ���´� ������Ʈ���װ�, �ű⿡ Ű���� �����ؼ� �־��ִ� �翬�� �������� ������ ������Ʈ�� �ȵ�
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

    // NET_CONSIGNMENT_ADD_ITEM_ACK SendData �޽��� ��
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

    // ������Ŷ�ΰ�?
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


	// ����(eNotExistSaleItem) �ߴٸ� �������� ��� �ȸ����̰�
	// ����(eSuccess) �ߴٸ� �������� ĳ������ ����ó�� �Ȱ��̴�.
	
	// �� �̿ܿ��� ������.
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
        // ������ �ȵȴ�. ���� �����ӿ� �˻� �߱⶧���� �̷���� �������� ĳ���͸� ����������Ѵ�.
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
    // ACK �޾Ƽ� �������� ó�� �ؾ��Ѵ�.
    // ���� �޴´�.    
    WORD wSaleTurnCount = SaleItem.wTotalTurnNum - SaleItem.sItemcustom.wTurnNum;

	LONGLONG llFeeTemp = 0;	// �α� ��Ͽ� ����.
    if( 0 < wSaleTurnCount )
    {
        __int64 llNowPoint = pChar->PointShopPoint();

        // �Ǹ� �ݾ� = �Ǹ��� ���� * �ǸŰ���
        __int64 nTotalPrice = wSaleTurnCount * SaleItem.llPrice;
        
        // �Ǹ� �ݾ� - ������
        double dFee = private_market::GLPrivateMarketDefine::Instance()->SalesCompletionFee();
        

        // ����� ���ؼ� 10000�� ���Ѵ�.
        static const unsigned int n10000 = 10000;

        __int64 nFee = static_cast<int>( dFee*n10000 ); // 1000
        __int64 llAddPoint = static_cast<__int64>( nTotalPrice * ( n10000 - nFee ) / n10000 );

		llFeeTemp = nTotalPrice-llAddPoint;

        switch( SaleItem.SellType ) 
        {
        case private_market::eSellType_ConsignmentSale_RanPoint:
            {
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 3 ) �Ǹ� ����
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 4 ) ������
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 1 ) ���� �� ����Ʈ
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 3 ) ���� �� ����Ʈ
                std::string strTemp;
                //strTemp += "��Ź �Ǹ� ����\r\n\r\n"; //sc::string::format( "����Ʈ�� �ŷ��Ϸ�\r\n\r\n" );
                strTemp += sc::string::format( "%s : %s\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 3 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( nTotalPrice, 3, "," ) );
                strTemp += sc::string::format( "%s : %s\r\n\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 4 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llFeeTemp, 3, "," ) );
                strTemp += sc::string::format( "%s : %s\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 1 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llNowPoint, 3, "," ) );
                strTemp += sc::string::format( "%s : %s\r\n", ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 2 ), NS_UITEXTCONTROL::MAKE_MONEY_FORMAT( llNowPoint+llAddPoint, 3, "," ) );
                
                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 0 ) ��Ź�Ǹ� �ý���
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

                // ID2SERVERTEXT( "PRIVATE_MARKET_POINTSHOP_TRADE", 0 ) ��Ź�Ǹ� �ý���
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

                PostInfo.byPostType       = EMPOST_MONEY;   // ���� Ÿ��
                PostInfo.llAttachMoney    = llAddPoint;    
            }
            break;
        }

        
    }

    // �������� �� �ȸ��� �ʰ� ���� �ִٸ� �����۰� �Բ� �����Ѵ�.
    if( 0 < SaleItem.sItemcustom.wTurnNum )
    {
        // ���ӸӴ� �ŷ��ΰ�?
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
            // �����ϴ� ���������� Ȯ��
            const SITEM* pITEM = GLogicData::GetInstance().GetItem( SaleItem.sItemcustom.GetNativeID() );
            if ( !pITEM )
                return false;

            LONGLONG llGameMoney = pITEM->sBasicOp.dwSellPrice * SaleItem.sItemcustom.wTurnNum;

            PostInfo.bAttach01        = false;
            PostInfo.byPostType       = EMPOST_MONEY;   // ���� Ÿ��
            PostInfo.llAttachMoney    = llGameMoney;      


			// ��� �� �α� ����
			//pChar->LogMoneyExchange( gamelog::ID_CHAR, 0, gamelog::ID_CHAR, pChar->m_CharDbNum, llGameMoney, ITEM_ROUTE_CONSIGMENT_SENDBACK, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap() );
			//pChar->LogMoneyExchange( gamelog::ID_CHAR, 0, gamelog::ID_CHAR, pChar->m_CharDbNum, pChar->GetInvenMoney(), ITEM_ROUTE_CONSIGMENT_SENDBACK, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap() );
        }
        else
        {
            // ���� �ŷ� �ߴٸ� ���� ��´�.
            if (SaleItem.SellType==private_market::eSellType_ConsignmentSale_Gold)
                PostInfo.byPostType       = EMPOST_MONEYITEM;   // ���� Ÿ��

            // �������� ��´�.
            PostInfo.bAttach01 = true;
        }


		PostInfo.dwUserNum = 0;

        // ����(����) �����ϰ� �������� �������� ������.
        m_pGaeaServer->AddGameAdoJob(
            db::DbActionPtr(
            new CPostSendSystemMail( PostInfo, SaleItem.sItemcustom )));
    }
    else
    {
		PostInfo.dwUserNum = 0;

        // ����(����) �����ϰ� �������� �������� ������.
        m_pGaeaServer->AddGameAdoJob(
            db::DbActionPtr(
            new CPostSendSystemMail( PostInfo, SITEMCUSTOM() )));
    }

	if( pChar )
	{
		// ��� �� �α� ����
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

// ����Ʈ�ŷ� ������ ����
// �����ڰ� �ִ� �ʵ弭���� �ͼ� ���� ó���Ѵ�.
// �ϴ�, ���ӸӴ� Ȥ�� ����Ʈ�� �����ϰ�, �ʿ��� �����͸� ���׼����� �����ؼ� ó���Ѵ�.
bool GLConsignmentSaleField::ReqItemPurchase( NET_MSG_GENERIC* nmg )
{
    NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
    GLMSG::NET_CONSIGNMENT_SALE_PURCHAS_REQ RecvData;
    if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
        return false;

    // ������ �˻�
    GLChar* pChar = m_pGaeaServer->GetCharByDbNum( RecvData.dwBuyerDbNum );
    if ( pChar == NULL )
    {
        // ĳ���Ͱ� ����.
		// ����ĳ�ü����� ��� �����ؼ� Ȧ�� ��ҽ�Ų��.
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
			// ĳ�� �������� �ߴܵ��� �˸�
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			SendData.strGUID = RecvData.strGUID;
			
			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

            return false;
        }

        // ����Ʈ ����
		pChar->PointShopPointSub( RecvData.llItemPriceTotal, pointshop::CONSIGNMENT_SALE,  pointshop::POINT_SUB_TYPE_CINSIGNMENT_BUY );

		// �α״� ConsignmentSalePurchase �ȿ��� ó���ȴ�. �Ǹ���/������ �����׷� Ȯ���� ConsignmentSalePurchase �ȿ��� �����ϱ� ����

        break;
    case private_market::eSellType_ConsignmentSale_Gold:
        if ( pChar->GetInvenMoney() < RecvData.llItemPriceTotal )
        {
            // �� ����
			// ĳ�� �������� �ߴܵ��� �˸�
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			SendData.strGUID = RecvData.strGUID;

			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

            return false;
        }

        pChar->SubtractInvenMoneyAndUpdate( RecvData.llItemPriceTotal, SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY );
        
		// �α״� ConsignmentSalePurchase �ȿ��� ó���ȴ�. �Ǹ���/������ �����׷� Ȯ���� ConsignmentSalePurchase �ȿ��� �����ϱ� ����
        
		break;
    }

    // DB ó���Ŀ� �α׸� ����� ����� ĳ���� �����Ѵ�.
    // Action�� ��� ���ԵǾ��ִ�.
    sc::MGUID Guid;
    char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
    StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, RecvData.strGUID.c_str() );
    sc::string::stringToUUID( szGuid, Guid );

    // ���� �߼ۿ� �ʿ��� �ּ� ����
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
	// �����ڰ� �ִ� �ʵ弭���̴�.
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
	GLMSG::NET_CONSIGNMENT_SALE_PAY_MONEY RecvData;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
		return false;

	// ������ �˻�
	GLChar* pChar = m_pGaeaServer->GetCharByDbNum( RecvData.dwBuyerDbNum );
	if ( pChar == NULL )
	{
		// ������ ���
		// Ȧ�� ���
		// ĳ�� �������� �ߴܵ��� �˸�
		GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
		SendData.strGUID = RecvData.strGUID;

		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );
		m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );
		return false;
	}

	// ĳ������ ������Ŷ�̴�
	switch( RecvData.nSellType )
	{
	case private_market::eSellType_ConsignmentSale_RanPoint:
		if ( pChar->PointShopPoint() < RecvData.llItemPriceTotal )
		{
			// ĳ�� �������� �ߴܵ��� �˸�
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			SendData.strGUID = RecvData.strGUID;

			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

			return FALSE;
		}

		//����Ʈ ����
		pChar->PointShopPointSub( RecvData.llItemPriceTotal, pointshop::CONSIGNMENT_SALE,  pointshop::POINT_SUB_TYPE_CINSIGNMENT_BUY );

		break;
	case private_market::eSellType_ConsignmentSale_Gold:
		if ( pChar->GetInvenMoney() < RecvData.llItemPriceTotal )
		{
			// �� ����
			// ĳ�� �������� �ߴܵ��� �˸�
			GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_ACK SendData;
			SendData.strGUID = RecvData.strGUID;

			msgpack::sbuffer PackBuffer;
			msgpack::pack( PackBuffer, SendData );
			m_pGaeaServer->SENDTOINTEGRATIONCACHE( NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK, PackBuffer );

			return FALSE;
		}

		pChar->SubtractInvenMoneyAndUpdate( RecvData.llItemPriceTotal, SUBTRACT_INVEN_MONEY_CONSIGMENT_SALE_ITEM_BUY );

		//	Note : �� ���.
		// LogMoneyExchange �ؾ���

		break;
	}

	// ĳ�ÿ� ��� ����
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
	// �����ڰ� �ִ� �ʵ弭���̴�.
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
	GLMSG::NET_CONSIGNMENT_SALE_SEND_POST_REQ RecvData;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
		return false;

	// DB ó���Ŀ� �α׸� ����� ����� ĳ���� �����Ѵ�.
	// Action�� ��� ���ԵǾ��ִ�.
	sc::MGUID Guid;
	char szGuid[sc::string::UUID_LENGTH_BRACES] = {0};
	StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, RecvData.strGUID.c_str() );
	sc::string::stringToUUID( szGuid, Guid );

	// ���� �߼ۿ� �ʿ��� �ּ� ����
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
	// ������ ���� ó�� ���� ������ �߻��� ���
	// ĳ���Ͱ� ������ ���Ŵ�� �����ְ�, ĳ�ÿ��� ���� ��� ó��
	// ĳ���Ͱ� ���ٸ� �������� �����ְ�, ĳ�ÿ��� ���� ��� ó��
	NET_MSG_PACK* pPacket = ( NET_MSG_PACK* )nmg;
	GLMSG::NET_CONSIGNMENT_SALE_PURCHASE_FB RecvData;
	if ( !msgpack::BufferToObject( pPacket->DataBuffer(), pPacket->DataSize(), RecvData ) )
		return false;

	// ������ �˻�
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
		// ���� ���� �ڵ� �߰�
		// �켱 �α׸� �����.
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

	// �����ڴ� ������ �־�� �Ѵ�.
    if ( pCharRecver )
    {
        strRecver = pCharRecver->GetName();
        dwRecvUserNum = 0;	// ���� ������ �־���. �������� ������ �ȵȴ�.
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
    // ���� �߼ۿ� �ʿ��� �ּ� ����
    ZeroMemory( &PostInfo, sizeof( SPOSTINFO ) );
    {		
        PostInfo.iDesign        = ATTENDANCE_REWARD_POST_DESIGN;    // ������ ������

        PostInfo.byPostClass    = EMPOST_SYSTEM;
        PostInfo.byPostState    = EMPOST_UNREAD;                    // ���� ����
        PostInfo.byPostType 	= EMPOST_ITEM;                      // ���� Ÿ��

        StringCchCopy( PostInfo.Title, sizeof( PostInfo.Title ), PostTitle.c_str() );
        StringCchCopy( PostInfo.Content, sizeof( PostInfo.Content ), PostContent.c_str() );

        PostInfo.llAttachMoney  = 0;                                // ÷�� ���
        PostInfo.llCharge       = 0;                                // û�� ���

        PostInfo.iSender        = SendChaNum;                       // ���� ���
        PostInfo.iRecver        = RecvChaNUm;                       // �޴� ���

        PostInfo.dwUserNum      = SendUserNum;
        PostInfo.nUserNumRecver = RecvUserNum;

        StringCchCopy( PostInfo.SenderName, sizeof( PostInfo.SenderName ), SendChaName.c_str() );
        StringCchCopy( PostInfo.RecverName, sizeof( PostInfo.RecverName ), RecvChaName.c_str() );
    }
}
